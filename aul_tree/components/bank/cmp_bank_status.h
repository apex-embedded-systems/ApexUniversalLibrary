#ifndef COMPONENT_BANK_STATUS
#define COMPONENT_BANK_STATUS 21 /* component ID number */
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
#define BANK_STATUS_REV  "$Date: 2015-02-04 20:27:51 -0600 (Wed, 04 Feb 2015) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define COMPONENT_BANK_STATUS_EC_DEFINITIONS(_) \
	_( COMPONENT_BANK_STATUS,	EC_BANK_COMPONENT_INVALID,          1,  "component invalid"			)\
	_( COMPONENT_BANK_STATUS,	EC_BANK_DATASET_ALLOCATE,           2,  "dataset allocate"			)\
	_( COMPONENT_BANK_STATUS,	EC_BANK_ADDRESS_SIZE,				3,  "address size"				)\
	_( COMPONENT_BANK_STATUS,	EC_BANK_ADDRESS_APERTURE,			4,  "address aperture"			)\
	_( COMPONENT_BANK_STATUS,	EC_BANK_DATA_APERTURE,				5,  "data aperture"				)\
	_( COMPONENT_BANK_STATUS,	EC_BANK_COMPONENT_LINK,				6,  "component link"			)


#endif /* COMPONENT_BANK_STATUS */

