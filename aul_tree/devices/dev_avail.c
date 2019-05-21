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

#include "dev_avail.h"
#include "../shared/status.h"
#include "dev_avail_module.h"   /* all devices available to build list below */
#include "../shared/compiler.h"

/*#include "../shared/typebase.h" */	 /* uint8_t, uint16_t, etc */
/*#include "../shared/sysval.h"   */

/*********************************************************************************/
#define DEVICE_AVAILABLE_MODULE_REV  "$Date: 2014-06-10 15:26:22 -0500 (Tue, 10 Jun 2014) $"

/*********************************************************************************/
/* if DEBUG then allow MODULE_DBG.  Note: MODULE_DEBUG conflicts with debug.h */
#define DEVICE_AVAILABLE_DBG		   _DEBUG
/*********************************************************************************/
/*#define DEVICE_AVAILABLE_LOG   DEBUG_OUT*/
#define DEVICE_AVAILABLE_LOG   DEBUG_FILE

/*********************************************************************************/
/* function name replacement for OS compatibility. 
	TODO: merge with other files that have same thing. 
*/
#if defined( _WIN32 )
# include <stdarg.h>						/* variable argument list */
# include <stdio.h>
# ifndef DEVICE_AVAIL_SNPRINTF
#  define DEVICE_AVAIL_SNPRINTF    _snprintf
# endif
#else
# ifndef DEVICE_AVAIL_SNPRINTF
#  ifdef COMPILER_APPLICATION
#   pragma message ( "aul_tree: dev_avail.c  COMPILER_APPLICATION" )
#   include <stdio.h>
#   include <string.h>   /* strerror() */
#  else
/*# include <linux/kernel.h> */
#   pragma message ( "aul_tree: dev_avail.c __KERNEL__" )
#  endif
#  define DEVICE_AVAIL_SNPRINTF    snprintf
#  pragma message ( "aul_tree: dev_avail.c  DEVICE_AVAIL_SNPRINTF=snprintf" )
# endif
#endif

/*******************************************************************/
struct device_available
{
	SYS_TYPE						type;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	const char *					symbol_name;
#endif
	device_definition_type *		definition;
};
typedef struct device_available device_available_type;

