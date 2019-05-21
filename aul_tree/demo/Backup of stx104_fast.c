
/********************************************************************************
   STX104_FAST
   -----------

_____________________________________________________________________________
REVISION HISTORY

	June  1, 2009
	June 30, 2009     - Revised to support non-interrupt functionality
					     found a bug in fifo.bin file opening.
	February 22, 2012 -
	November  4, 2013 - updated to support both Borland and Microsoft.
	November  8, 2013 - improve start up initialization
	May      24, 2014 - migration to Linux and new driver/library


_____________________________________________________________________________
DESCRIPTION

	This application is intended to perform fast ADC readout.

	All of the parameters are set via macros below.  There is no command line
	parameters.   In addition, at run time one can set the number of frames
	to capture to a file and manually start and stop data collection.

	Please search for CUSTOMER_TODO to determine all of the macros that can
	be adjusted.  The default values provide an initial low speed rate.

	It is possible at run time to manipulate digital outputs, analog outputs
	and analog input gains.

	This example code is intended to be simple.  There are many ways to implement
	this sort of design with many tradeoffs.  In this case we just want to
	demonstrate short duration data collection.

	Long term data collection would involve continuous appending to a file or
	writing several files of fixed length and then managing the files thereof.

	Note that in this specific case, the rate at which you perform a readout
	and the amount of data samples you request to be read out is a bit of a
	compromise since not all systems will be able to achieve a particular
	readout rate and/or there may be jitter introduced due to other overhead.
	In those cases you will need to increase the requested number of samples
	to be read out each cycle in order to compensate for the timing error.  The
	bottom line is that so long as your AVERAGE readout rate is the same
	AVERAGE rate as data is being sampled, then your system will be able to
	collect data long term without fear of a FIFO full condition.

	For example, if you choose to collect 100 frames per second and you readout
	the frames 10 times per second, then you will want to set the readout to
	something just above the average readout rate.

	Frames to read each cycle = Frames per second / read rate
	                          =  100 / 10 = 10 frames per readout.

	So, in this example you might set the number of requested frames to readout
	at 10 frames.

	uint16_t   	frame_buffer[10];
	uint16_t	frame_count_actual;
	uint16_t	frame_count_request = 10;
	// run this function 10 times per second
	error_code = STX104_AI_Frames_Get( 	stx104_board,
										frame_buffer,
										&frame_count_actual,  //number of frames actually returned
										frame_count_request   // not to exceed count.
									 );


	If your system cannot quite keep up on average, then increase the
	frame_count_request until average throughput is achieved.  In the example
	above, you might need to increase from 10 to 11.

	One way to observe the frame readout rate is to use a digital output, refer
	to the DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE macro below.

_____________________________________________________________________________
ASSUMPTIONS


	1. Compiled using Microsoft Visual C/C++ Studio 2010
	2. Compiled using Borland C++ V5.02 Compiler
	3. Compiled using GCC in Linux.

_____________________________________________________________________________
COPYRIGHT NOTICE:

	Copyright(c) 2013-2014 by Apex Embedded Systems. All rights reserved.
	This document is the confidential property of Apex Embedded Systems
	Any reproduction or dissemination is prohibited unless specifically
	authorized in writing.  Customers are free to use this software to
	develop applications for any Apex Embedded Systems product.

_____________________________________________________________________________
LICENSE:


_____________________________________________________________________________
TODO:

1. Improve error handling and reporting.  It assumes that basic functionality
	   has been worked out using the stx104_simple program, and so some of the
	   driver/library functions we are ignoring the return errors, if any.

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
/* the following includes have been implemented to simplify this file */
#include "../demo_shared/cfgrprt.h"
#include "../demo_shared/config.h"
#include "../demo_shared/menu.h"


/***************************************************************************/
/* Specific to Visual Studio
 * 	If you use sprintf(), strcpy() and/or strcat() you will need to enable
 * 	the following macro.
 */
//#define  _CRT_SECURE_NO_WARNINGS

/***************************************************************************/
/* COMPILER/LINKER NOTES:

   These notes only apply to Visual Studio or Borland.

i) Microsoft Visual C/C++ Compiler Version 6.0.
	COMPLILER: VC6 Settings...C/C++->preprocessor->additional include directories:
	"..\..\auldrv"  (without the quotes)
	LINKER:    VC6 Settings:  Link->General->Object/library modules:
	"..\..\auldrv\auldrv.lib"  (without the quotes)

ii) Using Microsoft Visual C/C++ Studio 2010
	 Project Properties: 
		Configuration Properties -> C/C++ -> General -> Additional Include Directories
				..\..\auldrv;%(AdditionalIncludeDirectories)

		Linker->Input->Additional Dependencies
				..\..\auldrv\auldrv.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)

		Configuration Properties -> Debugging -> Working Directory  $(ProjectDir)

iii) Borland:

		"cw3230.dll" must reside within the application executable folder or be
      in the directory path.

	  Make sure that enums are treated as ints.  Otherwise they are treated
     as bytes which is incompatible with most everything else.

iv) GCC - nothing to do.
*/

/***************************************************************************/
#define STX104_FAST_REV  "$Date: 2014-05-06 17:06:00 -0500 (Tue, 06 May 2014) $"


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***********  BEGIN -- CUSTOMER DEFINED GLOBAL MACROS   ********************/

/***************************************************************************/
/* CUSTOMER_TODO:  	Enable/Disable debug print messages
					NOTE:  also STX104_CONFIG_DEBUG_MESSAGE in
					"../demo_shared/config.h"
*/
#define STX104_FAST_DEBUG_MESSAGE  1
/***************************************************************************/
/* CUSTOMER_TODO: Comment out to not toggle DOUT3 each time we write data
                  to a file.  If there is no data to write, then do not toggle.
*/
#define DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE  1
/***************************************************************************/
/* CUSTOMER_TODO: Uncomment if you wish *NOT* to use interrupts */
#define DO_NOT_USE_INTERRUPTS  1
/***************************************************************************/
/* CUSTOMER_TODO: If you are using audacity to view binary files, uncomment
                  this macro in order to obtain twos complement data.
*/
#define CONVERT_STRAIGHT_BINARY_TO_TWOS_COMPLEMENT 1
/***************************************************************************/
/* CUSTOMER_TODO: Roughly the rate in milliseconds that the menu will be
  	  	  	  	   updated.  This also sets the readout rate of the analog
  	  	  	  	   input FIFO (i.e. STX104_AI_Frames_Get() ).
*/
#define MENU_UPDATE_TIME_MS  100 /* mS */
/***************************************************************************/
/* CUSTOMER_TODO: The number of times per second to write data to a file
 	 	 	 	  will now be same as as the rate to update the menu.
*/
#if defined( DO_NOT_USE_INTERRUPTS )
  #define FILE_OUTPUT_RATE_HZ  ( ((double) 1000) / ((double) MENU_UPDATE_TIME_MS) )
#else
  #define FILE_OUTPUT_RATE_HZ  ( ((double) 1000) / ((double) MENU_UPDATE_TIME_MS) )
#endif 
/***************************************************************************/
/* CUSTOMER_TODO: Analog input frame timer, value is in nano-seconds */
/*                        mmmuuunnn  */
#define AI_FRAME_TIME_NS   50000000
/***************************************************************************/
/* CUSTOMER_TODO: Set the analog input channels to scan */
#define AI_FIRST_CHANNEL  0
#define AI_LAST_CHANNEL   3
/***************************************************************************/
/* CUSTOMER_TODO: Set the destination file name */
#define AI_OUTPUT_FILE_NAME  "out"
/***************************************************************************/
/* CUSTOMER_TODO: Uncomment to enable logging of FIFO statistics.		   */
#define FIFO_LOG_DATA_AVAILABLE 1

