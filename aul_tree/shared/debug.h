#ifndef MODULE_DEBUG
#define MODULE_DEBUG       3   /* module ID number */
#define MODULE_DEBUG_NAME  "debug"
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

#include "../shared/compiler.h"
#include "../shared/module_macros.h"

/********************************************************************************/
/* #define DEBUG_PREPEND_TIMESTAMP  1  */
#define DEBUG_PREPEND_DEBUG      1
/********************************************************************************/
#define DEBUG_CONSOLE   "<e>"
#define DEBUG_FILE	    "<f>"

#define DEBUG_OUT			DEBUG_FILE

/*******************************************************************************/
#if defined( COMPILER_APPLICATION )
# pragma message  ( "aul_tree: debug.c COMPILER_APPLICATION" )
# include <stdio.h>
# include <stdarg.h>
#elif defined( __linux__ ) && defined( __KERNEL__ )
# define DEBUG_KERNEL 1
# pragma message  ( "aul_tree: debug.c __KERNEL__ DEBUG_KERNEL=1" )
# include <linux/kernel.h>
/*# include <linux/module.h>*/
#endif

/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/

void Debug_Log( const char * format, ... );

#if defined( DEBUG_KERNEL )
	void Debug_Log_VA( const char * format, va_list args );
#else
	void Debug_Log_VA( FILE * fd, const char * format, va_list args );
#endif

int Debug_Initialize( void );

int Debug_Terminate( void );

extern module_definition_type module_debug_definition;

/*******************************************************************************/
#ifdef __cplusplus
}
#endif


#endif /* MODULE_DEBUG */
