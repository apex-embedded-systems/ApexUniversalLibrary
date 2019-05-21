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
DONE: 2. verify compilation.
3. IO_Xfr() requires a test to verify virtual_address is within range.
4. IO_Interrupt_Enable() must include io_xfr information and then translate
   that to Jungo compatible stuff.

*********************************************************************************/

/* windriver headers */
#include "windrvr.h"
#include "wdc_defs.h"
#include "wdc_lib.h"
#include "utils.h"
/* #include "samples/shared/bits.h" */
#include "status_strings.h"

#include "io_private.h"
#include "io_shared.h"
#include "../shared/mem.h"			 /* memory allocation      */
#include "../shared/status.h"     /* error codes and APIs	*/
#include "../shared/error.h"      /* error handling			*/
#include "../shared/device.h"		 /* device qty	 and device undefined  */
#include "../shared/target.h"		 /* TARGET_BUS_APERTURE_WIDTH_BYTES */
#include "../shared/cmd.h"			 /* Ioctl() commands */
#include "../shared/cmd_macros.h"
#include "../str_safe/str_safe.h"
/*********************************************************************************/
#define IO_MODULE_REV  "$Date: 2014-06-25 10:29:34 -0500 (Wed, 25 Jun 2014) $"
/*********************************************************************************/
#define IO_DEBUG_STRING_LENGTH  256
/*********************************************************************************/
#define IO_BASE_ADDRESS_REGISTER 0
/*********************************************************************************/
#define IO_DEFAULT_DRIVER_NAME "auldrv"
/*********************************************************************************/
#define IO_WLG_DRV_DEBUG_MODE  WDC_DBG_DEFAULT
/*******************************************************************************/
typedef struct io_wlg_intr_diagnostics
{
	int device;
	uint32_t counter; /* Number of interrupts received */
	uint32_t lost;    /* Number of interrupts not yet handled */
	int		 wait_result; /* See WD_INTERRUPT_WAIT_RESULT values in windrvr.h */
	uint32_t enabled_int_type; /* Interrupt type that was actually enabled
                                 (MSI/MSI-X/Level Sensitive/Edge-Triggered) */
	uint32_t last_message; /* Message data of the last received MSI/MSI-X
                             (Windows Vista/7); N/A to line-based interrupts) */
};
/*********************************************************************************/
struct io_wlg_interrupt_context
{
	size_t								type;
	int									device;
	struct io_wlg_intr_diagnostics		diagnostics;
	io_int_handler						interrupt_handler;
	void *								private_data;
};
typedef struct io_wlg_interrupt_context io_wlg_interrupt_context_type;
/*********************************************************************************/
/* internal data structure specific to this module -- for internal use only */
struct io_wlg_device_private 
{
	WD_CARD    						info;
	WDC_DEVICE_HANDLE				handle;
	struct io_wlg_interrupt_context	interrupt_context;
	io_definition_type *			io_definition;
};
/*********************************************************************************/
static struct io_wlg_device_private io_wlg_private[DEVICE_QTY];  /* private dataset */ 

