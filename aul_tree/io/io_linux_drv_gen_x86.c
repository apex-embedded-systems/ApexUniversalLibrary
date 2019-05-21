
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

*********************************************************************************/

#include <linux/ioport.h>
#include <asm/io.h>

#include "io_private.h"
#include "io_shared.h"
#include "../shared/mem.h"
#include "../shared/status.h"     /* error codes and APIs					*/
#include "../shared/error.h"      /* error handling							*/
#include "../shared/device.h"		 /* device qty	 and device undefined	*/
#include "../shared/target.h"		 /* TARGET_BUS_APERTURE_WIDTH_BYTES		*/
#include "../shared/cmd.h"			 /* Ioctl() commands							*/
#include "../shared/cmd_macros.h"
/*********************************************************************************/
#define IO_MODULE_REV  "$Date: 2014-05-20 17:36:34 -0500 (Tue, 20 May 2014) $"
/*********************************************************************************/
/* enable or set to 1 in order to post dmesg when IO_Read or IO_Write functions */
#define IO_LDGX86_RW_LOG 1
/* enable or set to 1 in order to use OS I/O string operations */
#define IO_LDGX86_USE_STRING  0
/*********************************************************************************/
#ifndef __KERNEL__
# error "io_linux_genx86.c: Cannot be used in application space!"
#endif
/*******************************************************************************/
typedef struct io_ldgx86_intr_diagnostics
{
	int device;
	uint32_t counter;		/* Number of interrupts received */
	uint32_t lost;			/* Number of interrupts not yet handled */
	int		 wait_result;	/* See WD_INTERRUPT_WAIT_RESULT values in windrvr.h */
	uint32_t enabled_int_type; /* Interrupt type that was actually enabled
                                 (MSI/MSI-X/Level Sensitive/Edge-Triggered) */
	uint32_t last_message;	/* Message data of the last received MSI/MSI-X
                               (Windows Vista/7); N/A to line-based interrupts) */
};
/*********************************************************************************/
struct io_ldgx86_interrupt_context
{
	size_t								type;
	int									device;
	struct io_ldgx86_intr_diagnostics	diagnostics;
	io_int_handler						interrupt_handler;
	void *								private_data;
};
typedef struct io_ldgx86_interrupt_context io_ldgx86_interrupt_context_type;
/*********************************************************************************/
/* internal data structure specific to this module -- for internal use only */
struct io_ldgx86_device_private 
{
	struct resource *					resource;  /* Linux resource */
	struct io_ldgx86_interrupt_context	interrupt_context;
	io_definition_type *				io_definition;
};
/*********************************************************************************/
static struct io_ldgx86_device_private io_ldgx86_private[DEVICE_QTY];  /* private dataset */ 

/*********************************************************************************/
IO_ID_ENUM IO_ID_Get( void ) {	return IO_ID_LINUX_DRV_GEN_X86;  }
/*********************************************************************************/
int IO_Read_U8( int iod, size_t offset, uint8_t * value )
{
	int error_code;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	*value = (uint8_t) inb( offset );
#if ( IO_LDGX86_RW_LOG == 1 )
	Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]=>0x%02X\n", __func__, offset, (int) *value );
#endif
	error_code = SUCCESS;
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Read_U16( int iod, size_t offset, uint16_t * value )
{
	int error_code;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		uint8_t u8_lo;
		uint8_t u8_hi;
		error_code = IO_Read_U8( iod, offset + 0, &u8_lo );
		if ( error_code ) goto IO_Read_U16_Error;
		error_code = IO_Read_U8( iod, offset + 1, &u8_hi );
		if ( error_code ) goto IO_Read_U16_Error;
		*value = IO_U16_SET_U8_LO_HI(u8_lo,u8_hi);
	}
	else
	{
		*value = (uint16_t) inw( offset );
#if ( IO_LDGX86_RW_LOG == 1 )
		Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]=>0x%04X\n", __func__, offset, (int) *value );
#endif
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
	size_t		index;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
#if ( IO_LDGX86_USE_STRING == 1 )
	insb( (unsigned) offset, (void *)buffer, (unsigned) count );
#else
	for ( index = 0; index < count; index++ )
	{
		buffer[index] = (uint8_t) inb( offset );
# if ( IO_LDGX86_RW_LOG == 1 )
	Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]=>0x%02X\n", __func__, offset, (int) buffer[index] );
# endif
	}
