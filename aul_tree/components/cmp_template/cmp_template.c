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

#include "cmp_<template>.h"
#include "../../shared/cmd.h"
#include "../../shared/status.h"
#include "../../io/io_private.h"  /* IO_COPY_FROM_USER, IO_COPY_TO_USER */ 
#include "../../shared/error.h"
/*******************************************************************/
#define <TEMPLATE>_MODULE_REV  "$Date: 2015-02-03 09:39:41 -0600 (Tue, 03 Feb 2015) $"

/****************************************************************/
struct <template>_dataset
{
	SYS_TYPE				type;   /* must always be first */
	<template>_cfg_type		cfg;	/* must always be second */

};
typedef struct <template>_dataset <template>_dataset_type;
/*******************************************************************/
static REGISTER_TYPE <template>_register_table[] = 
{
	<TEMPLATE>_REGISTER_SET_DEFINITION( REG_EXTRACT_PARAMS )
	REG_LIST_NULL
};
/*******************************************************************/
static int <Template>_CMD_Power_Up(	component_type *	component,
								size_t				index,
								char *				buf, 
								size_t				buf_len 
						 )
{
	(void)  component;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}
/*******************************************************************/
static int <Template>_CMD_Power_Down(	component_type *	component,
								size_t				index,
								char *				buf, 
								size_t				buf_len 
							 )
{
	(void)  component;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}
/*******************************************************************/
static int <Template>_CMD_Cfg_Set( component_type *	component,
							size_t				index,		/* indicates which DI channel to obtain */
							char *				buf, 
							size_t				buf_len 
						  )
{
	int						error_code;
	<template>_cfg_type		candidate;
	<template>_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto <Template>_Cfg_Set_Error;		}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto <Template>_Cfg_Set_Error;	 }

/* perform any additional testing/verification here */

	Mem_Copy( &(ds->cfg), &candidate, sizeof( candidate ) );

/* the data will be committed to hardware upon a restart warm/cold */

	return SUCCESS;
<Template>_Cfg_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_<TEMPLATE>_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int <Template>_CMD_Cfg_Get( component_type *	component,
					 size_t				index,		/* indicates which DI channel to obtain */
					 char *				buf, 
					 size_t				buf_len 
			      )
{
	int  					error_code;
	<template>_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto <Template>_Cfg_Get_Error;	 }

	ds = ( <template>_dataset_type *) component->private_data;

	if ( IO_COPY_TO_USER( buf, &(ds->cfg), sizeof( <template>_dataset_type ) ) )
	{ error_code = EC_IO_COPY_TO_USER;		goto <Template>_Cfg_Get_Error;	 }

	return SUCCESS;
<Template>_Cfg_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_<TEMPLATE>_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/*******************************************************************/
static int <Template>_CMD_Port_Get(	component_type *	component,
						size_t				index,		/* indicates which DI to obtain */
						char *				buf, 
						size_t				buf_len 
					)
{
	<template>_dataset_type *	ds;
	(void)					buf_len;

	if ( index >= <TEMPLATE>_PORT_QTY ) return -EC_<TEMPLATE>_CHANNEL_OUT_OF_RANGE;

	ds	= ( <template>_dataset_type * ) component->private_data;
/*	if ( ds->port_internal[index].direction )
	{  error, this is an input port only
		return -EC_<TEMPLATE>_ILLEGAL_REQUEST;
	}*/

	if ( false == ds->cfg.simultaneous_update )
	{
		CmpMgr_Reg_Read(	component, 
							index, /* register index or ID */
							(char *) &(ds->port_internal[index].raw), 
							<TEMPLATE>_REGISTER_SIZE_FIXED 
						);
		ds->copy[index] = ds->port_internal[index].raw ^ ds->cfg.port[index].polarity;
	}
	*buf = (char)( ds->copy[index] );
	return SUCCESS;
}

