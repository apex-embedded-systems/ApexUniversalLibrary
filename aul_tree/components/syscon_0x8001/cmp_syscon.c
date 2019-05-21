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

#include "cmp_syscon.h"
#include "../../shared/cmd.h"
#include "../../shared/status.h"
#include "../../io/io_private.h"  /* IO_COPY_FROM_USER, IO_COPY_TO_USER */ 
#include "../../shared/error.h"
/*******************************************************************/
#define SYSCON_MODULE_REV  "$Date: 2015-02-03 06:18:04 -0600 (Tue, 03 Feb 2015) $"

/****************************************************************/
struct syscon_cfg
{
	SYS_TYPE			type;


};
typedef struct syscon_cfg syscon_cfg_type;
/****************************************************************/
struct syscon_dataset
{
	SYS_TYPE			type;
	syscon_cfg_type		cfg;

	/* hardware current state	*/
	size_t				permission;
};
typedef struct syscon_dataset syscon_dataset_type;
/*******************************************************************/
static REGISTER_TYPE syscon_register_table[] = 
{
	SYSCON_REGISTER_SET_DEFINITION( REG_EXTRACT_PARAMS )
	REG_LIST_NULL
};
/*******************************************************************/
static int SysCon_Permission_Write_Obtain( component_type * component )
{
	int		error_code = SUCCESS;
	uint8_t	permission;
	CmpMgr_Reg_Read( component, 
					 SYSCON_PERMISSION,
					 (char *) &permission, 
					 sizeof( uint8_t )
				  );
	if ( !(permission & CMP_PERMISSION_WRITE) ) error_code = EC_SYSCON_PERMISSION_WRITE;
	return error_code;
}
/*******************************************************************/
static void SysCon_Permission_Write_Restore( component_type * component )
{
	int		warn_code = SUCCESS;
	uint8_t	permission;
	CmpMgr_Reg_Write( component, 
					  SYSCON_PERMISSION,
					  (char *) &permission, 
					  sizeof( uint8_t )
				   );
	return warn_code;
}
/*******************************************************************/
static int SysCon_CMD_Power_Up(	component_type *	component,
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
static int SysCon_CMD_Power_Down(	component_type *	component,
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
static int SysCon_Physical_ID(	component_type *	component, 
								size_t				index,
								char *				buf, 
								size_t				buf_len 
							 )
{
	int			error_code;
	IOC_T		physical_id;
	uint16_t	physical_id_hardware;
	(void)	index;
	(void)	buf;
	(void)	buf_len;

	CmpMgr_Reg_Read( component, 
					 SYSCON_ID,
					 (char *) &physical_id_hardware, 
					 sizeof( uint16_t )
				  );
	physical_id = (IOC_T) physical_id_hardware;
	if ( IO_COPY_TO_USER( buf, &physical_id, sizeof( physical_id ) ) )
	{ error_code = EC_IO_COPY_TO_USER;	goto SysCon_Physical_ID_Error;	 }
	return SUCCESS;
SysCon_Physical_ID_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SYSCON_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int SysCon_Run_Set(	component_type *	component, 
							size_t				index,
							char *				buf, 
							size_t				buf_len 
						 )
{
	int			error_code;
	uint8_t		run_state;
	size_t		
	(void)	index;
	(void)	buf;
	(void)	buf_len;

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &run_state, buf, sizeof( run_state ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto SysCon_Cfg_Set_Error;	 }

	if ( SysCon_Permission_Write_Obtain() ) goto SysCon_Run_Set_Error;
	CmpMgr_Reg_Write( component, 
					  SYSCON_RUN,
					  (char *) &run_state, 
					  sizeof( uint8_t )
				  );
	SysCon_Permission_Write_Restore();
	physical_id = (IOC_T) physical_id_hardware;
	if ( IO_COPY_TO_USER( buf, &physical_id, sizeof( physical_id ) ) )
	{ error_code = EC_IO_COPY_TO_USER;	goto SysCon_Physical_ID_Error;	 }
	return SUCCESS;
SysCon_Run_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SYSCON_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int SysCon_CMD_Cfg_Set( component_type *	component,
							size_t				index,		/* indicates which DI channel to obtain */
							char *				buf, 
							size_t				buf_len 
						  )
{
	int						error_code;
	syscon_cfg_type			candidate;
	syscon_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto SysCon_Cfg_Set_Error;		}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto SysCon_Cfg_Set_Error;	 }

	ds = ( syscon_dataset_type *) component->private_data;

	if ( candidate.type != SYS_TYPE_MAKE( syscon_dataset_cfg_type ) ) 
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto SysCon_Cfg_Set_Error;	}

/* perform any additional testing/verification here */

	Mem_Copy( &(ds->cfg), &candidate, sizeof( candidate ) );

/* the data will be committed to hardware upon a restart warm/cold */

	return SUCCESS;
SysCon_Cfg_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SYSCON_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int SysCon_CMD_Cfg_Get( component_type *	component,
					 size_t				index,		/* indicates which DI channel to obtain */
					 char *				buf, 
					 size_t				buf_len 
			      )
{
	int  					error_code;
	syscon_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto SysCon_Cfg_Get_Error;	 }

	ds = ( syscon_dataset_type *) component->private_data;

	if ( IO_COPY_TO_USER( buf, &(ds->cfg), sizeof( syscon_dataset_type ) ) )
	{ error_code = EC_IO_COPY_TO_USER;		goto SysCon_Cfg_Get_Error;	 }

	return SUCCESS;
SysCon_Cfg_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SYSCON_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/******************************************************************
Description.  Updates I/O all at one time.
*/
static int SysCon_Update(	component_type *	component, 
					size_t				index,		/* not used */
					char *				buf, 
					size_t				buf_len 
				  )
{
	size_t					byte_position;
	syscon_dataset_type *	ds;
	(void)					buf;
	(void)					buf_len;

	ds = ( syscon_dataset_type * ) component->private_data;

	return SUCCESS;
}
/*******************************************************************/
static int SysCon_CMD_Restart( component_type *	component,
					 size_t				index,
					 char *				buf, 
					 size_t				buf_len 
				   )
{
	size_t						byte_position;
	size_t						operation;
	syscon_dataset_type *	ds;
	(void)						buf_len;

	if      ( NULL == buf  ) operation = RESTART_COLD;
	else if ( 0    == *buf ) operation = RESTART_COLD;
	else  	                operation = RESTART_WARM;

	ds = ( syscon_dataset_type * ) component->private_data;

	switch( operation )
	{	/*--------------------------------*/
		case RESTART_COLD:
			/* Set all configuration registers 
				1. reset all components
				2. submit config to all components
				3. run all components.

Well, this sequence not likely correct at this level.  Might be better to have
this exact sequence at an upper level, such as module.c because there we will
know which components have been inventoried.

Pass down any configuration at this point.
			*/
			break;
		/*--------------------------------*/
		case RESTART_WARM:		
			/* read all configuration registers */
			break;
		/*--------------------------------*/
		default:  /* cold or any other */
/*			 copy intialize values from config into hardware registers.  more like a reset.*/
			break;
	}

	return SUCCESS;
}
/*****************************************************************/
#include "../../shared/mem.h"
#include "../../shared/error.h"
/*****************************************************************/
int SysCon_Config_Initialize( syscon_cfg_type * cfg )
{
	Mem_Set( cfg, 0, sizeof( syscon_cfg_type ) );
	cfg->type = SYS_TYPE_MAKE( syscon_cfg_type );
	return SUCCESS;
}
/*****************************************************************/
int SysCon_Create( component_type * component, size_t instance )
{
	int						error_code;
	syscon_dataset_type *	ds;
	size_t					mem_size;
	void *					mem;

	mem_size = sizeof( syscon_dataset_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_SYSCON_DATASET_ALLOCATE;  goto SysCon_Create_Error; }
	Mem_Set( mem, 0, mem_size );
	ds = (syscon_dataset_type *) mem;
	ds->type     = SYS_TYPE_MAKE( syscon_dataset_type );
	
	error_code = SysCon_Config_Initialize( &(ds->cfg) );
	if ( error_code ) goto SysCon_Create_Error;

	component->definition   = &component_syscon_definition;
	component->physical_id  = IOC_CREATE_PHYSICAL_ID_COMPONENT( COMPONENT_SYSCON, instance );
	component->offset       = 0	   /* fill in at device level */;
	component->private_data = ds;  /* assign private data */
	component->status       = component->status | CMP_STS_BUILD(0,1,0,0,0); /* private data assigned */
	return SUCCESS;
SysCon_Create_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SYSCON_NAME, __func__, __LINE__, error_code );
	return -error_code;
} 
/*****************************************************************/
int SysCon_Destroy( component_type * component )
{
	int error_code;

	if ( NULL == component ) return SUCCESS;
	
	if ( NULL != component->definition ) 
	{ /* if it is null, we will just have to assume we are working with the 
	     correct data type.
	  */
		if ( SYS_TYPE_MAKE(component_definition_type) != component->definition->type )
		{  error_code = EC_SYSCON_COMPONENT_INVALID;	goto SysCon_Destroy_Error;  }
	}
	if ( NULL != component->private_data )
	{
		Mem_Free( NULL, &(component->private_data) );
		component->private_data = NULL;
	}
	return SUCCESS;
SysCon_Destroy_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SYSCON_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
static cmp_fnc syscon_jump_table[] =
{
	COMPONENT_SYSCON_CMD_DEFINITIONS( CMD_CMP_EXTRACT_JUMP_TABLE )
};
/*****************************************************************/
component_definition_type component_syscon_definition =
{
	SYS_TYPE_MAKE(component_definition_type),
	syscon_register_table,
	syscon_jump_table,
	SYSCON_JUMP_TABLE_QTY,
#if defined( TARGET_CHECK_BUILD )
	COMPONENT_SYSCON_NAME,
#endif
	SYSCON_MODULE_REV,
	SYSCON_COMMAND_REV,
	SYSCON_STATUS_REV
};