#endif
	error_code = SUCCESS;
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Read_Repeat_U16( int iod, size_t offset, uint16_t * buffer, size_t count )
{
	int			error_code;
	size_t		index;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		uint8_t u8_lo, u8_hi;
		for ( index = 0; index < count; index++ )
		{
			error_code = IO_Read_U8( iod, offset + 0, &u8_lo );
			if ( error_code ) goto IO_Read_Repeat_U16_Error;
			error_code = IO_Read_U8( iod, offset + 1, &u8_hi );
			if ( error_code ) goto IO_Read_Repeat_U16_Error;
			buffer[index] = IO_U16_SET_U8_LO_HI(u8_lo,u8_hi);
		}
	}
	else
	{
#if ( IO_LDGX86_USE_STRING == 1 )
		insw( (unsigned) offset, (void *) buffer, (unsigned) count );
#else
		for ( index = 0; index < count; index++ )
		{
			error_code = IO_Read_U16( iod, offset, &(buffer[index]) );
			if ( error_code ) goto IO_Read_Repeat_U16_Error;
		}
#endif
	}
	error_code = SUCCESS;
IO_Read_Repeat_U16_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Write_U8( int iod, size_t offset, uint8_t  value )
{
	int error_code;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	outb( value, offset );
#if ( IO_LDGX86_RW_LOG == 1 )
	Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]<=0x%02X\n", __func__, offset, value );
#endif
	error_code = SUCCESS;
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Write_U16( int iod, size_t offset, uint16_t  value )
{
	int	error_code;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		uint8_t u8_lo = IO_U16_GET_U8_LO( value );
		uint8_t u8_hi = IO_U16_GET_U8_HI( value );
		error_code = IO_Write_U8( iod, offset + 0, u8_lo );
		if ( error_code ) goto IO_Write_U16_Error;
		error_code = IO_Write_U8( iod, offset + 1, u8_hi );
		if ( error_code ) goto IO_Write_U16_Error;
	}
	else
	{
		outw( value, offset );
#if ( IO_LDGX86_RW_LOG == 1 )
		Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]<=0x%04X\n", __func__, offset, (int) value );
#endif
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
	size_t		index;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
#if ( IO_LDGX86_USE_STRING == 1 )
	outsb( (unsigned) offset, (void *)buffer, (unsigned) count );
#else
	for ( index = 0; index < count; index++ )
	{
		outb( buffer[index], offset );
# if ( IO_LDGX86_RW_LOG == 1 )
		Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]<=0x%02X\n", __func__, offset, buffer[index] );
# endif
	}
#endif
	error_code = SUCCESS;
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Write_Repeat_U16( int iod, size_t offset, uint16_t * buffer, size_t count )
{
	int			error_code;
	size_t		index;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		uint8_t u8;
		for ( index = 0; index < count; index++ )
		{
			u8 = IO_U16_GET_U8_LO( buffer[index] );
			error_code = IO_Write_U16( iod, offset + 0, u8 );
			if ( error_code ) goto IO_Write_Repeat_U16_Error;
			u8 = IO_U16_GET_U8_HI( buffer[index] );
			error_code = IO_Write_U16( iod, offset + 1, u8 );
			if ( error_code ) goto IO_Write_Repeat_U16_Error;
		}
	}
	else
	{
#if ( IO_LDGX86_USE_STRING == 1 )
	outsw( (unsigned) offset, (void *)buffer, (unsigned) count );
#else
		for ( index = 0; index < count; index++ )
		{
			error_code = IO_Write_U16( iod, offset, buffer[index] );
			if ( error_code ) goto IO_Write_Repeat_U16_Error;
		}
	}
#endif
	error_code = SUCCESS;
