/********************************************************************************
_____________________________________________________________________________  
Description:

_____________________________________________________________________________
Purpose:

_____________________________________________________________________________
Remark:

_____________________________________________________________________________
Assumption:

The parameter 'board' in all functions has been qualified and tested to be valid.

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


*********************************************************************************/

#include "dev_stx104.h"
#include "../../shared/status.h"
#include "../../shared/compiler.h"  /* inline */
#include "../../shared/callconv.h"
#include "../../aul/aul_shared.h"
#include "../../io/io.h"
#include "dev_stx104_index.h"

/*****************************************************************************/
#define STX104_INDEX_MODULE_REV "$Date: 2015-02-03 11:36:56 -0600 (Tue, 03 Feb 2015) $"
/*****************************************************************************/
const char * STX104_Index_SVN_Time_Stamp( void ) { return STX104_INDEX_MODULE_REV; }
/*****************************************************************************/
void STX104_Index_Bank_Set( devmgr_device_type * device, uint8_t bank /* 0 or 1 */ )
{
	size_t					address;
	uint8_t					scratch;
	uint8_t					value;

	address = device->io_region_offset + STX104_ADC_CONFIGURATION;
	IO_Read_U8( device->io_device, address, &scratch );
	if ( bank == 0 )
	{
		if ( (scratch & 0x80) == 0x80 )
		{
			scratch = scratch & 0x0F;
			value = scratch | 0x30;
			IO_Write_U8( device->io_device, address, value );
			value = scratch | 0x30;
			IO_Write_U8( device->io_device, address, value );
			value = scratch | 0xB0;
			IO_Write_U8( device->io_device, address, value );
			value = scratch | 0xA0;
			IO_Write_U8( device->io_device, address, value );	
		}
	}
	else
	{
		if ( (scratch & 0x80) == 0 )
		{
			scratch = scratch & 0x0F;
			value = scratch | 0xF0;
			IO_Write_U8( device->io_device, address, value );
			value = scratch | 0xF0;
			IO_Write_U8( device->io_device, address, value );
			value = scratch | 0x50;
			IO_Write_U8( device->io_device, address, value );
			value = scratch | 0xD0;
			IO_Write_U8( device->io_device, address, value );
		}
	}
}
/*****************************************************************************/
void STX104_Bank_Device_Set( devmgr_device_type * device, uint8_t bank /* 0 or 1 */ )
{
	STX104_Index_Bank_Set( device, bank );
}
/*****************************************************************************/
void STX104_Bank_Set( int board, uint8_t bank /* 0 or 1 */ )
{
	devmgr_device_type *	device;
	device  = AUL_Device_Get( board );
	STX104_Index_Bank_Set( device, bank );
}
/*****************************************************************************/
void STX104_Index_Device_Write_U8( devmgr_device_type * device, uint8_t index, uint8_t value )
{
	size_t					address;
	address = device->io_region_offset + STX104_INDEX_POINTER;
	IO_Write_U8( device->io_device, address, index );
	address = device->io_region_offset + STX104_INDEX_DATA;
	IO_Write_U8( device->io_device, address, value );
}
/*****************************************************************************/
void STX104_Index_Write_U8( int board, uint8_t index, uint8_t value )
{
	devmgr_device_type *	device;
	device  = AUL_Device_Get( board );
	STX104_Index_Device_Write_U8( device, index, value );
}
/*****************************************************************************/
void STX104_Index_Device_Write_U16( devmgr_device_type * device, uint8_t index, uint16_t value )
{
	size_t					address;
	address = device->io_region_offset + STX104_INDEX_POINTER;
	IO_Write_U8( device->io_device, address, index );
	address = device->io_region_offset + STX104_INDEX_DATA;
	IO_Write_U16( device->io_device, address, value );
}
/*****************************************************************************/
void STX104_Index_Write_U16( int board, uint8_t index, uint16_t value )
{
	devmgr_device_type *	device;
	device  = AUL_Device_Get( board );
	STX104_Index_Device_Write_U16( device, index, value );
}
/*****************************************************************************/
void STX104_Index_Device_Write_U32( devmgr_device_type * device, uint8_t index, uint32_t value )
{
	size_t					address;
	address = device->io_region_offset + STX104_INDEX_POINTER;
	IO_Write_U8( device->io_device, address, index );
	address = device->io_region_offset + STX104_INDEX_DATA;
	IO_Write_U16( device->io_device, address, IO_U32_GET_U16_LO( value ) );
	IO_Write_U16( device->io_device, address, IO_U32_GET_U16_HI( value ) );
}
/*****************************************************************************/
void STX104_Index_Write_U32( int board, uint8_t index, uint32_t value )
{
	devmgr_device_type *	device;
	device  = AUL_Device_Get( board );
	STX104_Index_Device_Write_U32( device, index, value );
}
/*****************************************************************************/
void STX104_Index_Device_Read_U8( devmgr_device_type * device, uint8_t index, uint8_t * value )
{
	size_t					address;
	address = device->io_region_offset + STX104_INDEX_POINTER;
	IO_Write_U8( device->io_device, address, index );
	address = device->io_region_offset + STX104_INDEX_DATA;
	IO_Read_U8( device->io_device, address, value );
}
/*****************************************************************************/
void STX104_Index_Read_U8( int board, uint8_t index, uint8_t * value )
{
	devmgr_device_type *	device;
	device  = AUL_Device_Get( board );
	STX104_Index_Device_Read_U8( device, index, value );
}
/*****************************************************************************/
void STX104_Index_Device_Read_U16( devmgr_device_type * device, uint8_t index, uint16_t * value )
{
	size_t					address;
	address = device->io_region_offset + STX104_INDEX_POINTER;
	IO_Write_U8( device->io_device, address, index );
	address = device->io_region_offset + STX104_INDEX_DATA;
	IO_Read_U16( device->io_device, address, value );
}
/*****************************************************************************/
void STX104_Index_Read_U16( int board, uint8_t index, uint16_t * value )
{
	devmgr_device_type *	device;
	device  = AUL_Device_Get( board );
	STX104_Index_Device_Read_U16( device, index, value );
}
/*****************************************************************************/
void STX104_Index_Device_Read_U32( devmgr_device_type * device, uint8_t index, uint32_t * value )
{
	size_t					address;
	uint16_t				u16_lo, u16_hi;
	address = device->io_region_offset + STX104_INDEX_POINTER;
	IO_Write_U8( device->io_device, address, index );
	address = device->io_region_offset + STX104_INDEX_DATA;
	IO_Read_U16( device->io_device, address, &u16_lo );
	IO_Read_U16( device->io_device, address, &u16_hi );
	*value = IO_U32_SET_U16_LO_HI( u16_lo, u16_hi );
}
/*****************************************************************************/
void STX104_Index_Read_U32( int board, uint8_t index, uint32_t * value )
{
	devmgr_device_type *	device;
	device  = AUL_Device_Get( board );
	STX104_Index_Device_Read_U32( device, index, value );
}
/*****************************************************************************/
int STX104_Index_Termination( void )
{
	return SUCCESS;
}
/*****************************************************************************/
int STX104_Index_Initialization( void )
{
	return SUCCESS;
}
