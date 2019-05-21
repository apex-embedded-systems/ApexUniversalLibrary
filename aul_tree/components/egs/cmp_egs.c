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
    
	COPYRIGHT NOTICE Copyright (c) 2015 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded 
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


*********************************************************************************/

#include "cmp_egs.h"
#include "../../shared/cmd.h"
#include "../../shared/status.h"
#include "../../io/io_private.h"  /* IO_COPY_FROM_USER, IO_COPY_TO_USER */ 
#include "../../shared/error.h"

#include "../../shared/debug.h"-

/*******************************************************************/
#define EGS_MODULE_REV  "$Date: 2016-09-29 16:57:39 -0500 (Thu, 29 Sep 2016) $"

//prototype
static int EGS_Config_To_Raw( 	egs_cfg_type * 		cfg, /* inbound configuration 	*/
								egs_cfg_reg_type * 	regs /* outbound registers 		*/
							 );
/****************************************************************/
struct egs_dataset
{
	SYS_TYPE			type;   /* must always be first */
	egs_cfg_type		cfg;	/* must always be second */

	/* raw configuration register values */
	egs_cfg_reg_type	cfg_reg; /* configuration registers */

	/* working copy of ports - used during update function */


};
typedef struct egs_dataset egs_dataset_type;
/*******************************************************************/
static REGISTER_TYPE egs_register_table[] = 
{
	EGS_REGISTER_SET_DEFINITION( REG_EXTRACT_PARAMS )
	REG_LIST_NULL
};
/*******************************************************************/
static int EGS_CMD_Power_Up(	component_type *	component,
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
static int EGS_CMD_Power_Down(	component_type *	component,
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
static int EGS_CMD_Cfg_Set( component_type *	component,
							size_t				index,		/* indicates which DI channel to obtain */
							char *				buf, 
							size_t				buf_len 
						  )
{
	int						error_code;
	egs_cfg_type			candidate;
	egs_cfg_reg_type		regs;      /* temporary, used to further test the integrity of data */
	egs_dataset_type *		ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto EGS_Cfg_Set_Error;		}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto EGS_Cfg_Set_Error;	 }

	ds = ( egs_dataset_type *) component->private_data;

	if ( candidate.type != ds->cfg.type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto EGS_Cfg_Set_Error;	}

	/* perform any additional testing/verification here */
	if ( candidate.ai_intrasample_time_sec < EGS_AIBT_MIN_SEC )
	{	error_code = EC_EGS_AI_INTRASAMPLE_RANGE;	goto EGS_Cfg_Set_Error;	}
	if ( candidate.ai_intrasample_time_sec > EGS_AIBT_MAX_SEC )
	{	error_code = EC_EGS_AI_INTRASAMPLE_RANGE;	goto EGS_Cfg_Set_Error;	}
	/* Confirm any detail related problems and build out registers.
	 * We will not commit the data just yet.
	 */
	error_code = EGS_Config_To_Raw( &candidate, &regs );

	/* save a copy to internal dataset */
	Mem_Copy( &(ds->cfg), &candidate, sizeof( candidate ) );

	/* the data will be committed to hardware upon a restart warm/cold */

	return SUCCESS;
EGS_Cfg_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_EGS_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int EGS_CMD_Cfg_Get( component_type *	component,
							size_t				index,		/* indicates which DI channel to obtain */
							char *				buf,
							size_t				buf_len
			      	  )
{
	int  					error_code;
	egs_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto EGS_Cfg_Get_Error;	 }

	ds = ( egs_dataset_type *) component->private_data;

	if ( IO_COPY_TO_USER( buf, &(ds->cfg), sizeof( egs_dataset_type ) ) )
	{ error_code = EC_IO_COPY_TO_USER;		goto EGS_Cfg_Get_Error;	 }

	return SUCCESS;
EGS_Cfg_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_EGS_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/*******************************************************************/
static int EGS_CMD_Port_Get(	component_type *	component,
							size_t				index,		/* indicates which DI to obtain */
							char *				buf,
							size_t				buf_len
						)
{
	egs_dataset_type *	ds;
	(void)					buf_len;

	//if ( index >= EGS_PORT_QTY ) return -EC_EGS_CHANNEL_OUT_OF_RANGE;

	ds	= ( egs_dataset_type * ) component->private_data;
/*	if ( ds->port_internal[index].direction )
	{  error, this is an input port only
		return -EC_EGS_ILLEGAL_REQUEST;
	}*/

	if ( false == ds->cfg.simultaneous_update )
	{

	}
	return SUCCESS;
}
/******************************************************************
*/
static int EGS_Config_To_Raw( 	egs_cfg_type * 		cfg, /* inbound configuration 	*/
								egs_cfg_reg_type * 	regs /* outbound registers 		*/
							 )
{
	regs->general_control  = 0x00;
	regs->general_control |= ( ( (uint8_t) cfg->logic_count_multipler ) & 0x03 ) 	<< 6;
	regs->general_control |= ( cfg->allow_fifo_fill ? 1 : 0 ) 						<< 4;
	regs->general_control |= ( cfg->pulse_counter_will_generate_buf1_to_buf2_transfers ? 1 : 0 ) << 3;
	regs->general_control |= ( cfg->pulse_counter_interrupt_enable ? 1 : 0 ) 		<< 2;
	regs->general_control |= ( cfg->encoder_direction    ? 1 : 0 ) 					<< 1;
	regs->general_control |= ( cfg->enable_pulse_counter ? 1 : 0 );

	regs->pulse_prescale_load = cfg->pulse_prescale_load;
	regs->position_counter    = cfg->position_counter;

	/* AIBT = ( Time_Between_Samples_ns - 5000ns ) / ( 25 ns )  */
	if ( cfg->ai_intrasample_time_sec > 0 )
	{
		regs->aibt = (uint16_t)( ( cfg->ai_intrasample_time_sec - 5.0e-9 ) / 25.0e-9 );
	}
	else
	{
		regs->aibt = (uint16_t)( ( cfg->ai_intrasample_time_ns - 5000 ) / 25 );
	}
	return SUCCESS;
}
/******************************************************************
*/
static int EGS_Config_From_Raw(	egs_cfg_type * 		cfg, /* outbound configuration 	*/
								egs_cfg_reg_type * 	regs /* inbound  registers 		*/
							  )
{
	cfg->logic_count_multipler 			= (enum egc_lcm)(( regs->general_control >> 6 ) & 0x03 );

	cfg->allow_fifo_fill                = ( ( regs->general_control >> 4 ) & 0x01 ) ? true : false;
	cfg->pulse_counter_will_generate_buf1_to_buf2_transfers = ( ( regs->general_control >> 3 ) & 0x01 ) ? true : false;
	cfg->pulse_counter_interrupt_enable = ( ( regs->general_control >> 2 ) & 0x01 ) ? true : false;
	cfg->encoder_direction              = ( ( regs->general_control >> 1 ) & 0x01 ) ? true : false;
	cfg->enable_pulse_counter           = (   regs->general_control        & 0x01 ) ? true : false;

	cfg->pulse_prescale_load = regs->pulse_prescale_load;
	cfg->position_counter    = regs->position_counter;

	/* AIBT = ( Time_Between_Samples_ns - 5000ns ) / ( 25 ns )
	 *   AIBT * ( 25 ns ) = ( Time_Between_Samples_ns - 5000ns )
	 *   Time_Between_Samples_ns = AIBT * ( 25 ns ) - 5000ns
	 */
	cfg->ai_intrasample_time_ns = (int)( ( (int) regs->aibt ) * 25 - 5000 );
	cfg->ai_intrasample_time_sec = (double)( cfg->ai_intrasample_time_ns * 1.0e-9 );

	return SUCCESS;
}

/******************************************************************
Description.  Updates I/O all at one time.
*/
static int EGS_CMD_Update(	component_type *	component,
						size_t				index,		/* not used */
						char *				buf,
						size_t				buf_len
				  	  )
{
	size_t					byte_position;
	egs_dataset_type *	ds;
	(void)					buf;
	(void)					buf_len;

	ds = ( egs_dataset_type * ) component->private_data;

//	for ( byte_position = 0; byte_position < EGS_PORT_QTY; byte_position++ )
//	{
//		if ( ds->port_internal[index].direction )
//		{ /* output port */
//			ds->port_internal[byte_position].raw = ds->copy[byte_position] ^ ds->cfg.port[byte_position].polarity;
///*
//NOTE:
//In this case the index is assumed to correspond one-for-one with the register ID
//or index into the register list.  In this case it works.  In other cases a mapping
//might be required.
//*/
//			CmpMgr_Reg_Write( component,
//							  byte_position,  /* register index or ID */
//							  (char *) &(ds->port_internal[byte_position].raw),
//							  EGS_REGISTER_SIZE_FIXED
//						    );
//		}
//		else
//		{ /* input port */
//			CmpMgr_Reg_Read( component,
//							 byte_position, /* register index or ID */
//							 (char *) &(ds->port_internal[byte_position].raw),
//							 EGS_REGISTER_SIZE_FIXED
//						 );
//			ds->copy[byte_position] = ds->port_internal[byte_position].raw ^ ds->cfg.port[byte_position].polarity;
//		}
//	}
	return SUCCESS;
}
/*******************************************************************/
static int EGS_CMD_Restart(  component_type *	component,
					 	 	 size_t				index,
					 	 	 char *				buf,
					 	 	 size_t				buf_len
				   	   )
{
	size_t				byte_position;
	size_t				operation;
	egs_dataset_type *	ds;
	(void)				buf_len;

	if      ( NULL == buf  ) operation = RESTART_COLD;
	else if ( 0    == *buf ) operation = RESTART_COLD;
	else  	                 operation = RESTART_WARM;

	ds = ( egs_dataset_type * ) component->private_data;

	switch( operation )
	{	/*--------------------------------*/
		case RESTART_COLD:
			/* We are assuming a cold start with the hardware */
			/* Compute the configuration registers */
			EGS_Config_To_Raw( &(ds->cfg), &(ds->cfg_reg) );
			/* commit computed registers to the hardware */
			EGS_Config_Register_Commit( component, &(ds->cfg_reg) );
			
/*TODO: we could fix up the bit map here if it all turns out to be null??*/
			EGS_CMD_Update( component,
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
#include "../../shared/mem.h"
#include "../../shared/error.h"
/*****************************************************************/
int EGS_Config_Initialize( egs_cfg_type * cfg )
{
	Mem_Set( cfg, 0, sizeof( egs_cfg_type ) );
	cfg->type = SYS_TYPE_MAKE( egs_cfg_type );
	return SUCCESS;
}
/*****************************************************************/
static int EGS_Probe( component_type * component, size_t instance )
{
	int				error_code;
	uint8_t			value_8b;
	uint16_t		value_16b;
	uint8_t			pattern[] = { 0x55, 0xAA };
	io_definition_type   io_definition_local;
	(void) 			component;
	(void) 			instance;

	error_code = IO_Ioctl( component->iod, CMD_IO_DEFINITION_GET, (unsigned long)((void *) &io_definition_local)  );

	value_8b = pattern[0];
	CmpMgr_Reg_Write( 	component,
						EGS_AI_INTRASAMPLE_TIMER_LSB,
						(char *) &value_8b,
						sizeof( value_8b )
					);
	value_8b = pattern[1];
	CmpMgr_Reg_Write( 	component,
						EGS_AI_INTRASAMPLE_TIMER_MSB,
						(char *) &value_8b,
						sizeof( value_8b )
					);

	CmpMgr_Reg_Read(	component,
						EGS_AI_INTRASAMPLE_TIMER_LSB,
						(char *) &value_8b,
						sizeof( value_8b )
					);
	if ( pattern[0] != value_8b )
	{ error_code = EC_EGS_PROBE;   goto EGS_Probe_Error;  }

	CmpMgr_Reg_Read(	component,
						EGS_AI_INTRASAMPLE_TIMER_MSB,
						(char *) &value_8b,
						sizeof( value_8b )
					);
	if ( pattern[1] != value_8b )
	{ error_code = EC_EGS_PROBE;   goto EGS_Probe_Error;  }

	return SUCCESS;
EGS_Probe_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_EGS_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/*****************************************************************/
int EGS_Create( component_type * component, size_t instance )
{
	int						error_code;
	egs_dataset_type *		ds;
	size_t					mem_size;
	void *					mem;





	component->physical_id  = IOC_CREATE_PHYSICAL_ID_COMPONENT( COMPONENT_EGS, instance );
	component->definition   = &component_egs_definition;

	//Debug_Log( DEBUG_OUT "EGS_Create. component->definition = %p\n", component->definition );

	error_code = EGS_Probe( component, instance );
	if ( error_code ) goto EGS_Create_Error;

/* already assigned:	component->offset       = 0	    */

	mem_size = sizeof( egs_dataset_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_EGS_DATASET_ALLOCATE;  goto EGS_Create_Error; }
	Mem_Set( mem, 0, mem_size );
	ds = (egs_dataset_type *) mem;
	ds->type = SYS_TYPE_MAKE( egs_dataset_type );
	
	component->private_data = ds;  /* assign private data */
	component->status       = component->status | CMP_STS_BUILD(0,1,0,0,0); /* private data assigned */

	error_code = EGS_Config_Initialize( &(ds->cfg) );
	if ( error_code ) goto EGS_Create_Error;

/* NOTE: this might be used here to guard???	component->definition   = &component_egs_definition;       */

	return SUCCESS;
EGS_Create_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_EGS_NAME, __func__, __LINE__, error_code );
	return -error_code;
} 
/*****************************************************************/
int EGS_Destroy( component_type * component )
{
	int error_code;

	if ( NULL == component ) return SUCCESS;
	
	if ( NULL != component->definition ) 
	{ /* if it is null, we will just have to assume we are working with the 
	     correct data type.
	  */
		if ( SYS_TYPE_MAKE(component_definition_type) != component->definition->type )
		{  error_code = EC_EGS_COMPONENT_INVALID;	goto EGS_Destroy_Error;  }
	}
	if ( NULL != component->private_data )
	{
		Mem_Free( NULL, &(component->private_data) );
		component->private_data = NULL;
	}
	return SUCCESS;
EGS_Destroy_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_EGS_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
static cmp_fnc egs_jump_table[] =
{
	COMPONENT_EGS_CMD_DEFINITIONS( CMD_CMP_EXTRACT_JUMP_TABLE )
};
/*****************************************************************/
component_definition_type component_egs_definition =
{
	SYS_TYPE_MAKE(component_definition_type),
	egs_register_table,
	egs_jump_table,
	EGS_JUMP_TABLE_QTY,
#if defined( TARGET_CHECK_BUILD )
	COMPONENT_EGS_NAME,
#endif
	EGS_MODULE_REV,
	EGS_COMMAND_REV,
	EGS_STATUS_REV
};

