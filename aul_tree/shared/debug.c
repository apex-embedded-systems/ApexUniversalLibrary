
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


#include "debug.h"
#include "../shared/status.h"     /* error codes and APIs		*/

#ifdef DEBUG_PREPEND_DEBUG
	#if defined( DEBUG_KERNEL )
		/*   automatically done in printk() dmesg buffer */
		#undef DEBUG_PREPEND_TIMESTAMP
	#elif defined( DEBUG_PREPEND_TIMESTAMP )
		#pragma message ( "aul_tree: debug.c DEBUG_PREPEND_TIMESTAMP" )
		#include <time.h>
	#endif
#endif

/* #include "../shared/cmd_macros.h" */
/*********************************************************************************/
#define DEBUG_MODULE_REV  "$Date: 2015-02-05 06:26:19 -0600 (Thu, 05 Feb 2015) $"

/*******************************************************************************/
#if defined( DEBUG_KERNEL )
 static void Debug_Log_VA_Helper( const char * format, va_list args )
#else
 static void Debug_Log_VA_Helper( FILE * fd, const char * format, va_list args )
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
# error Debug: No compatible OS or target
#endif
}
/*******************************************************************************/
#if defined( DEBUG_KERNEL )
 static void Debug_Prepend_Helper( const char * format, ... )
#else
 static void Debug_Prepend_Helper( FILE * fd, const char * format, ... )
#endif
{
	va_list  args;
	va_start( args, format );
#if defined( DEBUG_KERNEL )
	Debug_Log_VA_Helper( format, args );
#else
	Debug_Log_VA_Helper( fd, format, args );
#endif
	va_end( args );
}
/**********************************************************************************************************//**
@ingroup debug
\brief
Apex Universal Library function to record debug information
*/
#if defined( DEBUG_KERNEL )
 void Debug_Log_VA( const char * format, va_list args )
#else
 void Debug_Log_VA( FILE * fd, const char * format, va_list args )
#endif
{
	size_t		 index;
	const char * fmt;
	size_t		 append_to_file;

	/**********************************
	 *  find string length of format
	 */
	index = 0;
	fmt = format;
	while ( *fmt )
	{
		fmt++; index++;
	}

	/**********************************
	 * Determine where debug information is to be delivered.
	 * If debugging kernel (i.e. kprintf() ) then just strip off
	 * the apex defined prefix <?>.
	 */
#if !defined( DEBUG_KERNEL )
	fd = stdout;
#endif
	fmt = format;
	append_to_file = 0; /* we are not appending to a file */
	if ( index >= 3 )
	{
		if ( (format[0] == '<') && (format[2] == '>') )
		{
			switch( format[1] )
			{
				case 'e':  /* console */
#if !defined( DEBUG_KERNEL )
					fd = stdout;
#endif
					fmt = &(format[3]);
					break;
				case 'f':  /* append to file */
#if !defined( DEBUG_KERNEL )
					fd = fopen( "dmesg.txt", "at" );
					if ( fd )
					{
						append_to_file = 1;
					}
					else
					{
						fd = stdout;
					}
#endif
					fmt = &(format[3]);
					break;
			}
		}
	}

	/**********************************
	 * Prepend any required information.
	 * 1. "DEBUG"
	 * 2. Time stamp
	 */
#ifdef DEBUG_PREPEND_DEBUG
	/* prepend "DEBUG" */
	#if defined( DEBUG_KERNEL )
		Debug_Prepend_Helper( "DEBUG" );
	#else
		Debug_Prepend_Helper( fd, "DEBUG" );
	#endif

	/* prepend time stamp */
	#ifdef DEBUG_PREPEND_TIMESTAMP
		{
			struct tm * time_block;
			time_t		time_current;
			char *      time_ascii;
			size_t		index;

			time_current = time( NULL );
		/*TODO: replace localtime() with a reentrant version */
			time_block   = localtime( &time_current );
		/*TODO: replace asctime() with a reentrant version thereof  asctime_r() is a typical name.*/
		/*TODO: don't like the format!*/
			time_ascii   = asctime( time_block );
			index = 0;
			while( time_ascii[index] != '\n' ){ index++; }
			time_ascii[index] = '\0';

			Debug_Prepend_Helper( fd, " %s", time_ascii );
		}
	#endif /* DEBUG_PREPEND_TIMESTAMP */
	#if defined( DEBUG_KERNEL )
		Debug_Prepend_Helper( ": " );
	#else
		Debug_Prepend_Helper( fd, ": " );
	#endif
#endif /* DEBUG_PREPEND_DEBUG */


#if defined( DEBUG_KERNEL )
		Debug_Log_VA_Helper( fmt, args );
#else
		Debug_Log_VA_Helper( fd, fmt, args );
#endif

#if !defined( DEBUG_KERNEL )
		if ( append_to_file ) fclose( fd );
#endif
}
/**********************************************************************************************************//**
@ingroup debug
\brief
Apex Universal Library function to record debug information
*/
void Debug_Log( const char * format, ... )
{
	va_list		args;
#ifndef _DEBUG
	/* If not in debug mode, then we do nothing! */
	(void) 		format;
#else
	va_start( args, format );
 #if defined( DEBUG_KERNEL )
	Debug_Log_VA( format, args );
 #else
	FILE *		fd = stdout;
	Debug_Log_VA( fd, format, args );
 #endif
	va_end( args );
#endif
}

