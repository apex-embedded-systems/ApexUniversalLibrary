#ifndef MODULE_ARRAY_STATUS
#define MODULE_ARRAY_STATUS 10 /* module ID number */
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

#define ARRAY_STATUS_REV  "$Date: 2014-03-31 11:04:05 -0500 (Mon, 31 Mar 2014) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/

#define MODULE_ARRAY_STATUS_EC_DEFINITIONS(_) \
	_( MODULE_ARRAY_STATUS,	EC_ARRAY_ELEMENT_TYPE,		1,  "element type"				) \
	_( MODULE_ARRAY_STATUS,	EC_ARRAY_ELEMENT_QTY,		2,  "element quantity"			) \
	_( MODULE_ARRAY_STATUS,	EC_ARRAY_ELEMENT_SIZE,		3,  "element size"				) \
	_( MODULE_ARRAY_STATUS,	EC_ARRAY_VALIDITY,			4,  "validity error"				)



#endif /* MODULE_ARRAY_STATUS */

