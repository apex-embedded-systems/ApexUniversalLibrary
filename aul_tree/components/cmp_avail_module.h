#ifndef  COMPONENTS_AVAILABLE_MODULE
#define  COMPONENTS_AVAILABLE_MODULE

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

/***************************************************************************/
#define COMPONENT_AVAILABLE_MODULE_H_REV  "$Date: 2015-07-31 21:59:26 -0500 (Fri, 31 Jul 2015) $"
/***************************************************************************/

/*******************************************************************/

#include "../components/i8255m0/cmp_i8255m0.h"
#include "../components/i8254/cmp_i8254.h"
#include "../components/indirect/cmp_indirect.h"

#ifdef COMPONENT_I8255M0
# define COMPONENT_AVAIL_MODULE_1	{ COMPONENT_I8255M0, MODULE_PUBLIC, MODULE_STR(COMPONENT_I8255M0), (void *) & component_i8255m0_definition },
#else
# define COMPONENT_AVAIL_MODULE_1
#endif

#ifdef COMPONENT_I8254
# define COMPONENT_AVAIL_MODULE_2	{ COMPONENT_I8254,   MODULE_PUBLIC, MODULE_STR(COMPONENT_I8254),   (void *) & component_i8254_definition   },
#else
# define COMPONENT_AVAIL_MODULE_2
#endif

#ifdef COMPONENT_INDIRECT
# define COMPONENT_AVAIL_MODULE_3	{ COMPONENT_INDIRECT,   MODULE_PUBLIC, MODULE_STR(COMPONENT_INDIRECT),   (void *) & component_indirect_definition   },
#else
# define COMPONENT_AVAIL_MODULE_3
#endif

/* This is a predefined aggregate list (from above) that is used in module.c module_list_v2[]  */
#define COMPONENT_AVAIL_MODULES_V2	\
	COMPONENT_AVAIL_MODULE_1		\
	COMPONENT_AVAIL_MODULE_2		\
	COMPONENT_AVAIL_MODULE_3



#endif /* COMPONENTS_AVAILABLE_MODULE */

