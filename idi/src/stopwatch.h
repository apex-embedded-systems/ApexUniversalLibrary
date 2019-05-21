/*

	DESCRIPTION

	TO DO



  COPYRIGHT
	Copyright(c) 1999-2008 by Apex Embedded Systems.  
	All rights reserved.  This document is the 
	confidential property of Apex Embedded Systems
	Any reproduction or dissemination is prohibited 
	unless specifically authorized in writing.
*/
#ifndef  __STOPWATCH_H__
#define  __STOPWATCH_H__

#ifdef __cplusplus
extern "C" {
#endif

//temporary - begin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//temporary - end

#include <time.h>

#if ( defined( __GNUC__ ) )
 #include <sys/time.h>
 #define STOPWATCH_HANDLE_TYPE  size_t
 #define STOPWATCH_STATE_TYPE   int
#elif ( defined(__BORLANDC__) || defined( _MSC_VER ) || defined(WIN32_DLL) || defined( _DLL ) )
	struct timeval {
		long    tv_sec;         /* seconds */
		long    tv_usec;        /* microseconds */
	};
	#define STOPWATCH_HANDLE_TYPE  size_t
  #define STOPWATCH_STATE_TYPE   char
#elif ( defined(_CC51) || defined(__C51__) || defined(__CX51__) )
#else
#endif

typedef struct timeval STOPWATCH_TIMEVAL_TYPE;



#define STOPWATCH_INACTIVE  -1
#define STOPWATCH_ERROR     -1
#define STOPWATCH_QUANTITY   4

#define STOPWATCH_RESET      0
#define STOPWATCH_START      1
#define STOPWATCH_PAUSE      2
 

extern STOPWATCH_HANDLE_TYPE StopWatch_Open( void );
extern void StopWatch_Close( STOPWATCH_HANDLE_TYPE handle );
extern void StopWatch_Set( STOPWATCH_HANDLE_TYPE handle, STOPWATCH_STATE_TYPE state );
extern STOPWATCH_TIMEVAL_TYPE * StopWatch_Value( STOPWATCH_HANDLE_TYPE handle );
extern void StopWatch_TimeStamp_String( char * return_string, int return_string_size );
extern void StopWatch_Process( void );
extern void StopWatch_Termination( void );
extern void StopWatch_Initialization( void );

#ifdef __cplusplus
}
#endif

#endif /* __STOPWATCH_H__ */
