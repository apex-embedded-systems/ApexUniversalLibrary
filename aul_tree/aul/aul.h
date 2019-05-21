#ifndef MODULE_AUL
#define MODULE_AUL       13   /* module ID number */
#define MODULE_AUL_NAME  "aul"
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


*********************************************************************************/
/**
 *  @defgroup general_libarary Library Functions
 *  General functions used to manage access to devices as well as error handling.
 */
#include "../shared/callconv.h"
#include "../shared/device_list.h"
#include "../devices/dev_avail.h"

/***************************************************************************/
#define MODULE_AUL_REV  "$Date: 2015-01-27 12:17:54 -0600 (Tue, 27 Jan 2015) $"
/***************************************************************************/

#if(0)
#include "../devmgr/devmgr.h"
#include "../shared/module_macros.h"
#endif
#if(0)
/*******************************************************************************/
#define AUL_DEVICE_ACRONYM_SIZE   IO_DEFINITION_ACRONYM_SIZE 
/*******************************************************************************/
struct aul_device
{
/*	AUL_DEVICE_ENUM	type;*/
	size_t			open_and_active;
	const char		acronym[AUL_DEVICE_ACRONYM_SIZE];
};
typedef struct aul_device  aul_device_type;
#endif

/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/

AUL_DLL_API int AULCALLCONV AUL_Device_Open( const char * device_acronym_name, int * board );

AUL_DLL_API int AULCALLCONV AUL_Device_Close( int board );

/* New */
AUL_DLL_API int AULCALLCONV AUL_Device_List_Get( device_list_type * device_list );

AUL_DLL_API int AULCALLCONV AUL_Device_Logical_Get( const char * device_acronym_name, 
													DEVICE_LOGICAL_ENUM * logical_id 
												  );

AUL_DLL_API int AUL_Is_Logical_Device( int board, DEVICE_LOGICAL_ENUM logical );


/* New */
AUL_DLL_API int AULCALLCONV AUL_Ioctl( int 				board,
									   unsigned int		command,
									   unsigned long	argument
									 );

AUL_DLL_API int AULCALLCONV AUL_Initialization( const char * hardware_definition_file_name );

AUL_DLL_API int AULCALLCONV AUL_Termination( void );

/* deprecate this function call */
AUL_DLL_API const char * AULCALLCONV AUL_Last_Error_Get( void );
/* Alias of AUL_Last_Error_Get() */
AUL_DLL_API const char * AULCALLCONV AUL_Error_Last_Get( void );

AUL_DLL_API const char * AULCALLCONV AUL_Error_String( int error_code );

AUL_DLL_API int AULCALLCONV AUL_Library_Revision( unsigned long * revision );  /* library revision */

AUL_DLL_API const char * AULCALLCONV AUL_Library_Name( void );

AUL_DLL_API const char * AULCALLCONV AUL_Driver_Name( void );

/* Returns zero if driver debug is not active.  Use this to warn users of
 * potential reduced throughput resulting from excessive debug posting.
 */
AUL_DLL_API int AULCALLCONV AUL_Driver_Is_Debug_Active( int * debug_is_active );

AUL_DLL_API int AULCALLCONV AUL_Library_Is_Debug_Active( int * debug_is_active );

AUL_DLL_API int AULCALLCONV AUL_Library_Process_Id_Get( unsigned long * pid );

/* deprecate this function call */
AUL_DLL_API int AULCALLCONV AUL_Revision_Sys_File( unsigned long * revision );
/* New Alias to AUL_Revision_Sys_File - this is a more general name */
AUL_DLL_API int AULCALLCONV AUL_Driver_Revision( unsigned long * revision );

/*******************************************************************************/
#ifdef __cplusplus
}
#endif



#endif  /* MODULE_AUL */