/**************  END -- CUSTOMER DEFINED GLOBAL MACROS   *******************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/





/***************************************************************************/
#define HW_DEF_FILE_NAME_SIZE  256
/***************************************************************************/
#if defined( DO_NOT_USE_INTERRUPTS )
 #define AI_MAXIMUM_SAMPLE_BUFFER_SIZE   ( 32768 * 2 - 2 )
// #define AI_MAXIMUM_SAMPLE_BUFFER_SIZE   32768
#else
 #define AI_MAXIMUM_SAMPLE_BUFFER_SIZE   16384
#endif
/***************************************************************************/
#if defined( FIFO_LOG_DATA_AVAILABLE )
 #define FIFO_LOG_FILE_NAME  "fifo"
 #define FIFO_LOG_BUFFER_SIZE  256
#endif
 /***************************************************************************/
#ifndef DO_NOT_USE_INTERRUPTS
# define STX104_FAST_USE_INTERRUPTS  1
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
/*#define NULL_CHARACTER 0x00*/
/*#define DISPLAY_LINE_LENGTH 80*/

/***************************************************************************/
/***************************************************************************/
/***** Dataset definition	*****/
struct stx104_fast_data_private
{
	int			board;        /* must maintain 1 in this list */
	FILE *   	in_file;      /* must maintain 2 in this list */
	FILE *   	out_file;     /* must maintain 3 in this list */

	/***** Configuration information 	*****/
	STX104_CONFIGURATION_TYPE cfg;

	/***** Revision information 	*****/
	/* driver revision */
	unsigned long	aul_software_driver_revision_code;
	/* firmware revision reported by the hardware */
	uint16_t		stx104_firmware_revision;
	/* Number of frame bytes is computed so the event handler does not have
	   to recompute all the time.
	*/
	uint16_t 		frame_size_in_bytes;
	uint16_t 		frame_quantity_request_per_write_cycle;
	uint32_t 		software_frame_count_max;
	/*
	If not-zero then stop sampling upon maximum frame count (auto-trigger stop)
	*/
	uint32_t 		software_frame_count;

	BOOL 			active_sampling_interlock;
	BOOL			active_sampling_interlock_prev;
	/* used to indicate that a fifo_full_error has occurred */
	BOOL  			fifo_full_error;

#if defined( CONVERT_STRAIGHT_BINARY_TO_TWOS_COMPLEMENT )
	STX104_AI_ENCODING_ENUM 	ai_cfg_encoding;
#endif
#if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	BOOL 						dout3_state;
#endif
#if defined( FIFO_LOG_DATA_AVAILABLE )
	FILE * 			fifo_log_file;
	int 			fifo_log_buffer_index;
	uint32_t 		fifo_log_buffer[FIFO_LOG_BUFFER_SIZE];
#endif
	FILE *			data_output_file;
};
typedef struct stx104_fast_data_private stx104_fast_data_private_type;
/***************************************************************************/
static stx104_fast_data_private_type  stx104_fast_data_private;
/***************************************************************************/
static int Main_Process_Data( BOOL terminate, stx104_fast_data_private_type * ds );
static char STX104_Fast_Timer_Tick_State( void );
/* TODO: void CallBack_Source_A( void * result ); */

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/



