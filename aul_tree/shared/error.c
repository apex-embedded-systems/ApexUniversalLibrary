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
#include "error.h"
#include "../shared/mem.h"			 /* memory allocation      */
#include "../shared/status.h"     /* error codes and APIs	*/
#include "../str_safe/str_safe.h"
/* #include "../shared/module.h" */    /* module information  	*/
#include "../shared/debug.h"      /* debugging support		*/

#include "../shared/module_macros.h"
#include "../shared/cmd_macros.h"
#if defined( __linux__ ) /* handle strerror function call   */
	#ifndef __KERNEL__
		#include "string.h"
	#endif
#endif
/*********************************************************************************/
#define ERROR_MODULE_REV  "$Date: 2015-01-27 17:28:47 -0600 (Tue, 27 Jan 2015) $"

/*********************************************************************************/
/* if DEBUG then allow IO_DEBUG */
#define ERROR_DEBUG		  _DEBUG
/*********************************************************************************/
#define ERROR_DEBUG_LOG   DEBUG_OUT
/*********************************************************************************/
/*TODO: semaphore or spinlock this??  */
static int error_last = SUCCESS;
#define ERROR_STRING_BUF_LENGTH  256
static char error_last_human_readable[ERROR_STRING_BUF_LENGTH] = { '\0' };
/*********************************************************************************/
void Error_Log( const char * module, const char * function, int line, int error_code )
{
	if ( SUCCESS == error_code ) return;
	error_last = error_code;
	if ( error_last < 0 ) error_last = -error_last; 
	String_Copy( error_last_human_readable, Status_EC_String( error_code ),  ERROR_STRING_BUF_LENGTH );
/*TODO: expand to include other locations.  */
#if defined( ERROR_DEBUG_LOG )
	Debug_Log( ERROR_DEBUG_LOG "module %s, function %s, line %d, error: %s\n", 
										 module,    function,    line,    error_last_human_readable ); 
#endif
}
/********************************************************************************
Description
If any error other than SUCCESS is reported, this will return non-zero.
*/
int Error_Last( int * error_code_out, const char ** message_out )
{
	int error_active;
	if ( error_last < 0 ) error_last = -error_last;
	/* tell caller that we have an active error, not SUCCESS */
	if ( error_last ) error_active = 1;
	else              error_active = 0;
	/* post current error */
	*error_code_out = error_last;
	*message_out    = error_last_human_readable;
	/* clear previous error */
	error_last						= SUCCESS;
	Mem_Set( error_last_human_readable, 0, ERROR_STRING_BUF_LENGTH );
	return error_active;
}
/*********************************************************************************/
int Error_Terminate( void )
{
	return SUCCESS;
}
/*********************************************************************************/
int Error_Initialize( void )
{
	error_last = SUCCESS;
	Mem_Set( error_last_human_readable, 0, ERROR_STRING_BUF_LENGTH );
	return SUCCESS;
}
/*********************************************************************************/
module_definition_type module_error_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	Error_Initialize,
	Error_Terminate,
	MODULE_ERROR_NAME,
	ERROR_MODULE_REV,
	NULL,
	NULL
};
