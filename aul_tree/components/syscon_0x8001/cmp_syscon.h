#ifndef COMPONENT_SYSCON
#define COMPONENT_SYSCON		   20 /* component ID number */
#define COMPONENT_SYSCON_NAME	"syscon"
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

#include "../../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../../shared/typebool.h"
#include "cmp_syscon_reg.h"
#include "../../cmpmgr/cmpmgr.h"


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
struct syscon_cfg
{
	SYS_TYPE			type;
};
typedef struct syscon_cfg syscon_cfg_type;

/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************
Description
Hardware Scanning

This is used to map in a hardware scanner during hardware
initialization that will occur during Initialize() and/or
Open().  This is a special case for components that provide
address space such as the index-data array implementation.
*/
/*
#define I8255M0_HW_SCAN_AVAILABLE(_) _( I8255M0_BASE_ID, I8255M0_Hardware_Scan )
*/

int SysCon_Create( component_type * component, size_t instance );

int SysCon_Destroy( component_type * component );


uint8_t SysCon_Control_Create( SYSCON_CHANNEL_ENUM	channel,				
							   SYSCON_RW_ENUM		rw,	
							   SYSCON_MODE_ENUM		mode,
							   SYSCON_TYPE_ENUM		bcd
							 );

/******************************************************************
*/
#if defined( TARGET_CHECK_BUILD )
#endif

extern component_definition_type component_syscon_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_SYSCON */