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
#define STX104_GEN_MODULE_REV "$Date: 2015-07-31 21:59:26 -0500 (Fri, 31 Jul 2015) $"
/********************************************************************************************************/
const char * STX104_Gen_SVN_Time_Stamp( void ) { return STX104_GEN_MODULE_REV; }
/**********************************************************************************************************//**
@ingroup stx104_general
\brief
Apex Universal Library STX104 specific function that retrieves the states of the jumpers.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] jumpers A pointer to the ::STX104_GEN_JMP_TYPE structure that will be used to retrieve the jumper settings. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Gen_Jumpers_Get( int board, STX104_GEN_JMP_TYPE * jumpers_status )
{
	int				error_code;
	devmgr_device_type *	device;
	size_t			address;
	uint8_t			gen_reg;
	uint8_t			adc_stat_reg;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Gen_Revision_Get_Error;

	Mem_Set( jumpers_status, 0, sizeof( STX104_GEN_JMP_TYPE ) );

	STX104_Index_Read_U8( board, STX104_GENERAL_CONFIGURATION, &gen_reg );
	if ( ( gen_reg & 0x01 ) == 0x01 ) jumpers_status->M0_mode                   = true;
	if ( ( gen_reg & 0x02 ) == 0x02 ) jumpers_status->M1_mode                   = true;
	if ( ( gen_reg & 0x04 ) == 0x04 ) jumpers_status->M2_dacs_16bits            = true;
	if ( ( gen_reg & 0x08 ) == 0x08 ) jumpers_status->M3_moving_average_filter  = true;
	if ( ( gen_reg & 0x10 ) == 0x10 ) jumpers_status->M4_address_10bit_mode     = true;
	if ( ( gen_reg & 0x20 ) == 0x20 ) jumpers_status->DMA_ch3                   = true;
	if ( ( gen_reg & 0x40 ) == 0x40 ) jumpers_status->CLK_1mhz                  = true;

	address = device->io_region_offset + STX104_ADC_STATUS;
	IO_Read_U8( device->io_device, address, &adc_stat_reg );
	if ( ( adc_stat_reg & 0x40 ) == 0x40 ) jumpers_status->J9_ai_unipolar     = true;
	if ( ( adc_stat_reg & 0x20 ) == 0x20 ) jumpers_status->J8_ai_differential = true;

	return error_code;
STX104_Gen_Revision_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************/
int STX104_Gen_Device_Revision( devmgr_device_type * device, uint16_t * firmware_revision )
{
	uint16_t	value;

	*firmware_revision = 0x0000;
	value = 0x55AA;
	STX104_Bank_Device_Set( device, 1 );
	STX104_Index_Device_Write_U16( device, STX104_SCRATCH_PAD, value );
	STX104_Index_Device_Read_U16( device, STX104_SCRATCH_PAD, &value  );
	if ( value == 0x55AA ) STX104_Index_Device_Read_U16( device, STX104_BOARD_ID, firmware_revision  );
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup stx104_general
\brief
Apex Universal Library STX104 specific function that retrieves the STX104 board firmware revision.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] id_value Returns a UINT16 value that indicates the board firmware revision. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Gen_Revision_Get( int board, uint16_t * id_value )
{
	int						error_code;
	devmgr_device_type *	device;
	//uint16_t				value;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Gen_Revision_Get_Error;

	STX104_Gen_Device_Revision( device, id_value );

//	*id_value = 0x0000;
//	value = 0x55AA;
//	STX104_Bank_Set( board, 1 );
//	STX104_Index_Write_U16( board, STX104_SCRATCH_PAD, value );
//	STX104_Index_Read_U16( board, STX104_SCRATCH_PAD, &value  );
//	if ( value == 0x55AA ) STX104_Index_Read_U16( board, STX104_BOARD_ID, id_value  );

	return error_code;
STX104_Gen_Revision_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
/**********************************************************************************************************
*/
int STX104_Gen_Termination( void )
{
	return SUCCESS;
}
/**********************************************************************************************************
*/
int STX104_Gen_Initialization( void )
{
	return SUCCESS;
}