/******************************************************************
Description
List of all available defined devices.  This does not define all of
the instances of devices, just the baseline types. 

NOTE:  Device_available_list[index].definition->io_definition.physical_id 
			is actually a logical_id.  It does not contain minor_number.
*/
device_available_type device_available_list[] =
{
#if defined( DEVICES_AVAILABLE_MODULE )
	DEVICE_AVAIL_MODULE_DEFINITIONS(DEVICE_AVAIL_EXTRACT_DEVICE_DEFINITIONS)
#endif
	/* null terminated list -- null entry in the list */
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	{ SYS_TYPE_OF(NONE), "", NULL }
#else
	{ SYS_TYPE_OF(NONE), NULL }
#endif
};
/*******************************************************************/
int Device_Available_Find( IOC_T logical_id, device_definition_type ** definition_ptr_out )
{
	size_t index;
	IOC_T  logical_id_from_list;
	index = 0;
	/* make sure that we just have the logical_id, not a physical_id */
	logical_id = IOC_GET_LOGICAL_ID_DEVICE( logical_id );
	if ( IOC_INVALID_LOGICAL_ID_DEVICE( logical_id ) ) return -1;
	while ( NULL != device_available_list[index].definition )
	{
		logical_id_from_list = IOC_GET_LOGICAL_ID_DEVICE( device_available_list[index].definition->io_definition.physical_id ); 
		if ( logical_id == logical_id_from_list ) 
		{
			*definition_ptr_out = device_available_list[index].definition;
			return SUCCESS;
		}
		index++;
	}
	*definition_ptr_out = NULL;
	return -1;  /*TODO: ??*/
}
/*********************************************************************************/
size_t Device_Available_All_Count( void )
{
	size_t count;
	size_t index;
	count = 0;
	index = 0;
	while ( NULL != device_available_list[index].definition ) 
	{
		count++;		index++;
	}
	return count;
}
/*******************************************************************/
#include "../str_safe/str_safe.h"
/*******************************************************************/
DEVICE_LOGICAL_ENUM Device_Available_Find_Logical_Type_Using_Logical_Name( const char * logical_name )
{
	size_t				index;
	const char *		name;  /* short hand */
	DEVICE_LOGICAL_ENUM logical_id;
	index = 0;
	do
	{	name = device_available_list[index].definition->name;
		if ( String_Compare( name, logical_name, String_Length( name ) ) == 0 )
		{	
			logical_id = (DEVICE_LOGICAL_ENUM) device_available_list[index].definition->io_definition.physical_id;
			return logical_id;	
		}
		index++;
	} while ( NULL != device_available_list[index].definition );
	return DEVICE_LOGICAL_UNDEFINED;
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
/*********************************************************************************/
/*   REPORTING FUNCTIONS   */
#include "../shared/mem.h"
#include <stdio.h>
/*********************************************************************************/
static const char * device_available_csv_column_names[] =
{
	/*	DEVICE_AVAILABLE_CSV_COLUMN_NONE			*/	"",
	/*	DEVICE_AVAILABLE_CSV_COLUMN_ENUM_VALUE		*/	"VALUE",
	/*	DEVICE_AVAILABLE_CSV_COLUMN_ENUM_SYMBOL		*/	"SYMBOL",
	/*	DEVICE_AVAILABLE_CSV_COLUMN_MODULE_NAME		*/	"MODULE_NAME",
	/*	DEVICE_AVAILABLE_CSV_COLUMN_DEVICE_ACRONYM	*/	"DEVICE_ACRONYM",
	/*	DEVICE_AVAILABLE_CSV_COLUMN_UNKNOWN			*/	NULL
};
/*********************************************************************************/
int Device_Available_CSV_Column_Name( DEVICE_AVAILABLE_CSV_COLUMN_ENUM column, char * buf_io, size_t buf_size )
{
	const char *	string_pointer;
	size_t			string_length;

	if ( ( column >= DEVICE_AVAILABLE_CSV_COLUMN_UNKNOWN )		|| 
		  ( column <  DEVICE_AVAILABLE_CSV_COLUMN_ENUM_VALUE   )  )	goto Device_Available_CSV_Column_Name_Error;
	/* initialize */
	string_pointer = device_available_csv_column_names[column];
	String_Copy( buf_io, string_pointer, buf_size );
	string_length = String_Length( string_pointer ) + 1 /* null */;
	if ( string_length >= buf_size ) goto Device_Available_CSV_Column_Name_Error;
	return SUCCESS;
Device_Available_CSV_Column_Name_Error:
#if defined( DEVICE_AVAILABLE_LOG )
	Debug_Log( DEVICE_AVAILABLE_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
int Device_Available_CSV_Cell_Get( size_t row, DEVICE_AVAILABLE_CSV_COLUMN_ENUM column, char * cell_buf_io, size_t cell_buf_size )
{
	const char *	string_pointer;
	size_t			string_length;
	size_t status_count = Status_All_Count();

	if ( row >= status_count )								goto Device_Available_CSV_Cell_Get_Error;

	if ( ( column >= DEVICE_AVAILABLE_CSV_COLUMN_UNKNOWN )	 || 
		  ( column <  DEVICE_AVAILABLE_CSV_COLUMN_ENUM_VALUE  )  )	goto Device_Available_CSV_Cell_Get_Error;

	/* initialize */
	Mem_Set( cell_buf_io, '\0', cell_buf_size );
	/*String_Copy( cell_buf_io, "", cell_buf_size );*/
	string_length  = 0;
	string_pointer = NULL;
	switch( column )
	{
		case DEVICE_AVAILABLE_CSV_COLUMN_ENUM_VALUE:
/* TODO: */
			DEVICE_AVAIL_SNPRINTF( cell_buf_io, cell_buf_size, "%d", device_available_list[row].definition->io_definition.physical_id );
			if ( '\0' != cell_buf_io[cell_buf_size - 1] ) 
			{	cell_buf_io[cell_buf_size - 1] = '\0';		goto Device_Available_CSV_Cell_Get_Error;	}
			return SUCCESS;
			break;
		case DEVICE_AVAILABLE_CSV_COLUMN_ENUM_SYMBOL:
			string_pointer = device_available_list[row].symbol_name;
			break;
		case DEVICE_AVAILABLE_CSV_COLUMN_MODULE_NAME:
			string_pointer = device_available_list[row].definition->name;
			break;
		case DEVICE_AVAILABLE_CSV_COLUMN_DEVICE_ACRONYM:
			string_pointer = device_available_list[row].definition->io_definition.acronym;
			break;
		default:
			string_pointer = NULL;
			break;
	}
	if ( NULL == string_pointer ) return SUCCESS;
	string_length += String_Length( string_pointer ) + 1 /* null */;
	String_Concatenate( cell_buf_io, string_pointer, cell_buf_size );
	if ( string_length >= cell_buf_size ) goto Device_Available_CSV_Cell_Get_Error;
	return SUCCESS;
Device_Available_CSV_Cell_Get_Error:
#if defined( DEVICE_AVAILABLE_LOG )
	Debug_Log( DEVICE_AVAILABLE_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
int Device_Available_CSV_Column_Width( DEVICE_AVAILABLE_CSV_COLUMN_ENUM column )
{
	const char *	string_pointer;
	size_t			width, width_max;
	size_t			row_count;
	size_t			row;
#define DEVICE_AVAIL_CW_LEN  32
	char           buf[DEVICE_AVAIL_CW_LEN];

	if ( ( column >= DEVICE_AVAILABLE_CSV_COLUMN_UNKNOWN )	 || 
		  ( column <  DEVICE_AVAILABLE_CSV_COLUMN_ENUM_VALUE  )  )	goto Device_Available_CSV_Column_Width_Error;

	row_count = Device_Available_All_Count();
	width_max = 0;
	for ( row = 0; row < row_count; row++ )
	{
		switch( column )
		{
		case DEVICE_AVAILABLE_CSV_COLUMN_ENUM_VALUE:
			Mem_Set( buf, 0, DEVICE_AVAIL_CW_LEN );
			DEVICE_AVAIL_SNPRINTF( buf, DEVICE_AVAIL_CW_LEN, "%d", device_available_list[row].definition->io_definition.physical_id );
			if ( '\0' != buf[DEVICE_AVAIL_CW_LEN - 1] ) 
			{	buf[DEVICE_AVAIL_CW_LEN - 1] = '\0';		goto Device_Available_CSV_Column_Width_Error;	}
			string_pointer = buf;
			break;
		case DEVICE_AVAILABLE_CSV_COLUMN_ENUM_SYMBOL:
			string_pointer = device_available_list[row].symbol_name;
			break;
		case DEVICE_AVAILABLE_CSV_COLUMN_MODULE_NAME:
			string_pointer = device_available_list[row].definition->name;
			break;
		case DEVICE_AVAILABLE_CSV_COLUMN_DEVICE_ACRONYM:
			string_pointer = device_available_list[row].definition->io_definition.acronym;
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
Device_Available_CSV_Column_Width_Error:
#if defined( DEVICE_AVAILABLE_LOG )
	Debug_Log( DEVICE_AVAILABLE_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */

