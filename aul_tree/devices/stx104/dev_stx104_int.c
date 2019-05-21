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

1. Must verify interupt timing based on source selected (at least for analog inputs).
	if timing exceeds 5KHz, it should error out and request that these values be fixed.
	read aift and aibt and compute potential sample rate.
		STX104_INT_EXCEEDS_5KHZ


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

/**********************************************************************************************************
*/
enum
{
	STX104_INTR_XFR_STATUS_TRANSFER	= 0,
	STX104_INTR_XFR_CLEAR				= 1,
	STX104_INTR_XFR_FIFO_STATUS_MSB	= 2,
	STX104_INTR_XFR_FIFO_STATUS_LSB	= 3,
};
enum { STX104_INTR_XFR_COUNT = 4 };

enum
{
	STX104_INTR_MASK_NONE	= 0x00,
	STX104_INTR_MASK_SRC_A	= 0x01,
	STX104_INTR_MASK_SRC_B	= 0x02,
	STX104_INTR_MASK_SRC_C	= 0x04,
	STX104_INTR_MASK_SRC_D	= 0x08
};


/*******************************************************************/
#define STX104_INTR_MODULE_REV "$Date: 2014-05-17 00:20:44 -0500 (Sat, 17 May 2014) $"
/********************************************************************************************************/
const char * STX104_Intr_SVN_Time_Stamp( void ) { return STX104_INTR_MODULE_REV; }
/*******************************************************************/

#if(0)
/**********************************************************************************************************
*/
void STX104_Int_Enhanced_Registers_Set( int board, BOOL enable )
{
	int							error_code;
	devmgr_device_type *				device;
	uint16_t						reg_value;
	stx104_dataset_type *	ds;

	device = AUL_Device_Get( board );
	ds     = (stx104_dataset_type *) device->private_data;

	STX104_Index_Read_U16( board, STX104_INTERRUPT_CONFIGURATION, &reg_value );
	if ( enable == false )
	{
		reg_value &= 0x7FFF; /* ICFG.EIS     = 0 */
		reg_value &= 0xBFFF; /* ICFG.ISSBE   = 0 */
		reg_value &= 0xDFFF; /* ICFG.nINT_FF = 0 */
		//device_allocation[board].reg_store[STX104_REG_STORE_INT_ENHANCE].byte[0] = 0;
		ds->intr_enhance = 0;
	}
	else
	{
		reg_value |= 0x8000; /* ICFG.EIS     = 1 */
		reg_value |= 0x4000; /* ICFG.ISSBE   = 1 */
		reg_value |= 0x2000; /* ICFG.nINT_FF = 1 */
		//device_allocation[board].reg_store[STX104_REG_STORE_INT_ENHANCE].byte[0] = 1;
		ds->intr_enhance = 1;
	}
	STX104_Index_Write_U16( board, STX104_INTERRUPT_CONFIGURATION, reg_value );
}
#endif
/**********************************************************************************************************

static BOOL STX104_Int_IsEnabled(WDC_DEVICE_HANDLE hDev)
{
    if ( !IsValidDevice((PWDC_DEVICE)hDev, "STX104_Int_IsEnabled") )
        return FALSE;

    return WDC_IntIsEnabled(hDev);
}*/

