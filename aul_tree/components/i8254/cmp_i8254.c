/********************************************************************************
_____________________________________________________________________________  
Description:

_____________________________________________________________________________
Purpose:

_____________________________________________________________________________
Remark:

http://www.intel.com/design/archives/periphrl/index.htm
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

#include "cmp_i8254.h"
#include "../../shared/cmd.h"
#include "../../shared/status.h"
#include "../../io/io_private.h"  /* IO_COPY_FROM_USER, IO_COPY_TO_USER */ 
#include "../../shared/error.h"
/*******************************************************************/
#define I8254_MODULE_REV  "$Date: 2015-07-31 21:59:26 -0500 (Fri, 31 Jul 2015) $"

/****************************************************************/
struct i8254_dataset
{
	SYS_TYPE			type;   /* must always be first  */
	i8254_cfg_type		cfg;	/* must always be second */
#if(0)
	struct
	{
		I8254_RW_ENUM rw;	
	} ct[I8254_CT_QTY];
#endif
};
typedef struct i8254_dataset i8254_dataset_type;
/*******************************************************************/
static REGISTER_TYPE i8254_register_table[] = 
{
	I8254_REGISTER_SET_DEFINITION( REG_EXTRACT_PARAMS )
	REG_LIST_NULL
};

/*******************************************************************/
static int I8254_CMD_Power_Up(	component_type *	component, 
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
static int I8254_CMD_Power_Down(	component_type *	component, 
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


/*TODO: part of capabilities listing*/
#define I8254_CAPABILITY_CT_CHAN   I8254_CT_QTY

/* if enabled, it skips the register width calculations as it is always the same -- 1 byte. */
#define I8254_REGISTER_SIZE_FIXED   1


/*******************************************************************/
static int I8254_CMD_Cfg_Set(	component_type *	component, 
								size_t				index,		/* ignored */
								char *				buf, 
								size_t				buf_len 
							)
{
	int						error_code;
	size_t					chan;
	i8254_cfg_type			candidate;
	i8254_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto I8254_Cfg_Set_Error;		}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto I8254_Cfg_Set_Error;	 }

	ds = ( i8254_dataset_type *) component->private_data;

	if ( SYS_TYPE_MAKE( i8254_cfg_type ) != candidate.type ) 
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto I8254_Cfg_Set_Error;	}

	/* perform any additional testing/verification here */

	/* verify that the configuration cxfr information is of correct form */
	for ( chan = 0; chan < I8254_CT_QTY; chan++ )
	{
		if ( SYS_TYPE_MAKE( i8254_ct_xfr_type ) != candidate.cxfr[chan].type )
		{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto I8254_Cfg_Set_Error;	}
		/* readback commands not allowed here */
		if ( I8254_CHANNEL_READBACK == I8254_CTRL_GET_SC( candidate.cxfr[chan].control ) )
		{	error_code = EC_I8254_CFG_CXFR_COMMAND;			goto I8254_Cfg_Set_Error;	}
		/* read latch commands not allowed here */
		if ( I8254_RW_LATCH         == I8254_CTRL_GET_RW( candidate.cxfr[chan].control ) )
		{	error_code = EC_I8254_CFG_CXFR_COMMAND;			goto I8254_Cfg_Set_Error;	}
	}

	Mem_Copy( &(ds->cfg), &candidate, sizeof( i8254_cfg_type ) );

/* the data will be committed to hardware upon a restart warm/cold */

	return SUCCESS;
I8254_Cfg_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int I8254_CMD_Cfg_Get(	component_type *	component, 
								size_t				index,		/* indicates which DI channel to obtain */
								char *				buf, 
								size_t				buf_len 
							)
{
	int  					error_code;
	i8254_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto I8254_Cfg_Get_Error;	 }

	ds = ( i8254_dataset_type *) component->private_data;

	if ( IO_COPY_TO_USER( buf, &(ds->cfg), sizeof( i8254_dataset_type ) ) )
	{ error_code = EC_IO_COPY_TO_USER;		goto I8254_Cfg_Get_Error;	 }

	return SUCCESS;
