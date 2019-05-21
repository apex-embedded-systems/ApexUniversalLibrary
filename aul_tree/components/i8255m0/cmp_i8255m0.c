/********************************************************************************
_____________________________________________________________________________  
Description:

I8255M0 Component Module.
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

#include "cmp_i8255m0.h"
#include "../../shared/cmd.h"
#include "../../shared/status.h"
#include "../../io/io_private.h"  /* IO_COPY_FROM_USER, IO_COPY_TO_USER */ 
#include "../../shared/error.h"
/*******************************************************************/
#define I8255M0_MODULE_REV  "$Date: 2015-07-31 21:59:26 -0500 (Fri, 31 Jul 2015) $"

/***************************************************************
@ingroup i8255m0 
\brief
data structure
Data structure that is instantiated when this component is created which occurs
during the device open function.  The functions within this component will 
internally manage the parameters and data within this data structure.

TITLE:     object data
REQUIRED:  yes.
*/
struct i8255m0_dataset
{
	SYS_TYPE				type;   /* must always be first */
	i8255m0_cfg_type		cfg;	/* must always be second */
	/* working copy of the ports */
	uint8_t					copy[I8255M0_PORT_QTY];
	/* */
	struct
	{
		uint8_t raw;		/* raw port values 				*/
		uint8_t direction;	/* copy of direction 0=in, 1=out */
	} port_internal[I8255M0_PORT_QTY];
};
typedef struct i8255m0_dataset i8255m0_dataset_type;
/***************************************************************
@ingroup i8255m0 
\brief 
Register set definition.
Component register definition occurs here.  Basically, we are taking an X-Macro and 
'automatically' building out this array.  The macro definition I8255M0_REGISTER_SET_DEFINITION
can be found in cmp_i8255m0_reg.h.  We use a macro for definition because if you 
look at it, the macro is a simple to maintain table and all register specific locations
read/write access and so on are all contained within said header file.   The register set
can be actual hardware register definitions or virtual in that their meaning only applies within 
the software space and has no direct hardware definition; or any combination thereof.  It 
is the responsibility of all the functions within this component to manage these registers.

TITLE:     registers
REQUIRED:  Generally optional, required for this compoenent
*/
static REGISTER_TYPE i8255m0_register_table[] = 
{
	I8255M0_REGISTER_SET_DEFINITION( REG_EXTRACT_PARAMS )
	REG_LIST_NULL
};

