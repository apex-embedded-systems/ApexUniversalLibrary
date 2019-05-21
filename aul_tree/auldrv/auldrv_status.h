#ifndef MODULE_AULDRV_STATUS
#define MODULE_AULDRV_STATUS  7   /* module ID number */
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
#define AULDRV_STATUS_REV  "$Date: 2014-06-18 09:51:47 -0500 (Wed, 18 Jun 2014) $"

/*********************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
/* TODO: COPY_FROM_USER and COPY_FROM_USER needs to be generalized to one location */
#define MODULE_AULDRV_STATUS_EC_DEFINITIONS(_) \
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_NULL,					 	1,  "null"						)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_DRIVER_ALREADY_OPEN,	 	2,  "driver already open"		)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_DEVICE_ALREADY_OPEN,		3,  "device already open"		)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_REGION_ALREADY_OPEN,		4,  "region already open"		)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_OPEN_MINOR_INVALID,	    5,  "minor invalid"				)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_DRIVER_OPEN_NOT,			6,  "driver not open"			)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_OPEN_RESOURCES_TO_USER,	7,  "open resources to user"	)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_COPY_FROM_USER,			8,  "copy from user"			)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_COPY_TO_USER,				9,  "copy to user"				)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_IOCTL_MAGIC_NUMBER,	   10,  "command invalid type"		)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_IOCTL_COMMAND_UNKNOWN,   11,  "command unknown"			)\
	_( MODULE_AULDRV_STATUS,	EC_AULDRV_IOCTL_DEFINITION,		   12,  "definition not found"		)


#endif  /* MODULE_AULDRV_STATUS */