I8254_Cfg_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/*******************************************************************/
static void I8254_CT_Xfr_Read_Helper( component_type *		component,
									  I8254_CHANNEL_ENUM	channel,
									  uint8_t				control,
									  uint8_t *				status,
									  uint16_t *			count
									)
{
	I8254_RW_ENUM	rw;
	uint8_t			u8_lo = 0;
	uint8_t			u8_hi = 0;	
	/* read the status */
	CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &u8_lo, sizeof( u8_lo ) );
	/* mask off the output and null count bits */
	u8_lo = u8_lo & ~( I8254_CTRL_MASK_SC << I8254_CTRL_SHIFT_SC );
	/* now, insert the channel number - yes we are ignoring output bit and null count bit. */
	u8_lo = u8_lo | I8254_CTRL_SET_SC( channel );
	rw = I8254_CTRL_GET_RW( u8_lo );
	*status = u8_lo;
	/* if there is no count readback then we are done */
	if ( control & ~I8254_READBACK_LATCH_COUNT ) return;
	switch ( rw )
	{
	case I8254_RW_LSB:
		CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_lo ), sizeof( u8_lo ) );
		break;
	case I8254_RW_MSB:
		CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_hi ), sizeof( u8_hi ) );
		break;
	case I8254_RW_LSB_MSB:
		CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_lo ), sizeof( u8_lo ) );
		CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_hi ), sizeof( u8_hi ) );
		break;
	default:
		break;
	}
	*count = IO_U16_SET_U8_LO_HI(u8_lo,u8_hi);
}
/*******************************************************************/
static int I8254_CMD_Xfr(	component_type *	component, 
							size_t				index,		/* not used */
							char *				buf, 
							size_t				buf_len		/* not used */
						)
{
	int						error_code;
	I8254_CHANNEL_ENUM		channel;
	I8254_RW_ENUM			rw;
	/*i8254_dataset_type *	ds;*/
	i8254_ct_xfr_type		cxfr;
	uint8_t					control;
	(void)					buf_len;
	(void)					index;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;	goto I8254_CT_Xfr_Done;	}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &cxfr, buf, sizeof( cxfr ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto I8254_CT_Xfr_Done;	 }

	if ( SYS_TYPE_MAKE( i8254_ct_xfr_type ) != cxfr.type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto I8254_CT_Xfr_Done;	}
	
	/*ds = ( i8254_dataset_type *) component->private_data;*/

	/* Write the control value to the I8254 control register */
	control    = cxfr.control;
	channel    = I8254_CTRL_GET_SC(control);
	rw         = I8254_CTRL_GET_RW(control);
	error_code = SUCCESS;
	if ( I8254_CHANNEL_READBACK == channel )
	{ 	/* Read count value. Note: CT offset address just happens to match channel #.  */
		int chan_bits = I8254_CTRL_GET_MODE(control);
		/* if neither count or status latch is low, then nothing to do */
		if ( I8254_READBACK_LATCH_NONE == ( control & I8254_READBACK_LATCH_NONE ) ) goto I8254_CT_Xfr_Done;

		/* Need to readback the status as well to know how to deal with read/write. */
		control = cxfr.control & I8254_READBACK_LATCH_STATUS;
		CmpMgr_Reg_Write( component, I8254_CONTROL, (char *) &( control ), sizeof( control ) );

		/* process one or more channels as dictated by the chan_bits from the control word */
		if ( I8254_MODE_READBACK_CHAN_0 & chan_bits )
		{
			channel = I8254_CHANNEL_0;
			I8254_CT_Xfr_Read_Helper( component, channel, cxfr.control, 
				                      &(cxfr.status_read[channel]), &(cxfr.count_read[channel])  );
		}
		if ( I8254_MODE_READBACK_CHAN_1 & chan_bits )
		{
			channel = I8254_CHANNEL_1;
			I8254_CT_Xfr_Read_Helper( component, channel, cxfr.control, 
				                      &(cxfr.status_read[channel]), &(cxfr.count_read[channel])  );
		}
		if ( I8254_MODE_READBACK_CHAN_2 & chan_bits ) 
		{
			channel = I8254_CHANNEL_2;
			I8254_CT_Xfr_Read_Helper( component, channel, cxfr.control, 
				                      &(cxfr.status_read[channel]), &(cxfr.count_read[channel])  );
		}
	}
	else if ( I8254_RW_LATCH == rw )
	{ /* latch a single channel and read out the count value */
		I8254_MODE_ENUM mode;
		uint8_t			u8_lo = 0;
		uint8_t			u8_hi = 0;
		switch ( channel )
		{
			case I8254_CHANNEL_0: mode = I8254_MODE_READBACK_CHAN_0;	break;
			case I8254_CHANNEL_1: mode = I8254_MODE_READBACK_CHAN_1;	break;
			case I8254_CHANNEL_2: mode = I8254_MODE_READBACK_CHAN_2;	break;
			default: 
				error_code = EC_I8254_CHANNEL_OUT_OF_RANGE;
				goto I8254_CT_Xfr_Done;  /* wrong channel info */
				break;
		}
		/* Need to readback status of the counter to know how to read out the count */
		I8254_Control_Create( I8254_CHANNEL_READBACK,
							  I8254_RW_STA,
							  mode, /* readback channel for status */ 
							  I8254_TYPE_BINARY, /* readback ignores this */
							  &control
							);
		/* write the command to capture status and then read it. */
		CmpMgr_Reg_Write( component, I8254_CONTROL, (char *) &( control ), sizeof( control ) );
		CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &(cxfr.status_read[channel]), sizeof( cxfr.status_read[channel] ) );
		rw = I8254_CTRL_GET_RW( cxfr.status_read[channel] );
		/* now write the command to actually latch the data */
		CmpMgr_Reg_Write( component, I8254_CONTROL, (char *) &( cxfr.control ), sizeof( cxfr.control ) );
		switch ( rw )
		{
		case I8254_RW_LSB:
			CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_lo ), sizeof( u8_lo ) );
			break;
		case I8254_RW_MSB:
			CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_hi ), sizeof( u8_hi ) );
			break;
		case I8254_RW_LSB_MSB:
			CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_lo ), sizeof( u8_lo ) );
			CmpMgr_Reg_Read( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_hi ), sizeof( u8_hi ) );
			break;
		default:
			break;
		}
		cxfr.count_read[channel] = IO_U16_SET_U8_LO_HI(u8_lo,u8_hi);  /* save the count value to the counter transfer */
	}
	else
	{ /* Write count value */
		uint8_t u8_lo = IO_U16_GET_U8_LO( cxfr.count_write );
		uint8_t u8_hi = IO_U16_GET_U8_HI( cxfr.count_write );

		CmpMgr_Reg_Write( component, I8254_CONTROL, (char *) &( control ), sizeof( control ) );
		/* Write count value. Note: CT offset address just happens to match channel # */
		switch ( rw )
		{
			case I8254_RW_LSB:
				CmpMgr_Reg_Write( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_lo ), sizeof( u8_lo ) );
				break;
			case I8254_RW_MSB:
				CmpMgr_Reg_Write( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_hi ), sizeof( u8_hi ) );
				break;
			case I8254_RW_LSB_MSB:
				CmpMgr_Reg_Write( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_lo ), sizeof( u8_lo ) );
				CmpMgr_Reg_Write( component, channel /* I8254_CHANNEL_n */, (char *) &( u8_hi ), sizeof( u8_hi ) );
				break;
			default:
				break;
		}
		/* mask off channel info - for now ignore what D7 and D6 might be. */
		cxfr.status_read[channel] = cxfr.control & ~( I8254_CTRL_MASK_SC << I8254_CTRL_SHIFT_SC );
	}
