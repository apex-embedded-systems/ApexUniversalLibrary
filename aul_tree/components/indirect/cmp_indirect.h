#ifndef COMPONENT_INDIRECT
#define COMPONENT_INDIRECT		   21 /* component ID number */
#define COMPONENT_INDIRECT_NAME	"indirect"
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
#include "cmp_indirect_reg.h"
#include "../../cmpmgr/cmpmgr.h"


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/******************************************************************
*/
struct indirect_cfg
{
	/* set to SYS_TYPE__I8255_CFG_TYPE so that the I/O control
	   can confirm that we have the correct data 
	   structure.  Also an indication of allocation.
	*/
	SYS_TYPE	type;   /* SYS_TYPE__indirect_cfg_type */

	/* location of address register and its byte width */
	struct
	{
		size_t		offset;
		size_t		offset_lsb;
		size_t		offset_msb;
		size_t		size;		/* byte width -  1 or 2		*/
		size_t		aperture;	/* bus byte width - 1 or 2	*/
	} address;
	struct
	{
		size_t		offset;
		size_t		offset_lsb;
		size_t		offset_msb;
		size_t		size;		/* byte width - 1 or 2		*/
		size_t		aperture;	/* bus byte width - 1 or 2	*/
	} data; /* max 16-bits */
};
typedef struct indirect_cfg indirect_cfg_type;

/*******************************************************************/
#define INDIRECT_INITIALIZE_DATASET  \
	{	/* type			*/			SYS_TYPE_MAKE( indirect_dataset_type ), \
		/* cfg			*/			{ SYS_TYPE_MAKE( indirect_cfg_type ) }  \
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
#define INDIRECT_HW_SCAN_AVAILABLE(_) _( INDIRECT_BASE_ID, Indirect_Hardware_Scan )
*/

int Indirect_Create( component_type * component, size_t instance );

int Indirect_Destroy( component_type * component );

int Indirect_Config_Initialize( indirect_cfg_type * cfg );



/******************************************************************
*/
#if defined( TARGET_CHECK_BUILD )
#endif

extern component_definition_type component_indirect_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_INDIRECT */