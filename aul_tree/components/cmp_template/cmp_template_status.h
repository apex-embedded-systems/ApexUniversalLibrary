#ifndef COMPONENT_<TEMPLATE>_STATUS
#define COMPONENT_<TEMPLATE>_STATUS 16 /* component ID number */
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
#define <TEMPLATE>_STATUS_REV  "$Date: 2014-06-12 11:13:19 -0500 (Thu, 12 Jun 2014) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define COMPONENT_<TEMPLATE>_STATUS_EC_DEFINITIONS(_) \
	_( COMPONENT_<TEMPLATE>_STATUS,	EC_<TEMPLATE>_COMPONENT_INVALID,          1,  "component invalid"        )\
	_( COMPONENT_<TEMPLATE>_STATUS,	EC_<TEMPLATE>_DATASET_ALLOCATE,           2,  "dataset allocate"         )\
	_( COMPONENT_<TEMPLATE>_STATUS,	EC_<TEMPLATE>_CHANNEL_OUT_OF_RANGE,		  3,  "channel out of range"	  )\
	_( COMPONENT_<TEMPLATE>_STATUS,	EC_<TEMPLATE>_ILLEGAL_REQUEST,            4,  "illegal request"          )\
	_( COMPONENT_<TEMPLATE>_STATUS,	EC_<TEMPLATE>_INVALID_PORT_DIRECTION,     5,  "invalid direction"        )

#endif /* COMPONENT_<TEMPLATE>_STATUS */

