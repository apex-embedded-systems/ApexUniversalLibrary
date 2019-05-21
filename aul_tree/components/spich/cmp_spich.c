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

#include "cmp_spi.h"
#include "../../shared/cmd.h"
#include "../../shared/status.h"
#include "../../io/io_private.h"  /* IO_COPY_FROM_USER, IO_COPY_TO_USER */ 
#include "../../shared/error.h"
/*******************************************************************/
#define SPI_MODULE_REV  "$Date: 2014-06-27 09:56:24 -0500 (Fri, 27 Jun 2014) $"

/****************************************************************/
struct spi_dataset
{
	SYS_TYPE			type;   /* must always be first */
	spi_cfg_type		cfg;	/* must always be second */
	void *				private_data;

};
typedef struct spi_dataset spi_dataset_type;
/*******************************************************************/
static REGISTER_TYPE spi_register_table[] = 
{
	SPI_REGISTER_SET_DEFINITION( REG_EXTRACT_PARAMS )
	REG_LIST_NULL
};

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/* file specific to hardware */

enum spi_cmd
{
	SPI_CMD_NOP			= 0x0000,
	SPI_CMD_SELECT		= 0x1000, /* 0011 SSSS SSSS SSSS - up to twelve select lines	*/
	SPI_CMD_BATCH		= 0x2000, /* + 1:  run the buffer - blocking */
								  /* + 2:  clear buffer   */
	SPI_CMD_FIFO		= 0x3000, /* + 1:  run the FIFO   */
	SPI_CMD_DATA		= 0x4000,
	SPI_CMD_DELAY_HC	= 0x8000
};

#define SPI_BITS_DATA		8
#define SPI_BITS_DELAY_HC  15
#define SPI_BITS_SELECT		3
#define SPI_BITS_CMD		4 

#define SPI_MASK_DATA		( (1 << SPI_BITS_DATA	) -1 ) 
#define SPI_MASK_SELECT		( (1 << SPI_BITS_SELECT	) -1 )
#define SPI_MASK_DELAY_HC	( (1 << SPI_BITS_DELAY	) -1 ) 
#define SPI_MASK_CMD		( (1 << SPI_BITS_CMD	) -1 ) 

#define SPI_SHIFT_DATA		 0
#define SPI_SHIFT_DELAY_HC	 0
#define SPI_SHIFT_CMD		12

#define SPI_GET_DATA(x)		( ( (x) >> SPI_SHIFT_DATA  ) & SPI_MASK_DATA  )
#define SPI_GET_DELAY_HC(x)	( ( (x) >> SPI_SHIFT_DELAY ) & SPI_MASK_DELAY )

#define SPI_GET_CMD(x)		( (x) & ( SPI_MASK_CMD << SPI_SHIFT_CMD ) )

/* #define SPI_GET_CMD(x)		( ( (x) >> SPI_SHIFT_CMD   ) & SPI_MASK_CMD   )  */

/* build an operational code.  Note: data or delay will be OR'd with this value. */
#define SPI_CMD_OPCODE(x)	( ( ( x ) & SPI_MASK_CMD ) << SPI_SHIFT_CMD )

#include "cheetah.h"

/*******************************************************************/
/* max delay:  2^32 - 9 */
enum { SPI_TARGET_DELAY_MAX_NS = 4294967287 }; 
/*******************************************************************/
enum 
{ 
	SPI_TARGET_BIT_RATE_MIN_HZ = 100000,
	SPI_TARGET_BIT_RATE_MAX_HZ = 50000000 
};

/*******************************************************************/
#define SPI_TARGET_SW_BUFFER_ELEMENT_QTY  256
struct spi_cheetah_data_set
{
	SYS_TYPE	type;
	BOOL		open_and_active;
	BOOL		power_active;
	int			bit_rate_khz;   /* configuration must set this parameter */
	int			clock_ns;		/* configuration must set this parameter */
	int			half_clock_ns;	/* configuration must set this parameter */
	int			delay_interval_min_ns; /* config must set this: minimum delay step */
	Cheetah		cheetah;
	u32			unique_id;
	int			port;
	int			host_speed;
	int			rx_data_byte_count;
	uint16_t	tx_cmd_buffer[SPI_TARGET_SW_BUFFER_ELEMENT_QTY];
	uint8_t		rx_buffer[SPI_TARGET_SW_BUFFER_ELEMENT_QTY];
};
typedef struct spi_cheetah_data_set  spi_cheetah_data_set_type;

