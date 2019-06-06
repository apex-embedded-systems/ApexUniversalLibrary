#ifndef MODULE_AUL_GEN
#define MODULE_AUL_GEN       22   /* module ID number */
#define MODULE_AUL_GEN_NAME  "aul_gen"
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
/**
 *  @defgroup general_libarary Library Functions
 *  General functions used to manage access to devices as well as error handling.
 */
#include "../shared/callconv.h"
#include "../shared/device_list.h"
#include "../devices/dev_avail.h"

/***************************************************************************/
#define MODULE_AUL_GEN_REV  "$Date: 2015-01-29 06:34:32 -0600 (Thu, 29 Jan 2015) $"
/***************************************************************************/



/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/

/* Move all of these to i8254 API component level for maximum reuse 
   To make them available at the top of the library, add the AUL_DLL_API to 
   them.  Then if in Kernel, the AUL_DLL_API will become null.
*/
AUL_DLL_API int AULCALLCONV AUL_I8254_Transfer( int board, i8254_ct_xfr_type * cxfr );

AUL_DLL_API int AULCALLCONV AUL_I8254_Transfer_Array( int board, i8254_ct_xfr_array_type * cxa );

AUL_DLL_API int AULCALLCONV AUL_DO_Set( int board, int channel, BOOL value );

AUL_DLL_API int AULCALLCONV AUL_DI_Get( int board, int channel, BOOL * value );

AUL_DLL_API int AULCALLCONV AUL_DIO_Port_Write( int board, int channel, uint8_t value );

AUL_DLL_API int AULCALLCONV AUL_DIO_Port_Read( int board, int channel, uint8_t * value );

/*******************************************************************************/
#ifdef __cplusplus
}
#endif



#endif  /* MODULE_AUL_GEN */

