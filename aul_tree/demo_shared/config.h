#ifndef _CONFIG_H_
#define _CONFIG_H_
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

#include "../aul/aul_public.h"
#include <stdio.h>
/***************************************************************************/
/* #define STX104_CONFIG_INTERRUPTS_ACTIVE  1 */
#define STX104_CONFIG_DEBUG_MESSAGE      _DEBUG
/***************************************************************************/
struct stx104_configuration
{
	stx104_ai_cfg_type  	ai_cfg; 	/* analog inputs 						*/
	STX104_DI_CFG_TYPE  	di_cfg;     /* digital inputs						*/
	stx104_do_cfg_type  	do_cfg;     /* digital outputs						*/
	STX104_TRG_CFG_TYPE 	trg_cfg;    /* triggering							*/
	STX104_INT_CFG_TYPE 	int_cfg;    /* interrupts           				*/
	const char *         	name_interrupt_callback_a; /* used for reporting 	*/
	const char *         	name_interrupt_data_set_a; /* used for reporting 	*/
};
typedef struct stx104_configuration stx104_configuration_type;

/***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
void STX104_Config_Clear( stx104_configuration_type * cfg );

int STX104_Config_Initialize( int board, char * prefix, FILE * out_file, stx104_configuration_type * cfg );

int STX104_Config_Termination( int board, char * prefix, FILE * out_file, stx104_configuration_type * cfg );

int STX104_Config_Get( int board, char * prefix, FILE * out_file, stx104_configuration_type * cfg );

int STX104_Config_Commit( int board, char * prefix, FILE * out_file, stx104_configuration_type * cfg );


/***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif  /* _CONFIG_H_ */