/*******************************************************************/
static int Spi_Target_Config_Test( spi_config_type * cfg )
{
	int		error_code;
	double	scratch;

	switch( cfg->mode )
	{
		case SPI_MODE_0:	/* CPOL = 0, CPHA = 0 */
		case SPI_MODE_1:	/* CPOL = 0, CPHA = 1 */
		case SPI_MODE_2:	/* CPOL = 1, CPHA = 0 */
		case SPI_MODE_3:	/* CPOL = 1, CPHA = 1 */
			break;
		default:
			error_code = EC_SPI_CFG_MODE;	goto Spi_Target_Config_Test_Error;
	}

	error_code = Spi_Target_Bit_Rate_Khz(	cfg->bit_rate_hz,
											cfg->bit_interval_ns,
											NULL
										);
	if ( error_code ) goto Spi_Target_Config_Test_Error;
	return SUCCESS;
Spi_Target_Config_Test_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SPI_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
/*
Writes to buffer, but does not commit it.
Returns the number of command words that have been transferred.

Commands:
	DATA
	SELECT
	DELAY_HALF_CLOCKS
	BATCH
		COMMIT (RUN)
		CLEAR
	FIFO
		COMMIT
	NOP
*/
static int Spi_Target_Buffer_Write( spi_dataset_type * ds, uint16_t * buf, size_t buf_len )
{
	int			error_code;
	size_t		index;
	uint16_t	cmd;
	int			delay_ns;  /* note: this will not work correctly on a 16-bit system */
	int			delay_hc;
	int			count_cmd;
/*
	int			delay_steps;
	int			delay_min_ns;
*/
	spi_cheetah_data_set_type * cds;
	Cheetah						cheetah;

	cds = (spi_cheetah_data_set_type *) ds->private_data;
	cheetah    = cds->cheetah;
	error_code = SUCCESS;
	count_cmd  = 0;
	for ( index = 0; index < buf_len; index++ )
	{
		cmd = (uint16_t) SPI_GET_CMD( buf[index] );
		if      ( SPI_CMD_DATA == cmd )
		{
			if ( cds->rx_data_byte_count >= SPI_TARGET_SW_BUFFER_ELEMENT_QTY ) return count_cmd;
			ch_spi_queue_byte( cheetah, 1, SPI_GET_DATA( buf[index] ) );
			cds->rx_data_byte_count++;
			count_cmd++;
		}
		else if ( SPI_CMD_SELECT == cmd )
		{
			ch_spi_queue_ss( cheetah, (u08) SPI_GET_SELECT( buf[index] ) );
			count_cmd++;
		}
		else if ( SPI_CMD_DELAY_HALF_CLOCKS == cmd )
		{
			delay_hc = (int) SPI_GET_DELAY_HC( buf[index] );
			delay_ns = cds->half_clock_ns * delay_hc;
			count_cmd++;
			if ( 0 == delay_ns ) break;
/*			delay_steps  = delay_ns / delay_interval_min_ns;
			delay_min_ns = delay_steps * delay_interval_min_ns;
			if ( delay_ns > delay_min_ns )
			{
				delay_ns = delay_min_ns + delay_interval_min_ns;
			}  */
			if ( delay_ns > SPI_TARGET_DELAY_MAX_NS ) delay_ns = SPI_TARGET_DELAY_MAX_NS; /* clamp */
			ch_spi_queue_delay_ns( cheetah, delay_ns );
		}
		else if ( SPI_CMD_BATCH == cmd )
		{
			uint8_t task = SPI_GET_DATA( buf[index] );
			count_cmd++;

			switch( task )
			{
				case 1: /* run buffer - blocking */
					error_code = ch_spi_batch_shift( cheetah, cds->rx_data_byte_count, cds->rx_buffer );
					if ( error_code < 0 ) return error_code;
					return count_cmd;
				case 2: /* buffer clear */
					ch_spi_queue_clear( cds->cheetah );
					break;
			}
		}
		else if ( SPI_CMD_FIFO == cmd )
		{
			uint8_t task = SPI_GET_DATA( buf[index] );
			count_cmd++;

			switch( task )
			{
				case 1: /* run fifo - blocking */
					error_code = ch_spi_batch_shift( cheetah, cds->rx_data_byte_count, cds->rx_buffer );
//TODO: map cheetah error codes into status space.
					if ( error_code < 0 ) 
					{
						error_code = -error_code;
						/* translate error code into our status codes */
						error_code = EC_BUILD( COMPONENT_SPI_STATUS, error_code + SPI_TARGET_STATUS_OFFSET ); 
						return -error_code;
					}
					ch_spi_queue_clear( cheetah );
					return count_cmd;
				case 4: /* run fifo - nonblocking */
					error_code = ch_spi_async_submit( cheetah );
					if ( error_code < 0 ) return error_code;
					ch_spi_queue_clear( cheetah );
					return count_cmd;
			}
		}
		else if ( SPI_CMD_NOP == cmd )
		{
			count_cmd++;
		}
		else
		{
			error_code = -EC_SPI_COMMAND_INVALID;
			return error_code;
		}
	}
	return count_cmd;
}
/*******************************************************************/
static int Spi_Target_Buffer_Clear( spi_dataset_type * ds )
{
	spi_cheetah_data_set_type * cds;
	cds = (spi_cheetah_data_set_type *) ds->private_data;
	cds->rx_data_byte_count = 0;
	ch_spi_queue_clear( cds->cheetah );
	return SUCCESS;
}
/*******************************************************************/
static int Spi_Target_Buffer_Commit( spi_dataset_type * ds )
{
	int							error_code;
	spi_cheetah_data_set_type * cds;  /* cheetah dataset */
	cds = (spi_cheetah_data_set_type *) ds->private_data;
	error_code = ch_spi_batch_shift( cds->cheetah, cds->rx_data_byte_count, cds->rx_buffer );
	return error_code;
}
/*******************************************************************/
static int Spi_Target_Power( int active, spi_dataset_type * ds )
{
	Cheetah						cheetah;
	spi_cheetah_data_set_type * cds;
	u08							power;

	cds = (spi_cheetah_data_set_type *) ds->private_data;

	if ( true != cds->open_and_active ) return SUCCESS;

	if ( active != cds->power_active )
	{
		if ( active ) power = CH_TARGET_POWER_ON;
		else          power = CH_TARGET_POWER_OFF;

		ch_target_power( cds->cheetah, power );
		ch_sleep_ms( 100 );
		cds->power_active = active;
	}
	return SUCCESS;
}
/*******************************************************************/
static int Spi_Target_Config_Commit( spi_dataset_type * ds )
{
	int					error_code;
    Cheetah				cheetah;
    CheetahSpiPolarity	clock_polarity;
    CheetahSpiPhase		clock_phase;
    CheetahSpiBitorder	bit_order;
    u08					ss_polarity;
	int					bit_rate_khz;
	spi_cheetah_data_set_type * cds;
	double				scratch;

	cds = (spi_cheetah_data_set_type *) ds->private_data;
	cheetah = cds->cheetah;

	error_code = Spi_Target_Config_Test( &(ds->cfg) );
	if ( error_code ) return error_code;

	switch( ds->cfg.mode )
	{
		case SPI_MODE_0:	/* CPOL = 0, CPHA = 0 */
			clock_polarity = CH_SPI_POL_RISING_FALLING;
			clock_phase    = CH_SPI_PHASE_SAMPLE_SETUP;
			break;
		case SPI_MODE_1:	/* CPOL = 0, CPHA = 1 */
			clock_polarity = CH_SPI_POL_RISING_FALLING;
			clock_phase    = CH_SPI_PHASE_SETUP_SAMPLE;
			break;
		case SPI_MODE_2:	/* CPOL = 1, CPHA = 0 */
			clock_polarity = CH_SPI_POL_FALLING_RISING;
			clock_phase    = CH_SPI_PHASE_SAMPLE_SETUP;
			break;
		case SPI_MODE_3:	/* CPOL = 1, CPHA = 1 */
			clock_polarity = CH_SPI_POL_FALLING_RISING;
			clock_phase    = CH_SPI_PHASE_SETUP_SAMPLE;
			break;
	}

	if ( ds->cfg.shift_lsb_first ) bit_order = CH_SPI_BITORDER_LSB;
	else                           bit_order = CH_SPI_BITORDER_MSB;

	ch_spi_configure(	cheetah, 
						clock_polarity, 
						clock_phase, 
						bit_order,
						ds->chip_select_polarity; /* bit pattern */
					);

	Spi_Target_Bit_Rate_Khz( ds->cfg.bit_rate_hz, 
							 ds->cfg.bit_interval_ns,
							 &bit_rate_khz
						   );
	cds->bit_rate_khz = ch_spi_bitrate( cheetah, bit_rate_khz );

	scratch = (double) ( cds->bit_rate_khz ); /* KHz */
	scratch = scratch * 1000.0; /* Hz */
	scratch = 1.0 / scratch;    /* sec */
	scratch = scratch * 1.0e9;  /* nanoseconds  */
	cds->clock_ns = (uint32_t) scratch; /* clock interval nanoseconds */
	cds->delay_interval_min_ns = cds->clock_ns * 8;
	cds->half_clock_ns = (uint32_t) ( scratch / 2.0 ); /* nanoseconds for 1/2 clock interval */
	Spi_Target_Power( true /* power on */, ds );

	/* initialize the outputs */
	ch_spi_queue_clear( cheetah );
	ch_spi_queue_ss( cheetah, 0 );
	ch_spi_queue_oe( cheetah, 1 );
	ch_spi_batch_shift( cheetah, 0, NULL ); /* enable outputs and set the select lines to be inactive */
	ch_spi_queue_clear( cheetah );
}
/*******************************************************************/
static int Spi_Target_Initialize( spi_dataset_type * ds )
{
	int					error_code;
    Cheetah				cheetah;
    CheetahSpiPolarity	clock_polarity;
    CheetahSpiPhase		clock_phase;
    CheetahSpiBitorder	bit_order;
    u08					ss_polarity;
	spi_cheetah_data_set_type * cds;

	int		port = 0;

	cheetah = ch_open( port );

	if ( CH_UNABLE_TO_OPEN == cheetah )
	{ error_code = EC_SPI_HARDWARE_OPEN;		goto Spi_Initialize_Error; }
	else if ( CH_INCOMPATIBLE_DEVICE == cheetah )
	{ error_code = EC_SPI_INCOMPATIBLE_DEVICE;	goto Spi_Initialize_Error; }

	cds = (spi_cheetah_data_set_type *) Mem_Allocate( NULL, sizeof( spi_cheetah_data_set_type ) );
	if ( NULL == cds )
	{	error_code = EC_SPI_MEM_ERROR;  goto Spi_Initialize_Error; }

	cds.unique_id	   = ch_unique_id( cheetah );
	cds.port			   = ch_port( cheetah );
	cds.host_speed	   = ch_host_ifce_speed( cheetah );
	cds.cheetah         = cheetah;
	cds.type            = SYS_TYPE_MAKE( spi_cheetah_data_set_type );
	cds.open_and_active = true;
	ds->private_data           = (void *) cds;
	return SUCCESS;
Spi_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SPI_NAME, __func__, __LINE__, error_code );
	return -error_code;
}


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


