#ifndef COMPONENT_EGS_STATUS
#define COMPONENT_EGS_STATUS 30 /* component ID number */
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
#define EGS_STATUS_REV  "$Date: 2015-04-17 19:12:24 -0500 (Fri, 17 Apr 2015) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define COMPONENT_EGS_STATUS_EC_DEFINITIONS(_) \
	_( COMPONENT_EGS_STATUS,	EC_EGS_COMPONENT_INVALID,          	1,  "component invalid"        	)\
	_( COMPONENT_EGS_STATUS,	EC_EGS_DATASET_ALLOCATE,           	2,  "dataset allocate"         	)\
	_( COMPONENT_EGS_STATUS,	EC_EGS_AI_INTRASAMPLE_RANGE,		3,  "ai intrasample range"	  	)\
	_( COMPONENT_EGS_STATUS,	EC_EGS_ILLEGAL_REQUEST,            	4,  "illegal request"          	)\
	_( COMPONENT_EGS_STATUS,	EC_EGS_INVALID_PORT_DIRECTION,     	5,  "invalid direction"        	)\
	_( COMPONENT_EGS_STATUS,	EC_EGS_PROBE,				     	6,  "not present (probe fail)" 	)

/* NOTE:  In Linux probe or probing generally means to scan the hardware to see if the hardware
 *        really exists or not.  Otherwise, it is always assumed to be present.
 *
 */

#endif /* COMPONENT_EGS_STATUS */