//static char io_wlg_debug_string[IO_DEBUG_STRING_LENGTH];
/* may need semaphore? */
///*********************************************************************************/
//void IO_Debug_Log( const char * format, ... )
//{
//	va_list argp;
//	va_start( argp, format );
//	vsnprintf( io_wlg_debug_string, IO_DEBUG_STRING_LENGTH - 1, format, argp );
//	va_end( argp );
//	/* auldrv: <message> */
//	WDC_Trace(	"%s: %s", 
//					IO_DEFAULT_DRIVER_NAME,
//					io_wlg_debug_string
//				);
//}
///*********************************************************************************/
//void IO_Error_Log( const char * format, ... )
//{
//	va_list argp;
//	va_start( argp, format );
//	vsnprintf( io_wlg_debug_string, IO_DEBUG_STRING_LENGTH - 1, format, argp );
//	va_end( argp );
//#if defined( DEBUG )
//	WDC_Err(	"%s: %s", IO_DEFAULT_DRIVER_NAME, io_wlg_debug_string );
//#endif
//}
/*********************************************************************************/
IO_ID_ENUM IO_ID_Get( void ) {	return IO_ID_WIN32_LIB_GENERIC;  }
/*********************************************************************************/
int IO_Read_U8( int iod, size_t offset, uint8_t * value )
{
	int		error_code;
	size_t	address;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	address		 = offset - io_wlg_private[iod].io_definition->region_offset;
	error_code   = WDC_ReadAddr8(	io_wlg_private[iod].handle, 
									IO_BASE_ADDRESS_REGISTER,
									address, 
									value 
								 );
	if ( WD_STATUS_SUCCESS != error_code )  error_code = EC_IO_READ_U8_ERROR;
	else                                    error_code = SUCCESS;
	IO_Error_Set( error_code );
	return error_code;		
}
/*********************************************************************************/
int IO_Read_U16( int iod, size_t offset, uint16_t * value )
{
	int		error_code;
	size_t	address;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	address = offset - io_wlg_private[iod].io_definition->region_offset;
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		uint8_t u8_lo;
		uint8_t u8_hi;
		error_code = WDC_ReadAddr8(	io_wlg_private[iod].handle, 
									IO_BASE_ADDRESS_REGISTER,
									address + 0, 
									&u8_lo 
								  );
		if ( WD_STATUS_SUCCESS != error_code )  
		{	error_code = EC_IO_READ_U16_LO_U8_ERROR;	goto IO_Read_U16_Error;  }
		error_code = WDC_ReadAddr8(	io_wlg_private[iod].handle, 
										IO_BASE_ADDRESS_REGISTER,
										address + 1, 
										&u8_hi  
									);
		if ( WD_STATUS_SUCCESS != error_code )  
		{	error_code = EC_IO_READ_U16_HI_U8_ERROR;	goto IO_Read_U16_Error;  }	
		*value = IO_U16_SET_U8_LO_HI(u8_lo,u8_hi);
	}
	else
	{
		error_code = WDC_ReadAddr16(	io_wlg_private[iod].handle, 
										IO_BASE_ADDRESS_REGISTER,
										address, 
										value 
									);
		if ( WD_STATUS_SUCCESS != error_code )   
		{	error_code = EC_IO_READ_U16_ERROR;	goto IO_Read_U16_Error;  }
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
	size_t address;
	size_t byte_count = count;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	address = offset - io_wlg_private[iod].io_definition->region_offset;
	error_code = WDC_ReadAddrBlock(	io_wlg_private[iod].handle, 
										IO_BASE_ADDRESS_REGISTER, 
										address,						/* offset		*/
										byte_count,						/* number of bytes */
										(void *) buffer,				/* void * buffer */
										WDC_MODE_8,						/* mode */
										WDC_ADDR_RW_NO_AUTOINC
									);
	if ( error_code )		
	{	error_code = EC_IO_READ_U8_ERROR;	goto IO_Read_Repeat_U8_Error; }
	error_code = SUCCESS;
IO_Read_Repeat_U8_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Read_Repeat_U16( int iod, size_t offset, uint16_t * buffer, size_t count )
{
	int			error_code;
	size_t address;
	size_t byte_count = count * sizeof( uint16_t);
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	address = offset - io_wlg_private[iod].io_definition->region_offset;
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		size_t index;
		uint8_t u8_lo, u8_hi;
		for ( index = 0; index < count; index++ )
		{
			error_code = WDC_ReadAddr8(	io_wlg_private[iod].handle, 
											IO_BASE_ADDRESS_REGISTER,
											address + 0, 
											&u8_lo 
										);
			if ( error_code )		
			{	error_code = EC_IO_READ_U16_LO_U8_ERROR;	goto IO_Read_Repeat_U16_Error; }
			error_code = WDC_ReadAddr8(	io_wlg_private[iod].handle, 
											IO_BASE_ADDRESS_REGISTER,
											address + 1, 
											&u8_hi 
										);
			if ( error_code )		
			{	error_code = EC_IO_READ_U16_HI_U8_ERROR;	goto IO_Read_Repeat_U16_Error; }
			buffer[index] = IO_U16_SET_U8_LO_HI(u8_lo,u8_hi);
		}
	}
	else
	{
		error_code = WDC_ReadAddrBlock(	io_wlg_private[iod].handle, 
											IO_BASE_ADDRESS_REGISTER,
											address,							/* offset		*/
											byte_count,						/* number of bytes */
											(void *) buffer,				/* void * buffer */
											WDC_MODE_16,					/* mode */
											WDC_ADDR_RW_NO_AUTOINC
										);
		if ( error_code )		
		{	error_code = EC_IO_READ_U16_ERROR;	goto IO_Read_Repeat_U16_Error; }
	}
	error_code = SUCCESS;
IO_Read_Repeat_U16_Error:
	IO_Error_Set( error_code );
	return error_code;		
}
/*********************************************************************************/
int IO_Write_U8( int iod, size_t offset, uint8_t  value )
{
	int			error_code;
	size_t address;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	address = offset - io_wlg_private[iod].io_definition->region_offset;
	error_code = WDC_WriteAddr8(	io_wlg_private[iod].handle, 
									IO_BASE_ADDRESS_REGISTER,
									address, 
									value 
								);
	if ( error_code )		
	{	error_code = EC_IO_WRITE_U8_ERROR;	goto IO_Write_U8_Error; }
	error_code = SUCCESS;
IO_Write_U8_Error:
	IO_Error_Set( error_code );
	return error_code;	
}
/*********************************************************************************/
int IO_Write_U16( int iod, size_t offset, uint16_t  value )
{
	int	   error_code;
	size_t address;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	address = offset - io_wlg_private[iod].io_definition->region_offset;
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		uint8_t u8_lo = IO_U16_GET_U8_LO( value );
		uint8_t u8_hi = IO_U16_GET_U8_HI( value );
		error_code = WDC_WriteAddr8(	io_wlg_private[iod].handle, 
										IO_BASE_ADDRESS_REGISTER,
										address + 0, 
										u8_lo 
									);
		if ( error_code )		
		{	error_code = EC_IO_WRITE_U16_LO_U8_ERROR;	goto IO_Write_U16_Error; }
		error_code = WDC_WriteAddr8(	io_wlg_private[iod].handle, 
										IO_BASE_ADDRESS_REGISTER,
										address + 1, 
										u8_hi 
									);
		if ( error_code )		
		{	error_code = EC_IO_WRITE_U16_HI_U8_ERROR;	goto IO_Write_U16_Error; }
	}
	else
	{
		error_code = WDC_WriteAddr16(	io_wlg_private[iod].handle, 
										IO_BASE_ADDRESS_REGISTER,
										address, 
										value 
									  );
		if ( error_code )		
		{	error_code = EC_IO_WRITE_U16_ERROR;	goto IO_Write_U16_Error; }
	}
	error_code = SUCCESS;
