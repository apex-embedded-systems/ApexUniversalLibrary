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

#include "cmpmgr.h"
#include "cmpmgr_status.h"
#include "../components/cmp_avail.h"

#include "../shared/mem.h"			 /* memory copy      */
#include "../shared/status.h"     /* error codes and APIs	*/
#include "../shared/error.h"      /* error handling			*/
/*#include "../shared/sys.h"	*/		/* 	SYS_SPIN_LOCK_TYPE	SYS_FLAGS_TYPE   */

#include "../shared/cmd_macros.h"
/*******************************************************************/
#define CMPMGR_MODULE_REV  "$Date: 2014-06-25 10:29:34 -0500 (Wed, 25 Jun 2014) $"

/*********************************************************************************/
/* if DEBUG then allow CMPMGR_DEBUG */
#define CMPMGR_DEBUG			_DEBUG
/*********************************************************************************/
#define CMPMGR_DEBUG_LOG	DEBUG_OUT


/*****************************************************************
Description
Determines how the register will be written, either direct to 
hardware layer or via a linked component (via component_link).

Purpose
Allows for additional addressing modes.  
Used by all component for any sort of I/O transactions.
*/
void CmpMgr_Reg_Write(  component_type * component,	/* the component whos register is to be written	*/
						size_t			 reg_id,		/* the specific register, i.e. register row		*/
						char *			 buf, 
						size_t		  	 buf_len 
					 )
{
	int					error_code;
	uint32_t			command;
	component_type *	cmp_xfr;
	cmp_fnc *			table;
	/* cmp_link_rw_type *	rw_fncs; */

	cmp_xfr = component->component_link; /* points to the component which contains the addressing scheme, if any */
	if ( NULL != cmp_xfr )
	{ /* this is an alternate addressing scheme */
		table = cmp_xfr->definition->jump_table;  /* look up jump table */
		command = CMD_CMP_WRITE; 
#ifdef CMPMGR_DEBUG
		if ( command >= cmp_xfr->definition->jump_table_qty ) 
		{ error_code = EC_CMPMGR_REG_WRITE; goto CmpMgr_Reg_Write_Error; }
		if ( NULL == table[command] )
		{ error_code = EC_CMPMGR_REG_WRITE; goto CmpMgr_Reg_Write_Error; }
#endif
		error_code = ( * table[command] )(	component,
											reg_id,
											buf,
											buf_len
										 );
#if(0)
		rw_fncs = (cmp_link_rw_type *) cmp_xfr->private_data;
		/* return and don't do anything if the private data is of the wrong type */
		if ( rw_fncs->type != SYS_TYPE__CMP_READ_WRITE_TYPE ) return;  /* TODO: post an error!!! */
		( * rw_fncs->reg_write )(  component,	/* the component whos register is to be written		*/
									reg_id,		/* the components register id						*/
									buf,
									buf_len
								);
#endif
	}
	else
	{ /* assume direct i/o */
		Reg_Write(  component->definition->register_table,
					component->offset,
					component->iod,
					reg_id,
					buf,
					buf_len
				 );
	}
	return;
#ifdef CMPMGR_DEBUG
CmpMgr_Reg_Write_Error:
	Error_Log( MODULE_CMP_MGR_NAME, __func__, __LINE__, error_code );
#endif
}
/******************************************************************
*/
void CmpMgr_Reg_Read(   component_type * component,	/* the component whos register is to be read		*/
						size_t			 reg_id,	/* the specific register, i.e. register row		*/
						char *			 buf, 
						size_t			 buf_len 
				    )
{
	int					error_code;
	uint32_t			command;
	component_type *	cmp_xfr;
	cmp_fnc *			table;
	/* cmp_link_rw_type *	rw_fncs; */

	cmp_xfr = component->component_link; /* points to the component which contains the addressing scheme, if any */
	if ( NULL != cmp_xfr )
	{ /* this is an alternate addressing scheme */
		table = cmp_xfr->definition->jump_table;
		command = CMD_CMP_READ;
#ifdef CMPMGR_DEBUG
		if ( command >= cmp_xfr->definition->jump_table_qty )
		{ error_code = EC_CMPMGR_REG_READ; goto CmpMgr_Reg_Read_Error; }
		if ( NULL == table[command] )
		{ error_code = EC_CMPMGR_REG_READ; goto CmpMgr_Reg_Read_Error; }
#endif
		error_code = ( * table[command] )(	component,
											reg_id,
											buf,
											buf_len
										 );
#if(0)
		rw_fncs = (cmp_link_rw_type *) cmp_xfr->private_data;
		/* return and don't do anything if the private data is of the wrong type */
		if ( rw_fncs->type != SYS_TYPE__CMP_READ_WRITE_TYPE ) return;
		( * rw_fncs->reg_read )(   component,	/* the component whos register is to be written		*/
									reg_id,		/* the compoents register id						*/
									buf,
									buf_len
							   );
#endif
	}
	else
	{ /* assume direct i/o */
		Reg_Read(   component->definition->register_table,
					component->offset,
					component->iod,
					reg_id,
					buf,
					buf_len
				  );
	}
	return;
#ifdef CMPMGR_DEBUG
CmpMgr_Reg_Read_Error:
	Error_Log( MODULE_CMP_MGR_NAME, __func__, __LINE__, error_code );
#endif
}
/******************************************************************
Description
Provided the data types match, the incoming configuration is copied
directly to the internal private_data configuration space.  This
makes the assumption that the configuration has been correctly serialized;
or in other words, it assumes padding and all will match which it typically
does in a system.
*/
int CmpMgr_Config_Set(	component_type *	component, 
						size_t				index,
						char *				buf, 
						size_t				buf_len 
					 )
{
	int			error_code;
	char *		cfg;
	size_t		len_cfg;
	/*size_t	len_buf;*/
	SYS_TYPE	type_cfg;
	SYS_TYPE	type_buf;
	/*SYS_BASE_TYPE  type_base_cfg;*/
	/* SYS_BASE_TYPE  type_base_buf; */
/*	TODO: SYS_SPIN_LOCK_TYPE		write_lock;*/
/*	TODO: SYS_FLAGS_TYPE				flags;	*/		/* required for DOS - do not remove! */
	(void)		index;
	(void)		buf;
	(void)		buf_len;

/*TODO: copy_from_user*/

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;				goto CmpMgr_Config_Set_Error;		}
	if ( NULL == component->private_data )
	{	error_code = EC_CMP_PRIVATE_DATA_NULL;		goto CmpMgr_Config_Set_Error;		}
