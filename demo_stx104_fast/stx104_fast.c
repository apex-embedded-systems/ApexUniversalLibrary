/*****************************************************************************//**
 * @file  stx104_fast.c
 * STX104 any speed demonstration
 *
 * @mainpage   STX104 Fast
 * See main() for details.
 *
 * @subpage page_contact
 *
 * @subpage page_summary
 *
 * @subpage page_history
 *
 * @subpage page_compiler
 *
 * @page page_contact   Contact
 * Apex Embedded Systems<br>
 * email: customer.service@apexembeddedsystems.com<br>
 * voice: 608-256-0767<br>
 *
 *
 * @page page_history  Revision History
 * _____________________________________________________________________________
 * REVISION HISTORY
 * 	June  1, 2009
 * 	June 30, 2009     - Revised to support non-interrupt functionality
 * 					     found a bug in fifo.bin file opening.
 * 					     February 22, 2012 -
 * 					     November  4, 2013 - updated to support both Borland and Microsoft.
 * 					     November  8, 2013 - improve start up initialization
 * 					     May      24, 2014 - migration to Linux and new driver/library
 *
 * @page page_summary  Summary
 * _____________________________________________________________________________
 * <b>DESCRIPTION</b>
 *
 *	This application is intended to perform fast ADC readout.
 *
 *	All of the parameters are set via macros below.  There is no command line
 *	parameters.   In addition, at run time one can set the number of frames
 *	to capture to a file and manually start and stop data collection.
 *
 *	Please search for CUSTOMER_TODO to determine all of the macros that can
 *	be adjusted.  The default values provide an initial low speed rate.
 *
 *	It is possible at run time to manipulate digital outputs, analog outputs
 *	and analog input gains.
 *
 *	This example code is intended to be simple.  There are many ways to implement
 *	this sort of design with many tradeoffs.  In this case we just want to
 *	demonstrate short duration data collection.
 *
 *	Long term data collection would involve continuous appending to a file or
 *	writing several files of fixed length and then managing the files thereof.
 *
 *	Note that in this specific case, the rate at which you perform a readout
 *	and the amount of data samples you request to be read out is a bit of a
 *	compromise since not all systems will be able to achieve a particular
 *	readout rate and/or there may be jitter introduced due to other overhead.
 *	In those cases you will need to increase the requested number of samples
 *	to be read out each cycle in order to compensate for the timing error.  The
 *	bottom line is that so long as your AVERAGE readout rate is the same
 *	AVERAGE rate as data is being sampled, then your system will be able to
 *	collect data long term without fear of a FIFO full condition.
 *
 *	For example, if you choose to collect 100 frames per second and you readout
 *	the frames 10 times per second, then you will want to set the readout to
 *	something just above the average readout rate.
 *
 *	Frames to read each cycle = Frames per second / read rate
 *	                          =  100 / 10 = 10 frames per readout.
 *
 *	So, in this example you might set the number of requested frames to readout
 *	at 10 frames.
 *
 *		uint16_t   	frame_buffer[10];
 *		uint16_t	frame_count_actual;
 *		uint16_t	frame_count_request = 10;
 *		// run this function 10 times per second
 *		error_code = STX104_AI_Frames_Get( 	stx104_board,
 *										frame_buffer,
 *										&frame_count_actual,  //number of frames actually returned
 *										frame_count_request   // not to exceed count.
 *										 );
 *
 *
 *	If your system cannot quite keep up on average, then increase the
 *	frame_count_request until average throughput is achieved.  In the example
 *	above, you might need to increase from 10 to 11.
 *
 *	One way to observe the frame readout rate is to use a digital output, refer
 *	to the DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE macro below.
 *
 * _____________________________________________________________________________
 * <b>POSIX.1b RealTime Extensions Library</b>
 *
 * The librt.a library is used to support High Resolution POSIX Timers (HRT)
 * and the linker is pointed to this library.  The following functions are used
 * herein from the Real Time library:  timer_create(), timer_settime(), timer_delete().
 *
 *
 * Location of library (Ubuntu):  ./usr/lib/i386-linux-gnu/librt.a
 *
 * The library is used by the menu.c file.  Please search for "librt.a".
 *
 * Ubuntu:
 *
 * sudo apt-get update
 *
 * sudo apt-get install build-essential
 *
 * http://askubuntu.com/questions/398489/how-to-install-build-essential
 *
 *
 * yum install glibc-static
 *
 * _____________________________________________________________________________
 * <b>ASSUMPTIONS</b>
 *
 * 1. Not tested, likely broken 2014DEC02 - Compiled using Microsoft Visual C/C++ Studio 2010
 * 2. Not tested, likely broken 2014DEC02 - Compiled using Borland C++ V5.02 Compiler
 * 3. Compiled using GCC in Linux.
 *
 * _____________________________________________________________________________
 * <b>COPYRIGHT NOTICE</b>
 *
 * Copyright(c) 2013-2015 by Apex Embedded Systems. All rights reserved.
 * This document is the confidential property of Apex Embedded Systems
 * Any reproduction or dissemination is prohibited unless specifically
 * authorized in writing.  Customers are free to use this software to
 * develop applications for any Apex Embedded Systems product.
 *
 * _____________________________________________________________________________
 * <b>TODO</b>
 *
 * 1. Improve error handling and reporting.  It assumes that basic functionality
 * has been worked out using the stx104_simple program, and so some of the
 * driver/library functions we are ignoring the return errors, if any.
 *
 * 2. Interrupt testing
 *
 *******************************************************************************/

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
#include <time.h>
#include <unistd.h>  /* _POSIX_TIMERS  */

#include "../aul_tree/aul/aul_public.h"
/* the following includes have been implemented to simplify this file */
#include "../aul_tree/demo_shared/cfgrprt.h"
#include "../aul_tree/demo_shared/config.h"
#include "../aul_tree/demo_shared/menu.h"
#include "../aul_tree/demo_shared/demo_stx104_shared.h"


/*****************************************************************************//**
 * \def _CRT_SECURE_NO_WARNINGS
 * Specific to Visual Studio
 *
 * If you use sprintf(), strcpy() and/or strcat() you will need to enable
 * the following _CRT_SECURE_NO_WARNINGS macro.
 */
//#define  _CRT_SECURE_NO_WARNINGS

/*****************************************************************************//**
 * @page page_compiler  Compiler Notes
 *
 * COMPILER/LINKER NOTES:
 *
 * These notes only apply to Visual Studio or Borland only.
 *
 * i) Microsoft Visual C/C++ Compiler Version 6.0.
 *
 *    COMPLILER:
 *
 *    VC6 Settings...C/C++->preprocessor->additional include directories:
 *    "..\..\auldrv"  (without the quotes)
 *
 *    LINKER:
 *
 *    VC6 Settings:  Link->General->Object/library modules:
 *    "..\..\auldrv\auldrv.lDemo_STX104_Terminateib"  (without the quotes)
 *
 * ii) Using Microsoft Visual C/C++ Studio 2010
 *
 *     Project Properties:
 *
 *     Configuration Properties -> C/C++ -> General -> Additional Include Directories
 *     ..\..\auldrv;%(AdditionalIncludeDirectories)
 *
 *     Linker->Input->Additional Dependencies
 *     ..\..\auldrv\auldrv.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)
 *
 *     Configuration Properties -> Debugging -> Working Directory  $(ProjectDir)
 *
 * iii) Borland:
 *
 *     "cw3230.dll" must reside within the application executable folder or be
 *     in the directory path.
 *
 *     Make sure that enums are treated as ints.  Otherwise they are treated
 *     as bytes which is incompatible with most everything else.
 */

