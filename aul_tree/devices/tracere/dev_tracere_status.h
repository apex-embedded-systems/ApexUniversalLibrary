#ifndef DEVICE_TRACERE_STATUS
#define DEVICE_TRACERE_STATUS 17 /* component ID number */
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
#define TRACERE_STATUS_REV  "$Date: 2014-06-27 09:56:24 -0500 (Fri, 27 Jun 2014) $"

/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define DEVICE_TRACERE_STATUS_EC_DEFINITIONS(_) \
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_COMMAND_NOT_IMPLEMENTED,		1,  "function not implemented"	)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_NULL_POINTER,				2,  "null pointer encountered"	)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_CHANNEL_OUT_OF_RANGE,		3,  "channel out of range"		)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_COMPONENT_ALLOCATE,			4,  "component allocation"		)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_PRIVATE_ALLOCATE,			5,  "private data allocation"	)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_CONFIG_TYPE_NOT_FOUND,		6,  "config type not found"		)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_DATA_TYPE_ERROR,				7,  "incorrect data type"		)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_RANGE_SOURCE_LED,			8,  "led source out of range"	)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_CT_INVALID_MODE,				9,  "invalid mode request"		)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_CT_CHANNEL_MISMATCH,		   10,  "channel mismatch"			)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_FREQ_OUT_OF_RANGE,	   	   11,  "request out of range"		)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_TIME_OUT_OF_RANGE,	   	   12,  "request out of range"		)\
	_( DEVICE_TRACERE_STATUS,	EC_TRACERE_CLK_OUT_OF_RANGE,	   	   13,  "clock out of range"		)

#endif /* DEVICE_TRACERE_STATUS */

