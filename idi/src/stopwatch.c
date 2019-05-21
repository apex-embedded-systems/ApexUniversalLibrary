/****************************************************************************
	Apex Embedded Systems
	Stop Watch Functions
	
	Date:  March 31, 2003
	Updated: April 25, 2008
	
	Description:

	Produces Time Stamp Information.

	This is only accurate to 1 milli-second for DOS WIN32. 
	
	COPYRIGHT
	Copyright(c) 1999-2006 by Apex Embedded Systems.  
	All rights reserved.  This document is the 
	confidential property of Apex Embedded Systems
	Any reproduction or dissemination is prohibited 
	unless specifically authorized in writing.
  
****************************************************************************/

#if defined( __GNUC__ )
 #include "stopwatch.h"
#else
 #include "stopwatch.h"
#endif 

#if ( defined( __GNUC__ ) )
	static STOPWATCH_TIMEVAL_TYPE stopwatch_start[STOPWATCH_QUANTITY];
#elif ( defined(__BORLANDC__) || defined( _MSC_VER ) || defined(WIN32_DLL) || defined( _DLL ) )
	static long stopwatch_start[STOPWATCH_QUANTITY];  /* number of clock ticks */
#elif ( defined(_CC51) || defined(__C51__) || defined(__CX51__) )
#else
#endif

static char stopwatch_active[STOPWATCH_QUANTITY];
static STOPWATCH_TIMEVAL_TYPE stopwatch_result[STOPWATCH_QUANTITY];