/***************************************************************************/
#define STX104_FAST_REV  "$Date: 2014-05-06 17:06:00 -0500 (Tue, 06 May 2014) $"


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***********  BEGIN -- CUSTOMER DEFINED GLOBAL MACROS   ********************/

/*****************************************************************************//**
 * @def STX104_FAST_DEBUG_MESSAGE
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * Enable/Disable debug print messages NOTE:  also STX104_CONFIG_DEBUG_MESSAGE in
 * "../demo_shared/config.h".
*/
#define STX104_FAST_DEBUG_MESSAGE  _DEBUG
/***************************************************************************/
/* CUSTOMER_TODO: Comment out to not toggle DOUT3 each time we write data
                  to a file.  If there is no data to write, then do not toggle.
*/
#define DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE  1

/*****************************************************************************//**
 * @def STX104_FAST_USE_INTERRUPTS
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * 1. Uncomment if you wish to use interrupts
 *
 * 2. STX104_CONFIG_INTERRUPTS_ACTIVE in "../demo_shared/config.h" must also be enabled.
 *
 * 3. Comment out for software polling of analog input data.
 *
 * NOTE
 * For many applications, interrupts are not required.  To get started right away we
 * suggest running without interrupts (to minimize possible problems related to interrupts).
*/
/* #define STX104_FAST_USE_INTERRUPTS  STX104_CONFIG_INTERRUPTS_ACTIVE   */
/***************************************************************************/
/* CUSTOMER_TODO: If you are using audacity to view binary files, uncomment
                  this mDemo_STX104_Terminateacro in order to obtain twos complement data.
*/
/*****************************************************************************//**
 * @def CONVERT_STRAIGHT_BINARY_TO_TWOS_COMPLEMENT
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * If you are using audacity to view binary files, uncomment
   this macro in order to obtain twos complement data.
*/
#define CONVERT_STRAIGHT_BINARY_TO_TWOS_COMPLEMENT 1

/*****************************************************************************//**
 * @def MENU_UPDATE_TIME_MS
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * Roughly the rate in milliseconds that the menu will be updated.  Typically,
 * 100mS is about the right value for low speed applications.
 *
 * This also sets the readout rate of the analog input FIFO (i.e. STX104_AI_Frames_Get() ).
 *
 * NOTE: Your mileage will vary.  In other words, depending on CPU, hardware,
 * and your application.
*/
#define MENU_UPDATE_TIME_MS  100 /* mS */
/***************************************************************************/
/* CUSTOMER_TODO: The number of times per second to write data to a file
 	 	 	 	  will now be same as as the rate to update the menu.
*/
#if defined( STX104_FAST_USE_INTERRUPTS )
  #define FILE_OUTPUT_RATE_HZ  ( ((double) 1000) / ((double) MENU_UPDATE_TIME_MS) )
#else
  #define FILE_OUTPUT_RATE_HZ  ( ((double) 1000) / ((double) MENU_UPDATE_TIME_MS) )
#endif 

/*****************************************************************************//**
 * @def AI_FRAME_TIME_SEC
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * Analog input frame timer, value is in nano-seconds
 *
 * NOTES:
 * 1.  if set too fast, it will be stuck in Menu_AI_uint16_t_Get().  This
 *     example code is intended for low analog input data rates.
 * 2.  The driver software will internally compute the correct sample rate
 *     to an accuracy of +/-25nS.
*/
/*                        mmmuuunnn  */
//#define AI_FRAME_TIME_NS 50000000
#define AI_FRAME_TIME_NS      20000
/* NOTES:
 *    1. If kenerl module debugging _DEBUG is enabled (see Makefile)
 *       then it is likely that the maximum throughput rate will be
 *       much less.  On our P4 test bed the limit was around ???
 *       samples per second.  Commenting out the _DEBUG (line 11 of
 *       Makefile found in drv_aul_x86), compiling and reinstalling
 *       the driver (kernel module) we were able to achieve full
 *       speed, even if the library _DEBUG was still active.
 *
 */

/*****************************************************************************//**
 * @def MENU_MAIN_LOOP_USE_TIMER_RATHER_THAN_SLEEP_DELAYS_IMPROVED_TIMING
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * Comment out to simply revert to usleep() delays rather than using menu interval
 * timer mechanism based on signals.
*/
#define MENU_MAIN_LOOP_USE_TIMER_RATHER_THAN_SLEEP_DELAYS_IMPROVED_TIMING  1

/*****************************************************************************//**
 * @def AI_FIRST_CHANNEL
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * Set the first and last analog input channels to be scanned.
*/
#define AI_FIRST_CHANNEL  0
#define AI_LAST_CHANNEL   3

/*****************************************************************************//**
 * @def FIFO_LOG_DATA_AVAILABLE
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * Uncomment to enable logging of FIFO statistics.
*/
#define FIFO_LOG_DATA_AVAILABLE 1

/**************  END -- CUSTOMER DEFINED GLOBAL MACROS   *******************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

/***************************************************************************/
#ifdef STX104_FAST_USE_INTERRUPTS
 #define AI_MAXIMUM_SAMPLE_BUFFER_SIZE   16384
#else
 #define AI_MAXIMUM_SAMPLE_BUFFER_SIZE   ( 32768 * 2 - 2 )
// #define AI_MAXIMUM_SAMPLE_BUFFER_SIZE   32768
#endif


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/******************     BEGIN -- GLOBAL VARIABLES   ************************/

/***************************************************************************/
uint16_t ai_buffer[AI_MAXIMUM_SAMPLE_BUFFER_SIZE];
/***************************************************************************/
#define LOCAL_STRING_BUFFER_SIZE  128
#define FILE_NAME_SIZE            256
/***************************************************************************/
static struct demo_stx104_data  demo_stx104_data;
/***************************************************************************/
void CallBack_Source_A( void * result );
static void STX104_Fast_Process_Start_Configuration( struct demo_stx104_data * ds );
void STX104_Fast_Process_Stop_Request( struct demo_stx104_data * ds );

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

#if(0)
/***************************************************************************/
static const char * STX104_Fast_Error_String( int error_code )
{
	if ( error_code >= DEMO_STATUS_START )
	{

	}
	else
	{
		return AUL_Error_String( error_code );
	}
}
#endif