/*******************************************************************/
/* returns number of commands written, if > 0.
   if < 0, then error is returned.

torn between two methods:
1. transmit buffer as we write it, or at least as long as the hardware will accept it.
   Commit it when the 'hardware' buffer is full or the amount that can be read back is full.

   return from function when:
	- read back buffer is full
	- cannot stuff any more into hardware buffer.

	- if all of the buffer is written
		- commit to fifo: return immediately and/or commit what you can and return.
		- commit to buffer and 
		- commit to buffer: return, but do not 'run' it.
*/
static int Spi_CMD_Buffer_Write(	component_type *	component, 
									size_t				index,
									char *				buf, 
									size_t				buf_len 
								)
{
	spi_buffer_info_type	info;
	spi_dataset_type *		ds;
	spi_cheetah_data_set_type * cds;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto Spi_Buffer_Write_Error;		}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &info, buf, sizeof( info ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto Spi_Buffer_Write_Error;	 }
	/* verify we copied the correct data structure */
	if ( SYS_TYPE_MAKE( spi_buffer_info_type ) != info->type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto Spi_Buffer_Write_Error;	}

	ds = ( spi_dataset_type *) component->private_data;
	cds = (spi_cheetah_data_set_type *) ds->private_data;

	index				= 0;
	count_element       = info.count; /* number of SPI_CMD_WORDs */
	count_element_local = SPI_TARGET_SW_BUFFER_BYTES / sizeof( SPI_CMD_WORD );
	count_u8			= SPI_TARGET_SW_BUFFER_BYTES; /* size in bytes */
	while ( count_element > count_element_local )
	{
		if ( IO_COPY_FROM_USER( cds->temporary_buffer, &(info.buffer[index * count_u8]), count_u8 ) )
		{ error_code = EC_IO_COPY_FROM_USER;	goto Spi_Buffer_Write_Error;	 }
		/* write this portion of the buffer out to the hardware */
		error_code = Spi_Target_Buffer_Write( ds, cds->temporary_buffer, count_element );
		if ( error_code < 0 ) goto Spi_Buffer_Write_Error;
		/* advanced to the next portion of the buffer */
		if ( count_element != error_code ) 
		count_element = count_element - error_code;
		index++;
	}
	if ( count_element > 0 )
	{
		if ( IO_COPY_FROM_USER( cds->temporary_buffer, &(info.buffer[index * count_u8]), count_u8 ) )
		{ error_code = EC_IO_COPY_FROM_USER;	goto Spi_Buffer_Write_Error;	 }
		/* write this remaining portion of the buffer out to the hardware */
		error_code = Spi_Target_Buffer_Write( ds, cds->temporary_buffer, count_element );
		if ( error_code < 0 ) goto Spi_Buffer_Write_Error;
	}
	if ( error_code ) goto Spi_Buffer_Write_Error;
	return SUCCESS;
