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
#define STX104_TRG_MODULE_REV "$Date: 2014-05-17 00:20:44 -0500 (Sat, 17 May 2014) $"
/********************************************************************************************************/
const char * STX104_Trg_SVN_Time_Stamp( void ) { return STX104_TRG_MODULE_REV; }
/**********************************************************************************************************//**
@ingroup stx104_trg
\brief
Apex Univeral Library STX104 specific function that retrieves the trigger active state. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] state Pointer to the variable that will receive the trigger active state. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Trg_Active_Get( int board, BOOL * state ) 
{
	int				error_code;
	devmgr_device_type *	device;
	size_t			address;
	uint8_t			value;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Trg_Active_Get_Error;
	/*! get the ASR.TAS bit */
	address = device->io_region_offset + STX104_ADC_CONFIGURATION;
	IO_Read_U8( device->io_device, address, &value );
	if ( (value & 0x08) == 0x08 ) *state = true;
	else                          *state = false;
	return error_code;
STX104_Trg_Active_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************//**
@ingroup stx104_trg
\brief
Apex Univeral Library STX104 specific function that retrieves the triggering configuration. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] cfg A pointer to the ::STX104_TRG_CFG_TYPE structure that will be used to retrieve the analog input triggering configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Trg_CFG_Get( int board, STX104_TRG_CFG_TYPE * cfg )
{
	int				error_code;
	devmgr_device_type *	device;
	uint16_t			reg_value;
	uint32_t			tsd_reg;
   double			delay_ns;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Trg_CFG_Get_Error;

   Mem_Set( cfg, 0, sizeof( STX104_TRG_CFG_TYPE ) );
	STX104_Index_Read_U32( board, STX104_TRIGGER_START_DELAY,   &tsd_reg );
   delay_ns = ( (double) tsd_reg ) * STX104_CLOCK_RESOLUTION_NS;
	cfg->delay_sec  = delay_ns * 1e-9; /* convert to seconds */
	cfg->delay_ns = (uint32_t) delay_ns;

	STX104_Index_Read_U16( board, STX104_TRIGGER_CONFIGURATION, &reg_value   );
		
	if ( (reg_value & 0x8000) == 0x8000 ) cfg->enabled = true;

	cfg->start_source       = (STX104_TRG_START_ENUM)     (  reg_value & 0x000F        );
	cfg->stop_source        = (STX104_TRG_STOP_ENUM)      ( (reg_value & 0x00F0) >>  4 );
	cfg->start_sync_source  = (STX104_TRG_START_SYNC_ENUM)( (reg_value & 0x0F00) >>  8 );
	cfg->start_mode         = (STX104_TRG_MODE_ENUM)      ( (reg_value & 0x3000) >> 12 );
	return error_code;
STX104_Trg_CFG_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_trg
\brief
Apex Univeral Library STX104 specific function that sets the triggering configuration. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] cfg A pointer to the STX104_TRG_CFG_TYPE structure that will be used to set the analog input triggering configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Trg_CFG_Set( int board, STX104_TRG_CFG_TYPE * cfg )
{
	int				error_code;
	devmgr_device_type *	device;
	uint16_t			reg_value;
	uint32_t			tsd_reg;
   double			delay;
   double			delay_max_sec;
   double			delay_sec;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Trg_CFG_Set_Error;

	if ( cfg->enabled != 0 ) reg_value = 0x8000;
	else                     reg_value = 0x0000;

	reg_value |= (uint16_t)( (cfg->start_mode & 0x03) << 12 );
	reg_value |= (uint16_t)( (cfg->start_sync_source & 0x0F) << 8 );
	reg_value |= (uint16_t)( (cfg->stop_source & 0x0F) << 4 );
	reg_value |= (uint16_t)( (cfg->start_source & 0x0F) );

   delay_max_sec = STX104_CLOCK_RESOLUTION_NS * ((double) UINT32_MAX);
   delay_max_sec = delay_max_sec * 1.0e-9; /* convert to seconds */

   /* delay computation */
   delay_sec = 0.0;
   if ( cfg->delay_ns  > 0 )
   {
   	delay_sec = ((double) cfg->delay_ns ) * 1.0e-9; /* convert to seconds */
   }
   if ( cfg->delay_sec > 0 )
   {
   	delay_sec = cfg->delay_sec;
   }

   /* cap the value */
   if ( delay_sec > delay_max_sec )
   {
      delay = delay_max_sec;
   }
   else
   {
      delay = delay_sec;
   }

   /* convert to nS */
   delay = delay * 1.0e9;
   /* convert to clock ticks */
   delay = delay / STX104_CLOCK_RESOLUTION_NS;
	tsd_reg = (uint32_t) delay;

	STX104_Index_Write_U32( board, STX104_TRIGGER_START_DELAY,   tsd_reg );
	STX104_Index_Write_U16( board, STX104_TRIGGER_CONFIGURATION, reg_value  );

	return error_code;
STX104_Trg_CFG_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_trg
\brief
Apex Univeral Library STX104 specific function that performs a software trigger start operation. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Trg_Software_Start( int board ) /* software trigger start */
{
	int				error_code;
	devmgr_device_type *	device;
	size_t			address;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Trg_Software_Start_Error;
	address = device->io_region_offset + STX104_SOFTWARE_STROBE;
	IO_Write_U8( device->io_device, address, 0x55 );
	return error_code;
STX104_Trg_Software_Start_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_trg
\brief
Apex Univeral Library STX104 specific function that performs a software trigger stop operation. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Trg_Software_Stop( int board ) /* software trigger stop */
{
	int				error_code;
	devmgr_device_type *	device;
	size_t			address;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Trg_Software_Stop_Error;
	address = device->io_region_offset + STX104_SOFTWARE_STROBE;
	IO_Write_U8( device->io_device, address, 0xAA );
	return error_code;
STX104_Trg_Software_Stop_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_trg
\brief
Apex Univeral Library STX104 specific function that performs a software trigger sync operation. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Trg_Software_Sync( int board ) /* software trigger sync */
{
	int				error_code;
	devmgr_device_type *	device;
	size_t			address;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Trg_Software_Sync_Error;
	address = device->io_region_offset + STX104_SOFTWARE_STROBE;
	IO_Write_U8( device->io_device, address, 0x5A );
	return error_code;
STX104_Trg_Software_Sync_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
int STX104_Trg_Termination( void )
{
	return SUCCESS;
}
int STX104_Trg_Initialization( void )
{
	return SUCCESS;
}