/*TODO: assumes the configuration information immediately follows type.*/
	cfg      = (char *) component->private_data;
	cfg      = cfg + sizeof( SYS_TYPE );
	type_cfg = *((SYS_TYPE *) cfg);
	type_buf = *((SYS_TYPE *) buf);
	/*type_base_cfg = TYPE_GET_BASE_TYPE( type_cfg );*/
	/*type_base_buf = TYPE_GET_BASE_TYPE( type_buf );*/

	len_cfg = TYPE_GET_ELEMENT_SIZE( type_cfg );
	/*len_buf = TYPE_GET_ELEMENT_SIZE( type_buf );*/
	if ( type_cfg != type_buf ) 
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;		goto CmpMgr_Config_Set_Error;		}

	/* Need spin locks because the configuration data could be used
	   simultaneously by IRQs and that would not be good 
	*/
/*	TODO: write_lock.counter = 1;*/
/*	TODO: SYS_SPIN_LOCK_IRQ_SAVE( &write_lock, flags );*/
	Mem_Copy( (void *)cfg, (void *)buf, len_cfg );
/*	TODO: SYS_SPIN_UNLOCK_IRQ_RESTORE( &write_lock, flags );*/

	return SUCCESS;

CmpMgr_Config_Set_Error:
	Error_Log( MODULE_CMP_MGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/******************************************************************
*/
int CmpMgr_Config_Get(  component_type *	component,  
								size_t				index,
								char *				buf, 
								size_t				buf_len 
							)
{
	int			error_code;
	char *		cfg;
	size_t		len_cfg;
	/*size_t		len_buf;*/
	SYS_TYPE		type_cfg;
	SYS_TYPE		type_buf;
	/*SYS_BASE_TYPE  type_base_cfg;*/
	/*SYS_BASE_TYPE  type_base_buf;*/
	(void)		index;
	(void)		buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;					goto CmpMgr_Config_Get_Error;		}
	if ( NULL == component->private_data )
	{	error_code = EC_CMP_PRIVATE_DATA_NULL;		goto CmpMgr_Config_Get_Error;		}
/*TODO: assumes the configuration information immediately follows type.*/
	cfg      = (char *) component->private_data;
	cfg      = cfg + sizeof( SYS_TYPE );
	type_cfg = *((SYS_TYPE *) cfg);
	type_buf = *((SYS_TYPE *) buf);
	/*type_base_cfg = TYPE_GET_BASE_TYPE( type_cfg );*/
	/*type_base_buf = TYPE_GET_BASE_TYPE( type_buf );*/

	len_cfg = TYPE_GET_ELEMENT_SIZE( type_cfg );
	/*len_buf = TYPE_GET_ELEMENT_SIZE( type_buf );*/
	if ( type_cfg != type_buf ) 
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;		goto CmpMgr_Config_Get_Error;		}

	Mem_Copy( (void *)buf, (void *)cfg, len_cfg );
	return SUCCESS;

