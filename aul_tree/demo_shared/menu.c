/*****************************************************************************//**
 * @file  stx104_simple.c
 *
 *
 *
 *
 * _____________________________________________________________________________
 * POSIX.1b RealTime Extensions Library
 *
 * The librt.s library to support High Resolution POSIX Timers (HRT)
 * is added to the linker.  The following functions are a part of this
 * library:  timer_create(), timer_settime(), timer_delete().
 *
 * Location of library:  ./usr/lib/i386-linux-gnu/librt.a
 *
 * The library is used by the menu.c file.  Please search for "librt.a".
 *
 * Ubuntu:  apt-get install build-essential
 * http://askubuntu.com/questions/398489/how-to-install-build-essential
 *
 * yum install glibc-static
 *
 *
 */

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
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* replace with apex string handler */
#include "../aul/aul_public.h"
#include "../str_safe/str_safe.h"
#include "cfgrprt.h"
#include "menu.h"


/*************************************************************************/
#define LOCAL_STRING_BUFFER_SIZE  128

#ifndef NULL_CHARACTER
# define NULL_CHARACTER '\0'
#endif



/***************************************************************************/

static struct menu_command * menu_menu_list;

/***************************************************************************/
static const char * bool_str[] = { "false", "true " 	};  /* value ? bl[1] : bl[0].  if value then print bl[1] else print bl[0] */

/***************************************************************************/
#define MENU_MAX_INPUT_SIZE  14
/***************************************************************************/
#if ( LOCAL_STRING_BUFFER_SIZE < MENU_MAX_INPUT_SIZE )
# define MENU_MAX_INPUT_SIZE  LOCAL_STRING_BUFFER_SIZE
#endif
/******************************************************************//**
 * @brief Ask for an unsigned 16-bit value
 * @param in_file	Request input from this location (keyboard)
 * @param out_file  Produce resulting output to this location (display)
 * @param question  Question regarding the number request
 * @return  The unsigned 16-bit value
 */
uint16_t Menu_Ask_Uint16( FILE * in_file, FILE * out_file, const char * question )
{
	char in_str[LOCAL_STRING_BUFFER_SIZE];
	char *endptr;

	fprintf( out_file, "%s", question );
	fgets( in_str, MENU_MAX_INPUT_SIZE, in_file );

	return( (uint16_t) strtol(in_str, &endptr, 0) );
}
/******************************************************************//**
 * @brief Ask for an unsigned 16-bit value within a given range
 * @param in_file	Request input from this location (keyboard)
 * @param out_file  Produce resulting output to this location (display)
 * @param question  Question regarding the number request
 * @param min		minimum input range
 * @param max		maximum input range
 * @return  The unsigned 16-bit value.  Returns min if just return entered.
 */
uint16_t Menu_Ask_Uint16_Range( FILE * 			in_file,
								FILE * 			out_file,
								const char * 	question,
								uint16_t		min,
								uint16_t		max
							  )
{
	int  redo;
	char in_str[LOCAL_STRING_BUFFER_SIZE];
	char *endptr;
	uint16_t value;

	redo = 0;
	do
	{
		if ( redo ) fprintf( out_file, "\n" );
		fprintf( out_file, "%s", question );
		fgets( in_str, MENU_MAX_INPUT_SIZE, in_file );
		if ( 1 >= strlen( in_str ) )
		{ /* only carriage return, so just make it the minimum */
			value = min;
			redo  = 0;
		}
		else
		{
			value = (uint16_t) strtol( in_str, &endptr, 0 );
			redo  = 0;
		}

		if ( ( value < min ) && ( value > max ) ) redo = 1;
	} while ( 1 == redo );

	return value;
}
/******************************************************************//**
 * @brief Ask for an unsigned 32-bit value
 * @param in_file	Request input from this location (keyboard)
 * @param out_file  Produce resulting output to this location (display)
 * @param question  Question regarding the number request
 * @return  The unsigned 32-bit value
 */
uint32_t Menu_Ask_Uint32( FILE * in_file, FILE * out_file, const char * question )
{
	char in_str[LOCAL_STRING_BUFFER_SIZE];
	char *endptr;

	fprintf( out_file, "%s", question );
	fgets( in_str, MENU_MAX_INPUT_SIZE, in_file );

	return( (uint32_t) strtol(in_str, &endptr, 0) );
}
/******************************************************************//**
 * @brief Clear the line on the output up to the output line length.
 * @param out_file  The output typically the display.
 */
