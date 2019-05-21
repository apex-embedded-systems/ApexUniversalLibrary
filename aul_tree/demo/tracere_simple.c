/*****************************************************************************//**
 * @file  tracere_simple.c
 * STX104 low speed demonstration
 *
 * @mainpage   STX104 Simple
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
 *
 * 	June      1, 2009 - Original release
 * 	June     30, 2009 - Revised to support non-interrupt functionality
 * 	February 22, 2012 - found a bug in fifo.bin file opening.
 * 	November  4, 2013 - updated to support both Borland and Microsoft.
 *
 * 	November  8, 2013 - improve start up initialization
 * 	May      06, 2014 - Corrected menu revision information, note that pressing 'R'
 * 						will return the correct values.  This is a full report to
 * 						console.
 * 	May      24, 2014 - migration to Linux and new driver/library
 * 	June     13, 2014 - Major house cleaning of the code to bring it up to date
 * 						with most current library revision.  Borland and Microsoft
 * 						has not been tested.
 * 	December  2, 2014 - Update to support Doxygen documentation.  Review.
 *
 *
 * @page page_summary  Summary
 * _____________________________________________________________________________
 * <b>DESCRIPTION</b>
 *
 * This application is intended for simple STX104 I/O testing.  Basically,
 * a low analog input data rate and presentation.
 *
 * All of the parameters are set via macros below.  There is no command line
 * parameters.   In addition, at run time one can set the number of frames
 * to capture to a file and manually start and stop data collection.
 *
 * Please search for CUSTOMER_TODO to determine all of the macros that can
 * be adjusted.  The default values provide an initial low speed rate.
 *
 * It is possible at run time to manipulate digital outputs, analog outputs
 * and analog input gains.
 *
 * This example code is intended to be simple.  There are many ways to implement
 * this sort of design with many tradeoffs.  In this case we just want to
 * demonstrate short duration data collection.
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
 * has been worked out using the tracere_simple program, and so some of the
 * driver/library functions we are ignoring the return errors, if any.
 *
 * 2. Interrupt testing
 *
 *******************************************************************************/


#ifdef _WIN32
# include <windows.h>
# include <stdlib.h>
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



#include "../../aul_tree/aul/aul_public.h"
/* the following includes have been implemented to simplify this file */
#include "../aul_tree/demo_shared/demo_tracere_reports.h"
#include "../aul_tree/demo_shared/demo_tracere_config.h"
#include "../aul_tree/demo_shared/menu.h"
#include "../aul_tree/demo_shared/demo_tracere_shared.h"

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
/***************************************************************************/
/***************************************************************************/
/***********  BEGIN -- CUSTOMER DEFINED GLOBAL MACROS   ********************/

/*****************************************************************************//**
 * @def TRACERE_SIMPLE_DEBUG_MESSAGE
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * Enable/Disable debug print messages NOTE:  also TRACERE_CONFIG_DEBUG_MESSAGE in
 * "../demo_shared/config.h".
*/
#define TRACERE_SIMPLE_DEBUG_MESSAGE  _DEBUG

/*****************************************************************************//**
 * @def TRACERE_SIMPLE_USE_INTERRUPTS
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * 1. Uncomment if you wish to use interrupts
 *
 * 2. TRACERE_CONFIG_INTERRUPTS_ACTIVE in "../demo_shared/config.h" must also be enabled.
 *
 * 3. Comment out for software polling of analog input data.
 *
 * NOTE
 * For many applications, interrupts are not required.  To get started right away we
 * suggest running without interrupts (to minimize possible problems related to interrupts).
*/
/*#define TRACERE_SIMPLE_USE_INTERRUPTS  TRACERE_CONFIG_INTERRUPTS_ACTIVE*/

/*****************************************************************************//**
 * @def MENU_UPDATE_TIME_MS
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * Roughly the rate in milliseconds that the menu will be updated.  Typically,
 * 100mS is about the right value for low speed applications.
 *
 * NOTE: Your mileage will vary.  In other words, depending on CPU, hardware,
 * and your application.
*/
#define MENU_UPDATE_TIME_MS  100 