/***************************************************************************/
#if defined( FIFO_LOG_DATA_AVAILABLE )
static void STX104_Fast_FIFO_Log( struct demo_stx104_data * ds )
{
	uint32_t fifo_count;
	if ( NULL == ds->fifo_log_file ) return; /* nothing to do */
	STX104_AI_FIFO_Data_Count_Get( ds->board, &fifo_count );
	fwrite( &fifo_count, sizeof( uint32_t ), 1, ds->fifo_log_file );
}
#endif
/***************************************************************************/
static int STX104_Fast_Process_Data( struct demo_stx104_data * ds )
{
	uint16_t 		frame_count_actual;
	size_t			frame_count_write;
	int		 		error_code;
	int				board;    /* short hand */
	FILE *			out_file; /* short hand */
	uint32_t        scratch;
/*
#if defined( FIFO_LOG_DATA_AVAILABLE )
	uint32_t 		fifo_count;
#endif
*/
#if defined( FIFO_LOG_DATA_AVAILABLE )
	STX104_Fast_FIFO_Log( ds );
#endif
	if ( NULL == ds->data_output_file ) return 0; /* nothing to do */

	board    = ds->board;
	out_file = ds->out_file;
	frame_count_write = 0;
	error_code = STX104_AI_Frames_Get( board,               /* board descriptor */
									   &ai_buffer[0],       /* starting position of the destination buffer */
									   &frame_count_actual, /* actual frames that are copied to the buffer */
									   ds->frame_quantity_request_per_write_cycle /* frames requested */
									);
	if ( EC_STX104_AI_FIFO_FULL == error_code )
	{  /* Don't bother to write anything because at this point we
			now have data loss (and out of sync with channels),
			better to restart.
		*/
		STX104_Fast_Process_Stop_Request( ds );
		fprintf( out_file, "\nMPD: Terminate FIFO full error: " );
		fprintf( out_file, "terminating file write due to data loss.\n\n" );
		ds->fifo_full_error = true;
		/*frame_count_actual = 0;
		STX104_Fast_Process_Start_Configuration( ds );*/
		return 0;
	}
	if ( frame_count_actual > 0 )
	{ /* compute software frame count and max frames to further process */
		if ( ds->software_frame_count_max > 0 )
		{
			if ( ds->software_frame_count >= ds->software_frame_count_max )
			{
				frame_count_write = 0;
			}
			else
			{
				scratch  = ds->software_frame_count;
				scratch += (uint32_t) frame_count_actual;
				if ( scratch < ds->software_frame_count_max )
				{
					ds->software_frame_count = scratch;
					frame_count_write        = (size_t) frame_count_actual;
				}
				else
				{
					scratch = ds->software_frame_count_max - ds->software_frame_count;
					ds->software_frame_count   += scratch;
					frame_count_write 			= (size_t) scratch;
					STX104_Fast_Process_Stop_Request( ds );
				}
			}
		}
		else
		{
			frame_count_write         = (size_t) frame_count_actual;
			ds->software_frame_count += (uint32_t) frame_count_actual;
		}
/*printf( "  frame_quantity_request_per_write_cycle = %d\n", ds->frame_quantity_request_per_write_cycle );
printf( "  frame_count_actual                     = %d\n", frame_count_actual );
printf( "  frame_count_write                      = %d\n", frame_count_write );
printf( "  software_frame_count                   = %d\n", ds->software_frame_count );*/
		if ( frame_count_write > 0 )
		{ /* process frames */
#if defined( CONVERT_STRAIGHT_BINARY_TO_TWOS_COMPLEMENT )
			if ( ds->revision_firmware < STX104_REVISION_120328 )
      		{
      			size_t index;
      			for( index = 0; index < frame_count_write; index++ )
      			{
						ai_buffer[index] = ai_buffer[index] ^ 0x8000;
      			}
      		}
      		else
      		{
      			if ( ( AI_ENCODING_STRAIGHT_BINARY == ds->ai_cfg_encoding ) ||
					 ( AI_ENCODING_STRAIGHT_BINARY == ds->ai_cfg_encoding )  )
         		{
         			size_t index;
      				for( index = 0; index < frame_count_write; index++ )
            		{
      					ai_buffer[index] = ai_buffer[index] ^ 0x8000;
            		}
         		}
      		}
#endif
			fwrite( 	ai_buffer,					/* buffer to send to file */
						ds->frame_size_in_bytes,	/* size of each frame in bytes */
						frame_count_write,			/* number of frames in the buffer */
						ds->data_output_file		/* data file that buffer will be copied to */
				  );
		} /* frame_count_write */
	} /* frame_count_actual -- The number of samples sitting in the FIFO just after
	                           the completion of a frame readout.
	  */
	return (int) frame_count_write;
}
/***************************************************************************/
static void Static_Fast_CallBack_Source_A( void * result )
{
	STX104_INT_RESULT_TYPE* 	event_result;
	uint16_t					frame_count_actual;
	int							error_code;
	int							board;
	struct demo_stx104_data * 	ds;

	event_result = (STX104_INT_RESULT_TYPE*) result;
	ds = ( struct demo_stx104_data * ) event_result->user_defined_dataset;
	board = event_result->board;
#if defined( FIFO_LOG_DATA_AVAILABLE )
	/* log fifo activity */
	STX104_Fast_FIFO_Log( ds );
#endif
#if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
		ds->dout3_state = !ds->dout3_state;
		STX104_DO_Set( board, STX104_DO3, ds->dout3_state );
#endif
	error_code = STX104_AI_Frames_Get(	board, 				 /* board descriptor */
										&ai_buffer[0],       /* starting position of the destination buffer */
										&frame_count_actual, /* actual frames that are copied to the buffer */
										ds->frame_quantity_request_per_write_cycle /* frames requested */
									);
	if ( error_code )
	{  /* Don't bother to write anything because at this point we
			now have data loss, better to restart.
		*/
		STX104_Int_Enable_Set( board, false );
		STX104_Trg_Software_Stop( board );
		/*TODO fprintf( out_file, "\n\nCallBack_Source_A: FIFO full error: " );*/
		/*TODO fprintf( out_file, "terminating file write due to data loss.\n" );*/
		ds->fifo_full_error = true;
		frame_count_actual = 0;
		STX104_Fast_Process_Stop_Request( ds );;
		return;
	}
//printf( "event: frame_count_actual=%d\n", frame_count_actual );
	if ( frame_count_actual )
	{
		fwrite( &ai_buffer[0],			/* buffer to send to file */
				ds->frame_size_in_bytes,/* size of each frame in bytes */
				frame_count_actual,		/* number of frames in the buffer */
				ds->data_output_file	/* data file that buffer will be copied to */
		      );
	}
#if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
		ds->dout3_state = !ds->dout3_state;
		STX104_DO_Set( board, STX104_DO3, ds->dout3_state );
#endif
}
/***************************************************************************/
static int64_t STX104_Fast_Time_Delta( struct timespec * ts_start, struct timespec * ts_end )
{
    /* seconds, multiplied with 1 million */
    int64_t usec_start = ts_start->tv_sec * 1000000;
    int64_t usec_end   = ts_end->tv_sec * 1000000;
    /* Add full microseconds */
    usec_start += ts_start->tv_nsec/1000;
    usec_end += ts_end->tv_nsec/1000;
    /* round up if necessary */
    if (ts_start->tv_nsec % 1000 >= 500) ++usec_start;
    if (ts_end->tv_nsec % 1000 >= 500) ++usec_end;
    usec_end = usec_end - usec_start;
    return usec_end;
}
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
#define STX104_FAST_CONFIG_FROM_HARDWARE  1
/***************************************************************************

Initialization sequence:
	1. turn off interrupts first, just in case they are active.
	2. initialize triggering, make sure triggering has been stopped.
	3. Initialize analog section
	4. Initialize digital I/O section

Admittedly, there is no error handling here.  The purpose is to illustrate
the main points of configuration and initialization.
*/
static void STX104_Fast_Process_Start_Configuration( struct demo_stx104_data * ds )
{
	int   				error_code;  /* for debug use only */
	stx104_ai_cfg_type  ai_cfg;
	STX104_DI_CFG_TYPE  di_cfg;
	/*STX104_DO_CFG_TYPE  do_cfg;*/
	STX104_TRG_CFG_TYPE trg_cfg;
	STX104_INT_CFG_TYPE int_cfg;
	int					board;
	board = ds->board;
	/* interrupts */
	error_code = STX104_Int_Enable_Set( board, false ); /* make STX104 interrupts are disabled */
#ifdef STX104_FAST_CONFIG_FROM_HARDWARE
	error_code = STX104_Int_CFG_Get( board, &int_cfg );
#else
	memset( &int_cfg, 0, sizeof( STX104_INT_CFG_TYPE ) );
#endif
	int_cfg.callback_source_a = (AUL_INT_CALLBACK) Static_Fast_CallBack_Source_A;
	int_cfg.source_a          = ISA_THRESHOLD_COUNTER;
	int_cfg.threshold_source  = ITS_AI_FRAME;
	int_cfg.threshold         = ((uint32_t) ds->frame_quantity_request_per_write_cycle );  /* event rate */
#ifdef STX104_FAST_USE_INTERRUPTS
	error_code = STX104_Int_CFG_Set( board, &int_cfg );
#endif
	/* triggering   */
	error_code = STX104_Trg_Software_Stop( board );  /* make sure triggering has been stopped */
#ifdef STX104_FAST_CONFIG_FROM_HARDWARE
	error_code = STX104_Trg_CFG_Get( board, &trg_cfg );
#else
	memset( &trg_cfg, 0, sizeof( STX104_TRG_CFG_TYPE ) );
#endif
	trg_cfg.enabled           = true;
	trg_cfg.start_mode        = STM_IMMEDIATE;
	trg_cfg.start_sync_source = TSS_NONE;
	trg_cfg.start_source      = STS_SOFTWARE_STROBE; /* start via software */
	trg_cfg.stop_source       = ETS_SOFTWARE_STROBE; /* stop via software */
	error_code = STX104_Trg_CFG_Set( board, &trg_cfg );
	/* make sure trigger is now inactive/off */
	error_code = STX104_Trg_Software_Stop( board );
	/* analog inputs */
#ifdef STX104_FAST_CONFIG_FROM_HARDWARE
	error_code = STX104_AI_CFG_Get( board, &ai_cfg );
#else
	memset( &ai_cfg, 0, sizeof( stx104_ai_cfg_type ) ); /* clear configuration */
#endif
	ai_cfg.gain            = ACFG_GAIN_10V;
	ai_cfg.burst_timer_sec = 0.0; /* ignore */
	ai_cfg.frame_timer_sec = 0.0; /* ignore */
	ai_cfg.burst_timer_ns  = 5000; /* nS */
	ai_cfg.frame_timer_ns  = AI_FRAME_TIME_NS;
	ai_cfg.strobe_source   = AISS_AI_FRAME_TIMER;
	ai_cfg.first_channel   = AI_FIRST_CHANNEL;
	ai_cfg.last_channel    = AI_LAST_CHANNEL;
	ai_cfg.use_triggering_subsystem  = true;
	ai_cfg.high_speed_pre_queue_fifo = true;
#if defined( CONVERT_STRAIGHT_BINARY_TO_TWOS_COMPLEMENT )
	if ( ds->revision_firmware >= STX104_REVISION_120328 )
	{ /* let the hardware do the conversion! */
		ds->ai_cfg_encoding = AI_ENCODING_TWOS_COMPLEMENT;
		ai_cfg.encoding = AI_ENCODING_TWOS_COMPLEMENT;
	}
#endif
	error_code = STX104_AI_CFG_Set( board, &ai_cfg ); /* update configuration to board */
	/* digital inputs */
#ifdef STX104_FAST_CONFIG_FROM_HARDWARE
	error_code = STX104_DI_CFG_Get( board, &di_cfg );
#else
	memset( &di_cfg, 0, sizeof( STX104_DI_CFG_TYPE ) );
#endif
	di_cfg.swap_din1_and_din3 = true;
	error_code = STX104_DI_CFG_Set( board, &di_cfg );
	/* copy configurations to dataset */
	memset( &(ds->cfg), 0, sizeof(ds->cfg) );
	memcpy( &(ds->cfg.int_cfg), &int_cfg, sizeof(ds->cfg.int_cfg) );
	memcpy( &(ds->cfg.ai_cfg),  &ai_cfg,  sizeof(ds->cfg.ai_cfg)  );
	memcpy( &(ds->cfg.trg_cfg), &trg_cfg, sizeof(ds->cfg.trg_cfg) );
	memcpy( &(ds->cfg.di_cfg),  &di_cfg,  sizeof(ds->cfg.di_cfg)  );
	memcpy( &(ds->cfg.int_cfg), &int_cfg, sizeof(ds->cfg.int_cfg) );
}
/***************************************************************************/
static int STX104_Fast_Process_Start_Report( struct demo_stx104_data * ds )
{
	double 		frames_per_second;
	double 		frames_per_file_write;
	double 		samples_per_file_write;
	uint16_t 	samples_per_frame;
	FILE * 		out_file = ds->out_file;  /* short hand */
	/* determine if timing requests will work */
	fprintf( out_file, "\n\n" );
	fprintf( out_file, "-----------------\n" );
	fprintf( out_file, "Calculated values:\n" );
#ifndef STX104_FAST_USE_INTERRUPTS
	fprintf( out_file, " Interrupts in use      =      No\n" );
#else
    fprintf( out_file, " Interrupts in use      =     Yes\n" );
#endif
    fprintf( out_file, " Frames to be collected = " );
    if ( ds->software_frame_count_max > 0 )
    {
    	fprintf( out_file, "%7u\n", ds->software_frame_count_max );
    }
    else
    {
    	fprintf( out_file, "  MANUAL\n" );
    }

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
		return -EC_APP_FILE_OPEN_ERROR;
	}

	if ( samples_per_file_write >= ((long) 65535 ) )
	{
		fprintf( out_file, " Frame per write cycle too big\n" );
		return -1;
	}
	ds->frame_quantity_request_per_write_cycle = (uint16_t) frames_per_file_write ;

	/*  In order for the FIFO sample count to be maintained at an average DC
	 *  value it was important on your test system to increase the frame quantity
	 *  per file write cycle.  In other words, due to large amount of jitter in
	 *  our system the best compensation method is by increasing the frame request
	 *  (i.e. frame_quantity_request_per_write_cycle ) by a percentage.  Another
	 *  way of saying this is that when we achieve a relatively constant amount
	 *  of samples sitting in the FIFO (even though it is going up and down) then
	 *  we know that the system will keep up long term.
	 *
	 *  We can observe this behavior by plotting the fifo.csv file.
	 *
	 */
	ds->frame_quantity_request_per_write_cycle = (uint16_t) ( frames_per_file_write * 1.25 );
