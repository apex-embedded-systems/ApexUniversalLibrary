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

1. Do we really need module private_data????

*********************************************************************************/

#include "module.h"
#include "module_macros.h"
#include "../io/io.h"
#include "../shared/typebool.h"
//#include "../components/indirect/cmp_indirect.h"

/* #include "../shared/cmd_macros.h"*/
/*******************************************************************/
#define MODULE_MODULE_REV  "$Date: 2016-09-30 14:48:43 -0500 (Fri, 30 Sep 2016) $"

/*********************************************************************************/
/* if DEBUG then allow MODULE_DBG.  Note: MODULE_DEBUG conflicts with debug.h */
#define MODULE_DBG		   _DEBUG
/*********************************************************************************/
#define MODULE_DEBUG_LOG   DEBUG_OUT

/*********************************************************************************/
module_definition_type module_module_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	NULL,
	NULL,
	MODULE_MODULE_NAME,
	MODULE_MODULE_REV,
	NULL,
	NULL
};
/*********************************************************************************/
struct module_dataset_v2
{
	const size_t			id;				/* zero (0) reserved for end of list. */
	const size_t			module_public;
	const char *         	id_as_string;
	/* observe the first entry within definition to know what it is.
	   First entry is always a SYS_TYPE.
	*/
	void *					definition;	
};
/*********************************************************************************/

/* 2014SEP30:  DYNAMIC LINKING OF COMPONENTS:
 *   1. We will need APIs to add to the existing list below.
 *   2. Seems like we need some form of list manager to add structures to this list?
 *
 */


