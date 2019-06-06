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


*********************************************************************************/

#ifdef _WIN32
# include <windows.h>
# include <conio.h>  /* kbhit() and getch() */
#else
/* linux related */
# include <unistd.h> /* usleep */
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>



#include "../aul/aul_public.h"
#include "../str_safe/str_safe.h"
/* the following includes have been implemented to simplify this file */
#include "../demo_shared/cfgrprt.h"
#include "../demo_shared/config.h"
#include "../demo_shared/menu.h"
#include "../demo_shared/demo_tracere_shared.h"



/***************************************************************************/
#define DEMO_TRACERE_SHARED_REV  "$Date: 2014-05-06 17:06:00 -0500 (Tue, 06 May 2014) $"


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/*************** BEGIN -- MENU DISPLAY AND COMMAND PROCESSING **************/


/***************************************************************************/
/*
	            < < < <   	MENU COMMAND AND DISPLAY    > > > >
*/
/***************************************************************************/

/****  The struct menu_command_params can be found in menu.h */
#if(0)
/***************************************************************************/
int Demo_STX104_Menu_Frame_Max_Set( struct menu_command_params * p )
{
	struct demo_tracere_data * ds;  /* data set */
	uint32_t 	value;
	ds = (struct demo_tracere_data *) p->info;
	value = Menu_Ask_Uint32( p->in_file, p->out_file, "\n\nFrame count (0=manual start/stop)>" );
	if ( value > ( UINT32_MAX / 2 ) ) ds->software_frame_count_max = ( UINT32_MAX / 2 );
	else							  ds->software_frame_count_max = value;

	fprintf( p->out_file, "\n" );
	return SUCCESS;
}
/***************************************************************************/
int Demo_STX104_Menu_Trigger_Start( struct menu_command_params * p )
{
#if(0)
	struct demo_tracere_data * ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;
#endif
	STX104_Trg_Software_Start( p->board );
	return SUCCESS;
}
/***************************************************************************/
int Demo_STX104_Menu_Trigger_Stop( struct menu_command_params * p )
{
#if(0)
	struct demo_tracere_data * ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;
#endif
	STX104_Trg_Software_Stop( p->board );
	return SUCCESS;
}
#endif

/***************************************************************************/
int Demo_Tracere_Menu_Revisions( struct menu_command_params * p )
{
	int driver_compiled_with_debug_flag = 0;
	struct demo_tracere_data * ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;

	/* revision information pulled from stx104_simple global data structure */
	fprintf( p->out_file, " Library Revision:            %lu\n", ds->revision_library	);
	fprintf( p->out_file, " Driver Revision:             %lu",   ds->revision_driver 	);
	AUL_Driver_Is_Debug_Active( &driver_compiled_with_debug_flag );
	if ( driver_compiled_with_debug_flag ) fprintf( p->out_file, ", *** WARNING DEBUG ACTIVE ***\n" );
	else                                   fprintf( p->out_file, "\n" );

	fprintf( p->out_file, " Hardware Firmware Revision:  %04X", ds->revision_firmware 	);
	return SUCCESS;
}


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


/***************************************************************************/
/*
	           < < < <   	MENU DATA PRESENTATION    > > > >
*/
/***************************************************************************/

