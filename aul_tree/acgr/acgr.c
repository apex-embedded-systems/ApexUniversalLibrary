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
#include <stdio.h>
#include <stdlib.h>

/*#include "../shared/typebase.h" */  /* uint8_t, uint16_t, etc */

#include "acgr.h"
#include "../shared/status.h"
#include "../shared/typebool.h"
#include "../str_safe/str_safe.h"
#include "../shared/module.h"
#include "../shared/module_shared.h"
#include "../io/io.h"
/*#include "../shared/cmd_macros.h"*/

/* NOTE:  ACGR = Auto Code Generation and Reporting */

/**********************************************************************************************//**
 * A macro that defines acgr module revision, auto-generated.
 *
 *
 **************************************************************************************************/

#define ACGR_MODULE_REV  "$Date: 2015-02-03 11:36:30 -0600 (Tue, 03 Feb 2015) $"

/*********************************************************************************/

/**********************************************************************************************//**
 * \brief
 * A macro that defines acgr debug.
 *
 *
 * \param _DEBUG   This is a compile time macro.
 **************************************************************************************************/

#define ACGR_DBG		   _DEBUG
/*********************************************************************************/
#define ACGR_DEBUG_LOG   DEBUG_OUT

/**********************************************************************************************//**
 * TODO: public versus private modules.
 *
 * \author	Mike
 * \date	3/30/2014
 **************************************************************************************************/

#define ACGR_BUF_SIZE 128 

/**********************************************************************************************//**
 * A macro that defines acgr prepend timestamp.
 *
 *
 **************************************************************************************************/
#define ACGR_PREPEND_TIMESTAMP 

#if ( defined( ACGR_PREPEND_TIMESTAMP ) && ( TARGET_AUTO_CODE_GEN_REPORTS == 1 ) )
//# ifdef DEBUG_PREPEND_TIMESTAMP
#  include <time.h>
#  include <stdarg.h>
//# endif
/**********************************************************************************************//**
 * Acgr date time.
 *
 *
 * \param [in,out]	out	If non-null, the out.
 * \param	file_name		Filename of the file.
 * \param	comment			true to comment.
 *
 **************************************************************************************************/
static void ACGR_Date_Time( FILE * out, const char * file_name, size_t comment_include )
{
	struct tm * time_block;
	time_t		time_current;
	char *      time_ascii;
	size_t		index;
	int			row;
	char		buf[ACGR_BUF_SIZE];

	time_current = time( NULL );
/*TODO: replace localtime() with a reentrant version */
	time_block   = localtime( &time_current );
/*
TODO: replace asctime() with a reentrant version thereof  asctime_r() is a typical name.
TODO: don't like the format!
*/
	time_ascii   = asctime( time_block );
	index = 0;
	while( time_ascii[index] != '\n' ){ index++; }
	time_ascii[index] = '\0';
	
	if ( comment_include )
	{
		fprintf( out, "/* Auto Code Generation - AUL library" );
		row = Module_ID_Row_Get( MODULE_ACGR );
		if ( row >= 0 )
		{
			Mem_Set( buf, 0, ACGR_BUF_SIZE );
			Module_CSV_Cell_Get( row, MODULE_CSV_COLUMN_MODULE_TIME_STAMP, buf, ACGR_BUF_SIZE );
			fprintf( out, " ACGR version %s", buf );
		}
		fprintf( out, ".    */\n" );
		fprintf( out, "/* File: %s    */\n", file_name );
		fprintf( out, "/* Date: %s    */\n", time_ascii );
	}
}
#endif
/**********************************************************************************************//**
 * The acgr CSV module columns[].
 **************************************************************************************************/