/***************************************************************************/
BOOL Menu_Open_Files( stx104_fast_data_private_type * ds  )
{
#if defined( _WIN32 )
	errno_t  error_code;
#endif
	char file_name[FILE_NAME_SIZE];
	FILE * out_file;

	out_file = ds->out_file;

	/* open data file */
#if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, AI_OUTPUT_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".bin" );
	error_code = fopen_s( &data_output_file, file_name, "wb" );
#else
	strcpy( file_name, AI_OUTPUT_FILE_NAME );
	strcat( file_name, ".bin" );
	ds->data_output_file = fopen( file_name, "wb" );
#endif

	if ( NULL == ds->data_output_file )
	{
		fprintf( out_file, "Cannot open data output file.\n" );
		return false;
	}

#if defined( FIFO_LOG_DATA_AVAILABLE )

# if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, FIFO_LOG_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".bin" );
	error_code = fopen_s( &(ds->fifo_log_file), file_name, "wb" );
# else
	strcpy( file_name, FIFO_LOG_FILE_NAME );
	strcat( file_name, ".bin" );
	ds->fifo_log_file = fopen( file_name, "wb" );
# endif
	if ( NULL == ds->fifo_log_file )
	{
		fprintf( out_file, "Cannot open FIFO log output file.\n" );
		return false;
	}
	ds->fifo_log_buffer_index = 0;
#endif
	return true;
}
/***************************************************************************/
void Menu_Close_Files( stx104_fast_data_private_type * ds )
{
	FILE * out_file;

	out_file = ds->out_file;

	fprintf( out_file, "\n\nEnding data collection, writing to file..." );
	while( Main_Process_Data( true, ds ) );
	/* post totals */
	fprintf( out_file, "\n" );
	/* STX104_AI_Frame_Count_Get( board, &frame_count ); */
#if defined( __BORLANDC__ )
	fprintf( out_file, "  Total frames written = %8ld\n", ds->software_frame_count );
	fprintf( out_file, "  Total bytes written  = %8lu\n", ds->software_frame_count * ((uint32_t) ds->frame_size_in_bytes) );
#else
	fprintf( out_file, "  Total frames written = %8d\n", ds->software_frame_count );
	fprintf( out_file, "  Total bytes written  = %8u\n", ds->software_frame_count * ((uint32_t) ds->frame_size_in_bytes) );
#endif
	/* close the file */
	if ( NULL == ds->data_output_file )
	{
		fclose( ds->data_output_file );
		ds->data_output_file = NULL;
	}
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
/***************************************************************************

Initialization sequence:
	1. turn off interrupts first, just in case they are active.
	2. initialize triggering, make sure triggering has been stopped.
	3. Initialize analog section
	4. Initialize digital I/O section

Admittedly, there is no error handling here.  The purpose is to illustrate
the main points of initialization.
*/
static void Board_Initialization( stx104_fast_data_private_type * ds )
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
BOOL Menu_Board_Initialization( stx104_fast_data_private_type * ds )
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
/***************************************************************************/
void Menu_Board_Termination( stx104_fast_data_private_type * ds )
{
	int error_code; /* for debug */
	/* shut down events (interrupts) */
	error_code = STX104_Int_Enable_Set( ds->board, false );
	Menu_Close_Files( ds );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/



/***************************************************************************/
/***************************************************************************/
/***************************************************************************/










#if(0)
/***************************************************************************/
void Menu_Display_Clear_Line( FILE * out_file )
{
	char out_msg[DISPLAY_LINE_LENGTH];
	int cp;
	for( cp=0; cp<DISPLAY_LINE_LENGTH; cp++ ) out_msg[cp] = ' ';
	out_msg[DISPLAY_LINE_LENGTH-1] = NULL_CHARACTER;
	fprintf( out_file, "%s\r", out_msg );
}
#endif
#if(0)
/***************************************************************************/
void Menu_Display_Data( int board, FILE * in_file, FILE * out_file )
{
	int					error_code; /* used for debug purposes */
	STX104_GEN_JMP_TYPE jumpers;
	STX104_DI_ENUM di_chan;
	BOOL di_value;
	char out_msg[DISPLAY_LINE_LENGTH];
	char tmp_str[LOCAL_STRING_BUFFER_SIZE];
	stx104_ai_cfg_type ai_cfg;
	uint32_t fifo_count;
	uint32_t frame_count;
	BOOL   trigger_enabled;
	(void) in_file;

	/* shows that the software is running */
	out_msg[0] = timer_tick[timer_index];
	out_msg[1] = NULL_CHARACTER;
	/* get the digital input values */
#if defined( _WIN32 )
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, " DI:" );
#else
	strcat( out_msg, " DI:" );
#endif
	for ( di_chan=STX104_DI3; di_chan>=STX104_DI0; di_chan-- )
	{
		error_code = STX104_DI_Get( board, di_chan, &di_value );
#if defined( _WIN32 )
		if ( di_value ) strcat_s( out_msg, DISPLAY_LINE_LENGTH, "1" );
		else            strcat_s( out_msg, DISPLAY_LINE_LENGTH, "0" );
#else
		if ( di_value ) strcat( out_msg, "1" );
		else            strcat( out_msg, "0" );
#endif
	}

#if defined( __BORLANDC__ ) || defined( __linux__ )
	strcat( out_msg, "  AI: " );
	/* get the jumper values UNI/BIP, SE/DIF */
	error_code = STX104_Gen_Jumpers_Get( board, &jumpers );

	if ( jumpers.J9_ai_unipolar ) strcat( out_msg, "UNI" );
	else                          strcat( out_msg, "BIP" );

	strcat( out_msg, " " );

	if ( jumpers.J8_ai_differential ) strcat( out_msg, "DIF" );
	else                              strcat( out_msg, "SE " );

	strcat( out_msg, " " );

	/* get the current ADC value */
	STX104_AI_CFG_Get( board, &ai_cfg );
	switch( ai_cfg.gain )
	{
		case ACFG_GAIN_10V:   strcat( out_msg, "  10V " ); break;
		case ACFG_GAIN_5V:    strcat( out_msg, "   5V " ); break;
		case ACFG_GAIN_2_5V:  strcat( out_msg, "2.5 V " ); break;
		case ACFG_GAIN_1_25V: strcat( out_msg, "1.25V " ); break;
  	}

	/* post first channel */
	strcat( out_msg, "FC:" );
	sprintf( tmp_str, "%02d", ai_cfg.first_channel );
	strcat( out_msg, tmp_str );
	strcat( out_msg, "  " );
	/* post last channel */
	strcat( out_msg, "LC:" );
	sprintf( tmp_str, "%02d", ai_cfg.last_channel );
	strcat( out_msg, tmp_str );
	strcat( out_msg, "  " );
	/* get the trigger status (true => we are collecting data) */
	error_code = STX104_Trg_Active_Get( board, &trigger_enabled );
	strcat( out_msg, "TRG:" );
	if ( trigger_enabled ) strcat( out_msg, "1" );
	else                   strcat( out_msg, "0" );
	/* post the frame count */
	error_code = STX104_AI_Frame_Count_Get( board, &frame_count );

# if defined( __BORLANDC__ )
	sprintf( tmp_str, "  FRM:% 10ld", frame_count );
# else
	sprintf( tmp_str, "  FRM:% 10d", frame_count );
# endif
	strcat( out_msg, tmp_str );

	/* post the FIFO Data available count */
	error_code = STX104_AI_FIFO_Data_Count_Get( board, &fifo_count );
# if defined( __BORLANDC__ )
	sprintf( tmp_str, "  FD:%8ld", fifo_count );
# else
	sprintf( tmp_str, "  FD:%8d", fifo_count );
# endif
	strcat( out_msg, tmp_str );
	strcat( out_msg, " " );
#else
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, "  AI: " );
	/* get the jumper values UNI/BIP, SE/DIF */
	error_code = STX104_Gen_Jumpers_Get( board, &jumpers );

	if ( jumpers.J9_ai_unipolar ) strcat_s( out_msg, DISPLAY_LINE_LENGTH, "UNI" );
	else                          strcat_s( out_msg, DISPLAY_LINE_LENGTH, "BIP" );

	strcat_s( out_msg, DISPLAY_LINE_LENGTH, " " );

	if ( jumpers.J8_ai_differential ) strcat_s( out_msg, DISPLAY_LINE_LENGTH, "DIF" );
	else                              strcat_s( out_msg, DISPLAY_LINE_LENGTH, "SE " );

	strcat_s( out_msg, DISPLAY_LINE_LENGTH, " " );

	/* get the current ADC value */
	error_code = STX104_AI_CFG_Get( board, &ai_cfg );
	switch( ai_cfg.gain )
	{
		case ACFG_GAIN_10V:   strcat_s( out_msg, DISPLAY_LINE_LENGTH, "  10V " ); break;
		case ACFG_GAIN_5V:    strcat_s( out_msg, DISPLAY_LINE_LENGTH, "   5V " ); break;
		case ACFG_GAIN_2_5V:  strcat_s( out_msg, DISPLAY_LINE_LENGTH, "2.5 V " ); break;
		case ACFG_GAIN_1_25V: strcat_s( out_msg, DISPLAY_LINE_LENGTH, "1.25V " ); break;
	}
	/* post first channel */
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, "FC:" );
	sprintf_s( tmp_str, LOCAL_STRING_BUFFER_SIZE, "%02d", ai_cfg.first_channel );
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, tmp_str );
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, "  " );
	/* post last channel */
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, "LC:" );
	sprintf_s( tmp_str, LOCAL_STRING_BUFFER_SIZE, "%02d", ai_cfg.last_channel );
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, tmp_str );
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, "  " );
	/* get the trigger status (true => we are collecting data) */
	error_code = STX104_Trg_Active_Get( board, &trigger_enabled );
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, "TRG:" );
	if ( trigger_enabled ) strcat_s( out_msg, DISPLAY_LINE_LENGTH, "1" );
	else                   strcat_s( out_msg, DISPLAY_LINE_LENGTH, "0" );
	/* post the frame count */
	error_code = STX104_AI_Frame_Count_Get( board, &frame_count );
	sprintf_s( tmp_str, LOCAL_STRING_BUFFER_SIZE, "  FRM:% 10ld", frame_count );
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, tmp_str );
	/* post the FIFO Data available count */
	error_code = STX104_AI_FIFO_Data_Count_Get( board, &fifo_count );
	sprintf_s( tmp_str, LOCAL_STRING_BUFFER_SIZE, "  FD:%8ld", fifo_count );
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, tmp_str );
	strcat_s( out_msg, DISPLAY_LINE_LENGTH, " " );
#endif
	fprintf( out_file, "%s\r", out_msg );
/*   fprintf( out_file, "%s\n", out_msg ); */
}
#endif

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/*************** BEGIN -- MENU DISPLAY AND COMMAND PROCESSING **************/


/***************************************************************************/
/*
	            < < < <   	MENU COMMAND AND DISPLAY    > > > >
*/
/***************************************************************************/
static MENU_COMMAND_TYPE stx104_fast_menu_list[];

