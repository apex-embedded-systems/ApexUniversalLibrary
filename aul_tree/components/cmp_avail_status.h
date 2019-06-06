#ifndef  COMPONENTS_AVAILABLE_STATUS
#define  COMPONENTS_AVAILABLE_STATUS

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
#define COMPONENT_AVAILABLE_STATUS_H_REV  "$Date: 2015-07-31 21:59:26 -0500 (Fri, 31 Jul 2015) $"
/***************************************************************************/

/*******************************************************************/

#include "../components/i8255m0/cmp_i8255m0_status.h"
#include "../components/i8254/cmp_i8254_status.h"
#include "../components/indirect/cmp_indirect_status.h"

#ifdef COMPONENT_I8255M0_STATUS
# define COMPONENT_AVAIL_STATUS_CODE_1	COMPONENT_I8255M0_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR )
# define COMPONENT_AVAIL_STATUS_DS_1	COMPONENT_I8255M0_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR )
#else
# define COMPONENT_AVAIL_STATUS_CODE_1
# define COMPONENT_AVAIL_STATUS_DS_1
#endif

#ifdef COMPONENT_I8254_STATUS
# define COMPONENT_AVAIL_STATUS_CODE_2	COMPONENT_I8254_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR ) 
# define COMPONENT_AVAIL_STATUS_DS_2	COMPONENT_I8254_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR ) 
#else
# define COMPONENT_AVAIL_STATUS_CODE_2
# define COMPONENT_AVAIL_STATUS_DS_2
#endif

#ifdef COMPONENT_INDIRECT_STATUS
# define COMPONENT_AVAIL_STATUS_CODE_3	COMPONENT_INDIRECT_STATUS_EC_DEFINITIONS( ERROR_CODE_EXTRACTOR ) 
# define COMPONENT_AVAIL_STATUS_DS_3	COMPONENT_INDIRECT_STATUS_EC_DEFINITIONS( ERROR_DS_EXTRACTOR ) 
#else
# define COMPONENT_AVAIL_STATUS_CODE_3
# define COMPONENT_AVAIL_STATUS_DS_3
#endif

/*******************************************************************/

#define COMPONENT_AVAIL_STATUS_CODES	\
	COMPONENT_AVAIL_STATUS_CODE_1		\
	COMPONENT_AVAIL_STATUS_CODE_2		\
	COMPONENT_AVAIL_STATUS_CODE_3

#define COMPONENT_AVAIL_STATUS_NAMES	\
	COMPONENT_AVAIL_STATUS_DS_1			\
	COMPONENT_AVAIL_STATUS_DS_2			\
	COMPONENT_AVAIL_STATUS_DS_3




#endif /* COMPONENTS_AVAILABLE_STATUS */
