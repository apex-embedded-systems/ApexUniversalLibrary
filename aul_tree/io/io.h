#ifndef MODULE_IO
#define MODULE_IO      8 /* module ID number */
#define MODULE_IO_NAME "io"
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

/*******************************************************************************/
#include "io_shared.h"
/* #include "../shared/mem.h" */
#include "../shared/debug.h"      /* debugging support						*/
#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/cmd_macros.h"  /* IOC_T */
/*#include "../shared/sysval.h"  */  /* SYS_TYPE */

#include "../shared/module_macros.h"

/*********************************************************************************/
/* if DEBUG then allow IO_DEBUG */
#define IO_DEBUG		  _DEBUG
/*********************************************************************************/
#define IO_DEBUG_LOG   DEBUG_OUT


/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************/


int IO_Read_U8( int iod, size_t virtual_address, uint8_t * value );

int IO_Read_U16( int iod, size_t virtual_address, uint16_t * value );

int IO_Read_Repeat_U8( int iod, size_t virtual_address, uint8_t * buffer, size_t count );

int IO_Read_Repeat_U16( int iod, size_t virtual_address, uint16_t * buffer, size_t count );
	
	
int IO_Write_U8( int iod, size_t virtual_address, uint8_t  value );

int IO_Write_U16( int iod, size_t virtual_address, uint16_t  value );

int IO_Write_Repeat_U8( int iod, size_t virtual_address, uint8_t * buffer, size_t count );

int IO_Write_Repeat_U16( int iod, size_t virtual_address, uint16_t * buffer, size_t count );

int IO_Xfr( int iod, io_xfr_type * xfr );

IO_ID_ENUM IO_ID_Get( void );

/*******************************************************************************/
int IO_Interrupt_Disable( int iod );

int IO_Interrupt_Enable( int iod, io_int_handler handler, void * private_data );

/*******************************************************************************/

int IO_Open( int *				  iod,	  /* return device index */
			 io_definition_type * io_definition
		   );

int IO_Is_Definition_Available( void );

int IO_Definition_Get( io_definition_type * io_params );

int IO_Close( int * iod );

int IO_Error_Last_Get( void );

int IO_Ioctl( int iod, unsigned int command, unsigned long argument );

/*******************************************************************************/

int IO_Version( uint32_t * version );

int IO_Terminate( void );

int IO_Initialize( void );

/** Internal use only */
#if defined( IO_DEBUG )
 int IO_IsNotValid( int iod, size_t virtual_address, size_t data_bytes );
#endif

extern module_definition_type module_io_definition;

/*******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* IO_MODULE */

