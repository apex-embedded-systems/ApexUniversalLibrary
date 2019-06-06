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
#include "../demo_shared/demo_stx104_shared.h"



/***************************************************************************/
#define DEMO_STX104_SHARED_REV  "$Date: 2014-05-06 17:06:00 -0500 (Tue, 06 May 2014) $"

#if(0)
/***************************************************************************

Initialization sequence:
	1. turn off interrupts first, just in case they are active.
	2. initialize triggering, make sure triggering has been stopped.
	3. Initialize analog section
	4. Initialize digital I/O section

Admittedly, there is no error handling here.  The purpose is to illustrate
the main points of initialization.
*/
static void Board_Initialization( struct demo_stx104_data * ds )
{
	int   				error_code;  /* for debug */
	stx104_ai_cfg_type  ai_cfg;
	STX104_DI_CFG_TYPE  di_cfg;
	/*STX104_DO_CFG_TYPE  do_cfg;*/
	STX104_TRG_CFG_TYPE trg_cfg;
	STX104_INT_CFG_TYPE int_cfg;
	int					board;

	board = ds->board;


   /* interrupts */
	memset( &int_cfg, 0, sizeof( STX104_INT_CFG_TYPE ) );
	error_code = STX104_Int_CFG_Set( board, &int_cfg );
	error_code = STX104_Int_Enable_Set( board, false );
	/* triggering   */
	error_code = STX104_Trg_Software_Stop( board );
	memset( &trg_cfg, 0, sizeof( STX104_TRG_CFG_TYPE ) );
	trg_cfg.enabled           = true;
	trg_cfg.start_mode        = STM_IMMEDIATE;
	trg_cfg.start_sync_source = TSS_NONE;
	trg_cfg.start_source      = STS_SOFTWARE_STROBE; /* start via software */
	trg_cfg.stop_source       = ETS_SOFTWARE_STROBE; /* stop via software */
	error_code = STX104_Trg_CFG_Set( board, &trg_cfg );
	/* make sure trigger is now inactive/off */
	error_code = STX104_Trg_Software_Stop( board );
	/* analog inputs */
	memset( &ai_cfg, 0, sizeof( stx104_ai_cfg_type ) ); /* clear configuration */
	ai_cfg.gain           = ACFG_GAIN_10V;
	ai_cfg.burst_timer_ns = 5000; /* nS */
	ai_cfg.frame_timer_ns = AI_FRAME_TIME_NS;
	ai_cfg.strobe_source  = AISS_AI_FRAME_TIMER;
	ai_cfg.first_channel  = AI_FIRST_CHANNEL;
	ai_cfg.last_channel   = AI_LAST_CHANNEL;
	ai_cfg.use_triggering_subsystem  = true;
	ai_cfg.high_speed_pre_queue_fifo = true;
#if defined( CONVERT_STRAIGHT_BINARY_TO_TWOS_COMPLEMENT )
	if ( ds->stx104_firmware_revision >= STX104_REVISION_120328 )
	{ /* let the hardware do the conversion! */
	ds->ai_cfg_encoding = AI_ENCODING_TWOS_COMPLEMENT;
	}
#endif
	error_code = STX104_AI_CFG_Set( board, &ai_cfg ); /* update configuration to board */
	/* digital inputs */
	memset( &di_cfg, 0, sizeof( STX104_DI_CFG_TYPE ) );
	di_cfg.swap_din1_and_din3 = true;
	error_code = STX104_DI_CFG_Set( board, &di_cfg );
}
#endif


#if(0)
/***************************************************************************/
/*
	Read back the existing values programmed into the hardware and then
	modify accordingly.  In some cases one might need to reset all the
	values entirely.  The assumption here is that the hardware is starting
	up from a cold boot (i.e. hardware reset), or we had quit running
	the stx104_simple application and are restarting it.

	By reading back the existing values that have been programmed into the
	hardware, if one stops the program and restarts the program, the analog
	input channel (for example) will remain unchanged.
*/
BOOL Menu_Board_Initialization( struct demo_stx104_data * ds )
{
	;  /* data set */
	double frames_per_second;
	double frames_per_file_write;
	double samples_per_file_write;
	FILE * out_file;
	uint16_t samples_per_frame;
#if defined( DO_NOT_USE_INTERRUPTS )
#else
	STX104_INT_CFG_TYPE int_cfg;
#endif

	out_file = ds->out_file;

	/* open output files */
	if ( !Menu_Open_Files( ds ) )  return( false );
	/****************************/
	/* determine if timing requests will work */
	fprintf( out_file, "\n\n" );
	fprintf( out_file, "-----------------\n" );
	fprintf( out_file, "Calculated values:\n" );
#if defined( DO_NOT_USE_INTERRUPTS )
	fprintf( out_file, " Interrupts in use      = No\n" );
#else
    fprintf( out_file, " Interrupts in use      = Yes\n" );
#endif
	samples_per_frame   = (uint16_t) ( AI_LAST_CHANNEL - AI_FIRST_CHANNEL + 1 );
	ds->frame_size_in_bytes = samples_per_frame * sizeof( uint16_t /* sample data type */ );
	fprintf( out_file, " frame_size_in_bytes    = %7d\n", ds->frame_size_in_bytes );
	frames_per_second   = ( (double) 1.0 ) / ( ((double) AI_FRAME_TIME_NS) * 1.0e-9 /*ns*/ );
	fprintf( out_file, " frames_per_second      = %10.2f\n", frames_per_second );
	frames_per_file_write  = frames_per_second / ( (double) FILE_OUTPUT_RATE_HZ );
	fprintf( out_file, " frames_per_file_write  = %10.2f\n", frames_per_file_write );
	samples_per_file_write = frames_per_file_write * ((double) samples_per_frame );
	fprintf( out_file, " samples_per_file_write = %10.2f\n", samples_per_file_write );
	if ( samples_per_file_write > ((long) AI_MAXIMUM_SAMPLE_BUFFER_SIZE) )
	{
		fprintf( out_file, " Samples per file write cycle exceed AI_MAXIMUM_SAMPLE_BUFFER_SIZE\n" );
		return false;
	}

	if ( samples_per_file_write >= ((long) 65535 ) )
	{
		fprintf( out_file, " Frame per write cycle too big\n" );
		return false;
	}

/*  Due to the constant writing to the console in this case, we need to increase
	the amount of data read out of the FIFO so that the avera	STX104_Trg_Software_Startge rate is
    frames_per_file_write.

    The actual write rate can be confirmed by observing DOUT3 on the STX104
    when DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE macro is enabled.  DOUT3 will
    toggle each time we get around to writing to the file.

    If the toggling rate is equal or shorter than MENU_UPDATE_TIME_MS then
    you can simply set

    frame_quantity_request_per_write_cycle = (UINT16) ( frames_per_file_write );

    If on the other hand the toggling rate is slower than MENU_UPDATE_TIME_MS
    and/or it is not consistent, then you might need to increase the
    frame_quantity_request_per_write_cycle in order to compensate for the
    error....your mileage will very here depending on the CPU and other
    OS overhead.
*/
	ds->frame_quantity_request_per_write_cycle = (uint16_t) frames_per_file_write ;

	fprintf( out_file, " frame_quantity_request_per_write_cycle = %d\n", ds->frame_quantity_request_per_write_cycle );

	if ( ds->frame_quantity_request_per_write_cycle == 0 )
	{
		fprintf( out_file, " Frames per write cycle is zero\n" );
		return( false );
	}
	fprintf( out_file, "-----------------\n\n" );

	/****************************/
	Board_Initialization( ds );

	/****************************/
	ds->software_frame_count     = 0;
#if defined( FIFO_LOG_DATA_AVAILABLE )
	ds->fifo_log_buffer_index = 0;
#endif
#if defined( DO_NOT_USE_INTERRUPTS )
#else
	/* interrupts */
	memset( &int_cfg, 0, sizeof( STX104_INT_CFG_TYPE ) );
	int_cfg.callback_source_a = (AUL_INT_CALLBACK) CallBack_Source_A;
	int_cfg.source_a          = ISA_THRESHOLD_COUNTER;
	int_cfg.threshold_source  = ITS_AI_FRAME;
	int_cfg.threshold         = ((uint32_t) frame_quantity_request_per_write_cycle );  /* event rate */
	STX104_Int_CFG_Set( board, &int_cfg );
	STX104_Int_Enable_Set( board, true );
#endif
	return true;
}
#endif

#if(0)
/***************************************************************************/
void Menu_Board_Termination( struct demo_stx104_data * ds )
{
	int error_code; /* for debug */
	/* shut down events (interrupts) */
	error_code = STX104_Int_Enable_Set( ds->board, false );
	Menu_Close_Files( ds );
}
#endif

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/



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
/***************************************************************************/
int Demo_STX104_Menu_Frame_Max_Set( struct menu_command_params * p )
{
	struct demo_stx104_data * ds;  /* data set */
	uint32_t 	value;
	ds = (struct demo_stx104_data *) p->info;
	value = Menu_Ask_Uint32( p->in_file, p->out_file, "\n\nFrame count (0=manual start/stop)>" );
	if ( value > ( UINT32_MAX / 2 ) ) ds->software_frame_count_max = ( UINT32_MAX / 2 );
	else							  ds->software_frame_count_max = value;

	fprintf( p->out_file, "\n" );
	return SUCCESS;
}
/***************************************************************************/
int Demo_STX104_Menu_Trigger_Start( struct menu_command_params * p )
{
	struct demo_stx104_data * ds;  /* data set */
	ds = (struct demo_stx104_data *) p->info;
	STX104_Trg_Software_Start( p->board );
	return SUCCESS;
}
/***************************************************************************/
int Demo_STX104_Menu_Trigger_Stop( struct menu_command_params * p )
{
	struct demo_stx104_data * ds;  /* data set */
	ds = (struct demo_stx104_data *) p->info;
	STX104_Trg_Software_Stop( p->board );
	return SUCCESS;
}

/***************************************************************************/
int Demo_STX104_Menu_Revisions( struct menu_command_params * p )
{
	int driver_compiled_with_debug_flag = 0;
	struct demo_stx104_data * ds;  /* data set */
	ds = (struct demo_stx104_data *) p->info;
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
char Demo_STX104_Data_Timer_Tick_State( void )
{
	if ( timer_index == TIMER_INDEX_MAX ) timer_index = 0;
	else                                  timer_index++;
	return timer_tick[timer_index];
}
/***************************************************************************/
int Demo_STX104_Data_Timer_Tick( struct menu_data_params * mdp )
{
	/* shows that the software is running */
	mdp->out_str[mdp->column] = Demo_STX104_Data_Timer_Tick_State();
	mdp->column++;
	return SUCCESS;
}
/***************************************************************************/
int Demo_STX104_Data_DI( struct menu_data_params * mdp )
{
	struct demo_stx104_data * ds;
	BOOL value;
	int  channel;
	int  channel_count;

	ds = (struct demo_stx104_data *) mdp->info;
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
	struct demo_stx104_data * ds;
	stx104_ai_cfg_type 				ai_cfg;
	STX104_GEN_JMP_TYPE 			jumpers;

	ds = (struct demo_stx104_data *) mdp->info;

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
	struct demo_stx104_data * ds;

	ds 			= (struct demo_stx104_data *) mdp->info;
	error_code 	= STX104_Trg_Active_Get( ds->board, &trigger_enabled );
	if ( trigger_enabled ) Menu_Data_Append( mdp, "1" );
	else                   Menu_Data_Append( mdp, "0" );
	return SUCCESS;
}
/***************************************************************************/
int STX104_Data_Frames( struct menu_data_params * mdp )
{
#define STX104_FRAME_STR_SIZE  16
	struct demo_stx104_data * ds;
	/*int  					  frames;*/
	char 					  tmp_str[STX104_FRAME_STR_SIZE];

	ds = (struct demo_stx104_data *) mdp->info;

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


/***************** END -- MENU DISPLAY AND COMMAND PROCESSING ***************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/


/***************************************************************************/

/**************************************************************************/
int Demo_STX104_Main_Loop_Timing_Calculations( unsigned int time_msec, struct demo_stx104_data * ds )
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
void Demo_STX104_Files_Close( struct demo_stx104_data * ds )
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
const char demo_data_file_name[]         = { DEMO_STX104_DATA_FILE_NAME ".bin" };
const char demo_data_file_name_csv[]     = { DEMO_STX104_DATA_FILE_NAME ".csv" };
const char demo_fifo_log_file_name[]     = { DEMO_STX104_FIFO_LOG_FILE_NAME ".bin" };
const char demo_fifo_log_file_name_csv[] = { DEMO_STX104_FIFO_LOG_FILE_NAME ".csv" };
/**********************************************************************//**
@brief
*/
int Demo_STX104_Files_Open( const char * 				data_file_name,  	/* analog input data file */
							const char * 				fifo_log_file_name, /* fifo log file          */
							struct demo_stx104_data * 	ds
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
Brings up the STX104 interface.  First it connects to the driver and then it
opens the board returning the board descriptor.

Note:
The error_codes "error_code" can be used during debug to see
what the functions are returning.
*/
int Demo_STX104_Initialize(  char * prefix,    	/* output string prefix string  */
						 	 int argc,       	/* command line arguments count */
						 	 char* argv[],     	/* command line arguments list  */
						 	 struct demo_stx104_data * info  /* shared data */
                       	   )
{
	int   			error_code;
	int				board;
#if defined( DEMO_STX104_DEBUG )
	FILE *			out_file;
#endif
	(void) argc;  /* not used currently */
	(void) argv;
#if defined( DEMO_STX104_DEBUG )
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

#if defined( DEMO_STX104_DEBUG )
	fprintf( out_file, "%s  -> driver initialize success\n", prefix );
#endif
	error_code = AUL_Library_Revision( &(info->revision_library) );
#if defined( DEMO_STX104_DEBUG )
	fprintf( out_file, "%s  -> library revision = %lu\n", prefix, info->revision_library );
#endif
	error_code = AUL_Driver_Revision( &(info->revision_driver) );
#if defined( DEMO_STX104_DEBUG )
	fprintf( out_file, "%s  -> driver revision = %lu\n", prefix, info->revision_driver );
#endif

	error_code = AUL_Device_Open( "stx104", &board );
	if ( error_code ) return error_code;

	info->board = board;
#if defined( DEMO_STX104_DEBUG )
	fprintf( out_file, "%s  -> stx104 device open successfully\n", prefix );
#endif
	error_code = STX104_Gen_Revision_Get( board, &(info->revision_firmware) );
#if defined( DEMO_STX104_DEBUG )
	fprintf( out_file, "%s  -> stx104_firmware_revision = %04X\n",
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
void Demo_STX104_Terminate( char * prefix,    			/* output string prefix string  */
                        	struct demo_stx104_data * ds  /* shared data 	*/
                          )
{
	int 			error_code; /* used primarily with debugger */
	int            	board;
	FILE * 		   	out_file;
#ifndef DEMO_STX104_DEBUG
	(void) prefix;
#endif
	out_file = ds->out_file; /* short hand */
	board    = ds->board;  	/* short hand */
#if defined( DEMO_STX104_DEBUG )
	fprintf( out_file, "%sTermination begin:\n", prefix );
#endif
	/* set board to inactive state */
	error_code = STX104_Config_Termination( board, prefix, out_file, &(ds->cfg) );
	/* close the device */
	error_code = AUL_Device_Close( board );
#if defined( DEMO_STX104_DEBUG )
	fprintf( out_file, "%s  -> AUL_Device_Closed\n", prefix );
#endif
	/* close or free the driver */
	error_code = AUL_Termination();
#if defined( DEMO_STX104_DEBUG )
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


