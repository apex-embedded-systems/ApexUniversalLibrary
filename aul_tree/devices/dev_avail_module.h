#ifndef  DEVICES_AVAILABLE_MODULE
#define  DEVICES_AVAILABLE_MODULE

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
#include "../shared/target.h" /* TARGET_AUTO_CODE_GEN_REPORTS */

/***************************************************************************/
#define DEVICE_AVAILABLE_MODULE_H_REV  "$Date: 2015-07-30 20:32:00 -0500 (Thu, 30 Jul 2015) $"
/***************************************************************************/

/*******************************************************************/
#include "../devices/tracere/dev_tracere.h"
#include "../devices/stx104/dev_stx104.h"

/*
TODO: not sure how that would work exactly with respect to the X-macro
below.  We need to restrict changes in addition/removal of device modules
to just this file.
*/
#ifdef DEVICE_TRACERE	
#endif

#ifdef DEVICE_STX104
#endif

/*******************************************************************/
/* define device information here.  All the rest is automated */

#define DEVICE_AVAIL_MODULE_DEFINITIONS(_) \
	_( TRACERE, 	DEVICE_TRACERE, 	MODULE_PUBLIC, 	device_tracere_definition )\
	_( STX104,  	DEVICE_STX104,		MODULE_PUBLIC, 	device_stx104_definition  )\
/* <EGS_BEGIN>  */\
	_( STX104_EGS, 	DEVICE_STX104_EGS,	MODULE_PUBLIC, 	device_stx104_definition  )\
/* <EGS_END> */


/*******************************************************************/
#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
	#define DEVICE_AVAIL_EXTRACT_DEVICE_DEFINITIONS(name,base_id,module_public,definition) \
		{ SYS_TYPE_MAKE(device_available_type), MODULE_STR(DEVICE_LOGICAL_ ## name), &(definition) },
#else
	#define DEVICE_AVAIL_EXTRACT_DEVICE_DEFINITIONS(name,base_id,module_public,definition) \
		{ SYS_TYPE_MAKE(device_available_type), &(definition) }
#endif

/*******************************************************************/
#define DEVICE_AVAIL_EXTRACT_DEVICE_LOGICAL(name,base_id,module_public,definition)   \
	DEVICE_LOGICAL_ ## name  = IOC_CREATE_LOGICAL_ID_DEVICE(base_id),
/*******************************************************************/
#define DEVICE_AVAIL_EXTRACT_MODULE_INFO(name,base_id,module_public,definition)  \
	{ base_id, module_public, MODULE_STR(base_id), (void *) &(definition)  },

/*******************************************************************/
typedef enum
{
	DEVICE_LOGICAL_UNDEFINED        = IOC_LOGICAL_ID_NULL,
	DEVICE_AVAIL_MODULE_DEFINITIONS( DEVICE_AVAIL_EXTRACT_DEVICE_LOGICAL ) 
} DEVICE_LOGICAL_ENUM;
/*******************************************************************/

#if(0)
#define DEVICE_AVAIL_MODULES_V2 \
	{ DEVICE_TRACERE, MODULE_PUBLIC, MODULE_STR(DEVICE_TRACERE), (void *) &device_tracere_definition },\
	{ DEVICE_STX104,  MODULE_PUBLIC, MODULE_STR(DEVICE_STX104),  (void *) &device_stx104_definition  },
#endif

#endif /* DEVICES_AVAILABLE_MODULE */