/**********************************************************************************************************
*/
#if defined( _WIN32 )
static void __stdcall STX104_Int_Handler( void * private_data )
#else
static void AULCALLCONV STX104_Int_Handler( void * private_data )
#endif
{
/*	int								error_code;*/
	devmgr_device_type *					device;
	stx104_intr_params_type *	ds;
	uint8_t							status;
	io_xfr_type	*					xfr;
	size_t							counter; /* fail-safe exit strategy */
	STX104_INT_RESULT_TYPE		task_dataset;

	ds     = (stx104_intr_params_type *) private_data;
	device = ds->device;
	xfr    = ds->list_status;
	status = xfr[STX104_INTR_XFR_STATUS_TRANSFER].data.u8;

	/* mask any interrupts not active/enabled */
	status = status & ds->interrupt_list_active;

	counter = 3;
	while ( status ) /* status can only be those interrupts that are indeed active! */
	{
		if      ( status & STX104_INTR_MASK_SRC_A ) /* ISSA */
		{
			Mem_Set( &task_dataset, 0, sizeof( task_dataset ) );
			task_dataset.user_defined_dataset = ds->interrupt_dataset_a;
			task_dataset.board					 = ds->board;
			( * ds->interrupt_handler_a )( &task_dataset );
			status &= ~STX104_INTR_MASK_SRC_A; /* mark as done */
		}
		else if ( status & STX104_INTR_MASK_SRC_B ) /* ISSB */
		{
			Mem_Set( &task_dataset, 0, sizeof( task_dataset ) );
			task_dataset.user_defined_dataset = ds->interrupt_dataset_b;
			task_dataset.board					 = ds->board;
			( * ds->interrupt_handler_b )( &task_dataset );
			status &= ~STX104_INTR_MASK_SRC_B; /* mark as done */
		}
		else if ( status & STX104_INTR_MASK_SRC_C ) /* FIFO */
		{
			Mem_Set( &task_dataset, 0, sizeof( task_dataset ) );
			task_dataset.user_defined_dataset = ds->interrupt_dataset_c;
			task_dataset.board					 = ds->board;
			( * ds->interrupt_handler_c )( &task_dataset );
			status &= ~STX104_INTR_MASK_SRC_C; /* mark as done */
		}
		/* fail-safe exit strategy */
		counter--;
		if ( 0 == counter ) status = 0;
	}
}
/**********************************************************************************************************
*/
static int STX104_Int_Enable( int board )
{
	int							error_code;
	devmgr_device_type *				device;
	stx104_dataset_type *	ds;
	size_t						interrupt_list_active;
	size_t						xfr_count;
	io_xfr_type					xfr[STX104_INTR_XFR_COUNT];

	device = AUL_Device_Get( board );
	
	if ( device->io_irq_active ) 
	{	error_code = EC_STX104_CFG_INTERRUPT_ACTIVE;		goto STX104_Int_Enable_Error;	}	
	
	ds		 = (stx104_dataset_type *) device->private_data;
	interrupt_list_active = STX104_INTR_MASK_NONE;
	if ( NULL != ds->intr_params.interrupt_handler_a ) interrupt_list_active |= STX104_INTR_MASK_SRC_A;
	if ( NULL != ds->intr_params.interrupt_handler_b ) interrupt_list_active |= STX104_INTR_MASK_SRC_B;
	if ( NULL != ds->intr_params.interrupt_handler_c ) interrupt_list_active |= STX104_INTR_MASK_SRC_C;
	if ( NULL != ds->intr_params.interrupt_handler_d ) interrupt_list_active |= STX104_INTR_MASK_SRC_D;
	
	/* build transfer list that will be used by the interrupt handler */
	Mem_Set( xfr, 0, sizeof( io_xfr_type ) * STX104_INTR_XFR_COUNT );
	xfr[STX104_INTR_XFR_STATUS_TRANSFER].command = IO_XFR_READ_U8;
	xfr[STX104_INTR_XFR_STATUS_TRANSFER].offset  = STX104_ADC_STATUS + device->io_region_offset;
	xfr[STX104_INTR_XFR_CLEAR].command			 = IO_XFR_WRITE_U8;
	xfr[STX104_INTR_XFR_CLEAR].offset			 = STX104_CLEAR_INTERRUPTS + device->io_region_offset;
	xfr_count = 2;
	if ( NULL != ds->intr_params.interrupt_handler_c )  /* FIFO */
	{
		xfr[STX104_INTR_XFR_FIFO_STATUS_MSB].command = IO_XFR_READ_U8;
		xfr[STX104_INTR_XFR_FIFO_STATUS_MSB].offset  = STX104_FIFO_FLAGS + device->io_region_offset;
		xfr[STX104_INTR_XFR_FIFO_STATUS_LSB].command = IO_XFR_READ_U8;
		xfr[STX104_INTR_XFR_FIFO_STATUS_LSB].offset  = STX104_FIFO_DATA_STATUS + device->io_region_offset;
		xfr_count = 4;
	}
	
/*TODO: pass xfr down to interrupt enable.  Need to pass:  xfr_count, xfr*/

	/* make this a single atomic operation, just in case we need to move this to after interrupt enable */
	ds->intr_params.interrupt_list_active = interrupt_list_active;
	error_code = IO_Interrupt_Enable(	device->io_device, 
/*TODO:*/
													NULL, /* interrupt handler */ 
													NULL  /* private data */
											  );
	return error_code;
STX104_Int_Enable_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;

#if(0)
	DWORD dw_status;
	int error_code;
    PWDC_DEVICE device_pointer;
	WDC_DEVICE_HANDLE device_handle;
    AUL_DEVICE_CONTEXT_PTR device_context_pointer;
	WD_TRANSFER* trans_ptr;
	DWORD trans_count;
	WDC_ADDR_DESC* address_desc_ptr;
	uint8_t adc_control_register;

	AUL_Trace_Log("STX104_Int_Enable entered. Device handle: %d\n", board );

	device_handle  = device_allocation[board].device_handle;
	device_pointer = (PWDC_DEVICE) device_handle;

	if ( device_allocation[board].interrupt_active ) 
	{
		return( STX104_EC_DEVICE_CFG_UNAVAILABLE_INTERRUPT_ACTIVE );
	}

	error_code = SUCCESS;
	if ( (board >= 0) && (board <= AUL_DEVICE_ALLOCATION_QUANTITY) && (device_handle != NULL) )
	{		
		/* test validity of board pointers, just to be sure */
		if ( !IsValidDevice(device_pointer, "STX104_Int_Enable") )
		{
			return( STX104_EC_BOARD_NOT_FOUND );
		}
		/* test to see that we have a valid IRQ that we can actually enable interrupts */
/*TEMP		if ( device_allocation[board].device_info.Item[2].I.Int.dwInterrupt == AUL_IRQ_NONE )*/
		if ( device_allocation[board].device_info.Item[0].I.Int.dwInterrupt == AUL_IRQ_NONE )
		{
			AUL_Error_Log( "Int_Enable: Error - IRQ is zero or invalid, see hardware configuration file.\n" );
			return( STX104_EC_INT_IRQ_SET_TO_NONE );	
		}

		/* Check if interrupts are already enabled - kernel perspective */
		if ( WDC_IntIsEnabled(device_handle) )
		{
			return( STX104_EC_INT_INTERRUPTS_ALREADY_ENABLED );
		}

		device_context_pointer = (AUL_DEVICE_CONTEXT_PTR) WDC_GetDevContext( device_pointer );

		if ( device_context_pointer->interrupt_handler_c != NULL ) /* fifo flag */
		{
			trans_count = 4;	
		}
		else
		{
			trans_count = 2;
		}

		trans_ptr = (WD_TRANSFER*) calloc( trans_count, sizeof(WD_TRANSFER) );
/*trans_ptr = (WD_TRANSFER*) calloc( 5, sizeof(WD_TRANSFER) );*/

		if ( !trans_ptr )
		{
			AUL_Error_Log( "Int_Enable: Error - Cannot allocate memory.\n" );
			return( STX104_EC_INT_MEMORY_ERROR );
		}

		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_CHANNEL, device_allocation[board].reg_store[STX104_REG_STORE_AI_CHANNEL_REG].byte[0] );
		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_CLEAR_INTERRUPTS, 0x00 );

