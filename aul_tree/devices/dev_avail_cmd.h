#ifndef  DEVICES_AVAILABLE_COMMAND
#define  DEVICES_AVAILABLE_COMMAND

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
#define DEVICE_AVAILABLE_CMD_H_REV  "$Date: 2014-06-11 11:54:32 -0500 (Wed, 11 Jun 2014) $"
/***************************************************************************/

/*******************************************************************/
#include "../devices/tracere/dev_tracere_cmd.h"
#include "../devices/stx104/dev_stx104_cmd.h"

#ifdef DEVICE_TRACERE_COMMAND
# define DEVICE_AVAIL_CMD_CODE_1		DEVICE_TRACERE_CMD_DEFINITIONS( CMD_DEV_EXTRACT_ENUM )
# define DEVICE_AVAIL_CMD_DATASET_1		DEVICE_TRACERE_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )
# define DEVICE_AVAIL_CMD_BASE_ENUM_1	DEVICE_TRACERE_CMD_DEFINITIONS( CMD_DEV_EXTRACT_BASE_ENUM )	
#else
# define DEVICE_AVAIL_CMD_CODE_1		
# define DEVICE_AVAIL_CMD_DATASET_1
# define DEVICE_AVAIL_CMD_BASE_ENUM_1
#endif

#ifdef DEVICE_STX104_COMMAND
# define DEVICE_AVAIL_CMD_CODE_2		DEVICE_STX104_CMD_DEFINITIONS( CMD_DEV_EXTRACT_ENUM )
# define DEVICE_AVAIL_CMD_DATASET_2		DEVICE_STX104_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )
# define DEVICE_AVAIL_CMD_BASE_ENUM_2	DEVICE_STX104_CMD_DEFINITIONS( CMD_DEV_EXTRACT_BASE_ENUM )	
#else
# define DEVICE_AVAIL_CMD_CODE_2		
# define DEVICE_AVAIL_CMD_DATASET_2
# define DEVICE_AVAIL_CMD_BASE_ENUM_2
#endif

/*******************************************************************/

#define DEVICE_AVAIL_CMD_CODES	\
	DEVICE_AVAIL_CMD_CODE_1		\
	DEVICE_AVAIL_CMD_CODE_2	

#define DEVICE_AVAIL_CMD_DATASET	\
	DEVICE_AVAIL_CMD_DATASET_1		\
	DEVICE_AVAIL_CMD_DATASET_2

#define DEVICE_AVAIL_CMD_BASE_ENUM	\
	DEVICE_AVAIL_CMD_BASE_ENUM_1	\
	DEVICE_AVAIL_CMD_BASE_ENUM_2

/*******************************************************************/
#if(0)
#define DEVICE_AVAIL_CMD_CODES	\
	DEVICE_TRACERE_CMD_DEFINITIONS( CMD_DEV_EXTRACT_ENUM ) \
	DEVICE_STX104_CMD_DEFINITIONS( CMD_DEV_EXTRACT_ENUM )

#define DEVICE_AVAIL_CMD_DATASET							\
	DEVICE_TRACERE_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )	\
	DEVICE_STX104_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )
#endif

/*******************************************************************************/
/* Define base commands (un-encoded commands) for components. */

enum
{
	DEVICE_AVAIL_CMD_BASE_ENUM
};

#if(0)
#define DEVICE_AVAIL_CMD_BASE_CODES											\
enum																					\
{																						\
	DEVICE_TRACERE_CMD_DEFINITIONS( CMD_DEV_EXTRACT_BASE_ENUM )		\
	DEVICE_STX104_CMD_DEFINITIONS( CMD_DEV_EXTRACT_BASE_ENUM )		\
};
#endif

#endif /* DEVICES_AVAILABLE_COMMAND */