/*****************************************************************************//**
 * @def MENU_MAIN_LOOP_USE_TIMER_RATHER_THAN_SLEEP_DELAYS_IMPROVED_TIMING
 *
 * <b>CUSTOMER_TODO:</b>
 *
 * Comment out to simply revert to usleep() delays rather than using menu interval
 * timer mechanism based on signals.
*/
#define MENU_MAIN_LOOP_USE_TIMER_RATHER_THAN_SLEEP_DELAYS_IMPROVED_TIMING  1


/**************  END -- CUSTOMER DEFINED GLOBAL MACROS   *******************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/************************* BEGIN -- GLOBAL VARIABLES ***********************/

/***************************************************************************/
#define TRACERE_SIMPLE_REV  "$Date: 2015-01-12 14:07:46 -0600 (Mon, 12 Jan 2015) $"

/***************************************************************************/
static struct demo_tracere_data  demo_tracere_data;

/*************************  END -- GLOBAL VARIABLES  ***********************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/****************** BEGIN -- RUN TIME DATA COLLECTION  *********************/

#if(0)
/**********************************************************************//**
* @brief
* Reads the data from the hardware and deposits into ai_buffer.  In this case
* we just dump all but the last value; one could perform an average.
*
* It assumes that all the data comes from a single channel.  In this example,
* that is indeed the case.
*
* It updates the total number of hardware frames actually read.
*
* @param ds DataSet.  All of the STX104 configuration and control information.
*/
void STX104_Simple_Menu_AI_Data_Readout( struct demo_tracere_data * ds )
{
	int 		error_code;
	int   		board;
	BOOL  		data_available;
	uint16_t 	frame_count_actual;
#define AI_BUFFER_SIZE  256
	uint16_t 	ai_buffer[AI_BUFFER_SIZE];

	ds->fifo_full_error  		= false;
	ds->frames_have_been_read 	= false;

	board 				= ds->board;
	frame_count_actual 	= 0;
	data_available 		= false;
	do {
		error_code = STX104_AI_Frames_Get( board, &ai_buffer[0], &frame_count_actual, AI_BUFFER_SIZE );
		if ( EC_STX104_AI_FIFO_FULL == error_code )
		{
			ds->fifo_full_error = true;
			frame_count_actual = 0;
		}
		else
		{
			ds->fifo_full_error = false;
		}
		if ( frame_count_actual )
		{
			ds->software_frame_count += (uint32_t)frame_count_actual;
			/* Here, we are dumping all but the last sample retrieved.  One could
			   average the data or store to a file.
			*/
			ds->value_raw = ai_buffer[frame_count_actual - 1];
			data_available = true;
		}
	} while ( frame_count_actual );
	ds->frames_have_been_read = data_available;
#undef AI_BUFFER_SIZE
}
#endif
/**************************************************************************
TODO
Description
The interrupt callback function.  This callback function is called using a
separate thread, so code within the callback must be thread-safe.

*/
#if defined( STX104_SIMPLE_USE_INTERRUPTS )
static void CallBack_Source_A( void * result )
{
	STX104_INT_RESULT_TYPE * 		event_data;
	struct demo_tracere_data * 		ds;

	event_data   = (STX104_INT_RESULT_TYPE *)  result;
	ds = (struct demo_tracere_data *) event_data->user_defined_dataset;

	ds->interrupt_blocking = true;
	STX104_Simple_Menu_AI_Data_Readout( ds );
	ds->interrupt_blocking = false;
}
#endif
/******************* END -- RUN TIME DATA COLLECTION  **********************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/********************** BEGIN -- BOARD CONFIGURATION ***********************/

#if defined( TRACERE_SIMPLE_USE_INTERRUPTS )
static const char tracere_simple_interrupt_handler_name[]  = "CallBack_Source_A";
static const char tracere_simple_interrupt_data_set_name[] = "tracere_simple_data_private";
#endif


