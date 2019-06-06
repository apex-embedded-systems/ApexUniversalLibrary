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

#include "cmp_avail.h"

/*********************************************************************************/
#define COMPONENT_AVAILABLE_MODULE_REV  "$Date: 2015-02-02 17:40:08 -0600 (Mon, 02 Feb 2015) $"


#if(0)
/*******************************************************************/
#define COMPONENT_AVAIL_EXTRACT_LIST(id,public,definition) { SYS_TYPE_MAKE(component_type), IOC_CREATE_LOGICAL_ID_COMPONENT(id), &(definition) },

component_available_type component_available_list[] =
{
		COMPONENT_I8255M0_MODULE_INFORMATION( COMPONENT_AVAIL_EXTRACT_LIST )
};
#endif




/* This is a predefined aggregate list (from above) that is used by component manager. */
component_available_type component_available_list[] =
{
#if defined( COMPONENT_I8255M0 )
	{	SYS_TYPE_MAKE(component_type), IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_I8255M0), &(component_i8255m0_definition) }, 
#endif
#if defined( COMPONENT_I8254 )
	{	SYS_TYPE_MAKE(component_type), IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_I8254), &(component_i8254_definition) }, 
#endif
	/* null terminated list -- null entry in the list */
	{ SYS_TYPE_OF(NONE), IOC_PHYSICAL_ID_NULL, NULL }
};