I8254_CT_Xfr_Done:
	cxfr.error_code = error_code;
	/* copy results back to user space */
	if ( IO_COPY_TO_USER( buf, &cxfr, sizeof( cxfr ) ) ) error_code = EC_IO_COPY_TO_USER;
	if ( SUCCESS == error_code ) return error_code;
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int I8254_CMD_Xfr_Array(	component_type *	component, 
								size_t				index,	/* not used */
								char *				buf, 
								size_t				buf_len /* not used */
							 )
{
	int						error_code;
	size_t					xi;			/* short for transfer index */
	i8254_ct_xfr_array_type cxa;        /* short for counter/timer transfer array */
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto I8254_CT_Xfr_Array_Error;	}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &cxa, buf, sizeof( cxa ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto I8254_CT_Xfr_Array_Error;	 }

	if ( SYS_TYPE_MAKE( i8254_ct_xfr_array_type ) != cxa.type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto I8254_CT_Xfr_Array_Error;	}

	for ( xi = 0; xi < cxa.qty; xi++ )
	{
		/* The follow function will take care of copy from user and copy to user */
		error_code = I8254_CMD_Xfr(	component, 
									0 /* not used */, 
									(char *) &(cxa.cxfr[xi]),
									0 /* not used */
								 );
		if ( error_code ) goto I8254_CT_Xfr_Array_Error;
	}
	return SUCCESS;