/**********************************************************************//**
 * @brief
 * Commits the initial configuration information to the library and then
 * onto the hardware.
 *
 * @param  prefix  A string prefix used in reporting any errors.  Typically,
 *                 it is spaces to get proper indent which is purely to
 *                 improve readability of the output.
 * @param  ds	   DataSet.  See demo_tracere_data for further details.
 * @return If successful, returns SUCCESS (0), otherwise an error-code.
 */
int Tracere_Simple_Configuration( char * prefix, struct demo_tracere_data * ds )
{
	int 				 			error_code;
	struct tracere_configuration * 	cfg;
	FILE *							out_file;
	int								board;

	cfg      = &(ds->cfg); 	/* short hand */
	out_file = ds->out_file; /* short hand */
	board    = ds->board;    /* short hand */

	/**** clear the configuration table ****/
	STX104_Config_Clear( cfg );

	/**** initialize interrupts and triggers ****/
	error_code = STX104_Config_Initialize( board, prefix, out_file, cfg );
	if ( SUCCESS != error_code ) return (int) error_code;

	/*********************************************************************/
	/**** set our configuration parameters here for this application  ****/

	/****  Analog Input (AI) configuration ****/
#ifdef TIMER_PARAMETERS_IMPROVE_DYNAMIC_RANGE
	cfg->ai_cfg.burst_timer_sec = 5000e-9; /* nS */
	cfg->ai_cfg.frame_timer_sec = AI_FRAME_TIME_SEC;
#else
	cfg->ai_cfg.burst_timer_ns = 5000; /* nS */
	cfg->ai_cfg.frame_timer_ns = AI_FRAME_TIME_NS;
#endif
	cfg->ai_cfg.strobe_source  = AISS_AI_FRAME_TIMER;
	cfg->ai_cfg.first_channel  = STX104_AI_CH0;
	cfg->ai_cfg.last_channel   = cfg->ai_cfg.first_channel;
	//   ai_cfg.last_channel   = 100; // blatant error test!
	cfg->ai_cfg.use_triggering_subsystem = false;

	/****  Digital Input (DI) configuration ****/
	cfg->di_cfg.swap_din1_and_din3 = true;

	/****  Digital Output (DO) configuration ****/
	// nothing to do here -- we are going to just use a cleared data set.

	/****  Triggering (Trg) configuration ****/
	cfg->trg_cfg.enabled           = true;
	cfg->trg_cfg.start_mode        = STM_IMMEDIATE;
	cfg->trg_cfg.start_sync_source = TSS_NONE;
	cfg->trg_cfg.start_source      = STS_SOFTWARE_STROBE; /* start via software */
	cfg->trg_cfg.stop_source       = ETS_SOFTWARE_STROBE; /* stop via software */
#ifdef TIMER_PARAMETERS_IMPROVE_DYNAMIC_RANGE
	cfg->trg_cfg.delay_sec = 15025e-9; /* nS */
#else
	cfg->trg_cfg.delay_ns = 15025;
#endif

	/****  Interrupts (Int) configuration ****/
#if defined( STX104_SIMPLE_USE_INTERRUPTS )
	cfg->int_cfg.callback_source_a      = (AUL_INT_CALLBACK) CallBack_Source_A;
	cfg->int_cfg.callback_source_a_user_dataset = (void * ) ds;
	cfg->int_cfg.source_a               = ISA_THRESHOLD_COUNTER;
	cfg->int_cfg.threshold              = 10;
	cfg->int_cfg.threshold_source       = ITS_AI_FRAME;
	cfg->name_interrupt_callback_a      = tracere_simple_interrupt_handler_name;
	cfg->name_interrupt_data_set_a      = tracere_simple_interrupt_data_set_name;
#else

#endif
  	/****  commit the configuration ****/
   error_code = STX104_Config_Commit( board, prefix, out_file, cfg );
   if ( SUCCESS != error_code ) return (int) error_code;
   return SUCCESS;
}
/**********************  END -- BOARD CONFIGURATION  ***********************/
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
/**************************************************************************
 * Forward declaration.
 */