/****  The MENU_COMMAND_PARAMS_TYPE can be found in menu.h */
/***************************************************************************/
static int STX104_Fast_Menu_Frame_Max_Set( MENU_COMMAND_PARAMS_TYPE * p )
{
	stx104_fast_data_private_type * ds;  /* data set */
	uint32_t 	value;
	ds = (stx104_fast_data_private_type *) p->info;
	value = Menu_Ask_Uint32( p->in_file, p->out_file, "\n\nFrame count (0=manual start/stop)>" );
	if ( value > ( UINT32_MAX / 2 ) ) ds->software_frame_count_max = ( UINT32_MAX / 2 );
	else							  ds->software_frame_count_max = value;

	fprintf( p->out_file, "\n" );
	return MENU_OPTION_OK;
}
/***************************************************************************/
static int STX104_Fast_Menu_Trigger_Start( MENU_COMMAND_PARAMS_TYPE * p )
{
	stx104_fast_data_private_type * ds;  /* data set */
	BOOL not_done;
	ds = (stx104_fast_data_private_type *) p->info;
	not_done = Menu_Board_Initialization( ds );
	if ( not_done )
	{
		STX104_Trg_Software_Start( p->board );
		ds->active_sampling_interlock = true;
	}
	return MENU_OPTION_OK;
}
/***************************************************************************/
static int STX104_Fast_Menu_Trigger_Stop( MENU_COMMAND_PARAMS_TYPE * p )
{
	stx104_fast_data_private_type * ds;  /* data set */
	ds = (stx104_fast_data_private_type *) p->info;
	STX104_Trg_Software_Stop( p->board );
	ds->active_sampling_interlock = false;
	return MENU_OPTION_OK;
}
/***************************************************************************/
#if defined( FIFO_LOG_DATA_AVAILABLE )
static int STX104_Fast_Convert_FIFO_Log_File_To_CSV( MENU_COMMAND_PARAMS_TYPE * p )
{
	stx104_fast_data_private_type * ds;  /* data set */
	FILE * 		csv_file;
	uint32_t 	fifo_count;
	char 		file_name[FILE_NAME_SIZE];
	uint32_t 	row_count;
#if defined( __BORLANDC__ ) || defined( __linux__ )
#else
	errno_t  error_code;
#endif
	ds = (stx104_fast_data_private_type *) p->info;
	/* open data file */
#if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, FIFO_LOG_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".bin" );
	error_code = fopen_s( &(ds->fifo_log_file), file_name, "rb" );
#else
	strcpy( file_name, FIFO_LOG_FILE_NAME );
	strcat( file_name, ".bin" );
	ds->fifo_log_file = fopen( file_name, "rb" );
#endif
	if ( NULL == ds->fifo_log_file )
	{
		fprintf( ds->out_file, "Cannot open FIFO log output file.\n" );
		return MENU_OPTION_QUIT;
	}
#if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, FIFO_LOG_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".csv" );
	error_code = fopen_s( &csv_file, file_name, "wt" );
#else
	strcpy( file_name, FIFO_LOG_FILE_NAME );
	strcat( file_name, ".csv" );
	csv_file = fopen( file_name, "wt" );
#endif
	if ( NULL == csv_file )
	{
		fclose( ds->fifo_log_file );  ds->fifo_log_file = NULL;
		fprintf( ds->out_file, "Cannot open file CSV file.\n" );
		return MENU_OPTION_QUIT;
	}
	fprintf( ds->out_file, "\n" );
	fprintf( ds->out_file, "Log File CSV Dump:\n" );
	row_count = 0;

	while( (!feof(ds->fifo_log_file)) && (row_count <= 65534) )
	{
		fread( &fifo_count, sizeof( uint32_t ), 1, ds->fifo_log_file );
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
			fprintf( ds->out_file, "\r%c", STX104_Fast_Timer_Tick_State() );
		}
		row_count++;
	}
	fclose( ds->fifo_log_file );   ds->fifo_log_file = NULL;
	fclose( csv_file );
	return MENU_OPTION_OK;
}
#endif
/***************************************************************************/
static int STX104_Fast_Convert_Data_File_To_CSV( MENU_COMMAND_PARAMS_TYPE * p )
{
	stx104_fast_data_private_type * ds;  /* data set */
	FILE * 		csv_file;
	char 		file_name[FILE_NAME_SIZE];
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
	ds = (stx104_fast_data_private_type *) p->info;
	/* open data file */
#if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, AI_OUTPUT_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".bin" );
	error_code = fopen_s( &(ds->data_output_file), file_name, "rb" );
#else
	strcpy( file_name, AI_OUTPUT_FILE_NAME );
	strcat( file_name, ".bin" );
	ds->data_output_file = fopen( file_name, "rb" );
#endif
	if ( NULL == ds->data_output_file )
	{
		fprintf( ds->out_file, "Cannot open data output file.\n" );
		return MENU_OPTION_QUIT;
	}
#if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, AI_OUTPUT_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".csv" );
	error_code = fopen_s( &csv_file, file_name, "wt" );
#else
	strcpy( file_name, AI_OUTPUT_FILE_NAME );
	strcat( file_name, ".csv" );
	csv_file = fopen( file_name, "wt" );
#endif
	if ( NULL == csv_file )
	{
		fclose( ds->data_output_file );   ds->data_output_file = NULL;
		fprintf( ds->out_file, "Cannot open file CSV file.\n" );
		return MENU_OPTION_QUIT;
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
			fprintf( ds->out_file, "\r%c", STX104_Fast_Timer_Tick_State() );
		}

		fwrite( &out_str[0], sizeof( char ), strlen( out_str ), csv_file );
		frame_count++;
	}
	/* close the file */
	fclose( ds->data_output_file );  ds->data_output_file = NULL;
	fclose( csv_file );
	return MENU_OPTION_OK;
}
/***************************************************************************/
static int STX104_Fast_Revisions( MENU_COMMAND_PARAMS_TYPE * p )
{
	stx104_fast_data_private_type * ds;  /* data set */
	ds = (stx104_fast_data_private_type *) p->info;
	/* revision information pulled from stx104_simple global data structure */
	fprintf( p->out_file, " Software Driver Revision:    %lu\n",
			 ds->aul_software_driver_revision_code );
	fprintf( p->out_file, " Hardware Firmware Revision:  %04X",
			 ds->stx104_firmware_revision );

	return MENU_OPTION_OK;
}

