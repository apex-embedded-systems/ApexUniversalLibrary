#ifndef  COMPONENTS_AVAILABLE_COMMAND
#define  COMPONENTS_AVAILABLE_COMMAND

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
#define COMPONENT_AVAILABLE_CMD_H_REV  "$Date: 2015-07-31 21:59:26 -0500 (Fri, 31 Jul 2015) $"
/***************************************************************************/

#include "../components/i8255m0/cmp_i8255m0_cmd.h"
#include "../components/i8254/cmp_i8254_cmd.h"
#include "../components/indirect/cmp_indirect_cmd.h"

#ifdef COMPONENT_I8255M0_COMMAND
# define COMPONENT_AVAIL_CMD_CODE_1			COMPONENT_I8255M0_CMD_DEFINITIONS( CMD_CMP_EXTRACT_ENUM )
# define COMPONENT_AVAIL_CMD_DATASET_1		COMPONENT_I8255M0_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )
# define COMPONENT_AVAIL_CMD_BASE_ENUM_1	COMPONENT_I8255M0_CMD_DEFINITIONS( CMD_CMP_EXTRACT_BASE_ENUM )	
#else
# define COMPONENT_AVAIL_CMD_CODE_1		
# define COMPONENT_AVAIL_CMD_DATASET_1
# define COMPONENT_AVAIL_CMD_BASE_ENUM_1
#endif

#ifdef COMPONENT_I8254_COMMAND
# define COMPONENT_AVAIL_CMD_CODE_2			COMPONENT_I8254_CMD_DEFINITIONS( CMD_CMP_EXTRACT_ENUM )
# define COMPONENT_AVAIL_CMD_DATASET_2		COMPONENT_I8254_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )
# define COMPONENT_AVAIL_CMD_BASE_ENUM_2	COMPONENT_I8254_CMD_DEFINITIONS( CMD_CMP_EXTRACT_BASE_ENUM )	
#else
# define COMPONENT_AVAIL_CMD_CODE_2		
# define COMPONENT_AVAIL_CMD_DATASET_2
# define COMPONENT_AVAIL_CMD_BASE_ENUM_2
#endif

#ifdef COMPONENT_INDIRECT_COMMAND
# define COMPONENT_AVAIL_CMD_CODE_3			COMPONENT_INDIRECT_CMD_DEFINITIONS( CMD_CMP_EXTRACT_ENUM )
# define COMPONENT_AVAIL_CMD_DATASET_3		COMPONENT_INDIRECT_CMD_DEFINITIONS( CMD_DATASET_EXTRACTOR )
# define COMPONENT_AVAIL_CMD_BASE_ENUM_3	COMPONENT_INDIRECT_CMD_DEFINITIONS( CMD_CMP_EXTRACT_BASE_ENUM )	
#else
# define COMPONENT_AVAIL_CMD_CODE_3		
# define COMPONENT_AVAIL_CMD_DATASET_3
# define COMPONENT_AVAIL_CMD_BASE_ENUM_3
#endif


/*******************************************************************/

#define COMPONENT_AVAIL_CMD_CODES		\
	COMPONENT_AVAIL_CMD_CODE_1			\
	COMPONENT_AVAIL_CMD_CODE_2			\
	COMPONENT_AVAIL_CMD_CODE_3

#define COMPONENT_AVAIL_CMD_DATASET		\
	COMPONENT_AVAIL_CMD_DATASET_1		\
	COMPONENT_AVAIL_CMD_DATASET_2		\
	COMPONENT_AVAIL_CMD_DATASET_3

#define COMPONENT_AVAIL_CMD_BASE_ENUM	\
	COMPONENT_AVAIL_CMD_BASE_ENUM_1		\
	COMPONENT_AVAIL_CMD_BASE_ENUM_2		\
	COMPONENT_AVAIL_CMD_BASE_ENUM_3



/*******************************************************************************/
/* Define base commands (un-encoded commands) for components. */

enum
{
	COMPONENT_AVAIL_CMD_BASE_ENUM
};


#endif /* COMPONENTS_AVAILABLE_COMMAND */