/*

2013		WDC_ReadAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, &adc_control_register );
2013		adc_control_register &= 0x0F;
2013		adc_control_register |= ( (device_allocation[board].device_info.Item[0].I.Int.dwInterrupt & 0x0F) << 4 );
2013		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, adc_control_register );
2013		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_CLEAR_INTERRUPTS, 0x00 );
*/

WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, 0 );



{
uint8_t scratch;

WDC_ReadAddr8(device_allocation[board].device_handle, 0, STX104_ADC_STATUS, &scratch );
printf("a: STX104_ADC_STATUS =0x%02X\n",  scratch );
WDC_ReadAddr8(device_allocation[board].device_handle, 0, STX104_ADC_CONTROL, &scratch );
printf("a: STX104_ADC_CONTROL=0x%02X\n", scratch );
}

		trans_count = 0;

		/* read ADC status */
		address_desc_ptr                = &device_pointer->pAddrDesc[0 /* BAR0 */];
		/* STX104_INT_STATUS_TRANSFER = 0 */
		trans_ptr[trans_count].dwPort   = address_desc_ptr->kptAddr + STX104_ADC_STATUS;
		trans_ptr[trans_count].cmdTrans = WDC_ADDR_IS_MEM(address_desc_ptr) ? RM_BYTE : RP_BYTE;
		trans_count++;

		/* read fifo status if we need to */
		if ( device_context_pointer->interrupt_handler_c != NULL ) /* fifo flag */
		{
			address_desc_ptr                = &device_pointer->pAddrDesc[STX104_BAR /* BAR0 */];
			/* STX104_INT_FIFO_STATUS_MSB_TRANSFER = 1 */
			trans_ptr[trans_count].dwPort   = address_desc_ptr->kptAddr + STX104_FIFO_FLAGS;
			trans_ptr[trans_count].cmdTrans = WDC_ADDR_IS_MEM(address_desc_ptr) ? RM_BYTE : RP_BYTE;		
			trans_count++;
			
			/* STX104_INT_FIFO_STATUS_LSB_TRANSFER = 2 */
			address_desc_ptr                = &device_pointer->pAddrDesc[STX104_BAR /* BAR0 */];
			trans_ptr[trans_count].dwPort   = address_desc_ptr->kptAddr + STX104_FIFO_DATA_STATUS;
			trans_ptr[trans_count].cmdTrans = WDC_ADDR_IS_MEM(address_desc_ptr) ? RM_BYTE : RP_BYTE;
			trans_count++;
		}

		/* write to the interrupt clear register */
		address_desc_ptr = &device_pointer->pAddrDesc[STX104_BAR /* BAR0 */];
		trans_ptr[trans_count].dwPort = address_desc_ptr->kptAddr + STX104_CLEAR_INTERRUPTS;
		trans_ptr[trans_count].cmdTrans = WDC_ADDR_IS_MEM(address_desc_ptr) ? WM_BYTE : WP_BYTE;
		trans_ptr[trans_count].Data.Byte = 0x00;
		trans_count++;

