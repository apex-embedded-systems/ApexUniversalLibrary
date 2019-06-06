
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
#ifndef __KERNEL__
# include "../shared/target.h"
#endif

#include "../shared/compiler.h"
#include "status.h"
#include "typebase.h"  /* NULL */
#include "../shared/module_shared.h"
#include "../shared/mem.h"
#include "../shared/debug.h"			/* debugging support			*/

/*********************************************************************************/
/* function name replacement for OS compatibility */
#if defined( _WIN32 )
# include <stdarg.h>						/* variable argument list */
# include <stdio.h>
# ifndef STATUS_SNPRINTF
#  define STATUS_SNPRINTF    _snprintf
# endif
#else
# ifndef STATUS_SNPRINTF
#  ifdef COMPILER_APPLICATION
#   pragma message ( "aul_tree: status.c  COMPILER_APPLICATION" )
#   include <stdio.h>
#   include <string.h>   /* strerror() */
#  else
/*# include <linux/kernel.h> */
#   pragma message ( "aul_tree: status.c __KERNEL__" )
#  endif
#  define STATUS_SNPRINTF    snprintf
#  pragma message ( "aul_tree: status.c  STATUS_SNPRINTF=snprintf" )
# endif
#endif


/*********************************************************************************/
#define STATUS_MODULE_REV  "$Date: 2015-02-01 15:12:58 -0600 (Sun, 01 Feb 2015) $"

/*********************************************************************************/
/* if DEBUG then allow MODULE_DBG.  Note: MODULE_DEBUG conflicts with debug.h */
#define STATUS_DBG		   _DEBUG
/*********************************************************************************/
/*#define STATUS_DEBUG_LOG   DEBUG_OUT*/
#define STATUS_DEBUG_LOG   DEBUG_FILE

/*********************************************************************************/
struct status_dataset
{
	size_t			ec;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	const char *   ec_name;
#endif
	const char *	name;
};

/*********************************************************************************/
static const struct status_dataset status_ds_list[]=
{
#if defined( MODULE_SYS_STATUS )
	MODULE_SYS_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
/* 4 -- Memory    */
#if defined( MODULE_MEM_STATUS )
	MODULE_MEM_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
/* 10 -- Array    */
#if defined( MODULE_ARRAY_STATUS )
	MODULE_ARRAY_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
/* 7 -- AULDRV - generic AUL driver    */
#if defined( MODULE_AULDRV_STATUS )
	MODULE_AULDRV_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
/* 8 -- I/O    */
#if defined( MODULE_IO_STATUS )
	MODULE_IO_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
/* 11 -- Component manager    */
#if defined( MODULE_CMPMGR_STATUS )
	MODULE_CMPMGR_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
/* 12 -- Device manager    */
#if defined( MODULE_DEVMGR_STATUS )
	MODULE_DEVMGR_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
/* 13 - NOTE: AUL Library interface    */
#if defined( MODULE_AUL_STATUS )
	MODULE_AUL_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
/* 14 - AUL Hardware definition file import    */
#if defined( MODULE_AULHWDEF_STATUS )
	MODULE_AULHWDEF_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
/* 15 - Auto code generation and reporting    */
#if defined( MODULE_ACGR_STATUS )
	MODULE_ACGR_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif

/* All of the component level error codes are installed here 
    and these are found in cmp_avail_status.h
*/
#if defined ( COMPONENTS_AVAILABLE_STATUS )
	COMPONENT_AVAIL_STATUS_NAMES
#endif

#if defined ( DEVICES_AVAILABLE_STATUS )
	DEVICE_AVAIL_STATUS_NAMES
#endif
/*
#if defined( MODULE_I8255M0_STATUS )
	MODULE_I8255M0_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif

#if defined( MODULE_DAC6CH_STATUS )
	MODULE_DAC6CH_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif
*/


#if defined ( APPLICATION_STATUS )
	APPLICATION_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#endif

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	{ 0, NULL, NULL }
#else
	{ 0, NULL }
#endif
};
/*********************************************************************************/

/*********************************************************************************/
static const char status_success_name[] = "success";
static const char status_zero_length_string[] = "\0";
/*********************************************************************************/
#if defined( _WIN32    )
#include <windows.h>
#define STATUS_STRING_BUF_LENGTH  256
static char status_string_buf[STATUS_STRING_BUF_LENGTH];