static struct menu_command tracere_simple_menu_list[];

#define LOCAL_STRING_BUFFER_SIZE  128

/**********************************************************************//**
 * @brief
 * Set analog input (AI) to display values in hexadecimal value.  This function
 * is referenced in #tracere_simple_menu_list[] array.
 * @param p See menu_command_params for details, see menu.h.
 * @return Always returns SUCCESS (0)
 */
static int Tracere_Simple_Dout( struct menu_command_params * p, BOOL bit_value )
{
	int chan_port;
	int chan_bit;
	struct demo_tracere_data * ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;

	/* assuming 8-bit ports here */
	chan_port = p->channel >> 3;
	chan_bit  = p->channel - ( chan_port << 3 );
	/* mask off the bit */
	ds->dio_port[chan_port] &= (~0x01 << chan_bit );
	if ( bit_value ) ds->dio_port[chan_port] |= 0x01 << chan_bit;
	return Tracere_DIO_Bit_Set( p->board, p->channel, bit_value );
}
static int Tracere_Simple_Menu_DIO_Bit_Set( struct menu_command_params * p )
{
	return Tracere_Simple_Dout( p, true /* set */ );
}
static int Tracere_Simple_Menu_DIO_Bit_Clear( struct menu_command_params * p )
{
	uint16_t value;
	char   question_str[LOCAL_STRING_BUFFER_SIZE];

	Menu_Display_Clear_Line( p->out_file );
#ifdef __BORLANDC__
	sprintf( question_str, "DOUT%d = ", p->channel );
#else
	snprintf( question_str, LOCAL_STRING_BUFFER_SIZE, "DOUT%d = ", p->channel );
#endif
	p->channel = Menu_Ask_Uint16_Range( p->in_file, p->out_file, question_str, 0, ( TRACERE_DIO_BIT_QTY - 1 )  );
	return Tracere_Simple_Dout( p, false /* clear */ );
}



static int Tracere_Simple_Menu_DIO_Bit_Get( struct menu_command_params * p )
{
	int error_code;
	int chan_port;
	int chan_bit;
	BOOL bit_value;
	struct demo_tracere_data * ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;
	/* assuming 8-bit ports here */
	chan_port = p->channel >> 3;
	chan_bit  = p->channel - ( chan_port << 3 );
	/* mask off the bit */
	ds->dio_port[chan_port] &= (~0x01 << chan_bit );
	/* get bit */
	error_code = Tracere_DIO_Bit_Get( p->board, p->channel, &bit_value );
	if ( bit_value ) ds->dio_port[chan_port] |= 0x01 << chan_bit;
	return error_code;
}

static int Tracere_Simple_Menu_DIO_Port_Set( struct menu_command_params * p )
{
	int error_code;
	struct demo_tracere_data * ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;
	return Tracere_DIO_Port_Set( p->board, p->channel, ds->dio_port[p->channel] );
}

static int Tracere_Simple_Menu_DIO_Port_Get( struct menu_command_params * p )
{
	int error_code;
	struct demo_tracere_data * ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;
	return Tracere_DIO_Port_Set( p->board, p->channel, &(ds->dio_port[p->channel]) );
}

static int Tracere_Simple_Menu_LED_Set( struct menu_command_params * p )
{
	int error_code;
	struct demo_tracere_data * ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;
//??	return Tracere_LED_Set( p->board, p->channel, 0 );
}


#if(0)
/**********************************************************************//**
 * @brief
 * Set analog input (AI) display values in uncalibrated volts.  This function
 * is referenced in the #tracere_simple_menu_list[] array.
 * @param p See menu_command_params for details.
 * @return Always returns SUCCESS (0)
 */