/*TEMP:
WDC_ReadAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_STATUS, &asr );
printf("b: asr=0x%02X\n", asr );*/
printf(" 0: dwPort=0x%04X, cmdTrans=%d\n", trans_ptr[0].dwPort, trans_ptr[0].cmdTrans );
printf(" 1: dwPort=0x%04X, cmdTrans=%d\n", trans_ptr[1].dwPort, trans_ptr[1].cmdTrans );
printf("    trans_count = %d\n", trans_count );
printf("    WDC_IS_KP(device_handle) = %d\n", WDC_IS_KP(device_handle) );

		/* Store the interrupt transfer commands in the device context */
		device_context_pointer->int_xfer_commands = trans_ptr;

/*2013NOV26*/
WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_CLEAR_INTERRUPTS, 0x00 );
WDC_ReadAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, &adc_control_register );
adc_control_register &= 0x0F;
adc_control_register |= ( (device_allocation[board].device_info.Item[2].I.Int.dwInterrupt & 0x0F) << 4 );
WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, adc_control_register );
WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_CLEAR_INTERRUPTS, 0x00 );

printf(" 2: adc_control_register=0x%02X\n", (int) adc_control_register );

/*
temp    -- really need a way to see whether or not interrupt is active or not!  */
		/* Enable the interrupts */
/*
		dw_status = WDC_IntEnable(device_handle, trans_ptr, trans_count, INTERRUPT_CMD_COPY,
			                      STX104_Int_Handler, (PVOID)device_pointer, WDC_IS_KP(device_handle));
*/
dw_status  = WD_STATUS_SUCCESS;


		if ( WD_STATUS_SUCCESS != dw_status )
		{
			AUL_Error_Log("Failed enabling interrupts. Error 0x%lx - %s\n", dw_status, Stat2Str(dw_status) );
			free( trans_ptr );
			return( STX104_EC_INT_FAILED_TO_ENABLE_INTERRUPT );
		}

