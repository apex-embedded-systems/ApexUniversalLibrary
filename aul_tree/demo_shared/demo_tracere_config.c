
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

	COPYRIGHT NOTICE Copyright (c) 2014-2015 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded
	Systems Any reproduction or dissemination is prohibited unless specifically
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


*********************************************************************************/

#include "demo_tracere_config.h"
#include "../demo_shared/demo_tracere_reports.h"
/*
#include "../aul_tree/aul/aul_public.h"
#include <stdio.h>
*/
//#include <memory.h>


/***************************************************************************/
void Demo_Tracere_Config_Clear( tracere_cfg_type * cfg )
{
	/* using library internal memory manager */
	Mem_Set( cfg, 0, sizeof( tracere_cfg_type) );
}

/***************************************************************************/
/* NOTE: must have a name other than 'Tracere_Config_Initialize' */
int Demo_Tracere_Config_Initialize( int board, char * prefix, FILE* out_file, tracere_cfg_type * cfg )
{
	int 	error_code;
	(void)	board;

#if defined( TRACERE_CONFIG_DEBUG_MESSAGE )
	fprintf( out_file, "%sConfig initialize begin:\n", prefix );
#endif
//
//	/**** interrupts  ****/
//	/* Make sure board level interrupts are disabled */
//	error_code = Tracere_Int_Enable_Set( board, false );
//	memset( &(cfg->int_cfg), 0, sizeof( STX104_INT_CFG_TYPE ) );
//	error_code = STX104_Int_CFG_Set( board, &(cfg->int_cfg) );
//	if ( SUCCESS != error_code ) goto Tracere_Config_Initialize_Error;
//#if defined( TRACERE_CONFIG_DEBUG_MESSAGE )
//	fprintf( out_file, "%s  -> Interrupt config (disable) success\n", prefix );
//#endif

	error_code = Tracere_Config_Initialize( cfg );
	if ( SUCCESS != error_code ) goto Tracere_Config_Initialize_Error;

	return SUCCESS;
Tracere_Config_Initialize_Error:
	/* report errors here - maybe something has been set incorrectly */
	fprintf( out_file, "%s  -> Error: %s\n", prefix, AUL_Error_String( error_code ) );
	return error_code;
}
/***************************************************************************/
int Demo_Tracere_Config_Termination( int board, char * prefix, FILE* out_file, tracere_cfg_type * cfg )
{
	//int 	error_code;
	(void)  cfg;
	(void)	board;
#ifndef TRACERE_CONFIG_DEBUG_MESSAGE
	(void) prefix;
	(void) out_file;
#endif

#if defined( TRACERE_CONFIG_DEBUG_MESSAGE )
	fprintf( out_file, "%sConfig termination begin:\n", prefix );
#endif
#if defined( TRACERE_USE_INTERRUPTS )
	//Tracere_Intr_Enable_Set( board, false );
# if defined( TRACERE_CONFIG_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> interrupts disabled\n", prefix );
# endif
#endif
	return SUCCESS;
}
/***************************************************************************/
/* NOTE: must have a name other than 'Tracere_Config_Get' */
int Demo_Tracere_Config_Get( int board, char * prefix, FILE* out_file, tracere_cfg_type * cfg )
{
   int 		error_code;

   error_code = Tracere_Config_Get( board, cfg );
   if ( SUCCESS != error_code ) goto Tracere_Config_Get_Error;

   return SUCCESS;

Tracere_Config_Get_Error:
	/* report errors here - maybe something has been set incorrectly */
	fprintf( out_file, "%s  -> Error: %s\n", prefix, AUL_Error_String( error_code ) );
	return error_code;
}
/***************************************************************************/
int Demo_Tracere_Config_Commit( int board, char * prefix, FILE* out_file, tracere_cfg_type * cfg )
{
	int error_code;

	error_code = Tracere_Config_Set( board, cfg );
	if ( SUCCESS != error_code ) goto Tracere_Config_Commit_Error;

//	error_code = Tracere_LED_Set( board, cfg->led );
//	if ( SUCCESS != error_code ) goto Tracere_Config_Commit_Error;
//
//	error_code = Tracere_DIO_Config_Set( board, &(cfg->dio) );
//	if ( SUCCESS != error_code ) goto Tracere_Config_Commit_Error;
//
//	error_code = Tracere_CT_Config_Set( board, &(cfg->ct) );
//	if ( SUCCESS != error_code ) goto Tracere_Config_Commit_Error;
//
//#if defined( TRACERE_CONFIG_INTERRUPTS_ACTIVE )
//	error_code = Tracere_Int_CFG_Set( board, &(cfg->int_cfg) );
//	if ( SUCCESS != error_code ) goto STX104_Config_Commit_Error;
//#endif

	/* now commit the configuration to the hardware */
	error_code = Tracere_Restart( board, TRACERE_RESTART_COLD_ALL );
	if ( SUCCESS != error_code ) goto Tracere_Config_Commit_Error;

	return SUCCESS;

Tracere_Config_Commit_Error:
	/* report errors here - maybe something has been set incorrectly */
	fprintf( out_file, "%s  -> Error: %s\n", prefix, AUL_Error_String( error_code ) );
	return error_code;
}