static const MODULE_CSV_COLUMN_ENUM acgr_csv_module_columns[] =
{
	MODULE_CSV_COLUMN_NAME,
	MODULE_CSV_COLUMN_ID, 
	MODULE_CSV_COLUMN_TYPE,
	MODULE_CSV_COLUMN_MODULE_TIME_STAMP,
	MODULE_CSV_COLUMN_COMMAND_TIME_STAMP,
	MODULE_CSV_COLUMN_STATUS_TIME_STAMP,
};
/**********************************************************************************************//**
 * Acgr report CSV module.
 *
 *
 * \param	column_names_include	true to column names.
 * \param	file_name		Filename of the file.
 *
 * \return	An AULCALLCONV.
 **************************************************************************************************/
AUL_DLL_API int AULCALLCONV ACGR_Report_CSV_Module( size_t column_names_include, const char * file_name )
{
	int							error_code;
	FILE *						out;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	MODULE_CSV_COLUMN_ENUM	col;
	size_t						index;
	size_t						row;
	size_t						row_max;
	size_t						col_max;
	char						buf[ACGR_BUF_SIZE];
#endif

	out = fopen( file_name, "wt" );
	if ( NULL == out ) 
	{	error_code = EC_ACGR_FILE_OPEN_ERROR;	goto ACGR_Report_CSV_Module_Error;	}

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
# ifdef ACGR_PREPEND_TIMESTAMP
	ACGR_Date_Time( out, file_name, FALSE );
# endif
	row_max = Module_All_Count();
	col_max = sizeof( acgr_csv_module_columns ) / sizeof ( MODULE_CSV_COLUMN_ENUM );

	/** column names */
	if ( column_names_include )
	{
		for ( index = 0; index < col_max; index++ )
		{
			col = acgr_csv_module_columns[index];
			error_code = Module_CSV_Column_Name( col, buf, ACGR_BUF_SIZE );
			if ( error_code ) 
			{	error_code = EC_ACGR_COLUMN_NAME;	goto ACGR_Report_CSV_Module_Error;	}
			if ( index ) fprintf( out, "," );
			fprintf( out, "\"%s\"", buf );			
		} 
		fprintf( out, "\n" );
	}
	/* output a row at a time */
	for ( row = 0; row < row_max; row++ )
	{ /* rows */
		for ( index = 0; index < col_max; index++ )
		{ /* columns */
			col = acgr_csv_module_columns[index];
			error_code = Module_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
			if ( error_code ) 
			{	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Report_CSV_Module_Error;	}
			if ( index ) fprintf( out, "," );
			fprintf( out, "\"%s\"", buf );
		} 
		fprintf( out, "\n" );
	}
	fclose( out );
	return SUCCESS;
#else  /* TARGET_AUTO_CODE_GEN_REPORTS */
	fprintf( out, "ACGR_Report_CSV_Module: unavailable.  TARGET_AUTO_CODE_GEN_REPORTS != 1\n" );
	error_code = EC_ACGR_REPORTS_UNAVAILABLE;	
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */
	ACGR_Report_CSV_Module_Error:
	if ( NULL != out ) fclose( out );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_ACGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************//**
 * The acgr CSV command columns[].
 **************************************************************************************************/
static const COMMAND_CSV_COLUMN_ENUM acgr_csv_command_columns[] =
{
	COMMAND_CSV_COLUMN_ENUM_SYMBOL,
	COMMAND_CSV_COLUMN_ENUM_VALUE, 
};
/**********************************************************************************************//**
 * Acgr report CSV command.
 *
 *
 * \param	column_names_include	true to column names.
 * \param	file_name		Filename of the file.
 *
 * \return	An AULCALLCONV.
 **************************************************************************************************/
AUL_DLL_API int AULCALLCONV ACGR_Report_CSV_Command( size_t column_names_include, const char * file_name )
{
	int							error_code;
	FILE *						out;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	COMMAND_CSV_COLUMN_ENUM	col;
	size_t						index;
	size_t						row;
	size_t						row_max;
	size_t						col_max;
	char						buf[ACGR_BUF_SIZE];
#endif

	out = fopen( file_name, "wt" );
	if ( NULL == out ) 
	{	error_code = EC_ACGR_FILE_OPEN_ERROR;	goto ACGR_Report_CSV_Commands_Error;	}

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
# ifdef ACGR_PREPEND_TIMESTAMP
	ACGR_Date_Time( out, file_name, FALSE );
# endif
	row_max = Command_All_Count();
	col_max = sizeof( acgr_csv_command_columns ) / sizeof ( COMMAND_CSV_COLUMN_ENUM );

	/* column names */
	if ( column_names_include )
	{
		for ( index = 0; index < col_max; index++ )
		{
			col = acgr_csv_command_columns[index];
			error_code = Command_CSV_Column_Name( col, buf, ACGR_BUF_SIZE );
			if ( error_code ) 
			{	error_code = EC_ACGR_COLUMN_NAME;	goto ACGR_Report_CSV_Commands_Error;	}
			if ( index ) fprintf( out, "," );
			fprintf( out, "\"%s\"", buf );			
		} 
		fprintf( out, "\n" );
	}
	/* output a row at a time */
	for ( row = 0; row < row_max; row++ )
	{ /* rows */
		for ( index = 0; index < col_max; index++ )
		{ /* columns */
			col = acgr_csv_command_columns[index];
			error_code = Command_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
			if ( error_code ) 
			{	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Report_CSV_Commands_Error;	}
			if ( index ) fprintf( out, "," );
			fprintf( out, "\"%s\"", buf );
		} 
		fprintf( out, "\n" );
	}
	fclose( out );
	return SUCCESS;
#else  /* TARGET_AUTO_CODE_GEN_REPORTS */
	fprintf( out, "ACGR_Report_CSV_Command: unavailable.  TARGET_AUTO_CODE_GEN_REPORTS != 1\n" ); 
	error_code = EC_ACGR_REPORTS_UNAVAILABLE;	
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */
ACGR_Report_CSV_Commands_Error:
	if ( NULL != out ) fclose( out );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_ACGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************//**
 * The acgr CSV status columns[].
 **************************************************************************************************/
static const STATUS_CSV_COLUMN_ENUM acgr_csv_status_columns[] =
{
	STATUS_CSV_COLUMN_ENUM_SYMBOL,
	STATUS_CSV_COLUMN_MODULE_NAME,
	STATUS_CSV_COLUMN_ENUM_VALUE, 
	STATUS_CSV_COLUMN_MESSAGE,
};
/**********************************************************************************************//**
 * Acgr report CSV status.
 *
 *
 * \param	column_names_include	true to column names.
 * \param	file_name		Filename of the file.
 *
 * \return	An AULCALLCONV.
 **************************************************************************************************/
AUL_DLL_API int AULCALLCONV ACGR_Report_CSV_Status( size_t column_names_include, const char * file_name )
{
	int							error_code;
	FILE *						out;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	STATUS_CSV_COLUMN_ENUM	col;
	size_t						index;
	size_t						row;
	size_t						row_max;
	size_t						col_max;
	char						buf[ACGR_BUF_SIZE];
#endif
	out = fopen( file_name, "wt" );
	if ( NULL == out ) 
	{	error_code = EC_ACGR_FILE_OPEN_ERROR;	goto ACGR_Report_CSV_Status_Error;	}

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
# ifdef ACGR_PREPEND_TIMESTAMP
	ACGR_Date_Time( out, file_name, FALSE );
# endif
	row_max = Status_All_Count();
	col_max = sizeof( acgr_csv_status_columns ) / sizeof ( STATUS_CSV_COLUMN_ENUM );

	/* column names */
	if ( column_names_include )
	{
		for ( index = 0; index < col_max; index++ )
		{
			col = acgr_csv_status_columns[index];
			error_code = Status_CSV_Column_Name( col, buf, ACGR_BUF_SIZE );
			if ( error_code ) 
			{	error_code = EC_ACGR_COLUMN_NAME;	goto ACGR_Report_CSV_Status_Error;	}
			if ( index ) fprintf( out, "," );
			fprintf( out, "\"%s\"", buf );			
		} 
		fprintf( out, "\n" );
	}
	/* output a row at a time */
	for ( row = 0; row < row_max; row++ )
	{ /* rows */
		for ( index = 0; index < col_max; index++ )
		{ /* columns */
			col = acgr_csv_status_columns[index];
			error_code = Status_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
			if ( error_code ) 
			{	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Report_CSV_Status_Error;	}
			if ( index ) fprintf( out, "," );
			fprintf( out, "\"%s\"", buf );
		} 
		fprintf( out, "\n" );
	}
	fclose( out );
	return SUCCESS;
#else  /* TARGET_AUTO_CODE_GEN_REPORTS */
	fprintf( out, "ACGR_Report_CSV_Status: unavailable.  TARGET_AUTO_CODE_GEN_REPORTS != 1\n" );
	error_code = EC_ACGR_REPORTS_UNAVAILABLE;	
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */
ACGR_Report_CSV_Status_Error:
	if ( NULL != out ) fclose( out );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_ACGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************//**
 * The acgr command enum prefix[].
 **************************************************************************************************/
static const char acgr_command_enum_prefix[] =
{
	"\n"
	"#ifndef AUL_COMMAND_ENUM_H_\n"
	"#define AUL_COMMAND_ENUM_H_\n"
	"\n"
	"enum aul_command\n"
	"{\n"
};
/**********************************************************************************************//**
 * The acgr command enum postfix[].
 **************************************************************************************************/
static const char acgr_command_enum_postfix[] =
{
	"};\n"
	"typedef enum aul_command AUL_COMMAND_ENUM;\n"
	"\n"
	"#endif  /* AUL_COMMAND_ENUM_H_*/\n"
	"\n"
};
/**********************************************************************************************//**
 * Acgr source file command enumeration.
 *
 *
 * \param	file_name	Filename of the file.
 *
 * \return	An AULCALLCONV.
 **************************************************************************************************/
AUL_DLL_API int AULCALLCONV ACGR_Source_File_Command_Enumeration( const char * file_name )
{
	int							error_code;
	FILE *						out;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	size_t						col_width;	
	size_t						row;
	size_t						row_max;
	char						format[16];
	char						buf[ACGR_BUF_SIZE];
#endif

	out = fopen( file_name, "wt" );
	if ( NULL == out ) 
	{	error_code = EC_ACGR_FILE_OPEN_ERROR;	goto ACGR_Source_Files_Command_Enumeration_Error;	}

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
# ifdef ACGR_PREPEND_TIMESTAMP
	ACGR_Date_Time( out, file_name, TRUE );
# endif
	fprintf( out, "%s", acgr_command_enum_prefix );
	/* build the output format string by finding the longest symbol string */
	col_width = Command_CSV_Column_Width( COMMAND_CSV_COLUMN_ENUM_SYMBOL );
	String_Copy( format, "     %-", 16 );
	row = String_Length( format ); /* row is used temporarily to get to end character */
	sprintf( &(format[row]), "%d", col_width );
	String_Concatenate( format, "s =", 16 );
	/* output all the rows */
	row_max = Command_All_Count();
	for ( row = 0; row < row_max; row++ )
	{ /* rows */
		error_code = Command_CSV_Cell_Get( row, COMMAND_CSV_COLUMN_ENUM_SYMBOL, buf, ACGR_BUF_SIZE );
		fprintf( out, format, buf );
		error_code = Command_CSV_Cell_Get( row, COMMAND_CSV_COLUMN_ENUM_VALUE, buf, ACGR_BUF_SIZE );
		fprintf( out, "     %10s,\n", buf );
	}
	fprintf( out, "%s", acgr_command_enum_postfix );
	fclose( out );
	return SUCCESS;
#else  /* TARGET_AUTO_CODE_GEN_REPORTS */
	fprintf( out, "ACGR_Source_File_Command_Enumeration: unavailable.  TARGET_AUTO_CODE_GEN_REPORTS != 1\n" );
	error_code = EC_ACGR_REPORTS_UNAVAILABLE;	
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */
ACGR_Source_Files_Command_Enumeration_Error:
	if ( NULL != out ) fclose( out );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_ACGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************//**
 * The acgr status enum prefix[].
 **************************************************************************************************/
static const char acgr_status_enum_prefix[] =
{
	"\n"
	"#ifndef AUL_STATUS_ENUM_H_\n"
	"#define AUL_STATUS_ENUM_H_\n"
	"\n"
	"#define AUL_STATUS_LAST_VALUE  %s"
	"\n"
	"enum { SUCCESS = 0 };\n"
	"\n"
	"enum aul_status\n"
	"{\n"
};
/**********************************************************************************************//**
 * The acgr status enum postfix[].
 **************************************************************************************************/
static const char acgr_status_enum_postfix[] =
{
	"};\n"
	"typedef enum aul_status AUL_STATUS_ENUM;\n"
	"\n"
	"#endif  /* AUL_STATUS_ENUM_H_*/\n"
	"\n"
};

/**********************************************************************************************//**
 * Acgr source file status enumeration.
 *
 *
 * \param	file_name	Filename of the file.
 *
 * \return	An AULCALLCONV.
 **************************************************************************************************/
AUL_DLL_API int AULCALLCONV ACGR_Source_File_Status_Enumeration( const char * file_name )
{
	int							error_code;
	FILE *						out;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	size_t						col_width;	
	size_t						row;
	size_t						row_max;
	char						format[16];
	char						buf[ACGR_BUF_SIZE];
#endif

	out = fopen( file_name, "wt" );
	if ( NULL == out ) 
	{	error_code = EC_ACGR_FILE_OPEN_ERROR;	goto ACGR_Source_Files_Status_Enumeration_Error;	}

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
# ifdef ACGR_PREPEND_TIMESTAMP
	ACGR_Date_Time( out, file_name, TRUE );
# endif
	row_max = Status_All_Count();
	/* Output prefix along with last value */
	error_code = Status_CSV_Cell_Get( row_max - 1, STATUS_CSV_COLUMN_ENUM_VALUE, buf, ACGR_BUF_SIZE );
	fprintf( out, acgr_status_enum_prefix, buf );
	/* build the output format string by finding the longest symbol string */
	col_width = Status_CSV_Column_Width( STATUS_CSV_COLUMN_ENUM_SYMBOL );
	String_Copy( format, "     %-", 16 );
	row = String_Length( format ); /* row is used temporarily to get to end character */
	sprintf( &(format[row]), "%d", col_width );
	String_Concatenate( format, "s =", 16 );
	/* output all the rows */

	for ( row = 0; row < row_max; row++ )
	{ /* rows */
		error_code = Status_CSV_Cell_Get( row, STATUS_CSV_COLUMN_ENUM_SYMBOL, buf, ACGR_BUF_SIZE );
		fprintf( out, format, buf );
		error_code = Status_CSV_Cell_Get( row, STATUS_CSV_COLUMN_ENUM_VALUE, buf, ACGR_BUF_SIZE );
		fprintf( out, "     %10s,\n", buf );
	}
	fprintf( out, "%s", acgr_status_enum_postfix );
	fclose( out );
	return SUCCESS;
#else  /* TARGET_AUTO_CODE_GEN_REPORTS */
	fprintf( out, "ACGR_Source_File_Status_Enumeration: unavailable.  TARGET_AUTO_CODE_GEN_REPORTS != 1\n" ); 
	error_code = EC_ACGR_REPORTS_UNAVAILABLE;	
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */
ACGR_Source_Files_Status_Enumeration_Error:
	if ( NULL != out ) fclose( out );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_ACGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************//**
 * Acgr initialize.
 *
 *
 * \return	An int.
 **************************************************************************************************/
int ACGR_Initialize( void )
{
	return SUCCESS;
}
/**********************************************************************************************//**
 * Acgr terminate.
 *
 *
 * \return	An int.
 **************************************************************************************************/
int ACGR_Terminate( void )
{
	return SUCCESS;
}
/**********************************************************************************************//**
 * The module acgr definition.
 **************************************************************************************************/
module_definition_type module_acgr_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	ACGR_Initialize,
	ACGR_Terminate,
	MODULE_ACGR_NAME,
	ACGR_MODULE_REV,
	ACGR_COMMAND_REV,
	ACGR_STATUS_REV
};

/**********************************************************************************************//**
 * Tests acgr integrity.
 *
 *
 * \param	integrity_list	List of integrities.
 *
 * \return	An AULCALLCONV.
 **************************************************************************************************/
AUL_DLL_API int AULCALLCONV ACGR_Integrity_Test( const aul_integrity_type * integrity_list )
{
	int							error_code;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	MODULE_CSV_COLUMN_ENUM	col;
	size_t						row;
/*	size_t						row_max;*/
/*	const int					zero = 0;*/
	uint32_t					scratch;
	char						buf[ACGR_BUF_SIZE];

/*TODO: add improved error reporting...more detailed codes!*/
	/* test a row at a time */
	row = 0;
	while ( integrity_list[row].id )
	{ 
		Mem_Set( buf, 0, ACGR_BUF_SIZE );
		col = MODULE_CSV_COLUMN_ID;
		error_code = Module_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
		if ( error_code ) {	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Integrity_Test_Error;	}
		scratch = strtoul( buf, NULL, 10 );
		if ( scratch != integrity_list[row].id ) goto ACGR_Integrity_Test_Error; 

		if ( MODULE_IO == scratch )
		{
			if ( integrity_list[row].param != ( (size_t) IO_ID_Get() ) ) goto ACGR_Integrity_Test_Error;
		}

		Mem_Set( buf, 0, ACGR_BUF_SIZE );
		col = MODULE_CSV_COLUMN_MODULE_TIME_STAMP;
		error_code = Module_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
		if ( error_code ) {	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Integrity_Test_Error;	}
		scratch = strtoul( buf, NULL, 10 );
		if ( scratch != integrity_list[row].module_time_stamp ) goto ACGR_Integrity_Test_Error; 

		Mem_Set( buf, 0, ACGR_BUF_SIZE );
		col = MODULE_CSV_COLUMN_COMMAND_TIME_STAMP;
		error_code = Module_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
		if ( error_code ) {	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Integrity_Test_Error;	}
		scratch = strtoul( buf, NULL, 10 );
		if ( scratch != integrity_list[row].command_time_stamp ) goto ACGR_Integrity_Test_Error;

		Mem_Set( buf, 0, ACGR_BUF_SIZE );
		col = MODULE_CSV_COLUMN_STATUS_TIME_STAMP;
		error_code = Module_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
		if ( error_code ) {	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Integrity_Test_Error;	}
		scratch = strtoul( buf, NULL, 10 );
		if ( scratch != integrity_list[row].status_time_stamp ) goto ACGR_Integrity_Test_Error;

		row++;
	}
	return SUCCESS;
ACGR_Integrity_Test_Error:
#else  /* TARGET_AUTO_CODE_GEN_REPORTS */
	error_code = EC_ACGR_REPORTS_UNAVAILABLE;
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_ACGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/********************************************************************************
static const char acgr_integrity_prefix[] =
{
	"struct aul_integrity\n"
	"{\n"
	"     size_t         id;\n"
	"     size_t         param;\n"
	"     uint32_t       module_time_stamp;	// 0 = not populated \n"
	"     uint32_t       command_time_stamp;	// 0 = not populated \n"
	"     uint32_t       status_time_stamp;	// 0 = not populated \n"
	"};\n"
	"typedef struct aul_integrity aul_integrity_type;\n"
	"\n"
	"aul_integrity_type aul_integrity_list[] =\n"
	"{\n"
};*/

/**********************************************************************************************//**
 * The acgr integrity prefix[].
 **************************************************************************************************/
static const char acgr_integrity_prefix[] =
{
	"\n"
/* TODO: improve this include because the file could be located anywhere */
	"#include \"../acgr/acgr.h\"\n"
	"aul_integrity_type aul_integrity_list[] =\n"
	"{\n"
};
/**********************************************************************************************//**
 * The acgr integrity postfix[].
 **************************************************************************************************/
static const char acgr_integrity_postfix[] =
{
	"};\n"
	"\n"
};
/**********************************************************************************************//**
 * Acgr source file integrity.
 *
 * \param	file_name	Filename of the file.
 *
 * \return	An AULCALLCONV.
 **************************************************************************************************/
AUL_DLL_API int AULCALLCONV ACGR_Source_File_Integrity( const char * file_name )
{
	int							error_code;
	FILE *						out;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	MODULE_CSV_COLUMN_ENUM	col;
	size_t						row;
	size_t						row_max;
	char						buf[ACGR_BUF_SIZE];
	const int zero = 0;
#endif

	out = fopen( file_name, "wt" );
	if ( NULL == out ) 
	{	error_code = EC_ACGR_FILE_OPEN_ERROR;	goto ACGR_Source_File_Integrity_Error;	}

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
# ifdef ACGR_PREPEND_TIMESTAMP
	ACGR_Date_Time( out, file_name, TRUE );
# endif
	fprintf( out, "%s", acgr_integrity_prefix  );

	/* output a row at a time */
	row_max = Module_All_Count();
	for ( row = 0; row < row_max; row++ )
	{ /* rows */
		fprintf( out, "     {  " );

		Mem_Set( buf, 0, ACGR_BUF_SIZE );
		col = MODULE_CSV_COLUMN_ID;
		error_code = Module_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
		if ( error_code ) {	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Source_File_Integrity_Error;	}
		fprintf( out, "%10s,", buf );

/*TODO: We might need to get the time stamps directly from the driver as well
Maybe driver needs to be an additional module?*/
		if ( MODULE_IO == strtol( buf, NULL, 10 ) )  fprintf( out, "%10d,", IO_ID_Get() );
		else														fprintf( out, "%10d,", zero );

		Mem_Set( buf, 0, ACGR_BUF_SIZE );
		col = MODULE_CSV_COLUMN_MODULE_TIME_STAMP;
		error_code = Module_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
		if ( error_code ) {	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Source_File_Integrity_Error;	}
		if ( String_Length( buf ) > 0 ) fprintf( out, "%10s,", buf );
		else                            fprintf( out, "%10d,", zero );

		Mem_Set( buf, 0, ACGR_BUF_SIZE );
		col = MODULE_CSV_COLUMN_COMMAND_TIME_STAMP;
		error_code = Module_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
		if ( error_code ) {	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Source_File_Integrity_Error;	}
		if ( String_Length( buf ) > 0 ) fprintf( out, "%10s,", buf );
		else                            fprintf( out, "%10d,", zero );

		Mem_Set( buf, 0, ACGR_BUF_SIZE );
		col = MODULE_CSV_COLUMN_STATUS_TIME_STAMP;
		error_code = Module_CSV_Cell_Get( row, col, buf, ACGR_BUF_SIZE );
		if ( error_code ) {	error_code = EC_ACGR_CELL_ERROR;	goto ACGR_Source_File_Integrity_Error;	}
		if ( String_Length( buf ) > 0 ) fprintf( out, "%10s,", buf );
		else                            fprintf( out, "%10d,", zero );
		/*TODO MIKE verify this change   fprintf( out, "  },\n", buf );*/
		fprintf( out, "  },\n" );
	}
	fprintf( out, "     {  " ); 
	for ( row = 0; row < 4; row++ ) fprintf( out, "%10d,", zero );
	fprintf( out, "%10d   },\n", zero );
	fprintf( out, "%s", acgr_integrity_postfix );
	fclose( out );
	return SUCCESS;
#else  /* TARGET_AUTO_CODE_GEN_REPORTS */
	fprintf( out, "ACGR_Source_File_Integrity: unavailable.  TARGET_AUTO_CODE_GEN_REPORTS != 1\n" ); 
	error_code = EC_ACGR_REPORTS_UNAVAILABLE;	
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */
ACGR_Source_File_Integrity_Error:
	if ( NULL != out ) fclose( out );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_ACGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************//**
 * The acgr device logical enum prefix[].
 **************************************************************************************************/
static const char acgr_device_logical_enum_prefix[] =
{
	"\n"
	"#ifndef AUL_DEVICE_LOGICAL_ENUM_H_\n"
	"#define AUL_DEVICE_LOGICAL_ENUM_H_\n"
	"\n"
	"#define AUL_DEVICE_LOGICAL_LAST_VALUE  %s"
	"\n"
	"\n"
	"enum aul_device_logical\n"
	"{\n"
};
/**********************************************************************************************//**
 * The acgr device logical enum postfix[].
 **************************************************************************************************/
static const char acgr_device_logical_enum_postfix[] =
{
	"};\n"
	"typedef enum aul_device_logical AUL_DEVICE_LOGICAL_ENUM;\n"
	"\n"
	"#endif  /* AUL_DEVICE_LOGICAL_ENUM_H_*/\n"
	"\n"
};
/**********************************************************************************************//**
 * Acgr source file device logical  enumeration.
 *
 * \param	file_name	Filename of the file.
 *
 * \return	An AULCALLCONV.
 **************************************************************************************************/
AUL_DLL_API int AULCALLCONV ACGR_Source_File_Device_Logical_Enumeration( const char * file_name )
{
	int							error_code;
	FILE *						out;
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	size_t						col_width;	
	size_t						row;
	size_t						row_max;
	char						format[16];
	char						buf[ACGR_BUF_SIZE];
#endif
	out = fopen( file_name, "wt" );
	if ( NULL == out ) 
	{	error_code = EC_ACGR_FILE_OPEN_ERROR;	goto ACGR_Source_File_Device_Logical_Enumeration_Error;	}
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
# ifdef ACGR_PREPEND_TIMESTAMP
	ACGR_Date_Time( out, file_name, TRUE );
# endif
	row_max = Device_Available_All_Count();
	/* Output prefix along with last value */
	error_code = Device_Available_CSV_Cell_Get( row_max - 1, DEVICE_AVAILABLE_CSV_COLUMN_ENUM_VALUE, buf, ACGR_BUF_SIZE );
	fprintf( out, acgr_device_logical_enum_prefix, buf );
	/* build the output format string by finding the longest symbol string */
	col_width = Device_Available_CSV_Column_Width( DEVICE_AVAILABLE_CSV_COLUMN_ENUM_SYMBOL );
	String_Copy( format, "     %-", 16 );
	row = String_Length( format ); /* row is used temporarily to get to end character */
	sprintf( &(format[row]), "%d", col_width );
	String_Concatenate( format, "s =", 16 );
	/* output all the rows */

	for ( row = 0; row < row_max; row++ )
	{ /* rows */
		error_code = Device_Available_CSV_Cell_Get( row, DEVICE_AVAILABLE_CSV_COLUMN_ENUM_SYMBOL, buf, ACGR_BUF_SIZE );
		fprintf( out, format, buf );
		error_code = Device_Available_CSV_Cell_Get( row, DEVICE_AVAILABLE_CSV_COLUMN_ENUM_VALUE, buf, ACGR_BUF_SIZE );
		fprintf( out, "     %10s,\n", buf );
	}
	fprintf( out, "%s", acgr_device_logical_enum_postfix );
	fclose( out );
	return SUCCESS;
#else  /* TARGET_AUTO_CODE_GEN_REPORTS */
	fprintf( out, "ACGR_Source_File_Device_List_Enumeration: unavailable.  TARGET_AUTO_CODE_GEN_REPORTS != 1\n" ); 
	error_code = EC_ACGR_REPORTS_UNAVAILABLE;	
#endif /* TARGET_AUTO_CODE_GEN_REPORTS */
ACGR_Source_File_Device_Logical_Enumeration_Error:
	if ( NULL != out ) fclose( out );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_ACGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