/***************************************************************************/
#define     TIMER_INDEX_MAX  3
static int  timer_index;
static char timer_tick[4] = { '/', '-', '\\', '|' };
/***************************************************************************/
char Demo_Tracere_Data_Timer_Tick_State( void )
{
	if ( timer_index == TIMER_INDEX_MAX ) timer_index = 0;
	else                                  timer_index++;
	return timer_tick[timer_index];
}
/***************************************************************************/
int Demo_Tracere_Data_Timer_Tick( struct menu_data_params * mdp )
{
	/* shows that the software is running */
	mdp->out_str[mdp->column] = Demo_Tracere_Data_Timer_Tick_State();
	mdp->column++;
	return SUCCESS;
}
#if(0)
/***************************************************************************/
int Demo_STX104_Data_DI( struct menu_data_params * mdp )
{
	struct demo_tracere_data * ds;
	BOOL value;
	int  channel;
	int  channel_count;

	ds = (struct demo_tracere_data *) mdp->info;
	channel_count = ((int) STX104_DI3) - ((int) STX104_DI0) + 1;

	/* if width too small to fit then just quit */
	if ( channel_count > mdp->width_max ) return 0;

	for ( channel  = ((int) STX104_DI3); channel >= ((int) STX104_DI0); channel-- )
	{
		STX104_DI_Get( ds->board, (STX104_DI_ENUM) channel, &value );
		if ( value )  Menu_Data_Append( mdp, "1" );
		else          Menu_Data_Append( mdp, "0" );
	}
	return SUCCESS;
}
/***************************************************************************/
int Demo_STX104_Data_Config_AI( struct menu_data_params * mdp )
{
	struct demo_tracere_data * ds;
	stx104_ai_cfg_type 				ai_cfg;
	STX104_GEN_JMP_TYPE 			jumpers;

	ds = (struct demo_tracere_data *) mdp->info;

	/* get the jumper values UNI/BIP, SE/DIF */
	STX104_Gen_Jumpers_Get( ds->board, &jumpers );

	if ( jumpers.J9_ai_unipolar ) Menu_Data_Append( mdp, "UNI" );
	else                          Menu_Data_Append( mdp, "BIP" );

	Menu_Data_Append( mdp, " " );

	if ( jumpers.J8_ai_differential ) Menu_Data_Append( mdp, "DIF" );
	else                              Menu_Data_Append( mdp, "SE " );

	Menu_Data_Append( mdp, " " );

	/* get the current ADC value */
	STX104_AI_CFG_Get( ds->board, &ai_cfg );
	switch( ai_cfg.gain )
	{
		case ACFG_GAIN_10V:	 	Menu_Data_Append( mdp, "  10V " ); break;
		case ACFG_GAIN_5V:	 	Menu_Data_Append( mdp, "   5V " ); break;
		case ACFG_GAIN_2_5V:	Menu_Data_Append( mdp, "2.5 V " ); break;
		case ACFG_GAIN_1_25V: 	Menu_Data_Append( mdp, "1.25V " ); break;
	}
	return SUCCESS;
}
/***************************************************************************/
int Demo_STX104_Data_Trig( struct menu_data_params * mdp )
{
	int 	error_code;
	BOOL   	trigger_enabled;
	struct demo_tracere_data * ds;

	ds 			= (struct demo_tracere_data *) mdp->info;
	error_code 	= STX104_Trg_Active_Get( ds->board, &trigger_enabled );
	if ( trigger_enabled ) Menu_Data_Append( mdp, "1" );
	else                   Menu_Data_Append( mdp, "0" );
	return SUCCESS;
}
/***************************************************************************/
int STX104_Data_Frames( struct menu_data_params * mdp )
{
#define STX104_FRAME_STR_SIZE  16
	struct demo_tracere_data * ds;
	/*int  					  frames;*/
	char 					  tmp_str[STX104_FRAME_STR_SIZE];

	ds = (struct demo_tracere_data *) mdp->info;

#if(0)
#if defined( STX104_SIMPLE_USE_INTERRUPTS )
	while ( ds->interrupt_blocking ) {}; /* wait for interrupt to complete.*/
#endif
#endif

	if ( ds->fifo_full_error )
	{
		String_Copy( tmp_str, "FIFO_FULL", STX104_FRAME_STR_SIZE );
	}
	else
	{
		snprintf( tmp_str, STX104_FRAME_STR_SIZE, "%8d", ds->software_frame_count );
	}
	Menu_Data_Append( mdp, tmp_str );
	return SUCCESS;
#undef STX104_FRAME_STR_SIZE
}
#endif

/***************** END -- MENU DISPLAY AND COMMAND PROCESSING ***************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/


/***************************************************************************/

