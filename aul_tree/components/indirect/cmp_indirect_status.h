#ifndef COMPONENT_INDIRECT_STATUS
#define COMPONENT_INDIRECT_STATUS 21 /* component ID number */
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
#define INDIRECT_STATUS_REV  "$Date: 2014-06-25 10:29:34 -0500 (Wed, 25 Jun 2014) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define COMPONENT_INDIRECT_STATUS_EC_DEFINITIONS(_) \
	_( COMPONENT_INDIRECT_STATUS,	EC_INDIRECT_COMPONENT_INVALID,          1,  "component invalid"			)\
	_( COMPONENT_INDIRECT_STATUS,	EC_INDIRECT_DATASET_ALLOCATE,           2,  "dataset allocate"			)\
	_( COMPONENT_INDIRECT_STATUS,	EC_INDIRECT_ADDRESS_SIZE,				3,  "address size"				)\
	_( COMPONENT_INDIRECT_STATUS,	EC_INDIRECT_ADDRESS_APERTURE,			4,  "address aperture"			)\
	_( COMPONENT_INDIRECT_STATUS,	EC_INDIRECT_DATA_APERTURE,				5,  "data aperture"				)\
	_( COMPONENT_INDIRECT_STATUS,	EC_INDIRECT_COMPONENT_LINK,				6,  "component link"			)


#endif /* COMPONENT_INDIRECT_STATUS */