void Menu_Display_Clear_Line( FILE* out_file )
{
	char out_msg[MENU_DISPLAY_LINE_LENGTH];
	int cp;

	for( cp=0; cp<MENU_DISPLAY_LINE_LENGTH; cp++ ) out_msg[cp] = ' ';
	out_msg[MENU_DISPLAY_LINE_LENGTH-1] = NULL_CHARACTER;

	fprintf( out_file, "%s\r", out_msg );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


/******************************************************************//**
 * @brief
 * Set the analog input (AI) configuration first channel by data
 * passed from menu.  It assumes that the device has been opened
 * successfully.
 *
 * @param p  menu parameters passed in/out of this function
 * @return   Always returns SUCCESS
 */
int Menu_AI_First_Channel_Set( struct menu_command_params * p )
{
	stx104_ai_cfg_type ai_cfg;
	STX104_AI_CFG_Get( p->board, &ai_cfg );
	ai_cfg.first_channel = (STX104_AI_ENUM) p->channel;
	STX104_AI_CFG_Set( p->board, &ai_cfg );
	return SUCCESS;
}
/******************************************************************//**
 * @brief
 * Set the analog input (AI) configuration last channel by data
 * passed from menu.  It assumes that the device has been opened
 * successfully.
 *
 * @param p  menu parameters passed in/out of this function
 * @return   Always returns SUCCESS
 */
int Menu_AI_Last_Channel_Set( struct menu_command_params * p )
{
	stx104_ai_cfg_type ai_cfg;
	STX104_AI_CFG_Get( p->board, &ai_cfg );
	ai_cfg.last_channel  = (STX104_AI_ENUM) p->channel;
	STX104_AI_CFG_Set( p->board, &ai_cfg );
	return SUCCESS;
}
/******************************************************************//**
 * @brief
 * Set the analog input (AI) configuration first and last channel
 * by data passed from menu.  It assumes that the device has been
 * opened successfully.
 *
 * @param p  menu parameters passed in/out of this function
 * @return   Always returns SUCCESS
 */
int Menu_AI_Channel_Set( struct menu_command_params * p )
{
	stx104_ai_cfg_type ai_cfg;
	STX104_AI_CFG_Get( p->board, &ai_cfg );
	ai_cfg.first_channel = (STX104_AI_ENUM) p->channel;
	ai_cfg.last_channel  = (STX104_AI_ENUM) p->channel;
	STX104_AI_CFG_Set( p->board, &ai_cfg );
	return SUCCESS;
}
/**************************************************************************
Analog Input (AI) gain setting across channels.
*/
/******************************************************************//**
 * @brief
 * Set the analog input (AI) configuration gain.  It assumes that
 * the device has been opened successfully.
 *
 * @param p  menu parameters passed in/out of this function
 * @return   Always returns SUCCESS
 */
int Menu_AI_Gain_Set( struct menu_command_params * p )
{
	stx104_ai_cfg_type ai_cfg;
	STX104_AI_CFG_Get( p->board, &ai_cfg );
	ai_cfg.gain = (STX104_AI_GAIN_ENUM) p->channel;
	STX104_AI_CFG_Set( p->board, &ai_cfg );
 	return SUCCESS;
}
/***************************************************************************/
int Menu_AO_Value_Set( struct menu_command_params * p )
{
	uint16_t value;
	char   question_str[LOCAL_STRING_BUFFER_SIZE];
	Menu_Display_Clear_Line( p->out_file );
#ifdef __BORLANDC__
	sprintf( question_str, "AO%d> ", p->channel );
#else
	snprintf( question_str, LOCAL_STRING_BUFFER_SIZE, "AO%d> ", p->channel );
#endif
	value = Menu_Ask_Uint16( p->in_file, p->out_file, question_str );
	STX104_AO_Set( p->board, (STX104_AO_ENUM) p->channel, value );
	return SUCCESS;
}

/***************************************************************************/
int Menu_DO_All_Set( struct menu_command_params * p )
{
	uint16_t value;
	char   question_str[LOCAL_STRING_BUFFER_SIZE];
	Menu_Display_Clear_Line( p->out_file );
#ifdef __BORLANDC__
	sprintf( question_str, "DOUT ALL (nibble value)> " );
#else
	snprintf( question_str, LOCAL_STRING_BUFFER_SIZE, "DOUT ALL (nibble value)> " );
#endif
	value = Menu_Ask_Uint16( p->in_file, p->out_file, question_str );
	STX104_DO_All_Set( p->board, (uint8_t) ( value & 0x0F ) );
	return SUCCESS;
}
/***************************************************************************/
int Menu_DO_Value_Set( struct menu_command_params * p )
{
	uint16_t value;
	char   question_str[LOCAL_STRING_BUFFER_SIZE];
	Menu_Display_Clear_Line( p->out_file );
#ifdef __BORLANDC__
	sprintf( question_str, "DOUT%d = ", p->channel );
#else
	snprintf( question_str, LOCAL_STRING_BUFFER_SIZE, "DOUT%d = ", p->channel );
#endif
	value = Menu_Ask_Uint16( p->in_file, p->out_file, question_str );
	if ( value ) STX104_DO_Set( p->board, (STX104_DO_ENUM) p->channel, true  );
	else         STX104_DO_Set( p->board, (STX104_DO_ENUM) p->channel, false );
	return SUCCESS;
}

/***************************************************************************/
static int Menu_Bit( struct menu_command_params * p, BOOL * value, BOOL direction )
{
	int      error_code;
	uint16_t min;
	uint16_t max;
	char   	 question_str[LOCAL_STRING_BUFFER_SIZE];

	Menu_Display_Clear_Line( p->out_file );

	String_Copy( question_str, "\n", LOCAL_STRING_BUFFER_SIZE );
	if ( direction /* write */ )
	{
		if ( *value )  String_Concatenate( question_str, "set  ", LOCAL_STRING_BUFFER_SIZE );
		else           String_Concatenate( question_str, "clear", LOCAL_STRING_BUFFER_SIZE );

		String_Concatenate( question_str, " ", LOCAL_STRING_BUFFER_SIZE );
	}

	String_Concatenate( question_str, "chan> ", LOCAL_STRING_BUFFER_SIZE );

	if      ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_STX104 ) )
	{
		min = 0;	max = ( STX104_DO_QTY - 1 );
	}
	else if ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_TRACERE ) )
	{
		min = 0;	max = ( TRACERE_DIO_BIT_QTY - 1 );
	}
	else
	{
		error_code = EC_APP_TYPE_INVALID;
		goto Menu_Bit_Error;
	}

	p->channel = (size_t) Menu_Ask_Uint16_Range( p->in_file, p->out_file, question_str, min, max );

	if ( direction ) error_code = AUL_DO_Set( p->board, p->channel, *value );
	else             error_code = AUL_DI_Get( p->board, p->channel,  value );

	if ( error_code ) goto Menu_Bit_Error;
	return SUCCESS;