/*
TEMP:
STX104_Index_Write_U8( board, STX104_INTERRUPT_SOURCE_SELECT, 0x02 );
WDC_ReadAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, &adc_control_register );
adc_control_register &= 0x0F;
adc_control_register |= ( (device_allocation[board].device_info.Item[0].I.Int.dwInterrupt & 0x0F) << 4 );
WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, adc_control_register );
WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_CLEAR_INTERRUPTS, 0x00 );



2013		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_CLEAR_INTERRUPTS, 0x00 );
2013		WDC_ReadAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, &adc_control_register );
2013		adc_control_register &= 0x0F;
2013		adc_control_register |= ( (device_allocation[board].device_info.Item[0].I.Int.dwInterrupt & 0x0F) << 4 );
2013		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, adc_control_register );
2013		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_CHANNEL, device_allocation[board].reg_store[STX104_REG_STORE_AI_CHANNEL_REG].byte[0] );
2013		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_CLEAR_INTERRUPTS, 0x00 );

TEMP		adc_control_register |= ( (device_allocation[board].device_info.Item[2].I.Int.dwInterrupt & 0x0F) << 4 );
adc_control_register |= ( (device_allocation[board].device_info.Item[0].I.Int.dwInterrupt & 0x0F) << 4 );
WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, adc_control_register );

TEMP:
WDC_ReadAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_STATUS, &asr );
printf("c: asr=0x%02X\n", asr );

TEMP:
printf("STX104_Int_Enable: adc_control_register=0x%02X\n", adc_control_register );*/
{
uint8_t x;
uint16_t y;
STX104_Index_Read_U8( board, STX104_INTERRUPT_SOURCE_SELECT, &x );
printf("STX104_Int_Enable: STX104_INTERRUPT_SOURCE_SELECT=0x%02X\n", x );

/*STX104_Index_Write_U8( board, STX104_INTERRUPT_SOURCE_SELECT, 0 );*/

STX104_Index_Read_U16( board, STX104_INTERRUPT_CONFIGURATION, &y );
printf("STX104_Int_Enable: STX104_INTERRUPT_CONFIGURATION=0x%04X\n", y );
}
/*

		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, adc_control_register );

WDC_ReadAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_STATUS, &asr );
printf("d: asr=0x%02X\n", asr );

WDC_Sleep( 500000, WDC_SLEEP_BUSY  );  //WDC_SLEEP_NON_BUSY

TEMP:
WDC_ReadAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_STATUS, &asr );
printf("d: asr=0x%02X\n", asr );
*/

		device_allocation[board].interrupt_active = true;

		AUL_Trace_Log("STX104_Int_Enable: interrupts enabled successfully\n" );

/*TEMP:*/
printf("STX104_Int_Enable: interrupts enabled successfully\n" );

	}
	else
	{
		error_code = STX104_EC_BOARD_NOT_FOUND;
	}

	return( error_code );
#endif
}

/**********************************************************************************************************
*/
static int STX104_Int_Disable( int board )
{
	int						error_code;
	devmgr_device_type *			device;
	stx104_dataset_type *	ds;

	device = AUL_Device_Get( board );
	ds		 = (stx104_dataset_type *) device->private_data;
	ds->intr_params.interrupt_list_active = STX104_INTR_MASK_NONE;
	error_code = IO_Interrupt_Disable( device->io_device );
	return error_code;

#if(0)
	DWORD dw_status;
	int error_code;
	uint8_t adc_control_register;
    PWDC_DEVICE device_pointer;
	WDC_DEVICE_HANDLE device_handle;
	AUL_DEVICE_CONTEXT_PTR device_context_pointer;







    AUL_Trace_Log("STX104_Int_Disable entered. Device handle: %d\n", board );

	device_handle  = device_allocation[board].device_handle;
	device_pointer = (PWDC_DEVICE) device_handle;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Int_Enable_Set_Error;

		/* test validity of board pointers, just to be sure */
		if ( !IsValidDevice(device_pointer, "STX104_Int_Disable") )
		{
			return( STX104_EC_BOARD_NOT_FOUND );
		}

		device_context_pointer = (AUL_DEVICE_CONTEXT_PTR) WDC_GetDevContext( device_pointer );

		if ( !WDC_IntIsEnabled(device_handle) )
		{
			return( STX104_EC_INT_INTERRUPTS_ALREADY_DISABLED );
		}

		WDC_ReadAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, &adc_control_register );

		adc_control_register = adc_control_register & 0x0F; /* mask off the interrupts effectively disabling */

		WDC_WriteAddr8(device_allocation[board].device_handle, STX104_BAR, STX104_ADC_CONTROL, adc_control_register );

		/* Disable the interrupts */
		dw_status = WDC_IntDisable( device_handle );
		if ( WD_STATUS_SUCCESS != dw_status )
		{
			AUL_Error_Log("Failed disabling interrupts. Error 0x%lx - %s\n",dw_status, Stat2Str(dw_status));
		}

		/* Free the memory allocated for the interrupt transfer commands */
		if ( device_context_pointer->int_xfer_commands )
		{
			free( device_context_pointer->int_xfer_commands );
			device_context_pointer->int_xfer_commands = NULL;
		}

		device_allocation[board].interrupt_active = false;


	return( error_code );