CmpMgr_Config_Get_Error:
	Error_Log( MODULE_CMP_MGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/******************************************************************
Description
Determine the default address space usage of a single component.
Returns address space used by component within a given address 
space (i.e. direct or indirect).
*/
void CmpMgr_Offset_Range(	component_type *	component,	/* component to test												*/
							size_t *			start,		/* returns the start address, virtual address in table*/
							size_t *			end,		/* returns the end address, virtual address in table	*/
							size_t *			length		/* returns the contiguous length from start to end		*/
						)
{
	if ( NULL == component )
	{
		*start = 0;		*end = 0;		*length = 0;		return;
	}
	Reg_Table_Offset_Range( component->definition->register_table,
							start,
							end,
							length
						  );
}
/*******************************************************************/
static int CmpMgr_Available_Match_Logical_ID( component_available_type * list, IOC_T logical_id )
{
	int index = 0;
	if ( IOC_PHYSICAL_ID_NULL == list[index].logical_id ) return CMPMGR_EC_FAILURE; /* hit null already */
	do
	{	if ( logical_id == list[index].logical_id ) return index;
		index++;
	} while ( IOC_PHYSICAL_ID_NULL != list[index].logical_id );
	return CMPMGR_EC_FAILURE;
}
/*******************************************************************/
static int CmpMgr_Inventory_Match_Physical_ID( component_inventory_type * list, IOC_T physical_id )
{
	IOC_T	pid;
	int		index = 0;
	if ( IOC_PHYSICAL_ID_NULL == list[index].logical_id ) return CMPMGR_EC_FAILURE; /* hit null already */
	if ( IOC_INVALID_LOGICAL_ID_COMPONENT( physical_id ) ) return CMPMGR_EC_FAILURE;
	do
	{	pid = IOC_CREATE_PHYSICAL_ID_COMPONENT( list[index].logical_id, list[index].instance );
		if ( physical_id == pid ) return index;
		index++;
	} while ( IOC_PHYSICAL_ID_NULL != list[index].logical_id );
	return CMPMGR_EC_FAILURE;
}
/*******************************************************************/
int CmpMgr_Component_Match_Physical_ID( component_type * list, IOC_T physical_id )
{
	int index = 0;
	if ( IOC_PHYSICAL_ID_NULL == list[index].physical_id ) return CMPMGR_EC_FAILURE; /* hit null already */
	do
	{	if ( physical_id == list[index].physical_id ) return index;
		index++;
	} while ( IOC_PHYSICAL_ID_NULL != list[index].physical_id );
	return CMPMGR_EC_FAILURE;
}
/*****************************************************************
Description

Most likely this will be used as part of the device initialization process.
According to our prototype, devices need a Load() and Init() functions.

Brackets in the parameter list to remind us that it is a null terminated list

For the component_list[] it is possible to pass a partial list.  These will be verified first
that they exist in the available_list[].  It is assumed that the component_list[] has already
been allocated; specifically, the definition, physical_id, and private data have been assigned.  
It is also assumed that the component_list[] has sufficient room for all the components 
discovered during initialization (i.e. what is in the inventory_list[]).

The inventory_list[] can be built during hardware discovery or hard-coded within the device
files.  If the inventory_list[] is null, then it is assumed that only offset and component_link
fixups are required.  These fixups can be avoided by status=CMP_STS_BUILD(1,0,0,0,0);
If inventory_list[] is null, it will only apply offset fixups to direct addressing registers
which is all that there can be.

if component_link is not null in the component_list[], then it is assumed to have been fixed up.
It will make sure that the status value is updated appropriately.


*/
int CmpMgr_List_Setup(	component_inventory_type	inventory_list[],  /* INPUT -- enumerated inventory found in h/w or predefined for device */
						size_t						direct_offset,		 /* INPUT -- base address of device													*/
						int							component_qty,		 /* INPUT  -- output quantity limit													*/
						component_type				component_list[]   /* OUTPUT -- device component list, some optionally defined.				*/
					)
{
	int		error_code;
	int		inv_index;
	int		avl_index;
	int		cmp_index;
	size_t	iampid;						/* short for inventory address mode physical ID */
	IOC_T		inv_logical_id;
	IOC_T		cmp_logical_id;
	component_type *				cmp;	/* short hand for component_list[cmp_index]		*/
	component_inventory_type * inv;  /* short hand for inventory_list[inv_index]		*/

	/* verify integrity of input parameters */
	error_code = SUCCESS;
/*	if ( NULL == component_available_list )
	{	error_code = EC_CMPMGR_SETUP_AVAILABLE_LIST;		goto CmpMgr_Device_Setup_Error;		}
	if ( NULL == inventory_list )
	{	error_code = EC_CMPMGR_SETUP_INVENTORY_LIST;		goto CmpMgr_Device_Setup_Error;		}*/
	if ( NULL == component_list ) 
	{	error_code = EC_CMPMGR_SETUP_COMPONENT_LIST;		goto CmpMgr_Device_Setup_Error;		}
	if ( 0 == component_qty )
	{	error_code = EC_CMPMGR_SETUP_COMPONENT_QTY;		goto CmpMgr_Device_Setup_Error;		}

	/* Verify that all components already specified in component_list[] are found in
	   availability list.
	*/			
	cmp_index = 0;
	cmp_logical_id = IOC_GET_LOGICAL_ID_COMPONENT( component_list[cmp_index].physical_id );
	while ( IOC_PHYSICAL_ID_NULL != cmp_logical_id )
	{
		avl_index = CmpMgr_Available_Match_Logical_ID( component_available_list, cmp_logical_id );
		if ( CMPMGR_EC_FAILURE == avl_index ) 
		{	error_code = EC_CMPMGR_SETUP_AVAILABLE;			goto CmpMgr_Device_Setup_Error;	}

		cmp_index++;
		if ( cmp_index >= component_qty )
		{	error_code = EC_CMPMGR_SETUP_COMPONENT_QTY;		goto CmpMgr_Device_Setup_Error;	}
		cmp_logical_id = IOC_GET_LOGICAL_ID_COMPONENT( component_list[cmp_index].physical_id );
	} 

	if ( NULL != inventory_list )
	{  /* At this point cmp_index is at the next available element. 
			Now, instantiate the components from inventory list into
			device component list. 
		*/
		/* verify that inventory has at least one item in the list */
		inv_index = 0;
		inv_logical_id = inventory_list[inv_index].logical_id;
		if ( IOC_PHYSICAL_ID_NULL == inv_logical_id ) 
		{	error_code = EC_CMPMGR_SETUP_INVENTORY;			goto CmpMgr_Device_Setup_Error;		}

		inv_index = 0;	
		do
		{	/* find a logical ID match between what is available and the inventory */
			avl_index = CmpMgr_Available_Match_Logical_ID( component_available_list, inv_logical_id );
			if ( CMPMGR_EC_FAILURE == avl_index ) 
			{	error_code = EC_CMPMGR_SETUP_AVAILABLE;	goto CmpMgr_Device_Setup_Error;	}
			/* map parameters into the device level component */
			cmp = &(component_list[cmp_index]);
			inv = &(inventory_list[inv_index]);
			cmp->definition   = component_available_list[avl_index].definition;
			cmp->physical_id  = IOC_CREATE_PHYSICAL_ID_COMPONENT( inv->logical_id, inv->instance );
/*TODO: check for private data already fixed up???????*/
			cmp->private_data = inv->private_data;
			cmp->status			= CMP_STS_SET_PRIVATE_DATA_FXU( cmp->status, CMP_STS_PRIVATE_DATA_FXU_ON );
			cmp->offset			= inv->offset;
/*			 fixup direct addressing
			if ( IOC_PHYSICAL_ID_NULL == inv->address_mode_physical_id )
			{  DIRECT IO
				cmp->component_link = NULL;
				cmp->offset += direct_offset;
				cmp->status  = CMP_STS_SET_OFFSET_FXU( cmp->status, CMP_STS_OFFSET_FXU_ON );
			} */
			/* NOTE: INDIRECT IO addressing will be fixed up at the end of this function */
			inv_index++;	cmp_index++;
			inv_logical_id = inventory_list[inv_index].logical_id;
			if ( cmp_index >= component_qty ) 
			{	error_code = EC_CMPMGR_SETUP_LIST_FULL;	goto CmpMgr_Device_Setup_Error;	}
		} while ( IOC_PHYSICAL_ID_NULL != inv_logical_id );
	}
	/* Post-addressing-mode-fix-ups.  This includes both direct and indirect addressing.
		Any component that did not get an offset fix up will be subject to addressing
		mode fixups.
	*/
	cmp_index = 0;
	cmp = &(component_list[cmp_index]);
	while ( IOC_PHYSICAL_ID_NULL != cmp->physical_id )
	{
		if ( NULL == cmp->component_link )
		{ /* not fixed up, so green light to go further with fix-ups */
			if ( CMP_STS_OFFSET_FXU_OFF == CMP_STS_GET_OFFSET_FXU( cmp->status ) )
			{ /* offset and address mode confirmed to not have been fixed up */
				if ( NULL == inventory_list )
				{ /* can only be direct addressing scheme */
					iampid = IOC_PHYSICAL_ID_NULL;  /* force direct addressing fix ups */
				}
				else
				{	/* find in the inventory where this component is located to obtain address mode physical id */
					inv_index = CmpMgr_Inventory_Match_Physical_ID( inventory_list, cmp->physical_id );
					iampid    = inventory_list[inv_index].address_mode_physical_id;
				}
				if ( IOC_PHYSICAL_ID_NULL == iampid )
				{ /* direct addressing */
					cmp->component_link = NULL;	
					cmp->offset += direct_offset;
				}
				else
				{ /* indirect addressing */
					/* NOTE: re-using avl_index as just a general index */
					avl_index = CmpMgr_Component_Match_Physical_ID( component_list, iampid );
					if ( CMPMGR_EC_FAILURE == avl_index ) 
					{	error_code = EC_CMPMGR_SETUP_ADDRESSING;	goto CmpMgr_Device_Setup_Error;	}
					cmp->component_link = &(component_list[avl_index]);
				}
				cmp->status  = CMP_STS_SET_OFFSET_FXU( cmp->status, CMP_STS_OFFSET_FXU_ON );
			}
		}
		else
		{ /* component_link already fixed up so lets make sure that the status follows suit */
			if ( CMP_STS_OFFSET_FXU_OFF == CMP_STS_GET_OFFSET_FXU( cmp->status ) )
			{
				cmp->status  = CMP_STS_SET_OFFSET_FXU( cmp->status, CMP_STS_OFFSET_FXU_ON );
			}
		}
		cmp_index++;
		if ( cmp_index >= component_qty )
		{	error_code = EC_CMPMGR_SETUP_LIST_FULL;	goto CmpMgr_Device_Setup_Error;	}
		cmp = &(component_list[cmp_index]);
	}
	return SUCCESS;

CmpMgr_Device_Setup_Error:
	Error_Log( MODULE_CMP_MGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/******************************************************************
*/
int CmpMgr_Command(	component_type *	component,	/* component from a device		*/
					uint32_t			command,	/* component command, vector	*/ 
					size_t				index,		/* index within component		*/
					char *				buf, 
					size_t				buf_len 
				  )
{
	int			error_code;
	cmp_fnc *	table;

	if ( command >= component->definition->jump_table_qty ) 
	{	error_code = EC_CMP_COMMAND_INVALID;		goto CmpMgr_Command_Error;		}
	
	table = component->definition->jump_table;
	if ( NULL == table[command] )
	{	error_code = EC_CMP_COMMAND_INVALID;		goto CmpMgr_Command_Error;		}

	error_code = ( * table[command] )(	component,
										index,
										buf,
										buf_len
									 );
	if ( error_code < 0 ) goto CmpMgr_Command_Error;
	return error_code;

CmpMgr_Command_Error:
	Error_Log( MODULE_CMP_MGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************
*/
int CmpMgr_Terminate( void )
{
	return SUCCESS;
}
/****************************************************************
*/
int CmpMgr_Initialize( void )
{
	return SUCCESS;
}
/*****************************************************************************************
*/
module_definition_type module_cmpmgr_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	CmpMgr_Initialize,
	CmpMgr_Terminate,
	MODULE_CMP_MGR_NAME,
	CMPMGR_MODULE_REV,
	NULL,
	CMPMGR_STATUS_REV
};
