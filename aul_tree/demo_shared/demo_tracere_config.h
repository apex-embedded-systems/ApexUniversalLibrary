#ifndef _DEMO_TRACERE_CONFIG_H_
#define _DEMO_TRACERE_CONFIG_H_
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

	COPYRIGHT NOTICE Copyright (c) 2014-15 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded
	Systems Any reproduction or dissemination is prohibited unless specifically
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


*********************************************************************************/

#include "../aul/aul_public.h"
#include <stdio.h>
/***************************************************************************/
/* #define TRACERE_CONFIG_INTERRUPTS_ACTIVE  1 */
#define TRACERE_CONFIG_DEBUG_MESSAGE      _DEBUG

/***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
void Demo_Tracere_Config_Clear( tracere_cfg_type * cfg );

/* NOTE: must have a name other than 'Tracere_Config_Initialize' */
int Demo_Tracere_Config_Initialize( int board, char * prefix, FILE * out_file, tracere_cfg_type * cfg );

int Demo_Tracere_Config_Termination( int board, char * prefix, FILE * out_file, tracere_cfg_type * cfg );

/* NOTE: must have a name other than 'Tracere_Config_Get' */
int Demo_Tracere_Config_Get( int board, char * prefix, FILE * out_file, tracere_cfg_type * cfg );

int Demo_Tracere_Config_Commit( int board, char * prefix, FILE * out_file, tracere_cfg_type * cfg );


/***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif  /* _DEMO_TRACERE_CONFIG_H_ */

