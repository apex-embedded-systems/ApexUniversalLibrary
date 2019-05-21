#ifndef MODULE_CMPMGR_STATUS
#define MODULE_CMPMGR_STATUS 11 /* module ID number */
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
#define CMPMGR_STATUS_REV  "$Date: 2014-06-25 10:29:34 -0500 (Wed, 25 Jun 2014) $"

/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define MODULE_CMPMGR_STATUS_EC_DEFINITIONS(_) \
	_( MODULE_CMPMGR_STATUS,	EC_CMP_BUF_NULL,					1,  "null buffer"			) \
	_( MODULE_CMPMGR_STATUS,	EC_CMP_PRIVATE_DATA_NULL,			2,  "null private data"		) \
	_( MODULE_CMPMGR_STATUS,	EC_CMP_TYPE_TYPES_DO_NOT_MATCH,		3,  "types do not match"	) \
	_( MODULE_CMPMGR_STATUS,	EC_CMPMGR_SETUP_COMPONENT_LIST,		5,  "setup list"			) \
	_( MODULE_CMPMGR_STATUS,	EC_CMPMGR_SETUP_COMPONENT_QTY,		6,  "component quantity"	) \
	_( MODULE_CMPMGR_STATUS,	EC_CMPMGR_SETUP_INVENTORY,			7,  "setup inventory"		) \
	_( MODULE_CMPMGR_STATUS,	EC_CMPMGR_SETUP_AVAILABLE,			8,  "setup available"		) \
	_( MODULE_CMPMGR_STATUS,	EC_CMPMGR_SETUP_LIST_FULL,			9,  "setup list full"		) \
	_( MODULE_CMPMGR_STATUS,	EC_CMPMGR_SETUP_ADDRESSING,			10,  "setup addressing"		) \
	_( MODULE_CMPMGR_STATUS,	EC_CMP_COMMAND_INVALID,				11,  "command invalid"		) \
	_( MODULE_CMPMGR_STATUS,	EC_CMPMGR_REG_WRITE,				12,  "register write"		) \
	_( MODULE_CMPMGR_STATUS,	EC_CMPMGR_REG_READ,					13,  "register read"		)

#endif /* MODULE_CMPMGR_STATUS */