static void Status_String_Win32_Get( int error_code )
{
	wchar_t buf[STATUS_STRING_BUF_LENGTH];
	FormatMessage(	FORMAT_MESSAGE_FROM_SYSTEM, 
					NULL,
					error_code,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					buf,
					STATUS_STRING_BUF_LENGTH,
					NULL
					 );
	wcstombs( status_string_buf, buf, STATUS_STRING_BUF_LENGTH );
}
#endif
/*********************************************************************************/
const char * Status_EC_String( int error_code )
{
	size_t index;
	if ( SUCCESS == error_code ) return status_success_name;
	if ( error_code < 0 ) error_code = -error_code;

	if ( EC_ID_IS_OS( error_code ) )
	{
#if defined( _WIN32    )
/* http://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror  */
		Status_String_Win32_Get( error_code );
		return (const char *) status_string_buf;
#elif defined( __linux__ )
	#ifdef __KERNEL__
		return status_zero_length_string;  /* no strerror() within kernel */
	#else
		return strerror( error_code );
	#endif
#else
		return NULL;
#endif	
	}
	else
	{
		index = 0;
		while( 0 != status_ds_list[index].ec )
		{
			if ( ((size_t) error_code) == status_ds_list[index].ec )
			{
				return status_ds_list[index].name;
			}
			index++;
		}
	}
	return NULL;	
}
/*********************************************************************************/
size_t Status_All_Count( void )
{
	size_t count;
	size_t index;
	count = 0;
	index = 0;
	while ( 0 != status_ds_list[index].ec ) 
	{
		count++;		index++;
	}
	return count;
}
/*********************************************************************************/
int Status_Initialize( void )
{
#if defined( STATUS_DEBUG_LOG )
/*	size_t	index;

	index = 0;
	while( 0 != status_ds_list[index].ec )
	{
		Debug_Log( STATUS_DEBUG_LOG " %3d: ec=0x%08X id=%10s local=%04d hr=%s\n",
										index,
										status_ds_list[index].ec,
										Module_Name( EC_GET_ID( status_ds_list[index].ec ) ),
										EC_GET_CODE( status_ds_list[index].ec ),
										status_ds_list[index].name
					);
		index++;
	}*/
#endif
	return SUCCESS;
}
/*********************************************************************************/
int Status_Terminate( void )
{
	return SUCCESS;
}
/*********************************************************************************/
module_definition_type module_status_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	Status_Initialize,
	Status_Terminate,
	MODULE_STATUS_NAME,
	STATUS_MODULE_REV,
	NULL,
	NULL
};

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
/*********************************************************************************/
/*   REPORTING FUNCTIONS   */

