#ifndef MODULE_MEM_STATUS
#define MODULE_MEM_STATUS 4 /* module ID number */
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
#define MEM_STATUS_REV  "$Date: 2014-04-08 10:02:54 -0500 (Tue, 08 Apr 2014) $"

/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define MODULE_MEM_STATUS_EC_DEFINITIONS(_) \
	_( MODULE_MEM_STATUS,		EC_MEM_MISALIGNMENT,			1,  "misalignment"		)

#endif /* MODULE_MEM_STATUS */

