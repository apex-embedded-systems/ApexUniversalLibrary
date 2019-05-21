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


/*******************************************************************//**
 * @brief Tracer-E Device APIs and parameters
 *
 * @section COPYRIGHT
 * copyright (c) 2014 by Apex Embedded Systems. All rights reserved.
 */
/*******************************************************************//**
 *  @defgroup tracere Tracer-E Device Functions
 *  Tracer-E API access functions.  These are APIs which are device specific and
 *  are additional functions used for this device.  Note that it is possible to
 *  access the same functionality using AUL_Ioctl() using many of the CMD_TRACERE_<command>
 *  specific Tracer-E device enumerated commands.
 */
/*******************************************************************//**
 *  @defgroup tracere_general General
 *  @ingroup tracere
 *  Tracer-E general configuration and miscellaneous functions.
 */
/*******************************************************************//**
 *  @defgroup tracere_led LED Selection
 *  @ingroup tracere
 *  Tracer-E LED related functions and parameters.
 */
/*******************************************************************//**
 * @defgroup tracere_counter_timer Counter/Timer
 *  @ingroup tracere
 *  Tracer-E I8254 Counter/Timer functions.
 */
/*******************************************************************//**
 *  @defgroup tracere_intr Interrupts
 *  @ingroup tracere
 *  Functions related specifically to Tracer-E Interrupts.
 */
/*******************************************************************//**
 *  @defgroup tracere_dio Digital I/O
 *  @ingroup tracere
 *  Tracer-E I8255 mode-0 functions.
 */



#include "dev_tracere.h"
#include "../../shared/module.h"	/* Error_Log(), AUL_Board(), status, mem, cmd, devmgr */
/* #include "../../shared/status.h" */
#include "../../io/io.h"
/* #include "../../shared/cmd.h" */		/* Ioctl() commands				*/
/* #include "../../devmgr/devmgr.h" */

/*****************************************************************************/
#define TRACERE_MODULE_REV  "$Date: 2015-02-01 15:12:58 -0600 (Sun, 01 Feb 2015) $"

/*****************************************************************************/
/* if DEBUG then allow IO_DEBUG */
#define TRACERE_DEBUG			_DEBUG
/*****************************************************************************/
#define TRACERE_DEBUG_LOG		DEBUG_OUT

/*****************************************************************************/
#define TRACERE_I8255M0_QTY 2
/*****************************************************************************/
enum { TRACERE_COMPONENT_QTY	= ( TRACERE_I8255M0_QTY + 1 /* I8254 */ ) };

/*****************************************************************************/
static const char * tracere_dio_connector_pin_name[TRACERE_DIO_BIT_QTY] = 
{
   "J12.15",	"J12.13",	"J12.11",	"J12.9 ",
   "J12.7 ",	"J12.5 ",	"J12.3 ",	"J12.1 ",

   "J12.47",	"J12.45",	"J12.43",	"J12.41",
   "J12.39",	"J12.37",	"J12.35",	"J12.33",

   "J12.31",	"J12.29",	"J12.27",	"J12.25",
   "J12.23",	"J12.21",	"J12.19",	"J12.17",

   "J9.15",	"J9.13",	"J9.11",	"J9.9 ",
   "J9.7 ",	"J9.5 ",	"J9.3 ",	"J9.1 ",

   "J9.47",	"J9.45",	"J9.43",	"J9.41",
   "J9.39",	"J9.37",	"J9.35",	"J9.33",

   "J9.31",	"J9.29",	"J9.27",	"J9.25",
   "J9.23",	"J9.21",	"J9.19",	"J9.17"
};
/*****************************************************************************/
static const char * tracere_ct_clk_src_name[I8254_CT_QTY][8] =
{	
	{ /*dev_tracere.h CT0 */
		"CT_CLK0 J13.1",	
		"CT_CLK1 J13.2",
		"CT_OUT2      ",
		"IRQ_IN2 J13.8",
		"4 MHz        ",
		"10 MHz       ",
		"20 MHz       ",
		"40 MHz       "
	},
	{ /* CT1 */
		"CT_CLK1 J13.2",	
		"CT_OUT0 J13.5",
		"CT_OUT2      ",
		"IRQ_IN2 J13.8",
		"4 MHz        ",
		"10 MHz       ",
		"20 MHz       ",
		"40 MHz       "
	},
	{ /* CT2 */
		"CT_CLK2 J13.7",	
		"CT_OUT0 J13.5",
		"CT_OUT1 J13.6",
		"IRQ_IN2 J13.8",
		"4 MHz        ",
		"10 MHz       ",
		"20 MHz       ",
		"40 MHz       "
	}
};
/*****************************************************************************/
/* TODO: */
static const char * tracere_intr_src_name[TRACERE_INTR_QTY][8] =
{	
	{ /* Interrupt A (ISA) */
		"CT_OUT0 J13.5",	
		"CT_OUT1 ",
		"CT_OUT2      ",
		"IRQ_IN0",
		"IRQ_IN1",
		"IRQ_IN2",
		"I8255M0.C0 J12.31",
		"40 MHz       "
	},
	{ /* Interrupt B (ISB) */
		"CT_CLK1 J13.2",	
		"CT_OUT0 J13.5",
		"CT_OUT2      ",
		"IRQ_IN2 J13.8",
		"4 MHz        ",
		"10 MHz       ",
		"20 MHz       ",
		"40 MHz       "
	},
	{ /* CT2 */
		"CT_CLK2 J13.7",	
		"CT_OUT0 J13.5",
		"CT_OUT1      ",
		"IRQ_IN2 J13.8",
		"4 MHz        ",
		"10 MHz       ",
		"20 MHz       ",
		"40 MHz       "
	}
};

/*****************************************************************************/
struct tracere_dataset
{	
	SYS_TYPE			type;
	tracere_cfg_type	cfg;
};
typedef struct tracere_dataset tracere_dataset_type;


/*****************************************************************************/
#if(0)
/*****************************************************************************/
enum { TRACERE_LOCATE_PORT = 0, TRACERE_LOCATE_BIT = 1 };
/*****************************************************************************/
int Tracere_DIO_Capabilities( dio_capable_type * capable )
{
	capable.type = SYS_TYPE_MAKE(dio_capable_type);

	capable.dio.qty					= 48;
	capable.dio.overlap_din_dout	= true;

	capable.dio.port.bits_min		=  8; /* contiguous port bits */
	capable.dio.port.bits_max		=  8;

	capable.dout.qty				= 48;
	capable.din.qty					= 48;
	
	return SUCCESS;
}
/*****************************************************************************/
int Tracere_DIO_Config_Set( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
}
#define
/*****************************************************************************/
struct tracere_cfg
{
	SYS_TYPE	type;
	uint8_t		port_directions;
	struct
	{
		uint8_t		map;
	} dio[TRACERE_DIO_QTY];

