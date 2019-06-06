#ifndef  DEVICES_AVAILABLE_STATUS
#define  DEVICES_AVAILABLE_STATUS

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

/***************************************************************************/
#define DEVICE_AVAILABLE_STATUS_H_REV  "$Date: 2014-06-11 11:54:32 -0500 (Wed, 11 Jun 2014) $"
/***************************************************************************/

/*******************************************************************/
#include "../devices/tracere/dev_tracere_status.h"
#include "../devices/stx104/dev_stx104_status.h"

#ifdef DEVICE_TRACERE_STATUS
# define DEVICE_AVAIL_STATUS_CODE_1	DEVICE_TRACERE_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
# define DEVICE_AVAIL_STATUS_DS_1	DEVICE_TRACERE_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#else
# define DEVICE_AVAIL_STATUS_CODE_1
# define DEVICE_AVAIL_STATUS_DS_1
#endif

#ifdef DEVICE_STX104_STATUS
# define DEVICE_AVAIL_STATUS_CODE_2	DEVICE_STX104_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR ) 
# define DEVICE_AVAIL_STATUS_DS_2	DEVICE_STX104_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR ) 
#else
# define DEVICE_AVAIL_STATUS_CODE_2
# define DEVICE_AVAIL_STATUS_DS_2
#endif

/*******************************************************************/

#define DEVICE_AVAIL_STATUS_CODES	\
	DEVICE_AVAIL_STATUS_CODE_1		\
	DEVICE_AVAIL_STATUS_CODE_2

#define DEVICE_AVAIL_STATUS_NAMES	\
	DEVICE_AVAIL_STATUS_DS_1		\
	DEVICE_AVAIL_STATUS_DS_2

#if(0)
#define DEVICE_AVAIL_STATUS_CODES											\
	DEVICE_TRACERE_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )	\
	DEVICE_STX104_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )

/*******************************************************************/
#define DEVICE_AVAIL_STATUS_NAMES										\
	DEVICE_TRACERE_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )	\
	DEVICE_STX104_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR ) 
#endif

#endif /* DEVICES_AVAILABLE_STATUS */