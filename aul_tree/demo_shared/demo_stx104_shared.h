/************************************************************************//**
 * @file demo_stx104_shared.h
 */

#ifndef DEMO_STX104_SHARED_STATUS
#define DEMO_STX104_SHARED_STATUS   26
#define DEMO_STX104_SHARED_NAME     "demo_stx104"
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
#include "../demo_shared/config.h"

/*******************************************************************/
#define DEMO_STX104_SHARED_H_REV  "$Date: 2014-05-21 12:00:03 -0500 (Wed, 21 May 2014) $"

/***************************************************************************/
#define DEMO_STX104_DEBUG  _DEBUG

/***************************************************************************/
#define DEMO_STX104_FIFO_LOG_FILE_NAME  "fifo"
#define DEMO_STX104_FIFO_LOG_BUFFER_SIZE  256
/***************************************************************************/
#define DEMO_STX104_DATA_FILE_NAME "aiout"

#if(0) /* moved to app_status.h */
/***************************************************************************/
/* X-macro used to define error information in a nice table format
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define DEMO_STX104_SHARED_STATUS_EC_DEFINITIONS(_) \
	_( DEMO_STX104_SHARED_STATUS,	EC_DEMO_STX104_FILE_OPEN_ERROR,	         1,  "file open error" 	       )

/***************************************************************************/
enum demo_status
{
	DEMO_STX104_SHARED_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )//!< ERROR_CODE_EXTRACTOR
	EC_DEMO_STX104_END_OF_LIST
};
#endif

/************************************************************************//**
 * Complete STX104 data structure.
 */
struct demo_stx104_data
{
	int			board;        /**< Library descriptor. Must remain the first item in this data structure. 	*/
	FILE *   	in_file;      /**< Input file.  Must remain the second item in this data structure. 		*/
	FILE *   	out_file;     /**< Output file. Must remain the third item in this data structure. 			*/

	stx104_configuration_type cfg;	/**< stx104 configuration data structure */

	/* menu and main loop timing information */
	double			main_loop_time_sec;
	unsigned int	main_loop_time_usec;
	struct timespec main_loop_time_spec;

	/***** Revision information 	*****/
	unsigned long	revision_driver;  /**< driver revision information 	*/
	unsigned long   revision_library; /**< library revision information */

	/* firmware revision reported by the hardware */
	uint16_t		revision_firmware; /**< firmware revision reported by the stx104 hardware */

	/* used to indicate that a fifo_full_error has occurred */
	BOOL  			fifo_full_error;  /**< STX104 FIFO full error has occurred */


	/* Number of frame bytes is computed so the event handler does not have
	   to recompute all the time.
	*/
	uint16_t 		frame_size_in_bytes;					/**< frame size computed in number of bytes 	*/
	uint16_t 		frame_quantity_request_per_write_cycle; /**< frame quantity request per software cycle 	*/
	uint32_t 		software_frame_count_max;				/**< frame count maximum, software limit		*/
	/*
	If not-zero then stop sampling upon maximum frame count (auto-trigger stop)
	*/
	uint32_t 		software_frame_count;					/**< soft frame count, an accumulation thereof	*/

	BOOL			stop_request;
	BOOL			quit_request;

	STX104_AI_ENCODING_ENUM 	ai_cfg_encoding;			/**< STX104 Analog Input encoding schemes		*/

	BOOL 						dout3_state;				/**< DOUT3 current state. DOUT3 is used for testing.
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 and monitored with oscilloscope.
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	*/

	FILE * 			fifo_log_file;
	int 			fifo_log_buffer_index;
	uint32_t 		fifo_log_buffer[DEMO_STX104_FIFO_LOG_BUFFER_SIZE];

	FILE *			data_output_file;

	struct timespec	time_start;
	struct timespec	time_stop;

	/***** stx104_simple - data parameters  ****/
	/* if true then display Analog inputs as (uncalibrated) voltage */
	BOOL		menu_ai_voltage_value;
	BOOL		frames_have_been_read;  /* STX104_Simple_Menu_AI_Data_Readout */
	uint16_t  	value_raw;
	double  	voltage_uncalibrated;

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
int Demo_STX104_Menu_Frame_Max_Set( struct menu_command_params * p );
int Demo_STX104_Menu_Trigger_Start( struct menu_command_params * p );
int Demo_STX104_Menu_Trigger_Stop( struct menu_command_params * p );
int Demo_STX104_Menu_Revisions( struct menu_command_params * p );

/***************************************************************************/
/* DATA PRESENTATION */
char Demo_STX104_Data_Timer_Tick_State( void );
int Demo_STX104_Data_Timer_Tick( struct menu_data_params * mdp );
int Demo_STX104_Data_DI( struct menu_data_params * mdp );
int Demo_STX104_Data_Config_AI( struct menu_data_params * mdp );
int Demo_STX104_Data_Trig( struct menu_data_params * mdp );

int STX104_Data_Frames( struct menu_data_params * mdp );

/***************************************************************************/

int Demo_STX104_Main_Loop_Timing_Calculations( unsigned int time_msec, struct demo_stx104_data * ds );

void Demo_STX104_Files_Close( struct demo_stx104_data * ds );

int Demo_STX104_Files_Open( const char * 				data_file_name,  	/* analog input data file */
							const char * 				fifo_log_file_name, /* fifo log file          */
							struct demo_stx104_data * 	ds
						  );

void Demo_STX104_Terminate( char * prefix, struct demo_stx104_data * ds );

int Demo_STX104_Initialize( char * prefix,    	/* output string prefix string  */
							int argc,       	/* command line arguments count */
							char* argv[],     	/* command line arguments list  */
							struct demo_stx104_data * info  /* shared data */
						  );

/***************************************************************************/
#ifdef __cplusplus
}
#endif
/***************************************************************************/

#endif  /* DEMO_STX104_SHARED_STATUS */