Menu_Bit_Error:
	if ( error_code < 0 ) error_code = -error_code;
	fprintf( p->out_file, "\nError %d, %s\n", error_code, AUL_Error_String( error_code ) );
	return -error_code;
}
/***************************************************************************/
int Menu_DO_Bit_Set( struct menu_command_params * p )
{
	BOOL value = true;
	return Menu_Bit( p, &value, 1 /* out */ );
}
/***************************************************************************/
int Menu_DO_Bit_Clear( struct menu_command_params * p )
{
	BOOL value = false;
	return Menu_Bit( p, &value, 1 /* out */ );
}
/***************************************************************************/
int Menu_DI_Bit( struct menu_command_params * p )
{
	BOOL value = false;

	Menu_Bit( p, &value, 0 /* in */ );

	fprintf( p->out_file, "\nDI[%d] = %s\n", p->channel, (value ? bool_str[1] : bool_str[0] ) );
	return SUCCESS;
}

#if(0)
/***************************************************************************/
typedef int (* menu_bit_fnc )(   int board, int channel, BOOL * value );
/***************************************************************************/
static void Menu_DI_Bit_Helper( struct menu_command_params * 	p,
								int 							min,
								int 							max,
								char * 							bit_string,
								int								bit_string_qty,
								uint8_t *						port,
								int								port_qty,
								menu_bit_fnc 					di_get			/* function pointer */
							  )
{
	int		x;
	int		cp;
	int		y;
	int		channel;
	BOOL 	value;
	int		chan_port, chan_bit;
	(void)	port_qty;

	cp = max >> 2;  /* divide by four, due to four bits per nibble */
	if ( ( max >= 8 ) && ( cp > 0 ) ) cp--;  /* adjust for correct number of spaces */
	cp = cp + max + 1 /* null */;  /* total characters required */
	if ( cp > bit_string_qty ) return;
	bit_string[cp] = '\0'; /* null character at end */
	cp--;
	for ( channel = min; channel < max; channel++ )
	{
		BOOL test;
		(*di_get)( p->board, channel,  &test );
		value = test;
//after third call to this function 'cp' gets corrupted.  Why?
		chan_port = channel >> 3;
		chan_bit  = channel - ( chan_port << 3 );
		if ( value )
		{
			bit_string[cp] = '1';
			port[chan_port]  |= 0x01 << chan_bit;
		}
		else
		{
			bit_string[cp] = '0';
		}
		cp--;
		if ( (channel & 0x3) == 0x3 ) cp--; /* add a gap */
	}
}
#endif
/***************************************************************************/
int Menu_DI_Bit_All( struct menu_command_params * p )
{
	/* set this to the the largest number of ports that might ever exist */
#	define MENU_DI_PORT_QTY        6
#   define MENU_DI_PORT_BITS_SIZE  8
#   define MENU_DI_PORT_BITS_WIDTH IOC_IMAX_BITS( MENU_DI_PORT_BITS_SIZE )
	/* length = port_quantity * ( bits_per_port + nibble_spaces ) + null_character */
#	define MENU_DI_BIT_STRING_SIZE ( MENU_DI_PORT_QTY * ( MENU_DI_PORT_BITS_SIZE + 2 ) + 1 )

	uint8_t		port[MENU_DI_PORT_QTY];
	char        bit_string[MENU_DI_BIT_STRING_SIZE];
	uint16_t 	min;
	uint16_t 	max;
	int			index;
	int			cp;
	int			channel;
	BOOL 		value;
	int			chan_port, chan_bit;

	/* initialize values */
	for ( index = 0; index   < MENU_DI_BIT_STRING_SIZE; index++ ) bit_string[index] = ' ';
	for ( index = 0; index   < MENU_DI_PORT_QTY;        index++ ) port[index]       =  0 ;

	cp = MENU_DI_PORT_BITS_WIDTH;

	if      ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_STX104 ) )
	{
		min = 0;	max = STX104_DO_QTY;
		cp = max >> 2;  /* divide by four, due to four bits per nibble */
		if ( ( max >= 8 ) && ( cp > 0 ) ) cp--;  /* adjust for correct number of spaces */
		cp = cp + max + 1 /* null */;  /* total characters required */
		if ( cp > MENU_DI_BIT_STRING_SIZE ) return -1;
		bit_string[cp] = '\0'; /* null character at end */
		cp--;
		for ( channel = min; channel < max; channel++ )
		{
			STX104_DI_Get( p->board, channel,  &value );
			chan_port = channel >> 3;
			chan_bit  = channel - ( chan_port << MENU_DI_PORT_BITS_WIDTH );
			if ( value )
			{
				bit_string[cp] = '1';
				port[chan_port]  |= 0x01 << chan_bit;
			}
			else
			{
				bit_string[cp] = '0';
			}
			cp--;
			if ( (channel & 0x3) == 0x3 ) cp--; /* add a gap */
		}
	}
	else if ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_TRACERE ) )
	{
		min = 0;	max = TRACERE_DIO_BIT_QTY;
		cp = max >> 2;  /* divide by four, due to four bits per nibble */
		if ( ( max >= 8 ) && ( cp > 0 ) ) cp--;  /* adjust for correct number of spaces */
		cp = cp + max + 1 /* null */;  /* total characters required */
		if ( cp > MENU_DI_BIT_STRING_SIZE ) return -1;
		bit_string[cp] = '\0'; /* null character at end */
		cp--;
		for ( channel = min; channel < max; channel++ )
		{
			Tracere_DIO_Bit_Get( p->board, channel,  &value );
			chan_port = channel >> 3;
			chan_bit  = channel - ( chan_port << MENU_DI_PORT_BITS_WIDTH );
			if ( value )
			{
				bit_string[cp] = '1';
				port[chan_port]  |= 0x01 << chan_bit;
			}
			else
			{
				bit_string[cp] = '0';
			}
			cp--;
			if ( (channel & 0x3) == 0x3 ) cp--; /* add a gap */
		}
	}

	fprintf( p->out_file, "\n" );
	fprintf( p->out_file, "        %d . . . %d\n", max - 1, min );
	fprintf( p->out_file, " BITS: %s\n", bit_string );

	max = ( max - 1 ) >> MENU_DI_PORT_BITS_WIDTH;
	fprintf( p->out_file, "        [%d] . . . [%d]\n", max, min );
	fprintf( p->out_file, "PORTS:  " );

	for ( index = max; index >= 0; index-- )
	{
		fprintf( p->out_file, "0x%02X", port[index] );
		if ( (index & 0x3) == 0x3 ) fprintf ( p->out_file, " " ); /* add gap */
		fprintf ( p->out_file, " " );
	}
	fprintf( p->out_file, "\n" );

	return SUCCESS;
