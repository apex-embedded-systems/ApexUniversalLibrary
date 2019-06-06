#ifndef DEMO_MENU
#define DEMO_MENU  25
#define DEMO_MENU_NAME "demo_menu"
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

#include "../shared/status_macros.h"

/* TODO:  need to replace all functions with safe functions */
/*#include <string.h>*/
/*#include <stdio.h>*/

/***************************************************************************/

#define NULL_CHARACTER 0x00
#define MENU_DISPLAY_LINE_LENGTH 80

/***************************************************************************/
/* X-macro used to define error information in a nice table format
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define DEMO_MENU_EC_DEFINITIONS(_) \
	_( DEMO_MENU,	EC_DEMO_MENU_OK,	         		0,  ""     )\
	_( DEMO_MENU,	EC_DEMO_MENU_COMMAND_NOT_FOUND,		1,  ""     )\
	_( DEMO_MENU,	EC_DEMO_MENU_QUIT,	    			2,  ""     )\
	_( DEMO_MENU,	EC_DEMO_MENU_TIMER_SIGACTION,		3,  ""     )\
	_( DEMO_MENU,	EC_DEMO_MENU_TIMER_CREATE,			4,  ""     )\
	_( DEMO_MENU,	EC_DEMO_MENU_TIMER_SETTIME,			5,  ""     )\
	_( DEMO_MENU,	EC_DEMO_MENU_TIMER_ACTIVE,			6,  ""     )\
	_( DEMO_MENU,	EC_DEMO_MENU_TIMER_INFO_INVALID,	7,  ""     )



/***************************************************************************/
enum menu_status
{
	DEMO_MENU_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
	EC_DEMO_MENU_END_OF_LIST
};
#if(0)
/***************************************************************************/
enum
{
	MENU_OPTION_OK				  =  0,
	MENU_OPTION_COMMAND_NOT_FOUND = -1,
	MENU_OPTION_QUIT       		  = -2
};
#endif

/***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************/

/******************************************************************//**
 * @brief
 * Data type used to pass information to/from menu command functions.
 * It is provided more for convenience and reduction in parameters passed
 * to/from a menu related function.
 *
 * This data structure passes all of the common parameters that any of the
 * menu commands (i.e. functions defined in a menu list).
 */
struct menu_command_params
{
	int	 	board;
	size_t 	channel;
	FILE * 	in_file;
	FILE * 	out_file;
	void * 	info;  		/* data specific to application */
};

/**** data type of the menu command functions ****/
typedef int ( * MENU_COMMAND_FNC_TYPE )( 	struct menu_command_params * p );

/******************************************************************//**
 * @brief
 * Data structure used to create the menu structure or list that is presented to the
 * user.  For each of the menu items there maybe be key-stroke character(s) and an
 * associated function call made to perform the specific operation.
 *
 * There are various patterns "char_first" and "char_second" that create various
 * menu key strokes and how the lines of information are displayed.
 */
struct menu_command
{
	char					char_first; /**< First valid character accepted by this command.
	                                         if zero, then this is a printable only line	*/
	char					char_last;  /**< Last valid character accepted by this command.
											 If zero, then only single character command.   */
	MENU_COMMAND_FNC_TYPE	function;	/**< Function associated with this menu item.
											 If NULL, then no function is called. 			*/
	char * 					display;	/**< String information to post to the output.  	*/
};


/******************************************************************//**
 * @brief
 *
 *
 */
/**** data type used to pass information to/from data display functions ****/
struct menu_data_params
{
	int  	column;   /* column passed to helper function */
	int 	width_max;
	int    	out_str_size;
	char * 	out_str;
	void * 	info;  /* data specific to application -- private data */
};

/**** data type of the menu data functions ****/
typedef int ( * MENU_DATA_FNC_TYPE )( struct menu_data_params * mdp );


/******************************************************************//**
 * @brief
 * Data structure used to create the single output line of data.  Each
 * item will write out as:  "<prefix><data>".  The prefix begins at column_start.
 */
struct menu_data
{
	int						column_start; /**< Column start position of data to be output 			*/
	int						width_max;	  /**< Maximum number of characters, if zero ignored. 		*/
	MENU_DATA_FNC_TYPE		function;	  /**< Function that will write output data.		 		*/
	int						prefix_length;/**< Character length of the prefix.						*/
	char * 					prefix;		  /**< Prefix data written starting at column_start.		*/
};

/****************************************************************************/
/* Used to define the main loop operations */
typedef int ( * menu_loop_function_type )( void * private_data );
typedef int ( * menu_stop_request_function_type )( void * private_data );
/****************************************************************************/


/****************************************************************************/

uint16_t Menu_Ask_For_Value( FILE * in_file, FILE * out_file, const char * question );

uint16_t Menu_Ask_Uint16_Range( FILE * 			in_file,
								FILE * 			out_file,
								const char * 	question,
								uint16_t		min,
								uint16_t		max
							  );

void Menu_Display_Clear_Line( FILE * out_file );

/***************************************************************************/

int Menu_AI_First_Channel_Set( struct menu_command_params * p );

int Menu_AI_Last_Channel_Set( struct menu_command_params * p );

int Menu_AI_Channel_Set( struct menu_command_params * p );

int Menu_AI_Gain_Set( struct menu_command_params * p );

int Menu_AO_Value_Set( struct menu_command_params * p );

int Menu_DO_All_Set( struct menu_command_params * p );

int Menu_DO_Value_Set( struct menu_command_params * p );

int Menu_DO_Bit_Clear( struct menu_command_params * p );

int Menu_DO_Bit_Set( struct menu_command_params * p );

int Menu_DI_Bit( struct menu_command_params * p );

int Menu_DI_Bit_All( struct menu_command_params * p );



int Menu_DIO_Port_Write( struct menu_command_params * p );

int Menu_DIO_Port_Read( struct menu_command_params * p );



int Menu_Quit( struct menu_command_params * p );

int Menu_Report_File( struct menu_command_params * p );

int Menu_Report_Console( struct menu_command_params * p );

int Menu_Redraw( struct menu_command_params * p );


/***************************************************************************/

BOOL Menu_Get_Character( int * menu_character );

uint32_t Menu_Ask_Uint32( FILE * in_file, FILE * out_file, const char * question );

uint16_t Menu_Ask_Uint16( FILE * in_file, FILE * out_file, const char * question );

int	Menu_Periodic_Run( 	double							time_sec,
						menu_loop_function_type 		loop_function,
						menu_stop_request_function_type stop_request_function,
						void * 							private_data
					 );



int Menu_Command( struct menu_command menu_list[],
                  int 				board,
                  FILE * 			in_file,
                  FILE * 			out_file,
                  void *            info      /* private application data */
                );

void Menu_Display( struct menu_command 	menu_list[],
				   int 					board,
                   FILE * 				out_file,
                   void *            	info      /* private application data */
                 );

void Menu_Data_Append( struct menu_data_params * mdp, char * msg_str );

void Menu_Data(	struct menu_data * 	data_list,
				int 				column_begin,
                int 				board,
                FILE * 				out_file,
                void *				info
              );

void Menu_Termination( void );

void Menu_Initialization( struct menu_command menu_list[] );

/***************************************************************************/
#ifdef __cplusplus
}
#endif
/***************************************************************************/

#endif  /* DEMO_MENU */


