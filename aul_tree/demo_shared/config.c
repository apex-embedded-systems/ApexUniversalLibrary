
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

#include "config.h"
#include "../demo_shared/cfgrprt.h"
/*
#include "../aul_tree/aul/aul_public.h"
#include <stdio.h>
*/
//#include <memory.h>


/***************************************************************************/
void STX104_Config_Clear( stx104_configuration_type * cfg )
{
	/* using library internal memory manager */
	Mem_Set( cfg, 0, sizeof( stx104_configuration_type) );
}

/***************************************************************************/
int STX104_Config_Initialize( int board, char * prefix, FILE* out_file, stx104_configuration_type * cfg )
{
	int error_code;

#if defined( STX104_CONFIG_DEBUG_MESSAGE )
	fprintf( out_file, "%sConfig initialize begin:\n", prefix );
#endif
	/**** interrupts  ****/
	/* Make sure board level interrupts are disabled */
	error_code = STX104_Int_Enable_Set( board, false );
	memset( &(cfg->int_cfg), 0, sizeof( STX104_INT_CFG_TYPE ) );
	error_code = STX104_Int_CFG_Set( board, &(cfg->int_cfg) );
	if ( SUCCESS != error_code ) goto STX104_Config_Initialize_Error;
#if defined( STX104_CONFIG_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> Interrupt config (disable) success\n", prefix );
#endif
	/****  triggering   ****/
	/*   config and de-activate   */
	STX104_Trg_Software_Stop( board );
	memset( &(cfg->trg_cfg), 0, sizeof( STX104_TRG_CFG_TYPE ) );
	cfg->trg_cfg.stop_source  = ETS_SOFTWARE_STROBE; /* stop via software */
	error_code = STX104_Trg_CFG_Set( board, &(cfg->trg_cfg) );
	if ( SUCCESS != error_code ) goto STX104_Config_Initialize_Error;
	error_code = STX104_Trg_Software_Stop( board );
#if defined( STX104_CONFIG_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> Trigger config (de-activate) success\n", prefix );
#endif
	return SUCCESS;
STX104_Config_Initialize_Error:
	/* report errors here - maybe something has been set incorrectly */
	fprintf( out_file, "%s  -> Error: %s\n", prefix, AUL_Error_String( error_code ) );
	return error_code;
}
/***************************************************************************/
int STX104_Config_Termination( int board, char * prefix, FILE* out_file, stx104_configuration_type * cfg )
{
	int 		error_code;
#ifndef STX104_CONFIG_DEBUG_MESSAGE
	(void) prefix;
	(void) out_file;
#endif

#if defined( STX104_CONFIG_DEBUG_MESSAGE )
	fprintf( out_file, "%sConfig termination begin:\n", prefix );
#endif
#if defined( STX104_USE_INTERRUPTS )
	STX104_Int_Enable_Set( board, false );
# if defined( STX104_CONFIG_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> interrupts disabled\n", prefix );
# endif
#endif
	/* *** NOTE: disabling triggering here is purely optional  */
	/* triggering  - optional...just making sure triggering is really off  */
	error_code = STX104_Trg_CFG_Get( board, &(cfg->trg_cfg) );
	cfg->trg_cfg.stop_source = ETS_SOFTWARE_STROBE; /* stop via software */
	error_code = STX104_Trg_CFG_Set( board, &(cfg->trg_cfg) );
	STX104_Trg_Software_Stop( board );
#if defined( STX104_CONFIG_DEBUG_MESSAGE )
	fprintf( out_file, "%s  -> triggering stopped\n", prefix );
#endif
	return SUCCESS;
}
/***************************************************************************/
int STX104_Config_Get( int board, char * prefix, FILE* out_file, stx104_configuration_type * cfg )
{
   int 		error_code;

	error_code = STX104_AI_CFG_Get( board, &(cfg->ai_cfg) );
   if ( SUCCESS != error_code ) goto STX104_Config_Get_Error;

   error_code = STX104_DI_CFG_Get( board, &(cfg->di_cfg) );
   if ( SUCCESS != error_code ) goto STX104_Config_Get_Error;

   error_code = STX104_DO_CFG_Get( board, &(cfg->do_cfg) );
   if ( SUCCESS != error_code ) goto STX104_Config_Get_Error;

   error_code = STX104_Trg_CFG_Get( board, &(cfg->trg_cfg) );
   if ( SUCCESS != error_code ) goto STX104_Config_Get_Error;

#if defined( STX104_CONFIG_INTERRUPTS_ACTIVE )
   error_code = STX104_Int_CFG_Get( board, &(cfg->int_cfg) );
   if ( SUCCESS != error_code ) goto STX104_Config_Commit_Error;
#endif
   return SUCCESS;

   STX104_Config_Get_Error:
	/* report errors here - maybe something has been set incorrectly */
	fprintf( out_file, "%s  -> Error: %s\n", prefix, AUL_Error_String( error_code ) );
	return error_code;
}
/***************************************************************************/
int STX104_Config_Commit( int board, char * prefix, FILE* out_file, stx104_configuration_type * cfg )
{
	int error_code;

	error_code = STX104_AI_CFG_Set( board, &(cfg->ai_cfg) );
	if ( SUCCESS != error_code ) goto STX104_Config_Commit_Error;

	error_code = STX104_DI_CFG_Set( board, &(cfg->di_cfg) );
	if ( SUCCESS != error_code ) goto STX104_Config_Commit_Error;

	error_code = STX104_DO_CFG_Set( board, &(cfg->do_cfg) );
	if ( SUCCESS != error_code ) goto STX104_Config_Commit_Error;

	error_code = STX104_Trg_CFG_Set( board, &(cfg->trg_cfg) );
	if ( SUCCESS != error_code ) goto STX104_Config_Commit_Error;

#if defined( STX104_CONFIG_INTERRUPTS_ACTIVE )
	error_code = STX104_Int_CFG_Set( board, &(cfg->int_cfg) );
	if ( SUCCESS != error_code ) goto STX104_Config_Commit_Error;
#endif
	return SUCCESS;

STX104_Config_Commit_Error:
	/* report errors here - maybe something has been set incorrectly */
	fprintf( out_file, "%s  -> Error: %s\n", prefix, AUL_Error_String( error_code ) );
	return error_code;
}