	uint8_t			port_width;
	struct
	{
		uint8		map;
		uint8		
	} port[
};
typedef struct tracere_cfg tracere_cfg_type;



/*****************************************************************************/
int Tracere_DIO_Config_Initialize( tracere_cfg_type * cfg )
{
	size_t	channel;

	if ( NULL == cfg ) return -1;


	Mem_Set( cfg, 0, sizeof( tracere_cfg_type ) );
	cfg->type = SYS_TYPE_MAKE(tracere_cfg_type);

	for ( channel = 0; channel < TRACERE_DIO_QTY; channel++ )
	{
		cfg->dio[channel].map = channel;
	}
}
/*****************************************************************************/
static int Tracere_DIO_Dataset_Initialize( tracere_dataset_type * ds )
{

}

/*****************************************************************************/
static int Tracere_DIO_Config_Set( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	size_t	channel;
	size_t  instance;
	tracere_dataset_type *  ds = ( tracere_dataset_type *) device->private_data;
	tracere_config_type  * cfg = ( tracere_config_type *) buf;
	i8255m0_cfg_type   i8255m0_cfg[TRACERE_I8255M0_QTY];
	(void)	index;
	(void)	buf;
	(void)	buf_len;

	/* verify configuration data integrity */
	for ( channel = 0; channel < TRACERE_DIO_QTY; channel++ )
	{
		if ( cfg->dio[channel].map >= TRACERE_DIO_QTY ) 
		{  error_code = EC_I8255M0_CHANNEL_OUT_OF_RANGE;	goto Tracer_DIO_Config_Error;  }	
	}

	/* commit to dataset */
	for ( channel_logical = 0; channel_logical < TRACERE_DIO_QTY; channel_logical++ )
	{
		ds->dio[channel].i8255m0_instance = cfg->dio[channel].map / I8255M0_BITS_QTY;
		i8255m0_cfg.bit_map[instance] = ds->din_map[channel] - ds->i8255m0_instance * I8255M0_BITS_QTY;
	}
/*
- setup i8255m0
	- port polarities.
	- initial values.


*/
	component  = &(device->component_table[i8255m0_instance]);
	error_code  = CmpMgr_Command( component,		/* info about the component			*/
									command_base,		/* component command for jump table	*/
									i8255m0_channel,	/* index within component				*/
									buf, 
									1 
								);


	return SUCCESS;

Tracer_DIO_Config_Error:
	return -error_code;
}
/*****************************************************************************/
static int Tracere_DIO_Bit(	devmgr_device_type *	device,
							size_t					channel,
							size_t					command_base,
							char *					buf, 
							size_t					buf_len
					      )
{
	size_t					byte_position;
	size_t					bit_position;
	size_t					physical_address;
	uint8_t					mask;
	tracere_dataset_type *  ds = ( tracere_dataset_type *) device->private_data;

	physical_address = ds->map_bit[channel];
	byte_position = physical_address >>   3;
	bit_position  = physical_address & 0x07;





	return SUCCESS;
}
/*****************************************************************************/
static int Tracere_Location(	devmgr_device_type * device,
								component_type *	component_table,
								size_t				channel_in, 
								size_t *			i8255m0_instance_out,
								size_t *			channel_out,
								size_t				mode
							)
{



/* Assumption:  We know how many i8255m0 there are, but if dynamically determined
   we would not know this unless it is stored somewhere.  Indicating this for 
	future DIO implementations where components are determined dynamically (or
	inventoried).
*/
	int error_code = SUCCESS;
	(void) component_table;

	if ( TRACERE_LOCATE_PORT == mode )
	{

		if      ( channel_in >= TRACERE_I8255M0_QTY * I8255M0_PORT_QTY )
		{
			error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;
		}
		else if ( channel_in >= I8255M0_PORT_QTY )
		{
			*i8255m0_instance_out = 1;
			*channel_out = channel_in - I8255M0_PORT_QTY;
		}
		else
		{
			*i8255m0_instance_out = 0;
			*channel_out = channel_in;
		}

	}
	else /* assume locate bit */
	{
		if      ( channel_in >= TRACERE_I8255M0_QTY * I8255M0_BITS_QTY )
		{
			error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;
		}
		else if ( channel_in >= I8255M0_BITS_QTY )
		{
			*i8255m0_instance_out = 1;
			*channel_out = channel_in - I8255M0_BITS_QTY;
		}
		else
		{
			*i8255m0_instance_out = 0;
			*channel_out = channel_in;
		}
	}
	return error_code;
}
/*****************************************************************************/
static int Tracere_DIO(	devmgr_device_type *	device,
						size_t					channel,
						size_t					command_base,
						size_t					mode, 
						char *					buf, 
						size_t					buf_len
					   )
{
	int					error_code;
	size_t				i8255m0_instance;
	size_t				i8255m0_channel;
	component_type *	component;
	(void)				buf_len;

	error_code = Tracere_Location(	device, 
		device->component_table,
									channel,
									&i8255m0_instance, 
									&i8255m0_channel,
									mode
								 );
	if ( SUCCESS == error_code )
	{
		component  = &(device->component_table[i8255m0_instance]);

		error_code  = CmpMgr_Command( component,		/* info about the component			*/
									  command_base,		/* component command for jump table	*/
									  i8255m0_channel,	/* index within component				*/
									  buf, 
									  1 
									);
	}
	return -error_code;
}
#endif





/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/* EITHER LIBRARY or DRIVER LAYER */

/*****************************************************************************/
#include "../../io/io_private.h"  /* IO_COPY_FROM_USER */

/*****************************************************************************/
static int Tracere_CFG_DIO_Set( devmgr_device_type * device, tracere_dio_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;
	component_type *		component;
	i8255m0_cfg_type *		cfg_i8255m0;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_DIO_Set_Error; }
	/* make sure data structure is of correct type */
	if ( SYS_TYPE_MAKE( tracere_dio_cfg_type ) != cfg->type )
	{ error_code = EC_TRACERE_DATA_TYPE_ERROR;  goto Tracere_CFG_DIO_Set_Error;		}
	/* Config for i8255m0 #1 -- get latest */
	component   = &(device->component_table[TRACERE_COMPONENT_I8255M0_0]);
	cfg_i8255m0 = &(cfg->i8255m0_cfg[0]);
	error_code = CmpMgr_Command( component, CMD_I8255M0_CONFIG_SET_BASE, 0, (char *) cfg_i8255m0, sizeof( i8255m0_cfg_type ) );
	if ( error_code ) goto Tracere_CFG_DIO_Set_Error;
	/* Config for i8255m0 #2 -- get latest */
	component   = &(device->component_table[TRACERE_COMPONENT_I8255M0_1]);
	cfg_i8255m0 = &(cfg->i8255m0_cfg[1]);
	error_code = CmpMgr_Command( component, CMD_I8255M0_CONFIG_SET_BASE, 1, (char *) cfg_i8255m0, sizeof( i8255m0_cfg_type ) );
	if ( error_code ) goto Tracere_CFG_DIO_Set_Error;
	/* copy to tracere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( &(ds->cfg.dio), cfg, sizeof( tracere_dio_cfg_type ) );
	return SUCCESS;
Tracere_CFG_DIO_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CFG_DIO_Get( devmgr_device_type * device, tracere_dio_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;
	component_type *		component;
	i8255m0_cfg_type *		cfg_i8255m0;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_DIO_Get_Error; }
	/* pull config from trancere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( cfg, &(ds->cfg.dio), sizeof( ds->cfg.dio ) ); 
	/* Config for i8255m0 #1 -- get the latest */
	component   = &(device->component_table[TRACERE_COMPONENT_I8255M0_0]);
	cfg_i8255m0 = &(cfg->i8255m0_cfg[0]);
	error_code = CmpMgr_Command( component, CMD_I8255M0_CONFIG_GET_BASE, 0, (char *) cfg_i8255m0, sizeof( *cfg_i8255m0 ) );
	if ( error_code ) goto Tracere_CFG_DIO_Get_Error;
	/* Config for i8255m0 #2 -- get the latest */
	component   = &(device->component_table[TRACERE_COMPONENT_I8255M0_1]);
	cfg_i8255m0 = &(cfg->i8255m0_cfg[1]);
	error_code = CmpMgr_Command( component, CMD_I8255M0_CONFIG_GET_BASE, 1, (char *) cfg_i8255m0, sizeof( *cfg_i8255m0 ) );
	if ( error_code ) goto Tracere_CFG_DIO_Get_Error;

	cfg->type = SYS_TYPE_MAKE( tracere_dio_cfg_type );
	return SUCCESS;
Tracere_CFG_DIO_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CFG_CT_Set( devmgr_device_type * device, tracere_ct_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;
	component_type *		component;
	i8254_cfg_type *		cfg_i8254;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_CT_Set_Error; }
	/* */
	if ( SYS_TYPE_MAKE( tracere_ct_cfg_type ) != cfg->type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto Tracere_CFG_CT_Set_Error;	}
	/* make sure data structure is of correct type */
	if ( SYS_TYPE_MAKE( i8254_cfg_type ) != cfg->i8254_cfg.type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto Tracere_CFG_CT_Set_Error;	}
	/* pass configuration down to i8254 */
	component = &(device->component_table[TRACERE_COMPONENT_I8254]);
	cfg_i8254 = &(cfg->i8254_cfg);
	error_code = CmpMgr_Command( component, CMD_I8254_CONFIG_SET_BASE, 0, (char *) cfg_i8254, sizeof( i8254_cfg_type ) );
	if ( error_code ) goto Tracere_CFG_CT_Set_Error;
	/* copy to tracere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( &(ds->cfg.ct), cfg, sizeof( tracere_ct_cfg_type ) );
Tracere_CFG_CT_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CFG_CT_Get( devmgr_device_type * device, tracere_ct_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;
	component_type *		component;
	i8254_cfg_type *		cfg_i8254;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_CT_Get_Error; }
	/* pull config from trancere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( cfg, &(ds->cfg.ct), sizeof( ds->cfg.ct ) ); 
	/* Config for i8254 -- get the latest */
	component = &(device->component_table[TRACERE_COMPONENT_I8254]);
	cfg_i8254 = &(cfg->i8254_cfg);
	error_code = CmpMgr_Command( component, CMD_I8254_CONFIG_GET_BASE, 0, (char *) cfg_i8254, sizeof( *cfg_i8254 ) );
	if ( error_code ) goto Tracere_CFG_CT_Get_Error;

	cfg->type = SYS_TYPE_MAKE( tracere_ct_cfg_type );
	return SUCCESS;
Tracere_CFG_CT_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}


#if(0)
	uint16_t			ctsu;		/* counter/timer setup register */
	/* setup the rest of the hardware */
	ctsu  = 0;
	ctsu |= TRACERE_SET_GT2_INV(  candidate.ct2.gate_invert  );
	ctsu |= TRACERE_SET_CLK2_INV( candidate.ct2.clock_invert ); 
	ctsu |= TRACERE_SET_CLK2(     candidate.ct2.clock_source ); 

	ctsu |= TRACERE_SET_GT1_INV(  candidate.ct1.gate_invert  );
	ctsu |= TRACERE_SET_CLK1_INV( candidate.ct1.clock_invert ); 
	ctsu |= TRACERE_SET_CLK1(     candidate.ct1.clock_source ); 

	ctsu |= TRACERE_SET_GT0_INV(  candidate.ct0.gate_invert  );
	ctsu |= TRACERE_SET_CLK0_INV( candidate.ct0.clock_invert ); 
	ctsu |= TRACERE_SET_CLK0(     candidate.ct0.clock_source );
	ctsu |= TRACERE_SET_GT0(      candidate.ct0.gate_source  );

	uint16_t ctsu;

	candidate.ct2.gate_invert = TRACERE_GET_GT2_INV( ctsu );
	candidate.ct2.clock_invert = TRACERE_GET_CLK2_INV( ctsu ); 
	candidate.ct2.clock_source = TRACERE_GET_CLK2( ctsu ); 

	candidate.ct1.gate_invert = TRACERE_GET_GT1_INV( ctsu );
	candidate.ct1.clock_invert = TRACERE_GET_CLK1_INV( ctsu ); 
	candidate.ct1.clock_source = TRACERE_GET_CLK1( ctsu ); 

	candidate.ct0.gate_invert = TRACERE_GET_GT0_INV( ctsu );
	candidate.ct0.clock_invert = TRACERE_GET_CLK0_INV( ctsu ); 
	candidate.ct0.clock_source = TRACERE_GET_CLK0( ctsu );
	candidate.ct0.gate_source = TRACERE_GET_GT0( ctsu );
#endif