/***************************************************************************/
static MENU_COMMAND_TYPE stx104_fast_menu_list[] =
{
/* (*) if char_first=0, function=NULL, then char_last is prefix spaces.					*/
/*     if char_last < ' ' then it can be used as an index parameter           		*/
/*   char    char                                                             		*/
/*   first   last(*)    function                      display name            		*/
{   0,     0,  	NULL,							"\n"													},
{   0,     1,  	NULL,							"-------------------------------"						},  /* display only 	*/
{   0,     1,  	NULL,							"  < < <  M E N U  > > >"								},  /* display only 	*/
{   0,     1,  	NULL,							"-------------------------------"						},  /* display only 	*/
{	0,	   0,	STX104_Fast_Revisions,			NULL													},  /* display helper function 		*/
{   0,     4,  	NULL,							"---- triggering -----"		 							},  /* display only 	*/
{ 'a',	   0,	STX104_Fast_Menu_Frame_Max_Set,	" number of frames to collect"							},
{ 's',	   0,	STX104_Fast_Menu_Trigger_Start,	" begin or start analog input scan (s/w trig start)"	},
{ 'e',	   0,	STX104_Fast_Menu_Trigger_Stop,	" end or stop analog input scan (s/w trig stop)"		},
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
{ 'f',	   0, 	STX104_Fast_Convert_FIFO_Log_File_To_CSV," Binary FIFO file to CSV file"				},
{ 'd',	   0, 	STX104_Fast_Convert_Data_File_To_CSV,	 " Binary data file to CSV file"				},
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

/***************************************************************************/
#define     TIMER_INDEX_MAX  3
static int  timer_index;
static char timer_tick[4] = { '/', '-', '\\', '|' };
/***************************************************************************/
static char STX104_Fast_Timer_Tick_State( void )
{
	if ( timer_index == TIMER_INDEX_MAX ) timer_index = 0;
	else                                  timer_index++;
	return timer_tick[timer_index];
}
/***************************************************************************/
static int STX104_Fast_Data_Timer_Tick( MENU_DATA_PARAMS_TYPE * mdp )
{
	/* shows that the software is running */
	mdp->out_str[mdp->column] = STX104_Fast_Timer_Tick_State();
	mdp->column++;
	return SUCCESS;
}
/***************************************************************************/
static int STX104_Fast_Data_DI( MENU_DATA_PARAMS_TYPE * mdp )
{
	stx104_fast_data_private_type * ds;
	BOOL value;
	int  channel;
	int  channel_count;

	ds = (stx104_fast_data_private_type *) mdp->info;
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
static int STX104_Fast_Data_AI( MENU_DATA_PARAMS_TYPE * mdp )
{
	stx104_fast_data_private_type * ds;
	stx104_ai_cfg_type 				ai_cfg;
	STX104_GEN_JMP_TYPE 			jumpers;

	ds = (stx104_fast_data_private_type *) mdp->info;

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
static int STX104_Fast_Data_Trig( MENU_DATA_PARAMS_TYPE * mdp )
{
	int 	error_code;
	BOOL   	trigger_enabled;
	stx104_fast_data_private_type * ds;

	ds 			= (stx104_fast_data_private_type *) mdp->info;
	error_code 	= STX104_Trg_Active_Get( ds->board, &trigger_enabled );
	if ( trigger_enabled ) Menu_Data_Append( mdp, "1" );
	else                   Menu_Data_Append( mdp, "0" );
	return SUCCESS;
}

/***************************************************************************/
static MENU_DATA_TYPE stx104_fast_data_list[] =
{
	/*	column	max_width	display function				 Prefix length		Prefix  		*/
	{	0,			0,		STX104_Fast_Data_Timer_Tick,	  0,				NULL	  		},
#if defined( STX104_FAST_USE_INTERRUPTS )
	{	2,			0,		STX104_Simple_Interrupt_Status,	  0,				NULL	  		},
#endif
	{	5,			0,		STX104_Fast_Data_DI,			  3,				"DI:"	  		},
	{  14,			0,		STX104_Fast_Data_AI,			  3,				"AI:"	  		},
	{  32,			0,		STX104_Fast_Data_Trig,	  		  4,				"TRG:"	  		},
#if(0)
	{  37,			0,		STX104_Simple_Data_Frame,		  3,				"FR:"	  		},
	{  43,			0,		STX104_Simple_Data_AI_Value,	  0,				NULL	  		},
#endif
	{	0,			0,		NULL,							  0,				NULL	  		}
};

/***************** END -- MENU DISPLAY AND COMMAND PROCESSING ***************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/


#if(0)
/***************************************************************************/
/***************************************************************************/
void Menu_Post( FILE * out_file )
{
	fprintf( out_file, "\n\n\n" );
	fprintf( out_file, "-------------------------------\n" );
	fprintf( out_file, "  < < <  M E N U  > > > \n");
	fprintf( out_file, "-------------------------------\n" );
	fprintf( out_file, "     --- triggering ---\n" );
#if defined( __BORLANDC__ )
	fprintf( out_file, "   a) number of frames to collect       frames = %lu\n", software_frame_count_max );
#else
	fprintf( out_file, "   a) number of frames to collect       frames = %u\n", software_frame_count_max );
#endif
	fprintf( out_file, "   b) begin or start analog input scan\n" );
	fprintf( out_file, "   e) end or stop analog input scan\n" );
	fprintf( out_file, "     --- Analog Inputs (AI) ---\n" );
	fprintf( out_file, "   w)     +10.0V   range (PGA = 00)\n");
	fprintf( out_file, "   x)      +5.0V   range (PGA = 01)\n");
	fprintf( out_file, "   y)      +2.5V   range (PGA = 10)\n");
	fprintf( out_file, "   z)      +1.25V  range (PGA = 11)\n");
	fprintf( out_file, "     --- Analog Outputs (AO) ---\n" );
	fprintf( out_file, "   o)     write to channel-a\n");
	fprintf( out_file, "   p)     write to channel-b\n");
	fprintf( out_file, "     --- Digital Outputs (DO) ---\n" );
	fprintf( out_file, "   g)     write to all\n");
	fprintf( out_file, " h-k)     write to channel (DO0 to DO3)\n");
	fprintf( out_file, "     --- File Processing ---\n" );
#if defined( FIFO_LOG_DATA_AVAILABLE )
	fprintf( out_file, "   f)     convert fifo file to CSV file (TRG status must be zero)\n");
#endif
	fprintf( out_file, "   d)     convert data file to CSV file (TRG status must be zero)\n");
	fprintf( out_file, "     --- Other ---\n" );
	fprintf( out_file, "   r)     Redraw menu\n");
	fprintf( out_file, "   F)     Status and configuration report to 'report.txt'\n" );
	fprintf( out_file, "   R)     Status and configuration report to console\n" );
	fprintf( out_file, "   q)     Quit this test\n");
	fprintf( out_file, "PRESS KEY TO SELECT MENU ITEM> \n\n" );
	/*fprintf( out_file, "  DI = digital In,   AI = analog in, FC = 1st chan, LC = last chan\n" );*/
	/*fprintf( out_file, " TRG = trig active, FRM = frames,    FD = FIFO data avail\n" );*/
	fprintf( out_file, "\n" );
}
#endif

#if(0)
/***************************************************************************/
#if defined( FIFO_LOG_DATA_AVAILABLE )
void Menu_Convert_FIFO_Log_File_To_CSV( int board, FILE * in_file, FILE * out_file )
{
	FILE* csv_file;
	uint32_t fifo_count;
	char file_name[FILE_NAME_SIZE];
	uint32_t row_count;
#if defined( __BORLANDC__ ) || defined( __linux__ )
#else
	errno_t  error_code;
#endif
	(void) board;
	(void) in_file;

	/* open data file */
#if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, FIFO_LOG_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".bin" );
	error_code = fopen_s( &fifo_log_file, file_name, "rb" );
#else
	strcpy( file_name, FIFO_LOG_FILE_NAME );
	strcat( file_name, ".bin" );
	fifo_log_file = fopen( file_name, "rb" );
#endif
	if ( !fifo_log_file )
	{
		fprintf( out_file, "Cannot open fifo log output file.\n" );
		return;
	}
#if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, FIFO_LOG_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".csv" );
	error_code = fopen_s( &csv_file, file_name, "wt" );
#else
	strcpy( file_name, FIFO_LOG_FILE_NAME );
	strcat( file_name, ".csv" );
	csv_file = fopen( file_name, "wt" );
#endif
	if ( !csv_file )
	{
		fclose( fifo_log_file );  fifo_log_file = NULL;
		fprintf( out_file, "Cannot open file CSV file.\n" );
		return;
	}
	fprintf( out_file, "\n" );
	fprintf( out_file, "Log File CSV Dump:\n" );
	row_count = 0;

	while( (!feof(fifo_log_file)) && (row_count <= 65534) )
	{
		fread( &fifo_count, sizeof( uint32_t ), 1, fifo_log_file );
#if defined( __BORLANDC__ )
		sprintf( file_name, "%lu\n", fifo_count );
#elif defined( __linux__ )
		sprintf( file_name, "%u\n", fifo_count );
#else
		sprintf_s( file_name, FILE_NAME_SIZE, "%lu\n", fifo_count );
#endif
		fwrite( &file_name[0], sizeof( char ), strlen( file_name ), csv_file );

		if ( !(row_count % 256) ) 
		{
			fprintf( out_file, "\r%c", timer_tick[timer_index] );
			Menu_Tick_Status();
		}
		row_count++;
	}
	fclose( fifo_log_file );   fifo_log_file = NULL;
	fclose( csv_file );        csv_file      = NULL;
}
#endif
#endif

#if(0)
/***************************************************************************/
void Menu_Convert_Data_File_To_CSV( int board, FILE * in_file, FILE * out_file )
{
	FILE * csv_file;
	char file_name[FILE_NAME_SIZE];
	char out_str[LOCAL_STRING_BUFFER_SIZE];
	char tmp_str[16];
	uint16_t buf[16];
	long frame_count;
	int  frame_in_bytes_size;
	int chan;
	int channel_max;
#if defined( __BORLANDC__ ) || defined( __linux__ )
#else
	errno_t  error_code;
#endif
	(void) board;
	(void) in_file;

	/* open data file */
#if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, AI_OUTPUT_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".bin" );
	error_code = fopen_s( &data_output_file, file_name, "rb" );
#else
	strcpy( file_name, AI_OUTPUT_FILE_NAME );
	strcat( file_name, ".bin" );
	data_output_file = fopen( file_name, "rb" );
#endif
	if ( !data_output_file )
	{
		fprintf( out_file, "Cannot open data output file.\n" );
		return;
	}
#if defined( _WIN32 )
	strcpy_s( file_name, FILE_NAME_SIZE, AI_OUTPUT_FILE_NAME );
	strcat_s( file_name, FILE_NAME_SIZE, ".csv" );
	error_code = fopen_s( &csv_file, file_name, "wt" );
#else
	strcpy( file_name, AI_OUTPUT_FILE_NAME );
	strcat( file_name, ".csv" );
	csv_file = fopen( file_name, "wt" );
#endif
	if ( !csv_file )
	{
		//09JUN09: Error - fclose( fifo_log_file );
		fclose( data_output_file );   data_output_file = NULL;
		fprintf( out_file, "Cannot open file CSV file.\n" );
		return;
	}

	fprintf( out_file, "\n" );
	fprintf( out_file, "Data File CSV Dump:\n" );

	channel_max         = (int) ( AI_LAST_CHANNEL - AI_FIRST_CHANNEL );
	frame_in_bytes_size = (int) ( ( AI_LAST_CHANNEL - AI_FIRST_CHANNEL + 1 ) * sizeof( uint16_t /* sample data type */ ) );
	frame_count         = 0;
	
	/* NOTE: Excel file limit is 65535 lines */
	while( (!feof(data_output_file)) && (frame_count <= 65534) )
	{
		fread( &buf[0], frame_in_bytes_size, 1, data_output_file );
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
			fprintf( out_file, "\r%c", timer_tick[timer_index] );
			Menu_Tick_Status();
		}

		fwrite( &out_str[0], sizeof( char ), strlen( out_str ), csv_file );
		frame_count++;
	}
	/* close the file */
	fclose( data_output_file );  data_output_file = NULL;
	fclose( csv_file );
}
#endif

#if(0)
/***************************************************************************/
BOOL Menu_Process( int board, FILE * in_file, FILE * out_file )
{
	int  menu_char;
	BOOL not_done;
	MENU_COMMAND_PARAMS_TYPE p;

	not_done  = true;

	if( Menu_Get_Character( &menu_char ) )
	{ /* valid menu command character, now decode */
   	if ( 'a' == menu_char )
      {
      	//Menu_Frame_Max_Set( board, in_file, out_file );
		p.board    = board;
		p.channel  = menu_char - ((int) 'o');
		p.in_file  = in_file;
		p.out_file = out_file;
		p.info     = &stx104_fast_data_private;
		STX104_Fast_Menu_Frame_Max_Set( &p );
      }
		else if ( menu_char == 'b' )
		{
			not_done = Menu_Board_Initialization( &stx104_fast_data_private );
			if ( not_done )
			{
				STX104_Trg_Software_Start( board );
				active_sampling_interlock = true;
			}
		}
		else if ( menu_char == 'e' )
		{
			STX104_Trg_Software_Stop( board );
			active_sampling_interlock = false;
			//Menu_Board_Termination( board, in_file, out_file );
		}
		else if ( (menu_char >= 'o') && (menu_char <= 'p') )
		{
			Menu_Display_Clear_Line( out_file );
			p.board = board;
			p.channel = menu_char - ((int) 'o');
			p.in_file = in_file;
			p.out_file = out_file;
			Menu_AO_Value_Set( &p );
		}
		else if ( (menu_char >= 'w') && (menu_char <= 'z') )
		{
			p.board = board;
			p.channel = menu_char - ((int) 'w');
			p.in_file = in_file;
			p.out_file = out_file;
			Menu_AI_Gain_Set( &p );
		}
		else if ( (menu_char >= 'o') && (menu_char <= 'p') )
		{
			Menu_Display_Clear_Line( out_file );
			p.board = board;
			p.channel = menu_char - ((int) 'o') ;
			p.in_file = in_file;
			p.out_file = out_file;
			Menu_AO_Value_Set( &p );
		}
		else if ( menu_char == 'g' )
		{
			Menu_Display_Clear_Line( out_file );
			p.board = board;
			p.channel = 0;
			p.in_file = in_file;
			p.out_file = out_file;
			Menu_DO_All_Set( &p);
		}
		else if ( (menu_char >= 'h') && (menu_char <= 'k') )
		{
			Menu_Display_Clear_Line( out_file );
			p.board = board;
			p.channel = menu_char - ((int) 'h');
			p.in_file = in_file;
			p.out_file = out_file;
			Menu_DO_Value_Set( &p );
		}
#if defined( FIFO_LOG_DATA_AVAILABLE )
		else if ( menu_char == 'f' )
		{
			if ( !active_sampling_interlock ) 
			{
				Menu_Convert_FIFO_Log_File_To_CSV( board, in_file, out_file );
			}
		}
#endif
		else if ( menu_char == 'd' )
		{
			if ( !active_sampling_interlock ) 
			{
				Menu_Convert_Data_File_To_CSV( board, in_file, out_file );
			}
		}
		else if ( menu_char == 'r' )
		{
			Menu_Post( out_file );
		}
		else if ( menu_char == 'q' )
		{
			not_done = false;
			STX104_Trg_Software_Stop( board );
			active_sampling_interlock = false;
		}
		else if ( 'F' == menu_char )
		{
      		FILE * report_file;
#if defined( _WIN32 )
			errno_t  error_code;
			error_code = fopen_s( &report_file, "report.txt", "wt" );
#else
			report_file = fopen( "report.txt", "wt" );
#endif
			/*STX104_Report_Full( board, in_file, report_file );*/
			STX104_Report_Full( board, report_file );
			/* close the file */
			fclose( report_file );
		}
		else if ( 'R' == menu_char )
		{
			/*STX104_Report_Full( board, in_file, out_file );*/
			STX104_Report_Full( board, out_file );
		}
	}
	/*STX104_Report_Full( board, out_file );*/
	return not_done;
}
#endif
/***************************************************************************/
/***************************************************************************/
#if defined( DO_NOT_USE_INTERRUPTS )
/***************************************************************************/
static int Main_Process_Data( BOOL terminate, stx104_fast_data_private_type * ds )
{
	uint16_t 		frame_count_actual;
	size_t			frame_count_write;
	int		 		error_code;
	int				board;    /* short hand */
	FILE *			out_file; /* short hand */
	uint32_t        scratch;
#if defined( FIFO_LOG_DATA_AVAILABLE )
	uint32_t fifo_count;
#endif
	board    = ds->board;
	out_file = ds->out_file;

	frame_count_write = 0;
	error_code = STX104_AI_Frames_Get( board,               /* board descriptor */
										&ai_buffer[0],       /* starting position of the destination buffer */
										&frame_count_actual, /* actual frames that are copied to the buffer */
										ds->frame_quantity_request_per_write_cycle /* frames requested */
									);
	/*if ( STX104_EC_AI_FIFO_FULL == error_code )*/
	if ( EC_STX104_AI_FIFO_FULL == error_code )
	{  /* Don't bother to write anything because at this point we
			now have data loss, better to restart.
		*/
		if ( terminate )
		{
			fprintf( out_file, "\nMPD: Terminate FIFO full error: " );
			fprintf( out_file, "terminating file write due to data loss.\n\n" );
			ds->fifo_full_error = true;
			frame_count_actual = 0;
			Board_Initialization( ds );
			return 0;
		}
		else
		{
			STX104_Trg_Software_Stop( board );
			stx104_fast_data_private.active_sampling_interlock = false;
			fprintf( out_file, "\n\nMPD: FIFO full error: " );
			fprintf( out_file, "terminating file write due to data loss.\n\n" );
			STX104_Int_Enable_Set( board, false );
			ds->fifo_full_error = true;
			frame_count_actual = 0;
			Board_Initialization( ds );
			return 0;
		}
	}
   	
//printf( "event: frame_count_actual=%d\n", frame_count_actual );

	if ( frame_count_actual )
	{
		if ( ds->software_frame_count_max )
		{
			scratch  = ds->software_frame_count;
			scratch += (uint32_t) frame_count_actual;
			if ( scratch < ds->software_frame_count_max )
			{
				frame_count_write        = (size_t) frame_count_actual;
				ds->software_frame_count = scratch;
			}
			else
			{
				if ( !terminate )
				{
					STX104_Trg_Software_Stop( board );
					stx104_fast_data_private.active_sampling_interlock = false;
				}
				scratch = stx104_fast_data_private.software_frame_count_max - ds->software_frame_count;
				frame_count_write = (size_t) scratch;
				ds->software_frame_count = stx104_fast_data_private.software_frame_count_max;
			}
		}
		else
		{
			frame_count_write         = (size_t) frame_count_actual;
			ds->software_frame_count += (uint32_t) frame_count_actual;
		}

		if ( !terminate )
		{
#if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
			ds->dout3_state = !ds->dout3_state;
			STX104_DO_Set( board, STX104_DO3, ds->dout3_state );
#endif
		}

		if ( frame_count_write )
		{
#if defined( CONVERT_STRAIGHT_BINARY_TO_TWOS_COMPLEMENT )
			if ( ds->stx104_firmware_revision < STX104_REVISION_120328 )
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

			fwrite( 	&ai_buffer[0],				/* buffer to send to file */
						ds->frame_size_in_bytes,	/* size of each frame in bytes */
						frame_count_write,			/* number of frames in the buffer */
						ds->data_output_file		/* data file that buffer will be copied to */
				  );
		}
   }
#if defined( FIFO_LOG_DATA_AVAILABLE )
	if ( terminate )
	{
		STX104_AI_FIFO_Data_Count_Get( board, &fifo_count );
		if ( ds->fifo_log_file )
		{
			if ( ds->fifo_log_buffer_index >= (FIFO_LOG_BUFFER_SIZE - 1) )
			{
				fwrite( &(ds->fifo_log_buffer[0]), sizeof( uint32_t ), ds->fifo_log_buffer_index, ds->fifo_log_file );
				ds->fifo_log_buffer_index = 0;
			}
			ds->fifo_log_buffer[ds->fifo_log_buffer_index] = fifo_count;
			ds->fifo_log_buffer_index++;
		}
	}
	else
	{
		if ( ds->fifo_log_file )
		{
			if ( ds->fifo_log_buffer_index )
			{
				fwrite( &(ds->fifo_log_buffer[0]), sizeof( uint32_t ), ds->fifo_log_buffer_index, ds->fifo_log_file );
			}
			fclose( ds->fifo_log_file );	ds->fifo_log_file = NULL;
		} 		
	}
#endif
	return (int) frame_count_write;
}
/***************************************************************************/
#else
/***************************************************************************/
static void CallBack_Source_A( void* result )
{
	STX104_INT_RESULT_TYPE* event_result;
	UINT16 					frame_count_actual;
	int						error_code;
#if defined( FIFO_LOG_DATA_AVAILABLE )
	uint32_t fifo_count;
#endif

	event_result = (STX104_INT_RESULT_TYPE*) result;

	error_code = STX104_AI_Frames_Get(	event_result->board, /* board descriptor */
										&ai_buffer[0],       /* starting position of the destination buffer */
										&frame_count_actual, /* actual frames that are copied to the buffer */
										frame_quantity_request_per_write_cycle /* frames requested */
									);

	if ( STX104_EC_AI_FIFO_FULL == ec_stx104 )
	{  /* Don't bother to write anything because at this point we
			now have data loss, better to restart.
		*/
		STX104_Int_Enable_Set( board, false );
		STX104_Trg_Software_Stop( board );
		active_sampling_interlock = false;
		//TODO fprintf( out_file, "\n\nCallBack_Source_A: FIFO full error: " );
		//TODO fprintf( out_file, "terminating file write due to data loss.\n" );
		fifo_full_error = true;
		frame_count_actual = 0;
		Board_Initialization( board );
		return;
	}

//printf( "event: frame_count_actual=%d\n", frame_count_actual );
	if ( frame_count_actual )
	{
		#if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
		dout3_state = !dout3_state;
		STX104_DO_Set( board, STX104_DO3, dout3_state );
		#endif

		fwrite( &ai_buffer[0],			/* buffer to send to file */
				frame_size_in_bytes,	/* size of each frame in bytes */
				frame_count_actual,	/* number of frames in the buffer */
				data_output_file		/* data file that buffer will be copied to */
		      );

	}
#if defined( FIFO_LOG_DATA_AVAILABLE )
	STX104_AI_FIFO_Data_Count_Get( event_result->board, &fifo_count );
	if ( fifo_log_file )
	{
		if ( fifo_log_buffer_index >= (FIFO_LOG_BUFFER_SIZE - 1) )
		{
			fwrite( &fifo_log_buffer[0], sizeof( uint32_t ), fifo_log_buffer_index, fifo_log_file );
			fifo_log_buffer_index = 0;
		}

		fifo_log_buffer[fifo_log_buffer_index] = fifo_count;
		fifo_log_buffer_index++;
	}
#endif
}
/***************************************************************************/
#endif
/***************************************************************************/
/***************************************************************************/


/***************************************************************************/

/**************************************************************************
Description:
Brings up the STX104 interface.  First it connects to the driver and then it
opens the board returning the board descriptor.

Note:
The error_codes "error_code" can be used during debug to see
what the functions are returning.
*/
int Main_Initialization( char * prefix,    	/* output string prefix string  */
						 int argc,       	/* command line arguments count */
                         char* argv[],     	/* command line arguments list  */
                         stx104_fast_data_private_type * info  /* shared data */
                       )
{

	int   			error_code;
	int				board;
	FILE *			out_file;
	(void) argc;  /* not used currently */
	(void) argv;
	out_file = info->out_file; /* short hand */
#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "%sInitialization begin:\n", prefix );
#endif
	/**** connect to driver ****/
	/*error_code = AUL_Initialization( "aul_hwdef.txt" );*/
	error_code = AUL_Initialization( NULL );
	if ( error_code ) return error_code;

#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> driver initialize success\n", prefix );
#endif
	error_code = AUL_Revision( &(info->aul_software_driver_revision_code) );
#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> aul_software_driver_revision_code = %u\n",
							prefix, info->aul_software_driver_revision_code );
#endif
	error_code = AUL_Device_Open( "stx104", &board );
	if ( error_code ) return error_code;

	info->board = board;
#if defined( STX104_SIMPLE_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> stx104 device open successfully\n", prefix );
#endif
	error_code = STX104_Gen_Revision_Get( board, &(info->stx104_firmware_revision) );
#if defined( STX104_SIMPLE_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> stx104_firmware_revision = %04X\n",
   												prefix, info->stx104_firmware_revision );
#endif
   return SUCCESS;
}
/**************************************************************************
Description:
Shuts down the STX104 interface.  First it turns off interrupts (if used) and
then it turns off the triggering (optional...will not affect shutting things
down).  Second, it closes the board descriptor and then frees the driver.

Note:
The error_codes "error_code" can be used during debug to see
what the functions are returning.
*/
void Main_Termination( 	char * prefix,    		/* output string prefix string  */
                        stx104_fast_data_private_type * info  /* shared data 	*/
                      )
{
	int 			error_code; /* used primarily with debugger */
	int            	board;
	FILE * 		   	out_file;
#ifndef STX104_FAST_DEBUG_MESSAGE
	(void) prefix;
#endif
	out_file = info->out_file; /* short hand */
	board    = info->board;  	/* short hand */
#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "%sTermination begin:\n", prefix );
#endif
	/* set board to inactive state */
	error_code = STX104_Config_Termination( board, prefix, out_file, &(info->cfg) );
	/* close the device */
	error_code = AUL_Device_Close( board );
#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> AUL_Device_Closed\n", prefix );
#endif
	/* close or free the driver */
	error_code = AUL_Termination();
#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> AUL_Termination\n", prefix );
#endif
	/* close any remaining open files */
#if defined( FIFO_LOG_DATA_AVAILABLE )
	if ( NULL != info->fifo_log_file )
	{
		fclose( info->fifo_log_file );		info->fifo_log_file = NULL;
	}
#endif
	if ( NULL != info->data_output_file )
	{
		fclose( info->data_output_file );		info->data_output_file = NULL;
	}
	if ( NULL != info->out_file )
	{
		fclose( info->out_file );		info->out_file = NULL;
	}
	if ( NULL != info->in_file )
	{
		fclose( info->in_file );		info->in_file = NULL;
	}
}
/***************************************************************************/
int main(int argc, char* argv[])
{
	int 	board;
	int    	error_code;
	FILE *  out_file;
	FILE *  in_file;
	char   	prefix[] = "  ";
	(void) 	argc;
	(void) 	argv;

	/**** initialize global variables shared within STX104_Simple ****/
	memset( &stx104_fast_data_private, 0, sizeof( stx104_fast_data_private_type ) );
	stx104_fast_data_private.in_file  = stdin;
	stx104_fast_data_private.out_file = stdout;

	/* local file settings -- short hand */
	out_file = stx104_fast_data_private.out_file;
	in_file  = stx104_fast_data_private.in_file;

#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "Main: begin:\n" );
#endif
	/***** initialize driver and board software interface *****/
	error_code = Main_Initialization( 	prefix,   /* output string prefix string  */
										argc,     /* command line arguments count */
										argv,     /* command line arguments list  */
										&stx104_fast_data_private       /* data */
									);
	if ( error_code ) goto Main_Error;
#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "Main: Main_Initialization() complete\n" );
#endif
	board = stx104_fast_data_private.board;  /* short hand */

	/***** stx104 board level configuration *****/
//TODO:	error_code = STX104_Fast_Configuration( prefix, &stx104_fast_data_private );
	if ( error_code ) goto Main_Error;
#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "Main: STX104_Fast_Configuration() complete\n" );
#endif

	/***** menu initialization *****/
	Menu_Initialization( stx104_fast_menu_list );

	/***** Main loop here *****/
#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "Main: process loop begin\n\n" );
#endif
	Menu_Display( stx104_fast_menu_list, board, out_file, &stx104_fast_data_private );
#if defined( STX104_FAST_USE_INTERRUPTS )
	error_code = STX104_Int_Enable_Set( board, true );
	if ( error_code ) goto Main_Error;
#endif
	do
	{  /* process any incoming menu commands 		*/
		error_code = Menu_Command( stx104_fast_menu_list, board, in_file,
								   out_file, &stx104_fast_data_private );
		if ( error_code ) break;
		/* display status and updated information */
		Menu_Data( stx104_fast_data_list, 0, board, out_file, &stx104_fast_data_private );
#if defined( DO_NOT_USE_INTERRUPTS )
		error_code = Main_Process_Data( false, &stx104_fast_data_private );
		if ( error_code ) break;
#endif
#ifdef __linux__
		usleep( MENU_UPDATE_TIME_MS * 1000 ); /* we need to multiply by 1000 to convert to microseconds */
#else
		Sleep( MENU_UPDATE_TIME_MS );
#endif
	} while ( !error_code );
	if ( error_code ) goto Main_Error;
#if defined( STX104_FAST_USE_INTERRUPTS )
	error_code = STX104_Int_Enable_Set( board, false );
	if ( error_code ) goto Main_Error;
#endif
#if defined( STX104_FAST_DEBUG_MESSAGE )
	fprintf( out_file, "\n\nMain: process loop end\n" );
#endif



#if(0)
	/* display the menu options */
	Menu_Post( out_file );
	timer_index = 0;
	/* initialize analog inputs, triggers and interrupts to some known state */
	Board_Initialization( board );
	/* process key strokes or commands */
	do {
#if(0)
		not_done = Menu_Process( board, in_file, out_file );
#endif
		/* process any incoming menu commands 		*/
		error_code = Menu_Command( stx104_fast_menu_list, board, in_file,
								   out_file, &stx104_fast_data_private );

		Menu_Tick_Status();
	#if defined( DO_NOT_USE_INTERRUPTS )
		Main_Process_Data( false, &stx104_fast_data_private );
	#endif
		/* if active_sampling_interlock falling edge then process termination */
		if ( ( false == stx104_fast_data_private.active_sampling_interlock )     &&
			( true == stx104_fast_data_private.active_sampling_interlock_prev )   )
		{
			Menu_Board_Termination( board, in_file, out_file );
		}
		stx104_fast_data_private.active_sampling_interlock_prev = stx104_fast_data_private.active_sampling_interlock;
		//_sleep( MENU_UPDATE_TIME_MS );
#ifdef __linux__
		usleep( MENU_UPDATE_TIME_MS * 1000 ); /* we need to multiply by 1000 to convert to microseconds */
#else
		Sleep( MENU_UPDATE_TIME_MS );
#endif
	} while ( not_done );  /* main loop */

	/* shutdown the card */
	Menu_Board_Termination( board, in_file, out_file );
	/* close the device */
	error_code = AUL_Device_Close( board );
#endif




Main_Error:
	if ( error_code < 0 ) error_code = -error_code;
	if ( error_code )
	{
	printf( "Application Test: exit fail, error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );
	}
	Main_Termination( prefix,    				/* output string prefix string  */
					  &stx104_fast_data_private /* data */
				    );
	return -error_code;
}