static int STX104_Simple_Menu_AI_Volts( struct menu_command_params * p )
{
	struct demo_tracere_data * ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;
	ds->menu_ai_voltage_value = true;
	return SUCCESS;
}
/**********************************************************************//**
 * @brief
 * Sets the analog input (AI) channel that we wish to observe on the
 * status line.
 * @param p See menu_command_params for details.
 * @return  Always returns SUCCESS (0) since the parameter is not likely to be
 * out of range.
 */
static int STX104_Simple_Menu_AI_Channel_Set( struct menu_command_params * p )
{
	int 						error_code;
	tracere_ai_cfg_type *		ai_cfg;
	struct demo_tracere_data * 	ds;  /* data set */
	ds = (struct demo_tracere_data *) p->info;
	ai_cfg = &(ds->cfg.ai_cfg);

	STX104_AI_CFG_Get( p->board, ai_cfg );
	ai_cfg->first_channel = (STX104_AI_ENUM) p->channel;
	ai_cfg->last_channel  = (STX104_AI_ENUM) p->channel;

	error_code = STX104_AI_CFG_Set( ds->board, ai_cfg );
	return error_code;
}
#endif

#if defined( STX104_SIMPLE_USE_INTERRUPTS )
/**************************************************************************
 *TODO
 */
static int Tracere_Simple_Menu_Interrupt( struct menu_command_params * p )
{
	int 				error_code;
	BOOL	 			value_prev;
	BOOL				value;
	struct demo_tracere_data * ds;  /* data set */

	ds = (struct demo_tracere_data *) p->info;
	value_prev = ds->interrupt_enabled;
	switch( p->channel )
	{
	   case 0: value = true;		break;
	   case 1: value = false;		break;
	}
	if ( value != value_prev )
	{
		error_code = STX104_Int_Enable_Set( p->board, value );
	}
	if ( error_code ) ds->interrupt_enabled = false;
	else              ds->interrupt_enabled = value;

	return error_code;
}
#endif

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
static struct menu_command tracere_simple_menu_list[] =
{
/* (*) if char_first=0, function=NULL, then char_last is prefix spaces.			*/
/*     if char_last < ' ' then it can be used as an index parameter           */
/*   char    char                                                             */
/*   first   last(*)    function                      display name            */
{   0,     0,  	NULL,						"\n"													},  /* display only 				 */
{   0,     1,  	NULL,						"-------------------------------"						},  /* display only 				 */
{   0,     1,  	NULL,						"  < < <  M E N U  > > >"								},  /* display only 				 */
{   0,     1,  	NULL,						"-------------------------------"						},  /* display only 				 */
{	0,	   0,	Demo_Tracere_Menu_Revisions,	NULL												},  /* display helper function 		 */



{   0,     4,  	NULL,						"---- analog inputs -----"								},  /* display only 				 */
#if(0)
{ '0',   '9',  	STX104_Simple_Menu_AI_Channel_Set,		NULL										},  /* Line merges with below  		 */
{ 'a',	 'f',	STX104_Simple_Menu_AI_Channel_Set,		"channel select (0-15)"						},  /* */
{ 'u',	   0,	STX104_Simple_Menu_AI_Hex, 	"print values in hexadecimal" 							},
{ 'v',	   0,	STX104_Simple_Menu_AI_Volts,"print values in volts (uncal)"							},
{ 'w',	   0,	Menu_AI_Gain_Set,	   		"+10.0V   range (PGA = 00)"								},
{ 'x',	   1,	Menu_AI_Gain_Set,	   		" +5.0V   range (PGA = 01)"								},
{ 'y',	   2,	Menu_AI_Gain_Set,	   		" +2.5V   range (PGA = 10)"								},
{ 'z',	   3,	Menu_AI_Gain_Set,	   		" +1.25V  range (PGA = 11)"								},

{   0,     4,  	NULL,						"---- analog outputs -----"								},  /* display only 				 */
{ 'o',	   0,	Menu_AO_Value_Set, 			"write to channel-a"									},
{ 'p',	   1,	Menu_AO_Value_Set, 			"write to channel-b"									},
#endif

{   0,     4,  	NULL,						"---- digital i/o -----"								},  /* display only 				 */
{ 's',	   0,	Menu_DO_Bit_Set,			"set dio bit"					 						},
{ 'c',	   0,	Menu_DO_Bit_Clear,			"clear dio bit"					 						},
{ 'g',	   0,	Menu_DI_Bit,				"read dio bit"					 						},
{ 'a',     0,   Menu_DI_Bit_All,			"read dio all bits"										},

{ 'g',	   0,	Menu_DO_All_Set,			"write to all"											},
{ 'h',	 'k',	Menu_DO_Value_Set,			"write to channel (DO0 to DO3)" 						},

{   0,     4,  	NULL,						"---- other -----"										},  /* display only 				 */
#if defined( TRACERE_SIMPLE_USE_INTERRUPTS )
{ 'm',	 'n', 	Tracere_Simple_Menu_Interrupt, 	"Interrupt enable (m) disable (n)" 					},
#endif
{ 'r',	   0, 	Menu_Redraw,				"redraw menu"					 						},
{ 'F',	   0, 	Menu_Report_File,			"Status and configuration report to 'report.txt'"		},
{ 'R',	   0, 	Menu_Report_Console,		"Status and configuration report to console"	 		},
{ 'q',	   0, 	Menu_Quit,					"quit this demo"				 						},
{   0,     4,  	NULL,						"PRESS KEY TO SELECT MENU ITEM>"						},
{   0,     1,  	NULL,						"DI = Digital in.   AI = Analog in"  					},
{   0,     1,  	NULL,						"Polarity: BIP/UNI. Input: SE/DIF. FR = frames read."  	},
{   0,     0,  	NULL,						"\n"													},  /* display only 				 */

{ 0, 0, NULL, NULL }   /* stopper */

};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

