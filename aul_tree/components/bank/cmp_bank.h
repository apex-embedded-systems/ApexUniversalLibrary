#ifndef COMPONENT_BANK
#define COMPONENT_BANK		   31 /* component ID number */
#define COMPONENT_BANK_NAME	"bank"
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
#include "cmp_bank_reg.h"
#include "../../cmpmgr/cmpmgr.h"


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/******************************************************************
*/
struct bank_cfg
{
	/* set to SYS_TYPE__I8255_CFG_TYPE so that the I/O control
	   can confirm that we have the correct data 
	   structure.  Also an indication of allocation.
	*/
	SYS_TYPE	type;   /* SYS_TYPE__bank_cfg_type */

	/* location of bank register */
	size_t		offset;
	size_t		offset_lsb;
	size_t		offset_msb;
	size_t		size;		/* byte width -  1 or 2		*/
	size_t		aperture;	/* bus byte width - 1 or 2	*/

	/* only update the bank value when the new one is different
	 * compared to the internal copy 'bank' below.
	 */
	size_t		write_based_on_copy;  /* true / false */

	/* current bank value - specified in order to minimize the number
	 * of writes to a given bank register.
	 * TODO: make sure it is properly synchronized with the hardware!
	 */
	int			bank;
};
typedef struct bank_cfg bank_cfg_type;

/*******************************************************************/
#define BANK_INITIALIZE_DATASET  \
	{	/* type			*/			SYS_TYPE_MAKE( bank_dataset_type ), \
		/* cfg			*/			{ SYS_TYPE_MAKE( bank_cfg_type ) }  \
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
#define BANK_HW_SCAN_AVAILABLE(_) _( BANK_BASE_ID, Bank_Hardware_Scan )
*/

int Bank_Create( component_type * component, size_t instance );

int Bank_Destroy( component_type * component );

int Bank_Config_Initialize( bank_cfg_type * cfg );



/******************************************************************
*/
#if defined( TARGET_CHECK_BUILD )
#endif

extern component_definition_type component_bank_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_BANK */