IO_Write_U16_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Write_Repeat_U8( int iod, size_t offset, uint8_t * buffer, size_t count )
{
	int	   error_code;
	size_t address;
	size_t byte_count = count;
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint8_t ) );
	if ( error_code ) return error_code;
#endif
	address = offset - io_wlg_private[iod].io_definition->region_offset;
	error_code = WDC_WriteAddrBlock(	io_wlg_private[iod].handle, 
										IO_BASE_ADDRESS_REGISTER,
										address,						/* offset		*/
										byte_count,						/* number of bytes */
										(void *) buffer,				/* void * buffer */
										WDC_MODE_8,						/* mode */
										WDC_ADDR_RW_NO_AUTOINC
									 );
	if ( error_code )		
	{	error_code = EC_IO_WRITE_U8_ERROR;	goto IO_Write_Repeat_U8_Error; }
	error_code = SUCCESS;
IO_Write_Repeat_U8_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
int IO_Write_Repeat_U16( int iod, size_t offset, uint16_t * buffer, size_t count )
{
	int	   error_code;
	size_t address;
	size_t byte_count = count * sizeof( uint16_t);
#if defined( IO_DEBUG )
	error_code = IO_IsNotValid( iod, offset, sizeof( uint16_t ) );
	if ( error_code ) return error_code;
#endif
	address = offset - io_wlg_private[iod].io_definition->region_offset;
	if ( IO_Is_Restrict_8Bit( iod ) )
	{
		size_t index;
		uint8_t u8;
		for ( index = 0; index < count; index++ )
		{
			u8 = IO_U16_GET_U8_LO( buffer[index] );
			error_code = WDC_WriteAddr8(	io_wlg_private[iod].handle, 
											IO_BASE_ADDRESS_REGISTER,
											address + 0, 
											u8 
										);
			if ( error_code )		
			{	error_code = EC_IO_WRITE_U16_LO_U8_ERROR;	goto IO_Write_Repeat_U16_Error; }
			u8 = IO_U16_GET_U8_HI( buffer[index] );
			error_code = WDC_WriteAddr8(	io_wlg_private[iod].handle, 
											IO_BASE_ADDRESS_REGISTER,
											address + 1, 
											u8 
										);
			if ( error_code )		
			{	error_code = EC_IO_WRITE_U16_HI_U8_ERROR;	goto IO_Write_Repeat_U16_Error; }
		}
	}
	else
	{
		error_code = WDC_WriteAddrBlock(	io_wlg_private[iod].handle, 
											IO_BASE_ADDRESS_REGISTER,
											address,							/* offset		*/
											byte_count,						/* number of bytes */
											(void *) buffer,				/* void * buffer */
											WDC_MODE_16,					/* mode */
											WDC_ADDR_RW_NO_AUTOINC
										);
		if ( error_code )		
		{	error_code = EC_IO_WRITE_U16_ERROR;	goto IO_Write_Repeat_U16_Error; }
	}
	error_code = SUCCESS;
IO_Write_Repeat_U16_Error:
	IO_Error_Set( error_code );
	return error_code;
}
/*********************************************************************************/
static int IO_IsValidDevice( PWDC_DEVICE pdev )
{
	if ( ( NULL == pdev ) || ( NULL == WDC_GetDevContext( pdev ) ) )
	{
		int error_code = EC_IO_HANDLE_NULL;
		Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
		return -error_code;
	}
	return SUCCESS;
}
/*********************************************************************************/
static int IO_IsItemExist( PWDC_DEVICE pdev, ITEM_TYPE item )
{
	int	error_code;
	DWORD	i;
	DWORD dwNumItems = pdev->cardReg.Card.dwItems;

	for (i=0; i < dwNumItems; i++)
	{
		if ( pdev->cardReg.Card.Item[i].item == item )
		return SUCCESS;
	}
	error_code = EC_IO_INTERRUPT_NOT_DEFINED;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*********************************************************************************/
static void __stdcall IO_WLG_Interrupt_Handler( void * private_data )
{
	PWDC_DEVICE pdev = (PWDC_DEVICE) private_data;
	io_wlg_interrupt_context_type * interrupt_context = (io_wlg_interrupt_context_type *) WDC_GetDevContext( pdev);
	struct io_wlg_intr_diagnostics * ds = &(interrupt_context->diagnostics);

	//Mem_Set( ds, 0, sizeof( struct io_int_diagnostics ) );
	ds->counter				= pdev->Int.dwCounter;
	ds->lost					= pdev->Int.dwLost;
	//ds->wait_result		= (WD_INTERRUPT_WAIT_RESULT) pdev->Int.fStopped;
	ds->wait_result		= pdev->Int.fStopped;
	ds->enabled_int_type	= WDC_GET_ENABLED_INT_TYPE( pdev );
	ds->last_message		= WDC_GET_ENABLED_INT_LAST_MSG( pdev );
    
	/* Execute the application's interrupt handler routine */
	if ( NULL != interrupt_context->interrupt_handler )
	{
		interrupt_context->interrupt_handler( interrupt_context->private_data );
	}
}
//*********************************************************************************/
static int IO_WLG_Interrupt_Disable( struct io_device_dataset * ds )
{
	int				error_code;
	int				ld;  /* short for local device */
	DWORD			dwStatus;
	PWDC_DEVICE		wdc_device;
	//PAULDRV_DEV_CTX pDevCtx;
	io_wlg_interrupt_context_type *	interrupt_context;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s:enter\n", __func__ );
#endif
	ld = ds->iod;
	wdc_device = (PWDC_DEVICE) io_wlg_private[ld].handle;
	//if (!IsValidDevice(pDev, "AULDRV_IntDisable"))
   //     return WD_INVALID_PARAMETER;
	error_code = IO_IsValidDevice( wdc_device );
	if ( error_code ) goto IO_WLG_Interrupt_Disable_Error; 
	//pDevCtx = (PAULDRV_DEV_CTX)WDC_GetDevContext(pDev);
	interrupt_context = (io_wlg_interrupt_context_type *) WDC_GetDevContext( wdc_device ); 
	if ( !WDC_IntIsEnabled( io_wlg_private[ld].handle ) )
	{	error_code = EC_IO_INTERRUPT_INACTIVE;		goto IO_WLG_Interrupt_Disable_Error;  }

/* 
TODO: Interrupts at hardware must disabled at dev_<name>.c levels.
      In other words, at the device layer, but before IO_Interrupt_Disable()
		is called.
*/

    /* Disable the interrupts */
	dwStatus = WDC_IntDisable( io_wlg_private[ld].handle );
	if ( WD_STATUS_SUCCESS != dwStatus )
	{	error_code = EC_IO_INTERRUPT_DISABLE;		goto IO_WLG_Interrupt_Disable_Error;  }
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_WLG_Interrupt_Disable_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
	IO_Error_Set( error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_WLG_Interrupt_Enable( struct io_device_dataset * ds, io_int_handler handler, void * private_data )
{
	int				error_code;
	int				ld;  /* short for local device */
	DWORD			dwStatus;
	PWDC_DEVICE		wdc_device;
	//PAULDRV_DEV_CTX pDevCtx;
	io_wlg_interrupt_context_type *	interrupt_context;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s:enter\n", __func__ );
#endif
	ld = ds->iod;
	wdc_device = (PWDC_DEVICE) io_wlg_private[ld].handle;

	error_code = IO_IsValidDevice( wdc_device );
	if ( error_code ) goto IO_WLG_Interrupt_Enable_Error; 

	error_code = IO_IsItemExist( wdc_device, ITEM_INTERRUPT );
	if ( error_code ) goto IO_WLG_Interrupt_Enable_Error;  

    //pDevCtx = (PAULDRV_DEV_CTX)WDC_GetDevContext(pDev);
	interrupt_context = (io_wlg_interrupt_context_type *) WDC_GetDevContext( wdc_device ); 

	/* Check if interrupts are already enabled */
	if ( WDC_IntIsEnabled( io_wlg_private[ld].handle ) )
	{	error_code = EC_IO_INTERRUPT_ACTIVE;	goto IO_WLG_Interrupt_Enable_Error; }

    /* Define the number of interrupt transfer commands to use */
    #define NUM_TRANS_CMDS 0

//TODO: ??  rebuild xfr into wdc xfr.

    /* Store the diag interrupt handler routine, which will be executed by
       AULDRV_IntHandler() when an interrupt is received */
    interrupt_context->interrupt_handler = handler;
	 interrupt_context->private_data      = private_data;
    
    /* Enable the interrupts */
    /* NOTE: When adding read transfer commands, set the INTERRUPT_CMD_COPY flag
             in the 4th argument (dwOptions) passed to WDC_IntEnable() */
    dwStatus = WDC_IntEnable( io_wlg_private[ld].handle, /* device handle			*/
										NULL,						/* WD_TRANSFER *			*/
										0,							/* number of commands	*/
										0,							/* options					*/
										IO_WLG_Interrupt_Handler,/* typedef void (*INT_HANDLER)(void * private_data ) */
										(PVOID) wdc_device,	/* private data			*/
										WDC_IS_KP( io_wlg_private[ld].handle )
									 );
        
	if ( WD_STATUS_SUCCESS != dwStatus )
	{	error_code = EC_IO_INTERRUPT_ENABLE;	goto IO_WLG_Interrupt_Enable_Error; }

/* 
TODO: Interrupts at hardware must enabled at dev_<name>.c levels.
      In other words, at the device layer, but after IO_Interrupt_Enable()
		has been called.
*/

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_WLG_Interrupt_Enable_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
	IO_Error_Set( error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static const char io_wlg_full_name[] = IO_DEFAULT_DRIVER_NAME ".sys";
/*********************************************************************************/
static int IO_WLG_Ioctl( struct io_device_dataset * ds, unsigned int command, unsigned long argument )
{
	int		error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif	
	
	error_code = EC_IO_IOCTL_COMMAND_UNKNOWN; /* command invalid */
	switch( command )
	{	/* emulate the driver */
		case CMD_AULDRV_NAME:
			{
				if ( IO_COPY_TO_USER( argument, io_wlg_full_name, String_Length( io_wlg_full_name ) + 1 )  )
				{ error_code = EC_IO_COPY_TO_USER; goto IO_WLG_Ioctl_Error;  }
				else
				{ error_code = SUCCESS;  }
			}
			break;
		case CMD_AULDRV_DEVICE_LIST_GET:
			return -EC_IO_WARN_DEVICE_LIST_UNKNOWN;
		case CMD_AULDRV_DEBUG_ACTIVE_GET:
			{ /* return zero if IO_DEBUG is not defined, otherwise non-zero */
				int debug_active;
#if IO_WLG_DRV_DEBUG_MODE == WDC_DBG_NONE IO_DEBUG
				debug_active = 0;
#else
				debug_active = 1;
#endif
				if ( IO_COPY_TO_USER( argument, &debug_active, sizeof( debug_active ) ) )
				{ error_code = EC_AULDRV_COPY_TO_USER;	goto IO_WLG_Ioctl_Error;  }
			}
			break;
	}
	if ( error_code ) goto IO_WLG_Ioctl_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_WLG_Ioctl_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_WLG_Open( struct io_device_dataset * ds )
{
	DWORD							 wd_status;
	WD_CARD *						 info;		/* short cut pointer */
	WDC_DEVICE_HANDLE	*			 hp;		/* short cut pointer */
	WD_ITEMS *						 items;
	int								 ld;  /* short for local device */
	int								 error_code;
	io_wlg_interrupt_context_type * interrupt_context;

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	ld = ds->iod;
	interrupt_context = &(io_wlg_private[ld].interrupt_context);
	Mem_Set( interrupt_context, 0, sizeof( io_wlg_interrupt_context_type ) );
	interrupt_context->device = ld;

	/*** Set the device's resources information */     
	info   = &(io_wlg_private[ld].info);
	Mem_Set( info, 0, sizeof( WD_CARD ) );
	hp = &(io_wlg_private[ld].handle);
	/*	address space:	index = 0, 
		bus items:		index = 1,
		interrupts:		index = 2, if present
	*/
	if ( 0 == ds->io_definition.interrupt_number )  info->dwItems = 2;
	else                                            info->dwItems = 3;
//	info->dwItems = 3;			/* total items in our list */
	items = &(info->Item[0]);	/* short cut */
	/* Bus */
	items[0].item = ITEM_BUS;
	items[0].I.Bus.dwBusType = WD_BUS_ISA;
	/* BAR0 - BAR0  */
	items[1].item = ITEM_IO;
	items[1].fNotSharable = FALSE;
	items[1].I.IO.dwAddr = ds->io_definition.region_offset;
	items[1].I.IO.dwBytes = ds->io_definition.region_bytes;
	items[1].I.IO.dwBar = IO_BASE_ADDRESS_REGISTER; /* = 0 */

	if ( 0 != ds->io_definition.interrupt_number )
	{
		/* Int - Int: Interrupt Number, Level Sensitive */
		items[2].item = ITEM_INTERRUPT;
		items[2].fNotSharable = TRUE;
		items[2].I.Int.dwInterrupt = ds->io_definition.interrupt_number;
		items[2].I.Int.dwOptions = INTERRUPT_LEVEL_SENSITIVE;
	}
	/* Open a WDC device handle */
	wd_status = WDC_IsaDeviceOpen(	hp,					/* pointer to handle */ 
									info,				/* hardware info		*/
									interrupt_context, /* interrupt data	*/
									NULL,				/* reserved				*/
									NULL,				/* kernel plugin name */
									NULL				 /* kernel plugin data */
								);
	if ( WD_STATUS_SUCCESS != wd_status )
	{
		//ErrLog("Failed opening a WDC device handle. Error 0x%lx - %s\n",
		//	dwStatus, Stat2Str(dwStatus));
		int scratch = ld;
		if ( NULL != *hp ) IO_Close( &scratch );
		error_code = EC_IO_OPEN;
		io_wlg_private[ld].handle = NULL;
		goto IO_WLG_Open_Error;
	}
	io_wlg_private[ld].io_definition = &( ds->io_definition );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	ds->private_data = &(io_wlg_private[ld]);
	return SUCCESS;
IO_WLG_Open_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
	//IO_Error_Set( error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
static int IO_WLG_Close( struct io_device_dataset * ds )
{
	int			ld;  /* short hand for local device */
	DWORD		dwStatus;
	PWDC_DEVICE wdc_device;
	//PAULDRV_DEV_CTX pDevCtx;
	int error_code;
	io_wlg_interrupt_context_type * interrupt_context;
   
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: enter\n", __func__ );
#endif

	if ( NULL == ds->private_data ) return SUCCESS;

	ld			  = ds->iod;
	error_code = SUCCESS;
	wdc_device = (PWDC_DEVICE) io_wlg_private[ld].handle;

	//pDevCtx = (PAULDRV_DEV_CTX)WDC_GetDevContext(pDev);
	interrupt_context = (io_wlg_interrupt_context_type *) WDC_GetDevContext( wdc_device ); 
	/* Disable interrupts */
	if ( WDC_IntIsEnabled( io_wlg_private[ld].handle ) )
	{
		dwStatus = IO_Interrupt_Disable( ld );
		if (WD_STATUS_SUCCESS != dwStatus)
		{	error_code = EC_IO_INTERRUPT_DISABLE;	goto IO_WLG_Close_Error;	}
	}
	/* Close the device */
	dwStatus = WDC_IsaDeviceClose( io_wlg_private[ld].handle );
	if ( WD_STATUS_SUCCESS != dwStatus )
	{	error_code = EC_IO_CLOSE;		goto IO_WLG_Close_Error;		}
	///* Free interrupt allocated data space */
	//if ( interrupt_context ) Mem_Free( NULL, (void **) &interrupt_context );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	io_wlg_private[ld].handle = NULL;
	ds->private_data = NULL;
	return SUCCESS;
IO_WLG_Close_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
	//IO_Error_Set( error_code );
	if ( NULL != io_wlg_private[ld].handle ) io_wlg_private[ld].handle = NULL;
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
#if defined( IO_DEBUG )
static int IO_WLG_Validity( int iod, size_t virtual_address, size_t data_bytes )
{ /* Refer to IO_Validity().  Make sure that we perform any validity tests */
	(void) virtual_address;
	(void) data_bytes;
	return SUCCESS;
}
#endif
/*********************************************************************************/
static int IO_WLG_Terminate( void )
{
	int         error_code;
   DWORD 		status;

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	/* Uninit the WDC library and close the handle to WinDriver */
	status = WDC_DriverClose();
	if (WD_STATUS_SUCCESS != status)
	{	error_code = EC_IO_TERMINATION_CLOSE_ERROR;	goto IO_WLG_Terminate_Error;	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_WLG_Terminate_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
	//IO_Error_Set( error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
/* 10.40 license */
#define IO_DEFAULT_LICENSE_STRING "6f1eafff6b7cb01307be990e9bf1b80828d9bf96829aed.WD1040_32_NL_Apex_Embedded_Systems"

/*********************************************************************************/
static int IO_WLG_Initialize( void )
{
	int		error_code;
	size_t	index;
	DWORD	status;

#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	/* Redundant? Only if NULL=0. */
	for ( index = 0; index < DEVICE_QTY; index++ ) io_wlg_private[index].handle = NULL;
#if defined( WD_DRIVER_NAME_CHANGE )
	/* Set the driver name */
	if ( !WD_DriverName( IO_DEFAULT_DRIVER_NAME) )
	{	error_code = EC_IO_INIT_LIB_NAME_ERROR;	goto IO_WLG_Initialize_Error;	}
#endif
	/* Set WDC library's debug options (default: level TRACE, output to Debug Monitor) */
	status = WDC_SetDebugOptions( IO_WLG_DRV_DEBUG_MODE, NULL );
	if ( WD_STATUS_SUCCESS != status )
	{	error_code = EC_IO_INIT_LIB_DEBUG_ERROR;	goto IO_WLG_Initialize_Error;	}
	/* Open a handle to the driver and initialize the WDC library */
	status = WDC_DriverOpen( WDC_DRV_OPEN_DEFAULT, IO_DEFAULT_LICENSE_STRING );
	if ( WD_STATUS_SUCCESS != status )
	{	error_code = EC_IO_INIT_LIB_ERROR;			goto IO_WLG_Initialize_Error;	}
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
IO_WLG_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_IO_NAME, __func__, __LINE__, error_code );
	//IO_Error_Set( error_code );
#if defined( IO_DEBUG_LOG )
	Debug_Log( IO_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
#if(0)
/*********************************************************************************/
static int IO_WLG_Definition_Available( void )
{
	return -EC_IO_DEFINITION_NOT_FOUND;
}
/*********************************************************************************/
static int IO_WLG_Definition_Get( struct io_definition * io_definition )
{
	return -EC_IO_DEFINITION_NOT_FOUND;
}
#endif
/*********************************************************************************/
io_target_type io_target_definition =
{
	SYS_TYPE_MAKE(io_target_type),
	IO_WLG_Initialize,
	IO_WLG_Terminate,
	IO_WLG_Open,
	IO_WLG_Close,
	IO_WLG_Ioctl,
	IO_WLG_Interrupt_Enable,
	IO_WLG_Interrupt_Disable,
	NULL,		/*IO_WLG_Definition_Available, */
	NULL,		/*IO_WLG_Definition_Get,  */
#if defined( IO_DEBUG )
	IO_WLG_Validity,
#endif
	/* wlg = win32 library generic */
	IO_TARGET_NAME_SUFFIX( MODULE_IO_NAME, "wlg" ),
	IO_MODULE_REV,
	IO_COMMAND_REV,
	IO_STATUS_REV
};
