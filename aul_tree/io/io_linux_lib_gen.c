
/********************************************************************************
_____________________________________________________________________________  
Description:

_____________________________________________________________________________
Purpose:

_____________________________________________________________________________
Remark:


_____________________________________________________________________________
Example:

_____________________________________________________________________________
Copyright Notice:
    
	COPYRIGHT NOTICE Copyright (c) 2014 by Apex Embedded Systems. All rights 
	reserved. This document is the confidential property of Apex Embedded 
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:

1. interrupts.
DONE: 2. verify compilation.
3. IO_Xfr() requires a test to verify virtual_address is within range.
4. IO_Interrupt_Enable() must include io_xfr information and then
   deposit it here locally for later running during interrupt.
5. better error handling.  Have a function that reads the last state
   of io_llg_error and then resets it.
6. Posting last error?
7. 8-bit restriction review for all IO_Read_?() and IO_Write_?() functions.

*********************************************************************************/
/* linux specific headers */
/*#include <sys/stat.h>*/
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#include "io_private.h"
#include "io_shared.h"
#include "../shared/mem.h"			 /* memory allocation			*/
#include "../shared/status.h"     /* error codes and APIs		*/
#include "../shared/error.h"      /* error handling			*/
#include "../shared/device.h"		 /* device qty	 and device undefined  */
#include "../shared/target.h"		 /* TARGET_BUS_APERTURE_WIDTH_BYTES */
#include "../shared/cmd.h"			 /* Ioctl() commands */
#include "../str_safe/str_safe.h"

#include "../shared/cmd_macros.h"
/*********************************************************************************/
#define IO_MODULE_REV  "$Date: 2014-05-28 16:58:04 -0500 (Wed, 28 May 2014) $"

/*******************************************************************************/
typedef struct io_llg_intr_diagnostics
{
	int device;
	uint32_t counter; /* Number of interrupts received */
	uint32_t lost;    /* Number of interrupts not yet handled */
	int		 wait_result; /* See WD_INTERRUPT_WAIT_RESULT values in windrvr.h */
	uint32_t enabled_int_type; /* Interrupt type that was actually enabled
                                 (MSI/MSI-X/Level Sensitive/Edge-Triggered) */
	uint32_t last_message; /* Message data of the last received MSI/MSI-X
                             (Windows Vista/7); N/A to line-based interrupts) */
};
/*********************************************************************************/
struct io_llg_interrupt_context
{
	size_t							type;
	int								device;
	struct io_llg_intr_diagnostics	diagnostics;
	io_int_handler					interrupt_handler;
	void *							private_data;
};
typedef struct io_llg_interrupt_context io_llg_interrupt_context_type;
/*********************************************************************************/
/* internal data structure specific to this module -- for internal use only */
struct io_llg_device_private 
{
	int								fd;
	struct io_llg_interrupt_context	interrupt_context;
	io_definition_type *			io_definition;
};
/*********************************************************************************/
enum { IO_LLG_INVALID_FILE = -1 };
/*********************************************************************************/
static struct io_llg_device_private io_llg_private[DEVICE_QTY];  /* private dataset */ 
static int io_llg_fd = IO_LLG_INVALID_FILE;