/*******************************************************************/
/*TODO: move to a general use location so that it can be reused elsewhere.*/
#define I8255M0_BIT_MASK_QTY  8
uint8_t i8255m0_byte_bit_mask[I8255M0_BIT_MASK_QTY] =
{
	0x01, 0x02, 0x04, 0x08,
	0x10, 0x20, 0x40, 0x80
};
/*******************************************************************/
/*static void I8255M0_Bit_Map_Default_Helper( uint8_t * bit_map )
{
	size_t  bit;
	for ( bit = 0; bit < I8255M0_BITS_QTY; bit++ )
	{
		bit_map[bit] = (uint8_t) bit;
	}
}*/
/*******************************************************************/
static void I8255M0_Apply_Directions_Helper( i8255m0_dataset_type * ds )
{
	switch( ds->cfg.port_directions )
	{
		case I8255M0_PDIR_AIN__BIN__CIN: 
			ds->port_internal[I8255M0_REG_A].direction = 0;
			ds->port_internal[I8255M0_REG_B].direction = 0;
			ds->port_internal[I8255M0_REG_C].direction = 0;
			break;
		case I8255M0_PDIR_AIN__BIN__COUT:
			ds->port_internal[I8255M0_REG_A].direction = 0;
			ds->port_internal[I8255M0_REG_B].direction = 0;
			ds->port_internal[I8255M0_REG_C].direction = 1;
			break;
		case I8255M0_PDIR_AIN__BOUT_CIN:
			ds->port_internal[I8255M0_REG_A].direction = 0;
			ds->port_internal[I8255M0_REG_B].direction = 1;
			ds->port_internal[I8255M0_REG_C].direction = 0;
			break;
		case I8255M0_PDIR_AIN__BOUT_COUT:
			ds->port_internal[I8255M0_REG_A].direction = 0;
			ds->port_internal[I8255M0_REG_B].direction = 1;
			ds->port_internal[I8255M0_REG_C].direction = 1;
			break;
		case I8255M0_PDIR_AOUT_BIN__CIN:
			ds->port_internal[I8255M0_REG_A].direction = 1;
			ds->port_internal[I8255M0_REG_B].direction = 0;
			ds->port_internal[I8255M0_REG_C].direction = 0;
			break;
		case I8255M0_PDIR_AOUT_BIN__COUT:
			ds->port_internal[I8255M0_REG_A].direction = 1;
			ds->port_internal[I8255M0_REG_B].direction = 0;
			ds->port_internal[I8255M0_REG_C].direction = 1;
			break;
		case I8255M0_PDIR_AOUT_BOUT_CIN:
			ds->port_internal[I8255M0_REG_A].direction = 1;
			ds->port_internal[I8255M0_REG_B].direction = 1;
			ds->port_internal[I8255M0_REG_C].direction = 0;
			break;
		case I8255M0_PDIR_AOUT_BOUT_COUT:
			ds->port_internal[I8255M0_REG_A].direction = 1;
			ds->port_internal[I8255M0_REG_B].direction = 1;
			ds->port_internal[I8255M0_REG_C].direction = 1;
			break;
	}
}
/***************************************************************
@ingroup i8255m0 
\brief 
This function is called during a power up/down sequence, typically after hardware
has powered down and is in a sleep mode.  This does not apply to cold 
power up cycles, but more for sleep or drowse situations.  This has not
been tested.

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
TITLE:     Power up
REQUIRED:  optional

\return An error code maybe returned, otherwise SUCCESS
*/
static int I8255M0_CMD_Power_Up(	component_type *	component, 
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
/***************************************************************
@ingroup i8255m0 
\brief
This function is called during a power up/down sequence, typically after hardware
has powered down and is in a sleep mode.  This does not apply to cold 
power up cycles, but more for sleep or drowse situations.  This has not
been tested.

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
TITLE:     Power up
REQUIRED:  optional

\return An error code maybe returned, otherwise SUCCESS
*/
static int I8255M0_CMD_Power_Down(	component_type *	component, 
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
#define I8255M0_CAPABILITY_DO_CHAN   I8255M0_BITS_QTY
#define I8255M0_CAPABILITY_DI_CHAN   I8255M0_BITS_QTY

/* if enabled, it skips the register width calculations as it is always the same -- 1 byte. */
#define I8255M0_REGISTER_SIZE_FIXED   1


/***************************************************************
@ingroup i8255m0 
\brief 
This function is called and is passed the components configuration data.  This function 
will analyze the integrity and validity of the configuration data.  The configuration data is first
copied to candidate space, analyzed and then copied to the internal data set.  

Why do we copy to candidate space? This allows the component to reside in either library or
driver/kernel space.  Within kernel space one must copy from user space to kernel space and
therefore the copy to candidate (i.e. "i8255m0_cfg_type candidate;").

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
		   configuration data structure:  "struct i8255m0_cfg", see cmp_i8255m0.h.
TITLE:     Configuration Set
REQUIRED:  Generally optional, required for this component

\return An error code maybe returned, otherwise SUCCESS
*/
static int I8255M0_CMD_Cfg_Set( component_type *	component, 
							size_t				index,		/* indicates which DI channel to obtain */
							char *				buf, 
							size_t				buf_len 
						  )
{
	int						error_code;
	i8255m0_cfg_type		candidate;
	i8255m0_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto I8255M0_Cfg_Set_Error;		}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto I8255M0_Cfg_Set_Error;	 }

	ds = ( i8255m0_dataset_type *) component->private_data;

	if ( candidate.type != ds->cfg.type ) 
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto I8255M0_Cfg_Set_Error;	}

	switch( candidate.port_directions )
	{
		case I8255M0_PDIR_AIN__BIN__CIN	:
		case I8255M0_PDIR_AIN__BIN__COUT:
		case I8255M0_PDIR_AIN__BOUT_CIN	:
		case I8255M0_PDIR_AIN__BOUT_COUT:
		case I8255M0_PDIR_AOUT_BIN__CIN	:
		case I8255M0_PDIR_AOUT_BIN__COUT:
		case I8255M0_PDIR_AOUT_BOUT_CIN	:
		case I8255M0_PDIR_AOUT_BOUT_COUT:
			break;
		default:
			error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;
			goto I8255M0_Cfg_Set_Error;
	}
