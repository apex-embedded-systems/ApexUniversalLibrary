#ifndef MODULE_REPORT
#define MODULE_REPORT       29 /* module ID number */
#define MODULE_REPORT_NAME "report"
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
    
	COPYRIGHT NOTICE Copyright (c) 20140-2015 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded 
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


*********************************************************************************/

#include "../shared/module_macros.h"
#include "../shared/debug.h"


/*******************************************************************************/
struct report_prefix_list
{
	struct report_prefix_list * next;		/* append additional prefix information here */
	char *                      prefix;
};

/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/
#if defined( DEBUG_KERNEL )
 void Report_Write( struct report_prefix_list * list, char * format, ... );
#else
 void Report_Write( FILE * out, struct report_prefix_list * list, char * format, ... );
#endif

int Report_Initialize( void );

int Report_Terminate( void );

extern module_definition_type module_report_definition;

/*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* MODULE_ERROR */
