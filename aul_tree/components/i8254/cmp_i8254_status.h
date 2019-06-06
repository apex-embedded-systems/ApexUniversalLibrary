#ifndef COMPONENT_I8254_STATUS
#define COMPONENT_I8254_STATUS 19 /* component ID number */
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
#define I8254_STATUS_REV  "$Date: 2014-06-27 09:56:24 -0500 (Fri, 27 Jun 2014) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define COMPONENT_I8254_STATUS_EC_DEFINITIONS(_) \
	_( COMPONENT_I8254_STATUS,	EC_I8254_COMPONENT_INVALID,          1,  "component invalid"        )\
	_( COMPONENT_I8254_STATUS,	EC_I8254_DATASET_ALLOCATE,           2,  "dataset allocate"         )\
	_( COMPONENT_I8254_STATUS,	EC_I8254_CHANNEL_OUT_OF_RANGE,		 3,  "channel out of range"		)\
	_( COMPONENT_I8254_STATUS,	EC_I8254_CHANNEL_MISMATCH,           4,  "channel mismatch"         )\
	_( COMPONENT_I8254_STATUS,	EC_I8254_ILLEGAL_REQUEST,            5,  "illegal request"          )\
	_( COMPONENT_I8254_STATUS,	EC_I8254_CFG_CXFR_COMMAND,			 6,  "cfg xcfr command"			)\
	_( COMPONENT_I8254_STATUS,	EC_I8254_INVALID_MODE,				 9,  "invalid mode request"		)\
	_( COMPONENT_I8254_STATUS,	EC_I8254_FREQ_OUT_OF_RANGE,	   	    11,  "request out of range"		)\
	_( COMPONENT_I8254_STATUS,	EC_I8254_TIME_OUT_OF_RANGE,	   	    12,  "request out of range"		)\
	_( COMPONENT_I8254_STATUS,	EC_I8254_CLK_OUT_OF_RANGE,	   	    13,  "clock out of range"		)
	

#endif /* COMPONENT_I8254_STATUS */