/***************************************************************************/
/*
	           < < < <   	MENU DATA PRESENTATION    > > > >
*/
#if(0)
/**********************************************************************//**
 * @brief
 * Display the analog input (AI) channel that is currently selected.
 * @param mdp See menu_data_params for details.
 * @return Always returns SUCCESS (0)
 */
static int STX104_Simple_Data_AI_Channel( struct menu_data_params * mdp )
{
	struct demo_tracere_data * 			ds;
	struct tracere_ai_cfg *				ai_cfg;
#define STX104_SIMPLE_TEMP_STR_SIZE  4
	char 								tmp_str[STX104_SIMPLE_TEMP_STR_SIZE];

	ds = (struct demo_tracere_data *) mdp->info;
	ai_cfg = &(ds->cfg.ai_cfg);
#ifdef __BORLANDC__
	sprintf( tmp_str, "%02d", ai_cfg->first_channel );
#else
	snprintf( tmp_str, STX104_SIMPLE_TEMP_STR_SIZE, "%02d", ai_cfg->first_channel );
#endif
	Menu_Data_Append( mdp, tmp_str );
	return SUCCESS;
#undef STX104_SIMPLE_TEMP_STR_SIZE
}
/**********************************************************************//**
 * @brief
 *
 * @param mdp See menu_data_params for details.
 * @return Always returns SUCCESS (0)
 */