#undef MENU_DI_BIT_STRING_SIZE
#undef MENU_DI_PORT_BITS_SIZE
#undef MENU_DI_PORT_BITS_WIDTH
#undef MENU_DI_PORT_QTY
}

/***************************************************************************/
static int Menu_DIO_Port( struct menu_command_params * p, BOOL direction )
{
	int   	error_code;
	uint16_t min;
	uint16_t max;
	uint8_t  value;
	char   question_str[LOCAL_STRING_BUFFER_SIZE];

	Menu_Display_Clear_Line( p->out_file );

	String_Copy( question_str, "\nport ", LOCAL_STRING_BUFFER_SIZE );
	if ( direction ) String_Copy( question_str, "write", LOCAL_STRING_BUFFER_SIZE );
	else             String_Copy( question_str, "read ", LOCAL_STRING_BUFFER_SIZE );

	String_Concatenate( question_str, " chan> ", LOCAL_STRING_BUFFER_SIZE );

	min = 0;
	if      ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_STX104 ) )
	{
		max = 1 - 1;
	}
	else if ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_TRACERE ) )
	{
		min = 0;	max = ( TRACERE_DIO_PORT_QTY - 1 );
	}
	else
	{
		error_code = EC_APP_TYPE_INVALID;
		goto Menu_DIO_Port_Error;
	}

	p->channel = (size_t) Menu_Ask_Uint16_Range( p->in_file, p->out_file, question_str, min, max );

	if ( direction )
	{
		value = (size_t) Menu_Ask_Uint16_Range( p->in_file, p->out_file, "\n  data> ", 0, 255 );
		error_code = AUL_DIO_Port_Write( p->board, p->channel, value );
	}
	else
	{
		error_code = AUL_DIO_Port_Read(  p->board, p->channel, &value );
		if ( SUCCESS == error_code )
		{
			fprintf( p->out_file, "\n  port[%d] = 0x%02X\n", p->channel, value );
		}
	}
	if ( error_code ) goto Menu_DIO_Port_Error;
	return SUCCESS;