/* perform any additional testing/verification here */

	Mem_Copy( &(ds->cfg), &candidate, sizeof( candidate ) );

/* the data will be committed to hardware upon a restart warm/cold */

	return SUCCESS;
I8255M0_Cfg_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8255M0_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/***************************************************************
@ingroup i8255m0 
\brief 
Returns the internal configuration data that currently resides within the 
data set (i.e. "struct i8255m0_dataset" ).

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
		   configuration data structure:  "struct i8255m0_cfg", see cmp_i8255m0.h.
TITLE:     Configuration Get
REQUIRED:  Generally optional, required for this compoenent

\param[in] index  specifies the channel
\return An error code maybe returned, otherwise SUCCESS
*/
static int I8255M0_CMD_Cfg_Get( component_type *	component, 
					 size_t				index,		/* indicates which DI channel to obtain */
					 char *				buf, 
					 size_t				buf_len 
			      )
{
	int  					error_code;
	i8255m0_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto I8255M0_Cfg_Get_Error;	 }

	ds = ( i8255m0_dataset_type *) component->private_data;

	if ( IO_COPY_TO_USER( buf, &(ds->cfg), sizeof( i8255m0_dataset_type ) ) )
	{ error_code = EC_IO_COPY_TO_USER;		goto I8255M0_Cfg_Get_Error;	 }

	return SUCCESS;
