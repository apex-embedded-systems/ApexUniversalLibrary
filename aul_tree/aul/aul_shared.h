#ifndef AUL_SHARED_H__
#define AUL_SHARED_H__
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


*********************************************************************************/

#include "../devmgr/devmgr.h"
#include "../shared/module_macros.h"


/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/

devmgr_device_type * AUL_Device_Get( int board );

int AUL_Device_To_Index( devmgr_device_type * device, int * board );

int AUL_Board( int board, devmgr_device_type ** device );

extern module_definition_type module_aul_definition;

/*******************************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************************/
#endif  /* AUL_SHARED_H__ */
