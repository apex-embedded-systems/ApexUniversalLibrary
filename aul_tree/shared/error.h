#ifndef MODULE_ERROR
#define MODULE_ERROR       5 /* module ID number */
#define MODULE_ERROR_NAME "error"
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
    
	COPYRIGHT NOTICE Copyright (c) 20140-2015 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded 
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


*********************************************************************************/

#include "../shared/module_macros.h"

/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/

void Error_Log( const char * module, const char * function, int line, int error_code );

int Error_Last( int * error_code_out, const char ** message_out );

int Error_Initialize( void );

int Error_Terminate( void );

extern module_definition_type module_error_definition;

/*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* MODULE_ERROR */