#include "../str_safe/str_safe.h"
#include <stdio.h>
/*********************************************************************************/
static const char * status_csv_column_names[] =
{
	/*	STATUS_CSV_COLUMN_NONE			*/		"",
	/*	STATUS_CSV_COLUMN_ENUM_VALUE	*/		"VALUE",
	/*	STATUS_CSV_COLUMN_ENUM_SYMBOL	*/		"SYMBOL",
	/*	STATUS_CSV_COLUMN_MESSAGE		*/		"MESSAGE",
	/* STATUS_CSV_COLUMN_MODULE_NAME    */    	"MODULE_NAME",
												NULL
};
/*********************************************************************************/
int Status_CSV_Column_Name( STATUS_CSV_COLUMN_ENUM column, char * buf_io, size_t buf_size )
{
	const char *	string_pointer;
	size_t			string_length;

	if ( ( column >= STATUS_CSV_COLUMN_UNKNOWN )		|| 
		  ( column <  STATUS_CSV_COLUMN_ENUM_VALUE   )  )	goto Status_CSV_Column_Name_Error;

	/* initialize */
	string_pointer = status_csv_column_names[column];
	String_Copy( buf_io, string_pointer, buf_size );
	string_length = String_Length( string_pointer ) + 1 /* null */;
	if ( string_length >= buf_size ) goto Status_CSV_Column_Name_Error;
	return SUCCESS;
Status_CSV_Column_Name_Error:
#if defined( STATUS_DEBUG_LOG )
	Debug_Log( STATUS_DEBUG_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
int Status_CSV_Cell_Get( size_t row, STATUS_CSV_COLUMN_ENUM column, char * cell_buf_io, size_t cell_buf_size )
{
	const char *	string_pointer;
	size_t			string_length;
	size_t status_count = Status_All_Count();

	if ( row >= status_count )								goto Status_CSV_Cell_Get_Error;

	if ( ( column >= STATUS_CSV_COLUMN_UNKNOWN )	 || 
		  ( column <  STATUS_CSV_COLUMN_ENUM_VALUE  )  )	goto Status_CSV_Cell_Get_Error;

	/* initialize */
	Mem_Set( cell_buf_io, '\0', cell_buf_size );
	/*String_Copy( cell_buf_io, "", cell_buf_size );*/
	string_length  = 0;
	string_pointer = NULL;
	switch( column )
	{
		case STATUS_CSV_COLUMN_ENUM_VALUE:
/* TODO: */
			STATUS_SNPRINTF( cell_buf_io, cell_buf_size, "%d", status_ds_list[row].ec );
			if ( '\0' != cell_buf_io[cell_buf_size - 1] ) 
			{	cell_buf_io[cell_buf_size - 1] = '\0';		goto Status_CSV_Cell_Get_Error;	}
			return SUCCESS;
			break;
		case STATUS_CSV_COLUMN_ENUM_SYMBOL:
			string_pointer = status_ds_list[row].ec_name;
			break;
		case STATUS_CSV_COLUMN_MESSAGE:
			string_pointer = status_ds_list[row].name;
			break;
		case STATUS_CSV_COLUMN_MODULE_NAME:
			{
				size_t id = EC_GET_ID( status_ds_list[row].ec );
				string_pointer = Module_Name( id );
			}
			break;
		default:
			string_pointer = NULL;
			break;
	}
	if ( NULL == string_pointer ) return SUCCESS;
	string_length += String_Length( string_pointer ) + 1 /* null */;
	String_Concatenate( cell_buf_io, string_pointer, cell_buf_size );
	if ( string_length >= cell_buf_size ) goto Status_CSV_Cell_Get_Error;
	return SUCCESS;
Status_CSV_Cell_Get_Error:
#if defined( STATUS_DEBUG_LOG )
	Debug_Log( STATUS_DEBUG_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
int Status_CSV_Column_Width( STATUS_CSV_COLUMN_ENUM column )
{
	const char *	string_pointer;
	size_t			width, width_max;
	size_t			row_count;
	size_t			row;
#define STATUS_CW_LEN  32
	char           buf[STATUS_CW_LEN];

	if ( ( column >= STATUS_CSV_COLUMN_UNKNOWN )	 || 
		  ( column <  STATUS_CSV_COLUMN_ENUM_VALUE  )  )	goto Status_CSV_Column_Width_Error;

	row_count = Status_All_Count();
	width_max = 0;
	for ( row = 0; row < row_count; row++ )
	{
		switch( column )
		{
			case STATUS_CSV_COLUMN_ENUM_VALUE:
				Mem_Set( buf, 0, STATUS_CW_LEN );
				STATUS_SNPRINTF( buf, STATUS_CW_LEN, "%d", status_ds_list[row].ec );
				if ( '\0' != buf[STATUS_CW_LEN - 1] ) 
				{	buf[STATUS_CW_LEN - 1] = '\0';		goto Status_CSV_Column_Width_Error;	}
				string_pointer = buf;
				break;
			case STATUS_CSV_COLUMN_ENUM_SYMBOL:
				string_pointer = status_ds_list[row].ec_name;
				break;
			case STATUS_CSV_COLUMN_MESSAGE:
				string_pointer = status_ds_list[row].name;
				break;
		case STATUS_CSV_COLUMN_MODULE_NAME:
			{
				size_t id = EC_GET_ID( status_ds_list[row].ec );
				string_pointer = Module_Name( id );
			}
			break;
		default:
			string_pointer = NULL;
			break;
		}
		if ( NULL != string_pointer )
		{
			width = String_Length( string_pointer );
			if ( width > width_max ) width_max = width;
		}
	}
	return width_max;
Status_CSV_Column_Width_Error:
#if defined( STATUS_DEBUG_LOG )
	Debug_Log( STATUS_DEBUG_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */
