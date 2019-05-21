#ifndef DEVICE_STX104_STATUS
#define DEVICE_STX104_STATUS 18 /* component ID number */
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
#define STX104_STATUS_REV  "$Date: 2015-02-03 06:18:04 -0600 (Tue, 03 Feb 2015) $"

/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
#define DEVICE_STX104_STATUS_EC_DEFINITIONS(_) \
	_( DEVICE_STX104_STATUS,	EC_STX104_CHANNEL_OUT_OF_RANGE,					 1,  "channel out of range"					)\
	_( DEVICE_STX104_STATUS,	EC_STX104_COMPONENT_ALLOCATE,					 2,  "component allocation"					)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_ENCODING_OUT_OF_RANGE,				 3,  "ai encoding out of range"				)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_UNCALIBRATED_CONVERSION_WARNING,	 4,  "ai uncalibrated warning"				)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_JUMPERS_M0_M1_M2_M4,				 5,  "must install M0 M1 M2"				)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_FIRST_CHANNEL_OUT_OF_RANGE,		 6,  "ai channel_first range"				)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_LAST_CHANNEL_OUT_OF_RANGE,			 7,  "ai channel_last range"				)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_FIRST_CHAN_GREATER_THAN_LAST_CHAN,	 8,  "ai channel_first > channel_last"		)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_INTRA_SAMPLE_TIMING_TOO_LARGE_AIBT, 9,  "ai intrasample timing too large"		)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_INTRA_SAMPLE_TIMING_TOO_SMALL_AIBT,10,  "ai intrasample timing too small"		)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_FIFO_FULL,							11,  "ai fifo full"							)\
	_( DEVICE_STX104_STATUS,	EC_STX104_AI_FRAME_TIMING_TOO_SMALL_AIFT,		12,  "ai frame time too small"				)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_DI_CFG_COLLISION_ERROR,			13,  "spi cfg di collision"					)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_DO_CFG_COLLISION_ERROR,			14,  "spi cfg do collision"					)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_DO_SET_COLLISION_ERROR,			15,  "spi do set collision"					)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_FIRMWARE_ERROR,					16,  "spi firmware revision"				)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_INACTIVE_ERROR,					17,  "spi inactive"							)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_HALF_CLOCK_ERROR,					18,  "spi half clock"						)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_END_OF_CYCLE_DELAY_ERROR,			19,  "spi end cycle delay"					)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_SDI_MAP_ERROR,					20,  "spi sdi map"							)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_CSB_ERROR,						21,  "spi csb"								)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_TX_FULL,							22,  "spi tx full"							)\
	_( DEVICE_STX104_STATUS,	EC_STX104_SPI_RX_FULL,							23,  "spi rx full"							)\
	_( DEVICE_STX104_STATUS,	EC_STX104_PRIVATE_DATA_ALLOCATE,				24,  "private data allocation"				)\
	_( DEVICE_STX104_STATUS,	EC_STX104_CFG_INTERRUPT_ACTIVE,					25,  "interrupt active"						)\
	_( DEVICE_STX104_STATUS,	EC_STX104_COMMAND_NOT_IMPLEMENTED,				26,  "function not implemented"				)\
	_( DEVICE_STX104_STATUS,	EC_STX104_TRANSFER_DATA_TYPE_MISMATCH,			27,  "reg id and command data type"			)\
	_( DEVICE_STX104_STATUS,	EC_STX104_FIRMWARE_REVISION,					28,  "firmware revision"					)

#endif /* DEVICE_STX104_STATUS */