/*********************************************************************************/
IO_ID_ENUM IO_ID_Get( void ) {	return IO_ID_LINUX_LIB_GENERIC;  }
/*********************************************************************************/
int IO_Read_U8( int iod, size_t offset, uint8_t * value )
{
	int			error_code;
	io_xfr_type xfr = { 0 };
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	xfr.command = IO_XFR_READ_U8;
	xfr.offset = offset;
	error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
	if ( error_code ) goto IO_Read_U8_Error;
	*value = xfr.data.u8;  /* */
	error_code = SUCCESS;
IO_Read_U8_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Read_U16( int iod, size_t offset, uint16_t * value )
{
	int			error_code;
	io_xfr_type xfr = { 0 };
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		uint8_t u8_lo;
		uint8_t u8_hi;
		xfr.command = IO_XFR_READ_U8;
		xfr.offset  = offset + 0;
		error_code  = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
		if ( error_code ) goto IO_Read_U16_Error;
		u8_lo = xfr.data.u8;  /* */
		xfr.command = IO_XFR_READ_U8;
		xfr.offset  = offset + 1;
		error_code  = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
		if ( error_code ) goto IO_Read_U16_Error;
		u8_hi = xfr.data.u8;  /* */
		*value = IO_U16_SET_U8_LO_HI(u8_lo,u8_hi);
	}
	else
	{
		xfr.command = IO_XFR_READ_U16;
		xfr.offset  = offset;
		error_code  = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
		if ( error_code ) goto IO_Read_U16_Error;
		*value = xfr.data.u16;
	}
	error_code = SUCCESS;
IO_Read_U16_Error:
	IO_Error_Set( error_code );
	return error_code;	
}
/*********************************************************************************/
int IO_Read_Repeat_U8( int iod, size_t offset, uint8_t * buffer, size_t count )
{
	int			error_code;
	io_xfr_type xfr = { 0 };
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	xfr.command  = IO_XFR_READ_U8_REPEAT;
	xfr.offset   = offset;
	xfr.data.buf = (char *) buffer;
	xfr.buf_len  = count;
	error_code   = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Read_Repeat_U16( int iod, size_t offset, uint16_t * buffer, size_t count )
{
	int			error_code;
	io_xfr_type xfr = { 0 };
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		size_t index;
		uint8_t u8_lo, u8_hi;
		for ( index = 0; index < count; index++ )
		{
			xfr.command = IO_XFR_READ_U8;
			xfr.offset  = offset + 0;
			error_code  = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
			if ( error_code ) goto IO_Read_Repeat_U16_Error;
			u8_lo = xfr.data.u8;  /* */
			xfr.command = IO_XFR_READ_U8;
			xfr.offset = offset + 1;
			error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
			if ( error_code ) goto IO_Read_Repeat_U16_Error;
			u8_hi = xfr.data.u8;  /* */
			buffer[index] = IO_U16_SET_U8_LO_HI(u8_lo,u8_hi);
		}
	}
	else
	{
		xfr.command  = IO_XFR_READ_U16_REPEAT;
		xfr.offset   = offset;
		xfr.data.buf = (char *) buffer;
		xfr.buf_len  = count;
		error_code   = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
		if ( error_code ) goto IO_Read_Repeat_U16_Error;
	}
	error_code = SUCCESS;
IO_Read_Repeat_U16_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Write_U8( int iod, size_t offset, uint8_t  value )
{
	int			error_code;
	io_xfr_type xfr = { 0 };
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	xfr.command = IO_XFR_WRITE_U8;
	xfr.offset  = offset;
	xfr.data.u8 = value;  /* */
	error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Write_U16( int iod, size_t offset, uint16_t  value )
{
	int			error_code;
	io_xfr_type xfr = { 0 };
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		uint8_t u8_lo = IO_U16_GET_U8_LO( value );
		uint8_t u8_hi = IO_U16_GET_U8_HI( value );
		xfr.command = IO_XFR_WRITE_U8;
		xfr.offset  = offset + 0;
		xfr.data.u8 = u8_lo;  /* */
		error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
		if ( error_code ) goto IO_Write_U16_Error;
		xfr.command = IO_XFR_WRITE_U8;
		xfr.offset  = offset + 1;
		xfr.data.u8 = u8_hi;  /* */
		error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
		if ( error_code ) goto IO_Write_U16_Error;
	}
	else
	{
		xfr.command  = IO_XFR_WRITE_U16;
		xfr.offset   = offset;
		xfr.data.u16 = value;
		error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
		if ( error_code ) goto IO_Write_U16_Error;
	}
	error_code = SUCCESS;
IO_Write_U16_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Write_Repeat_U8( int iod, size_t offset, uint8_t * buffer, size_t count )
{
	int			error_code;
	io_xfr_type xfr = { 0 };
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	xfr.command = IO_XFR_WRITE_U8_REPEAT;
	xfr.offset = offset;
	xfr.data.buf = (char *) buffer;
	xfr.buf_len  = count;
	error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
	if ( error_code ) goto IO_Write_Repeat_U8_Error;
	error_code = SUCCESS;
IO_Write_Repeat_U8_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Write_Repeat_U16( int iod, size_t offset, uint16_t * buffer, size_t count )
{
	int			error_code;
	io_xfr_type xfr = { 0 };
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		size_t index;
		for ( index = 0; index < count; index++ )
		{
			xfr.command = IO_XFR_WRITE_U8;
			xfr.offset = offset + 0;
			xfr.data.u8 = IO_U16_GET_U8_LO( buffer[index] );  /* */
			error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
			if ( error_code ) goto IO_Write_Repeat_U16_Error;
			xfr.command = IO_XFR_WRITE_U8;
			xfr.offset = offset + 0;
			xfr.data.u8 = IO_U16_GET_U8_HI( buffer[index] );  /* */
			error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
			if ( error_code ) goto IO_Write_Repeat_U16_Error;
		}
	}
	else
	{
		xfr.command = IO_XFR_WRITE_U16_REPEAT;
		xfr.offset = offset;
		xfr.data.buf = (char *) buffer;
		xfr.buf_len  = count;
		error_code = ioctl( io_llg_private[iod].fd, CMD_IO_XFR, &xfr );
		if ( error_code ) goto IO_Write_Repeat_U16_Error;
	}
	error_code = SUCCESS;
IO_Write_Repeat_U16_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
static int IO_LLG_Interrupt_Disable( struct io_device_dataset * ds )
{
	(void) ds;
	return -EC_IO_NOT_IMPLEMENTED;
}
/*********************************************************************************/
static int IO_LLG_Interrupt_Enable( struct io_device_dataset * ds, io_int_handler handler, void * private_data )
{
	(void) ds;
	(void) handler;
	(void) private_data;
	return -EC_IO_NOT_IMPLEMENTED;
}
/*********************************************************************************/
static int IO_LLG_Ioctl( struct io_device_dataset * ds, unsigned int command, unsigned long argument )
{
	int		error_code;
	(void)   ds;
	(void)   command;
	(void)   argument;
	IOC_T	 module_logical_id;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif

	if ( IO_LLG_INVALID_FILE == io_llg_fd ) /* must be opened */
	{ error_code = EC_IO_CORE_UNINITIALIZED;	goto IO_LLG_Ioctl_Error;	}

	module_logical_id = IOC_GET_LOGICAL_ID_MODULE( command );
	if ( AULDRV_LOGICAL_ID == module_logical_id )
	{ /* pass onto controller */
		error_code = ioctl( io_llg_fd, command, argument );
	}
	else
	{ /* otherwise pass onto device */
		if ( DEVICE_UNDEFINED == ds->iod )
		{ error_code = EC_IO_NOT_CAPTURED;	goto IO_LLG_Ioctl_Error;	}
		error_code = ioctl( io_llg_private[ds->iod].fd, command, argument );
	}
	if ( error_code ) goto IO_LLG_Ioctl_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_LLG_Ioctl_Error:
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return error_code;
}
/*********************************************************************************/
int IO_LLG_Open( struct io_device_dataset * ds )
{
	int		error_code;
	/* size_t	index; */
	/* size_t	minor_number; */
	int		ld;  /* short for local device */
	io_llg_interrupt_context_type * interrupt_context;
	static char name_path_device[IO_DEFINITION_ACRONYM_SIZE];

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif

	ld	= ds->iod;
	interrupt_context = &(io_llg_private[ld].interrupt_context);
	Mem_Set( interrupt_context, 0, sizeof( io_llg_interrupt_context_type ) );
	interrupt_context->device = ld;

	/* Set the device's resources information */

	/* open driver */
/*
#if(0)
	interrupt_context = &(io_llg_private[*device].interrupt_context);
	Mem_Set( interrupt_context, 0, sizeof( struct io_interrupt_context ) );
	interrupt_context->device = *device;
#endif
*/
	String_Copy( name_path_device, "/dev/", IO_DEFINITION_ACRONYM_SIZE );
	String_Concatenate( name_path_device, ds->io_definition.acronym, IO_DEFINITION_ACRONYM_SIZE );
/* TODO: add "/dev/" prefix */
	io_llg_private[ld].fd = open( name_path_device, O_RDWR );
	if ( io_llg_private[ld].fd < 0 )
	/*TODO: { error_code = EC_BUILD_OS( errno );		goto IO_LLG_Open_Error; }*/
	{ error_code = errno ;		goto IO_LLG_Open_Error; }
	io_llg_private[ld].io_definition = &( ds->io_definition );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	ds->private_data = &(io_llg_private[ld]);
	return SUCCESS;
IO_LLG_Open_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
/*	IO_Error_Set( error_code );*/
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_LLG_Close( struct io_device_dataset * ds )
{
	int			ld;  /* short hand for local device */
	int			error_code;
	io_llg_interrupt_context_type * interrupt_context;

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: enter\n", __func__ );
#endif

	if ( NULL == ds->private_data ) return SUCCESS;

	ld		   = ds->iod;
	error_code = SUCCESS;

	/* interrupt context */

	/* disable interrupts */

	/* close the device/region */
	close( io_llg_private[ld].fd );
	if ( io_llg_fd < 0 )
	/*TODO: { error_code = EC_BUILD_OS( errno );		goto IO_LLG_Close_Error; }*/
	{ error_code = errno;		goto IO_LLG_Close_Error; }
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	io_llg_private[ld].fd = IO_LLG_INVALID_FILE;
	ds->private_data      = NULL;
	return SUCCESS;
IO_LLG_Close_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
	/* IO_Error_Set( error_code ); */
/*	if ( NULL != io_llg_private[ld].handle ) io_llg_private[ld].handle = NULL; */
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
#if defined( IO_DEBUG )
static int IO_LLG_Validity( int iod, size_t virtual_address, size_t data_bytes )
{ /* Refer to IO_Validity().  Make sure that we perform any validity tests */
	(void) virtual_address;
	(void) data_bytes;
	if ( IO_LLG_INVALID_FILE == io_llg_fd ) return -EC_IO_CORE_UNINITIALIZED;
	if ( DEVICE_UNDEFINED == iod ) return -EC_IO_NOT_CAPTURED;
	return SUCCESS;
}
#endif
/*********************************************************************************/
int IO_LLG_Terminate( void )
{
	int	error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif

	close( io_llg_fd );
	if ( io_llg_fd < 0 )
	/*TODO: { error_code = EC_BUILD_OS( errno );		goto IO_LLG_Terminate_Error; }*/
	{ error_code = errno;		goto IO_LLG_Terminate_Error; }
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	io_llg_fd = IO_LLG_INVALID_FILE;
	return SUCCESS;
IO_LLG_Terminate_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
/*	IO_Error_Set( error_code );*/
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_LLG_Initialize( void )
{
	int		error_code;
	size_t	index;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif

	Mem_Set( io_llg_private, 0, sizeof( struct io_llg_device_private ) * DEVICE_QTY );

/*	TODO: change driver name??  have the name come from auldrv.h
	TODO: set debug options??*/
	for ( index = 0; index < DEVICE_QTY; index++ ) io_llg_private[index].fd = IO_LLG_INVALID_FILE;
	io_llg_fd = open( "/dev/auldrv", O_RDWR );
	if ( io_llg_fd < 0 )
	/*TODO: { error_code = EC_BUILD_OS( errno );		goto IO_LLG_Initialize_Error; }*/
	{ error_code = errno;		goto IO_LLG_Initialize_Error; }
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_LLG_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
/*	IO_Error_Set( error_code );*/
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_LLG_Definition_Available( void )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	if ( IO_LLG_INVALID_FILE == io_llg_fd ) /* must be opened */
	{ error_code = EC_IO_CORE_UNINITIALIZED;	goto IO_LLG_Definition_Available_Error;	}

	error_code = ioctl( io_llg_fd, CMD_AULDRV_DEVICE_DEFINITION_AVAILABLE, 0 );
	if      ( error_code ) goto IO_LLG_Definition_Available_Error;

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_LLG_Definition_Available_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_LLG_Definition_Get( struct io_definition * io_definition )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	if ( IO_LLG_INVALID_FILE == io_llg_fd ) /* must be opened */
	{ error_code = EC_IO_CORE_UNINITIALIZED;	goto IO_LLG_Definition_Get_Error;	}

	/* returns the devices, by acronym name, associated parameters */
	error_code = ioctl( io_llg_fd, CMD_AULDRV_DEVICE_DEFINITION_GET, io_definition );
	if      ( error_code ) goto IO_LLG_Definition_Get_Error;

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_LLG_Definition_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
io_target_type io_target_definition =
{
	SYS_TYPE_MAKE(io_target_type),
	IO_LLG_Initialize,
	IO_LLG_Terminate,
	IO_LLG_Open,
	IO_LLG_Close,
	IO_LLG_Ioctl,
	IO_LLG_Interrupt_Enable,
	IO_LLG_Interrupt_Disable,
	IO_LLG_Definition_Available,/* IO_LLG_Definition_Available, */
	IO_LLG_Definition_Get,		/* IO_LLG_Definition_Get, */
#if defined( IO_DEBUG )
	IO_LLG_Validity,
#endif
	/* wlg = win32 library generic */
	IO_TARGET_NAME_SUFFIX( MODULE_IO_NAME, "llg" ),
	IO_MODULE_REV,
	IO_COMMAND_REV,
	IO_STATUS_REV
};