/*************************************************************************
/													                       
*/
STOPWATCH_HANDLE_TYPE StopWatch_Open( void )
{
	STOPWATCH_HANDLE_TYPE handle;
	int x;
	
	handle = STOPWATCH_ERROR;

	for( x=0; x < STOPWATCH_QUANTITY; x++ )
	{	
		if ( stopwatch_active[x] == STOPWATCH_INACTIVE )
		{
			stopwatch_active[x] = x;
			handle = x;
			break;
		}
	}
	return( handle ); 	
}
/*************************************************************************
/													                       
*/
void StopWatch_Close( STOPWATCH_HANDLE_TYPE handle )
{
	stopwatch_active[handle] = STOPWATCH_INACTIVE;	
}
/*************************************************************************
/													                       
*/
void StopWatch_Set( STOPWATCH_HANDLE_TYPE handle, STOPWATCH_STATE_TYPE state )
{
#if ( defined( __GNUC__ ) )
	long lscratcha;
	struct timezone time_zone;
	STOPWATCH_TIMEVAL_TYPE  time_pause;
#elif ( defined(__BORLANDC__) || defined( _MSC_VER ) || defined(WIN32_DLL) || defined( _DLL ) )
	long lscratcha;
	long lscratchb;
#elif ( defined(_CC51) || defined(__C51__) || defined(__CX51__) )
#else
#endif
	switch( state )
	{
		case STOPWATCH_RESET: /* - - - - - - - - - - - - - - */
#if ( defined( __GNUC__ ) )
			stopwatch_result[handle].tv_usec = 0;
			stopwatch_result[handle].tv_sec = 0;
#elif ( defined(__BORLANDC__) || defined( _MSC_VER ) || defined(WIN32_DLL) || defined( _DLL ) )
			stopwatch_result[handle].tv_usec = 0;
			stopwatch_result[handle].tv_sec = 0;
#elif ( defined(_CC51) || defined(__C51__) || defined(__CX51__) )
#else
#endif
			break;
		case STOPWATCH_START: /* - - - - - - - - - - - - - - */	
#if ( defined( __GNUC__ ) )
			gettimeofday( &stopwatch_start[handle], &time_zone );
#elif ( defined(__BORLANDC__) || defined( _MSC_VER ) || defined(WIN32_DLL) || defined( _DLL ) )
			stopwatch_start[handle] = clock(); 
#elif ( defined(_CC51) || defined(__C51__) || defined(__CX51__) )
#else
#endif
			break;
		case STOPWATCH_PAUSE: /* - - - - - - - - - - - - - - */
#if ( defined( __GNUC__ ) )
			gettimeofday( &time_pause, &time_zone );
			/* sum microseconds */	
			stopwatch_result[handle].tv_usec = stopwatch_result[handle].tv_usec + time_pause.tv_usec - stopwatch_start[handle].tv_usec;
			if ( stopwatch_result[handle].tv_usec < 0 ) /* (pause_time - start_time) uSec underflow */ 
			{
				stopwatch_result[handle].tv_sec = stopwatch_result[handle].tv_sec - 1;
				stopwatch_result[handle].tv_usec = stopwatch_result[handle].tv_usec + 1000000;
			}
			else if ( stopwatch_result[handle].tv_usec > 1000000 ) /* (pause_time - start_time) uSec overflow */ 
			{
				lscratcha = ( stopwatch_result[handle].tv_usec / 1000000 ); /* seconds */
				stopwatch_result[handle].tv_sec  = stopwatch_result[handle].tv_sec + lscratcha; /* sum seconds */
				stopwatch_result[handle].tv_usec	= stopwatch_result[handle].tv_usec - ( lscratcha * 1000000 ); /* uSec */
			}
			/* sum seconds */
			stopwatch_result[handle].tv_sec = stopwatch_result[handle].tv_sec + time_pause.tv_sec - stopwatch_start[handle].tv_sec;
#elif ( defined(__BORLANDC__) || defined( _MSC_VER ) || defined(WIN32_DLL) || defined( _DLL ) )
			lscratcha = clock();
			lscratchb = ( ( lscratcha - stopwatch_start[handle] ) / CLK_TCK ); /* delta seconds */
			/* sum seconds */
			stopwatch_result[handle].tv_sec = stopwatch_result[handle].tv_sec + lscratchb;
			lscratchb = ( lscratcha - stopwatch_start[handle] ) - ( lscratchb * CLK_TCK ); /* delta clock ticks */
			lscratchb = ( lscratchb * 1000000L ) / CLK_TCK; /* delta microseconds */
			/* sum microseconds */
			stopwatch_result[handle].tv_usec = stopwatch_result[handle].tv_usec + lscratchb;
#elif ( defined(_CC51) || defined(__C51__) || defined(__CX51__) )
#else
#endif
			break;
	}
}
/*************************************************************************
/													                       
*/
STOPWATCH_TIMEVAL_TYPE * StopWatch_Value( STOPWATCH_HANDLE_TYPE handle )
{
	return ( &stopwatch_result[handle] );	
}

/*************************************************************************
/													                       
*/
void StopWatch_TimeStamp_String( char * return_string, int return_string_size )
{
/*
This function needs work, but it is good enough for now.
*/
#if ( defined( __GNUC__ ) )
	struct timezone time_zone;
	STOPWATCH_TIMEVAL_TYPE time_value;
	gettimeofday( &time_value, &time_zone );
	sprintf( return_string, "%ld_%ld", time_value.tv_sec, time_value.tv_usec );
#elif ( defined(__BORLANDC__) || defined( _MSC_VER ) || defined(WIN32_DLL) || defined( _DLL ) )
	long time_value;
	time_value = clock();
	sprintf( return_string, "%ld", time_value ); 
#elif ( defined(_CC51) || defined(__C51__) || defined(__CX51__) )
#else
#endif
}

/*************************************************************************
/													                       
*/
void StopWatch_Process( void )
{
	
}

/*************************************************************************
/													                       
*/
void StopWatch_Termination( void )
{
	
}

/*************************************************************************
/													                       
*/
void StopWatch_Initialization( void )
{
	int x;
	
	/* stopwatch initialization */
	for( x=0; x < STOPWATCH_QUANTITY; x++ ) 
	{
		stopwatch_active[x] = STOPWATCH_INACTIVE;
		stopwatch_result[x].tv_sec = 0;
		stopwatch_result[x].tv_usec = 0;
	}	
	
}

