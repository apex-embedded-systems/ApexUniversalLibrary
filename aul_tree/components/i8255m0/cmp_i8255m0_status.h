#ifndef COMPONENT_I8255M0_STATUS
#define COMPONENT_I8255M0_STATUS 16 /* component ID number */
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
#define I8255M0_STATUS_REV  "$Date: 2014-06-03 20:17:32 -0500 (Tue, 03 Jun 2014) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define COMPONENT_I8255M0_STATUS_EC_DEFINITIONS(_) \
	_( COMPONENT_I8255M0_STATUS,	EC_I8255M0_COMPONENT_INVALID,          1,  "component invalid"        )\
	_( COMPONENT_I8255M0_STATUS,	EC_I8255M0_DATASET_ALLOCATE,           2,  "dataset allocate"         )\
	_( COMPONENT_I8255M0_STATUS,	EC_I8255M0_CHANNEL_OUT_OF_RANGE,	   3,  "channel out of range"	  )\
	_( COMPONENT_I8255M0_STATUS,	EC_I8255M0_ILLEGAL_REQUEST,            4,  "illegal request"          )\
	_( COMPONENT_I8255M0_STATUS,	EC_I8255M0_INVALID_PORT_DIRECTION,     5,  "invalid direction"        )

#endif /* COMPONENT_I8255M0_STATUS */

