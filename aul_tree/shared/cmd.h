#ifndef MODULE_COMMAND
#define MODULE_COMMAND       2 /* module ID number */
#define MODULE_COMMAND_NAME "cmd"
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


/********************************************************************************/
#include "../shared/compiler.h"
#include "../shared/cmd_macros.h"

/********************************************************************************/
/* all command list references here */
#if defined( COMPILER_APPLICATION ) 
/* application or library space */
	#include "../components/cmp_avail_cmd.h"
	#include "../devices/dev_avail_cmd.h"
	#include "../io/io_cmd.h"
	#include "../acgr/acgr_cmd.h"
/* TEMPORARY */
#include "../auldrv/auldrv_cmd.h"

#else
/* kernel or driver space */
	#include "../components/cmp_avail_cmd.h"
	#include "../devices/dev_avail_cmd.h"
	#include "../io/io_cmd.h"
	#include "../auldrv/auldrv_cmd.h"
	/* #include "../acgr/acgr_cmd.h" */
#endif


/********************************************************************************/
/* Component command builder */
/* #define CMP_CMD(pid,cmd) IOC_CREATE_CMD( SYS_IOC_TYPE, SYS_CMD_TOTAL_QTY + (cmd), pid ) */

/********************************************************************************/
/* enumerate all of the error codes for this project. */
enum
{

#if defined( MODULE_SYS_COMMAND )
	MODULE_SYS_CMD_EC_DEFINITIONS( CMD_MOD_EXTRACT_ENUM )
#endif
/* 10 -- Array  */
#if defined( MODULE_ARRAY_COMMAND )
	MODULE_ARRAY_CMD_EC_DEFINITIONS( CMD_MOD_EXTRACT_ENUM )
#endif
/* 7 -- AULDRV - generic AUL driver    */
#if defined( MODULE_AULDRV_COMMAND )
	MODULE_AULDRV_CMD_DEFINITIONS( CMD_MOD_EXTRACT_ENUM )
#endif
/* 8 -- I/O  */
#if defined( MODULE_IO_COMMAND )
	MODULE_IO_CMD_DEFINITIONS( CMD_MOD_EXTRACT_ENUM )
#endif
/* 11 -- Component manager  */
#if defined( MODULE_CMPMGR_COMMAND )
	MODULE_CMPMGR_CMD_EC_DEFINITIONS( CMD_MOD_EXTRACT_ENUM )
#endif
/* 15 - Auto code generation and reporting  */
#if defined( MODULE_ACGR_COMMAND )
	MODULE_ACGR_CMD_DEFINITIONS( CMD_MOD_EXTRACT_ENUM )
#endif

/* All of the component level error codes are installed here 
    and these are found in cmp_avail_status.h
*/
#if defined ( COMPONENTS_AVAILABLE_COMMAND )
	COMPONENT_AVAIL_CMD_CODES
#endif

/* All of the device level error codes are installed here 
    and these are found in dev_avail_status.h
*/
#if defined ( DEVICES_AVAILABLE_COMMAND )
	DEVICE_AVAIL_CMD_CODES
#endif

	CMD_END_OF_LIST
};

#if(0)
/*******************************************************************************/
#if defined ( COMPONENTS_AVAILABLE_COMMAND )
	COMPONENT_AVAIL_CMD_BASE_CODES
#endif
/*******************************************************************************/
#if defined ( DEVICES_AVAILABLE_COMMAND )

	DEVICE_AVAIL_CMD_BASE_CODES
#endif
#endif

/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/


int Command_Initialize( void );

int Command_Terminate( void );

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )

size_t Command_All_Count( void );

int Command_CSV_Column_Name( COMMAND_CSV_COLUMN_ENUM column, char * buf_io, size_t buf_size );

int Command_CSV_Cell_Get( size_t row, COMMAND_CSV_COLUMN_ENUM column, char * cell_buf_io, size_t cell_buf_size );

int Command_CSV_Column_Width( COMMAND_CSV_COLUMN_ENUM column );

#endif

/*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* MODULE_COMMAND */
