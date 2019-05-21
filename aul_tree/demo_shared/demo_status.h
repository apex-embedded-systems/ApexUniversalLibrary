#ifndef APPLICATION_STATUS
#define APPLICATION_STATUS 18 /* ID number */
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
#define APPLICATION_STATUS_REV  "$Date: 2015-01-27 16:28:10 -0600 (Tue, 27 Jan 2015) $"

/*******************************************************************************/
/* X-macro used to define error information in a nice table format
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define APPLICATION_STATUS_EC_DEFINITIONS(_) \
	_( APPLICATION_STATUS,	EC_STX104_CHANNEL_OUT_OF_RANGE,					 1,  "channel out of range"					)\
	_( APPLICATION_STATUS,	EC_STX104_COMPONENT_ALLOCATE,					 2,  "component allocation"					)\
	_( APPLICATION_STATUS,	EC_STX104_AI_ENCODING_OUT_OF_RANGE,				 3,  "ai encoding out of range"				)\
	_( APPLICATION_STATUS,	EC_STX104_AI_UNCALIBRATED_CONVERSION_WARNING,	 4,  "ai uncalibrated warning"				)\
	_( APPLICATION_STATUS,	EC_STX104_AI_JUMPERS_M0_M1_M2_M4,				 5,  "must install M0 M1 M2"				)\
	_( APPLICATION_STATUS,	EC_STX104_AI_FIRST_CHANNEL_OUT_OF_RANGE,		 6,  "ai channel_first range"				)\
	_( APPLICATION_STATUS,	EC_STX104_AI_LAST_CHANNEL_OUT_OF_RANGE,			 7,  "ai channel_last range"				)\
	_( APPLICATION_STATUS,	EC_STX104_AI_FIRST_CHAN_GREATER_THAN_LAST_CHAN,	 8,  "ai channel_first > channel_last"		)\
	_( APPLICATION_STATUS,	EC_STX104_AI_INTRA_SAMPLE_TIMING_TOO_LARGE_AIBT, 9,  "ai intrasample timing too large"		)\
	_( APPLICATION_STATUS,	EC_STX104_AI_INTRA_SAMPLE_TIMING_TOO_SMALL_AIBT,10,  "ai intrasample timing too small"		)\
	_( APPLICATION_STATUS,	EC_STX104_AI_FIFO_FULL,							11,  "ai fifo full"							)\
	_( APPLICATION_STATUS,	EC_STX104_AI_FRAME_TIMING_TOO_SMALL_AIFT,		12,  "ai frame time too small"				)\
	_( APPLICATION_STATUS,	EC_STX104_SPI_DI_CFG_COLLISION_ERROR,			13,  "spi cfg di collision"					)

#endif /* APPLICATION_STATUS */


