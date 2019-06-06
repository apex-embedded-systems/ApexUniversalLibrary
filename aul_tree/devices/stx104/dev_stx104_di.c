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
#define STX104_DI_MODULE_REV "$Date: 2014-05-17 00:20:44 -0500 (Sat, 17 May 2014) $"
/********************************************************************************************************/
const char * STX104_DI_SVN_Time_Stamp( void ) { return STX104_DI_MODULE_REV; }
/**********************************************************************************************************//**
@ingroup stx104_di
\brief
Apex Univeral Library STX104 specific function that retrieves all of the digital input values. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] value Pointer to the UINT8 type variable that will receive the digital input values. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DI_All_Get( int board, uint8_t * value )
{
	int				error_code;
	devmgr_device_type *	device;
	size_t			address;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DI_All_Get_Error;
	address = device->io_region_offset + STX104_DIGITAL_INPUTS;
	IO_Read_U8( device->io_device, address, value );
	return error_code;
STX104_DI_All_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_di
\brief
Apex Univeral Library STX104 specific function that retrieves the digital input (DI) configuration. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] cfg A pointer to the ::STX104_DI_CFG_TYPE structure that will return the digital input configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DI_CFG_Get( int board, STX104_DI_CFG_TYPE * cfg )
{
	int				error_code;
	devmgr_device_type *	device;
	uint8_t			reg_value;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DI_CFG_Get_Error;
	STX104_Index_Read_U8( board, STX104_DIGITAL_INPUT_CONFIGURATION, &reg_value );
   Mem_Set( cfg, 0, sizeof( STX104_DI_CFG_TYPE ) );
	if ( reg_value & 0x80 ) cfg->deglitch_200ns     = true;
	if ( reg_value & 0x40 ) cfg->swap_din1_and_din3 = true;
	if ( reg_value & 0x08 ) cfg->polarity.din3      = true;
	if ( reg_value & 0x04 ) cfg->polarity.din2      = true;
	if ( reg_value & 0x02 ) cfg->polarity.din1      = true;
	if ( reg_value & 0x01 ) cfg->polarity.din0      = true;	
	return error_code;
STX104_DI_CFG_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_di
\brief
Apex Univeral Library STX104 specific function that sets the digital input (DI) configuration. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] cfg A pointer to the ::STX104_DI_CFG_TYPE structure that will set the digital input configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DI_CFG_Set( int board, STX104_DI_CFG_TYPE * cfg )
{
	int				error_code;
	devmgr_device_type *	device;
	uint8_t			reg_value;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DI_CFG_Set_Error;
#if defined( STX104_SPI_H_ )
	/* test to make sure that we are not interferring with active SPI interface */
	error_code = STX104_SPI_DI_Config_Request_Check( board, cfg, "STX104_DI_CFG_Set" );
	if ( error_code ) goto STX104_DI_CFG_Set_Error;
#endif
	reg_value = 0;
	if ( cfg->deglitch_200ns  )    reg_value |=  0x80;
	if ( cfg->swap_din1_and_din3 ) reg_value |=  0x40;
	if ( cfg->polarity.din3  )     reg_value |=  0x08;
	if ( cfg->polarity.din2  )     reg_value |=  0x04;
	if ( cfg->polarity.din1  )     reg_value |=  0x02;
	if ( cfg->polarity.din0  )     reg_value |=  0x01;
	STX104_Index_Write_U8( board, STX104_DIGITAL_INPUT_CONFIGURATION, reg_value );	
	return error_code;
STX104_DI_CFG_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************//**
@ingroup stx104_di
\brief
Apex Univeral Library STX104 specific function that retrieves the specified digital input (DI) channel. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] channel The channel number that the data value will be read from; STX104_DI0 (0) <= channel <= STX104_DI3 (3) (i.e. ::STX104_DI_ENUM); 
\param[out] value Pointer to the variable that will receive the digital input value 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_DI_Get( int board, STX104_DI_ENUM channel, BOOL * value )
{
	int				error_code;
	devmgr_device_type *	device;
	size_t			address;
	uint8_t			data_di;
	uint8_t			mask;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_DI_Get_Error;
	if ( (channel >= STX104_DI0) && (channel <= STX104_DI3) )
	{
		mask = 0x01 << channel;
		address = device->io_region_offset + STX104_DIGITAL_INPUTS;
		IO_Read_U8( device->io_device, address, &data_di );
		if ( data_di & mask ) *value = true;
		else                  *value = false;
	}
	else
	{
		error_code = EC_STX104_CHANNEL_OUT_OF_RANGE;		goto STX104_DI_Get_Error;
	}
	return error_code;
STX104_DI_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}


/**********************************************************************************************************
*/
int STX104_DI_Termination( void )
{
	return SUCCESS;
}
/**********************************************************************************************************
*/
int STX104_DI_Initialization( void )
{
	return SUCCESS;
}
