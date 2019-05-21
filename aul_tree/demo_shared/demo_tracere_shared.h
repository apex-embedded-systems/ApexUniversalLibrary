/************************************************************************//**
 * @file demo_tracere_shared.h
 */

#ifndef DEMO_TRACERE_SHARED_STATUS
#define DEMO_TRACERE_SHARED_STATUS   27
#define DEMO_TRACERE_SHARED_NAME     "demo_tracere"
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

/***************************************************************************/
#include <time.h>  /* time related parameters and functions */
#include <inttypes.h>

#include "../aul/aul_public.h"
#include "../demo_shared/demo_tracere_config.h"

/*******************************************************************/
#define DEMO_TRACERE_SHARED_H_REV  "$Date: 2014-05-21 12:00:03 -0500 (Wed, 21 May 2014) $"

/***************************************************************************/
#define DEMO_TRACERE_DEBUG  _DEBUG

/***************************************************************************/
#define DEMO_TRACERE_FIFO_LOG_FILE_NAME  "tefifo"
#define DEMO_TRACERE_FIFO_LOG_BUFFER_SIZE  256
/***************************************************************************/
#define DEMO_TRACERE_DATA_FILE_NAME "teout"

#if(0) /* moved to app_status.h */
/***************************************************************************/
/* X-macro used to define error information in a nice table format
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define DEMO_TRACERE_SHARED_STATUS_EC_DEFINITIONS(_) \
	_( DEMO_TRACERE_SHARED_STATUS,	EC_DEMO_TRACERE_FILE_OPEN_ERROR,	         1,  "file open error" 	       )

/***************************************************************************/
enum demo_status
{
	DEMO_TRACERE_SHARED_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )//!< ERROR_CODE_EXTRACTOR
	EC_DEMO_TRACERE_END_OF_LIST
};
#endif

/************************************************************************//**
 * Complete Tracer-E data structure.
 */
struct demo_tracere_data
{
	int			board;        /**< Library descriptor. Must remain the first item in this data structure. 	*/
	FILE *   	in_file;      /**< Input file.  Must remain the second item in this data structure. 		*/
	FILE *   	out_file;     /**< Output file. Must remain the third item in this data structure. 			*/

	tracere_cfg_type cfg;	/**< tracere configuration data structure */

	/* menu and main loop timing information */
	double			main_loop_time_sec;
	unsigned int	main_loop_time_usec;
	struct timespec main_loop_time_spec;

	/***** Revision information 	*****/
	unsigned long	revision_driver;  /**< driver revision information 	*/
	unsigned long   revision_library; /**< library revision information */


	/* firmware revision reported by the hardware */
	uint16_t		revision_firmware; /**< firmware revision reported by the stx104 hardware */

	FILE * 			fifo_log_file;
	int 			fifo_log_buffer_index;
	uint32_t 		fifo_log_buffer[DEMO_TRACERE_FIFO_LOG_BUFFER_SIZE];

	FILE *			data_output_file;

	struct timespec	time_start;
	struct timespec	time_stop;

	/*****  	*****/
	BOOL			stop_request;
	BOOL			quit_request;

	/***** tracere_simple - data parameters  ****/
	BOOL		dio_port[TRACERE_DIO_PORT_QTY];

	//BOOL		menu_ai_voltage_value;
	//BOOL		frames_have_been_read;  /* STX104_Simple_Menu_AI_Data_Readout */
	//uint16_t  	value_raw;
	//double  	voltage_uncalibrated;

	BOOL 		interrupt_enabled;
	BOOL		interrupt_blocking;
};

/***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************/


extern const char demo_data_file_name[];
extern const char demo_data_file_name_csv[];
extern const char demo_fifo_log_file_name[];
extern const char demo_fifo_log_file_name_csv[];

/***************************************************************************/
/* MENU COMMAND AND DISPLAY  -- TODO: move to menu ???*/
#if(0)
int Demo_Tracere_Menu_Frame_Max_Set( struct menu_command_params * p );
int Demo_Tracere_Menu_Trigger_Start( struct menu_command_params * p );
int Demo_Tracere_Menu_Trigger_Stop( struct menu_command_params * p );
#endif
int Demo_Tracere_Menu_Revisions( struct menu_command_params * p );

/***************************************************************************/
/* DATA PRESENTATION */
char Demo_Tracere_Data_Timer_Tick_State( void );
int Demo_Tracere_Data_Timer_Tick( struct menu_data_params * mdp );
#if(0)
int Demo_Tracere_Data_DI( struct menu_data_params * mdp );
int Demo_Tracere_Data_Config_AI( struct menu_data_params * mdp );
int Demo_Tracere_Data_Trig( struct menu_data_params * mdp );
#endif

/***************************************************************************/

int Demo_Tracere_Main_Loop_Timing_Calculations( unsigned int time_msec, struct demo_tracere_data * ds );

void Demo_Tracere_Files_Close( struct demo_tracere_data * ds );

int Demo_Tracere_Files_Open( const char * 				data_file_name,  	/* analog input data file */
							const char * 				fifo_log_file_name, /* fifo log file          */
							struct demo_tracere_data * 	ds
						  );

void Demo_Tracere_Terminate( char * prefix, struct demo_tracere_data * ds );

int Demo_Tracere_Initialize( char * prefix,    	/* output string prefix string  */
							 int argc,       	/* command line arguments count */
							 char* argv[],     	/* command line arguments list  */
							 struct demo_tracere_data * info  /* shared data */
						   );

/***************************************************************************/
#ifdef __cplusplus
}
#endif
/***************************************************************************/

#endif  /* DEMO_TRACERE_SHARED_STATUS */