IO_Write_Repeat_U16_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
static int IO_LDGX86_Interrupt_Disable( struct io_device_dataset * ds )
{
	(void) ds;
	return -EC_IO_NOT_IMPLEMENTED;
}
/*********************************************************************************/
static int IO_LDGX86_Interrupt_Enable( struct io_device_dataset * ds, io_int_handler handler, void * private_data )
{
	(void) ds;
	(void) handler;
	(void) private_data;
	return -EC_IO_NOT_IMPLEMENTED;
}
/*********************************************************************************/
static int IO_LDGX86_Ioctl( struct io_device_dataset * ds, unsigned int command, unsigned long argument )
{
	int		error_code;
	(void)   ds;
	(void)   command;
	(void)   argument;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif	
	error_code = EC_IO_IOCTL_COMMAND_UNKNOWN; /* command invalid */

	if ( error_code ) goto IO_LDGX86_Ioctl_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_LDGX86_Ioctl_Error:
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return error_code;
}
/*********************************************************************************/
static int IO_LDGX86_Open( struct io_device_dataset * ds )
{
	int					error_code;
	struct resource *	resource;
	int					ld;  /* short for local device */
	io_ldgx86_interrupt_context_type * interrupt_context;
	static char name_path_device[IO_DEFINITION_ACRONYM_SIZE];
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	ld	= ds->iod;
	interrupt_context = &(io_ldgx86_private[ld].interrupt_context);
	Mem_Set( interrupt_context, 0, sizeof( io_ldgx86_interrupt_context_type ) );
	interrupt_context->device = ld;

	/* open/request region */
	io_ldgx86_private[ld].resource = NULL;
	resource = request_region( (unsigned long) ds->io_definition.region_offset, 
							   (unsigned long) ds->io_definition.region_bytes, 
							   (const char *)  ds->io_definition.acronym 
							 );
	if ( NULL == resource ) 
	{  error_code = EC_IO_REGION_REQUEST;  goto IO_LDGX86_Open_Error;	}



	//map in and enable interrupts here.

/*
#if(0)
	interrupt_context = &(io_ldgx86_private[*device].interrupt_context);
	Mem_Set( interrupt_context, 0, sizeof( struct io_interrupt_context ) );
	interrupt_context->device = *device;
#endif
*/
	io_ldgx86_private[ld].resource = resource;
	io_ldgx86_private[ld].io_definition = &( ds->io_definition );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	ds->private_data = &(io_ldgx86_private[ld]);
	return SUCCESS;
IO_LDGX86_Open_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
/*	IO_Error_Set( error_code );*/
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_LDGX86_Close( struct io_device_dataset * ds )
{
	int					error_code;
	struct resource *	resource;
	int					ld;  /* short hand for local device */
	io_ldgx86_interrupt_context_type * interrupt_context;

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: enter\n", __func__ );
#endif

	if ( NULL == ds->private_data ) return SUCCESS;

	ld		   = ds->iod;
	error_code = SUCCESS;

	/* interrupt context */

	/* disable interrupts */

	/* close the device/region */
	release_region( (unsigned long) io_ldgx86_private[ld].io_definition->region_offset,
			        (unsigned long) io_ldgx86_private[ld].io_definition->region_bytes
			      );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	io_ldgx86_private[ld].resource      = NULL;
	io_ldgx86_private[ld].io_definition = NULL;
	ds->private_data = NULL;
	return SUCCESS;
IO_LDGX86_Close_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
	/* IO_Error_Set( error_code ); */
/*	if ( NULL != io_ldgx86_private[ld].handle ) io_ldgx86_private[ld].handle = NULL; */
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
#if defined( IO_DEBUG )
static int IO_LDGX86_Validity( size_t virtual_address, size_t data_bytes )
{ /* Refer to IO_Validity().  Make sure that we perform any validity tests */
	(void) virtual_address;
	(void) data_bytes;
	return SUCCESS;
}
#endif
/*********************************************************************************/
static int IO_LDGX86_Terminate( void )
{
	int	error_code;
	int index;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	/* close the device/regions, if any */
	for ( index = 0; index < DEVICE_QTY; index++ )
	{
		if ( io_ldgx86_private[index].resource )
		{
			release_region( (unsigned long) io_ldgx86_private[index].io_definition->region_offset,
							(unsigned long) io_ldgx86_private[index].io_definition->region_bytes
						  );
			io_ldgx86_private[index].resource      = NULL;
			io_ldgx86_private[index].io_definition = NULL;
		}
	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_LDGX86_Terminate_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
/*	IO_Error_Set( error_code );*/
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_LDGX86_Initialize( void )
{
	int		error_code;
	size_t	index;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	Mem_Set( io_ldgx86_private, 0, sizeof( struct io_ldgx86_device_private ) * DEVICE_QTY );

	for ( index = 0; index < DEVICE_QTY; index++ ) 
	{
		io_ldgx86_private[index].resource      = NULL;
		io_ldgx86_private[index].io_definition = NULL;
	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_LDGX86_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
/*	IO_Error_Set( error_code );*/
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
#if(0)
/*********************************************************************************/
static int IO_LDGX86_Definition_Available( void )
{
	return -EC_IO_DEFINITION_NOT_FOUND;
}
/*********************************************************************************/
static int IO_LDGX86_Definition_Get( struct io_definition * io_definition )
{
	return -EC_IO_DEFINITION_NOT_FOUND;
}
#endif
/*****************************************************************************************
*/
io_target_type io_target_definition =
{
	SYS_TYPE_MAKE(io_target_type),
	IO_LDGX86_Initialize,
	IO_LDGX86_Terminate,
	IO_LDGX86_Open,
	IO_LDGX86_Close,
	IO_LDGX86_Ioctl,
	IO_LDGX86_Interrupt_Enable,
	IO_LDGX86_Interrupt_Disable,
	NULL, /* IO_LDGX86_Definition_Available, */
	NULL, /* IO_LDGX86_Definition_Get,  */
#if defined( IO_DEBUG )
	IO_LDGX86_Validity,
#endif
	/* wls = win32 library simulator */
	IO_TARGET_NAME_SUFFIX( MODULE_IO_NAME, "ldgx86" ),
	IO_MODULE_REV,
	IO_COMMAND_REV,
	IO_STATUS_REV
};