#endif
}
/**********************************************************************************************************//**
@ingroup stx104_events
\brief
Apex Univeral Library STX104 specific function that retrieves the event (interrupt) configuration. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] cfg A pointer to the ::STX104_INT_CFG_TYPE structure that will be used to retrieve the event (interrupt) configuration information. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Int_CFG_Get( int board, STX104_INT_CFG_TYPE * cfg )
{
	int							error_code;
	devmgr_device_type *				device;
	stx104_dataset_type *	ds;
	uint8_t						interrupt_sources;
	uint16_t						interrupt_cfg;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Int_CFG_Get_Error;
	ds = (stx104_dataset_type *) device->private_data;
	Mem_Set( cfg, 0, sizeof( STX104_INT_CFG_TYPE ) );

	STX104_Index_Read_U32( board, STX104_INTERRUPT_THRESHOLD,   &cfg->threshold );
		
	STX104_Index_Read_U8(  board, STX104_INTERRUPT_SOURCE_SELECT, &interrupt_sources );
	cfg->source_a = (STX104_INT_SRCA_ENUM)( interrupt_sources & 0x0F );
	cfg->source_b = (STX104_INT_SRCB_ENUM)( interrupt_sources >> 4   );

	STX104_Index_Read_U16( board, STX104_INTERRUPT_CONFIGURATION, &interrupt_cfg );
	if ( (interrupt_cfg & 0x1000) != 0 ) cfg->threshold_sync_to_triggering = true;
	cfg->threshold_source     = (STX104_INT_THRESHOLD_SRC_ENUM)( (interrupt_cfg >> 8) & 0x000F );
	cfg->blocks_per_interrupt = (STX104_INT_BLOCKS_PER_ENUM)( interrupt_cfg & 0x000F );

	cfg->callback_source_a = ds->intr_params.interrupt_handler_a;
	cfg->callback_source_b = ds->intr_params.interrupt_handler_b;
	cfg->callback_fifo_blk = ds->intr_params.interrupt_handler_c;

	cfg->callback_source_a_user_dataset = ds->intr_params.interrupt_dataset_a;
	cfg->callback_source_b_user_dataset = ds->intr_params.interrupt_dataset_b;
	cfg->callback_fifo_blk_user_dataset = ds->intr_params.interrupt_dataset_c;

	return error_code;
STX104_Int_CFG_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_events
\brief
Apex Univeral Library STX104 specific function that retrieves the event (interrupt) configuration. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] cfg A pointer to the ::STX104_INT_CFG_TYPE structure that will be used to set the event (interrupt) configuration information. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Int_CFG_Set( int board, STX104_INT_CFG_TYPE * cfg )
{
	int							error_code;
	devmgr_device_type *				device;
	stx104_dataset_type *	ds;
	uint8_t						interrupt_sources;
	uint16_t						interrupt_cfg;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Int_CFG_Set_Error;
	ds = (stx104_dataset_type *) device->private_data;

	if ( device->io_irq_active ) 
	{	
		error_code = EC_STX104_CFG_INTERRUPT_ACTIVE;			
		goto STX104_Int_CFG_Set_Error;	
	}

	Mem_Set( &(ds->intr_params), 0, sizeof( stx104_intr_params_type ) );
	interrupt_sources = 0;
	/* test for callback-A */
	if ( NULL != cfg->callback_source_a ) 
	{
		interrupt_sources  = cfg->source_a & 0x0F;
		ds->intr_params.interrupt_handler_a = cfg->callback_source_a;
		ds->intr_params.interrupt_dataset_a = cfg->callback_source_a_user_dataset;
/*		ds->intr_params.interrupt_callback_a_active = true;
		ds->intr_params.interrupt_list_active |= STX104_INTR_MASK_SRC_A; */
	}
	else
	{
		interrupt_sources  = ISA_NONE & 0x0F;
	}
	/* test for callback-B */
	if ( NULL != cfg->callback_source_b ) 
	{
		interrupt_sources |= ( (cfg->source_b & 0x0F) << 4 );
		ds->intr_params.interrupt_handler_b = cfg->callback_source_b;
		ds->intr_params.interrupt_dataset_b = cfg->callback_source_a_user_dataset;
/*		ds->intr_params.interrupt_callback_b_active = true;
		ds->intr_params.interrupt_list_active |= STX104_INTR_MASK_SRC_B; */
	}
	else
	{
		interrupt_sources |= ( (ISB_NONE & 0x0F) << 4 );
	}
	/* test for fifo block callback */
	if ( cfg->callback_fifo_blk )
	{
		ds->intr_params.interrupt_handler_c = cfg->callback_fifo_blk;
		ds->intr_params.interrupt_dataset_c = cfg->callback_fifo_blk_user_dataset;
/*		ds->intr_params.interrupt_callback_c_active = true;
		ds->intr_params.interrupt_list_active |= STX104_INTR_MASK_SRC_C; */
	}
	/* determine interrupt configuration register */
	interrupt_cfg  = 0xE000; /* EIS='1', ISSBE='1', nINT_FF='1' */
	if ( cfg->threshold_sync_to_triggering ) interrupt_cfg |= 0x1000;                                             
	interrupt_cfg |= ( (cfg->threshold_source & 0x0F) << 8 );
	interrupt_cfg |= ( cfg->blocks_per_interrupt & 0x0F );
	/* write to hardware registers */
	STX104_Index_Write_U32( board, STX104_INTERRUPT_THRESHOLD,   cfg->threshold );
	STX104_Index_Write_U8( board, STX104_INTERRUPT_SOURCE_SELECT, interrupt_sources );
	STX104_Index_Write_U16( board, STX104_INTERRUPT_CONFIGURATION, interrupt_cfg  );

	ds->intr_params.board               = board;
	ds->intr_params.device				= device;
	ds->intr_params.message_a           = cfg->source_a & 0x0F;
	ds->intr_params.message_b           = cfg->source_b & 0x0F;

	return error_code;