static int STX104_Simple_Data_AI_Value( struct menu_data_params * mdp )
{
	struct demo_tracere_data * 			ds;
#define STX104_SIMPLE_TEMP_STR_SIZE  16
	char 								tmp_str[STX104_SIMPLE_TEMP_STR_SIZE];

	ds = (struct demo_tracere_data *) mdp->info;

	/****  Update analog input data - if interrupts, then automatic ****/
#if defined( STX104_SIMPLE_USE_INTERRUPTS )
	while ( ds->interrupt_blocking ) {}; //wait for interrupt to complete.
#else
	STX104_Simple_Menu_AI_Data_Readout( ds );
#endif
	//if ( data_set->frames_have_been_read )  //other implementations may require this test!
	//{
	if ( ds->menu_ai_voltage_value )
	{
		STX104_AI_Raw_To_Volts( ds->board, ds->value_raw, &(ds->voltage_uncalibrated) );
#ifdef __BORLANDC__
	sprintf( tmp_str, "% 2.4f  ", data_set->voltage_uncalibrated );
#else
	snprintf( tmp_str, STX104_SIMPLE_TEMP_STR_SIZE, "% 2.4f  ", ds->voltage_uncalibrated );
#endif
	}
	else
	{
#ifdef __BORLANDC__
	sprintf( tmp_str, "0x%04X  ", data_set->value_raw );
#else
	snprintf( tmp_str, STX104_SIMPLE_TEMP_STR_SIZE, "0x%04X  ", ds->value_raw );
#endif
	}
	//}
	Menu_Data_Append( mdp, tmp_str );
	return SUCCESS;
#undef STX104_SIMPLE_TEMP_STR_SIZE
}
#endif


#if defined( STX104_SIMPLE_USE_INTERRUPTS )
/**************************************************************************
 TODO
 */
static int STX104_Simple_Data_Interrupt_Status( struct menu_data_params * mdp )
{
	struct demo_tracere_data * 			ds;
	ds = (struct demo_tracere_data *) mdp->info;

	/**** initialize ****/
	if ( ds->interrupt_enabled )  mdp->out_str[mdp->column] = 'I';
	else                          mdp->out_str[mdp->column] = '-';

	mdp->column++;
	return SUCCESS;
}
#endif

/**********************************************************************//**
 *TODO
 */
static struct menu_data tracere_simple_data_list[] =
{
	/*	column	max_width	display function				 Prefix length		Prefix  		*/
	{	0,			0,		Demo_STX104_Data_Timer_Tick,	  0,				NULL	  		},
#if defined( STX104_SIMPLE_USE_INTERRUPTS )
	{	2,			0,		STX104_Simple_Data_Interrupt_Status,	0,			NULL	  		},
#endif
	{	5,			0,		Demo_STX104_Data_DI,			  3,				"DI:"	  		},
	{  13,			0,		Demo_STX104_Data_Config_AI,		  3,				"AI:"	  		},
	{  31,			0,		STX104_Simple_Data_AI_Channel,	  2,				"CH"	  		},

	{  35,			0,		STX104_Simple_Data_AI_Value,	  2,				": "	  		},
	{  48,			0,		STX104_Data_Frames,		  		  3,				"FR:"	  		},
	{	0,			0,		NULL,							  0,				NULL	  		}
};
/***************** END -- MENU DISPLAY AND COMMAND PROCESSING ***************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************  BEGIN -- MAIN PORTION OF CODE (TOP LEVEL)  ***************/

/**********************************************************************//**
 * @brief
 *
 * @param private_data
 * @return Always returns SUCCESS (0)
 */
