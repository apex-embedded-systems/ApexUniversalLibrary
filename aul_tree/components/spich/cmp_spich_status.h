#ifndef COMPONENT_SPI_STATUS
#define COMPONENT_SPI_STATUS 16 /* component ID number */
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
#define SPI_STATUS_REV  "$Date: 2014-06-12 11:13:19 -0500 (Thu, 12 Jun 2014) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define COMPONENT_SPI_STATUS_EC_DEFINITIONS(_) \
	_( COMPONENT_SPI_STATUS,	EC_SPI_COMPONENT_INVALID,          1,  "component invalid"        )\
	_( COMPONENT_SPI_STATUS,	EC_SPI_DATASET_ALLOCATE,           2,  "dataset allocate"         )\
	_( COMPONENT_SPI_STATUS,	EC_SPI_CHANNEL_OUT_OF_RANGE,	   3,  "channel out of range"	  )\
	_( COMPONENT_SPI_STATUS,	EC_SPI_ILLEGAL_REQUEST,            4,  "illegal request"          )\
	_( COMPONENT_SPI_STATUS,	EC_SPI_INVALID_PORT_DIRECTION,     5,  "invalid direction"        )

	_( COMPONENT_SPI_STATUS,	EC_SPI_COMMAND_INVALID,			   5,  "invalid command"		)
	_( COMPONENT_SPI_STATUS,	EC_SPI_COMMAND_INVALID,       -CH_UNABLE_TO_LOAD_LIBRARY,  "cheetah unable to load library"		)


#endif /* COMPONENT_SPI_STATUS */