static const struct module_dataset_v2 module_list_v2[]=
{
/* 2  */
#if defined( MODULE_STATUS )
	{ MODULE_STATUS, MODULE_PUBLIC, MODULE_STR( MODULE_STATUS ), (void *) &module_status_definition },
#endif
/* 6  */
#if defined( MODULE_WARN )
	{ MODULE_WARN, MODULE_PUBLIC, MODULE_STR( MODULE_WARN ), (void *) &module_warn_definition },
#endif
/* 3  */
#if defined( MODULE_DEBUG )
	{ MODULE_DEBUG, MODULE_PUBLIC, MODULE_STR( MODULE_DEBUG ),(void *) &module_debug_definition },
#endif
/* 4  */
#if defined( MODULE_MEM )
	{ MODULE_MEM, MODULE_PUBLIC, MODULE_STR( MODULE_MEM ),(void *) &module_mem_definition },
#endif
/* 5  */
#if defined( MODULE_ERROR )
	{ MODULE_ERROR, MODULE_PUBLIC, MODULE_STR( MODULE_ERROR ),(void *) &module_error_definition },
#endif
/* 1  */
#if defined( MODULE_MODULE )
	{ MODULE_MODULE, MODULE_PUBLIC, MODULE_STR( MODULE_MODULE ),(void *) &module_module_definition },
#endif
/* 8  */
#if defined( MODULE_IO )
	{ MODULE_IO, MODULE_PUBLIC, MODULE_STR( MODULE_IO ),(void *) &module_io_definition },
#endif
/* 9  */
#if defined( MODULE_REG )
	{ MODULE_REG, MODULE_PUBLIC, MODULE_STR( MODULE_REG ),(void *) &module_reg_definition },
#endif
/* 10  */
#if defined( MODULE_ARRAY )
	{ MODULE_ARRAY, MODULE_PUBLIC, MODULE_STR( MODULE_ARRAY ),(void *) &module_array_definition },
#endif
/* 11 - NOTE: Component manager takes care of initialize() or terminate() of all components.
            Therefore, module manager does not have to do it.  Why?                        */
#if defined( MODULE_CMP_MGR )
	{ MODULE_CMP_MGR, MODULE_PUBLIC, MODULE_STR( MODULE_CMP_MGR ),(void *) &module_cmpmgr_definition },
#endif
/* 12 - NOTE: Component manager takes care of initialize() or terminate() of all components.
            Therefore, module manager does not have to do it.  Why?                       */
#if defined( MODULE_DEV_MGR )
	{ MODULE_DEV_MGR, MODULE_PUBLIC, MODULE_STR( MODULE_DEV_MGR ),(void *) &module_devmgr_definition },
#endif
/* 13 - NOTE: AUL Library interface   */
#if defined( MODULE_AUL )
	{ MODULE_AUL, MODULE_PUBLIC, MODULE_STR( MODULE_AUL ),(void *) &module_aul_definition },
#endif
/* 14 - AUL Hardware definition file import  */
#if defined( MODULE_AULHWDEF )
	{ MODULE_AULHWDEF, MODULE_PUBLIC, MODULE_STR( MODULE_AULHWDEF ),(void *) &module_aulhwdef_definition },
#endif
/* 15 - Auto code generation and reporting   */
#if defined( MODULE_ACGR )
	{ MODULE_ACGR, MODULE_PUBLIC, MODULE_STR( MODULE_ACGR ),(void *) &module_acgr_definition },
#endif
/* 29 - Report   */
#if defined( MODULE_REPORT )
	{ MODULE_REPORT, MODULE_PUBLIC, MODULE_STR( MODULE_REPORT ),(void *) &module_report_definition },
#endif

/* predefined components in aggregate    */
#if defined( COMPONENTS_AVAILABLE_MODULE )
	COMPONENT_AVAIL_MODULES_V2
#endif
/* devices in aggregate     */
#if defined( DEVICES_AVAILABLE_MODULE )
	DEVICE_AVAIL_MODULE_DEFINITIONS(DEVICE_AVAIL_EXTRACT_MODULE_INFO)
#endif

/* DYNAMIC components instantiated here. */


/* Null terminator      */
	{ 0, 0, NULL, NULL },
};
/*********************************************************************************/
size_t Module_All_Count( void )
{
	size_t count;
	size_t index;
	count = 0;
	index = 0;
	while ( 0 != module_list_v2[index].id ) 
	{
		count++;		index++;
	}
	return count;
}
/*********************************************************************************/
size_t Module_Public_Count( void )
{
	size_t count;
	size_t index;
	count = 0;
	index = 0;
	while ( 0 != module_list_v2[index].id ) 
	{
		if ( module_list_v2[count].module_public ) count++;		
		index++;
	}
	return count;
}
/*********************************************************************************/
/*********************************************************************************/
typedef enum 
{ 
	MODULE_TYPE_NONE		= 0, /**< end marker  */
	MODULE_TYPE_MODULE    	= 1,
	MODULE_TYPE_DEVICE	 	= 2,
	MODULE_TYPE_COMPONENT 	= 3,
	MODULE_TYPE_QTY       	= 4  /**< end marker  */
} MODULE_TYPE_ENUM;
/*********************************************************************************/
#define MODULE_BUF_SIZE 10
static char module_buf[MODULE_BUF_SIZE];
/*********************************************************************************/
static const char * Module_String_Time_Stamp_Convert( const char * str_in )
{
	char *  str;
/* TODO  TSSVN_Time_Stamp_YYMMDDHH_String( char * str_out, const char * str_in, size_t str_out_size );    */
	if ( NULL == str_in ) return str_in;
	str = (char *) str_in;
	/* year */
	module_buf[0] = str[ 9];
	module_buf[1] = str[10];
	/* month */
	module_buf[2] = str[12];
	module_buf[3] = str[13];
	/* day */
	module_buf[4] = str[15];
	module_buf[5] = str[16];
	/* hour */
	module_buf[6] = str[18];
	module_buf[7] = str[19];
	module_buf[8] = '\0';

	return (const char *) module_buf;
}
/*********************************************************************************/
static const char * Module_String_Module_Get( size_t row, MODULE_CSV_COLUMN_ENUM col )
{
	module_definition_type * md = (module_definition_type *) ( module_list_v2[row].definition );

	switch( col )
	{
		case MODULE_CSV_COLUMN_NAME:				return md->name;
		case MODULE_CSV_COLUMN_TYPE:				return "MODULE";
		case MODULE_CSV_COLUMN_MODULE_TIME_STAMP:	return Module_String_Time_Stamp_Convert( md->svn_module_date );
		case MODULE_CSV_COLUMN_COMMAND_TIME_STAMP:	return Module_String_Time_Stamp_Convert( md->svn_command_date );
		case MODULE_CSV_COLUMN_STATUS_TIME_STAMP:	return Module_String_Time_Stamp_Convert( md->svn_status_date );
		default:
			break;
	}
	return NULL;
}
/*********************************************************************************/
static const char * Module_String_Device_Get( size_t row, MODULE_CSV_COLUMN_ENUM col )
{
	device_definition_type * dd = (device_definition_type *) ( module_list_v2[row].definition );

	switch( col )
	{
		case MODULE_CSV_COLUMN_NAME:				return dd->name;
		case MODULE_CSV_COLUMN_TYPE:				return "DEVICE";
		case MODULE_CSV_COLUMN_MODULE_TIME_STAMP:	return Module_String_Time_Stamp_Convert( dd->svn_module_date );
		case MODULE_CSV_COLUMN_COMMAND_TIME_STAMP:	return Module_String_Time_Stamp_Convert( dd->svn_command_date );
		case MODULE_CSV_COLUMN_STATUS_TIME_STAMP:	return Module_String_Time_Stamp_Convert( dd->svn_status_date );
		default:
			break;
	}
	return NULL;
}
/*********************************************************************************/
static const char * Module_String_Component_Get( size_t row, MODULE_CSV_COLUMN_ENUM col )
{
	component_definition_type * cd = (component_definition_type *) ( module_list_v2[row].definition );

	switch( col )
	{
		case MODULE_CSV_COLUMN_NAME:				return cd->name;
		case MODULE_CSV_COLUMN_TYPE:				return "COMPONENT";
		case MODULE_CSV_COLUMN_MODULE_TIME_STAMP:	return Module_String_Time_Stamp_Convert( cd->svn_module_date );
		case MODULE_CSV_COLUMN_COMMAND_TIME_STAMP:	return Module_String_Time_Stamp_Convert( cd->svn_command_date );
		case MODULE_CSV_COLUMN_STATUS_TIME_STAMP:	return Module_String_Time_Stamp_Convert( cd->svn_status_date );
		default:
			break;
	}
	return NULL;
}
/*********************************************************************************/
static MODULE_TYPE_ENUM Module_Type_Identify( size_t index )
{
	MODULE_TYPE_ENUM  type = MODULE_TYPE_NONE;
	SYS_TYPE * struct_type = (SYS_TYPE *)( module_list_v2[index].definition );

	switch( TYPE_GET_BASE_TYPE( *struct_type ) )
	{
		case SYS_TYPE__module_definition_type:
			type = MODULE_TYPE_MODULE;
			break;
		case SYS_TYPE__device_definition_type:
			type = MODULE_TYPE_DEVICE;
			break;
		case SYS_TYPE__component_definition_type:
			type = MODULE_TYPE_COMPONENT;
			break;
		default:
			type = MODULE_TYPE_NONE;
			break;
	}
	return type;
}
/*********************************************************************************/
static const char * Module_String_List_Cell_Get( size_t row, MODULE_CSV_COLUMN_ENUM column )
{
	MODULE_TYPE_ENUM  type = Module_Type_Identify( row );

	if ( MODULE_CSV_COLUMN_ID == column ) return module_list_v2[row].id_as_string;

	switch( type )
	{
		case MODULE_TYPE_MODULE:	 return Module_String_Module_Get( row, column );
		case MODULE_TYPE_DEVICE:	 return Module_String_Device_Get( row, column );
		case MODULE_TYPE_COMPONENT: return Module_String_Component_Get( row, column );
		default:
			break;
	}
	return NULL;
}
/*********************************************************************************/
static const char * Module_String_List_Name_Get( int index )
{
	return Module_String_List_Cell_Get( index, MODULE_CSV_COLUMN_NAME );
}
/*********************************************************************************/
const char * Module_Name( int module_id )
{
	size_t	index;

	index	= 0;
	while ( 0 != module_list_v2[index].id )
	{
		if ( ((size_t) module_id) == module_list_v2[index].id )
		{
			return Module_String_List_Cell_Get( index, MODULE_CSV_COLUMN_NAME );
		}
		index++;
	}
	return NULL;
}
/*********************************************************************************/
/*********************************************************************************/
static int Module_List_Initialize( size_t index )
{
	int error_code;
	SYS_TYPE * type = (SYS_TYPE *)( module_list_v2[index].definition );
	error_code = SUCCESS; /* If DEVICE or COMPONENT */
	switch( TYPE_GET_BASE_TYPE( *type ) )
	{
		case SYS_TYPE__module_definition_type:
			{
				module_definition_type * md = (module_definition_type *) ( module_list_v2[index].definition );
				if ( NULL != md->initialize )
				{
					error_code = ( * md->initialize )();
				}
				else
				{
					error_code = SUCCESS;
				}
			}
			break;
		case SYS_TYPE__device_definition_type:
			{
				device_definition_type * dev = (device_definition_type *) ( module_list_v2[index].definition );
				if ( NULL != dev->initialize )
				{
					error_code = ( * dev->initialize )();
				}
				else
				{
					error_code = SUCCESS;
				}
			}
			break;
		default:break;
			/* We do not initialize/terminate devices or components.  That is handled
				on demand.
			*/
	}
	return error_code;
}
/*********************************************************************************/
static int Module_List_Terminate( size_t index )
{
	int error_code;
	SYS_TYPE * type = (SYS_TYPE *)( module_list_v2[index].definition );
	error_code = SUCCESS; /* If DEVICE or COMPONENT */
	switch( TYPE_GET_BASE_TYPE( *type ) )
	{
		case SYS_TYPE__device_definition_type:
			{
				device_definition_type * dev = (device_definition_type *) ( module_list_v2[index].definition );
				if ( NULL != dev->terminate )
				{
					error_code = ( * dev->terminate )();
				}
				else
				{
					error_code = SUCCESS;
				}
			}
			break;
		case SYS_TYPE__module_definition_type:
			{
				module_definition_type * md = (module_definition_type *) ( module_list_v2[index].definition );
				if ( NULL != md->terminate )
				{
					error_code = ( * md->terminate )();
				}
				else
				{
					error_code = SUCCESS;
				}
			}
			break;
		default:break;
			/* We do not initialize/terminate devices or components.  That is handled
				on demand.
			*/
	}
	return error_code;
}
/*********************************************************************************/
int Module_Initialize( void )
{
	int		error_code;
	size_t	index, iy;
#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	error_code	= SUCCESS;
#ifdef MODULE_DBG
	/* determine if there are any module ID duplicates */
	index = 0;
	while ( 0 != module_list_v2[index].id )
	{
		iy = 0;
		while ( 0 != module_list_v2[iy].id )
		{
			if ( iy != index )
			{
				if ( module_list_v2[index].id == module_list_v2[iy].id )
				{
#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG " ERROR: module duplicates at:\n" );
	Debug_Log( MODULE_DEBUG_LOG "        0x%08X %s and 0x%08X %s\n", 
							module_list_v2[index].id, Module_String_List_Name_Get( index ),
							module_list_v2[iy].id,    Module_String_List_Name_Get( iy )
				);
#endif
					error_code = -1;		goto Module_Initialize_Error;
				}
			}
			iy++;
		}
		index++;
	}
#endif

	/* initialize MODULES */
#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG " INIT MODULES:\n" );
#endif
	index	= 0;
	while ( 0 != module_list_v2[index].id )
	{
		error_code = Module_List_Initialize( index );
#if defined( MODULE_DEBUG_LOG )
		Debug_Log( MODULE_DEBUG_LOG "  --> %-16s ec=%d\n", Module_String_List_Name_Get( index ), error_code );
#endif
		if ( error_code ) goto Module_Initialize_Error;
		index++;
	}

#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG " Module list:\n" );
	index = 0;
	while ( 0 != module_list_v2[index].id )
	{
		Debug_Log( MODULE_DEBUG_LOG " %2d) 0x%08X %s\n", index, module_list_v2[index].id, Module_String_List_Name_Get( index ) );
		index++;
	}
#endif

#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
Module_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG "%s: exit failed\n", __func__ );
#endif
	return -error_code;
}
/*********************************************************************************/
int Module_Terminate( void )
{
	int		error_code;
	int		index;

	error_code	= SUCCESS;
	/* terminate MODULES */
#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG " TERMINATE MODULES:\n" );
#endif
	index	= ( (int) Module_All_Count() ) - 1;
	while ( index >= 0 )
	{
		error_code = Module_List_Terminate( index );
#if defined( MODULE_DEBUG_LOG )
		Debug_Log( MODULE_DEBUG_LOG "  --> %-16s ec=%d\n", Module_String_List_Name_Get( index ), error_code );
#endif
		if ( error_code ) goto Module_Terminate_Error;
		index--;
	}
#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG "\n" );
#endif
Module_Terminate_Error:
#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return error_code;
}

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
/*********************************************************************************/
/*   REPORTING FUNCTIONS   */