/*	ds->frame_quantity_request_per_write_cycle = (uint16_t) ( frames_per_file_write * 1.5  );*/


	fprintf( out_file, " frame_quantity_request_per_write_cycle = %d\n", ds->frame_quantity_request_per_write_cycle );
	if ( ds->frame_quantity_request_per_write_cycle == 0 )
	{
		fprintf( out_file, " Frames per write cycle is zero\n" );
		return -EC_APP_FILE_OPEN_ERROR;
	}
	fprintf( out_file, "-----------------\n\n" );
	return SUCCESS;
}
/***************************************************************************/
static int STX104_Fast_Process_Start( struct demo_stx104_data * ds )
{
	int error_code;
	/* open data and fifo log files */
	error_code = Demo_STX104_Files_Open( NULL, /* use default analog input data file name */
								   	   	 NULL, /* use default fifo log file name   		*/
								   	   	 ds
								 	   );
	if ( error_code ) return error_code;
	/* produce report indicating setup */
	error_code = STX104_Fast_Process_Start_Report( ds );
	if ( error_code ) return error_code;
	/* initialize device and data structures */
	ds->software_frame_count = 0;
	ds->stop_request         = false;
	STX104_Fast_Process_Start_Configuration( ds );
	/* start time -- for statistical calculation use */
#if _POSIX_TIMERS
	clock_gettime( CLOCK_REALTIME, &(ds->time_start) );
#endif
	/* issue hardware trigger (i.e. enable analog input data sampling ) */
	error_code = STX104_Trg_Software_Start( ds->board );
	/*ds->active_sampling_interlock = true;*/
#ifdef STX104_FAST_USE_INTERRUPTS
	error_code = STX104_Int_Enable_Set( board, true );
#endif
	return error_code;
}
/***************************************************************************/
static void STX104_Fast_Process_Stop_Report( struct demo_stx104_data * ds )
{
	int64_t delta_usec;
	double  frames_per_sec;
	/* Post summary information */
	fprintf( ds->out_file, "\n\nData collection complete\n" );
	/* STX104_AI_Frame_Count_Get( board, &frame_count ); */
#if defined( __BORLANDC__ )
	fprintf( ds->out_file, "  Total frames written = %8ld\n", ds->software_frame_count );
	fprintf( ds->out_file, "  Total bytes written  = %8lu\n", ds->software_frame_count * ((uint32_t) ds->frame_size_in_bytes) );
#else
	fprintf( ds->out_file, "  Total frames written = %8d\n", ds->software_frame_count );
	fprintf( ds->out_file, "  Total bytes written  = %8u\n", ds->software_frame_count * ((uint32_t) ds->frame_size_in_bytes) );
#endif
	/*fprintf( out_file, "\n" );*/
	delta_usec = STX104_Fast_Time_Delta( &(ds->time_start), &(ds->time_stop) );
	fprintf( ds->out_file, "  Time interval        = %8lu us\n", (long unsigned int) delta_usec );
	fprintf( ds->out_file, "  Total frames         = %8lu\n",    (long unsigned int) ds->software_frame_count );
	frames_per_sec = 1.0e6 /* us/sec */ * ((double) ds->software_frame_count) / ((double) delta_usec );
	fprintf( ds->out_file, "  Frames/sec           = %10.1f\n", frames_per_sec );
	fprintf( ds->out_file, "\n" );
}
/***************************************************************************/
void STX104_Fast_Process_Stop( struct demo_stx104_data * ds )
{
#ifdef STX104_FAST_USE_INTERRUPTS
	int error_code;  /* debug only */
#endif
	if ( !ds->stop_request ) return; /* nothing to do */
	ds->stop_request = false;
	/* issue a trigger stop */
#ifdef STX104_FAST_USE_INTERRUPTS
	error_code = STX104_Trg_Software_Stop( ds->board );
#endif
	/* shut down events (interrupts) */
#ifdef STX104_FAST_USE_INTERRUPTS
	error_code = STX104_Int_Enable_Set( ds->board, false );
#endif
	/* end of sample timing */
#if _POSIX_TIMERS
	clock_gettime( CLOCK_REALTIME, &(ds->time_stop) );
#endif
	/* output statistics */
	STX104_Fast_Process_Stop_Report( ds );
	/* close out data and log files */
	Demo_STX104_Files_Close( ds );
	fprintf( ds->out_file, "Done\n\n" );
}
/***************************************************************************/
void STX104_Fast_Process_Stop_Request( struct demo_stx104_data * ds )
{
	ds->stop_request = true;
}

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
/****  The struct menu_command_params can be found in menu.h */
/***************************************************************************/
static int STX104_Fast_Menu_Trigger_Start( struct menu_command_params * p )
{
	int error_code;
	struct demo_stx104_data * ds;  /* data set */
	ds = (struct demo_stx104_data *) p->info;
	error_code = STX104_Fast_Process_Start( ds );
	return error_code;
}
/***************************************************************************/
static int STX104_Fast_Menu_Trigger_Stop( struct menu_command_params * p )
{
	struct demo_stx104_data * ds;  /* data set */
	ds = (struct demo_stx104_data *) p->info;
	STX104_Fast_Process_Stop_Request( ds );
	return SUCCESS;
}
/***************************************************************************/
#if defined( FIFO_LOG_DATA_AVAILABLE )
static int STX104_Fast_Menu_Convert_FIFO_Log_File_To_CSV( struct menu_command_params * p )
{
	struct demo_stx104_data * ds;  /* data set */
	FILE * 		csv_file;
	uint32_t 	fifo_count;
	char 		file_name[FILE_NAME_SIZE];
	uint32_t 	row_count;
#if defined( __BORLANDC__ ) || defined( __linux__ )
#else
	errno_t  error_code;
#endif
	ds = (struct demo_stx104_data *) p->info;
	/* open data file */
#if defined( _WIN32 )
	error_code = fopen_s( &(ds->fifo_log_file), demo_fifo_log_file_name, "rb" );
#else
	ds->fifo_log_file = fopen( demo_fifo_log_file_name, "rb" );
#endif
	if ( NULL == ds->fifo_log_file )
	{
		fprintf( ds->out_file, "Cannot open FIFO log output file.\n" );
		return -EC_DEMO_MENU_QUIT;
	}
#if defined( _WIN32 )
	error_code = fopen_s( &csv_file, demo_fifo_log_file_name_csv, "wt" );
#else
	csv_file = fopen( demo_fifo_log_file_name_csv, "wt" );
#endif
	if ( NULL == csv_file )
	{
		fclose( ds->fifo_log_file );  ds->fifo_log_file = NULL;
		fprintf( ds->out_file, "Cannot open file CSV file.\n" );
		return -EC_DEMO_MENU_QUIT;
	}
	fprintf( ds->out_file, "\n" );
	fprintf( ds->out_file, "Log File CSV Dump:\n" );
	row_count = 0;

	while( (!feof(ds->fifo_log_file)) && (row_count <= 65534) )
	{
		fread( &fifo_count, sizeof( uint32_t ), 1, ds->fifo_log_file );
		if ( feof(ds->fifo_log_file) ) break;
#if defined( __BORLANDC__ )
		sprintf( file_name, "%lu\n", fifo_count );
#elif defined( _WIN32 )
		sprintf_s( file_name, FILE_NAME_SIZE, "%lu\n", fifo_count );
#else /* assume Linux */
		sprintf( file_name, "%u\n", fifo_count );
#endif
		fwrite( &file_name[0], sizeof( char ), strlen( file_name ), csv_file );

		if ( !(row_count % 256) )
		{
			fprintf( ds->out_file, "\r%c", Demo_STX104_Data_Timer_Tick_State() );
		}
		row_count++;
	}
	fclose( ds->fifo_log_file );   ds->fifo_log_file = NULL;
	fclose( csv_file );
	return SUCCESS;
}
#endif
/***************************************************************************/
static int STX104_Fast_Menu_Convert_Data_File_To_CSV( struct menu_command_params * p )
{
	struct demo_stx104_data * ds;  /* data set */
	FILE * 		csv_file;
	/*char 		file_name[FILE_NAME_SIZE];*/
	char 		out_str[LOCAL_STRING_BUFFER_SIZE];
	char 		tmp_str[16];
	uint16_t 	buf[16];
	long 		frame_count;
	int  		frame_in_bytes_size;
	int 		chan;
	int 		channel_max;
#if defined( __BORLANDC__ ) || defined( __linux__ )
#else
	errno_t  error_code;
#endif
	ds = (struct demo_stx104_data *) p->info;
	/* open data file */
#if defined( _WIN32 )
	error_code = fopen_s( &(ds->data_output_file), demo_data_file_name, "rb" );
#else
	ds->data_output_file = fopen( demo_data_file_name, "rb" );
#endif
	if ( NULL == ds->data_output_file )
	{
		fprintf( ds->out_file, "Cannot open data output file.\n" );
		return -EC_DEMO_MENU_QUIT;
	}
#if defined( _WIN32 )
	error_code = fopen_s( &csv_file, demo_data_file_name_csv, "wt" );
#else
	csv_file = fopen( demo_data_file_name_csv, "wt" );
#endif
	if ( NULL == csv_file )
	{
		fclose( ds->data_output_file );   ds->data_output_file = NULL;
		fprintf( ds->out_file, "Cannot open file CSV file.\n" );
		return -EC_DEMO_MENU_QUIT;
	}

	fprintf( ds->out_file, "\n" );
	fprintf( ds->out_file, "Data File CSV Dump:\n" );

	channel_max         = (int) ( AI_LAST_CHANNEL - AI_FIRST_CHANNEL );
	frame_in_bytes_size = (int) ( ( AI_LAST_CHANNEL - AI_FIRST_CHANNEL + 1 ) * sizeof( uint16_t /* sample data type */ ) );
	frame_count         = 0;

	/* NOTE: Excel file limit is 65535 lines */
	while( (!feof(ds->data_output_file)) && (frame_count <= 65534) )
	{
		fread( &buf[0], frame_in_bytes_size, 1, ds->data_output_file );
		if ( feof(ds->data_output_file) ) break;
#if defined( _WIN32 )
		strcpy_s( out_str, LOCAL_STRING_BUFFER_SIZE, "" );
#else
		strcpy( out_str, "" );
#endif
		for( chan=0; chan<=channel_max; chan++ )
		{
# if defined( _WIN32 )
			sprintf_s( tmp_str, 16, "%u", buf[chan] );
			strcat_s( out_str, LOCAL_STRING_BUFFER_SIZE, tmp_str );
			if ( chan == channel_max )  strcat_s( out_str, LOCAL_STRING_BUFFER_SIZE, "\n" );
			else                        strcat_s( out_str, LOCAL_STRING_BUFFER_SIZE, "," );
# else
 			sprintf( tmp_str, "%u", buf[chan] );
			strcat( out_str, tmp_str );
			if ( chan == channel_max )  strcat( out_str, "\n" );
			else                        strcat( out_str, "," );
# endif
		}

		if ( !(frame_count % 256) )
		{
			fprintf( ds->out_file, "\r%c", Demo_STX104_Data_Timer_Tick_State() );
		}

		fwrite( &out_str[0], sizeof( char ), strlen( out_str ), csv_file );
		frame_count++;
	}
	/* close the file */
	fclose( ds->data_output_file );  ds->data_output_file = NULL;
	fclose( csv_file );
	return SUCCESS;
}
/**********************************************************************//**
 * @brief
 * Menu structure or list.
 *
 * The underlying menu processing is performed via menu.c functions.
 * The menu was implemented in this fashion for flexibility which is
 * reused for other demonstration code.  It also removes a lot of
 * clutter from the demo source files.
 *
 * Note that each of the functions used within this list must handle
 * a single pointer to a menu_command_params data structure.
 */
