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

*********************************************************************************/

#include "dev_stx104.h"
#include "../../shared/status.h"
#include "../../shared/mem.h"
#include "../../shared/error.h"
#include "../../shared/compiler.h"  /* inline */
#include "../../shared/callconv.h"
#include "../../aul/aul_shared.h"
#include "../../io/io.h"
#include "dev_stx104_index.h"

/*******************************************************************/
#define STX104_DO_MODULE_REV "$Date: 2014-06-27 20:40:23 -0500 (Fri, 27 Jun 2014) $"
/********************************************************************************************************/
const char * STX104_DO_SVN_Time_Stamp( void ) { return STX104_DO_MODULE_REV; }
/**********************************************************************************************************//**
@ingroup stx104_do
\brief
Apex Univeral Library STX104 specific function that simultaneously retrieves all of the digital outputs (DO). 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] value Pointer to the variable that will receive the digital input values. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DO_All_Get( int board, uint8_t * value )
{
	int						error_code;
	devmgr_device_type *	device;
	stx104_dataset_type *	ds;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DO_All_Get_Error;
	ds = (stx104_dataset_type *) device->private_data;
	*value = ds->do_reg & 0x0F;	
	return error_code;
	STX104_DO_All_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_do
\brief
Apex Univeral Library STX104 specific function that simultaneously sets all of the digital outputs (DO). 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] value A byte (UINT8) value that will set/clear all digital outputs. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DO_All_Set( int board, uint8_t value )
{
	int						error_code;
	devmgr_device_type *	device;
	stx104_dataset_type *	ds;
	size_t					address;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DO_All_Set_Error;

	ds = (stx104_dataset_type *) device->private_data;

	if ( ds->spi_interface_active )
	{
		error_code = EC_STX104_SPI_DO_SET_COLLISION_ERROR;		
		goto STX104_DO_All_Set_Error;		
	}
	else
	{
		ds->do_reg = 0x0F & value;
		address = device->io_region_offset + STX104_DIGITAL_OUTPUTS;
		IO_Write_U8( device->io_device, address, ds->do_reg );
	}
	return error_code;
STX104_DO_All_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_do
\brief
Apex Univeral Library STX104 specific function that retireves the digital output (DO) configuration. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] cfg A pointer to the ::stx104_do_cfg_type structure that will be used to retrieve the digital output configuration information. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DO_CFG_Get( int board, stx104_do_cfg_type * cfg )
{
	int				error_code;
	devmgr_device_type *	device;
	uint8_t  reg_value_low;
	uint16_t reg_value;
	uint16_t firmware_revision;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DO_CFG_Get_Error;

	Mem_Set( cfg, 0, sizeof( stx104_do_cfg_type ) );
	STX104_Gen_Revision_Get( board, &firmware_revision );
	if ( firmware_revision >= STX104_REVISION_140101 )
	{ /* get the extended digital output configuration */
		STX104_Index_Read_U16( board, STX104_DIGITAL_OUTPUT_CONFIGURATION, &reg_value );
		reg_value_low = (uint8_t) ( reg_value & 0x00FF );
			
		reg_value = reg_value >> 8;
		cfg->do0_map = (STX104_DO0_MAP_ENUM)(reg_value & 0x03 );
		reg_value = reg_value >> 2;
		cfg->do1_map = (STX104_DO1_MAP_ENUM)(reg_value & 0x03 );
		reg_value = reg_value >> 2;
		cfg->do2_map = (STX104_DO2_MAP_ENUM)(reg_value & 0x03 );
		reg_value = reg_value >> 2;
		cfg->do3_map = (STX104_DO3_MAP_ENUM)(reg_value & 0x03 );
	}
	else
	{
		STX104_Index_Read_U8( board, STX104_DIGITAL_OUTPUT_CONFIGURATION, &reg_value_low );
	}
	/* convert lower bits */
	if ( reg_value_low & 0x80 ) cfg->polarity.sshp    = true;
	if ( reg_value_low & 0x20 ) cfg->polarity.ct_out0 = true;
	if ( reg_value_low & 0x10 ) cfg->polarity.ct_out2 = true;
	if ( reg_value_low & 0x08 ) cfg->polarity.dout3   = true;
	if ( reg_value_low & 0x04 ) cfg->polarity.dout2   = true;
	if ( reg_value_low & 0x02 ) cfg->polarity.dout1   = true;
	if ( reg_value_low & 0x01 ) cfg->polarity.dout0   = true;

	return error_code;
