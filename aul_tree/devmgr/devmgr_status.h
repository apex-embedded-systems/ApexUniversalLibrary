#ifndef MODULE_DEVMGR_STATUS
#define MODULE_DEVMGR_STATUS 12 /* module ID number */
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

/*********************************************************************************/
#define DEVMGR_STATUS_REV  "$Date: 2014-03-31 11:04:05 -0500 (Mon, 31 Mar 2014) $"
/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define MODULE_DEVMGR_STATUS_EC_DEFINITIONS(_) \
	_( MODULE_DEVMGR_STATUS,	EC_DEVMGR_COMMAND_INVALID,			   1,  "command invalid"			)\
	_( MODULE_DEVMGR_STATUS,	EC_DEVMGR_ALLOCATE_MEMORY,				2,  "memory allocate"			)\
	_( MODULE_DEVMGR_STATUS,	EC_DEVMGR_ALLOCATE_DEVICE,				3,  "device allocate"			)\
	_( MODULE_DEVMGR_STATUS,	EC_DEVMGR_IO_DEFINITION,				4,  "io definition"				)\
	_( MODULE_DEVMGR_STATUS,	EC_DEVMGR_DEVICE_DEFINITION,			5,  "device definition"			)\
	_( MODULE_DEVMGR_STATUS,	EC_DEVMGR_DEVICE_NULL,					6,  "device null pointer"		)


#endif /* MODULE_DEVMGR_STATUS */

