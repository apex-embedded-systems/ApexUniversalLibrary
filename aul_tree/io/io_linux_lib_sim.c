
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

#include "io_private.h"
#include "io_shared.h"
#include "../shared/mem.h"
#include "../shared/status.h"     /* error codes and APIs					*/
#include "../shared/error.h"      /* error handling							*/
#include "../shared/device.h"		 /* device qty	 and device undefined	*/
#include "../shared/target.h"		 /* TARGET_BUS_APERTURE_WIDTH_BYTES		*/
#include "../shared/cmd.h"			 /* Ioctl() commands							*/
/*#include "errno-base.h"	*/		 /* system level error codes				*/
#include "../shared/cmd_macros.h"
#include "../devices/dev_avail_module.h"	/* DEVICE_STX104, etc */
#include "../str_safe/str_safe.h"
#include "../shared/cmd_macros.h"
/*********************************************************************************/
#define IO_MODULE_REV  "$Date: 2014-06-10 15:02:29 -0500 (Tue, 10 Jun 2014) $"
/*********************************************************************************/
#define IO_ADDRESS_SPACE  1024
/*********************************************************************************/
/* simulator memory space */
static uint8_t io_space[IO_ADDRESS_SPACE];
/*********************************************************************************/
IO_ID_ENUM IO_ID_Get( void ) {	return IO_ID_LINUX_LIB_SIMULATOR;  }
/*********************************************************************************/
int IO_Read_U8( int iod, size_t offset, uint8_t * value )
{
	int			error_code;
	uint8_t *	mem;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	mem = (uint8_t *) &(io_space[offset]);
	*value = *mem;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]=>0x%02X\n", __func__, offset, (int) *value );
#endif
	error_code = SUCCESS;
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Read_U16( int iod, size_t offset, uint16_t * value )
{
	int		   error_code;
	uint16_t * mem;
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
		mem = (uint16_t *) &(io_space[offset]);
		*value = *mem;
#if defined( IO_DEBUG_LOG )
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
	uint8_t *	mem;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	for ( index = 0; index < count; index++ )
	{
		mem = (uint8_t *) &(io_space[offset]);
		buffer[index] = *mem;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]=>0x%02X\n", __func__, offset, (int) *mem );
#endif
	}
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
		for ( index = 0; index < count; index++ )
		{
			error_code = IO_Read_U16( iod, offset, &(buffer[index]) );
			if ( error_code ) return error_code;
		}
	}
	error_code = SUCCESS;
IO_Read_Repeat_U16_Error:
	IO_Error_Set( error_code );
	return error_code;	
}
/*********************************************************************************/
int IO_Write_U8( int iod, size_t offset, uint8_t  value )
{
	int		  error_code;
	uint8_t * mem;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	mem = (uint8_t *) &(io_space[offset]);
	*mem = value;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]<=0x%02X\n", __func__, offset, *mem );
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
		uint16_t * mem;
		mem = (uint16_t *) &(io_space[offset]);
		*mem = value;
#if defined( IO_DEBUG_LOG )
		Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]<=0x%04X\n", __func__, offset, (int) *mem );
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
	uint8_t *	mem;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	for ( index = 0; index < count; index++ )
	{
		mem = (uint8_t *) &(io_space[offset]);
		*mem = buffer[index];
#if defined( IO_DEBUG_LOG )
		Debug_Log( IO_DEBUG_LOG "%s: mem[0x%04X]<=0x%02X\n", __func__, offset, *mem );
#endif
	}
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
		for ( index = 0; index < count; index++ )
		{
			error_code = IO_Write_U16( iod, offset, buffer[index] );
			if ( error_code ) goto IO_Write_Repeat_U16_Error;
		}
	}
	error_code = SUCCESS;
