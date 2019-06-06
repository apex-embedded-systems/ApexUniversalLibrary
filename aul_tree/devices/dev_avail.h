#ifndef  MODULE_DEVICES_AVAILABLE
#define  MODULE_DEVICES_AVAILABLE

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

#include "../devmgr/devmgr.h"
#include "dev_avail_module.h"   /* DEVICE_LOGICAL_ENUM */

/***************************************************************************/
#define DEVICE_AVAILABLE_H_REV  "$Date: 2014-06-11 10:55:10 -0500 (Wed, 11 Jun 2014) $"
/***************************************************************************/


/*********************************************************************************/
typedef enum
{
	DEVICE_AVAILABLE_CSV_COLUMN_NONE			=  0,
	DEVICE_AVAILABLE_CSV_COLUMN_ENUM_VALUE		=  1,
	DEVICE_AVAILABLE_CSV_COLUMN_ENUM_SYMBOL		=  2,
	DEVICE_AVAILABLE_CSV_COLUMN_MODULE_NAME		=  3,
	DEVICE_AVAILABLE_CSV_COLUMN_DEVICE_ACRONYM	=  4,
	DEVICE_AVAILABLE_CSV_COLUMN_UNKNOWN			=  5
} DEVICE_AVAILABLE_CSV_COLUMN_ENUM;

/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/


/*******************************************************************/

int Device_Available_Find( IOC_T logical_id, device_definition_type ** definition_ptr_out );

size_t Device_Available_All_Count( void );

DEVICE_LOGICAL_ENUM Device_Available_Find_Logical_Type_Using_Logical_Name( const char * name_request );



#if defined( TARGET_AUTO_CODE_GEN_REPORTS )

int Device_Available_CSV_Column_Name( DEVICE_AVAILABLE_CSV_COLUMN_ENUM column, char * buf_io, size_t buf_size );

int Device_Available_CSV_Cell_Get( size_t row, DEVICE_AVAILABLE_CSV_COLUMN_ENUM column, char * cell_buf_io, size_t cell_buf_size );

int Device_Available_CSV_Column_Width( DEVICE_AVAILABLE_CSV_COLUMN_ENUM column );

#endif

/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/
#endif /* MODULE_DEVICES_AVAILABLE */
