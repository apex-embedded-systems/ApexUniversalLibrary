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



*********************************************************************************/

#include "io_private.h"
#include "../shared/mem.h"
#include "../shared/status.h"     /* error codes and APIs					*/
#include "../shared/error.h"      /* error handling							*/
#include "../shared/device.h"		 /* device qty	 and device undefined	*/
#include "../shared/target.h"		 /* TARGET_BUS_APERTURE_WIDTH_BYTES		*/
#include "../shared/cmd.h"			 /* Ioctl() commands							*/
/* #include "errno-base.h"	*/			 /* system level error codes				*/
#include "../shared/ts_svn.h"
/* #include "../shared/cmd_macros.h" */

/*********************************************************************************/

/*********************************************************************************/
struct io_dataset 
{
	int			initialize_count;
	int			last_error_code;
};
/*********************************************************************************/
static struct io_dataset io = {  0  };
static struct io_device_dataset io_ds[DEVICE_QTY] = {  0  };  /* ds = dataset */

/*********************************************************************************/
/*********************************************************************************/
#if defined( IO_DEBUG )
/*********************************************************************************/
static int IO_IsNotReady_Core( void ) /* io has not been initialized */
{
	if ( 0 == io.initialize_count ) return EC_IO_CORE_UNINITIALIZED;
	return SUCCESS;
}
/*********************************************************************************/
static int IO_IsNotReady_Device_Uncaptured( int device )
{
	int error_code;
	/* IO not initialized */
	if ( 0 == io.initialize_count )
	{	error_code = EC_IO_CORE_UNINITIALIZED;		return error_code; 	}
	/* IO not open */
	if ( 0 == io_ds[device].active )
	{	error_code = EC_IO_OPEN_NOT;					return error_code;	}
	return SUCCESS; /* yes, it is valid */
}
/*********************************************************************************/
static int IO_IsNotReady_Device( int iod )
{
	int error_code;
	/* IO not initialized */
	if ( 0 == io.initialize_count )
	{	error_code = EC_IO_CORE_UNINITIALIZED;	return error_code; 	}
/*TODO*/
	if ( DEVICE_UNDEFINED == iod )
	{	error_code = EC_IO_NOT_CAPTURED;		return error_code;	}
	/* IO not open */
	if ( 0 == io_ds[iod].active )
	{	error_code = EC_IO_OPEN_NOT;			return error_code;	}
	return SUCCESS; /* yes, it is valid */
}
/*********************************************************************************/
int IO_IsNotValid( int iod, size_t virtual_address, size_t data_bytes )
{
	/* IO not captured - i.e. limited to one open device (search for IO_Capture() IO_Release() ) */
	if ( DEVICE_UNDEFINED == iod )
	{	io.last_error_code = EC_IO_NOT_CAPTURED;				goto IO_IsNotValid_Error; /* not valid */	}
	/* U16 data on even boundary - test to be sure that 16-bit addresses are on even boundaries */
	if ( ( sizeof( uint16_t ) == data_bytes ) && ( virtual_address & 0x01 ) )
	{	io.last_error_code = EC_IO_ADDRESS_ODD_U16;			goto IO_IsNotValid_Error; /* not valid */	}

#if defined( IO_DEBUG )
	io.last_error_code = (* io_target_definition.valid_not)( iod, virtual_address, data_bytes );
	if ( io.last_error_code ) goto IO_IsNotValid_Error;
#endif
	return 0; /* yes, it is valid */
IO_IsNotValid_Error:
	if ( io.last_error_code < 0 ) io.last_error_code = -io.last_error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, io.last_error_code );
	return 1;
}
#endif
/*********************************************************************************/
int IO_Is_Restrict_8Bit( int iod )
{
	if ( io_ds[iod].bus_restrict_8bit ) return 1;
	return 0;
}
/*********************************************************************************/
void IO_Error_Set( int error_code )
{
	if ( error_code < 0 ) error_code = -error_code;
/*TODO: how to pass function name or something more meaningful.*/
	if ( error_code ) Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
}
/*********************************************************************************/
static int IO_Error_Last_Get_And_Clear( void )
{
	int error_code = io.last_error_code;
	io.last_error_code = SUCCESS;
	return error_code;
}
/*********************************************************************************/
static int IO_Error_Last_Get_Command( unsigned long argument )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	error_code = IO_Error_Last_Get_And_Clear();
	if ( error_code < 0 ) error_code = -error_code;
	/* if argument is null, then assume for internal use */
	if ( ((unsigned long) NULL ) == argument ) return -error_code;
	if ( IO_COPY_TO_USER( argument, &error_code, sizeof( error_code ) ) )
	{ error_code = EC_IO_COPY_TO_USER;	goto IO_Error_Last_Get_Command_Error;  }
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Error_Last_Get_Command_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Error_Last_Get( void )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Error_Last_Get_Error;
#endif
	error_code = IO_Error_Last_Get_Command( (unsigned long) NULL );
	if ( error_code ) goto IO_Error_Last_Get_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Error_Last_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_Version_Command( unsigned long argument )
{
	int			error_code;
	uint32_t		version;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	version = TSSVN_YYMMDDHH_DEC( io_target_definition.svn_module_date );
	/* if argument is null, then assume for internal use */
	if ( ((unsigned long) NULL ) == argument ) return version;

	if ( IO_COPY_TO_USER( argument, &version, sizeof( version ) ) )
	{ error_code = EC_IO_COPY_TO_USER;	goto IO_Version_Command_Error;  }
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Version_Command_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Version( uint32_t * version )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Version_Error;
#endif
	error_code = IO_Version_Command( (unsigned long) version );
	if ( error_code ) goto IO_Version_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Version_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_Definition_Available_Command( void )
{
	int	error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	/* reports SUCCESS if available, otherwise error code */
	if ( NULL != io_target_definition.definition_available )
	{
		error_code = ( * io_target_definition.definition_available)();
		if ( error_code ) goto IO_Definition_Available_Command_Error;
	}
	else
	{	error_code = EC_IO_DEFINITION_NOT_FOUND;  goto IO_Definition_Available_Command_Error;	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Definition_Available_Command_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Is_Definition_Available( void )
{
	int	error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Is_Definition_Available_Error;
#endif
	error_code = IO_Definition_Available_Command();
	if ( error_code ) goto IO_Is_Definition_Available_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Is_Definition_Available_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_Definition_Get_Command( unsigned long argument )
{
	int						error_code;
	io_definition_type   io_definition_local;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	if ( NULL != io_target_definition.definition_get )
	{
		/* get the io_definition from user space */
		if ( IO_COPY_FROM_USER( &io_definition_local, argument, sizeof( io_definition_local ) ) )
		{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Definition_Get_Command_Error;	 }

		error_code = ( * io_target_definition.definition_get)( &io_definition_local );
		if ( error_code ) goto IO_Definition_Get_Command_Error;

		/* pass it back to the user with additional information */
		if ( IO_COPY_TO_USER( argument, &io_definition_local, sizeof( io_definition_local ) ) )
		{ error_code = EC_IO_COPY_TO_USER;	goto IO_Definition_Get_Command_Error;  }
	}
	else
	{	error_code = EC_IO_DEFINITION_NOT_FOUND;  goto IO_Definition_Get_Command_Error;	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Definition_Get_Command_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Definition_Get( io_definition_type * io_params )
{
	int	error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
#if defined( IO_DEBUG )
	error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Definition_Get_Error;
#endif
	error_code = IO_Definition_Get_Command( (unsigned long) io_params );
	if ( error_code ) goto IO_Definition_Get_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Definition_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
/* TODO: make this something that can be sized at start up. */
#define IO_BUFFER_LOCAL_SIZE  512
static char io_buffer_local[IO_BUFFER_LOCAL_SIZE];
/*********************************************************************************/
static int IO_Xfr_Command( int iod, unsigned long argument )
{
	int			error_code;
	size_t		address_limit;
	io_xfr_type xfr;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &xfr, argument, sizeof( xfr ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Xfr_Command_Error;	 }
	/* Test the range of the virtual_address to make sure it is within
	   the devices range.
	*/
	address_limit = io_ds[iod].io_definition.region_offset;
	if ( xfr.offset < address_limit /* lower end */ )
	{	error_code = EC_IO_ADDRESS_RANGE;	goto IO_Xfr_Command_Error_Return_To_User;	}
	address_limit += io_ds[iod].io_definition.region_bytes; 
	if ( xfr.offset >= address_limit /* upper end */ )
	{	error_code = EC_IO_ADDRESS_RANGE;	goto IO_Xfr_Command_Error_Return_To_User;	}

	error_code = SUCCESS;
	switch( xfr.command )
	{
		case IO_XFR_WRITE_U8:
			IO_Write_U8(  iod, xfr.offset, xfr.data.u8     );
			break;
		case IO_XFR_WRITE_U16:
			IO_Write_U16( iod, xfr.offset, xfr.data.u16    );
			break;
		case IO_XFR_READ_U8:
			IO_Read_U8(   iod, xfr.offset, &(xfr.data.u8)  );
			break;
		case IO_XFR_READ_U16:
			IO_Read_U16(  iod, xfr.offset, &(xfr.data.u16) );
			break;

/* TODO: all repeat functions to be merged to reduce code duplication. */

		case IO_XFR_WRITE_U8_REPEAT:
			{
				size_t  index				= 0;
				size_t  count_element		= xfr.buf_len;			/* count in elements    */
				size_t	count_u8			= IO_BUFFER_LOCAL_SIZE;	/* buffer size in bytes */
				size_t	count_element_local = IO_BUFFER_LOCAL_SIZE / sizeof( uint8_t );
				while ( count_element > count_element_local )
				{
					if ( IO_COPY_FROM_USER( io_buffer_local, &(xfr.data.buf[index * count_u8]), count_u8 ) )
					{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Xfr_Command_Error;	 }
					IO_Write_Repeat_U8( iod, xfr.offset, (uint8_t *) io_buffer_local, count_element_local );
					count_element = count_element - count_element_local;
					index++;
				}
				count_u8 = count_element * sizeof( uint8_t );
				if ( IO_COPY_FROM_USER( io_buffer_local, &(xfr.data.buf[index * count_u8]), count_u8 ) )
				{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Xfr_Command_Error;	 }
				IO_Write_Repeat_U8( iod, xfr.offset, (uint8_t *) io_buffer_local, count_element );
			}
			break;
		case IO_XFR_WRITE_U16_REPEAT:
			{
				size_t  index				= 0;
				size_t  count_element		= xfr.buf_len;			/* count in elements    */
				size_t	count_u8			= IO_BUFFER_LOCAL_SIZE;	/* buffer size in bytes */
				size_t	count_element_local = IO_BUFFER_LOCAL_SIZE / sizeof( uint16_t );
				while ( count_element > count_element_local )
				{
					if ( IO_COPY_FROM_USER( io_buffer_local, &(xfr.data.buf[index * count_u8]), count_u8 ) )
					{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Xfr_Command_Error;	 }
					IO_Write_Repeat_U16( iod, xfr.offset, (uint16_t *) io_buffer_local, count_element_local );
					count_element = count_element - count_element_local;
					index++;
				}
				count_u8 = count_element * sizeof( uint16_t );
				if ( IO_COPY_FROM_USER( io_buffer_local, &(xfr.data.buf[index * count_u8]), count_u8 ) )
				{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Xfr_Command_Error;	 }
				IO_Write_Repeat_U16( iod, xfr.offset, (uint16_t *) io_buffer_local, count_element );
			}
			break;
		case IO_XFR_READ_U8_REPEAT:
			{
				size_t  index				= 0;
				size_t  count_element		= xfr.buf_len;			/* count in elements    */
				size_t	count_u8			= IO_BUFFER_LOCAL_SIZE;	/* buffer size in bytes */
				size_t	count_element_local = IO_BUFFER_LOCAL_SIZE / sizeof( uint8_t );
				while ( count_element > count_element_local )
				{
					IO_Read_Repeat_U8( iod, xfr.offset, (uint8_t *) io_buffer_local, count_element_local );
					if ( IO_COPY_TO_USER( &(xfr.data.buf[index * count_u8]), io_buffer_local, count_u8 ) )
					{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Xfr_Command_Error;	 }
					count_element = count_element - count_element_local;
					index++;
				}
				IO_Read_Repeat_U8( iod, xfr.offset, (uint8_t *) io_buffer_local, count_element );
				count_u8 = count_element * sizeof( uint8_t );
				if ( IO_COPY_TO_USER( &(xfr.data.buf[index * count_u8]), io_buffer_local, count_u8 ) )
				{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Xfr_Command_Error;	 }
			}
			break;
		case IO_XFR_READ_U16_REPEAT:
			{
				size_t  index				= 0;
				size_t  count_element		= xfr.buf_len;			/* count in elements    */
				size_t	count_u8			= IO_BUFFER_LOCAL_SIZE;	/* buffer size in bytes */
				size_t	count_element_local = IO_BUFFER_LOCAL_SIZE / sizeof( uint16_t );
				while ( count_element > count_element_local )
				{
					IO_Read_Repeat_U16( iod, xfr.offset, (uint16_t *) io_buffer_local, count_element_local );
					if ( IO_COPY_TO_USER( &(xfr.data.buf[index * count_u8]), io_buffer_local, count_u8 ) )
					{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Xfr_Command_Error;	 }
					count_element = count_element - count_element_local;
					index++;
				}
				IO_Read_Repeat_U16( iod, xfr.offset, (uint16_t *) io_buffer_local, count_element );
				count_u8 = count_element * sizeof( uint16_t );
				if ( IO_COPY_TO_USER( &(xfr.data.buf[index * count_u8]), io_buffer_local, count_u8 ) )
				{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Xfr_Command_Error;	 }
			}
			break;
		default:
			break;
	}
	/* return results to user space */
	xfr.error_code = error_code;
	if ( IO_COPY_TO_USER( argument, &xfr, sizeof( xfr ) ) )
	{ error_code = EC_IO_COPY_TO_USER;		goto IO_Xfr_Command_Error;	 }

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Xfr_Command_Error_Return_To_User:
	/* return results to user space */
	xfr.error_code = error_code;
	if ( IO_COPY_TO_USER( argument, &xfr, sizeof( xfr ) ) )
	{ error_code = EC_IO_COPY_TO_USER; }
IO_Xfr_Command_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Xfr( int iod, io_xfr_type * xfr )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s:enter\n", __func__ );
#endif
#if defined( IO_DEBUG )
	if ( SUCCESS != ( error_code = IO_IsNotReady_Device( iod ) ) ) goto IO_Xfr_Error;
#endif
	error_code = IO_Xfr_Command( iod, (unsigned long) xfr );
	if ( error_code ) goto IO_Xfr_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Xfr_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_Interrupt_Disable_Command( int iod )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s:enter\n", __func__ );
#endif
	if ( NULL != io_target_definition.interrupt_disable )
	{
		error_code = ( *io_target_definition.interrupt_disable )( &(io_ds[iod]) );
		if ( error_code ) goto IO_Interrupt_Disable_Command_Error;
	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Interrupt_Disable_Command_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Interrupt_Disable( int iod )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
#if defined( IO_DEBUG )
	if ( SUCCESS != ( error_code = IO_IsNotReady_Device( iod ) ) ) goto IO_Interrupt_Disable_Error;
#endif
	error_code = IO_Interrupt_Disable_Command( iod );
	if ( error_code ) goto IO_Interrupt_Disable_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Interrupt_Disable_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Interrupt_Enable_Command( int iod, unsigned long argument )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s:enter\n", __func__ );
#endif
	if ( NULL != io_target_definition.interrupt_enable )
	{
		struct io_interrupt_info info;
		/* get transfer command from user space */
		if ( IO_COPY_FROM_USER( &info, argument, sizeof( info ) ) )
		{ error_code = EC_IO_COPY_FROM_USER;	goto IO_Interrupt_Enable_Command_Error;	 }

		error_code = ( *io_target_definition.interrupt_enable )( &(io_ds[iod]), info.handler, info.private_data );
		if ( error_code ) goto IO_Interrupt_Enable_Command_Error;
	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Interrupt_Enable_Command_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Interrupt_Enable( int iod, io_int_handler handler, void * private_data )
{
	int error_code;
	struct io_interrupt_info info;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s:enter\n", __func__ );
#endif
#if defined( IO_DEBUG )
	if ( SUCCESS != ( error_code = IO_IsNotReady_Device( iod ) ) )	goto IO_Interrupt_Enable_Error;
#endif
	info.handler = handler;
	info.private_data = private_data;
	info.interrupt_number = io_ds[iod].io_definition.interrupt_number;
	error_code = IO_Interrupt_Enable_Command( iod, (unsigned long) &info  );
	if ( error_code ) goto IO_Interrupt_Enable_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Interrupt_Enable_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Ioctl( int iod, unsigned int command, unsigned long argument )
{
	int		error_code;
	IOC_T	module_logical_id;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	if ( !IOC_MAGIC_IS_VALID( command )  ) 
	{	error_code = EC_IO_IOCTL_MAGIC_NUMBER;	goto IO_Ioctl_Error;		}
	
	/* Is the command intended for a module? */
/*	if ( IOC_INVALID_LOGICAL_ID_MODULE( module_logical_id ) ) */
/*	{	error_code = EC_IO_IOCTL_LOGICAL_ID;	goto IO_Ioctl_Error;		}   */
	/* Is the command intended for this specific module? */

	/* If intended for driver top level, then pass it along. */
	module_logical_id = IOC_GET_LOGICAL_ID_MODULE( command );
	if ( AULDRV_LOGICAL_ID == module_logical_id )
	{
		error_code = ( * io_target_definition.ioctl)( &(io_ds[iod]), command, argument );
		goto IO_Ioctl_Finish;
	}
	/* Make sure the command is intended for this specific software module */
	module_logical_id = IOC_GET_LOGICAL_ID_MODULE( command );
	if ( IO_LOGICAL_ID != module_logical_id )
	{	error_code = EC_IO_IOCTL_LOGICAL_ID;	goto IO_Ioctl_Error;		}

	switch( command )
	{
	/*------------------------------------------------------------------------*/
	case CMD_IO_NOP:							/* does not require device to be open */
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Ioctl_Error;
#endif
		error_code = SUCCESS;
		break;
	/*------------------------------------------------------------------------*/
	case CMD_IO_ERROR_LAST:					/* does not require device to be open */
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Ioctl_Error;
#endif
		error_code = IO_Error_Last_Get_Command( argument );
		break;
	/*------------------------------------------------------------------------*/
	case CMD_IO_VERSION:						/* does not require device to be open */
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Ioctl_Error;
#endif
		error_code = IO_Version_Command( argument );
		break;
	/*------------------------------------------------------------------------*/
	case CMD_IO_ID:							/* does not require device to be open */
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Ioctl_Error;
#endif
		{
			int io_id = (int) IO_ID_Get();
			if ( IO_COPY_TO_USER( argument, &io_id, sizeof( io_id ) ) )
			{ error_code = EC_IO_COPY_TO_USER; }
		}
		break;
	/*------------------------------------------------------------------------*/
	case CMD_IO_DEFINITION_AVAILABLE:	/* does not require device to be open */
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Ioctl_Error;
#endif
		error_code = IO_Definition_Available_Command();
		break;
	/*------------------------------------------------------------------------*/
	case CMD_IO_DEFINITION_GET:			/* does not require device to be open */
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Core();	if ( error_code )	goto IO_Ioctl_Error;
#endif
		error_code = IO_Definition_Get_Command( argument );
		break;
	/*------------------------------------------------------------------------*/
	case CMD_IO_XFR:
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Device( iod );		if ( error_code )	goto IO_Ioctl_Error;
#endif
		error_code = IO_Xfr_Command( iod, argument );
		break;
	/*------------------------------------------------------------------------*/
	case CMD_IO_INTERRUPT_ENABLE:
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Device( iod );		if ( error_code )	goto IO_Ioctl_Error;
#endif
		error_code = IO_Interrupt_Enable_Command( iod, argument );
		break;
	/*------------------------------------------------------------------------*/
	case CMD_IO_INTERRUPT_DISABLE:
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Device( iod );		if ( error_code )	goto IO_Ioctl_Error;
#endif
		error_code = IO_Interrupt_Disable_Command( iod );
		break;
	/*------------------------------------------------------------------------*/
	default:
#if defined( IO_DEBUG )
		error_code = IO_IsNotReady_Device( iod );		if ( error_code )	goto IO_Ioctl_Error;
#endif
		error_code = ( * io_target_definition.ioctl)( &(io_ds[iod]), command, argument );
		break;
	}
IO_Ioctl_Finish:
	if ( error_code ) goto IO_Ioctl_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_Ioctl_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Open( int *				  iod,		 /* return io descriptor */ 
			 io_definition_type * io_definition
		   )
{
	int				error_code;
	int				ld;  /* short hand for local descriptor */
	size_t			index;
	ld = DEVICE_UNDEFINED;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif

	if ( NULL == iod ) 
	{ error_code = EC_IO_OPEN_DEVICE_POINTER;  goto IO_Open_Error;	}

/* TODO -- still need to detect if already open.
	if ( ( DEVICE_UNDEFINED != *iod                ) &&
		 ( 0                != io_ds[index].active )  )
	{ error_code = EC_IO_OPEN_ALREADY;		goto IO_Open_Error;		}
*/

	io.last_error_code = SUCCESS;
	
	if ( NULL == io_definition ) 
	{ error_code = EC_IO_OPEN_IO_DEFINITION;  goto IO_Open_Error;	}
	/* find an empty location in which to allocate the device */
	for ( index = 0; index < DEVICE_QTY; index++ )
	{
		if ( 0 == io_ds[index].active )
		{
			ld = index;
			break;
		}
	}
	if ( DEVICE_UNDEFINED == ld )
	{	error_code = EC_IO_DEVICE_ALLOCATE;  goto IO_Open_Error;	}
	/* make a copy of the io_definition for use during xfr and other situations */
	Mem_Copy( &(io_ds[ld].io_definition), io_definition, sizeof( io_definition_type ) );
#if defined( TARGET_BUS_APERTURE_WIDTH_BYTES )
	if ( TARGET_BUS_APERTURE_WIDTH_BYTES < sizeof( uint16_t ) /* bytes */ )
	{
		io_ds[ld].bus_restrict_8bit = 1; /* yes, restrict */
	}
	else
	{
		io_ds[ld].bus_restrict_8bit = io_definition->region_restrict_8bit;
	}
#else
	io_ds[ld].bus_restrict_8bit = io_definition->region_restrict_8bit;
#endif
	io_ds[ld].iod = ld;
	if ( NULL != io_ds[ld].private_data ) 
	{ error_code = EC_IO_OPEN_ALREADY;	goto IO_Open_Error; }
	error_code = ( * io_target_definition.open)( &(io_ds[ld]) );
	if ( error_code )
	{	IO_Close( iod );		goto IO_Open_Error;		}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	IO_Error_Set( SUCCESS );
	*iod                = ld;
	io_ds[index].active =  1;
	return SUCCESS;
IO_Open_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
	if ( DEVICE_UNDEFINED != ld ) 
	{	
		io_ds[ld].active = 0;
		io_ds[ld].iod = DEVICE_UNDEFINED;
		*iod		  = DEVICE_UNDEFINED;
	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Close( int * iod )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: enter\n", __func__ );
#endif
	if ( NULL == iod ) 
	{ error_code = EC_IO_CLOSE_DEVICE_UNKNOWN;	goto IO_Close_Error;	}
	if ( DEVICE_UNDEFINED == *iod ) 
	{ error_code = EC_IO_CLOSE_DEVICE_UNKNOWN;	goto IO_Close_Error;	}
	if ( 0 == io_ds[*iod].active )
	{	error_code = EC_IO_OPEN_NOT;					goto IO_Close_Error;		}

	error_code = ( * io_target_definition.close)( &(io_ds[*iod]) );
	if ( error_code ) goto IO_Close_Error;

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	IO_Error_Set( SUCCESS );
	io_ds[*iod].active = 0;
	io_ds[*iod].iod    = DEVICE_UNDEFINED; /* device dataset */
	*iod = DEVICE_UNDEFINED;               /* dataset */
	return SUCCESS;
IO_Close_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
	if ( NULL != iod ) 
	{
		if ( DEVICE_UNDEFINED != *iod ) 
		{	
			io_ds[*iod].active = 0;
			io_ds[*iod].iod    = DEVICE_UNDEFINED;
			*iod			   = DEVICE_UNDEFINED;
		}
	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Terminate( void )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	if ( 1 != io.initialize_count )
	{ error_code = EC_IO_TERMINATION_CLOSE_ERROR;  goto IO_Terminate_Error; } 

	error_code = ( * io_target_definition.terminate)();
	if ( error_code ) goto IO_Terminate_Error;	
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	IO_Error_Set( SUCCESS );
	io.initialize_count--;
	return SUCCESS;
IO_Terminate_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
int IO_Initialize( void )
{
	int	error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	/* initialize only once */
	if ( 0 != io.initialize_count )
	{ error_code = EC_IO_INIT_LIB_ERROR;  goto IO_Initialize_Error; }

	/* initialize internal parameters */
	Mem_Set( io_ds, 0, sizeof( struct io_device_dataset ) * DEVICE_QTY );
	io.last_error_code	= SUCCESS;
	module_io_definition.name             = io_target_definition.name;

/* TODO: determine which source file are actually the most current */
	module_io_definition.svn_module_date  = io_target_definition.svn_module_date;
	module_io_definition.svn_command_date = io_target_definition.svn_command_date;
	module_io_definition.svn_status_date  = io_target_definition.svn_status_date;

	error_code = ( * io_target_definition.initialize)();
	if ( error_code ) goto IO_Initialize_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	IO_Error_Set( SUCCESS );
	io.initialize_count++;
	return SUCCESS;
IO_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( module_io_definition.name, __func__, __LINE__, error_code );
	io.last_error_code = error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
module_definition_type module_io_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	IO_Initialize,
	IO_Terminate,
	/* the pointers below are set during IO_Initialize() */
	NULL, /*io_target_definition.name,				*/
	NULL, /*io_target_definition.svn_module_date,	*/
	NULL, /*io_target_definition.svn_command_date,	*/
	NULL, /*io_target_definition.svn_status_date	*/
};