/*****************************************************************************/
static int Tracere_CFG_Intr_Set( devmgr_device_type * device, tracere_intr_cfg_type * cfg )
{
	int	error_code = EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
	(void) device;
	(void) cfg;

/*Tracere_CFG_Intr_Set_Error:*/
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CFG_Intr_Get( devmgr_device_type * device, tracere_intr_cfg_type * cfg )
{
	int	error_code = EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
	(void) device;
	(void) cfg;

/*Tracere_CFG_Intr_Get_Error:*/
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CFG_Set( devmgr_device_type * device, tracere_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_Set_Error; }

	if ( SYS_TYPE_MAKE( tracere_cfg_type ) != cfg->type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto Tracere_CFG_Set_Error;	}

	error_code = Tracere_CFG_DIO_Set( device, &(cfg->dio) );
	if ( error_code ) goto Tracere_CFG_Set_Error;

	error_code = Tracere_CFG_CT_Set( device, &(cfg->ct) );
	if ( error_code ) goto Tracere_CFG_Set_Error;

	ds = (tracere_dataset_type *) device->private_data;
	ds->cfg.led = cfg->led; 
	return SUCCESS;
Tracere_CFG_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CFG_Get( devmgr_device_type * device, tracere_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_Get_Error; }
	/* pull config from trancere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( cfg, &(ds->cfg), sizeof( tracere_cfg_type ) );

	error_code = Tracere_CFG_DIO_Get( device, &(cfg->dio) );
	if ( error_code ) goto Tracere_CFG_Get_Error;

	error_code = Tracere_CFG_CT_Get( device, &(cfg->ct) );
	if ( error_code ) goto Tracere_CFG_Get_Error;

	cfg->type = SYS_TYPE_MAKE( tracere_cfg_type );
	return SUCCESS;
Tracere_CFG_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}


/*****************************************************************************/
static int Tracere_CMD_NOP( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
#if defined( TRACERE_DEBUG_LOG )
	Debug_Log( TRACERE_DEBUG_LOG "%s\n", __func__ );
#endif
	return SUCCESS;
}
/*****************************************************************************/
static int Tracere_CMD_Initialize( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
/*acquire io-space maps and such.
allocate device data sets?*/
	return -EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
}
/*****************************************************************************/
static int Tracere_CMD_Terminate( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return -EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
}
/*****************************************************************************/
static int Tracere_CMD_Power_Up( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return -EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
}
/*****************************************************************************/
static int Tracere_CMD_Power_Down( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return -EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
}
/*****************************************************************************/
static int Tracere_CMD_Close( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int					error_code;
	component_type *	component_table;
	(void) index;
	(void) buf;
	(void) buf_len;

	component_table = device->component_table;
	/* NOTE: <component>_Destroy will null the pointer when complete */
	/* remove i8254 instance 0 private data */
	I8254_Destroy( &(component_table[TRACERE_COMPONENT_I8254]) );
	/* remove i8255m0 instance 1 private data */
	I8255M0_Destroy( &(component_table[TRACERE_COMPONENT_I8255M0_1]) ); 
	/* remove i8255m0 instance 0 private data */
	I8255M0_Destroy( &(component_table[TRACERE_COMPONENT_I8255M0_0]) ); 
	/* NOTE: Mem_Free will null the pointer when it is done */
	device->component_table_qty = 0;
	if ( NULL != device->component_table ) Mem_Free( NULL, (void **) &(device->component_table) );
	if ( NULL != device->private_data    ) Mem_Free( NULL, (void **) &(device->private_data)    );
	return SUCCESS;
/* Tracere_CMD_Close_Error: */
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_Open( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int						error_code;
	void *					mem;
	component_type *		component_table;
	component_type *		cmp;
	tracere_dataset_type *	ds;      /* ds = data set */
	size_t					mem_size;
	(void)					index;
	(void)					buf;
	(void)					buf_len;
	
	error_code = SUCCESS;
/* Allocate memory for the components list. */
	mem_size = sizeof( component_type ) * (TRACERE_COMPONENT_QTY + 1 /* NULL */);
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_TRACERE_COMPONENT_ALLOCATE;  goto Tracere_CMD_Open_Error; }
	Mem_Set( mem, 0, mem_size );
	component_table = (component_type *) mem;

/* Create and add components to our list */
	/* Create i8255m0 #1 component and link as the first component. */
	cmp = &(component_table[TRACERE_COMPONENT_I8255M0_0]);
	error_code = I8255M0_Create( cmp, 0 /* instance */ );
	if ( error_code ) goto Tracere_CMD_Open_Error;
	cmp->component_link = NULL;	/* using direct I/O, no indirect addressing */
	cmp->offset			= device->io_region_offset + 0 /* i8255m0 local offset */;
	cmp->iod			= device->io_device;
	/* Offset assigned. Note that other parameters could have been assigned. */
	cmp->status			= cmp->status | CMP_STS_BUILD(1,0,0,0,0);	
	
	/* Create i8255m0 #2 component and link as the second component. */
	cmp = &(component_table[TRACERE_COMPONENT_I8255M0_1]);
	error_code = I8255M0_Create( cmp, 1 /* instance */ );
	if ( error_code ) goto Tracere_CMD_Open_Error;
	cmp->component_link = NULL;	/* using direct I/O, no indirect addressing */
	cmp->offset			= device->io_region_offset + 4 /* i8255m0 local offset */;
	cmp->iod			= device->io_device;
	/* Offset assigned. Note that other parameters could have been assigned. */
	cmp->status			= cmp->status | CMP_STS_BUILD(1,0,0,0,0);	

	/* Create i8254 component and link */
	cmp = &(component_table[TRACERE_COMPONENT_I8254]);
	error_code = I8254_Create( cmp, 0 /* instance */ );
	if ( error_code ) goto Tracere_CMD_Open_Error;
	cmp->component_link = NULL;	/* using direct I/O, no indirect addressing */
	cmp->offset			= device->io_region_offset + 8 /* i8255m0 local offset */;
	cmp->iod			= device->io_device;
	/* Offset assigned. Note that other parameters could have been assigned. */
	cmp->status			= cmp->status | CMP_STS_BUILD(1,0,0,0,0);	

	/* Create interrupt component and link */

/* Allocate and create our internal data set */
	mem_size = sizeof( tracere_dataset_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_TRACERE_PRIVATE_ALLOCATE;  goto Tracere_CMD_Open_Error;  }
	/* initialize the dataset */
	Mem_Set( mem, 0, mem_size );
	ds = (tracere_dataset_type *) mem;
	ds->type					= SYS_TYPE_MAKE( tracere_dataset_type );
	device->component_table     = component_table;
	device->component_table_qty = TRACERE_COMPONENT_QTY;
	device->private_data        = ds;
	if ( error_code ) goto Tracere_CMD_Open_Error;
	return SUCCESS;
Tracere_CMD_Open_Error:
	Tracere_CMD_Close( device, 0, NULL, 0 );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_Config_Set( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int			error_code;
	SYS_TYPE	type;
	(void)		index;
	(void)		buf_len;

	error_code = EC_TRACERE_CONFIG_TYPE_NOT_FOUND;
	type = *( ( SYS_TYPE *) buf );
	switch ( type )
	{
		case SYS_TYPE_MAKE( tracere_cfg_type ):
			{
				tracere_cfg_type candidate;
				if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
				{ error_code = EC_IO_COPY_FROM_USER;	goto Tracere_CMD_Config_Set_Error;	 }
				error_code = Tracere_CFG_Set( device, &candidate );
			}
			break;
		case SYS_TYPE_MAKE( tracere_dio_cfg_type ):
			{
				tracere_dio_cfg_type candidate;
				if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
				{ error_code = EC_IO_COPY_FROM_USER;	goto Tracere_CMD_Config_Set_Error;	 }
				error_code = Tracere_CFG_DIO_Set( device, &candidate );
			}
			break;
		case SYS_TYPE_MAKE( tracere_ct_cfg_type ):
			{
				tracere_ct_cfg_type candidate;
				if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
				{ error_code = EC_IO_COPY_FROM_USER;	goto Tracere_CMD_Config_Set_Error;	 }
				error_code = Tracere_CFG_CT_Set( device, &candidate );
			}
			break;
		case SYS_TYPE_MAKE( tracere_intr_cfg_type ):
			{
				tracere_intr_cfg_type candidate;
				if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
				{ error_code = EC_IO_COPY_FROM_USER;	goto Tracere_CMD_Config_Set_Error;	 }
				error_code = Tracere_CFG_Intr_Set( device, &candidate );
			}
			break;
		default:
			break;
	}
	if ( error_code ) goto Tracere_CMD_Config_Set_Error;
	return SUCCESS;
Tracere_CMD_Config_Set_Error:
	Tracere_CMD_Close( device, 0, NULL, 0 );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_Config_Get( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int			error_code;
	SYS_TYPE	type;
	(void)		index;
	(void)		buf_len;

	error_code = EC_TRACERE_CONFIG_TYPE_NOT_FOUND;
	type = *( ( SYS_TYPE *) buf );
	switch ( type )
	{
		case SYS_TYPE_MAKE( tracere_cfg_type ):
			{
				tracere_cfg_type  cfg;
				error_code = Tracere_CFG_Get( device, &cfg );
				if ( IO_COPY_TO_USER( buf, &cfg, sizeof( tracere_cfg_type ) ) )
				{ error_code = EC_IO_COPY_TO_USER;	goto Tracere_CMD_Config_Get_Error;	 }
			}
			break;
		case SYS_TYPE_MAKE( tracere_dio_cfg_type ):
			{
				tracere_dio_cfg_type  cfg;
				error_code = Tracere_CFG_DIO_Get( device, &cfg );
				if ( IO_COPY_TO_USER( buf, &cfg, sizeof( tracere_dio_cfg_type ) ) )
				{ error_code = EC_IO_COPY_TO_USER;	goto Tracere_CMD_Config_Get_Error;	 }
			}
			break;
		case SYS_TYPE_MAKE( tracere_ct_cfg_type ):
			{
				tracere_ct_cfg_type  cfg;
				error_code = Tracere_CFG_CT_Get( device, &cfg );
				if ( IO_COPY_TO_USER( buf, &cfg, sizeof( tracere_ct_cfg_type ) ) )
				{ error_code = EC_IO_COPY_TO_USER;	goto Tracere_CMD_Config_Get_Error;	 }
			}
			break;
		case SYS_TYPE_MAKE( tracere_intr_cfg_type ):
			{
				tracere_intr_cfg_type  cfg;
				error_code = Tracere_CFG_Intr_Get( device, &cfg );
				if ( IO_COPY_TO_USER( buf, &cfg, sizeof( tracere_intr_cfg_type ) ) )
				{ error_code = EC_IO_COPY_TO_USER;	goto Tracere_CMD_Config_Get_Error;	 }
			}
			break;
		default:
			break;
	}
	if ( error_code ) goto Tracere_CMD_Config_Get_Error;
	return SUCCESS;
Tracere_CMD_Config_Get_Error:
	Tracere_CMD_Close( device, 0, NULL, 0 );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#if(0)
/*****************************************************************************/
static int Tracere_CMD_Connectivity( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;

/* TODO:
1. Build a report indicating which layers we are able to communicate.
2. What do we want to report.
2.a. Can we communicate with the board in question?
2.b. Can we communicate with expected driver?
2.c. Provide some positive feedback to the application that we are indeed
     able to communicate with hardware and all software layers.
*/

	return -EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
}
#endif
/*****************************************************************************/
static int Tracere_CMD_Write( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return -EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
}
/*****************************************************************************/
static int Tracere_CMD_Read( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return -EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
}
/*****************************************************************************/
static int Tracere_CMD_Reg_Write( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{ /* function used to write directly to/from registers */
	int		error_code;
	(void)  device;
	(void)	index;
	(void)  buf;
	(void)	buf_len;
	error_code = IO_Xfr( device->io_device, ( io_xfr_type *) buf );
	return error_code;
}
/*****************************************************************************/
static int Tracere_CMD_Reg_Read( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{ /* function used to write directly to/from registers */
	int		error_code;
	(void)  device;
	(void)	index;
	(void)  buf;
	(void)	buf_len;
	error_code = IO_Xfr( device->io_device, ( io_xfr_type *) buf );
	return error_code;
}
/*****************************************************************************/
static int Tracere_CMD_Restart_DIO( devmgr_device_type * device, char warm )
{
	int					error_code;
	component_type *	component;

	component  = &(device->component_table[TRACERE_COMPONENT_I8255M0_0]); 
	error_code = CmpMgr_Command( component,			/* info about the component			*/
								 CMD_I8255M0_RESTART_BASE,	/* component command for jump table	*/
								 0,					/* channel			  				*/
								 &warm,				/* value we are passing along		*/
								 sizeof( warm )		/* buffer length */
								);
	if ( error_code ) return error_code;
	component  = &(device->component_table[TRACERE_COMPONENT_I8255M0_1]); 
	error_code = CmpMgr_Command( component,			/* info about the component			*/
								 CMD_I8255M0_RESTART_BASE,	/* component command for jump table	*/
								 0,					/* channel			  				*/
								 &warm,				/* value we are passing along		*/
								 sizeof( warm )		/* buffer length */
								);
	return error_code;
}
/*****************************************************************************/
static int Tracere_CMD_Restart_CT( devmgr_device_type * device, char warm )
{
	int					error_code;
	tracere_dataset_type *	ds;
	component_type *	component;
	size_t				address;
	uint16_t			ctsu;
	uint8_t				u8;

	ds = (tracere_dataset_type *) device->private_data;

	ctsu  = 0;
	ctsu |= TRACERE_SET_GT2_INV(  ds->cfg.ct.ct2.gate_invert  );
	ctsu |= TRACERE_SET_CLK2_INV( ds->cfg.ct.ct2.clock_invert ); 
	ctsu |= TRACERE_SET_CLK2(     ds->cfg.ct.ct2.clock_source ); 

	ctsu |= TRACERE_SET_GT1_INV(  ds->cfg.ct.ct1.gate_invert  );
	ctsu |= TRACERE_SET_CLK1_INV( ds->cfg.ct.ct1.clock_invert ); 
	ctsu |= TRACERE_SET_CLK1(     ds->cfg.ct.ct1.clock_source ); 

	ctsu |= TRACERE_SET_GT0_INV(  ds->cfg.ct.ct0.gate_invert  );
	ctsu |= TRACERE_SET_CLK0_INV( ds->cfg.ct.ct0.clock_invert ); 
	ctsu |= TRACERE_SET_CLK0(     ds->cfg.ct.ct0.clock_source );
	ctsu |= TRACERE_SET_GT0(      ds->cfg.ct.ct0.gate_source  );

	address = device->io_region_offset + TRACERE_I8254_CT_CFG_LSB;
	u8      = IO_U16_GET_U8_LO( ctsu );
	IO_Write_U8( device->io_device, address, u8 );

	address = device->io_region_offset + TRACERE_I8254_CT_CFG_MSB;
	u8      = IO_U16_GET_U8_HI( ctsu );
	IO_Write_U8( device->io_device, address, u8 );

	component  = &(device->component_table[TRACERE_COMPONENT_I8254]); 
	error_code = CmpMgr_Command( component,			/* info about the component			*/
				/*TODO*/		 CMD_I8254_RESTART_BASE,	/* component command for jump table	*/
								 0,					/* channel			  				*/
								 &warm,				/* value we are passing along		*/
								 sizeof( warm )		/* buffer length */
							   );
	return error_code;
}
/*****************************************************************************/
static int Tracere_CMD_Restart( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int					  error_code;
	TRACERE_RESTART_ENUM  restart_mode;
	char				  warm;
	(void)	index;
	(void)	buf_len;

	if ( NULL == buf )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CMD_Restart_Error; }

	if ( IO_COPY_FROM_USER( &restart_mode, buf, sizeof( restart_mode ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto Tracere_CMD_Restart_Error;	 }

	error_code = SUCCESS;
	warm = restart_mode & TRACERE_RESTART_WARM;
	switch( restart_mode )
	{
		case TRACERE_RESTART_COLD_DIO:
		case TRACERE_RESTART_WARM_DIO:
			error_code = Tracere_CMD_Restart_DIO( device, warm );
			break;
		case TRACERE_RESTART_COLD_CT:
		case TRACERE_RESTART_WARM_CT:
			error_code = Tracere_CMD_Restart_CT( device, warm );
			break;
		case TRACERE_RESTART_COLD_INTR:
		case TRACERE_RESTART_WARM_INTR:
			break;
		case TRACERE_RESTART_COLD_LED:
		case TRACERE_RESTART_WARM_LED:
			break;
		case TRACERE_RESTART_COLD_ALL:
		case TRACERE_RESTART_WARM_ALL:
			error_code = Tracere_CMD_Restart_DIO( device, warm );
			if ( error_code ) break;
			error_code = Tracere_CMD_Restart_CT( device, warm );
			break;
		default:
			break;
	}
	if ( error_code ) goto Tracere_CMD_Restart_Error;
	return SUCCESS;
Tracere_CMD_Restart_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_DIO_Bit_Set( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int					error_code;
	size_t				cmp_index;
	component_type *	component;
	BOOL				bit_value;
	uint8_t				raw;
	(void)	buf_len; /* unfortunately due to ioctl we cannot depend on this value */

	if ( NULL == buf )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CMD_DIO_Bit_Set_Error; }

	if ( IO_COPY_FROM_USER( &bit_value, buf, sizeof( bit_value ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto Tracere_CMD_DIO_Bit_Set_Error;	 }

	if ( index >= TRACERE_DIO_BIT_QTY )
	{  error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;	goto Tracere_CMD_DIO_Bit_Set_Error;  }

	if ( index >= I8255M0_BITS_QTY ) 
	{
		cmp_index = TRACERE_COMPONENT_I8255M0_1;
		index     = index - I8255M0_BITS_QTY;
	}
	else                             
	{
		cmp_index = TRACERE_COMPONENT_I8255M0_0;
	}

	component  = &(device->component_table[cmp_index]);
	if ( bit_value ) raw = 1;
	else             raw = 0;
	error_code  = CmpMgr_Command( component,			/* info about the component			*/
								  CMD_I8255M0_DO_SET_BASE,	/* component command for jump table	*/
								  index,				/* channel			  				*/
								  (char *) &raw,		/* value we are passing along		*/
								  sizeof( raw )			/* buffer length */
								);
	if ( error_code ) goto Tracere_CMD_DIO_Bit_Set_Error;
	return SUCCESS;
Tracere_CMD_DIO_Bit_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_DIO_Bit_Get( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int					error_code;
	size_t				cmp_index;
	component_type *	component;
	BOOL				bit_value;
	uint8_t				raw;
	(void)	buf_len; /* unfortunately due to ioctl we cannot depend on this value */

	if ( NULL == buf )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CMD_DIO_Bit_Get_Error; }

	if ( index >= TRACERE_DIO_BIT_QTY )
	{  error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;	goto Tracere_CMD_DIO_Bit_Get_Error;  }

	if ( index >= I8255M0_BITS_QTY ) 
	{
		cmp_index = TRACERE_COMPONENT_I8255M0_1;
		index     = index - I8255M0_BITS_QTY;
	}
	else                             
	{
		cmp_index = TRACERE_COMPONENT_I8255M0_0;
	}

	component  = &(device->component_table[cmp_index]);
	error_code  = CmpMgr_Command( component,				/* info about the component			*/
								  CMD_I8255M0_DI_GET_BASE,	/* component command for jump table	*/
								  index,					/* channel			  				*/
								  (char *) &raw,			/* value we are passing along		*/
								  sizeof( raw )				/* buffer length */
								);
	if ( error_code ) goto Tracere_CMD_DIO_Bit_Get_Error;
	if ( raw ) bit_value = true;
	else       bit_value = false;

	if ( IO_COPY_TO_USER( buf, &bit_value, sizeof( bit_value ) ) )
	{ error_code = EC_IO_COPY_TO_USER;	goto Tracere_CMD_DIO_Bit_Get_Error;	 }
	return SUCCESS;
Tracere_CMD_DIO_Bit_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_DIO_Port_Set( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int					error_code;
	size_t				cmp_index;
	component_type *	component;
	uint8_t				port_value;
	(void)	buf_len; /* unfortunately due to ioctl we cannot depend on this value */

	if ( NULL == buf )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CMD_DIO_Port_Set_Error; }

	if ( IO_COPY_FROM_USER( &port_value, buf, sizeof( port_value ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto Tracere_CMD_DIO_Port_Set_Error;	 }

	if ( index >= TRACERE_DIO_PORT_QTY )
	{  error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;	goto Tracere_CMD_DIO_Port_Set_Error;  }

	if ( index >= I8255M0_PORT_QTY )
	{
		cmp_index = TRACERE_COMPONENT_I8255M0_1;
		index = index - I8255M0_PORT_QTY;
	}
	else
	{
		cmp_index = TRACERE_COMPONENT_I8255M0_0;
	}

	component  = &(device->component_table[cmp_index]);
	error_code  = CmpMgr_Command( component,				/* info about the component			*/
								  CMD_I8255M0_PORT_SET_BASE,/* component command for jump table	*/
								  index,					/* channel			  				*/
								  (char *) &port_value,		/* value we are passing along		*/
								  sizeof( port_value )		/* buffer length */
								);
	if ( error_code ) goto Tracere_CMD_DIO_Port_Set_Error;
	return SUCCESS;
Tracere_CMD_DIO_Port_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_DIO_Port_Get( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int					error_code;
	size_t				cmp_index;
	component_type *	component;
	uint8_t				port_value;
	(void)	buf_len; /* unfortunately due to ioctl we cannot depend on this value */

	if ( NULL == buf )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CMD_DIO_Port_Get_Error; }

	if ( index >= TRACERE_DIO_PORT_QTY )
	{  error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;	goto Tracere_CMD_DIO_Port_Get_Error;  }

	if ( index >= I8255M0_PORT_QTY ) cmp_index = TRACERE_COMPONENT_I8255M0_1;
	else                             cmp_index = TRACERE_COMPONENT_I8255M0_0;

	component  = &(device->component_table[cmp_index]);
	error_code  = CmpMgr_Command( component,				/* info about the component			*/
								  CMD_I8255M0_PORT_GET_BASE,/* component command for jump table	*/
								  index,					/* channel			  				*/
								  (char *) &port_value,		/* value we are passing along		*/
								  sizeof( port_value )		/* buffer length */
								);
	if ( error_code ) goto Tracere_CMD_DIO_Port_Get_Error;
	if ( IO_COPY_TO_USER( buf, &port_value, sizeof( port_value ) ) )
	{ error_code = EC_IO_COPY_TO_USER;	goto Tracere_CMD_DIO_Port_Get_Error;	 }
	return SUCCESS;
Tracere_CMD_DIO_Port_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_DIO_Update( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return -EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
}
/*****************************************************************************/
#define TRACERE_LED_SHIFT_GENERAL_MSB  4
/*****************************************************************************/
static int Tracere_CMD_LED_Set( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int						error_code;
	size_t					address;
	TRACERE_LED_ENUM		led_source;
	/* tracere_dataset_type *	ds; */
	uint8_t					value_reg;
	(void)	index;
	(void)	buf_len;
	/* ds = (tracere_dataset_type *) device->private_data; */
	if ( NULL == buf )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CMD_LED_Set_Error; }

	if ( IO_COPY_FROM_USER( &led_source, buf, sizeof( led_source ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto Tracere_CMD_LED_Set_Error;	 }

/* TODO: this really ought to be a software component? */
	if ( led_source > TRACERE_LED_ON )
	{ error_code = EC_TRACERE_RANGE_SOURCE_LED;  goto Tracere_CMD_LED_Set_Error;	}

	address = device->io_region_offset + TRACERE_GENERAL_MSB;
	IO_Read_U8( device->io_device, address, &value_reg );
	value_reg = value_reg & ( ~( TRACERE_MASK_LED << TRACERE_LED_SHIFT_GENERAL_MSB ) );
/*TODO: bit string manipulators for this register */
	value_reg = (uint8_t )( ( ( led_source & TRACERE_MASK_LED ) << TRACERE_LED_SHIFT_GENERAL_MSB ) | value_reg );
	IO_Write_U8( device->io_device, address, value_reg );
	/* TODO: IO_Write_U8( device->io_device, address, value_reg ); */
	return SUCCESS;
Tracere_CMD_LED_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_LED_Get( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int						error_code;
	size_t					address;
	TRACERE_LED_ENUM		led_source;
	/* tracere_dataset_type *	ds; */
	uint8_t					value_reg;
	(void)	index;
	(void)	buf_len;
	/* ds = (tracere_dataset_type *) device->private_data; */
	if ( NULL == buf )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CMD_LED_Get_Error; }
/* TODO: this really ought to be a software component */
	address = device->io_region_offset + TRACERE_GENERAL_MSB;
	IO_Read_U8( device->io_device, address, &value_reg );
/*TODO: bit string manipulators for this register */
	led_source = (TRACERE_LED_ENUM)( ( value_reg >> TRACERE_LED_SHIFT_GENERAL_MSB ) & TRACERE_MASK_LED );
	if ( IO_COPY_TO_USER( buf, &led_source, sizeof( led_source ) ) )
	{ error_code = EC_IO_COPY_TO_USER;	goto Tracere_CMD_LED_Get_Error;	 }
	return SUCCESS;
Tracere_CMD_LED_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int Tracere_CMD_I8254_Transfer( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int						error_code;
	component_type *		component;
	/* tracere_dataset_type *	ds; */
	(void)	index;
	(void)	buf_len;
	/* ds = (tracere_dataset_type *) device->private_data; */
	if ( NULL == buf )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CMD_I8254_Transfer_Error; }
	component    = &(device->component_table[TRACERE_COMPONENT_I8254]);
	error_code   = CmpMgr_Command( component,				/* info about the component			*/
								   CMD_I8254_CT_XFR_BASE,	/* component command for jump table	*/
								   index,					/* channel			  				*/
								   buf,		/* value we are passing along		*/
								   buf_len	/* buffer length */
								 );
	if ( error_code ) goto Tracere_CMD_I8254_Transfer_Error;
	return SUCCESS;
Tracere_CMD_I8254_Transfer_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
dev_fnc tracere_jump_table[] =
{
	DEVICE_TRACERE_CMD_DEFINITIONS( CMD_DEV_EXTRACT_JUMP_TABLE )
};

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
@ingroup tracere_dio
@brief
Write the data at a digital I/O 8-bit port.\n
Port 0 is associated with J12.I8255M0.PortA.\n
Port 1 is associated with J12.I8255M0.PortB.\n
Port 2 is associated with J12.I8255M0.PortC.\n
Port 3 is associated with J9.I8255M0.PortA.\n
Port 4 is associated with J9.I8255M0.PortB.\n
Port 5 is associated with J9.I8255M0.PortC.\n

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] channel The port number, range 0 to (::TRACERE_DIO_PORT_QTY - 1).
@param[in] value A pointer to an unsigned char that will be used to set the DIO port configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Port_Set( int board, int channel, uint8_t   value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Port_Set_Error;
	error_code = Tracere_CMD_DIO_Port_Set( device, channel, (char *) &value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_DIO_Port_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_dio
@brief
Read the data at a digital I/O 8-bit port.\n
Port 0 is associated with J12.I8255M0.PortA.\n
Port 1 is associated with J12.I8255M0.PortB.\n
Port 2 is associated with J12.I8255M0.PortC.\n
Port 3 is associated with J9.I8255M0.PortA.\n
Port 4 is associated with J9.I8255M0.PortB.\n
Port 5 is associated with J9.I8255M0.PortC.\n

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] channel The port number, range 0 to (::TRACERE_DIO_PORT_QTY - 1).
@param[out] value A pointer to an unsigned char that will be used to retrieve the DIO port configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Port_Get( int board, int channel, uint8_t * value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Port_Get_Error;
	error_code = Tracere_CMD_DIO_Port_Get( device, channel, (char *) value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_DIO_Port_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_dio
Write the digital I/O bit.\n
Bit  0 is associated with J12.I8255M0.PortA.D0.\n
Bit  1 is associated with J12.I8255M0.PortA.D1.\n
...\n
Bit 47 is associated with J9.I8255M0.PortC.D7\n

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] channel The bit number, range 0 to (::TRACERE_DIO_BIT_QTY  - 1).
@param[in] value A pointer to the ::BOOL structure that will be used to set the DIO bit configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Bit_Set( int board, int channel, BOOL   value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Bit_Set_Error;
	error_code = Tracere_CMD_DIO_Bit_Set( device, channel, (char *) &value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_DIO_Bit_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_dio
@brief
Read the digital I/O bit.\n
Bit  0 is associated with J12.I8255M0.PortA.D0.\n
Bit  1 is associated with J12.I8255M0.PortA.D1.\n
...\n
Bit 47 is associated with J9.I8255M0.PortC.D7\n

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] channel The bit number, range 0 to (::TRACERE_DIO_BIT_QTY  - 1).
@param[out] value A pointer to the ::BOOL structure that will be used to retrieve the DIO bit configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Bit_Get( int board, int channel, BOOL * value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Bit_Get_Error;
	error_code = Tracere_CMD_DIO_Bit_Get( device, channel, (char *) value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_DIO_Bit_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_dio
@brief
Get the pin name of the selected bit.  The pin name will be of the form "J??.??".  The purpose
of this function is to return the connector pin name for purposes of debugging and clarification.\n
Bit  0 is associated with J12.I8255M0.PortA.D0.   Will return "J12.15".\n
Bit  1 is associated with J12.I8255M0.PortA.D1.\n
...\n
Bit 47 is associated with J9.I8255M0.PortC.D7.    Will return "J9.17"\n

@param[in] channel The bit number, range 0 to (::TRACERE_DIO_BIT_QTY  - 1).
@param[out] pin_name A pointer to a string which contains the string of the pin/bit name.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Bit_Connector_Pin_Name_Get( int channel, const char ** pin_name )
{
	int error_code;

	if ( ( channel >= TRACERE_DIO_BIT_QTY ) || ( channel < 0 ) )
	{ error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;  goto Tracere_DIO_Bit_Connector_Pin_Name_Get_Error; }
	*pin_name = tracere_dio_connector_pin_name[channel];
	return SUCCESS;
Tracere_DIO_Bit_Connector_Pin_Name_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_dio
@brief
Set the Tracer-E device digital I/O component configuration.  This
is the configuration of all I8255M0 components.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] config A pointer to the ::tracere_dio_cfg_type structure that will be used to set the DIO configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Config_Set( int board, tracere_dio_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Config_Set_Error;
	error_code = Tracere_CFG_DIO_Set( device, config );
	if ( !error_code ) return error_code;
Tracere_DIO_Config_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_dio
@brief
Get the Tracer-E device digital I/O component configuration.  This
is for all I8255M0 related configuration information.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[out] config A pointer to the ::tracere_dio_cfg_type structure that will be used to retrieve the DIO configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Config_Get( int board, tracere_dio_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Config_Get_Error;
	error_code = Tracere_CFG_DIO_Get( device, config );
	if ( !error_code ) return error_code;
Tracere_DIO_Config_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_dio
@brief
Initializes a Tracer-E digital I/O configuration data structure for first use.

@param[out] config A pointer to the ::tracere_dio_cfg_type structure that will be used to initialize the structure.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Config_Initialize( tracere_dio_cfg_type * config )
{
	int error_code;
	Mem_Set( config, 0, sizeof( tracere_dio_cfg_type ) );
	config->type = SYS_TYPE_MAKE( tracere_dio_cfg_type );
	error_code = I8255M0_Config_Initialize( &(config->i8255m0_cfg[TRACERE_COMPONENT_I8255M0_0]) );
	if ( error_code ) goto Tracere_DIO_Config_Initialize_Error;
	error_code = I8255M0_Config_Initialize( &(config->i8255m0_cfg[TRACERE_COMPONENT_I8255M0_1]) );
	if ( error_code ) goto Tracere_DIO_Config_Initialize_Error;
	return SUCCESS;
Tracere_DIO_Config_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************/
/**********************************************************************************************************/


/**********************************************************************************************************//**
@ingroup tracere_intr
@brief
Set the Tracer-E device interrupt configuration.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] config A pointer to the ::tracere_dio_cfg_type structure that will be used to set the Interrupt configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_Intr_Config_Set( int board, tracere_intr_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_Intr_Config_Set_Error;
	error_code = Tracere_CFG_Intr_Set( device, config );
	if ( !error_code ) return error_code;
Tracere_Intr_Config_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_intr
@brief
Get the Tracer-E device interrupt configuration.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[out] config A pointer to the ::tracere_dio_cfg_type structure that will be used to retrieve the Interrupt configuration information.
@return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_Intr_Config_Get( int board, tracere_intr_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_Intr_Config_Get_Error;
	error_code = Tracere_CFG_Intr_Get( device, config );
	if ( !error_code ) return error_code;
Tracere_Intr_Config_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_intr
@brief
Initializes a Tracer-E interrupt configuration data structure for first use.

@param[out] config A pointer to the ::tracere_intr_cfg_type structure that will be used to initialize the structure.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_Intr_Config_Initialize( tracere_intr_cfg_type * config )
{
	int error_code;

	Mem_Set( config, 0, sizeof( tracere_intr_cfg_type ) );
	config->type = SYS_TYPE_MAKE( tracere_intr_cfg_type );

	return SUCCESS;
/* Tracere_Intr_Config_Initialize_Error: */
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************/
/**********************************************************************************************************/


/**********************************************************************************************************//**
@ingroup tracere_counter_timer
@brief
Set the Tracer-E device counter-timer component configuration.  This is the configuration of the I8254 component.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] config A pointer to the ::tracere_ct_cfg_type structure that will be used to set the Counter/Timer configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_CT_Config_Set( int board, tracere_ct_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_CT_Config_Set_Error;
	error_code = Tracere_CFG_CT_Set( device, config );
	if ( !error_code ) return error_code;
Tracere_CT_Config_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_counter_timer
@brief
Get the Tracer-E device counter-timer component configuration.  This is the configuration of the I8254 component.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[out] config A pointer to the ::tracere_ct_cfg_type structure that will be used to retrieve the Counter/Timer configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_CT_Config_Get( int board, tracere_ct_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_CT_Config_Get_Error;
	error_code = Tracere_CFG_CT_Get( device, config );
	if ( !error_code ) return error_code;
Tracere_CT_Config_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_counter_timer
@brief
Initializes a Tracer-E counter/timer configuration data structure for first use.

@param[out] config A pointer to the ::tracere_ct_cfg_type structure that will be used to initialize the structure.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_CT_Config_Initialize( tracere_ct_cfg_type * config )
{
	int		error_code;
	/*size_t	chan;*/

	Mem_Set( config, 0, sizeof( tracere_ct_cfg_type ) );
	config->type = SYS_TYPE_MAKE( tracere_ct_cfg_type );

	/* setup i8254 */
	error_code = I8254_Config_Initialize( &(config->i8254_cfg) );
	if ( error_code ) goto Tracere_CT_Config_Initialize_Error;
	return SUCCESS;
Tracere_CT_Config_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup  tracere_counter_timer
@brief
This function creates a valid I8254 control word.  The control word of size uint8_t is
returned via the i8254_control_word parameter.  It takes the incoming parameters and
performs all necessary bit string insertions to build a valid I8254 control word.

@param[in] channel The counter/timer channel in which this control word applies.  See ::I8254_CHANNEL_ENUM.
@param[in] rw      Indicates if the control word is a write or read instruction.  See ::I8254_RW_ENUM.
@param[in] mode    Indicates the mode of operation.  See ::I8254_MODE_ENUM.
@param[in] bcd	   Indicates whether the data is of BCD or binary type.  See ::I8254_TYPE_ENUM.
@param[out] i8254_control_word Pointer to the location in which the control word is  to be written.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_I8254_Control_Create( I8254_CHANNEL_ENUM	channel,				
														  I8254_RW_ENUM			rw,	
														  I8254_MODE_ENUM		mode,
														  I8254_TYPE_ENUM		bcd,
														  uint8_t *				i8254_control_word
														)
{
	*i8254_control_word = I8254_CTRL_CREATE( channel,rw,mode,bcd );
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup tracere_counter_timer
@brief
Determines, based on the current I8254 control word proposed, whether or not it is valid
to compute the initial count based on a requested frequency.  Returns SUCCESS if it is
OK to go ahead and compute an initial count based on a requested frequency.  Otherwise,
revert to computing initial count based on a requested time interval.

@param[in]  	control		The constructed I8254 control word that we intend to write to hardware.

@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_I8254_Do_Freq_Hz( uint8_t control  /* i8254 control word	*/ )
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
/**********************************************************************************************************//**
@ingroup  tracere_counter_timer
@brief
For a given set of counter/timer clock and control word inputs and desired output frequency desired, compute the
initial count to be loaded into the timer.  Report the actual frequency and compute the error between desired
frequency and actual frequency.

@param[in] 	requested_hz		Requested frequency in Hertz (cycles per second)
@param[in] 	clock_external_hz	External clock frequency in Hertz, if used.  If not used enter zero.
@param[in] 	ct_clock_source		Counter/Timer clock source.  See ::TRACERE_GATE_CT0_ENUM, ::TRACERE_GATE_CT1_ENUM or ::TRACERE_GATE_CT2_ENUM.
@param[in] 	channel				Counter/Timer channel
@param[in] 	control				Control word that will be written to the I8254 in order to load the inital count.
								 Generally built using ::Tracere_I8254_Control_Create function.
@param[out] initial_count		Pointer to the initial count that is computed.
@param[out] actual_hz			Actual frequency, which maybe different that the requested frequency.
@param[out] error_actual		Error between requested and actual error.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_I8254_Count_Freq_Hz(	double		requested_hz,	/**< requested frequency						*/
														double		clock_external_hz,	/**< default, used if not CTn_nMHz				*/
														int			ct_clock_source,	/**< TRACERE_CLK_CTn_4MHZ _10MHZ _20MHZ _40MHZ 	*/
														int			channel,			/**< CT channel									*/
														uint8_t		control,			/**< i8254 control word							*/
														uint16_t *	initial_count,		/**< i8254 count to load						*/
														double *    actual_hz,			/**< actual output frequency     				*/
														double *    error_actual		/**< computed error, x1.0e6 to get ppm 			*/
													)
{
	double				clk_freq;
	double 				divisor;
	double				scratch;
	int					error_code;
	I8254_MODE_ENUM		mode;
	uint16_t			divisor_u16;
	
	clk_freq = 0.0; /* default */
	switch( channel )
	{ /* OK, this is redundant, but if FPGA firmware were to change, then we need all this */
		case 0:
			switch( ct_clock_source )
			{
				case TRACERE_CLK_CT0_4_MHZ:		clk_freq =  4.0e6;	break;	
				case TRACERE_CLK_CT0_10_MHZ:	clk_freq = 10.0e6;	break;
				case TRACERE_CLK_CT0_20_MHZ:	clk_freq = 20.0e6;	break;
				case TRACERE_CLK_CT0_40_MHZ:	clk_freq = 40.0e6;	break;
				default:						clk_freq = clock_external_hz;	break;
			}
			break;
		case 1:
			switch( ct_clock_source )
			{
				case TRACERE_CLK_CT1_4_MHZ:		clk_freq =  4.0e6;	break;	
				case TRACERE_CLK_CT1_10_MHZ:	clk_freq = 10.0e6;	break;
				case TRACERE_CLK_CT1_20_MHZ:	clk_freq = 20.0e6;	break;
				case TRACERE_CLK_CT1_40_MHZ:	clk_freq = 40.0e6;	break;
				default:/**********************************************************************************************************/
					/**********************************************************************************************************/						clk_freq = clock_external_hz;	break;
			}
			break;
		case 2:
			switch( ct_clock_source )
			{
				case TRACERE_CLK_CT2_4_MHZ:		clk_freq =  4.0e6;	break;	
				case TRACERE_CLK_CT2_10_MHZ:	clk_freq = 10.0e6;	break;
				case TRACERE_CLK_CT2_20_MHZ:	clk_freq = 20.0e6;	break;
				case TRACERE_CLK_CT2_40_MHZ:	clk_freq = 40.0e6;	break;
				default:						clk_freq = clock_external_hz;	break;
			}
			break;
	}

	if ( ( 0.0 == clk_freq ) || ( clk_freq > 40.0e6 ) )
	{ error_code = EC_TRACERE_CLK_OUT_OF_RANGE; goto Tracere_I8254_Count_Freq_Hz_Error; }

	if ( ( requested_hz > clk_freq ) || ( 0.0 == requested_hz ) )
	{ error_code = EC_TRACERE_FREQ_OUT_OF_RANGE; goto Tracere_I8254_Count_Freq_Hz_Error; }

	mode = I8254_CTRL_GET_MODE( control );
	switch( mode )
	{
		case I8254_MODE_RATE_GENERATOR:
			divisor = clk_freq / requested_hz;
			break;
		case I8254_MODE_SQUARE_WAVE_GENERATOR:
			if ( requested_hz > ( clk_freq / 2.0 ) )
			{ error_code = EC_TRACERE_FREQ_OUT_OF_RANGE; goto Tracere_I8254_Count_Freq_Hz_Error; }
			divisor = clk_freq / requested_hz;
			break;
		default:
			error_code = EC_TRACERE_CT_INVALID_MODE;
			goto Tracere_I8254_Count_Freq_Hz_Error;
	}
	if ( divisor > ((double) UINT16_MAX ) )
	{ error_code = EC_TRACERE_FREQ_OUT_OF_RANGE; goto Tracere_I8254_Count_Freq_Hz_Error; }

	if ( divisor < 1.0 )
	{ error_code = EC_TRACERE_FREQ_OUT_OF_RANGE; goto Tracere_I8254_Count_Freq_Hz_Error; }

	divisor_u16 = (uint16_t) divisor;
	scratch = divisor - ( (double) divisor_u16 );
	if ( scratch > 0.5 ) divisor_u16++;
	else if ( scratch < -0.5 ) divisor_u16--;

	if ( 0 == divisor_u16 )
	{ error_code = EC_TRACERE_FREQ_OUT_OF_RANGE; goto Tracere_I8254_Count_Freq_Hz_Error; }

	scratch 	   = ( (double) divisor_u16 );
	*actual_hz     = clk_freq / scratch;
	scratch 	   = divisor - scratch;
	*error_actual  = scratch / divisor;
	*initial_count = divisor_u16;
	return SUCCESS;
Tracere_I8254_Count_Freq_Hz_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup  tracere_counter_timer
@brief
For a given set of counter/timer clock and control word inputs and desired output timer interval desired, compute the
initial count to be loaded into the timer.  Report the actual time interval and compute the error between desired
time interval and actual time interval.

@param[in] 	requested_sec		Requested time interval in seconds.
@param[in] 	clock_external_hz	External clock frequency in Hertz, if used.  If not used enter zero.
@param[in] 	ct_clock_source		Counter/Timer clock source.  See ::TRACERE_GATE_CT0_ENUM, ::TRACERE_GATE_CT1_ENUM or ::TRACERE_GATE_CT2_ENUM.
@param[in] 	channel				Counter/Timer channel
@param[in] 	control				Control word that will be written to the I8254 in order to load the inital count.
								 Generally built using ::Tracere_I8254_Control_Create function.
@param[out] initial_count		Pointer to the initial count that is computed.
@param[out] actual_sec			Actual time interval in seconds, which maybe different that the requested time interval.
@param[out] error_actual		Error between requested and actual error.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_I8254_Count_Time_Sec(	double		requested_sec,		/* requested time interval (seconds)	*/
															double		clock_external_hz,	/* default, used if not CTn_nMHz		*/
															int			ct_clock_source,	/* TRACERE_CLK_CTn_4MHZ _10MHZ _20MHZ _40MHZ */
															int			channel,			/* CT channel							*/
															uint8_t		control,			/* i8254 control word					*/
															uint16_t *	initial_count,		/* i8254 count to load					*/
															double *    actual_sec,			/* actual output time interval     		*/
															double *    error_actual		/* computed error, x1.0e6 to get ppm 	*/
														)
{
	double				clk_freq;
	double				interval_clock;
	double				count;
	double				scratch;
	int					error_code;
	uint16_t			count_u16;
	I8254_MODE_ENUM		mode;
	
	clk_freq = 0.0; /* default */
	switch( channel )
	{ /* OK, this is redundant, but if FPGA firmware were to change, then we need all this */
		case 0:
			switch( ct_clock_source )
			{
				case TRACERE_CLK_CT0_4_MHZ:		clk_freq =  4.0e6;	break;	
				case TRACERE_CLK_CT0_10_MHZ:	clk_freq = 10.0e6;	break;
				case TRACERE_CLK_CT0_20_MHZ:	clk_freq = 20.0e6;	break;
				case TRACERE_CLK_CT0_40_MHZ:	clk_freq = 40.0e6;	break;
				default:						clk_freq = clock_external_hz;	break;
			}
			break;
		case 1:
			switch( ct_clock_source )
			{
				case TRACERE_CLK_CT1_4_MHZ:		clk_freq =  4.0e6;	break;	
				case TRACERE_CLK_CT1_10_MHZ:	clk_freq = 10.0e6;	break;
				case TRACERE_CLK_CT1_20_MHZ:	clk_freq = 20.0e6;	break;
				case TRACERE_CLK_CT1_40_MHZ:	clk_freq = 40.0e6;	break;
				default:						clk_freq = clock_external_hz;	break;
			}
			break;
		case 2:
			switch( ct_clock_source )
			{
				case TRACERE_CLK_CT2_4_MHZ:		clk_freq =  4.0e6;	break;	
				case TRACERE_CLK_CT2_10_MHZ:	clk_freq = 10.0e6;	break;
				case TRACERE_CLK_CT2_20_MHZ:	clk_freq = 20.0e6;	break;
				case TRACERE_CLK_CT2_40_MHZ:	clk_freq = 40.0e6;	break;
				default:						clk_freq = clock_external_hz;	break;
			}
			break;
	}

	if ( ( 0.0 == clk_freq ) || ( clk_freq > 40.0e6 ) )
	{ error_code = EC_TRACERE_CLK_OUT_OF_RANGE; goto Tracere_I8254_Count_Time_Sec_Error; }

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
			error_code = EC_TRACERE_CT_INVALID_MODE;
			goto Tracere_I8254_Count_Time_Sec_Error;
	}

	if ( ( count < 0.5 ) || ( count > ( ((double) UINT16_MAX) + 0.5 ) ) )
	{ error_code = EC_TRACERE_TIME_OUT_OF_RANGE; goto Tracere_I8254_Count_Time_Sec_Error; }

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
			error_code = EC_TRACERE_CT_INVALID_MODE;
			goto Tracere_I8254_Count_Time_Sec_Error;

	}
	scratch 	   = count - scratch;
	*error_actual  = scratch / count;
	*initial_count = count_u16;
	return SUCCESS;
Tracere_I8254_Count_Time_Sec_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup  tracere_counter_timer
@brief
This function extracts data from a I8254 control word that has been read from the I8254 hardware.
The I8254 control word of size uint8_t is written to this function via i8254_control_word.

@param[out] channel Extracted channel information.  See ::I8254_CHANNEL_ENUM.
@param[out] rw      Extracted read/write information.  See ::I8254_RW_ENUM.
@param[out] mode    Extracted mode information.  See ::I8254_MODE_ENUM.
@param[out] bcd	    Extracted BCD/binary data format information.  See ::I8254_TYPE_ENUM.
@param[in]  i8254   I8254 control word to be decoded.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_I8254_Control_Extract( I8254_CHANNEL_ENUM	channel,
														   I8254_RW_ENUM *		rw,
														   I8254_MODE_ENUM *	mode,
														   I8254_TYPE_ENUM *	bcd,
														   uint8_t 				i8254_control_word
														)
{
	int error_code;
	I8254_CHANNEL_ENUM chan_expected;

	if ( channel >= I8254_CT_QTY )
	{ error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE; goto Tracere_I8254_Control_Extract_Error; }

	chan_expected = I8254_CTRL_GET_SC( i8254_control_word );
	if ( channel != chan_expected )
	{ error_code = EC_TRACERE_CT_CHANNEL_MISMATCH; goto Tracere_I8254_Control_Extract_Error; }

	*rw		= I8254_CTRL_GET_RW( i8254_control_word );
	*mode	= I8254_CTRL_GET_MODE( i8254_control_word );
	*bcd	= I8254_CTRL_GET_BCD( i8254_control_word );
	return SUCCESS;
Tracere_I8254_Control_Extract_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_counter_timer
@brief
If successful returns the logical string name of the selected clock source for the given
I8254 counter/timer channel.

@param[in]  	ct_cfg 		Specific counter/timer channel configuration information
@param[in]		ch_channel	Counter/timer channel.  Range: 0 to ( ::TRACERE_CT_QTY - 1 )
@param[out]		clock_name	Pointer to the returned clock name.  This is the actual selected clock
                             within the configuration data structure for the given channel.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_CT_CLock_Name_Get( tracere_ct_cfg_type * ct_cfg, int ct_channel, const char ** clock_name )
{
	int		error_code;
	int		clock_enum;

	if ( ( ct_channel >= TRACERE_CT_QTY ) || ( ct_channel < 0 ) )
	{ error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;  goto Tracere_CT_CLock_Name_Get_Error; }
	switch(ct_channel)
	{
		case 0:	clock_enum = ct_cfg->ct0.clock_source;	break;
		case 1: clock_enum = ct_cfg->ct1.clock_source;	break;
		case 2: clock_enum = ct_cfg->ct2.clock_source;  break;
		default:
			error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;
			goto Tracere_CT_CLock_Name_Get_Error;
	}
	*clock_name = tracere_ct_clk_src_name[ct_channel][clock_enum];
	return SUCCESS;
Tracere_CT_CLock_Name_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_counter_timer
@brief
Performs a counter/timer transaction.

@param[in]  	board 	The descriptor obtained from the function ::AUL_Device_Open.
@param[inout] 	value 	Counter/Timer transfer data structure.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_I8254_Transfer( int board, i8254_ct_xfr_type * cxfr )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_I8254_Transfer_Error;
	/* error_code = DevMgr_Command( device, CMD_TRACERE_LED_GET, 0, (char *) &value, sizeof( value ) ); */
	error_code = Tracere_CMD_I8254_Transfer( device, 0, (char *) cxfr, sizeof( *cxfr ) );
	if ( !error_code ) return error_code;
Tracere_I8254_Transfer_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_counter_timer
@brief
Obtains the current count from the selected I8254 counter/timer.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] channel Counter/Timer selection.  Range: 0 to (::I8254_CT_QTY - 1).
@param[out] value Pointer to the uint16_t counter data pointed to by value.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_CT_Count_Get( int board, int channel, uint16_t * value )
{
	int						error_code;
	i8254_ct_xfr_type		cxfr;
/*	I8254_MODE_ENUM			mode;  */
	component_type *		component;
	devmgr_device_type *	device;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_CT_Count_Get_Error;
	if ( channel >= I8254_CT_QTY )
	{ error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;	goto Tracere_CT_Count_Get_Error;	}
	Mem_Set( &cxfr, 0, sizeof( cxfr ) );
	cxfr.type    = SYS_TYPE_MAKE( i8254_ct_xfr_type );
	cxfr.control = I8254_CTRL_CREATE( channel,I8254_RW_LATCH,0,0 );
	component    = &(device->component_table[TRACERE_COMPONENT_I8254]);
	error_code   = CmpMgr_Command( component,				/* info about the component			*/
								   CMD_I8254_CT_XFR_BASE,	/* component command for jump table	*/
								   channel,					/* channel			  				*/
								   (char *) &cxfr,			/* value we are passing along		*/
								   sizeof( cxfr )			/* buffer length 					*/
								 );
	if ( error_code ) goto Tracere_CT_Count_Get_Error;
	*value = cxfr.count_read[channel];
	return SUCCESS;
Tracere_CT_Count_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************/
/**********************************************************************************************************/


/**********************************************************************************************************//**
@ingroup tracere_led
@brief
Select the LED signal source.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] value A value of type ::TRACERE_LED_ENUM enum that will be used to set the LED information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_LED_Set( int board, TRACERE_LED_ENUM   value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_LED_Set_Error;
	/* error_code = DevMgr_Command( device, CMD_TRACERE_LED_SET, 0, (char *) &value, sizeof( value ) ); */
	error_code = Tracere_CMD_LED_Set( device, 0, (char *) &value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_LED_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_led
@brief
Get the LED signal source.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[out] value A pointer to the ::TRACERE_LED_ENUM enum that will be used to retrieve the LED information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_LED_Get( int board, TRACERE_LED_ENUM * value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_LED_Set_Error;
	/* error_code = DevMgr_Command( device, CMD_TRACERE_LED_GET, 0, (char *) &value, sizeof( value ) ); */
	error_code = Tracere_CMD_LED_Get( device, 0, (char *) value, sizeof( *value ) );
	if ( !error_code ) return error_code;
Tracere_LED_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************//**
@ingroup tracere_general
\brief
Apex Universal Library Tracere specific function that retrieves the Tracer board firmware revision.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] id_value Returns a UINT16 value that indicates the board firmware revision.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_Revision_Get( int board, uint16_t * id_value )
{
	int						error_code;
	devmgr_device_type *	device;
	uint16_t				value;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_Gen_Revision_Get_Error;

	*id_value = TRACERE_REVISION_021102; /* implied revision code */
	return error_code;
STX104_Gen_Revision_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup tracere_general
@brief
Set the Tracer-E device level (all components) configuration.  This allows an application to create an initialization
data structure and pass the entire structure at once to the library rather than separate configurations for each of
the major building blocks which make up the Tracer-E.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] config A pointer to the ::tracere_dio_cfg_type structure that will be used to set the general configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_Config_Set( int board, tracere_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_Config_Set_Error;
	error_code = Tracere_CFG_Set( device, config );
	if ( !error_code ) return error_code;
Tracere_Config_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************//**
@ingroup tracere_general
@brief
Get the Tracer-E device entire configuration.  This includes all of the configuration above.

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[out] config A pointer to the ::tracere_dio_cfg_type structure that will be used to retrieve the general configuration information.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_Config_Get( int board, tracere_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_Config_Get_Error;
	error_code = Tracere_CFG_Get( device, config );
	if ( !error_code ) return error_code;
Tracere_Config_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup  tracere_general
@brief
Initializes a Tracer-E configuration data structure for first use.  That means it also initializes
all the underlying data structures within this data structure (i.e. digital i/o configuration and so on).
This function was created primarily to ensure that the data structure is properly initialized.

@param[out] config A pointer to the ::tracere_cfg_type structure that will be used to initialize the structure.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.

<b>Example</b>
@code
int              error_code;
tracere_cfg_type cfg;                           // instantiate the data structure

error_code = Tracere_Config_Initialize( &cfg ); // initialize the data structure
if ( error_code < 0 ) return error_code;
@endcode
*/
AUL_DLL_API int AULCALLCONV Tracere_Config_Initialize( tracere_cfg_type * config )
{
	int error_code;

	Mem_Set( config, 0, sizeof( tracere_cfg_type ) );
	config->type = SYS_TYPE_MAKE( tracere_cfg_type );

	error_code = Tracere_DIO_Config_Initialize( &(config->dio) );
	if ( error_code ) goto Tracere_Config_Initialize_Error;

	error_code = Tracere_CT_Config_Initialize( &(config->ct) );
	if ( error_code ) goto Tracere_Config_Initialize_Error;

	error_code = Tracere_Intr_Config_Initialize( &(config->intr) );
	if ( error_code ) goto Tracere_Config_Initialize_Error;

/* TODO: led */

	return SUCCESS;
Tracere_Config_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@fn AUL_DLL_API int AULCALLCONV Tracere_Restart( int board, TRACERE_RESTART_ENUM value )
@ingroup tracere_general
@brief
This function is used to commit initialization data (i.e. configurations) to the hardware
and also restart hardware into a known state. For example TRACERE_RESTART_COLD_DIO will
commit any digital I/O related configuration and put the hardware into a known state as
defined by the configuration parameters.  In all cases, a COLD or WARM reset will do the
same things in part because the underlying hardware configurations do not have read-back
capability and so a warm start is not possible.  A WARM restart is the case where existing
configuration data is read back from the hardware and the software begins at that point.
It is possible to restart the entire device (board) or just sub-sections (components).

@param[in] board The descriptor obtained from the function ::AUL_Device_Open.
@param[in] value A value of type ::TRACERE_RESTART_ENUM enum that will be used to set the
				 Tracer-E command restart.
@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_Restart( int board, TRACERE_RESTART_ENUM value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_Restart_Error;
	error_code = Tracere_CMD_Restart( device, 0, (char *) &value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_Restart_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************/
/**********************************************************************************************************/


/*****************************************************************************/
#endif /* __KERNEL__ */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/****************************************************************************
Description
Application - Run during application initialization
Kernel		- Run during kernel initialization

Acronym and base_address are pulled from driver or insmod.
*/
static int Tracere_Initialization( void )
{
	return SUCCESS;
}

static int Tracere_Termination( void )
{
#if defined( TRACERE_DEBUG_LOG )
	Debug_Log( TRACERE_DEBUG_LOG "%s\n", __func__ );
#endif
	return SUCCESS;
}
/*****************************************************************/
enum { TRACERE_DEFAULT_IO_BASE_ADDRESS		= 0x0300 };
enum { TRACERE_DEFAULT_IO_BYTES				= 16     };
/* Bus width restrictions: can only be 8-bits wide anyway due to hardware */
enum { TRACERE_DEFAULT_BUS_8BIT_RESTRICT	=  1	 }; /* =1 ==> yes, restrict to 8-bit transactions */
enum { TRACERE_DEFAULT_MINOR_NUMBER			=  0     };
enum { TRACERE_DEFAULT_INTERRUPT_NUMBER		=  0     };
/*****************************************************************/
device_definition_type device_tracere_definition =
{
	SYS_TYPE_MAKE(device_definition_type),
	tracere_jump_table,
	TRACERE_JUMP_TABLE_QTY,
	Tracere_Initialization,
	Tracere_Termination,
	/* io_definition: logical_id, minor, base, width, irq, acronym */
	{	IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_TRACERE ),	
		TRACERE_DEFAULT_MINOR_NUMBER,
		TRACERE_DEFAULT_IO_BASE_ADDRESS,					
		TRACERE_DEFAULT_IO_BYTES,
		TRACERE_DEFAULT_BUS_8BIT_RESTRICT,/* over-ridden by TARGET_BUS_APERTURE_WIDTH_BYTES */
		TRACERE_DEFAULT_INTERRUPT_NUMBER, 
		DEVICE_TRACERE_NAME 
	},
	DEVICE_TRACERE_NAME,
	TRACERE_MODULE_REV,
	TRACERE_COMMAND_REV,
	TRACERE_STATUS_REV
};
