#ifndef MODULE_SHARED_H__
#define MODULE_SHARED_H__
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



/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/

size_t Module_All_Count( void );

/* TODO: do not make public as it might cause other issues??  */
const char * Module_Name( int module_id );

int Module_Initialize( void );

int Module_Terminate( void );

/*******************************************************************************/
#ifdef __cplusplus
}
#endif

/*******************************************************************************/

#endif /* MODULE_SHARED_H__ */
