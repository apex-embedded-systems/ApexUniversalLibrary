#ifndef MODULE_IO_STATUS
#define MODULE_IO_STATUS 8 /* module ID number */
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

#define IO_STATUS_REV  "$Date: 2014-06-10 15:02:29 -0500 (Tue, 10 Jun 2014) $"


/*******************************************************************************/
/* X-macro used to define error information in a nice table format 
	Requires the use of a wide screen; today this seems reasonable.

	Column definitions:
	module-ID, enumerated name, module error code number,  human readable information
*/
/* TODO: COPY_FROM_USER and COPY_FROM_USER needs to be generalized to one location */
#define MODULE_IO_STATUS_EC_DEFINITIONS(_) \
	_( MODULE_IO_STATUS,	EC_IO_INUSE,					   1,  "in use"						)\
	_( MODULE_IO_STATUS,	EC_IO_CORE_UNINITIALIZED,		   2,  "core uninitialized"			)\
	_( MODULE_IO_STATUS,	EC_IO_ALLOCATE_INTERRUPT_CONTEXT,  3,  "allocate context"			)\
	_( MODULE_IO_STATUS,	EC_IO_DEVICE_ALLOCATE,			   4,  "device allocate"			)\
	_( MODULE_IO_STATUS,	EC_IO_OPEN,						   5,  "open"						)\
	_( MODULE_IO_STATUS,	EC_IO_CLOSE,					   6,  "close"						)\
	_( MODULE_IO_STATUS,	EC_IO_HANDLE_NULL,				   7,  "handle null"				)\
	_( MODULE_IO_STATUS,	EC_IO_INTERRUPT_DISABLE,		   8,  "interrupt disable"			)\
	_( MODULE_IO_STATUS,	EC_IO_INTERRUPT_ENABLE,			   9,  "interrupt enable"			)\
	_( MODULE_IO_STATUS,	EC_IO_INTERRUPT_ACTIVE,			  10,  "interrupt active"			)\
	_( MODULE_IO_STATUS,	EC_IO_INTERRUPT_INACTIVE,		  11,  "interrupt inactive"			)\
	_( MODULE_IO_STATUS,	EC_IO_INTERRUPT_NOT_DEFINED,	  12,  "interrupt not defined"		)\
	_( MODULE_IO_STATUS,	EC_IO_INIT_LIB_DEBUG_ERROR,		  13,  "library debug"				)\
	_( MODULE_IO_STATUS,	EC_IO_INIT_LIB_ERROR,			  14,  "library initialize error"	)\
	_( MODULE_IO_STATUS,	EC_IO_INIT_LIB_NAME_ERROR,		  15,  "library name"				)\
	_( MODULE_IO_STATUS,	EC_IO_SYS_VERSION_ERROR,		  16,  "driver version"				)\
	_( MODULE_IO_STATUS,	EC_IO_TERMINATION_CLOSE_ERROR,	  17,  "termination close"			)\
	_( MODULE_IO_STATUS,	EC_IO_ADDRESS_RANGE,			  18,  "address out of range"		)\
	_( MODULE_IO_STATUS,	EC_IO_DEFINITION_NOT_FOUND,		  19,  "definition not found"		)\
	_( MODULE_IO_STATUS,	EC_IO_IOCTL_MAGIC_NUMBER,		  20,  "magic does not match"		)\
	_( MODULE_IO_STATUS,	EC_IO_IOCTL_LOGICAL_ID,			  21,  "logical ID does not match"	)\
	_( MODULE_IO_STATUS,	EC_IO_COPY_FROM_USER,			  22,  "copy from user"				)\
	_( MODULE_IO_STATUS,	EC_IO_COPY_TO_USER,				  23,  "copy to user"				)\
	_( MODULE_IO_STATUS,	EC_IO_IOCTL_COMMAND_UNKNOWN,	  24,  "ioctl command unknown"		)\
	_( MODULE_IO_STATUS,	EC_IO_NOT_IMPLEMENTED,			  25,  "function not implemented"	)\
	_( MODULE_IO_STATUS,	EC_IO_NOT_CAPTURED,				  26,  "not captured"				)\
	_( MODULE_IO_STATUS,	EC_IO_ADDRESS_ODD_U16,			  27,  "odd address u16"			)\
	_( MODULE_IO_STATUS,	EC_IO_OPEN_IO_DEFINITION,		  28,  "open io definition"			)\
	_( MODULE_IO_STATUS,	EC_IO_CLOSE_DEVICE_UNKNOWN,		  29,  "close device unknown"		)\
	_( MODULE_IO_STATUS,	EC_IO_OPEN_DEVICE_POINTER,		  30,  "open device pointer"		)\
	_( MODULE_IO_STATUS,	EC_IO_OPEN_NOT,					  31,  "not open"					)\
	_( MODULE_IO_STATUS,	EC_IO_READ_U8_ERROR,			  32,  "read U8"					)\
	_( MODULE_IO_STATUS,	EC_IO_READ_U16_ERROR,			  33,  "read U16"					)\
	_( MODULE_IO_STATUS,	EC_IO_READ_U16_LO_U8_ERROR,		  34,  "read U16 lo U8"				)\
	_( MODULE_IO_STATUS,	EC_IO_READ_U16_HI_U8_ERROR,		  35,  "read U16 hi U8"				)\
	_( MODULE_IO_STATUS,	EC_IO_WRITE_U8_ERROR,			  36,  "write U8"					)\
	_( MODULE_IO_STATUS,	EC_IO_WRITE_U16_ERROR,			  37,  "write U16"					)\
	_( MODULE_IO_STATUS,	EC_IO_WRITE_U16_LO_U8_ERROR,	  38,  "write U16 lo U8"			)\
	_( MODULE_IO_STATUS,	EC_IO_WRITE_U16_HI_U8_ERROR,	  39,  "write U16 hi U8"			)\
	_( MODULE_IO_STATUS,	EC_IO_REGION_REQUEST,			  40,  "region request fail"		)\
	_( MODULE_IO_STATUS,	EC_IO_OPEN_ALREADY,				  41,  "open already"				)\
	_( MODULE_IO_STATUS,	EC_IO_WARN_DEVICE_LIST_UNKNOWN,	  42,  "device list unknown"		)\
	_( MODULE_IO_STATUS,	EC_IO_WARN_DEVICE_LIST_PARTIAL,	  43,  "device list partial"		)

#endif /* MODULE_IO_STATUS */

