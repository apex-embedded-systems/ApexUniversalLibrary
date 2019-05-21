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



#include "reg.h"

#include "../shared/status_macros.h"	/* status values							  */
#include "../io/io.h"					/* actual read/write from/to hardware */

#include "../shared/cmd_macros.h"
/*********************************************************************************/
#define REG_MODULE_REV  "$Date: 2014-05-16 13:31:25 -0500 (Fri, 16 May 2014) $"

/*********************************************************************************/

/*********************************************************************************/
#define REG_DEBUG_LOG   DEBUG_OUT


/********************************************************************************
Description
Reads data from hardware I/O space given register parameters.
It takes into account the targets actual bus width as specified
in the target.h file.  The register aperture maybe different than
the bus aperture, so it is the responsibility of this function to
determine the actual aperture; whichever is narrower takes precedence.

This function is capable of reading one or more elements to a given
register.

Assumptions
1. Register actually has read access.
2. Address is byte resolution
3. Actual address location that is calculated is a valid readable location.
*/
static void Reg_IO_Direct_Read(	REGISTER_TYPE *	reg, 
								size_t			offset,
								int				iod,
								char *			buf,
								size_t			buf_len
						   )
{
	size_t		address;
	size_t		element_total;
	size_t		u16_total;
	size_t		index;
	size_t		element_index;
	size_t		size_reg;
	uint16_t *	buf_u16;
	size_t		bus_aperture;
	size_t		reg_aperture;
	const size_t u16_size = sizeof( uint16_t );

	/* offset is a base address adjustment, i.e. multiple UARTs with same register_tables */
	address  = reg->virtual_address + offset; 

#if defined( REG_DEBUG )
	if ( REG_DIR_NONE == ( REG_GET_DIRECTION( reg->info ) & REG_DIR_READ ) )
	{
		Debug_Log( REG_DEBUG_LOG "%s: cannot read \"%s\" at 0x%04X\n", __func__, reg->name, address );
		return;
	}
#endif

/* TODO: this is not really where I want this, but for now it is OK. */
	bus_aperture = REG_BYTES_LESS_ONE( TARGET_BUS_APERTURE_WIDTH_BYTES );
	reg_aperture = REG_GET_REGISTER_APERTURE( reg->info );
	if ( reg_aperture > bus_aperture ) reg_aperture = bus_aperture;

	if ( REG_TEST_AUTO_INCREMENT( reg->info ) )
	{ /*** 
			Auto-Increment Read Operations (AIRO)
			Read sequential byte or word locations, incrementing the address as we go.
			Size of the actual elements in the buffer do not matter.  In this case the 
			register or bus aperture are the primary drivers in how the data is written.
			NOTE: this portion has been used rarely.
	  */
		if ( REG_BYTES_LESS_ONE(1) == reg_aperture )
		{ /*  8-BIT AIRO -- begin */
			for ( index = 0; index < buf_len; index++ )
			{
				IO_Read_U8( iod, address + index, (uint8_t *) &(buf[index]) );
			}
		} /*  8-BIT AIRO -- end   */
		else
		{ /* 16-BIT AIRO -- begin */
			buf_u16   = (uint16_t *) buf;	  	/* char to uint16_t     */
			u16_total = buf_len / u16_size; 	/* total uint16_t count */
			for ( index = 0; index < u16_total; index++ )
			{
				IO_Read_U16( iod, address + index * u16_size, &(buf_u16[index]) );
			}
		} /* 16-BIT AIRO -- end   */
	}
	else
	{ /***
			Register Read Operations (RRO)
			Read from the same register location.  The address is only incremented
			to support reads from specific register.  This supports multiple
			element read operations (i.e. string reads).
	  */
		size_reg = REG_GET_ELEMENT_SIZE( reg->info );
		if ( REG_BYTES_LESS_ONE(1) == reg_aperture )
		{ /* Register aperture is restricted to 8-bits */
			if ( REG_SIZE_UINT8 == size_reg )
			{ /*  8-BIT REGISTER RRO -- begin */
				IO_Read_Repeat_U8( iod, address, (uint8_t *)buf, buf_len );
			} /*  8-BIT REGISTER RRO -- end   */
			else
			{ /* 16-BIT REGISTER RRO w/8-bit restrict -- begin */
				element_total   = buf_len / size_reg;
				for ( element_index = 0; element_index < element_total; element_index++ )
				{
					for( index = 0; index < u16_size; index++ )
					{
						/*buf[index + word] = IOKERN_READ_uint8_t( address + word );*/
						IO_Read_U8( iod, address + index, (uint8_t *) &(buf[element_index*u16_size + index]) );
					}
				}
			} /* 16-BIT REGISTER RRO w/8-bit restrict -- end   */		
		}
		else
		{ /* 16-BIT RRO -  one or more elements to register location -- begin */
			buf_u16       = (uint16_t *) buf;
			element_total = buf_len / u16_size; /* fixed to 16-bits */
			if      ( REG_SIZE_UINT8 == size_reg  )
			{ /* 8-bit data to 16-bit width path.  Register is uint8_t and will read LSB only */
				uint16_t value_u16;
				for ( element_index = 0; element_index < element_total; element_index++ )
				{
					IO_Read_U16( iod, address, &value_u16 );
					buf[element_index] = (uint8_t)( value_u16 & UINT8_MAX );
				}
			}
			else if ( REG_SIZE_UINT16 == size_reg )
			{ /* Register is uint16_t in size  -- read element(s) directly */
				IO_Read_Repeat_U16( iod, address, buf_u16, element_total );
			}
			else
			{ /* Register is uint32_t or larger  -- write 16-bits at a time */
				element_total = buf_len / size_reg;
				/* number of 16-bit writes for given register */
				u16_total     = size_reg / u16_size;
				for ( element_index = 0; element_index < element_total; element_index++ )
				{
					for( index = 0; index < u16_total; index++ )
					{
						IO_Read_U16( iod, 
									 address + index * u16_size, 
							         &(buf_u16[element_index * u16_size + index] ) 
								   );
					}
				}
			}
		} /* 16-BIT RRO -  one or more elements to register location -- end   */
	}
}
/********************************************************************************
Description
Writes data out to hardware I/O space given register parameters.
It takes into account the targets actual bus width as specified
in the target.h file.  The register aperture maybe different than
the bus aperture, so it is the responsibility of this function to
determine the actual aperture; whichever is narrower takes precedence.

This function is capable of writing one or more elements to a given
register.

Assumptions
1. Register actually has write access.
2. Address is byte resolution
3. Actual address location that is calculated is a valid writable location.
*/
static void Reg_IO_Direct_Write( 	REGISTER_TYPE *		reg,
									size_t				offset,
									int					iod,
									char *				buf,
									size_t				buf_len
						    )
{
	size_t		address;
	size_t		element_total;
	size_t		u16_total;
	size_t		index;
	size_t		element_index;
	size_t		size_reg;
	uint16_t *	buf_u16;
	size_t		bus_aperture;
	size_t		reg_aperture;
	const size_t u16_size = sizeof( uint16_t );

	/* offset is a base address adjustment, i.e. multiple UARTs with same register_tables */
	address  = reg->virtual_address + offset;

#if defined( REG_DEBUG )
	if ( REG_DIR_NONE == ( REG_GET_DIRECTION( reg->info ) & REG_DIR_WRITE ) )
	{
		Debug_Log( REG_DEBUG_LOG "%s: cannot write \"%s\" at 0x%04X\n", __func__, reg->name, address );
		return;
	}
#endif

/* TODO: this is not really where I want this, but for now it is OK. */
	bus_aperture = REG_BYTES_LESS_ONE( TARGET_BUS_APERTURE_WIDTH_BYTES );
	reg_aperture = REG_GET_REGISTER_APERTURE( reg->info );
	if ( reg_aperture > bus_aperture ) reg_aperture = bus_aperture;

	if ( REG_TEST_AUTO_INCREMENT( reg->info ) ) /* NOTE: auto-increment feature is just never used. */
	{ /*** 
			Auto-Increment Write Operations (AIWO)
			Write to sequential byte or word locations, incrementing the address as we go.
			Size of the actual elements in the buffer do not matter.  In this case the 
			register or bus aperture are the primary drivers in how the data is written.
			NOTE: this portion has been used rarely.
	  */
		if ( REG_BYTES_LESS_ONE(1) == reg_aperture )
		{ /*  8-BIT AIWO -- begin */
			for ( index = 0; index < buf_len; index++ )
			{
				IO_Write_U8( iod, address + index, (uint8_t)(buf[index]) );
			}
		} /* 8-BIT AIWO -- end   */
		else
		{ /* 16-BIT AIWO -- begin */
			buf_u16   = (uint16_t *) buf;   /* char to uint16_t     */
			u16_total = buf_len / u16_size; /* total uint16_t count */
			for ( index = 0; index < u16_total; index++ )
			{
				IO_Write_U16( iod, address + index * u16_size, buf_u16[index] );
			}
		} /* 16-BIT AIWO -- end */
	}
	else
	{ /***
			Register Write Operations (RWO)
			Write to the same register location.  The address is only incremented
			to support writes to specific register.  This supports multiple
			element write operations (i.e. string writes).
	  */
		size_reg = REG_GET_ELEMENT_SIZE( reg->info ); /* register size in bytes */
		if ( REG_BYTES_LESS_ONE(1) == reg_aperture )
		{ /* Register aperture is restricted to 8-bits */
			if ( REG_SIZE_UINT8 == size_reg )
			{ /*  8-BIT REGISTER RWO -- begin */
				IO_Write_Repeat_U8( iod, address, (uint8_t *)buf, buf_len );
			} /*  8-BIT REGISTER RWO -- end   */
			else
			{ /* 16-BIT REGISTER RWO w/8-bit restrict -- begin */
				element_total = buf_len / size_reg;
				for ( element_index = 0; element_index < element_total; element_index++ )
				{
					for( index = 0; index < u16_size; index++ )
					{
						IO_Write_U8( iod, address + index, (uint8_t)(buf[element_index*u16_size + index]) );
					}
				}
			} /* 16-BIT REGISTER RWO w/8-bit restrict -- end   */
		}
		else
		{ /* 16-BIT RWO -  one or more elements to register location -- begin */
			buf_u16       = (uint16_t *) buf;
			element_total = buf_len / u16_size; /* fixed to 16-bits */
			if      ( REG_SIZE_UINT8 == size_reg  )
			{ /* 8-bit data to 16-bit width path.  Register is uint8_t and will write LSB only */
				for ( element_index = 0; element_index < element_total; element_index++ )
				{
					IO_Write_U16( iod, address, (uint16_t) buf[element_index] );
				}
			}
			else if ( REG_SIZE_UINT16 == size_reg )
			{ /* Register is uint16_t in size  -- write element(s) directly */
				IO_Write_Repeat_U16( iod, address, buf_u16, element_total );
			}
			else
			{ /* Register is uint32_t or larger  -- write 16-bits at a time */
				element_total = buf_len / size_reg;
				/* number of 16-bit writes for given register */
				u16_total     = size_reg / u16_size;
				for ( element_index = 0; element_index < element_total; element_index++ )
				{
					for( index = 0; index < u16_total; index++ )
					{
						IO_Write_U16( iod, 
									  address + index * u16_size, 
							          buf_u16[element_index * u16_size + index] 
						            );
					}
				}
			}
		} /* 16-BIT RWO -  one or more elements to register location -- end   */
	}
}
/*********************************************************************************/
void Reg_Write(   REGISTER_TYPE		register_table[],	/* the register table to be referenced for this particular device */
				  size_t			offset,				/* to support multiple devices with same register table, but at different physical offsets */
				  int				iod,				/* io descriptor */
				  size_t			reg_id,				/* register row */
				  char *			buf, 
				  size_t			buf_len 
			  )
{
/* TODO: 
	1.	check for write access    
	2.  distinguish between various default address spaces 	
	3.  check for valid reg_id range.

There could simply be another form of Reg_Write() that performs more testing for 
external stuff as compared to internal register transfers.

	Reg_Write_Qual();

*/
#if defined( REG_DEBUG )
{ /* determine if reg_id is within valid range */
	size_t index;
	index = 0;
	while ( REG_INFO_NONE != register_table[index].info ) index++;
	if ( reg_id >= index ) return;
}
#endif


#if REG_DEFAULT_ADDRESS_SPACE == REG_SPACE_IO_DIRECT
	Reg_IO_Direct_Write( &(register_table[reg_id]), offset, iod, buf, buf_len ); 
#endif
}
/*********************************************************************************/
void Reg_Read( REGISTER_TYPE 	register_table[],	/* the register table to be referenced for this particular device */
			   size_t			offset,				/* to support multiple devices with same register table, but at different physical offsets */
			   int				iod,				/* io descriptor */
			   size_t			reg_id,				/* register row */
			   char *			buf, 
			   size_t			buf_len 
			  )
{
/* TODO: 
	1.	check for write access    
	2.  distinguish between various default address spaces 	
	3.  check for valid reg_id range.

There could simply be another form of Reg_Read() that performs more testing for external stuff as compared to internal
register transfers.

	Reg_Read_Qual();
*/
#if defined( REG_DEBUG )
{ /* determine if reg_id is within valid range */
	size_t index;
	index = 0;
	while ( REG_INFO_NONE != register_table[index].info ) index++;
	if ( reg_id >= index ) return;
}
#endif

#if REG_DEFAULT_ADDRESS_SPACE == REG_SPACE_IO_DIRECT
	Reg_IO_Direct_Read( &(register_table[reg_id]), offset, iod, buf, buf_len );  
#endif
}
/********************************************************************************
Assumption
Address is byte resolution.
*/
void Reg_Table_Offset_Range(  REGISTER_TYPE 	register_table[],	/* register table to test								*/
							  size_t *			start,				/* returns the start address, virtual address in table	*/
							  size_t *			end,				/* returns the end address, virtual address in table	*/
							  size_t *			length				/* returns the contiguous length from start to end		*/
						         )
{
	size_t index;
	size_t first, last; /*, len;*/
	/*size_t flag;*/
	size_t reg_byte_count;

	first =  SIZE_MAX;
	last  = 0;
	/*len   = 0;*/
	index = 0;
	/*flag  = 0;*/

	if ( NULL == register_table )
	{ /* we have come across a virtual component which has no physical registers */
		*start  = 0;
		*end    = 0;
		*length = 0;
		return;
	}

	while ( register_table[index].info )
	{
		reg_byte_count = REG_GET_ELEMENT_SIZE( register_table[index].info );

		if ( register_table[index].virtual_address > last )   
		{
			last   = register_table[index].virtual_address;
			if ( reg_byte_count > REG_WIDTH_BYTES(1) /* for clarity */ )
			{
				last = last + ( reg_byte_count - 1 );
			}
		}

		if ( register_table[index].virtual_address < first ) 
		{
			first = register_table[index].virtual_address;
		}
		index++;
	}

	*start	= first;
	*end		= last;
	*length	= last - first + 1;
}
/*********************************************************************************/
int Reg_Table_Initialize( REGISTER_TYPE table[] )
{ /* TODO: does not appear to be used. */
	(void) table;
	return SUCCESS;
}
/*********************************************************************************/
int Reg_Terminate( void )
{
	return SUCCESS;
}
/*********************************************************************************/
int Reg_Initialize( void )
{
	return SUCCESS;
}
/*********************************************************************************/
module_definition_type module_reg_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	Reg_Initialize,
	Reg_Terminate,
	MODULE_REG_NAME,
	REG_MODULE_REV,
	NULL,
	NULL
};