Menu_DIO_Port_Error:
	if ( error_code < 0 ) error_code = -error_code;
	fprintf( p->out_file, "\nError %d, %s\n", error_code, AUL_Error_String( error_code ) );
	return -error_code;
}
/***************************************************************************/
int Menu_DIO_Port_Write( struct menu_command_params * p )
{
	return Menu_DIO_Port( p, true /* write */ );
}
/***************************************************************************/
int Menu_DIO_Port_Read( struct menu_command_params * p )
{
	return Menu_DIO_Port( p, false /* read */ );
}



/***************************************************************************/
int Menu_Quit( struct menu_command_params * p )
{
	(void) p;
	return -EC_DEMO_MENU_QUIT;
}
/***************************************************************************/
int Menu_Report_File( struct menu_command_params * p )
{
	FILE * report_file;
#ifdef __BORLANDC__
#elif _WIN32
	int  error_code;
#endif

#ifdef __BORLANDC__
	report_file = fopen( "report.txt", "wt" );
#elif _WIN32
	error_code = fopen_s( &report_file, "report.txt", "wt" );
#else
	report_file = fopen( "report.txt", "wt" );
#endif
	if      ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_STX104 ) )
	{
		STX104_Report_Full( p->board, report_file );
	}
	else if ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_TRACERE ) )
	{
		Tracere_Report_Full( p->board, report_file );
	}
	/* close the file */
	fclose( report_file );
	return SUCCESS;
}
/***************************************************************************/
int Menu_Report_Console( struct menu_command_params * p )
{
	if      ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_STX104 ) )
	{
		STX104_Report_Full( p->board, p->out_file );
	}
	else if ( AUL_Is_Logical_Device( p->board, DEVICE_LOGICAL_TRACERE ) )
	{
		Tracere_Report_Full( p->board, p->out_file );
	}
	return SUCCESS;
}
/***************************************************************************/
int Menu_Redraw( struct menu_command_params * p )
{
	Menu_Display( menu_menu_list, p->board, p->out_file, p->info );
	return SUCCESS;
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

#ifdef __linux__

#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
/* #include <sys/time.h> */
#include "../shared/mem.h"

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/*                       < < <  K E Y B O A R D > > >					  	*/
/****************************************************************************/
/****************************************************************************/


/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
static int Menu_Keyboard_Get( void )
{
	int character;
	struct termios orig_term_attr;
	struct termios new_term_attr;

	/* set the terminal to raw mode */
	tcgetattr(fileno(stdin), &orig_term_attr);
	memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
	new_term_attr.c_lflag &= ~(ECHO|ICANON);
	new_term_attr.c_cc[VTIME] = 0;
	new_term_attr.c_cc[VMIN] = 0;
	tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

	/* read a character from the stdin stream without blocking */
	/*   returns EOF (-1) if no character is available */
	character = fgetc(stdin);

	/* restore the original terminal attributes */
	tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

	return character;
}



/***************************************************************************/
/***************************************************************************/

#endif /* __linux__ */



/***************************************************************************/
BOOL Menu_Get_Character( int * menu_character )
{
	BOOL result = false;

#ifdef __BORLANDC__
	if ( kbhit() )
	{
		*menu_character = getch();
		result = true;
	}
	else
	{
		*menu_character = NULL_CHARACTER;
	}
#elif _WIN32
	if ( _kbhit() )
	{
		*menu_character = getch();
		result = true;
	}
	else
	{
		*menu_character = NULL_CHARACTER;
	}
#else /* assuming linux */

	int character = Menu_Keyboard_Get();
	if ( character >= 0 )
	{
		*menu_character = character;
		result = true;
	}
	else
	{
		*menu_character = NULL_CHARACTER;
	}
#endif
	return result;
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/



/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/*                       < < <  T I M I N G  > > >						  	*/
/****************************************************************************/
/****************************************************************************/

#ifdef __BORLANDC__
#elif _WIN32
#else /* assuming linux */

#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include <time.h>

/***************************************************************************/
#define MENU_TIMER_SIG SIGRTMAX              /* timer notification signal */
/***************************************************************************/

/******************************************************************//**
 * @struct menu_timer_data
 * @brief
 *
 */
struct menu_timer_data
{
	menu_loop_function_type  		loop_function;
	menu_stop_request_function_type	stop_request_function;
	void * 							private_data;
	timer_t							timer_id;
};
/***************************************************************************/
static struct menu_timer_data  menu_timer_info;
/******************************************************************//**
 * @brief
 *
 *
 * The librt.a library to support High Resolution POSIX Timers (HRT)
 * is added to the linker.  The following functions are a part of this
 * library:  timer_create(), timer_settime(), timer_delete().
 *
 * Location of library:  ./usr/lib/i386-linux-gnu/librt.a
 *
 *
 */
static void Menu_Signal_Action_Handler( int signal, siginfo_t * si, void * context )
{
	int							error_code;
	struct menu_timer_data * 	mtd;
	(void) signal;
	(void) context;

	mtd = (struct menu_timer_data *) ( si->si_value.sival_ptr );
	error_code = (* mtd->loop_function)( mtd->private_data );
	if ( -EC_DEMO_MENU_QUIT == error_code )
	{
		timer_delete( mtd->timer_id );
		mtd->timer_id = NULL;
		(* mtd->stop_request_function )( mtd->private_data );
	}
}
/******************************************************************//**
 * @brief
 *
 *
 * The librt.a library to support High Resolution POSIX Timers (HRT)
 * is added to the linker.  The following functions are a part of this
 * library:  timer_create(), timer_settime(), timer_delete().
 *
 * Location of library:  ./usr/lib/i386-linux-gnu/librt.a
 *
 * @param time_sec
 * @param loop_function
 * @param stop_request_function
 * @param private_data
 * @return
 */
int	Menu_Periodic_Run( 	double							time_sec,
						menu_loop_function_type 		loop_function,
						menu_stop_request_function_type stop_request_function,
						void * 							private_data
					 )
{
	int							error_code;
	struct sigaction 			sa; /* signal action */
	struct sigevent	 			se; /* signal event  */
	struct menu_timer_data * 	mtd;/* menu timer data */
	struct itimerspec 			ts; /* timer specification */
	error_code = SUCCESS;

	/* create a signal handler for the timing signal */
	sa.sa_flags 		= SA_SIGINFO;
	sa.sa_sigaction 	= Menu_Signal_Action_Handler;
	sigemptyset( &sa.sa_mask );

	if ( sigaction( MENU_TIMER_SIG, &sa, NULL ) == -1 )
	{ error_code = -EC_DEMO_MENU_TIMER_SIGACTION;	goto Menu_Periodic_Run_Error;	}

	mtd = &(menu_timer_info);

	if ( NULL != mtd->timer_id )
	{ error_code = -EC_DEMO_MENU_TIMER_ACTIVE;		goto Menu_Periodic_Run_Error;	}

	if ( time_sec < 0.001 )
	{ error_code = -EC_DEMO_MENU_TIMER_INFO_INVALID;  goto Menu_Periodic_Run_Error;	}

	if ( NULL == loop_function )
	{ error_code = -EC_DEMO_MENU_TIMER_INFO_INVALID;  goto Menu_Periodic_Run_Error;	}

	if ( NULL == stop_request_function )
	{ error_code = -EC_DEMO_MENU_TIMER_INFO_INVALID;  goto Menu_Periodic_Run_Error;	}

	mtd->loop_function 			= loop_function;
	mtd->stop_request_function 	= stop_request_function;
	mtd->private_data			= private_data;
	se.sigev_notify = SIGEV_SIGNAL;			/* notify via signal 	*/
	se.sigev_signo	= MENU_TIMER_SIG;		/* timer signal			*/
	se.sigev_value.sival_ptr = mtd;

	ts.it_interval.tv_sec = (long int) time_sec;
	time_sec = time_sec - ( (double) ts.it_interval.tv_sec );  /* remove whole seconds */
	time_sec = time_sec * 1.0e9; /* convert to nanoseconds */
	ts.it_interval.tv_nsec = (long) time_sec;
	ts.it_value.tv_sec  = ts.it_interval.tv_sec;
	ts.it_value.tv_nsec = ts.it_interval.tv_nsec;

	/* http://man7.org/linux/man-pages/man2/timer_create.2.html */
	error_code = timer_create( CLOCK_REALTIME /* clock_id */, &se, &(mtd->timer_id) );
	if ( error_code )
	{ error_code = -EC_DEMO_MENU_TIMER_CREATE;  goto Menu_Periodic_Run_Error;	}

	/* http://man7.org/linux/man-pages/man2/timer_settime.2.html */
	error_code = timer_settime( mtd->timer_id, 0, &ts, NULL );
	if ( error_code )
	{ error_code = -EC_DEMO_MENU_TIMER_SETTIME;  goto Menu_Periodic_Run_Error;	}

/*printf( "mtd->timer_id = %p\n", mtd->timer_id );*/

Menu_Periodic_Run_Error:
	return error_code;
}

#endif /* assuming linux */

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/


/**************************************************************************

Assumptions:
1. Limited characters used 
*/
void Menu_Display( 	struct menu_command 	menu_list[],
					int 				board,
					FILE * 				out_file,
					void *            	info      /* private application data */
                 )
{
	size_t	index;
	BOOL	command_valid;
	BOOL	display_only;
	BOOL	line_terminate;
#define MENU_DISPLAY_LIST_SIZE 12
	char    char_list[MENU_DISPLAY_LIST_SIZE];
	char    char_str[2];
	struct menu_command_params p;

	p.board    = board;
	p.in_file  = NULL;
	p.out_file = out_file;
	p.info     = info;
	p.channel  = 0;

	index = 0;
	strcpy( char_list, "" );

	while ( ( NULL != menu_list[index].function )  ||
		   ( NULL != menu_list[index].display  )   )
	{
		command_valid  = false;
		display_only   = false;
		line_terminate = false;

		/* build character list */
		if ( ( menu_list[index].char_first >= ' ' ) && ( menu_list[index].char_last >= ' ' ) )
		{ /* character input range exists */
			if ( strlen( char_list ) > 0 ) strcat( char_list, "," );
			char_str[0] = menu_list[index].char_first;
			char_str[1] = NULL_CHARACTER;
			strcat( char_list,  char_str );
			strcat( char_list,  "-" );
			char_str[0] = menu_list[index].char_last;
			char_str[1] = NULL_CHARACTER;
			strcat( char_list,  char_str  );
		}
		else if ( menu_list[index].char_first >= ' ' )
		{ /* only first character exists */
			if ( strlen( char_list ) > 0 ) strcat( char_list, "," );
			char_str[0] = menu_list[index].char_first;
			char_str[1] = NULL_CHARACTER;
			strcat( char_list,  char_str  );
		}
		else
		{  /* display string only  -- because char_first is a null character */
			display_only = true;
			if ( menu_list[index].char_last < ' ' )
			{  /* print out prefix spaces */
				size_t index;
				for ( index = 0; index < ((size_t) menu_list[index].char_last); index++ )
				{
					fprintf( out_file, " " );
				}
			}
			/* determine whether or not to run display helper function */
			if ( NULL != menu_list[index].function ) command_valid = true;
			/* output display item if available */
			if ( NULL != menu_list[index].display )
			{
			fprintf( out_file, "%s", menu_list[index].display );
			line_terminate = true;
			}
		}
		/* output line */
		if ( false == display_only )
		{
			if ( NULL != menu_list[index].display )
			{
				fprintf( out_file, "%8s) %s", char_list, menu_list[index].display );
				strcpy( char_list, "" );
				line_terminate = true;
			}
		}
		/* run display helper function */
		if ( command_valid )
		{
			(* menu_list[index].function )( &p );
			line_terminate = true;
			/*command_valid = false;*/
		}
		/* line termination */
		if ( line_terminate )
		{
			fprintf( out_file, "\n" );
			/*line_terminate = false;*/
		}
		index++;
	}  /* while */
}
/***************************************************************************/
int Menu_Command( struct menu_command menu_list[],
                  int 				board,
                  FILE * 			in_file,
                  FILE * 			out_file,
                  void *            info      /* private application data */
                )
{
	size_t	index;
	int  	char_menu;
	int		error_code;
	BOOL	command_valid;
	struct menu_command_params p;

	p.board    = board;
	p.in_file  = in_file;
	p.out_file = out_file;
	p.info     = info;
	p.channel  = 0;

	error_code = SUCCESS;

	if( Menu_Get_Character( &char_menu ) )
	{
		index = 0;
		while ( ( NULL != menu_list[index].function )  ||
				( NULL != menu_list[index].display  )   )
		{
			command_valid = false;
			/* determine channel info and command validity */
			if ( menu_list[index].char_last >= ' ' )
			{
				if ( ( char_menu >= menu_list[index].char_first ) &&
					 ( char_menu <= menu_list[index].char_last  )  )
				{
					command_valid = true;
				}
			}
			else if ( char_menu == menu_list[index].char_first )
			{
				command_valid = true;
			}

			if ( command_valid )
			{ /* command found, process it */
				/* build channel information here */
				if ( ( menu_list[index].char_first >= ' ' ) && ( menu_list[index].char_last >= ' ' ) )
				{
					p.channel = (size_t) ( char_menu - menu_list[index].char_first );
					/*NOTE: this is an exception that is not well handled!!!*/
					if ( 'a' == menu_list[index].char_first ) p.channel = p.channel + 10;
				}
				else if ( menu_list[index].char_last < ' ' )
				{
					p.channel = (int) menu_list[index].char_last;
				}

				error_code = (* menu_list[index].function )( &p );
				return error_code;
			}
			else
			{  /* command not found - test next */
			index++;
			}
		} /* while */
	} /* if menu character available */
	return error_code;
}

/***************************************************************************/
static char menu_data_str[MENU_DISPLAY_LINE_LENGTH];

/**************************************************************************
Assumption:
*/
void Menu_Data_Append( struct menu_data_params * mdp, char * msg_str )
{
	int msg_str_length;
	int index;

	msg_str_length = (int) strlen( msg_str );

	if ( (mdp->column + msg_str_length) > mdp->out_str_size )
	{
		msg_str_length = mdp->out_str_size - mdp->column;
	}

	for ( index = 0; index < msg_str_length; index++ )
	{
		mdp->out_str[mdp->column + index] = msg_str[index];
	}

	mdp->column = mdp->column + msg_str_length;
}
/***************************************************************************/
void Menu_Data(	struct menu_data * 	data_list,
				int 				column_begin,
                int 				board,
                FILE * 				out_file,
                void *				info  		/* private data */
             )
{
	int index;
	/* int max_width; */
	int col_delta;
	/*int column;*/
	int row;
	int error_code;  /* used for debugging */
	BOOL not_done;
	struct menu_data_params mdp; /* mdp = short hand for menu data parameters */
	(void) board;

	/* assign initial values */
	mdp.out_str_size = ((int) (MENU_DISPLAY_LINE_LENGTH - 1) );
	mdp.info         = info;
	mdp.out_str      = menu_data_str;

	mdp.column   = 0;
	row          = 0;
	not_done     = true;

	/* add spaces, if necessary  */
	col_delta = data_list[row].column_start - column_begin;
	for ( index = 0; index < col_delta; index++ )
	{
		menu_data_str[mdp.column] = ' ';
		mdp.column++;
	}

	while ( not_done )
	{
		/* add spaces, if necessary  */
		col_delta = data_list[row].column_start - mdp.column;
		for ( index = 0; index < col_delta; index++ )
		{
			menu_data_str[mdp.column] = ' ';
			mdp.column++;
		}
		/* post prefix, if present */
		if ( data_list[row].prefix_length > 0 )
		{
			for ( index = 0; index < data_list[row].prefix_length; index++ )
			{
				menu_data_str[mdp.column] = data_list[row].prefix[index];
				mdp.column++;
			}
		}
		/* run helper function */
		if ( NULL != data_list[row].function )
		{
			mdp.width_max = ((int) MENU_DISPLAY_LINE_LENGTH) - mdp.column - 1 /* null */;
			/* if width max specified then test against biggest value */
			if ( data_list[row].width_max > 0 )
			{
				if ( data_list[row].width_max < mdp.width_max )
				mdp.width_max = data_list[row].width_max;
			}
			/* call the menu data helper function */
			error_code = ( *(data_list[row].function) )( &mdp );
			if ( mdp.column < 0 )
			{
				menu_data_str[0] = NULL_CHARACTER;
				not_done = false;
			}
		}

		/* prepare for next row, quit if at end of rows )  */
		row++;
		if (  ( NULL == data_list[row].function )  &&
		( NULL == data_list[row].prefix   )   )
		{
			not_done = false;
		}

		/*if ( row == 2 ) not_done = false;*/

	} /* while */

	menu_data_str[mdp.column] = NULL_CHARACTER;
	fprintf( out_file, "%s\r", menu_data_str );

	/*fprintf( out_file, "%s   col=%d\n", menu_data_str, mdp.column );*/
}

/***************************************************************************/
void Menu_Initialization( struct menu_command menu_list[] )
{
#if __linux__
/*	Menu_Keyboard_Initialize();*/
	Mem_Set( &menu_timer_info, 0, sizeof( menu_timer_info ) );
#endif
	menu_menu_list = menu_list;
}
/***************************************************************************/
void Menu_Termination( void )
{
#if __linux__
/*	Menu_Keyboard_Terminate();*/
#endif
	menu_menu_list = NULL;
}
/* file end */