#include "../str_safe/str_safe.h"
/*********************************************************************************/
static const char * module_csv_column_names[] =
{
	/*	MODULE_CSV_COLUMN_NONE					*/		"",
	/*	MODULE_CSV_COLUMN_NAME					*/		"NAME",
	/*	MODULE_CSV_COLUMN_ID					*/		"ID",
	/*	MODULE_CSV_COLUMN_TYPE					*/		"TYPE",
	/*	MODULE_CSV_COLUMN_MODULE_TIME_STAMP		*/		"MODULE_TS",
	/*	MODULE_CSV_COLUMN_COMMAND_TIME_STAMP	*/		"COMMAND_TS",
	/*	MODULE_CSV_COLUMN_STATUS_TIME_STAMP		*/		"STATUS_TS",
														NULL
};
/*********************************************************************************/
int Module_CSV_Column_Name( MODULE_CSV_COLUMN_ENUM column, char * buf_io, size_t buf_size )
{
	const char *	string_pointer;
	size_t			string_length;

	if ( ( column >= MODULE_CSV_COLUMN_UNKNOWN ) || 
		  ( column <  MODULE_CSV_COLUMN_NAME    )  )	goto Module_CSV_Column_Name_Error;

	/* initialize */
	string_pointer = module_csv_column_names[column];
	String_Copy( buf_io, string_pointer, buf_size );
	string_length = String_Length( string_pointer ) + 1 /* null */;
	if ( string_length >= buf_size ) goto Module_CSV_Column_Name_Error;
	return SUCCESS;
Module_CSV_Column_Name_Error:
#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
int Module_CSV_Cell_Get( size_t row, MODULE_CSV_COLUMN_ENUM column, char * cell_buf_io, size_t cell_buf_size )
{
	const char *	string_pointer;
	size_t			string_length;
	size_t module_count = Module_All_Count();

	if ( row >= module_count )							goto Module_CSV_Cell_Get_Error;

	if ( ( column >= MODULE_CSV_COLUMN_UNKNOWN ) || 
		  ( column <  MODULE_CSV_COLUMN_NAME    )  )	goto Module_CSV_Cell_Get_Error;

	/* if we wish to not make a module public */
	if ( TRUE != module_list_v2[row].module_public ) goto Module_CSV_Cell_Get_Error;

	/* initialize */
	String_Copy( cell_buf_io, "", cell_buf_size );
	string_length = 0;
	string_pointer = Module_String_List_Cell_Get( row, column );
	if ( NULL == string_pointer ) return SUCCESS;
	string_length += String_Length( string_pointer ) + 1 /* null */;
	String_Concatenate( cell_buf_io, string_pointer, cell_buf_size );
	if ( string_length >= cell_buf_size ) goto Module_CSV_Cell_Get_Error;
	return SUCCESS;
Module_CSV_Cell_Get_Error:
#if defined( MODULE_DEBUG_LOG )
	Debug_Log( MODULE_DEBUG_LOG "%s: exit failed\n\n", __func__ );
#endif
	return -1;
}
/*********************************************************************************/
int Module_ID_Row_Get( size_t module_id )
{
	size_t	index;
	index	= 0;
	while ( 0 != module_list_v2[index].id )
	{
		if ( module_id == module_list_v2[index].id )
		{
			return index;
		}
		index++;
	}
	return -1;
}
/*********************************************************************************/
#endif
