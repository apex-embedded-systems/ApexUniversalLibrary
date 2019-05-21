#ifndef COMPONENT_SYSCON_STATUS
#define COMPONENT_SYSCON_STATUS 20 /* component ID number */
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

/*******************************************************************/
#define SYSCON_STATUS_REV  "$Date: 2014-08-04 13:36:31 -0500 (Mon, 04 Aug 2014) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define COMPONENT_SYSCON_STATUS_EC_DEFINITIONS(_) \
	_( COMPONENT_SYSCON_STATUS,	EC_SYSCON_COMPONENT_INVALID,         1,  "component invalid"        )\
	_( COMPONENT_SYSCON_STATUS,	EC_SYSCON_DATASET_ALLOCATE,          2,  "dataset allocate"         )\
	_( COMPONENT_SYSCON_STATUS,	EC_SYSCON_PERMISSION_WRITE,			 3,  "permission"				)



#endif /* COMPONENT_SYSCON_STATUS */

