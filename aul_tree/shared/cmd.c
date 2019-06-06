
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
#include "cmd.h"
#include "typebase.h"  /* NULL */
#include "status_macros.h"
#include "../shared/mem.h"
#include "../shared/debug.h"
#include "../shared/compiler.h"
#include "../str_safe/str_safe.h"

/*********************************************************************************/
/* function name replacement for OS compatibility */
#if defined( _WIN32 )
# include <stdarg.h>						/* variable argument list */
# include <stdio.h>
# ifndef CMD_SNPRINTF
#  define CMD_SNPRINTF    _snprintf
# endif
#else
# ifndef CMD_SNPRINTF
#  ifdef COMPILER_APPLICATION
#   include <stdio.h>
#   pragma message ( "aul_tree: cmd.c COMPILER_APPLICATION" )
#  else
/*#  include <linux/kernel.h> */
#   pragma message ( "aul_tree: cmd.c __KERNEL__" )
#  endif
#  define CMD_SNPRINTF    snprintf
# endif
#endif


/*********************************************************************************/
/* if DEBUG then allow MODULE_DBG.  Note: MODULE_DEBUG conflicts with debug.h */
#define COMMAND_DBG		   _DEBUG
/*********************************************************************************/
/* #define COMMAND_DEBUG_LOG   DEBUG_OUT */
#define COMMAND_DEBUG_LOG   DEBUG_FILE


#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
/*********************************************************************************/
struct command_dataset
{
	size_t			cmd;
	const char *	cmd_name;
};
/*********************************************************************************/
static const struct command_dataset command_ds_list[]=
{

#if defined( MODULE_SYS_COMMAND )
	MODULE_SYS_CMD_EC_DEFINITIONS( CMD_DATASET_EXTRACTOR )
#endif
/* 10 -- Array  */
#if defined( MODULE_ARRAY_COMMAND )
	MODULE_ARRAY_CMD_EC_DEFINITIONS( CMD_DATASET_EXTRACTOR )
#endif
/* 7 -- AULDRV - generic AUL driver    */
#if defined( MODULE_AULDRV_COMMAND )
	MODULE_AULDRV_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )
#endif
/* 8 -- I/O  */
#if defined( MODULE_IO_COMMAND )
	MODULE_IO_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )
#endif
/* 11 -- Component manager  */
#if defined( MODULE_CMPMGR_COMMAND )
	MODULE_CMPMGR_CMD_EC_DEFINITIONS( CMD_DATASET_EXTRACTOR )
#endif
/* 15 - Auto code generation and reporting  */
#if defined( MODULE_ACGR_COMMAND )
	MODULE_ACGR_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )
#endif

/* All of the component level error codes are installed here 
    and these are found in cmp_avail_status.h
*/
#if defined ( COMPONENTS_AVAILABLE_COMMAND )
	COMPONENT_AVAIL_CMD_DATASET
#endif

/* All of the device level error codes are installed here 
    and these are found in dev_avail_cmd.h
*/
#if defined ( DEVICES_AVAILABLE_COMMAND )
	DEVICE_AVAIL_CMD_DATASET
#endif
	{ 0, NULL }
};
/*********************************************************************************/
size_t Command_All_Count( void )
{
	size_t count;
	size_t index;
	count = 0;
	index = 0;
	while ( 0 != command_ds_list[index].cmd ) 
	{
		count++;		index++;
	}
	return count;
}

/*********************************************************************************/
#endif
/*********************************************************************************/
/*********************************************************************************/
int Command_Initialize( void )
{
	return SUCCESS;
}
/*********************************************************************************/
int Command_Terminate( void )
{
	return SUCCESS;
}

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
/*********************************************************************************/
/*   REPORTING FUNCTIONS   */