STX104_Int_CFG_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_events
\brief
Apex Univeral Library STX104 specific function that retrieves the event (interrupt) enable state. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] enable  Pointer to the variable that will receive the event (interrupt) enable state.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Int_Enable_Get( int board, BOOL * enable )
{
	int				error_code;
	devmgr_device_type *	device;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Int_Enable_Get_Error;
	if ( device->io_irq_active ) *enable = TRUE;
	else                         *enable = FALSE;
	return error_code;
STX104_Int_Enable_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_events
\brief
Apex Univeral Library STX104 specific function that sets the event (interrupt) enable state. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] enable Value that sets the event (interrupt) enable state. if set to true, then interrupts are enabled; otherwise, interrupts are disabled. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_Int_Enable_Set( int board, BOOL enable )
{
	int				error_code;
	devmgr_device_type *	device;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Int_Enable_Set_Error;
	/* enable/disable interrupt */
	if ( enable != 0 ) 
	{
		error_code = STX104_Int_Enable( board );
	}
	else
	{
		error_code = STX104_Int_Disable( board );
	}	
	return error_code;
STX104_Int_Enable_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}


/**********************************************************************************************************
*/
int STX104_Int_Termination( void )
{
	return SUCCESS;
}
/**********************************************************************************************************
*/
int STX104_Int_Initialization( void )
{
	return SUCCESS;
}

