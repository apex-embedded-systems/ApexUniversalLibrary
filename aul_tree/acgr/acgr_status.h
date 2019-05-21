#ifndef  MODULE_ACGR_STATUS
#define  MODULE_ACGR_STATUS 15  /* module ID number */

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
#define ACGR_STATUS_REV  "$Date: 2014-04-28 14:30:08 -0500 (Mon, 28 Apr 2014) $"

/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define MODULE_ACGR_STATUS_EC_DEFINITIONS(_) \
	_( MODULE_ACGR_STATUS,	EC_ACGR_REPORTS_UNAVAILABLE,	1,  "reports unavailable"	)\
	_( MODULE_ACGR_STATUS,	EC_ACGR_FILE_OPEN_ERROR,		2,  "file open"				)\
	_( MODULE_ACGR_STATUS,	EC_ACGR_COLUMN_NAME,			3,  "column name"			)\
	_( MODULE_ACGR_STATUS,	EC_ACGR_CELL_ERROR,				4,  "cell error"			)



#endif /* MODULE_ACGR_STATUS */