I8254_CT_Xfr_Array_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
/* Updates I/O all at one time, or a polling operation */
static int I8254_CMD_Update(	component_type *	component, 
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
static int I8254_Restart(	component_type *	component, 
							size_t				index,
							char *				buf, 
							size_t				buf_len 
						)
{
	int						error_code;
	size_t					operation;
	i8254_dataset_type *	ds;
	(void)	index;
	(void)	buf_len;

	if      ( NULL == buf  ) operation = RESTART_COLD;
	else if ( 0    == *buf ) operation = RESTART_COLD;
	else  	                 operation = RESTART_WARM;

	error_code = SUCCESS;
	ds = ( i8254_dataset_type *) component->private_data;
	switch( operation )
	{	/*--------------------------------*/
		case RESTART_COLD:
			{ /* commit all configuration to hardware */
				i8254_ct_xfr_array_type cxa;
				cxa.type = SYS_TYPE_MAKE( i8254_ct_xfr_array_type );
				cxa.qty  = I8254_CT_QTY;
				cxa.cxfr = ds->cfg.cxfr;  /* use configuration array */
				error_code = I8254_CMD_Xfr_Array( component, 0, (char *) &cxa, 0 );
			}
			break;
		/*--------------------------------*/
		case RESTART_WARM:
			{ /* retrieve all that we can from the hardware */
				i8254_ct_xfr_type    cxfr;
				i8254_ct_xfr_type *  cfg_cxfr;
				size_t				 chan;

				Mem_Set( &cxfr, 0, sizeof( cxfr ) );
				cxfr.control = I8254_CTRL_CREATE(	I8254_CHANNEL_READBACK,				\
													I8254_RW_CNT_STA,					\
													I8254_MODE_READBACK_CHAN_ALL,		\
													I8254_TYPE_BINARY /* ignored */		\
												);
				error_code = I8254_CMD_Xfr( component, 0, (char *) &cxfr, 0 );
				cxfr.error_code = error_code;
				cfg_cxfr = ds->cfg.cxfr;
				
				for ( chan = 0; chan < I8254_CT_QTY; chan++ )
				{
					cfg_cxfr[chan].error_code      = cxfr.error_code;
					cfg_cxfr[chan].type            = SYS_TYPE_MAKE( i8254_ct_xfr_type );
					cfg_cxfr[chan].status_read[0]  = cxfr.status_read[chan]; /* raw status */
					/* mask off channel info  */
					cfg_cxfr[chan].control         = cxfr.status_read[chan] & ~( I8254_CTRL_MASK_SC << I8254_CTRL_SHIFT_SC );
					/* insert channel info -- this completes reconstruction of the control value */
					cfg_cxfr[chan].control        |= I8254_CTRL_SET_SC( chan );
					cfg_cxfr[chan].count_read[0]   = cxfr.count_read[chan];
				}
			}		
			break;
		/*--------------------------------*/
		default:  /* cold or any other */
/*			 copy intialize values from config into hardware registers.  more like a reset.*/
			break;
	}
	return error_code;
}
#if(0)
/*****************************************************************/
uint8_t I8254_Control_Create( I8254_CHANNEL_ENUM	channel,				
							  I8254_RW_ENUM			rw,	
							  I8254_MODE_ENUM		mode,
							  I8254_TYPE_ENUM		bcd
							)
{
	uint8_t control;

	control = I8254_CTRL_CREATE( channel,rw,mode,bcd );
	return control;
}
#endif
/*****************************************************************/
int I8254_Config_Initialize( i8254_cfg_type * cfg )
{
	size_t	chan;
	Mem_Set( cfg, 0, sizeof( i8254_cfg_type ) );
	cfg->type = SYS_TYPE_MAKE( i8254_cfg_type );


	for ( chan = 0; chan < I8254_CT_QTY; chan++ )
	{
		cfg->cxfr[chan].type    = SYS_TYPE_MAKE( i8254_ct_xfr_type );
		cfg->cxfr[chan].control = I8254_CTRL_CREATE( chan, 								\
				                                     I8254_RW_LSB_MSB, 					\
				                                     I8254_MODE_SQUARE_WAVE_GENERATOR, 	\
				                                     I8254_TYPE_BINARY 					);
	}

	return SUCCESS;
}
/*****************************************************************/
int I8254_Create( component_type * component, size_t instance )
{
	int						error_code;
	i8254_dataset_type *	ds;
	size_t					mem_size;
	void *					mem;

	mem_size = sizeof( i8254_dataset_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_I8254_DATASET_ALLOCATE;  goto I8254_Create_Error; }
	Mem_Set( mem, 0, mem_size );
	ds = (i8254_dataset_type *) mem;
	ds->type = SYS_TYPE_MAKE( i8254_dataset_type );
	
	error_code = I8254_Config_Initialize( &(ds->cfg) );
	if ( error_code ) goto I8254_Create_Error;

	component->definition   = &component_i8254_definition;
	component->physical_id  = IOC_CREATE_PHYSICAL_ID_COMPONENT( COMPONENT_I8254, instance );
	component->offset       = 0	   /* fill in at device level */;
	component->private_data = ds;  /* assign private data */
	component->status       = component->status | CMP_STS_BUILD(0,1,0,0,0); /* private data assigned */
	return SUCCESS;
I8254_Create_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
} 
/*****************************************************************/
int I8254_Destroy( component_type * component )
{
	int error_code;

	if ( NULL == component ) return SUCCESS;
	
	if ( NULL != component->definition ) 
	{ /* if it is null, we will just have to assume we are working with the 
	     correct data type.
	  */
		if ( SYS_TYPE_MAKE(component_definition_type) != component->definition->type )
		{  error_code = EC_I8254_COMPONENT_INVALID;	goto I8254_Destroy_Error;  }
	}
	if ( NULL != component->private_data )
	{
		Mem_Free( NULL, &(component->private_data) );
		/*component->private_data = NULL;*/
	}
	return SUCCESS;
I8254_Destroy_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* LIBRARY LAYER ONLY */
/*****************************************************************************/
#ifndef __KERNEL__
/**********************************************************************************************************/
/**********************************************************************************************************/
/*
 * All of the following functions (APIs) are available to the application.
 *
 */
/**********************************************************************************************************//**
@ingroup  i8254
\brief
 Apex Universal Library I8254 specific function

\param[in] control is a control word built to be sent down to the I8254.
\param[in] rw
\param[in] mode
\param[in] bcd
\param[out] i8254_control_word
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV I8254_Control_Create(	I8254_CHANNEL_ENUM	channel,				
													I8254_RW_ENUM		rw,	
													I8254_MODE_ENUM		mode,
													I8254_TYPE_ENUM		bcd,
													uint8_t *			i8254_control_word
												)
{
	*i8254_control_word = I8254_CTRL_CREATE( channel,rw,mode,bcd );
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup  i8254
\brief
 Apex Universal Library I8254 specific function

\param[in] channel
\param[out] rw
\param[out] mode
\param[out] bcd
\param[in] i8254_control_word
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV I8254_Control_Extract(	I8254_CHANNEL_ENUM	channel,
													I8254_RW_ENUM *		rw,	
													I8254_MODE_ENUM *	mode,
													I8254_TYPE_ENUM *	bcd,
													uint8_t 			i8254_control_word
												  )
{
	int error_code;
	I8254_CHANNEL_ENUM chan_expected;

	if ( channel >= I8254_CT_QTY ) 
	{ error_code = EC_I8254_CHANNEL_OUT_OF_RANGE; goto I8254_Control_Extract_Error; }

	chan_expected = I8254_CTRL_GET_SC( i8254_control_word );
	if ( channel != chan_expected ) 
	{ error_code = EC_I8254_CHANNEL_MISMATCH; goto I8254_Control_Extract_Error; }

	*rw		= I8254_CTRL_GET_RW( i8254_control_word );
	*mode	= I8254_CTRL_GET_MODE( i8254_control_word );
	*bcd	= I8254_CTRL_GET_BCD( i8254_control_word );
	return SUCCESS;
I8254_Control_Extract_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup  i8254
\brief
 Apex Universal Library I8254 specific function

\param[in] requested_hz
\param[in] clock_external_hz
\param[in] ct_clock_source
\param[in] channel
\param[in] control
\param[out] initial_count
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV I8254_Count_Freq_Hz(	double		requested_hz,		/* requested freq		*/
													double		clock_source_hz,	/* default, used if not CTn_nMHz			*/
													int			ct_clock_source,	/* TRACERE_CLK_CTn_4MHZ _10MHZ _20MHZ _40MHZ */
													uint8_t		control,			/* i8254 control word	*/
													uint16_t *	initial_count,		/* i8254 count to load	*/
													double *    actual_hz,			/* actual output frequency     */
													double *    error_actual		/* computed error, x1.0e6 to get ppm */
												)
{
	double				clk_freq;
	double 				divisor;
	double				scratch;
	int					error_code;
	I8254_MODE_ENUM		mode;
	uint16_t			divisor_u16;
	
	clk_freq = clock_source_hz;

	if ( ( 0.0 == clk_freq ) || ( clk_freq > 40.0e6 ) )
	{ error_code = EC_I8254_CLK_OUT_OF_RANGE; goto I8254_Count_Freq_Hz_Error; }

	if ( ( requested_hz > clk_freq ) || ( 0.0 == requested_hz ) )
	{ error_code = EC_I8254_FREQ_OUT_OF_RANGE; goto I8254_Count_Freq_Hz_Error; }

	mode = I8254_CTRL_GET_MODE( control );
	switch( mode )
	{
		case I8254_MODE_RATE_GENERATOR:
			divisor = clk_freq / requested_hz;
			break;
		case I8254_MODE_SQUARE_WAVE_GENERATOR:
			if ( requested_hz > ( clk_freq / 2.0 ) )
			{ error_code = EC_I8254_FREQ_OUT_OF_RANGE; goto I8254_Count_Freq_Hz_Error; }
			divisor = clk_freq / requested_hz;
			break;
		default:
			error_code = EC_I8254_INVALID_MODE;
			goto I8254_Count_Freq_Hz_Error;
	}
	if ( divisor > ((double) UINT16_MAX ) )
	{ error_code = EC_I8254_FREQ_OUT_OF_RANGE; goto I8254_Count_Freq_Hz_Error; }

	if ( divisor < 1.0 )
	{ error_code = EC_I8254_FREQ_OUT_OF_RANGE; goto I8254_Count_Freq_Hz_Error; }

	divisor_u16 = (uint16_t) divisor;
	scratch = divisor - ( (double) divisor_u16 );
	if ( scratch > 0.5 ) divisor_u16++;
	else if ( scratch < -0.5 ) divisor_u16--;

	if ( 0 == divisor_u16 )
	{ error_code = EC_I8254_FREQ_OUT_OF_RANGE; goto I8254_Count_Freq_Hz_Error; }

	scratch 	   = ( (double) divisor_u16 );
	*actual_hz     = clk_freq / scratch;
	scratch 	   = divisor - scratch;
	*error_actual  = scratch / divisor;
	*initial_count = divisor_u16;
	return SUCCESS;
I8254_Count_Freq_Hz_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup  i8254
\brief
 Apex Universal Library I8254 specific function

\param[in] requested_sec 
\param[in] clock_source_hz 
\param[in] channel 
\param[in] control 
\param[out] initial_count 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV I8254_Count_Time_Sec(	double		requested_sec,		/* requested freq						*/
													double		clock_source_hz,	/* default, used if not CTn_nMHz		*/
													uint8_t		control,			/* i8254 control word					*/
													uint16_t *	initial_count,		/* i8254 count to load					*/
													double *    actual_sec,			/* actual output time interval			*/
													double *    error_actual		/* computed error, x1.0e6 to get ppm	*/
												)
{
	double				clk_freq;
	double				interval_clock;
	double				count;
	double				scratch;
	int					error_code;
	uint16_t			count_u16;
	I8254_MODE_ENUM		mode;
	
	clk_freq = clock_source_hz;

	if ( ( 0.0 == clk_freq ) || ( clk_freq > 40.0e6 ) )
	{ error_code = EC_I8254_CLK_OUT_OF_RANGE; goto I8254_Count_Time_Sec_Error; }

	interval_clock 	= 1.0 / clk_freq;  /* convert to frequency */
	mode 			= I8254_CTRL_GET_MODE( control );
	switch( mode )
	{
		case I8254_MODE_INTERRUPT_ON_TERMINAL_COUNT:  			/* (N + 1) * CLK */
			count = ( requested_sec - interval_clock ) / interval_clock;
			/* count must be zero or greater up to UINT16_MAX */
			break;
		case I8254_MODE_RATE_GENERATOR:							/* N * CLK */
			count = requested_sec / interval_clock;
			break;
		case I8254_MODE_SQUARE_WAVE_GENERATOR:					/* N * CLK */
			count = requested_sec / interval_clock;
			break;
		case I8254_MODE_HARDWARE_RETRIGGERABLE_ONE_SHOT:		/* N * CLK */
			count = requested_sec / interval_clock;
			/* count must be zero or greater up to UINT16_MAX */
			break;
		case I8254_MODE_SOFTWARE_TRIGGERED_STROBE:				/* (N + 1) * CLK */
			count = ( requested_sec - interval_clock ) / interval_clock;
			/* count must be zero or greater up to UINT16_MAX */
			break;
		case I8254_MODE_HARDWARE_TRIGGER_STROBE:				/* (N + 1) * CLK */
			count = ( requested_sec - interval_clock ) / interval_clock;
			/* count must be zero or greater up to UINT16_MAX */
			break;
		default:
			error_code = EC_I8254_INVALID_MODE;
			goto I8254_Count_Time_Sec_Error;
	}

	if ( ( count < 0.5 ) || ( count > ( ((double) UINT16_MAX) + 0.5 ) ) )
	{ error_code = EC_I8254_TIME_OUT_OF_RANGE; goto I8254_Count_Time_Sec_Error; }

	if ( ( count >= 0.5 ) && ( count < 1.0 ) ) count_u16 = 1;
	else 									   count_u16 = (uint16_t) count;

	if ( 0 == count_u16 ) count_u16 = UINT16_MAX;

	scratch = ( (double) count_u16 );
	switch( mode )
	{
		case I8254_MODE_INTERRUPT_ON_TERMINAL_COUNT:  			/* (N + 1) * CLK */
			*actual_sec = ( scratch + 1.0 ) * interval_clock;
			break;
		case I8254_MODE_RATE_GENERATOR:							/* N * CLK */
			*actual_sec = scratch * interval_clock;
			break;
		case I8254_MODE_SQUARE_WAVE_GENERATOR:					/* N * CLK */
			*actual_sec = scratch * interval_clock;
			break;
		case I8254_MODE_HARDWARE_RETRIGGERABLE_ONE_SHOT:		/* N * CLK */
			*actual_sec = scratch * interval_clock;
			break;
		case I8254_MODE_SOFTWARE_TRIGGERED_STROBE:				/* (N + 1) * CLK */
			*actual_sec = ( scratch + 1.0 ) * interval_clock;
			break;
		case I8254_MODE_HARDWARE_TRIGGER_STROBE:				/* (N + 1) * CLK */
			*actual_sec = ( scratch + 1.0 ) * interval_clock;
			break;
		default:
			error_code = EC_I8254_INVALID_MODE;
			goto I8254_Count_Time_Sec_Error;

	}
	scratch 	   = count - scratch;
	*error_actual  = scratch / count;
	*initial_count = count_u16;
	return SUCCESS;
I8254_Count_Time_Sec_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
/* returns SUCCESS if frequency calculation is to be performed */
/* returns SUCCESS if it OK to compute frequency.   If non-zero, then revert to time interval calculations */
AUL_DLL_API int AULCALLCONV I8254_Do_Freq_Hz( uint8_t control  /* i8254 control word	*/ )
{
	int					error_code;
	I8254_MODE_ENUM		mode;
	error_code = SUCCESS;
	mode = I8254_CTRL_GET_MODE( control );
	switch( mode )
	{
		case I8254_MODE_RATE_GENERATOR:
		case I8254_MODE_SQUARE_WAVE_GENERATOR:
			break;
		default:
			error_code = -1;
			break;
	}
	return error_code;
}



/*****************************************************************************/
#endif /* __KERNEL__ */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/




/* <INTERNAL_BEGIN> */

#if(0)
/*******************************************************************/
static int I8254_State_Get(	component_type *	component, 
						size_t				index,		/* indicates which DI to obtain */
						char *				buf, 
						size_t				buf_len 
					)
{
	int						error_code;
	i8254_dataset_type *	ds;
	i8254_ct_type			ct;
	uint8_t					reg_value;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;			goto I8254_State_Get_Error;	}

	if ( index >= I8254_CT_QTY )
	{ error_code = EC_I8254_CHANNEL_OUT_OF_RANGE; goto I8254_State_Get_Error;	}
	
	ds = ( i8254_dataset_type *) component->private_data;

	/* write the control value */
	reg_value   = 0xC0;							/* read-back command		*/
	reg_value  |= 0x02 << index;				/* set channel				*/
	reg_value  |= I8254_READBACK_LATCH_NONE;	/* initialize latch requests*/
	reg_value  &= I8254_READBACK_LATCH_STATUS;	/* set status latch request */
	CmpMgr_Reg_Write( component, I8254_CONTROL, (char *) &( reg_value ), sizeof( reg_value ) );

	/* read the value */
	CmpMgr_Reg_Read( component, index, (char *) &( reg_value ), sizeof( reg_value ) );

	Mem_Set( &ct, 0, sizeof( ct ) );
	ct.type		= SYS_TYPE_MAKE(i8254_ct_type);
	ct.bcd		= I8254_CTRL_GET_BCD(reg_value);
	ct.mode		= I8254_CTRL_GET_MODE(reg_value);
	ct.channel = index;
	
	if ( IO_COPY_TO_USER( buf, &ct, sizeof( ct ) ) )
	{ error_code = EC_IO_COPY_TO_USER;		goto I8254_State_Get_Error;	 }

	return SUCCESS;
I8254_State_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8254_NAME, __func__, __LINE__, error_code );
	return -error_code;
}





/******************************************************************
Description.  Updates I/O all at one time.
*/
static int I8254_Update(	component_type *	component, 
					size_t				index,		/* not used */
					char *				buf, 
					size_t				buf_len 
				  )
{
	size_t					byte_position;
	i8254_dataset_type *	ds;
	(void)					buf;
	(void)					buf_len;

	ds			= ( i8254_dataset_type * ) component->private_data;

	for ( byte_position = 0; byte_position < I8254_CT_QTY; byte_position++ )
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
			CmpMgr_Reg_Write(	component, 
							byte_position,  /* register index or ID */
							(char *) &(ds->port_internal[byte_position].raw), 
							I8254_REGISTER_SIZE_FIXED 
						 );	
		}
		else
		{ /* input port */
			CmpMgr_Reg_Read(	component, 
							byte_position, /* register index or ID */
							(char *) &(ds->port_internal[byte_position].raw), 
							I8254_REGISTER_SIZE_FIXED 
						 );
			ds->copy[byte_position] = ds->port_internal[byte_position].raw ^ ds->cfg.port[byte_position].polarity;
		}
	}
	return SUCCESS;
}
/*******************************************************************/
static int I8254_Restart( component_type *	component, 
					 size_t				index,
					 char *				buf, 
					 size_t				buf_len 
				   )
{
	size_t						byte_position;
	size_t						operation;
	i8254_dataset_type *	ds;
	(void)						buf_len;

	if      ( NULL == buf  ) operation = RESTART_COLD;
	else if ( 0    == *buf ) operation = RESTART_COLD;
	else  	                operation = RESTART_WARM;

	ds = ( i8254_dataset_type * ) component->private_data;

	I8254_Apply_Directions_Helper( ds );

	switch( operation )
	{	/*--------------------------------*/
		case RESTART_COLD:
/*TODO: test for valid configuration by observing the type value (SYS data type).*/
			CmpMgr_Reg_Write( component, I8254_REG_CFG, (char *)&(ds->cfg.port_directions), sizeof(uint8_t) );

			for ( byte_position = 0; byte_position < I8254_CT_QTY; byte_position++ )
			{
				ds->copy[byte_position] = ds->cfg.port[byte_position].init_value;
			}
			
/*TODO: we could fix up the bit map here if it all turns out to be null??*/
			I8254_Update( component,
								index,
								buf,
								buf_len
							  );
			break;
		/*--------------------------------*/
		case RESTART_WARM:
			CmpMgr_Reg_Write( component, I8254_REG_CFG, (char *)&(ds->cfg.port_directions), sizeof(uint8_t) );

			for ( byte_position = 0; byte_position < I8254_CT_QTY; byte_position++ )
			{
				CmpMgr_Reg_Read(	component, 
								byte_position, 
								(char *) &(ds->port_internal[byte_position].raw), 
								I8254_REGISTER_SIZE_FIXED 
							 );
				ds->copy[byte_position] = ds->port_internal[byte_position].raw ^ ds->cfg.port[byte_position].polarity;
			}			
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
#endif

/* <INTERNAL_END> */

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
static cmp_fnc i8254_jump_table[] =
{
	COMPONENT_I8254_CMD_DEFINITIONS( CMD_CMP_EXTRACT_JUMP_TABLE )
};
/*****************************************************************/
component_definition_type component_i8254_definition =
{
	SYS_TYPE_MAKE(component_definition_type),
	i8254_register_table,
	i8254_jump_table,
	I8254_JUMP_TABLE_QTY,
#if defined( TARGET_CHECK_BUILD )
	COMPONENT_I8254_NAME,
#endif
	I8254_MODULE_REV,
	I8254_COMMAND_REV,
	I8254_STATUS_REV
};

