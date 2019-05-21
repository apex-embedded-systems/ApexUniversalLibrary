#ifndef COMPONENT_<TEMPLATE>
#define COMPONENT_<TEMPLATE>		   16 /* component ID number */
#define COMPONENT_<TEMPLATE>_NAME	"<template>"
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
#include "cmp_<template>_reg.h"
#include "../../cmpmgr/cmpmgr.h"


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/******************************************************************
*/
struct <template>_cfg
{
	/* set to SYS_TYPE__I8255_CFG_TYPE so that the I/O control
	   can confirm that we have the correct data 
	   structure.  Also an indication of allocation.
	*/
	SYS_TYPE	type;   /* SYS_TYPE__<template>_cfg_type */
	/* Set to true when we want all inputs/outputs to be 
	   updated one command at one time.  This reduces the
	   number of I/O cycles and speeds up processing 
	   considerably.
	*/
	BOOL		simultaneous_update;	

};
typedef struct <template>_cfg <template>_cfg_type;

/*******************************************************************/
#define <TEMPLATE>_INITIALIZE_DATASET  \
	{	/* type			*/			SYS_TYPE_MAKE( <template>_dataset_type ), \
		/* cfg			*/			{ SYS_TYPE_MAKE( <template>_cfg_type ) }  \
	}

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
#define <TEMPLATE>_HW_SCAN_AVAILABLE(_) _( <TEMPLATE>_BASE_ID, <TEMPLATE>_Hardware_Scan )
*/

int <Template>_Create( component_type * component, size_t instance );

int <Template>_Destroy( component_type * component );

int <Template>_Config_Initialize( <template>_cfg_type * cfg );



/******************************************************************
*/
#if defined( TARGET_CHECK_BUILD )
#endif

extern component_definition_type component_<template>_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_<TEMPLATE> */