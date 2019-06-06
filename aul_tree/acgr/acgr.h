#ifndef MODULE_ACGR
#define MODULE_ACGR       15   /* module ID number */
#define MODULE_ACGR_NAME  "acgr"

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


#include "../shared/callconv.h"
#include "../shared/module_macros.h"

/* NOTE:  ACGR = Auto Code Generation and Reporting */
/***************************************************************************/
#define ACGR_H_REV  "$Date: 2014-06-11 10:55:10 -0500 (Wed, 11 Jun 2014) $"
/***************************************************************************/


/*********************************************************************************/
struct aul_integrity
{
	size_t			id;
	size_t			param;
	uint32_t			module_time_stamp;	/* 0 = not populated	*/ 
	uint32_t			command_time_stamp;	/* 0 = not populated */
	uint32_t			status_time_stamp;	/* 0 = not populated */
};
typedef struct aul_integrity aul_integrity_type;

/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/

AUL_DLL_API int AULCALLCONV ACGR_Report_CSV_Module( size_t column_names_include, const char * file_name );

AUL_DLL_API int AULCALLCONV ACGR_Report_CSV_Command( size_t column_names_include, const char * file_name );

AUL_DLL_API int AULCALLCONV ACGR_Report_CSV_Status( size_t column_names_include, const char * file_name );

AUL_DLL_API int AULCALLCONV ACGR_Source_File_Command_Enumeration( const char * file_name );

AUL_DLL_API int AULCALLCONV ACGR_Source_File_Status_Enumeration( const char * file_name );

AUL_DLL_API int AULCALLCONV ACGR_Source_File_Integrity( const char * file_name );

AUL_DLL_API int AULCALLCONV ACGR_Integrity_Test( const aul_integrity_type * integrity_list );

AUL_DLL_API int AULCALLCONV ACGR_Source_File_Device_Logical_Enumeration( const char * file_name );

extern module_definition_type module_acgr_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/
#endif /* MODULE_ACGR */