static struct menu_command stx104_fast_menu_list[] =
{
/* (*) if char_first=0, function=NULL, then char_last is prefix spaces.					*/
/*     if char_last < ' ' then it can be used as an index parameter           		*/
/*   char    char                                                             		*/
/*   first   last(*)    function                      display name            		*/
{   0,     0,  	NULL,							"\n"													},
{   0,     1,  	NULL,							"-------------------------------"						},  /* display only 	*/
{   0,     1,  	NULL,							"  < < <  M E N U  > > >"								},  /* display only 	*/
{   0,     1,  	NULL,							"-------------------------------"						},  /* display only 	*/
{	0,	   0,	Demo_STX104_Menu_Revisions,		NULL													},  /* display helper function 		*/
{   0,     4,  	NULL,							"---- triggering -----"		 							},  /* display only 	*/
{ 'a',	   0,	Demo_STX104_Menu_Frame_Max_Set,	" number of frames to collect"							},
{ 's',	   0,	STX104_Fast_Menu_Trigger_Start,		" begin or start analog input scan (s/w trig start)"	},
{ 'e',	   0,	STX104_Fast_Menu_Trigger_Stop,		" end or stop analog input scan (s/w trig stop)"		},
{   0,     4,  	NULL,							"---- Analog Inputs (AI) -----"							},  /* display only 	*/
{ 'w',	   0,	Menu_AI_Gain_Set,	   			" +10.0V  range (PGA = 00)"								},
{ 'x',	   1,	Menu_AI_Gain_Set,	   			"  +5.0V  range (PGA = 01)"								},
{ 'y',	   2,	Menu_AI_Gain_Set,	   			"  +2.5V  range (PGA = 10)"								},
{ 'z',	   3,	Menu_AI_Gain_Set,	   			" +1.25V  range (PGA = 11)"								},
{   0,     4,  	NULL,							"---- Analog Outputs (AO) -----"						},  /* display only 				 */
{ 'o',	   0,	Menu_AO_Value_Set, 				" write to channel-a"									},
{ 'p',	   1,	Menu_AO_Value_Set, 				" write to channel-b"									},
{   0,     4,  	NULL,							"---- Digital Outputs (DO) -----"						},  /* display only 				 */
{ 'g',	   0,	Menu_DO_All_Set,				" write to all"											},
{ 'h',	 'k',	Menu_DO_Value_Set,				" write to channel (DO0 to DO3)" 						},
{   0,     4,  	NULL,							"---- Convert Existing Files -----"						},  /* display only 				 */
{   0,     4,  	NULL,							"     NOTE: TRG=0 or no active data collection"			},  /* display only 				 */
#if defined( FIFO_LOG_DATA_AVAILABLE )
{ 'f',	   0, 	STX104_Fast_Menu_Convert_FIFO_Log_File_To_CSV,	" Binary FIFO file to CSV file"				},
#endif
{ 'b',	   0, 	STX104_Fast_Menu_Convert_Data_File_To_CSV,	 	" Binary data file to CSV file"				},
{   0,     4,  	NULL,							"---- other -----"										},  /* display only 				 */
{ 'r',	   0, 	Menu_Redraw,					" redraw menu"					 						},
{ 'F',	   0, 	Menu_Report_File,				" Status and configuration report to 'report.txt'"		},
{ 'R',	   0, 	Menu_Report_Console,			" Status and configuration report to console"	 		},
{ 'q',	   0, 	Menu_Quit,						" quit this demo"				 						},
{   0,     4,  	NULL,							" PRESS KEY TO SELECT MENU ITEM>"						},
{   0,     1,  	NULL,							"DI = Digital in.   TRG = Trig active" 					},
{   0,     1,  	NULL,							"Polarity: BIP/UNI. Input: SE/DIF. FR = frames read."  	},
{   0,     0,  	NULL,							"\n"													},  /* display only 				 */

{ 0, 0, NULL, NULL }   /* stopper */
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


/***************************************************************************/
/*
	           < < < <   	MENU DATA PRESENTATION    > > > >
*/
/***************************************************************************/


/**********************************************************************//**
 *TODO
 */
static struct menu_data stx104_fast_data_list[] =
{
	/*	column	max_width	display function				 Prefix length		Prefix  		*/
	{	0,			0,		Demo_STX104_Data_Timer_Tick,	  0,				NULL	  		},
#if defined( STX104_FAST_USE_INTERRUPTS )
	{	2,			0,		STX104_Simple_Interrupt_Status,	  0,				NULL	  		},
#endif
	{	5,			0,		Demo_STX104_Data_DI,			  3,				"DI:"	  		},
	{  14,			0,		Demo_STX104_Data_Config_AI,		  3,				"AI:"	  		},
	{  32,			0,		Demo_STX104_Data_Trig,	  		  4,				"TRG:"	  		},
	{  38,			0,		STX104_Data_Frames,		  		  3,				"FR:"	  		},
#if(0)
	{  43,			0,		STX104_Simple_Data_AI_Value,	  0,				NULL	  		},
#endif
	{	0,			0,		NULL,							  0,				NULL	  		}
};

/***************** END -- MENU DISPLAY AND COMMAND PROCESSING ***************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/



/***************************************************************************/


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************  BEGIN -- MAIN PORTION OF CODE (TOP LEVEL)  ***************/
/**************************************************************************/

/**********************************************************************//**
 * @brief
 *
 * @param private_data
 * @return Always returns SUCCESS (0)
 */
int STX104_Fast_Main_Stop_Request( void * private_data )
{
	struct demo_stx104_data * 	ds;
	ds = (struct demo_stx104_data *) private_data;
	STX104_Fast_Process_Stop_Request( ds );
	ds->quit_request = true;
	return SUCCESS;
}
/**********************************************************************//**
 * @brief
 *
 * @param private_data
 * @return
 */
#if MENU_MAIN_LOOP_USE_TIMER_RATHER_THAN_SLEEP_DELAYS_IMPROVED_TIMING == 1
int STX104_Fast_Main_Loop( void * private_data )
{
	int 						error_code;
	int    						board;
	FILE * 						out_file;
	FILE * 						in_file;
	struct demo_stx104_data * 	ds;
	int							frame_count;  /* indicates how many frames per loop, debug use only */
	BOOL						dout3_state;

	ds = (struct demo_stx104_data *) private_data;
	out_file= ds->out_file;
	in_file = ds->in_file;
	board 	= ds->board;

#define STX104_FAST_MAIN_FUNCTION_EACH_TIME_MEASURE  0

#if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	dout3_state = 1;
	STX104_DO_Set( board, STX104_DO3, dout3_state );
#endif
	error_code = Menu_Command( stx104_fast_menu_list, board, in_file, out_file, ds );

#if STX104_FAST_MAIN_FUNCTION_EACH_TIME_MEASURE == 1
# if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	dout3_state = 0;
	STX104_DO_Set( board, STX104_DO3, dout3_state );
# endif
#endif

#if STX104_FAST_MAIN_FUNCTION_EACH_TIME_MEASURE == 1
# if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	dout3_state = 1;
	STX104_DO_Set( board, STX104_DO3, dout3_state );
# endif
#endif
	/* display status and updated information */
	Menu_Data( stx104_fast_data_list, 0, board, out_file, ds );
#if STX104_FAST_MAIN_FUNCTION_EACH_TIME_MEASURE == 1
# if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	dout3_state = 0;
	STX104_DO_Set( board, STX104_DO3, dout3_state );
# endif
#endif

#if STX104_FAST_MAIN_FUNCTION_EACH_TIME_MEASURE == 1
# if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	dout3_state = 1;
	STX104_DO_Set( board, STX104_DO3, dout3_state );
# endif
#endif
 #ifndef  STX104_FAST_USE_INTERRUPTS
	/* collect more data in a non-interrupt situation */
	frame_count = STX104_Fast_Process_Data( ds );
 #endif
#if STX104_FAST_MAIN_FUNCTION_EACH_TIME_MEASURE == 1
# if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	dout3_state = 0;
	STX104_DO_Set( board, STX104_DO3, dout3_state );
# endif
#endif


#if STX104_FAST_MAIN_FUNCTION_EACH_TIME_MEASURE == 1
# if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	dout3_state = 1;
	STX104_DO_Set( board, STX104_DO3, dout3_state );
# endif
#endif
	STX104_Fast_Process_Stop( ds );

#if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	dout3_state = 0;
	STX104_DO_Set( board, STX104_DO3, dout3_state );
#endif

#undef STX104_FAST_MAIN_FUNCTION_EACH_TIME_MEASURE

	return error_code;
}
#else
int STX104_Fast_Main_Loop( void * private_data )
{
	int 						error_code;
	int    						board;
	FILE * 						out_file;
	FILE * 						in_file;
	struct demo_stx104_data * 	ds;
	int							frame_count;  /* indicates how many frames per loop, debug use only */


	ds = (struct demo_stx104_data *) private_data;
	out_file= ds->out_file;
	in_file = ds->in_file;
	board 	= ds->board;
 #if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	ds->dout3_state = !ds->dout3_state;
	STX104_DO_Set( board, STX104_DO3, ds->dout3_state );
 #endif
	error_code = Menu_Command( stx104_fast_menu_list, board, in_file, out_file, ds );
	/* display status and updated information */
	Menu_Data( stx104_fast_data_list, 0, board, out_file, ds );
 #ifndef  STX104_FAST_USE_INTERRUPTS
	/* collect more data in a non-interrupt situation */
	frame_count = STX104_Fast_Process_Data( ds );
 #endif
	STX104_Fast_Process_Stop( ds );
 #if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	ds->dout3_state = !ds->dout3_state;
	STX104_DO_Set( board, STX104_DO3, ds->dout3_state );
 #endif
 #ifdef __linux__
	usleep( ds->main_loop_time_usec ); /* we need to multiply by 1000 to convert to microseconds */
 #else
	Sleep( MENU_UPDATE_TIME_MS );
 #endif
	return error_code;
}
#endif

/**********************************************************************//**
 * @brief
 *
 * @param argc  not currently used
 * @param argv  not currently used
 * @return SUCCESS (0) if no errors encountered, otherwise errors are reported
 * as a negative value.
 */
int main(int argc, char* argv[])
{
	int    						error_code;
	int 						board;
	FILE *  					out_file;
	/*FILE *  					in_file;*/
	struct demo_stx104_data * 	ds;
	char   						prefix[] = "  ";

	ds = &demo_stx104_data;

	/**** initialize global variables shared ****/
	memset( ds, 0, sizeof( struct demo_stx104_data ) );
	ds->in_file  = stdin;
	ds->out_file = stdout;
	/* local file settings -- short hand */
	out_file = ds->out_file;
	/*in_file  = ds->in_file;*/
	fprintf( out_file, "Main: begin:\n" );
	/***** initialize driver and board software interface *****/
	error_code = Demo_STX104_Initialize( prefix,   /* output string prefix string  */
										 argc,     /* command line arguments count */
										 argv,     /* command line arguments list  */
										 &demo_stx104_data       /* data */
								       );
	if ( error_code ) goto Main_Error;
	fprintf( out_file, "Main: Demo_STX104_Initialize() complete\n" );
	board = ds->board;  /* short hand */

	/***** menu timing calculations *****/
	error_code = Demo_STX104_Main_Loop_Timing_Calculations( MENU_UPDATE_TIME_MS, ds );
	if ( error_code ) goto Main_Error;

	/***** stx104 board level configuration *****/
/* TODO:	error_code = STX104_Fast_Configuration( prefix, &demo_stx104_data );
	if ( error_code ) goto Main_Error;
	fprintf( out_file, "Main: STX104_Fast_Configuration() complete\n" );
*/

	/***** menu initialization *****/
	Menu_Initialization( stx104_fast_menu_list );

	/***** Main loop here *****/
	fprintf( out_file, "Main: process loop begin\n\n" );
	Menu_Display( stx104_fast_menu_list, board, out_file, ds );

#if MENU_MAIN_LOOP_USE_TIMER_RATHER_THAN_SLEEP_DELAYS_IMPROVED_TIMING == 1
	error_code = Menu_Periodic_Run( ds->main_loop_time_sec,
									STX104_Fast_Main_Loop,
									STX104_Fast_Main_Stop_Request,
									ds
								  );
	while(1)
	{
		if ( true == ds->quit_request )
		{
			STX104_Fast_Process_Stop( ds );
			break;
		}
		/* keeps cpu available */
		usleep(1);
	}
#else
	do
	{  /* process any incoming menu commands */
		error_code = STX104_Fast_Main_Loop( ds );
	} while ( !error_code );
#endif
	STX104_Fast_Process_Stop( ds );
	if ( error_code ) goto Main_Error;

	fprintf( out_file, "\n\nMain: process loop end\n" );
Main_Error:
	if ( error_code < 0 ) error_code = -error_code;
	if ( error_code )
	{
	printf( "Application Test: exit fail, error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );
	}
	Demo_STX104_Terminate( prefix,    		/* output string prefix string  */
					       ds               /* data */
				         );
	return -error_code;
}

/******************  END -- MAIN PORTION OF CODE (TOP LEVEL)  ***************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/


/*<INTERNAL_BEGIN>*/
#if(0)  /********************************************* OLD STUFF - BEGIN */



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

#if defined( DO_NOT_USE_INTERRUPTS )
#else
	STX104_INT_CFG_TYPE int_cfg;
#endif

	out_file = ds->out_file;

	/* open output files */
	if ( !STX104_Fast_Files_Open( ds ) )  return( false );


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


	/****************************/
	Board_Initialization( ds );

	/****************************/
	/*ds->software_frame_count     = 0;*/
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
	/* TODO: copy to cfg */
#endif
	return true;
}





/***************************************************************************/
void STX104_Fast_Files_Close( struct demo_stx104_data * ds )
{
	int64_t delta_usec;
	double  frames_per_sec;
	FILE * out_file;

	out_file = ds->out_file;

	fprintf( out_file, "\n\nEnding data collection, writing to file..." );
	while( STX104_Fast_Process_Data( true, ds ) );



	fprintf( out_file, "\n\n" );
#if (0)
	if ( NULL == ds->data_output_file )
	{
		/* write out any remaining data */
		do {
			ec_stx104 = STX104_AI_Frames_Get(
				board,               /* board descriptor */
				&ai_buffer[0],       /* starting position of the destination buffer */
				&frame_count_actual, /* actual frames that are copied to the buffer */
				frame_quantity_request_per_write_cycle /* frames requested */
				);

			if ( STX104_EC_AI_FIFO_FULL == ec_stx104 )
   		{  /* Don't bother to write anything because at this point we
               now have data loss, better to restart.
            */
            fprintf( out_file, "\nMCF: FIFO full error: " );
            fprintf( out_file, "terminating file write due to data loss.\n\n" );
   			fifo_full_error = true;
            frame_count_actual = 0;
            Board_Initialization( board );
            continue;
   		}
         else
         {
         	fprintf( out_file, "." );

   			if ( software_frame_count_max )
      		{
   				scratch  = software_frame_count;
   				scratch += (uint32_t) frame_count_actual;
      			if ( scratch < software_frame_count_max )
      			{
      				frame_count_write = (size_t) frame_count_actual;
      			}
      			else
      			{
      				scratch = software_frame_count_max - software_frame_count;
         			frame_count_write = (size_t) scratch;
         			scratch = software_frame_count_max;
      			}
      			software_frame_count = scratch;
      		}
      		else
      		{
      			frame_count_write = (size_t) frame_count_actual;
               software_frame_count += (uint32_t) frame_count_actual;
      		}


				fwrite(
					&ai_buffer[0],			/* buffer to send to file */
					frame_size_in_bytes,	/* size of each frame in bytes */
					frame_count_write,	/* number of frames in the buffer */
					data_output_file		/* data file that buffer will be copied to */
					);
         }
		} while ( frame_count_actual );

		/* post totals */
		fprintf( out_file, "\n" );
		STX104_AI_Frame_Count_Get( board, &frame_count );
		fprintf( out_file, "  Total frames written = %8ld\n", software_frame_count );
		fprintf( out_file, "  Total bytes written  = %8lu\n", software_frame_count * ((uint32_t) frame_size_in_bytes) );
		/* close the file */
		fclose( data_output_file );   data_output_file = NULL;
		fprintf( out_file, "\n\n" );
	}
# if defined( FIFO_LOG_DATA_AVAILABLE )
	if ( fifo_log_file )
	{
		if ( fifo_log_buffer_index )
		{
			fwrite( &fifo_log_buffer[0], fifo_log_buffer_index, sizeof( uint32_t ), fifo_log_file );
		}
		fclose( fifo_log_file );   fifo_log_file = NULL;
	}
# endif
#endif
}



#if(0)
static void STX104_Fast_Time_Print( FILE * out_file, struct timeval * tv )
{
	/*struct timezone tz;*/
	struct tm * 	tm;
	tm = localtime( tv->tv_sec );
	fprintf( out_file, "%d:%02d:%02d %d", tm->tm_hour, tm->tm_min,
			 m->tm_sec, tv.tv_usec );
}

static void STX104_Fast_Time_Print( FILE * out_file, struct timespec * ts )
{
    /* seconds, multiplied with 1 million */
    int64_t usec = ts->tv_sec * 1000000;
    /* Add full microseconds */
    usec += ts->tv_nsec/1000;
    /* round up if necessary */
    if (ts->tv_nsec % 1000 >= 500)
    {
        ++usec;
    }
    fprintf( out_file, "Microseconds: %ld\n", usec );
}
#endif





#endif   /********************************************* OLD STUFF - END */
/*<INTERNAL_END>*/
