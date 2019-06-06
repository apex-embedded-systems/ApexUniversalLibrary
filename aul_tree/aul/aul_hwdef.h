#ifndef MODULE_AULHWDEF
#define MODULE_AULHWDEF       14   /* module ID number */
#define MODULE_AULHWDEF_NAME  "aul_hwdef"
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


/*******************************************************************************/
/*#include "../shared/sysval.h" */
#include "../shared/cmd_macros.h"
#include "../shared/module_macros.h"

/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/********************************************************************************/

int AULHwDef_Device_Populate_Definitions( const char * device_definition_file_name );

int AULHwDef_IsValid( void * hwdef );

int AULHwDef_List_Is_Populated( void );

int AULHwDef_Acronym_Find( const char * acronym, size_t * minor_number, void ** hwdef );

int AULHwDef_Parameters_Get(	void *	hwdef,
								IOC_T  * logical_id_out,
								size_t * region_offset_out,
								size_t * region_bytes_out,
								size_t * interrupt_number_out,
								size_t * region_restrict_8bit_out
									);

int AULHwDef_Parameters_Using_Index_Get(	size_t	 index,
								char *   acronym,
								IOC_T *	 logical_id_out,
								size_t * region_offset_out,
								size_t * region_bytes_out,
								size_t * interrupt_number_out,
								size_t * region_restrict_8bit_out
							);

int AULHwDef_Terminate( void );
int AULHwDef_Initialize( void );

extern module_definition_type module_aulhwdef_definition;

/*******************************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************************/

#endif  /* MODULE_AULHWDEF */

