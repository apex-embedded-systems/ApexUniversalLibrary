#ifndef MODULE_AULHWDEF_STATUS
#define MODULE_AULHWDEF_STATUS  14   /* module ID number */
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
    
	COPYRIGHT NOTICE Copyright (c) 2014-2015 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded 
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:

*/
/*********************************************************************************/
#define AULHWDEF_STATUS_REV  "$Date: 2015-01-28 06:21:36 -0600 (Wed, 28 Jan 2015) $"

/*********************************************************************************/


/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define MODULE_AULHWDEF_STATUS_EC_DEFINITIONS(_) \
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_CANNOT_FIND_DEFINITION_FILE,	1,  "definition file not found"	)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_COLUMN_NAME_MISSING,			2,  "column name missing"		)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_COLUMN_NAME_UNKNOWN,			3,  "column name unknown"		)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_FILE_LOAD,						4,  "file load"					)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_IRQ_OUT_OF_RANGE,				5,  "irq range"					)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_BASE_ADDRESS_OUT_OF_RANGE,		6,  "base address range"		)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_TYPE_UNDEFINED,					7,  "type undefined"			)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_MNEMONIC_ERROR,					8,  "acronym"					)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_ACRONYM_NOT_FOUND,				9,  "acronym find"				)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_INVALID_HWDEF,				   10,  "invalid"					)\
	_( MODULE_AULHWDEF_STATUS,	EC_AULHWDEF_LOGICAL_ID,					   11,  "logical id"				)


#endif  /* AUL_HWDEF_H_ */

