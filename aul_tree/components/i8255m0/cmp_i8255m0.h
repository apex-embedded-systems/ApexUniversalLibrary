#ifndef COMPONENT_I8255M0
#define COMPONENT_I8255M0		   16 /* component ID number */
#define COMPONENT_I8255M0_NAME	"i8255m0"
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
#include "cmp_i8255m0_reg.h"
#include "../../cmpmgr/cmpmgr.h"


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/******************************************************************
*/
struct i8255m0_cfg
{
	/* set to SYS_TYPE__I8255_CFG_TYPE so that the I/O control
	   can confirm that we have the correct data 
	   structure.  Also an indication of allocation.
	*/
	SYS_TYPE	type;   /* SYS_TYPE__i8255m0_cfg_type */
	/* Set to true when we want all inputs/outputs to be 
	   updated one command at one time.  This reduces the
	   number of I/O cycles and speeds up processing 
	   considerably.
	*/
	BOOL		simultaneous_update;	
	/*
	*/
	I8255M0_DIRECTION_ENUM		port_directions;
	/*
		Maps logical bits to physical bits.
		address_physical = bit_map[address_logical];

TODO: remove this logical to physical mapping because it makes no sense to do it
at this layer.  It makes more sense to implement at a layer above the devices 
rather than at the component layer.
	*/
/*	uint8_t		bit_map[I8255M0_BITS_QTY];  */
	/* configuration */
	struct
	{
		uint8_t polarity;
		uint8_t init_value;
	} port[I8255M0_PORT_QTY];
};
typedef struct i8255m0_cfg i8255m0_cfg_type;

/*******************************************************************/
#define I8255M0_INITIALIZE_DATASET  \
	{	/* type			*/			SYS_TYPE_MAKE( i8255m0_dataset_type ), \
		/* cfg			*/			{ SYS_TYPE_MAKE( i8255m0_cfg_type ) }  \
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
#define I8255M0_HW_SCAN_AVAILABLE(_) _( I8255M0_BASE_ID, I8255M0_Hardware_Scan )
*/

int I8255M0_Create( component_type * component, size_t instance );

int I8255M0_Destroy( component_type * component );

int I8255M0_Config_Initialize( i8255m0_cfg_type * cfg );



/******************************************************************
*/
#if defined( TARGET_CHECK_BUILD )
#endif

extern component_definition_type component_i8255m0_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_I8255M0 */