/*********************************************************************************/
static const char * command_csv_column_names[] =
{
	/*	COMMAND_CSV_COLUMN_NONE			*/		"",
	/*	COMMAND_CSV_COLUMN_ENUM_VALUE	*/		"VALUE",
	/*	COMMAND_CSV_COLUMN_ENUM_SYMBOL	*/		"SYMBOL",
												NULL
};
/*********************************************************************************/
int Command_CSV_Column_Name( COMMAND_CSV_COLUMN_ENUM column, char * buf_io, size_t buf_size )
{
	const char *	string_pointer;
	size_t			string_length;

	if ( ( column >= COMMAND_CSV_COLUMN_UNKNOWN )		|| 
		  ( column <  COMMAND_CSV_COLUMN_ENUM_VALUE   )  )	goto Command_CSV_Column_Name_Error;

	/* initialize */
	string_pointer = command_csv_column_names[column];
	String_Copy( buf_io, string_pointer, buf_size );
	string_length = String_Length( string_pointer ) + 1 /* null */;
	if ( string_length >= buf_size ) goto Command_CSV_Column_Name_Error;
	return SUCCESS;
Command_CSV_Column_Name_Error:
#if defined( STATUS_DEBUG_LOG )
	Debug_Log( STATUS_DEBUG_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
int Command_CSV_Cell_Get( size_t row, COMMAND_CSV_COLUMN_ENUM column, char * cell_buf_io, size_t cell_buf_size )
{
	const char *	string_pointer;
	size_t			string_length;
#define COMMAND_CW_LEN  32
	char           buf[COMMAND_CW_LEN];

	size_t status_count = Command_All_Count();

	if ( row >= status_count )										goto Command_CSV_Cell_Get_Error;

	if ( ( column >= COMMAND_CSV_COLUMN_UNKNOWN )	 || 
		  ( column <  COMMAND_CSV_COLUMN_ENUM_VALUE  )  )	goto Command_CSV_Cell_Get_Error;

	/* initialize */
	Mem_Set( cell_buf_io, '\0', cell_buf_size );
	string_length  = 0;
	string_pointer = NULL;
	switch( column )
	{
		case COMMAND_CSV_COLUMN_ENUM_VALUE:
			Mem_Set( buf, 0, COMMAND_CW_LEN );
			CMD_SNPRINTF( buf, COMMAND_CW_LEN, "%d", command_ds_list[row].cmd );
			if ( '\0' != buf[COMMAND_CW_LEN - 1] ) 
			{	buf[COMMAND_CW_LEN - 1] = '\0';		goto Command_CSV_Cell_Get_Error;	}
			string_pointer = buf;
			break;
		case COMMAND_CSV_COLUMN_ENUM_SYMBOL:
			string_pointer = command_ds_list[row].cmd_name;
			break;
		/* case COMMAND_CSV_COLUMN_MESSAGE:
			string_pointer = command_ds_list[row].name;
			break;    */
		default: break;
	}
	if ( NULL == string_pointer ) return SUCCESS;
	string_length += String_Length( string_pointer ) + 1 /* null */;
	String_Concatenate( cell_buf_io, string_pointer, cell_buf_size );
	if ( string_length >= cell_buf_size ) goto Command_CSV_Cell_Get_Error;
	return SUCCESS;
Command_CSV_Cell_Get_Error:
#if defined( COMMAND_DEBUG_LOG )
	Debug_Log( COMMAND_DEBUG_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
int Command_CSV_Column_Width( COMMAND_CSV_COLUMN_ENUM column )
{
	const char *	string_pointer;
	size_t			width, width_max;
	size_t			row_count;
	size_t			row;
#define COMMAND_CW_LEN  32
	char           buf[COMMAND_CW_LEN];

	if ( ( column >= COMMAND_CSV_COLUMN_UNKNOWN )	 || 
		  ( column <  COMMAND_CSV_COLUMN_ENUM_VALUE  )  )	goto Command_CSV_Column_Width_Error;

	row_count = Command_All_Count();
	width_max = 0;
	for ( row = 0; row < row_count; row++ )
	{
		switch( column )
		{
			case COMMAND_CSV_COLUMN_ENUM_VALUE:
				Mem_Set( buf, 0, COMMAND_CW_LEN );
				CMD_SNPRINTF( buf, COMMAND_CW_LEN, "%d", command_ds_list[row].cmd );
				if ( '\0' != buf[COMMAND_CW_LEN - 1] ) 
				{	buf[COMMAND_CW_LEN - 1] = '\0';		goto Command_CSV_Column_Width_Error;	}
				string_pointer = buf;
				return SUCCESS;
				break;
			case COMMAND_CSV_COLUMN_ENUM_SYMBOL:
				string_pointer = command_ds_list[row].cmd_name;
				break;
			default: 
				string_pointer = NULL;
				break;
			/*case STATUS_CSV_COLUMN_MESSAGE:
				string_pointer = status_ds_list[row].name;
				break;  */
		}
		if ( NULL != string_pointer )
		{
			width = String_Length( string_pointer );
			if ( width > width_max ) width_max = width;
		}
	}
	return width_max;
Command_CSV_Column_Width_Error:
#if defined( COMMAND_DEBUG_LOG )
	Debug_Log( COMMAND_DEBUG_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
#endif
