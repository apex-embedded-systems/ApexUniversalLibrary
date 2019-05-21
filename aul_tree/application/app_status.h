#ifndef APPLICATION_STATUS
#define APPLICATION_STATUS 28 /* ID number */
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
#define APPLICATION_STATUS_REV  "$Date: 2015-01-27 17:09:29 -0600 (Tue, 27 Jan 2015) $"

/*******************************************************************************/
/* X-macro used to define error information in a nice table format
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define APPLICATION_STATUS_EC_DEFINITIONS(_) \
	_( APPLICATION_STATUS,	EC_APP_TYPE_INVALID,		 1,  "invalid data type"	)\
	_( APPLICATION_STATUS,	EC_APP_FILE_OPEN_ERROR,		 2,  "failed to open file"  )

#endif /* APPLICATION_STATUS */

