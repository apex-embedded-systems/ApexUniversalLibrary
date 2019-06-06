#ifndef MODULE_STATUS
#define MODULE_STATUS       2 /* module ID number */
#define MODULE_STATUS_NAME "status"
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
#include "status_macros.h"

/********************************************************************************/
/* List status headers here */
#if(0)
#if defined( COMPILER_APPLICATION ) 
/* application or library space */
	#include "../io/io_status.h"
	#include "../cmpmgr/cmpmgr_status.h"
	#include "../devmgr/devmgr_status.h"
	#include "../components/cmp_avail_status.h"
	#include "../devices/dev_avail_status.h"
	#include "../aul/aul_status.h"
	#include "../aul/aul_hwdef_status.h"
	#include "../acgr/acgr_status.h"
	#include "../shared/mem_status.h"
	#include "../shared/module_macros.h"

#else
/* kernel or driver space */
	#include "../io/io_status.h"
	#include "../auldrv/auldrv_status.h"
	/* #include "../cmpmgr/cmpmgr_status.h" */
	/* #include "../devmgr/devmgr_status.h" */
	/* #include "../components/cmp_avail_status.h" */
	/* #include "../devices/dev_avail_status.h" */
	/* #include "../aul/aul_status.h" */
	/* #include "../aul/aul_hwdef_status.h" */
	/* #include "../acgr/acgr_status.h" */
	#include "../shared/module_macros.h"
#endif
#endif

#include "../io/io_status.h"
#include "../cmpmgr/cmpmgr_status.h"
#include "../devmgr/devmgr_status.h"
#include "../components/cmp_avail_status.h"
#include "../devices/dev_avail_status.h"
#include "../shared/mem_status.h"
#include "../shared/module_macros.h"

/* TEMPORARY */
#include "../auldrv/auldrv_status.h"

#if defined( COMPILER_APPLICATION )
	#include "../aul/aul_status.h"
	#include "../aul/aul_hwdef_status.h"
	#include "../acgr/acgr_status.h"
	#include "../application/app_status.h"
#else
	#include "../auldrv/auldrv_status.h"
#endif


/********************************************************************************/
/* enumerate all of the error codes for this project. */
enum
{
/*
 ASSUMPTION: all OS error codes fit within the first 16-bits of the error code.
 OS error codes not shown in this enumeration but sit below any of the numbers
  generated here.
 Windows:
    http://msdn.microsoft.com/en-us/library/windows/desktop/ms679360%28v=vs.85%29.aspx
    http://msdn.microsoft.com/en-us/library/windows/desktop/ms681381%28v=vs.85%29.aspx
 Linux:  /usr/include/asm/errno.h
    http://www.gnu.org/software/libc/manual/html_node/Error-Codes.html
    http://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/unix_system_errors.html
*/

#if defined( MODULE_SYS_STATUS )
	MODULE_SYS_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
/* 4 -- Memory    */
#if defined( MODULE_MEM_STATUS )
	MODULE_MEM_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
/* 10 -- Array  */
#if defined( MODULE_ARRAY_STATUS )
	MODULE_ARRAY_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
/* 7 -- AULDRV - generic AUL driver    */
#if defined( MODULE_AULDRV_STATUS )
	MODULE_AULDRV_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
/* 8 -- I/O    */
#if defined( MODULE_IO_STATUS )
	MODULE_IO_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
/* 11 -- Component manager    */
#if defined( MODULE_CMPMGR_STATUS )
	MODULE_CMPMGR_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
/* 12 -- Component manager   */
#if defined( MODULE_DEVMGR_STATUS )
	MODULE_DEVMGR_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
/* 13 - NOTE: AUL Library interface  */
#if defined( MODULE_AUL_STATUS )
	MODULE_AUL_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
/* 14 - AUL Hardware definition file import*/
#if defined( MODULE_AULHWDEF_STATUS )
	MODULE_AULHWDEF_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
/* 15 - Auto code generation and reporting  */
#if defined( MODULE_ACGR_STATUS )
	MODULE_ACGR_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif

/* All of the component level error codes are installed here 
    and these are found in cmp_avail_status.h
*/
#if defined ( COMPONENTS_AVAILABLE_STATUS )
	COMPONENT_AVAIL_STATUS_CODES
#endif

#if defined ( DEVICES_AVAILABLE_STATUS )
	DEVICE_AVAIL_STATUS_CODES
#endif

/*
#if defined( MODULE_I8255M0_STATUS )
	MODULE_I8255M0_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif

#if defined( MODULE_DAC6CH_STATUS )
	MODULE_DAC6CH_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
*/

#if defined ( APPLICATION_STATUS )
	APPLICATION_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
#endif
	EC_END_OF_LIST
};

/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/

const char * Status_EC_String( int error_code );

size_t Status_All_Count( void );

int Status_Initialize( void );

int Status_Terminate( void );

extern module_definition_type module_status_definition;

#if defined( TARGET_AUTO_CODE_GEN_REPORTS )

int Status_CSV_Column_Name( STATUS_CSV_COLUMN_ENUM column, char * buf_io, size_t buf_size );

int Status_CSV_Cell_Get( size_t row, STATUS_CSV_COLUMN_ENUM column, char * cell_buf_io, size_t cell_buf_size );

int Status_CSV_Column_Width( STATUS_CSV_COLUMN_ENUM column );

#endif

/*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* MODULE_STATUS */
