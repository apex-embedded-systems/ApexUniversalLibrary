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
/*#include "../../io/io.h"*/
#include "dev_stx104_index.h"

/*******************************************************************/
#define STX104_SPI_MODULE_REV "$Date: 2014-06-27 20:40:23 -0500 (Fri, 27 Jun 2014) $"
/********************************************************************************************************/
const char * STX104_SPI_SVN_Time_Stamp( void ) { return STX104_SPI_MODULE_REV; }
/**********************************************************************************************************
*/
int STX104_SPI_DI_Config_Request_Check( int board, STX104_DI_CFG_TYPE * di_cfg_request, const char * prefix )
{
	int						error_code;
	devmgr_device_type *			device;
	STX104_DI_CFG_TYPE	di_cfg;  /* current configuration */
	STX104_SPI_CFG_TYPE	spi_cfg;
	stx104_dataset_type *	ds;
	(void)					prefix;
	error_code	= SUCCESS;
	device		= AUL_Device_Get( board );
	ds				= (stx104_dataset_type *) device->private_data;
	if ( !(ds->spi_interface_active) ) return SUCCESS; 
	/* So long as the new request does not affect the digital 
	   input mapped by SDI_MAP we are OK.
	*/
	STX104_DI_CFG_Get( board, &di_cfg );
	if ( di_cfg_request->deglitch_200ns != di_cfg.deglitch_200ns ) 
	{	error_code = EC_STX104_SPI_DI_CFG_COLLISION_ERROR;		goto STX104_SPI_DI_Config_Request_Check_Error;		}
	
	STX104_SPI_CFG_Get( board, &spi_cfg );
	switch( spi_cfg.sdi_map )
	{
		case SPI_SDI_DIN0:
			if ( di_cfg_request->polarity.din0 != di_cfg.polarity.din0 )  error_code = EC_STX104_SPI_DI_CFG_COLLISION_ERROR; 
			break;
		case SPI_SDI_DIN1:
			if ( di_cfg_request->polarity.din1 != di_cfg.polarity.din1				)  error_code = EC_STX104_SPI_DI_CFG_COLLISION_ERROR;
			if ( di_cfg_request->swap_din1_and_din3 != di_cfg.swap_din1_and_din3 )  error_code = EC_STX104_SPI_DI_CFG_COLLISION_ERROR;
			break;
		case SPI_SDI_DIN2:
			if ( di_cfg_request->polarity.din2 != di_cfg.polarity.din2 )  error_code = EC_STX104_SPI_DI_CFG_COLLISION_ERROR;
			break; 
		case SPI_SDI_DIN3:
			if ( di_cfg_request->polarity.din3 != di_cfg.polarity.din3				)  error_code = EC_STX104_SPI_DI_CFG_COLLISION_ERROR;
			if ( di_cfg_request->swap_din1_and_din3 != di_cfg.swap_din1_and_din3 )  error_code = EC_STX104_SPI_DI_CFG_COLLISION_ERROR;
			break;
	}
	if ( error_code ) goto STX104_SPI_DI_Config_Request_Check_Error;
	return SUCCESS;
STX104_SPI_DI_Config_Request_Check_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
int STX104_SPI_DO_Config_Request_Check( int board, stx104_do_cfg_type* do_cfg_request, const char * prefix )
{
	int						error_code;
	devmgr_device_type *			device;
	stx104_do_cfg_type	do_cfg;  /* current configuration */
	stx104_dataset_type *	ds;
	(void)					prefix;
	error_code	= SUCCESS;
	device		= AUL_Device_Get( board );
	ds				= (stx104_dataset_type *) device->private_data;
	if ( !(ds->spi_interface_active) ) return SUCCESS; 
	/* So long as the new request does not affect the digital 
	   outputs used by SPI we are OK.
	*/
	STX104_DO_CFG_Get( board, &do_cfg );

	if ( do_cfg_request->do0_map != do_cfg.do0_map ) error_code = EC_STX104_SPI_DO_CFG_COLLISION_ERROR;
	if ( do_cfg_request->do1_map != do_cfg.do1_map ) error_code = EC_STX104_SPI_DO_CFG_COLLISION_ERROR;
	if ( do_cfg_request->do2_map != do_cfg.do2_map ) error_code = EC_STX104_SPI_DO_CFG_COLLISION_ERROR;

	if ( do_cfg_request->polarity.dout0 != do_cfg.polarity.dout0 ) error_code = EC_STX104_SPI_DO_CFG_COLLISION_ERROR;
	if ( do_cfg_request->polarity.dout1 != do_cfg.polarity.dout1 ) error_code = EC_STX104_SPI_DO_CFG_COLLISION_ERROR;
	if ( do_cfg_request->polarity.dout2 != do_cfg.polarity.dout2 ) error_code = EC_STX104_SPI_DO_CFG_COLLISION_ERROR;

	if ( error_code ) goto STX104_SPI_DO_Config_Request_Check_Error;
	return SUCCESS;
STX104_SPI_DO_Config_Request_Check_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
int STX104_SPI_DO_Set_Request_Check( int board, STX104_DO_ENUM channel, const char * prefix )
{
	int							error_code;
	devmgr_device_type *				device;
	stx104_dataset_type *	ds;
	(void)						prefix;
	device = AUL_Device_Get( board );
	ds     = (stx104_dataset_type *) device->private_data;
	if ( false == ds->spi_interface_active ) return SUCCESS; 

	if ( channel != STX104_DO3 ) /* only digital output not part of SPI */
	{	error_code = EC_STX104_SPI_DO_SET_COLLISION_ERROR;	goto STX104_SPI_DO_Set_Request_Check_Error;	}
	return SUCCESS;
STX104_SPI_DO_Set_Request_Check_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
static int STX104_SPI_Firmware_Revision_Check( int board, const char * prefix )
{
	int		error_code;
	uint16_t firmware_revision;
	(void)	prefix;
	error_code = STX104_Gen_Revision_Get( board, &firmware_revision );
	if ( error_code ) goto STX104_SPI_Firmware_Revision_Check_Error;
#ifndef STX104_REVISION_140101
/*   AUL_Error_Log("%s: Error - firmware does not support SPI interface\n", prefix );*/
	error_code = EC_STX104_SPI_FIRMWARE_ERROR;
	goto STX104_SPI_Firmware_Revision_Check_Error;
#else
	if ( firmware_revision < STX104_REVISION_140101 )
	{	error_code = EC_STX104_SPI_FIRMWARE_ERROR;	goto STX104_SPI_Firmware_Revision_Check_Error;	}
#endif
	return SUCCESS;
STX104_SPI_Firmware_Revision_Check_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
static int STX104_SPI_Active_Check( int board, const char * prefix )
{
	int							error_code;
	devmgr_device_type *				device;
	stx104_dataset_type *	ds;
	(void)						prefix;
	device = AUL_Device_Get( board );
	ds = (stx104_dataset_type *) device->private_data;
	if ( false == ds->spi_interface_active )
	{	error_code = EC_STX104_SPI_INACTIVE_ERROR;	goto STX104_SPI_Active_Check_Error;	}
	return SUCCESS;
STX104_SPI_Active_Check_Error:
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_CFG_Set( int board, STX104_SPI_CFG_TYPE * cfg )
{
	int 				error_code;
	devmgr_device_type *	device;
	uint16_t       reg_uint16;
	double 			time_max_ns;
	double			time_half_ns;
	double			time_ecd_ns;	
	/*                                                 SCHI[11:0]   */
	time_max_ns  = STX104_CLOCK_RESOLUTION_NS * ( 4.0 + 4095.0 );
	time_half_ns = ((double) cfg->half_clock_ns );
	if ( time_half_ns > time_max_ns ) 
	{	/*AUL_Error_Log("STX104_SPI_CFG_Set: Error - half clock time too long\n" );*/
		return EC_STX104_SPI_HALF_CLOCK_ERROR;
	}
	/*                                                half_clock    ECD[3:0] */
	time_max_ns  = STX104_CLOCK_RESOLUTION_NS * 4.0 + time_max_ns * 15.0;
	time_ecd_ns = ((double) cfg->end_cycle_delay_ns );
	if ( time_ecd_ns > time_max_ns ) 
	{	error_code = EC_STX104_SPI_END_OF_CYCLE_DELAY_ERROR;	goto STX104_SPI_CFG_Set_Error;	}	
	if ( ( cfg->sdi_map < SPI_SDI_DIN0 ) || ( cfg->sdi_map > SPI_SDI_DIN3 ) )
	{	error_code = EC_STX104_SPI_SDI_MAP_ERROR;					goto STX104_SPI_CFG_Set_Error;	}
	if ( ( cfg->chip_select_behavior < SPI_CSB_SOFTWARE ) || ( cfg->chip_select_behavior > SPI_CSB_TOGGLE_16 ) )
	{	error_code = EC_STX104_SPI_CSB_ERROR;						goto STX104_SPI_CFG_Set_Error;	}

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_CFG_Set_Error;	
	error_code = STX104_SPI_Firmware_Revision_Check( board, "STX104_SPI_CFG_Set" );
	if ( error_code ) goto STX104_SPI_CFG_Set_Error;
	/* half clock */
	time_half_ns = ((double) cfg->half_clock_ns );
	time_half_ns = ( time_half_ns / STX104_CLOCK_RESOLUTION_NS ) - 4.0;
	reg_uint16   = (uint16_t) time_half_ns;
	STX104_Index_Write_U16( board, STX104_SPI_HALF_CLOCK, reg_uint16 );
	time_half_ns = ((double) cfg->half_clock_ns );
	/* sdi_map */
	reg_uint16 = ( (uint16_t) cfg->sdi_map ) & 0x0003; 
	/* compute End of Cycle Delay (ECD) */
	time_ecd_ns = ((double) cfg->end_cycle_delay_ns );
	time_ecd_ns = ( time_ecd_ns - ( STX104_CLOCK_RESOLUTION_NS * 4.0 ) ) / time_half_ns;
	reg_uint16  = reg_uint16 << 4;	
	reg_uint16 |= ( ((uint16_t) time_ecd_ns ) & 0x000F );

	reg_uint16   = reg_uint16 << 1;
	if ( cfg->sdio_wrap ) reg_uint16 |= 0x0001;

	reg_uint16  = reg_uint16 << 3;
	reg_uint16 |= ( (uint16_t) cfg->chip_select_behavior ) & 0x0003; 
		
	reg_uint16  = reg_uint16 << 1;
	if ( cfg->sdo_polarity ) reg_uint16 |= 0x0001;		
		
	reg_uint16  = reg_uint16 << 1;
	if ( cfg->sdi_polarity ) reg_uint16 |= 0x0001;			
		
	reg_uint16  = reg_uint16 << 1;
	if ( cfg->sclk_phase ) reg_uint16 |= 0x0001;	
			
	reg_uint16  = reg_uint16 << 1;
	if ( cfg->sclk_polarity ) reg_uint16 |= 0x0001;
			
	STX104_Index_Write_U16( board, STX104_SPI_CONFIGURATION, reg_uint16 );					
	return SUCCESS;
STX104_SPI_CFG_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_CFG_Get( int board, STX104_SPI_CFG_TYPE * cfg )
{
	int 				error_code;
	devmgr_device_type *	device;
	uint16_t       reg_uint16;
	double 			time_ns;
	
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_CFG_Get_Error;	

	error_code = STX104_SPI_Firmware_Revision_Check( board, "STX104_SPI_CFG_Get" );
	if ( error_code ) goto STX104_SPI_CFG_Get_Error;

	Mem_Set( cfg, 0, sizeof( STX104_SPI_CFG_TYPE ) );		
	/* SPI Configuration Register */
	STX104_Index_Read_U16( board, STX104_SPI_CONFIGURATION, &reg_uint16 );
		
	cfg->sdi_map = (STX104_SPI_SDI_ENUM) ( ( reg_uint16 & 0x3000 ) >> 12 );
		
	/* NOTE: actual time in ns is calculated down at the bottom of this function */
	cfg->end_cycle_delay_ns = (uint32_t) ( ( reg_uint16 & 0x0F00 ) >> 8 );
		
	if ( ( reg_uint16 & 0x0080 ) == 0x0080  ) cfg->sdio_wrap = true;
		
	cfg->chip_select_behavior = (STX104_SPI_CSB_ENUM) ( ( reg_uint16 & 0x0030 ) >> 4 );
		
	if ( ( reg_uint16 & 0x0008  ) == 0x0008  ) cfg->sdo_polarity = true;
		
	if ( ( reg_uint16 & 0x0004  ) == 0x0004  ) cfg->sdi_polarity = true;
			
	if ( ( reg_uint16 & 0x0002  ) == 0x0002  ) cfg->sclk_phase = true;
			
	if ( ( reg_uint16 & 0x0001  ) == 0x0001  ) cfg->sclk_polarity = true;
			
	/* half clock */
	STX104_Index_Read_U16( board, STX104_SPI_HALF_CLOCK, &reg_uint16 );
	reg_uint16 = reg_uint16 & 0x0FFF; /* mask so we only get lower 12 bits */
	time_ns = STX104_CLOCK_RESOLUTION_NS * ( (double)( reg_uint16 + 4 ) );
	cfg->half_clock_ns = (uint16_t) time_ns;	
	/*                                             half-clock                                     */
	time_ns = ((double) cfg->end_cycle_delay_ns ) * time_ns  + STX104_CLOCK_RESOLUTION_NS * 4.0;
	cfg->end_cycle_delay_ns = (uint32_t) time_ns;
	return SUCCESS;
STX104_SPI_CFG_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;	
}
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Active_Set( int board, BOOL  active_state )
{
	int 							error_code;
	devmgr_device_type *				device;
	stx104_dataset_type *	ds;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_Active_Set_Error;
	error_code = STX104_SPI_Firmware_Revision_Check( board, "STX104_SPI_Active_Set" );
	if ( error_code ) goto STX104_SPI_Active_Set_Error;
	ds = (stx104_dataset_type *) device->private_data;
	ds->spi_interface_active = active_state;	
	return SUCCESS;
STX104_SPI_Active_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Active_Get( int board, BOOL* active_state )
{
	int 							error_code;
	devmgr_device_type *				device;
	stx104_dataset_type *	ds;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_Active_Get_Error;
	error_code = STX104_SPI_Firmware_Revision_Check( board, "STX104_SPI_Active_Get" );
	if ( error_code ) goto STX104_SPI_Active_Get_Error;
	ds = (stx104_dataset_type *) device->private_data;
	*active_state = ds->spi_interface_active;
	return SUCCESS;
STX104_SPI_Active_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Chip_Select_Commit_Set( int board, BOOL value )
{
	int 							error_code;
	devmgr_device_type *				device;
	uint8_t						bit_value;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_Chip_Select_Commit_Set_Error;

	error_code = STX104_SPI_Active_Check( board, "STX104_SPI_Chip_Select_Commit_Set" );
	if ( error_code ) goto STX104_SPI_Chip_Select_Commit_Set_Error;

	if ( true == value ) bit_value = 1;
	else                 bit_value = 0;		
	STX104_Index_Write_U8( board, STX104_SPI_SOFTWARE_CHIP_SELECT, bit_value );
	return SUCCESS;
STX104_SPI_Chip_Select_Commit_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Chip_Select_Commit_Get( int board, BOOL* value )
{
	int 				error_code;
	devmgr_device_type *	device;
	uint8_t			bit_value;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_Chip_Select_Commit_Get_Error;
	error_code = STX104_SPI_Active_Check( board, "STX104_SPI_Chip_Select_Commit_Get" );
	if ( error_code ) goto STX104_SPI_Chip_Select_Commit_Get_Error;

	STX104_Index_Read_U8( board, STX104_SPI_SOFTWARE_CHIP_SELECT, &bit_value );
	if ( bit_value & 0x01 ) *value = true;
	else                    *value = false;	
	return SUCCESS;
STX104_SPI_Chip_Select_Commit_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Status_Get( int board, uint8_t* value )
{
	int 				error_code;
	devmgr_device_type *	device;
	uint8_t			status_register_value;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_Status_Get_Error;
	error_code = STX104_SPI_Active_Check( board, "STX104_SPI_Status_Get" );
	if ( error_code ) goto STX104_SPI_Status_Get_Error;
	STX104_Index_Read_U8( board, STX104_SPI_SOFTWARE_CHIP_SELECT, &status_register_value );	
	*value = status_register_value;
	return SUCCESS;
STX104_SPI_Status_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
#define STX104_SPI_TX_FULL      0x08
#define STX104_SPI_TX_EMPTY     0x04
#define STX104_SPI_RX_FULL      0x80
#define STX104_SPI_RX_EMPTY     0x40
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Transmit(	int			board,
																	uint8_t *	tx_buffer,
																	uint8_t		tx_count,
																	uint8_t *	tx_count_actual
										                 )
{
	int 				error_code;
	devmgr_device_type *	device;
	uint8_t			tx_index;
	uint8_t			tx_index_max;
	BOOL			not_done;
/*	BOOL			rx_not_empty;*/
	uint8_t			status;
	uint8_t  			tx_byte;
/*	size_t			index;*/
/*	uint8_t			buffer[STX104_SPI_BUFFER_LENGTH_MAX];*/
	(void)			tx_count_actual;
	tx_index = 0;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_Transmit_Error;
	error_code = STX104_SPI_Active_Check( board, "STX104_SPI_Transceiver" );
	if ( error_code ) goto STX104_SPI_Transmit_Error;

	if ( 0 == tx_count ) return( error_code );

	STX104_SPI_Chip_Select_Commit_Set( board, false ); /* remove commit */

	/* prefill the TX buffer */
	not_done = true; 
	tx_index = 0;
	tx_index_max = tx_count - 1;
	while ( not_done )
	{
		STX104_Index_Read_U8( board, STX104_SPI_STATUS, &status );

		if ( !( status & STX104_SPI_TX_FULL ) )
		{
			tx_byte = tx_buffer[tx_index];
			STX104_Index_Write_U8( board, STX104_SPI_TRANSMIT_FIFO_DATA, tx_byte );
			if ( tx_index < tx_index_max )  tx_index++;
			else                            not_done = false;  /* done writing to buffer */
		}
		else
		{
/*			AUL_Error_Log("STX104_SPI_Transceiver: Error - TX Full Error.\n" );*/
			error_code = EC_STX104_SPI_TX_FULL;
			not_done	  = false;
			break;
		}
	}

	if ( error_code ) goto STX104_SPI_Transmit_Error;

	STX104_SPI_Chip_Select_Commit_Set( board, true ); /* commit the TX buffer */
	return SUCCESS;
STX104_SPI_Transmit_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Receive(	int			board,
																uint8_t *	rx_buffer,
																uint8_t		rx_count_max,
																uint8_t *	rx_count_actual
															 )
{
	int				error_code;
	devmgr_device_type *	device;

	uint8_t			rx_index;
	uint8_t			rx_byte;
	uint8_t			status;
	(void)			rx_count_max;
	
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_Receive_Error;
	rx_index = 0;

	error_code = STX104_SPI_Active_Check( board, "STX104_SPI_Receive_Buffer_Get" );
	if ( error_code ) goto STX104_SPI_Receive_Error;

	while ( rx_index < STX104_SPI_BUFFER_LENGTH_MAX ) 
	{
		STX104_Index_Read_U8( board, STX104_SPI_STATUS, &status );	

		if ( status & STX104_SPI_RX_EMPTY ) 
		{
			break; /* done, buffer is now empty */
		}
		else
		{
			STX104_Index_Read_U8( board, STX104_SPI_RECEIVE_FIFO_DATA, &rx_byte );
			rx_buffer[rx_index] = rx_byte;
			rx_index++;
		}
	} /* while */

	*rx_count_actual = rx_index;
	return SUCCESS;
STX104_SPI_Receive_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Transceiver_Blocking(  int			board,
																				  uint8_t * tx_buffer,
																				  uint8_t   tx_count,
																				  uint8_t *	tx_count_actual,
																				  uint8_t * rx_buffer,
																				  uint8_t   rx_count_max,
																				  uint8_t * rx_count_actual,
																				  uint8_t * tx_retry_count
																				)
{
	int				error_code;
	devmgr_device_type *	device;

	uint8_t			tx_index;
	uint8_t			tx_index_max;
	uint8_t			rx_index;
	BOOL			not_done;
	BOOL			rx_not_empty;
	uint8_t			status;
	uint8_t  		tx_byte;
	uint8_t			rx_byte;
	size_t			index;
	uint8_t			buffer[STX104_SPI_BUFFER_LENGTH_MAX];
	(void)			rx_count_max;
	
	error_code = SUCCESS;
	rx_index = 0;
	tx_index = 0;
	*tx_retry_count = 0;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_SPI_Transceiver_Blocking_Error;

	error_code = STX104_SPI_Active_Check( board, "STX104_SPI_Buffer_Out_And_Commit_Blocking" );
	if ( error_code ) goto STX104_SPI_Transceiver_Blocking_Error;

	/* resynchronize receive buffer, dumping any existing data in the buffer */
	error_code = STX104_SPI_Receive( board, buffer, STX104_SPI_BUFFER_LENGTH_MAX, &rx_index /* actual count, well we don't care what it is */ );
	if ( error_code ) goto STX104_SPI_Transceiver_Blocking_Error;
	rx_index = 0;

	if ( 0 == tx_count ) return SUCCESS;

	STX104_SPI_Chip_Select_Commit_Set( board, false ); /* remove commit */

	/* prefill the TX buffer */
	not_done = true; 
	tx_index = 0;
	tx_index_max = tx_count - 1;
	while ( not_done )
	{
		STX104_Index_Read_U8( board, STX104_SPI_STATUS, &status );
		/*Sleep(20);*/

/*TODO: what if the buffers are full, either TX or RX.  How do we deal with that.  Why is the TX buffer full.  status = 0x98 !!!*/

		if ( !( status & STX104_SPI_TX_FULL ) )
		{
			tx_byte = tx_buffer[tx_index];
			STX104_Index_Write_U8( board, STX104_SPI_TRANSMIT_FIFO_DATA, tx_byte );
			if ( tx_index < tx_index_max )  tx_index++;
			else                            not_done = false;  /* done writing to buffer */
		}
		else
		{	/*AUL_Error_Log("STX104_SPI_Buffer_Out_And_Commit_Blocking: Error - TX Full Error.\n" );*/
			error_code = EC_STX104_SPI_TX_FULL;
			not_done   = false;
		}
	}
		
	if ( error_code ) goto STX104_SPI_Transceiver_Blocking_Error;
	/* Post read out the receive buffer.
		Expecting at least the same number of bytes back as we transmitted. 
	*/
/*TODO: if transmit buffer is not empty continuously send the commit, if commit
       is sent 10 times and 100mS goes and transmit buffer still not empty, then error out.*/
	STX104_SPI_Chip_Select_Commit_Set( board, true ); /* commit the TX buffer */
	/*Sleep(20);*/
	index = 0;
	while( index < 20 )
	{
		STX104_Index_Read_U8( board, STX104_SPI_STATUS, &status );
		/*Sleep(20);*/
		if ( status & STX104_SPI_TX_EMPTY )
		{
			break;
		}
		else
		{
			STX104_SPI_Chip_Select_Commit_Set( board, false ); 
			/*Sleep(20);*/
			STX104_SPI_Chip_Select_Commit_Set( board, true ); /* commit the TX buffer */
			/*Sleep(20);*/
			*tx_retry_count = *tx_retry_count + 1;
		}
		index++;
	}
	if ( 20 == index ) not_done     = false;
	else               not_done     = true;
	/*not_done = true;*/
/* TODO:
TODO: what if the transmit buffer is empty or does not transmit, we will be stuck in an infinite loop!
*/
	/*rx_index   = 0;*/
	status       = 0;
	rx_not_empty = 0;
	/*not_done   = true;*/
	while ( not_done )
	{
		STX104_Index_Read_U8( board, STX104_SPI_STATUS, &status );	
		/*Sleep(20);*/
		if ( status & STX104_SPI_RX_FULL ) 
		{	/*AUL_Error_Log("STX104_SPI_Buffer_Out_And_Commit_Blocking: Error - RX Full Error.\n" );*/
			error_code = EC_STX104_SPI_RX_FULL;		goto STX104_SPI_Transceiver_Blocking_Error;
		}
		rx_not_empty = !(status & STX104_SPI_RX_EMPTY);
		if ( rx_not_empty )
		{
			STX104_Index_Read_U8( board, STX104_SPI_RECEIVE_FIFO_DATA, &rx_byte );
			rx_buffer[rx_index] = rx_byte;
			rx_index++;
		}
		if ( rx_index < tx_count )
		{ 
			/* still need more bytes */
		}
		else if ( rx_not_empty )
		{
			/* read out bytes until empty (which means more bytes than transmit bytes) */
		}
		else
		{ /* now we are done */
			not_done = false;
		}
	} /* while */
	
	*tx_count_actual = tx_index;
	*rx_count_actual = rx_index;
	return SUCCESS;
STX104_SPI_Transceiver_Blocking_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************
*/
int STX104_SPI_Termination( void )
{
	return SUCCESS;
}
/**********************************************************************************************************
*/
int STX104_SPI_Initialization( void )
{
	return SUCCESS;
}