Spi_Buffer_Write_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SPI_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int Spi_CMD_Power_Up(	component_type *	component, 
								size_t				index,
								char *				buf, 
								size_t				buf_len 
						 )
{
	spi_dataset_type * ds;
	(void)  component;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	ds = ( spi_dataset_type *) component->private_data;
	return Spi_Target_Power( true, ds );
}
/*******************************************************************/
static int Spi_CMD_Power_Down(	component_type *	component, 
								size_t				index,
								char *				buf, 
								size_t				buf_len 
							 )
{
	spi_dataset_type * ds;
	(void)  component;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	ds = ( spi_dataset_type *) component->private_data;
	return Spi_Target_Power( false, ds );
}
/*******************************************************************/
static int Spi_CMD_Cfg_Set( component_type *	component, 
							size_t				index,		/* indicates which DI channel to obtain */
							char *				buf, 
							size_t				buf_len 
						  )
{
	int						error_code;
	spi_cfg_type			candidate;
	spi_dataset_type *		ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto Spi_Cfg_Set_Error;		}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto Spi_Cfg_Set_Error;	 }

/* perform any additional testing/verification here */
	error_code = Spi_Target_Config_Test( &candidate );
	if ( error_code ) goto Spi_Cfg_Set_Error;

	ds = ( spi_dataset_type *) component->private_data;
	Mem_Copy( &(ds->cfg), &candidate, sizeof( candidate ) );