STX104_DO_CFG_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_do
\brief
Apex Univeral Library STX104 specific function that sets the digital output (DO) configuration. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] cfg A pointer to the ::stx104_do_cfg_type structure that will be used to set the digital output configuration information. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DO_CFG_Set( int board, stx104_do_cfg_type * cfg )
{
	int				error_code;
	devmgr_device_type *	device;
	uint8_t			reg_value_low;
	uint16_t			reg_value;
	uint16_t			firmware_revision;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DO_CFG_Set_Error;
#if defined( STX104_SPI_H_ )
	/* test to make sure that we are not interferring with active SPI interface */
	error_code = STX104_SPI_DO_Config_Request_Check( board, cfg, "STX104_DO_CFG_Set" );
	if ( error_code ) goto STX104_DI_CFG_Set_Error;
#endif
	reg_value_low = 0;
	if ( cfg->polarity.sshp    ) reg_value_low |=  0x80;
	if ( cfg->polarity.ct_out0 ) reg_value_low |=  0x20;
	if ( cfg->polarity.ct_out2 ) reg_value_low |=  0x10;
	if ( cfg->polarity.dout3   ) reg_value_low |=  0x08;
	if ( cfg->polarity.dout2   ) reg_value_low |=  0x04;
	if ( cfg->polarity.dout1   ) reg_value_low |=  0x02;
	if ( cfg->polarity.dout0   ) reg_value_low |=  0x01;

	STX104_Gen_Revision_Get( board, &firmware_revision );
	if ( firmware_revision >= STX104_REVISION_140101 )
	{ /* update the extended digital output configuration */
		reg_value  = ((uint16_t) cfg->do3_map) & 0x03;
		reg_value  = reg_value << 2;
		reg_value |= ((uint16_t) cfg->do2_map) & 0x03;
		reg_value  = reg_value << 2;
		reg_value |= ((uint16_t) cfg->do1_map) & 0x03;
		reg_value  = reg_value << 2;
		reg_value |= ((uint16_t) cfg->do0_map) & 0x03;
		reg_value  = reg_value << 8;
		reg_value |= ((uint16_t) reg_value_low ) & 0xFF;
		STX104_Index_Write_U16( board, STX104_DIGITAL_OUTPUT_CONFIGURATION, reg_value );
	}
	else
	{
		STX104_Index_Write_U8( board, STX104_DIGITAL_OUTPUT_CONFIGURATION, reg_value_low );
	}			
	return error_code;
STX104_DO_CFG_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************//**
@ingroup stx104_do
\brief
Apex Univeral Library STX104 specific function that retrieves the specified digital output (DO) channel. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] channel  The channel number that the data value will be read from; STX104_DO0 (0) <= channel <= STX104_DO3 (3) (i.e. ::STX104_DO_ENUM). 
\param[out] value Pointer to the variable that will receive the digital output value. This is a copy of the last value written. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DO_Get( int board, STX104_DO_ENUM channel, BOOL * value )
{
	int						error_code;
	devmgr_device_type *	device;
	uint8_t					mask;
	stx104_dataset_type *	ds;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DO_Get_Error;
	ds = (stx104_dataset_type *) device->private_data;
	if ( (channel >= STX104_DO0) && (channel <= STX104_DO3) )
	{
		mask = 0x01 << channel;
		if ( ds->do_reg & mask ) *value = true;
		else                     *value = false;
	}
	else
	{
		error_code = EC_STX104_CHANNEL_OUT_OF_RANGE;		goto STX104_DO_Get_Error;
	}
	return error_code;
STX104_DO_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************//**
@ingroup stx104_do
\brief
Apex Univeral Library STX104 specific function that sets the specified digital output (DO) channel. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] channel  The channel number that the data value will be read from; STX104_DO0 (0) <= channel <= STX104_DO3 (3) (i.e. ::STX104_DO_ENUM).
\param[in] value The digital output value to be written
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DO_Set( int board, STX104_DO_ENUM channel, BOOL value )
{
	int						error_code;
	devmgr_device_type *	device;
	size_t					address;
	uint8_t					mask;
	stx104_dataset_type *	ds;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DO_Set_Error;
	ds = (stx104_dataset_type *) device->private_data;
#if defined( STX104_SPI_H_ )
	/* test to make sure that we are not interferring with active SPI interface */
	error_code = STX104_SPI_DO_Set_Request_Check( board, channel, "STX104_DO_Set" );
	if ( error_code ) goto STX104_DO_Set_Error;
#endif

	if ( (channel >= STX104_DO0) && (channel <= STX104_DO3) )
	{
		mask = 0x01 << channel;

		if ( value == false ) ds->do_reg &= ~mask;
		else                  ds->do_reg |= mask;
		
		address = device->io_region_offset + STX104_DIGITAL_OUTPUTS;
		IO_Write_U8( device->io_device, address, ds->do_reg );
	}
	else
	{
		error_code = EC_STX104_CHANNEL_OUT_OF_RANGE;		goto STX104_DO_Set_Error;
	}
	return error_code;
STX104_DO_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}



/**********************************************************************************************************
*/
int STX104_DO_Termination( void )
{
	return SUCCESS;
}

/**********************************************************************************************************
*/
int STX104_DO_Initialization( void )
{
	return SUCCESS;
}