/******************************************************************
Description.  Updates I/O all at one time.
*/
static int <Template>_CMD_Update(	component_type *	component,
					size_t				index,		/* not used */
					char *				buf, 
					size_t				buf_len 
				  )
{
	size_t					byte_position;
	<template>_dataset_type *	ds;
	(void)					buf;
	(void)					buf_len;

	ds = ( <template>_dataset_type * ) component->private_data;

	for ( byte_position = 0; byte_position < <TEMPLATE>_PORT_QTY; byte_position++ )
	{
		if ( ds->port_internal[index].direction )
		{ /* output port */
			ds->port_internal[byte_position].raw = ds->copy[byte_position] ^ ds->cfg.port[byte_position].polarity;
/*
NOTE:
In this case the index is assumed to correspond one-for-one with the register ID
or index into the register list.  In this case it works.  In other cases a mapping
might be required.
*/
			CmpMgr_Reg_Write( component, 
							  byte_position,  /* register index or ID */
							  (char *) &(ds->port_internal[byte_position].raw), 
							  <TEMPLATE>_REGISTER_SIZE_FIXED 
						    );	
		}
		else
		{ /* input port */
			CmpMgr_Reg_Read( component, 
							 byte_position, /* register index or ID */
							 (char *) &(ds->port_internal[byte_position].raw), 
							 <TEMPLATE>_REGISTER_SIZE_FIXED 
						 );
			ds->copy[byte_position] = ds->port_internal[byte_position].raw ^ ds->cfg.port[byte_position].polarity;
		}
	}
	return SUCCESS;
}
/*******************************************************************/
static int <Template>_CMD_Restart( component_type *	component,
					 size_t				index,
					 char *				buf, 
					 size_t				buf_len 
				   )
{
	size_t						byte_position;
	size_t						operation;
	<template>_dataset_type *	ds;
	(void)						buf_len;

	if      ( NULL == buf  ) operation = RESTART_COLD;
	else if ( 0    == *buf ) operation = RESTART_COLD;
	else  	                operation = RESTART_WARM;

	ds = ( <template>_dataset_type * ) component->private_data;

	<Template>_Apply_Directions_Helper( ds );

	switch( operation )
	{	/*--------------------------------*/
		case RESTART_COLD:
/*TODO: test for valid configuration by observing the type value (SYS data type).*/
			CmpMgr_Reg_Write( component, <TEMPLATE>_REG_CFG, (char *)&(ds->cfg.port_directions), sizeof(uint8_t) );

			for ( byte_position = 0; byte_position < <TEMPLATE>_PORT_QTY; byte_position++ )
			{
				ds->copy[byte_position] = ds->cfg.port[byte_position].init_value;
			}
			
/*TODO: we could fix up the bit map here if it all turns out to be null??*/
			<Template>_Update( component,
								index,
								buf,
								buf_len
							  );
			break;
		/*--------------------------------*/
		case RESTART_WARM:
			break;
		/*--------------------------------*/
		default:  /* cold or any other */
/*			 copy intialize values from config into hardware registers.  more like a reset.*/
			break;
	}

	return SUCCESS;
}
/*****************************************************************/
//int <Template>_CMD_Present(	component_type *	component,
//							size_t				index,
//							char *				buf,
//							size_t				buf_len
//						  )
//{
//	return false;
//}
/*****************************************************************/
#include "../../shared/mem.h"
#include "../../shared/error.h"
/*****************************************************************/
int <Template>_Config_Initialize( <template>_cfg_type * cfg )
{
	Mem_Set( cfg, 0, sizeof( <template>_cfg_type ) );
	cfg->type = SYS_TYPE_MAKE( <template>_cfg_type );
	cfg->port_directions = <TEMPLATE>_PDIR_AIN__BIN__CIN;
	return SUCCESS;
}
/*****************************************************************/
static int <Template>_Present( component_type * component, size_t instance )
{
	(void) component;
	(void) instance;
	return false;
}
/*****************************************************************/
int <Template>_Create( component_type * component, size_t instance )
{
	int						error_code;
	<template>_dataset_type *	ds;
	size_t					mem_size;
	void *					mem;

	mem_size = sizeof( <template>_dataset_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_<TEMPLATE>_DATASET_ALLOCATE;  goto <Template>_Create_Error; }
	Mem_Set( mem, 0, mem_size );
	ds = (<template>_dataset_type *) mem;
	
	error_code = <Template>_Config_Initialize( &(ds->cfg) );
	if ( error_code ) goto <Template>_Create_Error;

	component->definition   = &component_<template>_definition;
	component->physical_id  = IOC_CREATE_PHYSICAL_ID_COMPONENT( COMPONENT_I8255M0, instance );
	component->offset       = 0	   /* fill in at device level */;
	component->private_data = ds;  /* assign private data */
	component->status       = component->status | CMP_STS_BUILD(0,1,0,0,0); /* private data assigned */

	return SUCCESS;
<Template>_Create_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_<TEMPLATE>_NAME, __func__, __LINE__, error_code );
	return -error_code;
} 

/*****************************************************************/
int <Template>_Destroy( component_type * component )
{
	int error_code;

	if ( NULL == component ) return SUCCESS;
	
	if ( NULL != component->definition ) 
	{ /* if it is null, we will just have to assume we are working with the 
	     correct data type.
	  */
		if ( SYS_TYPE_MAKE(component_definition_type) != component->definition->type )
		{  error_code = EC_<TEMPLATE>_COMPONENT_INVALID;	goto <Template>_Destroy_Error;  }
	}
	if ( NULL != component->private_data )
	{
		Mem_Free( NULL, &(component->private_data) );
		component->private_data = NULL;
	}
	return SUCCESS;
<Template>_Destroy_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_<TEMPLATE>_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
static cmp_fnc <template>_jump_table[] =
{
	COMPONENT_<TEMPLATE>_CMD_DEFINITIONS( CMD_CMP_EXTRACT_JUMP_TABLE )
};
/*****************************************************************/
component_definition_type component_<template>_definition =
{
	SYS_TYPE_MAKE(component_definition_type),
	<template>_register_table,
	<template>_jump_table,
	<TEMPLATE>_JUMP_TABLE_QTY,
#if defined( TARGET_CHECK_BUILD )
	COMPONENT_<TEMPLATE>_NAME,
#endif
	<TEMPLATE>_MODULE_REV,
	<TEMPLATE>_COMMAND_REV,
	<TEMPLATE>_STATUS_REV
};