/**************************************************************************/
int Demo_Tracere_Main_Loop_Timing_Calculations( unsigned int time_msec, struct demo_tracere_data * ds )
{
	double scratch;
	long int time_sec;
	ds->main_loop_time_sec  = ( (double) time_msec ) * 1.0e-3;  /* msec to sec 	*/
	ds->main_loop_time_usec = time_msec * 1000; 				/* msec to usec */
	time_sec = (long int) ds->main_loop_time_sec;
	ds->main_loop_time_spec.tv_sec = time_sec;
	scratch = ds->main_loop_time_sec - ( (double) time_sec );
	scratch = scratch * 1.0e9;  /* sec to nsec */
	ds->main_loop_time_spec.tv_nsec = (long) scratch;
	return SUCCESS;
}
/***************************************************************************/
void Demo_Tracere_Files_Close( struct demo_tracere_data * ds )
{
	if ( NULL != ds->data_output_file )
	{
		fclose( ds->data_output_file );
		ds->data_output_file = NULL;
	}
	if ( NULL != ds->fifo_log_file )
	{
		fclose( ds->fifo_log_file );
		ds->fifo_log_file = NULL;
	}
}
/***************************************************************************/
const char demo_data_file_name[]         = { DEMO_TRACERE_DATA_FILE_NAME ".bin" };
const char demo_data_file_name_csv[]     = { DEMO_TRACERE_DATA_FILE_NAME ".csv" };
const char demo_fifo_log_file_name[]     = { DEMO_TRACERE_FIFO_LOG_FILE_NAME ".bin" };
const char demo_fifo_log_file_name_csv[] = { DEMO_TRACERE_FIFO_LOG_FILE_NAME ".csv" };
/**********************************************************************//**
@brief
*/
int Demo_Tracere_Files_Open( const char * 				data_file_name,  	/* analog input data file */
							 const char * 				fifo_log_file_name, /* fifo log file          */
							 struct demo_tracere_data * ds
						   )
{
	if ( NULL == ds ) return -1; /* error codes? */

	if ( NULL == data_file_name ) data_file_name = demo_data_file_name;

#if defined( _WIN32 )
	error_code = fopen_s( &(ds->data_output_file), data_file_name, "wb" );
#else
	ds->data_output_file = fopen( data_file_name, "wb" );
#endif
	if ( NULL == ds->data_output_file )
	{
		fprintf( ds->out_file, "Cannot open '%s' file.\n", data_file_name );
		return -EC_APP_FILE_OPEN_ERROR;
	}

	if ( NULL == fifo_log_file_name ) fifo_log_file_name = demo_fifo_log_file_name;

#if defined( _WIN32 )
	error_code = fopen_s( &(ds->fifo_log_file), fifo_log_file_name, "wb" );
#else
	ds->fifo_log_file = fopen( fifo_log_file_name, "wb" );
#endif
	if ( NULL == ds->fifo_log_file )
	{
		fprintf( ds->out_file, "Cannot open '%s' file.\n", fifo_log_file_name );
		return -EC_APP_FILE_OPEN_ERROR;
	}
	ds->fifo_log_buffer_index = 0;
	return SUCCESS;
}
/**********************************************************************//**
@brief
Brings up the Tracer-E interface.  First it connects to the driver and then it
opens the board returning the board descriptor.

Note:
The error_codes "error_code" can be used during debug to see
what the functions are returning.
*/
int Demo_Tracere_Initialize(  char * prefix,    	/* output string prefix string  */
						 	  int argc,       		/* command line arguments count */
						 	  char* argv[],     	/* command line arguments list  */
						 	  struct demo_tracere_data * info  /* shared data */
                       	    )
{
	int   			error_code;
	int				board;
#if defined( DEMO_TRACERE_DEBUG )
	FILE *			out_file;
#else
	(void)			prefix;
#endif
	(void) argc;  /* not used currently */
	(void) argv;
#if defined( DEMO_TRACERE_DEBUG )
	out_file = info->out_file; /* short hand */
	fprintf( out_file, "%sInitialization begin:\n", prefix );
#endif
	/**** connect to driver ****/
#ifdef _WIN32
	error_code = AUL_Initialization( "aul_hwdef.txt" );
#else
	error_code = AUL_Initialization( NULL );
#endif
	if ( error_code ) return error_code;

#if defined( DEMO_TRACERE_DEBUG )
	fprintf( out_file, "%s  -> driver initialize success\n", prefix );
#endif
	error_code = AUL_Library_Revision( &(info->revision_library) );
#if defined( DEMO_TRACERE_DEBUG )
	fprintf( out_file, "%s  -> library revision = %lu\n", prefix, info->revision_library );
#endif
	error_code = AUL_Driver_Revision( &(info->revision_driver) );
#if defined( DEMO_TRACERE_DEBUG )
	fprintf( out_file, "%s  -> driver revision = %lu\n", prefix, info->revision_driver );
#endif

	error_code = AUL_Device_Open( "tracere", &board );
	if ( error_code ) return error_code;

	info->board = board;
#if defined( DEMO_TRACERE_DEBUG )
	fprintf( out_file, "%s  -> tracere device open successfully\n", prefix );
#endif
	error_code = STX104_Gen_Revision_Get( board, &(info->revision_firmware) );
#if defined( DEMO_TRACERE_DEBUG )
	fprintf( out_file, "%s  -> tracere_firmware_revision = %04X\n",
   												prefix, info->revision_firmware );
#endif
   return SUCCESS;
}
/**********************************************************************//**
@brief
Shuts down the STX104 interface.  First it turns off interrupts (if used) and
then it turns off the triggering (optional...will not affect shutting things
down).  Second, it closes the board descriptor and then frees the driver.

Note:
The error_codes "error_code" can be used during debug to see
what the functions are returning.
*/
void Demo_Tracere_Terminate( char * prefix,    			/* output string prefix string  */
                        	struct demo_tracere_data * ds  /* shared data 	*/
                          )
{
	int 			error_code; /* used with debugger */
	int            	board;
	FILE * 		   	out_file;
#ifndef DEMO_TRACERE_DEBUG
	(void) 			prefix;
#endif
	out_file = ds->out_file; /* short hand */
	board    = ds->board;  	/* short hand */
#if defined( DEMO_TRACERE_DEBUG )
	fprintf( out_file, "%sTermination begin:\n", prefix );
#endif
	/* set board to inactive state */
	error_code = STX104_Config_Termination( board, prefix, out_file, &(ds->cfg) );
	/* close the device */
	error_code = AUL_Device_Close( board );
#if defined( DEMO_TRACERE_DEBUG )
	fprintf( out_file, "%s  -> AUL_Device_Closed\n", prefix );
#endif
	/* close or free the driver */
	error_code = AUL_Termination();
#if defined( DEMO_TRACERE_DEBUG )
	fprintf( out_file, "%s  -> AUL_Termination\n", prefix );
#endif
	/* close any remaining open files */

	if ( NULL != ds->fifo_log_file )
	{
		fclose( ds->fifo_log_file );		ds->fifo_log_file = NULL;
	}
	if ( NULL != ds->data_output_file )
	{
		fclose( ds->data_output_file );		ds->data_output_file = NULL;
	}
	if ( NULL != ds->out_file )
	{
		fclose( ds->out_file );		ds->out_file = NULL;
	}
	if ( NULL != ds->in_file )
	{
		fclose( ds->in_file );		ds->in_file = NULL;
	}
}