IO_Write_Repeat_U16_Error:
	IO_Error_Set( error_code );
	return error_code;	
}
/*********************************************************************************/
static int IO_LLS_Interrupt_Disable( struct io_device_dataset * ds )
{
	(void) ds;
	return -EC_IO_NOT_IMPLEMENTED;
}
/*********************************************************************************/
static int IO_LLS_Interrupt_Enable( struct io_device_dataset * ds, io_int_handler handler, void * private_data )
{
	(void) ds;
	(void) handler;
	(void) private_data;
	return -EC_IO_NOT_IMPLEMENTED;
}
/*********************************************************************************/
#include "../shared/device_list.h"
static struct io_definition io_lls_definition[DEVICE_QTY];
/*********************************************************************************/
static int IO_LLS_Ioctl( struct io_device_dataset * ds, unsigned int command, unsigned long argument )
{
	int		error_code;
	(void)   ds;
	(void)   command;
	(void)   argument;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif	
	error_code = EC_IO_IOCTL_COMMAND_UNKNOWN; /* command invalid */
	switch( command )
	{	/* emulate the driver */
		case CMD_AULDRV_NAME:
			{
				if ( IO_COPY_TO_USER( argument, io_target_definition.name, String_Length( io_target_definition.name ) + 1 )  )
				{ error_code = EC_IO_COPY_TO_USER; goto IO_LLS_Ioctl_Error;  }
				else
				{ error_code = SUCCESS;  }
			}
			break;
		case CMD_AULDRV_DEVICE_LIST_GET:
			{ /* copy the io_definitions into place */
				device_list_type	device_list;
				size_t				count;
				Mem_Set( &device_list, 0, sizeof( device_list ) );
				count = 0;
				while( IOC_PHYSICAL_ID_NULL != io_lls_definition[count].physical_id )
				{
					Mem_Copy( &(device_list.region[count]), &(io_lls_definition[count]), sizeof( struct io_definition ) );
					count++;
				}
				device_list.region_count = count;
				device_list.type         = SYS_TYPE_MAKE( device_list_type );
				if ( IO_COPY_TO_USER( argument, &device_list, sizeof(device_list) ) )
				{ error_code = EC_IO_COPY_TO_USER; goto IO_LLS_Ioctl_Error;  }
			}
			return -EC_IO_WARN_DEVICE_LIST_PARTIAL;
		case CMD_AULDRV_DEBUG_ACTIVE_GET:
			{ /* return zero if IO_DEBUG is not defined, otherwise non-zero */
				int debug_active;
#ifdef IO_DEBUG
				debug_active = 1;
#else
				debug_active = 0;
#endif
				if ( IO_COPY_TO_USER( argument, &debug_active, sizeof( debug_active ) ) )
				{ error_code = EC_AULDRV_COPY_TO_USER;	goto IO_LLS_Ioctl_Error;  }
			}
			break;
	}
	if ( error_code ) goto IO_LLS_Ioctl_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_LLS_Ioctl_Error:
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return error_code;
}
/*********************************************************************************/
static int IO_LLS_Open( struct io_device_dataset * ds )
{
	int	error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif	
	error_code = SUCCESS;
	if ( ( ds->io_definition.region_offset + ds->io_definition.region_bytes ) >= IO_ADDRESS_SPACE )
	{	error_code = EC_IO_OPEN;	goto IO_LLS_Open_Error;	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	ds->private_data = io_space;
	return SUCCESS;
IO_LLS_Open_Error:
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return error_code;
}
/*********************************************************************************/
static int IO_LLS_Close( struct io_device_dataset * ds )
{
	ds->private_data = NULL;
	return SUCCESS;
}
/*********************************************************************************/
#if defined( IO_DEBUG )
static int IO_LLS_Validity( int iod, size_t virtual_address, size_t data_bytes )
{
	int error_code;
	/* simulator specific - test to be sure that we are within the IO address space */
	if ( ( sizeof( uint16_t ) == data_bytes ) && ( (virtual_address + 1) >= IO_ADDRESS_SPACE ) )
	{	error_code = EC_IO_ADDRESS_RANGE;				return error_code;	}
	else if ( virtual_address >= IO_ADDRESS_SPACE )
	{	error_code = EC_IO_ADDRESS_RANGE;				return error_code; /* not valid */	}
	return SUCCESS;
}
#endif
/*********************************************************************************/
static int IO_LLS_Terminate( void )
{
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
}
/*********************************************************************************/
static int IO_LLS_Initialize( void )
{
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif

	/* initialize our internal I/O emulation space */
	Mem_Set( io_space, 0, sizeof( io_space ) );

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
}
/*********************************************************************************/
static struct io_definition io_lls_definition[DEVICE_QTY] = 
{
/*   id  minor  offset		bytes   restrict_8   intr#			acronym   */
	{ IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_STX104  ),		
			 0,		0x300,	0x10,			0,				0,		{ "stx104" }		},
	{ IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_TRACERE ),		
			 1,		0x310,	0x10,			0,				0,		{ "tracere" }		},
	{ 0,		
			 0,			 0,		0,			0,				0,		{ ""		}		},
};
/*********************************************************************************/
static int IO_LLS_Definition_Available( void )
{
	return SUCCESS;
}
/*********************************************************************************/
static int IO_LLS_Definition_Get( struct io_definition * io_definition )
{
/* search for matching acronym.  If that fails, then search by minor number.  */
	size_t						index;
	size_t						length;
	char *						acronym;

	/* fist, attempt to match acronyms */
	index = 0;
	acronym = io_lls_definition[index].acronym;
	while ( NULL != acronym )
	{
		length = String_Length( acronym );
		if ( 0 == length ) break; 
		if ( 0 == String_Compare( acronym, io_definition->acronym, IO_DEFINITION_ACRONYM_SIZE ) )
		{	/* copy selected values into the user structure */
			Mem_Copy( io_definition, &(io_lls_definition[index]), sizeof( struct io_definition ) );
			return SUCCESS;
		}
		index++;
		acronym = io_lls_definition[index].acronym;
	}
/* IO_LLS_Definition_Get_Error:  */
	return EC_IO_DEFINITION_NOT_FOUND;
}
/*****************************************************************************************
*/
io_target_type io_target_definition =
{
	SYS_TYPE_MAKE(io_target_type),
	IO_LLS_Initialize,
	IO_LLS_Terminate,
	IO_LLS_Open,
	IO_LLS_Close,
	IO_LLS_Ioctl,
	IO_LLS_Interrupt_Enable,
	IO_LLS_Interrupt_Disable,
	IO_LLS_Definition_Available,
	IO_LLS_Definition_Get,
#if defined( IO_DEBUG )
	IO_LLS_Validity,
#endif
	/* wls = win32 library simulator */
	IO_TARGET_NAME_SUFFIX( MODULE_IO_NAME, "lls" ),
	IO_MODULE_REV,
	IO_COMMAND_REV,
	IO_STATUS_REV
};
