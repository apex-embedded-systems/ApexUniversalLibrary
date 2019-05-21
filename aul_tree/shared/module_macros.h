#ifndef  MODULE_MACROS_H_
#define  MODULE_MACROS_H_

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

#include "../shared/typebase.h"	 /* uint32_t		*/
#include "../shared/sysval.h"

/*********************************************************************************/
enum { MODULE_PRIVATE = 0, MODULE_PUBLIC = 1 };
/*********************************************************************************/
/* use MODULE_STR(x) to turn "x" into a string */
#define MODULE_STR_DO(x) #x
#define MODULE_STR(x) MODULE_STR_DO(x)
/*********************************************************************************/
typedef enum
{
	MODULE_CSV_COLUMN_NONE					=  0,
	MODULE_CSV_COLUMN_NAME					=  1,
	MODULE_CSV_COLUMN_ID					=  2,
	MODULE_CSV_COLUMN_TYPE					=  3,
	MODULE_CSV_COLUMN_MODULE_TIME_STAMP		=  4,
	MODULE_CSV_COLUMN_COMMAND_TIME_STAMP	=  5,
	MODULE_CSV_COLUMN_STATUS_TIME_STAMP		=  6,
	MODULE_CSV_COLUMN_UNKNOWN				=  7
} MODULE_CSV_COLUMN_ENUM;
/*********************************************************************************/
typedef int  ( * module_fnc     )( void );
/*******************************************************************************/
struct module_definition
{
	const SYS_TYPE		type;

	module_fnc		initialize;
	module_fnc		terminate;
	/* Purpose of revision information: 
			1. Module management.  
			2. Determine if application and module status/commands are in sync.
	*/
	const char *	name;
	const char *	svn_module_date;
	const char *    svn_command_date;
	const char *	svn_status_date;
};
typedef struct module_definition module_definition_type;


#endif /* MODULE_MACROS_H_ */
