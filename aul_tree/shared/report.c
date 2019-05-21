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
    
	COPYRIGHT NOTICE Copyright (c) 2014-2015 by Apex Embedded Systems. All rights 
	reserved. This document is the confidential property of Apex Embedded 
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


The report must become independent of Debug.


*********************************************************************************/
#include "report.h"
//#include "../str_safe/str_safe.h"
//#include "../shared/module_macros.h"
//#include "../shared/cmd_macros.h"
#include "../shared/status.h"     /* error codes and APIs		*/

#ifndef DEBUG_KERNEL
#	include <string.h>
#	include <stdio.h>
#endif

/*********************************************************************************/
#define REPORT_MODULE_REV  "$Date: 2015-01-27 17:28:47 -0600 (Tue, 27 Jan 2015) $"

/*********************************************************************************/
/* if DEBUG then allow IO_DEBUG */
#define REPORT_DEBUG		  	_DEBUG
/*********************************************************************************/
#define REPORT_DEBUG_LOG   		DEBUG_OUT

/*******************************************************************************/
#if defined( REPORT_KERNEL )
 static void Report_VA_Helper( const char * format, va_list args )
#else
 static void Report_VA_Helper( FILE * fd, const char * format, va_list args )
#endif
{
#if ( defined( WIN32 ) && defined( _MSC_VER ) )
	vfprintf( fd, format, args );
#elif defined( __MSDOS__ )
 	vfprintf( fd, format, args );
#elif defined( __linux__ )
# ifdef __KERNEL__
	/*TODO: vprintk( KERN_DEBUG "" format, args );*/
	vprintk( format, args );
# else
	vfprintf( fd, format, args );
# endif
#elif defined( __GNUC__ )
# ifdef __KERNEL__
	/*TODO: vprintk( KERN_DEBUG "" format, args );*/
	vprintk( format, args );
# else
	vfprintf( fd, format, args );
# endif
#else
# error Report: No compatible OS or target
#endif
}

/**********************************************************************************************************//**
@ingroup report
\brief
Apex Universal Library function to output prefix (first part of line) of report.

\param[out] out  		The destination of the report.  Can be 'stdout' or any file.
\param[in] list  		Indentation and any prefix verbage required for this report.
*/
#if defined( DEBUG_KERNEL )
 static void Report_Prefix( struct report_prefix_list * list )
#else
 static void Report_Prefix( FILE * out, struct report_prefix_list * list )
#endif
{
	if ( NULL == list         ) return;
	if ( NULL == list->prefix ) return;
	do
	{
#if defined( DEBUG_KERNEL )
		Report_VA_Helper( DEBUG_OUT "%s", list->prefix );
#else
		fprintf( out, "%s", list->prefix );
#endif
		list = list->next;
	} while ( NULL != list );
}

/**********************************************************************************************************//**
@ingroup report
\brief
Apex Universal Library function to output prefix (first part of line) of report.  If KERNEL then
we can only write to dmesg buffer.  If user level, then we can write to either dmesg.txt or to a
separate file or console.

\param[out] out  		The destination of the report.  Can be 'stdout' or any file.
\param[in] list  		Indentation and any prefix verbage required for this report.
*/
#if defined( DEBUG_KERNEL )
 void Report_Write( struct report_prefix_list * list, char * format, ... )
#else
 void Report_Write( FILE * out, struct report_prefix_list * list, char * format, ... )
#endif
{
	va_list		args;
	va_start( args, format );
#if defined( DEBUG_KERNEL )
	Debug_Log_VA( format, va_list );
#else
	if ( NULL == out )
	{
		Debug_Log_VA( out, format, args );
		//Debug_Log_VA( out, DEBUG_OUT format, args );
	}
	else
	{
		Report_Prefix( out, list );
		vfprintf( out, format, args );
	}
#endif
	va_end( args );
}
/*********************************************************************************/
int Report_Terminate( void )
{
	return SUCCESS;
}
/*********************************************************************************/
int Report_Initialize( void )
{
	return SUCCESS;
}
/*********************************************************************************/
module_definition_type module_report_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	Report_Initialize,
	Report_Terminate,
	MODULE_REPORT_NAME,
	REPORT_MODULE_REV,
	NULL,
	NULL
};
