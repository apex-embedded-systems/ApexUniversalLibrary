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
#define STX104_AO_MODULE_REV "$Date: 2015-01-12 14:07:46 -0600 (Mon, 12 Jan 2015) $"
/********************************************************************************************************/
const char * STX104_AO_SVN_Time_Stamp( void ) { return STX104_AO_MODULE_REV; }
/**********************************************************************************************************//**
@ingroup stx104_ao
\brief
Apex Universal Library STX104 specific function that sets both analog output channels simultaneously (to minimize skewing between the two channels).

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] value_ao_a Unsigned 16-bit integer value to write out to the analog output channel-A [J7.17 (DAC_OUT_1)] 
\param[in] value_ao_b Unsigned 16-bit integer value to write out to the analog output channel-B [J7.16 (DAC_OUT_2)] 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AO_All_Set( int board, uint16_t value_ao_a, uint16_t value_ao_b )
{
	int						error_code;
	devmgr_device_type *	device;
	size_t					address;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AO_All_Set_Error;

	address = device->io_region_offset + STX104_DAC_CHANA; 
	IO_Write_U16( device->io_device, address, value_ao_a );

	address = device->io_region_offset + STX104_DAC_CHANB; 
	IO_Write_U16( device->io_device, address, value_ao_b );

	return error_code;

STX104_AO_All_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************//**
@ingroup stx104_ao
\brief
Apex Universal Library STX104 specific function that sets a single analog output channel.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] channel The channel number that the data value is to be written out to; STX104_AO_A (0) <= channel <= STX104_AO_B (1) (i.e. ::STX104_AO_ENUM). 
\param[in] value Unsigned 16-bit integer value to write out to the analog output channel. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AO_Set( int board, STX104_AO_ENUM channel, uint16_t value )
{
	int				error_code;
	devmgr_device_type *	device;
	size_t			address;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AO_Set_Error;

	/*if ( (channel >= STX104_AO_A) && (channel <= STX104_AO_B) )*/
	if ( channel <= STX104_AO_B )
	{
		address = device->io_region_offset + STX104_DAC_CHANA + channel * 2;
		IO_Write_U16( device->io_device, address, value );
	}
	else
	{
		error_code = EC_STX104_CHANNEL_OUT_OF_RANGE;		goto STX104_AO_Set_Error;
	}
	return error_code;

STX104_AO_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************
*/
int STX104_AO_Termination( void )
{
	return SUCCESS;
}
/**********************************************************************************************************
*/
int STX104_AO_Initialization( void )
{
	return SUCCESS;
}