I8255M0_Cfg_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8255M0_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/***************************************************************
@ingroup i8255m0 
\brief 
Returns the requested Digital Input (DI) bit.  

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
		   configuration data structure:  "struct i8255m0_cfg", see cmp_i8255m0.h.
TITLE:     Digital Input Bit Get
REQUIRED:  Specific to this component

\param[in] buf    points to the data byte to be written (0 or non-zero value)
\param[in] index  specifies the channel or port
\return An error code maybe returned, otherwise SUCCESS
*/
static int I8255M0_CMD_DI_Get(  component_type *	component, 
					 size_t				index,		/* indicates which DI channel to obtain */
					 char *				buf, 
					 size_t				buf_len 
			      )
{
	size_t					byte_position;
	size_t					bit_position;
	size_t					physical_address;
	uint8_t					mask;
	i8255m0_dataset_type *	ds;
	(void)					buf_len;

	if ( index >= I8255M0_BITS_QTY ) return -EC_I8255M0_CHANNEL_OUT_OF_RANGE;

	ds	= ( i8255m0_dataset_type * ) component->private_data;
	physical_address = index;
	byte_position	 = physical_address >>   3;
	bit_position	 = physical_address & 0x07;
	mask = i8255m0_byte_bit_mask[bit_position];
	if ( false == ds->cfg.simultaneous_update )
	{
		CmpMgr_Reg_Read(	component, 
							byte_position, /* register index or ID */
							(char *) &(ds->port_internal[byte_position].raw), 
							I8255M0_REGISTER_SIZE_FIXED 
					   );
		ds->copy[byte_position] = ds->port_internal[byte_position].raw ^ ds->cfg.port[byte_position].polarity;
	}
	*buf = (char)( ds->copy[byte_position] & mask );
	return SUCCESS;
}
/***************************************************************
@ingroup i8255m0 
\brief 
Sets the Digital Output (DO) bit.

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
		   configuration data structure:  "struct i8255m0_cfg", see cmp_i8255m0.h.
TITLE:     Digital Output Bit Set
REQUIRED:  Specific to this component

\param[in] buf    points to the data byte to be read (0 or non-zero value)
\param[in] index  specifies the channel or port
\return An error code maybe returned, otherwise SUCCESS
*/
/*
TODO: semaphore - there is a time where data is unknown!
*/
static int I8255M0_CMD_DO_Set(  component_type *	component, 
					 size_t				index,		/* indicates which DI to obtain */
					 char *				buf, 
					 size_t				buf_len 
			      )
{
	size_t					byte_position;
	size_t					bit_position;
	size_t					physical_address;
	uint8_t					mask;
	uint8_t					value;
	i8255m0_dataset_type *	ds;
	(void)					buf_len;

	if ( index >= I8255M0_BITS_QTY ) return -EC_I8255M0_CHANNEL_OUT_OF_RANGE;
	ds	= ( i8255m0_dataset_type * ) component->private_data;
	physical_address = index;
	byte_position    = physical_address >>   3;
	bit_position     = physical_address & 0x07;
	mask             = i8255m0_byte_bit_mask[bit_position];
	if ( !(ds->port_internal[byte_position].direction) )
	{ /* error, this is an output port only */
		return -EC_I8255M0_ILLEGAL_REQUEST; 
	}
	/* map in appropriate byte */
	value = ds->copy[byte_position];
	/* perform bit set/clear operation */
	if ( 0 == *buf ) value = value & ~mask;
	else             value = value | mask;
	/* map out appropriate byte */
	ds->copy[byte_position] = value;

	if ( false == ds->cfg.simultaneous_update )
	{	/* NOTE: it is more efficient to selectively write out than to write all out
			because ISA bus times are much slower than processor time.
		*/
		ds->port_internal[byte_position].raw = value ^ ds->cfg.port[byte_position].polarity;
		CmpMgr_Reg_Write(	component, 
							byte_position, /* register index or ID */
							(char *) &(ds->port_internal[byte_position].raw), 
							I8255M0_REGISTER_SIZE_FIXED 
						);
	}
	return SUCCESS;
}
/***************************************************************
@ingroup i8255m0 
\brief 
Returns the requested Port data, in this case we have 8-bit ports A, B, and C.  

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
		   configuration data structure:  "struct i8255m0_cfg", see cmp_i8255m0.h.
TITLE:     Port Input Get  (8-bit ports)
REQUIRED:  Specific to this component

\param[in] buf    points to the data byte to be written
\param[in] index  specifies the channel or port
\return An error code maybe returned, otherwise SUCCESS
*/
static int I8255M0_CMD_Port_Get(	component_type *	component, 
						size_t				index,		/* indicates which port to obtain */
						char *				buf, 
						size_t				buf_len 
					)
{
	i8255m0_dataset_type *	ds;
	(void)					buf_len;

	if ( index >= I8255M0_PORT_QTY ) return -EC_I8255M0_CHANNEL_OUT_OF_RANGE;

	ds	= ( i8255m0_dataset_type * ) component->private_data;
/*	if ( ds->port_internal[index].direction )
	{  error, this is an input port only
		return -EC_I8255M0_ILLEGAL_REQUEST;
	}*/

	if ( false == ds->cfg.simultaneous_update )
	{
		CmpMgr_Reg_Read(	component, 
							index, /* register index or ID */
							(char *) &(ds->port_internal[index].raw), 
							I8255M0_REGISTER_SIZE_FIXED 
						);
		ds->copy[index] = ds->port_internal[index].raw ^ ds->cfg.port[index].polarity;
	}
	*buf = (char)( ds->copy[index] );
	return SUCCESS;
}
/***************************************************************
@ingroup i8255m0 
\brief 
Sets Port data, in this case we have 8-bit ports A, B, and C.   

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
		   configuration data structure:  "struct i8255m0_cfg", see cmp_i8255m0.h.
TITLE:     Port Output Set  (8-bit ports)
REQUIRED:  Specific to this component

\param[in] buf    points to the data byte to be read and then written to port
\param[in] index  specifies the channel or port
\return An error code maybe returned, otherwise SUCCESS
*/
/*
TODO: semaphore - there is a time where data is unknown!
*/
static int I8255M0_CMD_Port_Set(	component_type *	component, 
						size_t				index,		/* indicates port number */
						char *				buf, 
						size_t				buf_len 
					)
{
	uint8_t					value;
	i8255m0_dataset_type *	ds;
	(void)					buf_len;

	if ( index >= I8255M0_PORT_QTY ) return -EC_I8255M0_CHANNEL_OUT_OF_RANGE;

	ds	= ( i8255m0_dataset_type * ) component->private_data;

	if ( !(ds->port_internal[index].direction) )
	{ /* error, this is an output port only */
		return -EC_I8255M0_ILLEGAL_REQUEST; 
	}
	/* map in appropriate byte */
	value = ds->copy[index];
	/* set the port */
	value = *buf;
	/* map out appropriate byte */
	ds->copy[index] = value;

	if ( false == ds->cfg.simultaneous_update )
	{	/* NOTE: it is more efficient to selectively write out than to write all out
			because ISA bus times are much slower than processor time.
		*/
		ds->port_internal[index].raw = value ^ ds->cfg.port[index].polarity;
		CmpMgr_Reg_Write(	component, 
							index,  /* register index or ID */
							(char *) &(ds->port_internal[index].raw), 
							I8255M0_REGISTER_SIZE_FIXED 
						);
	}
	return SUCCESS;
}
/***************************************************************
@ingroup i8255m0 
\brief 
Updates or copies data to/from component data set to/from hardware all at once.
This provides a 'simultaneous' behavior which reduces software induced phase delays between 
writes to hardware registers.  Called by a cold restart (see I8255M0_CMD_Restart() ).

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
		   configuration data structure:  "struct i8255m0_cfg", see cmp_i8255m0.h.
TITLE:     Update (All)
REQUIRED:  Specific to this component

\param[in] buf    not used
\param[in] index  not used
\return An error code maybe returned, otherwise SUCCESS
*/
/*
Description.  Updates I/O all at one time.
*/
static int I8255M0_CMD_Update(	component_type *	component, 
					size_t				index,		/* not used */
					char *				buf, 
					size_t				buf_len 
				  )
{
	size_t					byte_position;
	i8255m0_dataset_type *	ds;
	(void)					buf;
	(void)					buf_len;

	ds = ( i8255m0_dataset_type * ) component->private_data;

	for ( byte_position = 0; byte_position < I8255M0_PORT_QTY; byte_position++ )
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
							  I8255M0_REGISTER_SIZE_FIXED 
						    );	
		}
		else
		{ /* input port */
			CmpMgr_Reg_Read( component, 
							 byte_position, /* register index or ID */
							 (char *) &(ds->port_internal[byte_position].raw), 
							 I8255M0_REGISTER_SIZE_FIXED 
						 );
			ds->copy[byte_position] = ds->port_internal[byte_position].raw ^ ds->cfg.port[byte_position].polarity;
		}
	}
	return SUCCESS;
}
/***************************************************************
@ingroup i8255m0 
\brief 
Performs a cold or warm restart depending on 'buf'.  
RESTART_COLD - assume a cold power up
RESTART_WARM - CPU reset has just occurred (no power cycling) or software is 
               being restarted, but hardware may have still been running.

During a RESTART_WARM for certain kinds of hardware, one may choose to read back all 
the registers from the hardware to determine its current state and with judicial 
verification adjust the software to the same state as the hardware.  Why do this?  It
provides a scheme whereby we can pick up where we left off.  This is particularly important
if hardware is controlling an ongoing (industrial) process and their had just been
a software restart.

NOTE:      "_CMD_" indicates command table (see cmp_i8255m0_cmd.h) 
		   configuration data structure:  "struct i8255m0_cfg", see cmp_i8255m0.h.
TITLE:     Restart
REQUIRED:  Specific to this component

\param[in] buf    If NULL or points to zero value, then cold restart.
\param[in] index  not used
\return An error code maybe returned, otherwise SUCCESS
*/
static int I8255M0_CMD_Restart( component_type *	component, 
					 size_t				index,
					 char *				buf, 
					 size_t				buf_len 
				   )
{
	size_t						byte_position;
	size_t						operation;
	i8255m0_dataset_type *	ds;
	(void)						buf_len;

	if      ( NULL == buf  ) operation = RESTART_COLD;
	else if ( 0    == *buf ) operation = RESTART_COLD;
	else  	                operation = RESTART_WARM;

	ds = ( i8255m0_dataset_type * ) component->private_data;

	I8255M0_Apply_Directions_Helper( ds );

	switch( operation )
	{	/*--------------------------------*/
		case RESTART_COLD:
/*TODO: test for valid configuration by observing the type value (SYS data type).*/
			CmpMgr_Reg_Write( component, I8255M0_REG_CFG, (char *)&(ds->cfg.port_directions), sizeof(uint8_t) );

			for ( byte_position = 0; byte_position < I8255M0_PORT_QTY; byte_position++ )
			{
				ds->copy[byte_position] = ds->cfg.port[byte_position].init_value;
			}
			
/*TODO: we could fix up the bit map here if it all turns out to be null??*/
			I8255M0_CMD_Update( component,
								index,
								buf,
								buf_len
							  );
			break;
		/*--------------------------------*/
		case RESTART_WARM:
			CmpMgr_Reg_Write( component, I8255M0_REG_CFG, (char *)&(ds->cfg.port_directions), sizeof(uint8_t) );

			for ( byte_position = 0; byte_position < I8255M0_PORT_QTY; byte_position++ )
			{
				CmpMgr_Reg_Read(	component, 
								byte_position, 
								(char *) &(ds->port_internal[byte_position].raw), 
								I8255M0_REGISTER_SIZE_FIXED 
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
/***************************************************************
@ingroup i8255m0 
\brief 
Top level function used to initialize the components dataset.

NOTE:      top level function
TITLE:     Initialize
REQUIRED:  Optional
*/
int I8255M0_Config_Initialize( i8255m0_cfg_type * cfg )
{
	Mem_Set( cfg, 0, sizeof( i8255m0_cfg_type ) );
	cfg->type = SYS_TYPE_MAKE( i8255m0_cfg_type );
	cfg->port_directions = I8255M0_PDIR_AIN__BIN__CIN;
	return SUCCESS;
}
/***************************************************************
@ingroup i8255m0 
\brief 
Top level function, typically called by device open functions to 
instantiate this component.

NOTE:      top level function
TITLE:     Create
REQUIRED:  Yes
*/
int I8255M0_Create( component_type * component, size_t instance )
{
	int						error_code;
	i8255m0_dataset_type *	ds;
	size_t					mem_size;
	void *					mem;

	mem_size = sizeof( i8255m0_dataset_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_I8255M0_DATASET_ALLOCATE;  goto I8255M0_Create_Error; }
	Mem_Set( mem, 0, mem_size );
	ds = (i8255m0_dataset_type *) mem;
	ds->type = SYS_TYPE_MAKE( i8255m0_dataset_type );
	
	error_code = I8255M0_Config_Initialize( &(ds->cfg) );
	if ( error_code ) goto I8255M0_Create_Error;

	component->definition   = &component_i8255m0_definition;
	component->physical_id  = IOC_CREATE_PHYSICAL_ID_COMPONENT( COMPONENT_I8255M0, instance );
	component->offset       = 0	   /* fill in at device level */;
	component->private_data = ds;  /* assign private data */
	component->status       = component->status | CMP_STS_BUILD(0,1,0,0,0); /* private data assigned */
	return SUCCESS;
I8255M0_Create_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8255M0_NAME, __func__, __LINE__, error_code );
	return -error_code;
} 
/***************************************************************
@ingroup i8255m0 
\brief 
Top level function, typically called by device close functions to 
remove this component.

NOTE:      top level function
TITLE:     Destroy
REQUIRED:  Yes
*/
int I8255M0_Destroy( component_type * component )
{
	int error_code;

	if ( NULL == component ) return SUCCESS;
	
	if ( NULL != component->definition ) 
	{ /* if it is null, we will just have to assume we are working with the 
	     correct data type.
	  */
		if ( SYS_TYPE_MAKE(component_definition_type) != component->definition->type )
		{  error_code = EC_I8255M0_COMPONENT_INVALID;	goto I8255M0_Destroy_Error;  }
	}
	if ( NULL != component->private_data )
	{
		Mem_Free( NULL, &(component->private_data) );
		component->private_data = NULL;
	}
	return SUCCESS;
I8255M0_Destroy_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_I8255M0_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************/
/*****************************************************************/
/***************************************************************
@ingroup i8255m0 
\brief 
Function jump table. Refer to cmp_i8255m0_cmd.h.
It has become standard practice that all functions that are used within
this jump table have a "_CMD_" contained within a function name. It is 
not required, but it makes readability and understanding the code a bit
easier as the "_CMD_" reminds you that the function resides in the jump
table and will have a corresponding command code which can be called 
using the AUL_Ioctl() function at the top level.

TITLE:     Jump table
REQUIRED:  Yes
*/
static cmp_fnc i8255m0_jump_table[] =
{
	COMPONENT_I8255M0_CMD_DEFINITIONS( CMD_CMP_EXTRACT_JUMP_TABLE )
};
/***************************************************************
@ingroup i8255m0 
\brief 
This is used by the "cmp_avail_module.h" to indicate that this component
is available for instantiation.  Further, this information flows up into
the "module.c" (search for COMPONENT_AVAIL_MODULES_V2 ) which becomes part
of the big module listing. 


NOTE:      Components are considered "Component Modules"
TITLE:     Definition
REQUIRED:  Yes
*/
component_definition_type component_i8255m0_definition =
{
	SYS_TYPE_MAKE(component_definition_type),
	i8255m0_register_table,
	i8255m0_jump_table,
	I8255M0_JUMP_TABLE_QTY,
#if defined( TARGET_CHECK_BUILD )
	COMPONENT_I8255M0_NAME,
#endif
	I8255M0_MODULE_REV,
	I8255M0_COMMAND_REV,
	I8255M0_STATUS_REV
};

/* <INTERNAL_BEGiN> */
/*****************************************************************/
/*

At device level, we might have the following:
do.map[0].cmp_index = index to the first component supporting do[0]...do[m-1]
do.map[1].cmp_index = index to the next component supporting do[m]...do[n-1]
do.map[2].cmp_index = ...and so on....

Might also pre-compute to save time at upper level:
do.map[0].cmp_qty = total do supported.

So, to determine the channel location (DO bit location):
1. determine the component location, using totals.  The 
   components may not be contiguous in a list, so from 
	capability report from each component, the device would 
	have to determine the mapping...OK, so devices get a bit
	more complex, but just a little bit.
2. determine port location within the component (if required)
3. determine bit location within the component (if required)
Note: #2 and #3 could be done by simply calling a function in the
      component to set these values or just pass it and let it 
		figure out exact location....might be bit-banded which then
		is trivial.
--> this can be repeated with multiple devices too!

capabilities list, something like the following;

component_capability_type i8255m0_capability = 
{
	type       = SYS_TYPE_MAKE( DIO_CAPABLE );
	private_data = (void *) dio_capable_type 
							channel    = 24;
							port_width = 8;
							?

I8255M0:
	do.qty			= 24;
	do.group.min	=  8;  // direction to 8-bits at a time.
	do.group.max	=  8;
	do.overlap_di	= TRUE;
	di.qty			= 24;
	di.group.min   =  8;
	di.group.max   =  8;
	di.overlap_do	= TRUE;
	dio.qty			= 24;
	dio.overlap		= TRUE;
STX104:
	do.qty			=  4;
	do.group.min	=  4;
	do.group.max	=  4;
	do.overlap_di	= FALSE;
	di.qty			=  4;
	di.group.min   =  4;
	di.group.max   =  4;
	di.overlap_do	= FALSE;
	dio.qty			=  0;
	dio.overlap		= FALSE;

NEW DIO BOARD:
	do.qty			=  64;
	do.group.min	=   1;  // direction to 1-bits at a time.
	do.group.max	=  16;  // max port size is 16-bits
	do.overlap_di	= TRUE;
	di.qty			=  64;
	di.group.min   =   1;
	di.group.max   =  16;
	di.overlap_do	= TRUE;
	dio.qty			=  64;
	dio.overlap		= TRUE;
YUKON:
	do.qty			=  4;
	do.group.min	=  1;  // direction to 1-bits at a time.
	do.group.max	=  1;
	do.overlap_di	= TRUE;
	di.qty			=  4;
	di.group.min   =  1;
	di.group.max   =  1;
	di.overlap_do	= TRUE;
	dio.qty			=  4;
	dio.overlap		= TRUE;

Do we really need to indicate any of this??
	How to indicate di/do overlaps?
	How to indicate port arrangement and their respective overlaps.

From application stand point all I want to know is how many DI and DO
are available? If they overlap.  

Example, I am looking for 3 DI and 4 DO.

COMPONENT: 
di_ports = ( di / di.group.min ); 
do_ports = ( do / do.group.min );
if ( ( di > 0 ) && ( 0 == di_ports ) ) di_ports++;
if ( ( do > 0 ) && ( 0 == do_ports ) ) do_ports++;

if ( dio.overlap ) 
{ 
	di_bits = di_ports * do.group.min;
	do_bits = do_ports * do.group.min;
	dio_bits = di_bits + do_bits;
}

if ( ( di_bits <= di.qty )                                 && 
     ( do_bits <= do.qty )                                 && 
     ( ( dio_bits <= dio.qty ) && ( TRUE == dio.overlap )  )
{
	// we are OK!
}
else
{
	// does not work 
}

----------------------------
NOTE:

As a result of this, the driver will have to either tell you if a
configuration is satisfied that meets the needs above, or the 
driver will have to return with a configuration along with a 
wiring map.  Sounds like Advantage!!!


}
*/

/* <INTERNAL_END> */