/* the data will be committed to hardware upon a restart warm/cold */

	return SUCCESS;
Spi_Cfg_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SPI_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int Spi_CMD_Cfg_Get( component_type *	component, 
					 size_t				index,		/* indicates which DI channel to obtain */
					 char *				buf, 
					 size_t				buf_len 
			      )
{
	int  					error_code;
	spi_dataset_type *		ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto Spi_Cfg_Get_Error;	 }

	ds = ( spi_dataset_type *) component->private_data;

	if ( IO_COPY_TO_USER( buf, &(ds->cfg), sizeof( spi_dataset_type ) ) )
	{ error_code = EC_IO_COPY_TO_USER;		goto Spi_Cfg_Get_Error;	 }

	return SUCCESS;
Spi_Cfg_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SPI_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/*******************************************************************/
static int Spi_CMD_Write(	component_type *	component, 
						size_t				index,		/* indicates which DI to obtain */
						char *				buf, 
						size_t				buf_len 
					)
{
/*
	At this point it is assumed that the buffer has been encoded
	as required for whatever target.  Here we just move it along.

	Writes to the hardware as if it is a temporary out-going buffer.

	The bytes are assumed to be encoded with address/register information
	if required.  For example, PGA280 the first byte is the command followed
	by optional data.

	The PGA280, 8-bit data, for example will then be wrapped into SPI commands
	at a layer just above in order for the right things to occur.

	Example: reset all PGA280 error flags
	SPI_CMD_SELECT	| 1      <-- pga280 at chip select 0, enable it.  (similar to indirect offset, I suppose)
	SPI_CMD_DATA	| 0x44
	SPI_CMD_DATA	| 0xFF
	SPI_CMD_SELECT  | 0
	SPI_CMD_DELAY	| 0      <-- might need delay before next transmission?


*/
	return SUCCESS;
}
/******************************************************************
Description.  Updates I/O all at one time.
*/
static int Spi_CMD_Update(	component_type *	component, 
					size_t				index,		/* not used */
					char *				buf, 
					size_t				buf_len 
				  )
{
	size_t					byte_position;
	spi_dataset_type *	ds;
	(void)					buf;
	(void)					buf_len;

	ds = ( spi_dataset_type * ) component->private_data;

	for ( byte_position = 0; byte_position < SPI_PORT_QTY; byte_position++ )
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
							  SPI_REGISTER_SIZE_FIXED 
						    );	
		}
		else
		{ /* input port */
			CmpMgr_Reg_Read( component, 
							 byte_position, /* register index or ID */
							 (char *) &(ds->port_internal[byte_position].raw), 
							 SPI_REGISTER_SIZE_FIXED 
						 );
			ds->copy[byte_position] = ds->port_internal[byte_position].raw ^ ds->cfg.port[byte_position].polarity;
		}
	}
	return SUCCESS;
}
/*******************************************************************/
static int Spi_CMD_Restart( component_type *	component, 
					 size_t				index,
					 char *				buf, 
					 size_t				buf_len 
				   )
{
	size_t						byte_position;
	size_t						operation;
	spi_dataset_type *	ds;
	(void)						buf_len;

	if      ( NULL == buf  ) operation = RESTART_COLD;
	else if ( 0    == *buf ) operation = RESTART_COLD;
	else  	                operation = RESTART_WARM;

	ds = ( spi_dataset_type * ) component->private_data;

	Spi_Apply_Directions_Helper( ds );

	switch( operation )
	{	/*--------------------------------*/
		case RESTART_COLD:
/*TODO: test for valid configuration by observing the type value (SYS data type).*/
			CmpMgr_Reg_Write( component, SPI_REG_CFG, (char *)&(ds->cfg.port_directions), sizeof(uint8_t) );

			for ( byte_position = 0; byte_position < SPI_PORT_QTY; byte_position++ )
			{
				ds->copy[byte_position] = ds->cfg.port[byte_position].init_value;
			}
			
/*TODO: we could fix up the bit map here if it all turns out to be null??*/
			Spi_Update( component,
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


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/







/*****************************************************************/
#include "../../shared/mem.h"
#include "../../shared/error.h"

/*******************************************************************/
AUL_DLL_API int Spi_Nanoseconds_to_Half_Clocks(	unsigned int	 hc_ns,		/* half-clock nanoseconds			*/
												unsigned int	 delay_ns,	/* delay in nanoseconds				*/
												unsigned int *	 delay_hc	/* calculated delay in half-clocks	*/
											  )
{
	*delay_ns = delay_ns / hc_ns;
}
/*******************************************************************/
AUL_DLL_API  int Spi_Target_Bit_Rate_Khz(	uint32_t bit_rate_hz, 
											uint32_t bit_interval_ns,
											int *	 bit_rate_khz
										)
{
	uint32_t interval_khz = 0;

	if ( ( 0 == cfg->bit_interval_hz ) && ( 0 == cfg->bit_interval_ns ) )
		return EC_SPI_BIT_INTERVAL_MIN;

	if ( 0 == cfg->bit_interval_ns )
	{
		if ( cfg->bit_rate_hz < SPI_TARGET_BIT_RATE_MIN_HZ ) return EC_SPI_BIT_RATE_MIN;

		if ( cfg->bit_rate_hz > SPI_TARGET_BIT_RATE_MAX_HZ ) return EC_SPI_BIT_RATE_MAX;

		interval_khz = cfg->bit_rate_hz / 1000;
	}
	else
	{
		scratch = ( (double) cfg->bit_interval_ns ) * 1.0e-9; /* seconds */
		scratch =     1.0 / scratch;   /* hz  */
		scratch = scratch / 1000.0;	   /* khz */
		interval_khz = (uint32_t) scratch;

		if ( interval_khz < ( SPI_TARGET_BIT_RATE_MIN_HZ / 1000 ) ) return EC_SPI_BIT_INTERVAL_MAX;
		if ( interval_khz > ( SPI_TARGET_BIT_RATE_MAX_HZ / 1000 ) ) return EC_SPI_BIT_INTERVAL_MIN;
	}
	if ( NULL != bit_rate_khz ) *bit_rate_khz = interval_khz;
	return SUCCESS;
}
/*****************************************************************/
AUL_DLL_API int Spi_Config_Initialize( spi_cfg_type * cfg )
{
	Mem_Set( cfg, 0, sizeof( spi_cfg_type ) );
	cfg->type = SYS_TYPE_MAKE( spi_cfg_type );
	return SUCCESS;
}
/*****************************************************************/
int Spi_Create( component_type * component, size_t instance )
{
	int						error_code;
	spi_dataset_type *		ds;
	size_t					mem_size;
	void *					mem;

	mem_size = sizeof( spi_dataset_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_SPI_DATASET_ALLOCATE;  goto Spi_Create_Error; }
	Mem_Set( mem, 0, mem_size );
	ds = (spi_dataset_type *) mem;
	
	error_code = Spi_Config_Initialize( &(ds->cfg) );
	if ( error_code ) goto Spi_Create_Error;

	component->definition   = &component_spi_definition;
	component->physical_id  = IOC_CREATE_PHYSICAL_ID_COMPONENT( COMPONENT_I8255M0, instance );
	component->offset       = 0	   /* fill in at device level */;
	component->private_data = ds;  /* assign private data */
	component->status       = component->status | CMP_STS_BUILD(0,1,0,0,0); /* private data assigned */

	error_code = Spi_Target_Initialize( ds );
	if ( error_code ) 
	{
		//TODO: terminate - deallocate memory
		goto Spi_Create_Error;
	}

	return SUCCESS;
Spi_Create_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SPI_NAME, __func__, __LINE__, error_code );
	return -error_code;
} 
/*****************************************************************/
int Spi_Destroy( component_type * component )
{
	int error_code;

	if ( NULL == component ) return SUCCESS;
	
	if ( NULL != component->definition ) 
	{ /* if it is null, we will just have to assume we are working with the 
	     correct data type.
	  */
		if ( SYS_TYPE_MAKE(component_definition_type) != component->definition->type )
		{  error_code = EC_SPI_COMPONENT_INVALID;	goto Spi_Destroy_Error;  }
	}
	if ( NULL != component->private_data )
	{
		Mem_Free( NULL, &(component->private_data) );
		component->private_data = NULL;
	}
	return SUCCESS;
Spi_Destroy_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_SPI_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
static cmp_fnc spi_jump_table[] =
{
	COMPONENT_SPI_CMD_DEFINITIONS( CMD_CMP_EXTRACT_JUMP_TABLE )
};
/*****************************************************************/
component_definition_type component_spi_definition =
{
	SYS_TYPE_MAKE(component_definition_type),
	spi_register_table,
	spi_jump_table,
	SPI_JUMP_TABLE_QTY,
#if defined( TARGET_CHECK_BUILD )
	COMPONENT_SPI_NAME,
#endif
	SPI_MODULE_REV,
	SPI_COMMAND_REV,
	SPI_STATUS_REV
};