int Tracere_Simple_Main_Stop_Request( void * private_data )
{
	struct demo_tracere_data * 	ds;
	ds = (struct demo_tracere_data *) private_data;
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
int Tracere_Simple_Main_Loop( void * private_data )
{
	int 						error_code;
	int    						board;
	FILE * 						out_file;
	FILE * 						in_file;
	struct demo_tracere_data * 	ds;

	ds = (struct demo_tracere_data *) private_data;
	out_file= ds->out_file;
	in_file = ds->in_file;
	board 	= ds->board;

 #if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
		ds->dout3_state = !ds->dout3_state;
		STX104_DO_Set( board, STX104_DO3, ds->dout3_state );
 #endif

	error_code = Menu_Command( tracere_simple_menu_list, board, in_file, out_file, ds );
	/* display status and updated information */
	Menu_Data( tracere_simple_data_list, 0, board, out_file, ds );

#if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	ds->dout3_state = !ds->dout3_state;
	STX104_DO_Set( board, STX104_DO3, ds->dout3_state );
#endif

	return error_code;
}
#else
int Tracere_Simple_Main_Loop( void * private_data )
{
	int 						error_code;
	int    						board;
	FILE * 						out_file;
	FILE * 						in_file;
	struct demo_tracere_data * 	ds;

	ds = (struct demo_tracere_data *) private_data;
	out_file= ds->out_file;
	in_file = ds->in_file;
	board 	= ds->board;
 #if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
		ds->dout3_state = !ds->dout3_state;
		STX104_DO_Set( board, STX104_DO3, ds->dout3_state );
 #endif
	error_code = Menu_Command( tracere_simple_menu_list, board, in_file, out_file, ds );
	/* display status and updated information */
	Menu_Data( tracere_simple_data_list, 0, board, out_file, ds );
 #if defined( DOUT3_TOGGLE_EACH_PERIODIC_WRITE_CYCLE )
	ds->dout3_state = !ds->dout3_state;
	STX104_DO_Set( board, STX104_DO3, ds->dout3_state );
 #endif
 #ifdef __linux__
	usleep( ds->main_loop_time_usec );
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
int main( int argc, char* argv[] )
{
	int 						error_code;
	int    						board;
	FILE * 						out_file;
	/*FILE * 						in_file;*/
	struct demo_tracere_data * 	ds;
	char   						prefix[] = "  ";

	ds = &demo_tracere_data;

	/**** initialize global variables shared ****/
	memset( ds, 0, sizeof( struct demo_tracere_data ) );
	ds->in_file  = stdin;
	ds->out_file = stdout;
	/* local file settings -- short hand */
	out_file = ds->out_file;
	/*in_file  = ds->in_file;*/
	fprintf( out_file, "Main: begin:\n" );
	/***** initialize driver and board software interface *****/
	error_code = Demo_Tracere_Initialize( prefix,   /* output string prefix string  */
										  argc,     /* command line arguments count */
										  argv,     /* command line arguments list  */
										  &demo_tracere_data       /* data */
								        );
	if ( error_code ) goto Main_Error;
	fprintf( out_file, "Main: Demo_Tracere_Initialize() complete\n" );
	board = ds->board;  /* short hand */

	/***** menu timing calculations *****/
	error_code = Demo_Tracere_Main_Loop_Timing_Calculations( MENU_UPDATE_TIME_MS, ds );
	if ( error_code ) goto Main_Error;

	/***** tracere board level configuration *****/
	error_code = Tracere_Simple_Configuration( prefix, &demo_tracere_data );
	if ( error_code ) goto Main_Error;
	fprintf( out_file, "Main: Tracere_Simple_Configuration() complete\n" );

	/***** menu initialization *****/
	Menu_Initialization( tracere_simple_menu_list );

	/***** Main loop here *****/
	fprintf( out_file, "Main: process loop begin\n\n" );
	Menu_Display( tracere_simple_menu_list, board, out_file, ds );

#if MENU_MAIN_LOOP_USE_TIMER_RATHER_THAN_SLEEP_DELAYS_IMPROVED_TIMING == 1
	error_code = Menu_Periodic_Run( ds->main_loop_time_sec,
									STX104_Simple_Main_Loop,
									STX104_Simple_Main_Stop_Request,
									ds
								  );
	while(1)
	{
		if ( true == ds->quit_request ) break;
		/* keeps cpu available */
		usleep(1);
	}
#else
	do
	{  /* process any incoming menu commands */
		error_code = Tracere_Simple_Main_Loop( ds );
	} while ( !error_code );
#endif

	fprintf( out_file, "\n\nMain: process loop end\n" );
Main_Error:
	if ( error_code < 0 ) error_code = -error_code;
	if ( error_code )
	{
	printf( "Application Test: exit fail, error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );
	}
	Demo_Tracere_Terminate( prefix,    		/* output string prefix string  */
					        ds               /* data */
				          );
	return -error_code;
}
/* file end */
