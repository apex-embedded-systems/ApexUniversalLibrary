#ifndef MODULE_AUL_STATUS
#define MODULE_AUL_STATUS  13   /* module ID number */
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


*********************************************************************************/

/*********************************************************************************/
#define AUL_STATUS_REV  "$Date: 2014-06-10 15:02:29 -0500 (Tue, 10 Jun 2014) $"

/*********************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define MODULE_AUL_STATUS_EC_DEFINITIONS(_) \
	_( MODULE_AUL_STATUS,	EC_AUL_DEVICE_POINTER,		1,  "device pointer"			)\
	_( MODULE_AUL_STATUS,	EC_AUL_DUPLICATE_DEVICE,	2,  "already open"				)\
	_( MODULE_AUL_STATUS,	EC_AUL_ACRONYM_SIZE,		3,  "acronym size"				)\
	_( MODULE_AUL_STATUS,	EC_AUL_DEVICE_NOT_LISTED,	4,  "device not listed"			)\
	_( MODULE_AUL_STATUS,	EC_AUL_DEVICE_NULL_POINTER,	5,  "device null pointer"		)\
	_( MODULE_AUL_STATUS,	EC_AUL_DEVICE_NOT_OPEN,		6,  "device not open"			)\
	_( MODULE_AUL_STATUS,	EC_AUL_IOCTL_COMMAND,		7,  "ioctl command"				)\
	_( MODULE_AUL_STATUS,	EC_AUL_IOCTL_COMPONENT,		8,  "component"					)\
	_( MODULE_AUL_STATUS,	EC_AUL_BOARD_RANGE,			9,  "board range"				)\
	_( MODULE_AUL_STATUS,	EC_AUL_BOARD_DATASET_NULL, 10,  "board dataset null"		)\
	_( MODULE_AUL_STATUS,	EC_AUL_BOARD_NOT_OPEN,	   11,  "board not open"			)\
	_( MODULE_AUL_STATUS,	EC_AUL_DATA_TYPE_ERROR,	   12,  "data type mismatch"		)\
	_( MODULE_AUL_STATUS,	EC_AUL_INCOMPLETE_CODE,	   13,  "code incomplete"			)


#endif  /* MODULE_AUL_STATUS */