#if(0)
/**********************************************************************************************************//**
@ingroup debug
\brief
Apex Universal Library function to record debug information
*/
void Debug_Log( const char * format, ... )
{
	va_list		args;
#ifndef _DEBUG
	(void) 		format;
#else

 #if !defined( DEBUG_KERNEL )
	FILE *		fd;
 #endif
	size_t		 index;
	const char * fmt;
	size_t		 append_to_file;

	/* find string length of format */
	index = 0;
	fmt = format;
	while ( *fmt ) 
	{
		fmt++; index++;
	}

	/* defaults */
/* If debugging kernel (i.e. kprintf() ) then just strip off 
	the apex defined prefix <?>.  
*/
 #if !defined( DEBUG_KERNEL )
	fd = stdout;
 #endif

	fmt = format;
	append_to_file = 0; /* we are not appending to a file */
	if ( index >= 3 )
	{
		if ( (format[0] == '<') && (format[2] == '>') )
		{
			switch( format[1] )
			{
				case 'e':  /* console */
 #if !defined( DEBUG_KERNEL )
					fd = stdout;
 #endif
					fmt = &(format[3]);
					break;
				case 'f':  /* append to file */
 #if !defined( DEBUG_KERNEL )
					fd = fopen( "dmesg.txt", "at" );
					if ( fd ) 
					{
						append_to_file = 1;
					}
					else
					{
						fd = stdout;
					} 
 #endif
					fmt = &(format[3]);
					break;
			}
		}
	}

/****************************************/
 #ifdef DEBUG_PREPEND_DEBUG
	#if defined( DEBUG_KERNEL )
		Debug_Log_Helper( "DEBUG" );
	#else
		Debug_Log_Helper( fd, "DEBUG" );
	#endif

	#ifdef DEBUG_PREPEND_TIMESTAMP
	{
		struct tm * time_block;
		time_t		time_current;
		char *      time_ascii;
		size_t		index;

		time_current = time( NULL );
	/*TODO: replace localtime() with a reentrant version */
		time_block   = localtime( &time_current );
	/*TODO: replace asctime() with a reentrant version thereof  asctime_r() is a typical name.*/
	/*TODO: don't like the format!*/
		time_ascii   = asctime( time_block );
		index = 0;
		while( time_ascii[index] != '\n' ){ index++; }
		time_ascii[index] = '\0';

		Debug_Log_Helper( fd, " %s", time_ascii );
	}
	#endif /* DEBUG_PREPEND_TIMESTAMP */
	#if defined( DEBUG_KERNEL )
		Debug_Log_Helper( ": " );
	#else
		Debug_Log_Helper( fd, ": " );
	#endif
 #endif /* DEBUG_PREPEND_DEBUG */
/****************************************/


	va_start( args, format );
 #if defined( DEBUG_KERNEL )
	Debug_Log_VA( fmt, args );
 #else
	Debug_Log_VA( fd, fmt, args );
 #endif
	va_end( args );
 #if !defined( DEBUG_KERNEL )
	if ( append_to_file ) fclose( fd );
 #endif

#endif /* _DEBUG */
}
#endif

/*********************************************************************************/
int Debug_Initialize( void )
{
	return SUCCESS;
}
/*********************************************************************************/
int Debug_Terminate( void )
{
	return SUCCESS;
}
/*********************************************************************************/
module_definition_type module_debug_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	Debug_Initialize,
	Debug_Terminate,
	MODULE_DEBUG_NAME,
	DEBUG_MODULE_REV,
	NULL,
	NULL
};
