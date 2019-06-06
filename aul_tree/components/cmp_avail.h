#ifndef  MODULE_COMPONENTS_AVAILABLE
#define  MODULE_COMPONENTS_AVAILABLE

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

/* Contains all the component headers from which we conditionally
   build the component_available_list[] below.
*/
#include "cmp_avail_module.h"
#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/sysval.h"
#include "../cmpmgr/cmpmgr.h"

/*****************************************************************
Description
Used within cmp_avail.c
*/
struct component_available
{
	SYS_TYPE					type;
	size_t						logical_id;
	component_definition_type *	definition; 
};
typedef struct component_available component_available_type;
/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/

extern component_available_type component_available_list[];

/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/
#endif /* MODULE_COMPONENTS_AVAILABLE */
