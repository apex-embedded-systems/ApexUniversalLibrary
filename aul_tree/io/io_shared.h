#ifndef IO_SHARED_H__
#define IO_SHARED_H__
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

/*******************************************************************************/
#define IO_SHARED_H_REV  "$Date: 2014-06-09 17:39:15 -0500 (Mon, 09 Jun 2014) $"

/*******************************************************************************/
#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/cmd_macros.h"  /* IOC_T */

/* #include "../shared/module_macros.h" */
/********************************************************************************
Description
Byte ordering is about the output to the registers, not about our CPU!
We let the compiler optimize, hopefully.
*/
/* http://stackoverflow.com/questions/3957252/is-there-any-way-to-compute-the-width-of-an-integer-type-at-compile-time
   Number of bits in inttype_MAX, or in any (1<<k)-1 where 0 <= k < 2040 */
#define UINT_BITS_WIDTH_GIVEN_UINT_MAX(uintx_max) ( ( ( (uintx_max) / ( (uintx_max) % 255 + 1 ) / 255 ) % 255 ) * 8 + 7 - 86 / ( ( (uintx_max) % 255 ) + 12 ) )

/*** 16-bit */
#define IO_U16_SHIFT_LO  0
#define IO_U16_SHIFT_HI  ( UINT_BITS_WIDTH_GIVEN_UINT_MAX(UINT8_MAX) )
#define IO_U8_MASK		 UINT8_MAX

#define IO_U16_GET_U8_HI(u16_value_in)  ( (uint8_t)( ( (u16_value_in) >> IO_U16_SHIFT_HI ) & IO_U8_MASK ) )
#define IO_U16_GET_U8_LO(u16_value_in)  ( (uint8_t)( ( (u16_value_in) >> IO_U16_SHIFT_LO ) & IO_U8_MASK ) )

#define IO_U16_SET_U8_HI(var_io,u8_value_in)  ( (uint16_t)( ( ( ( (uint16_t) u8_value_in ) & IO_U8_MASK ) << IO_U16_SHIFT_HI ) | ( (uint16_t) var_io ) ) )
#define IO_U16_SET_U8_LO(var_io,u8_value_in)  ( (uint16_t)( ( ( ( (uint16_t) u8_value_in ) & IO_U8_MASK ) << IO_U16_SHIFT_LO ) | ( (uint16_t) var_io ) ) )

#define IO_U16_SET_U8_LO_HI(u8_lo,u8_hi)											\
	( (uint16_t)(																			\
		( ( ( (uint16_t) u8_lo ) & IO_U8_MASK ) << IO_U16_SHIFT_LO )  |	\
		( ( ( (uint16_t) u8_hi ) & IO_U8_MASK ) << IO_U16_SHIFT_HI )  )	)


/*** 32-bit */
#define IO_U32_SHIFT_LO  0
#define IO_U32_SHIFT_HI  ( UINT_BITS_WIDTH_GIVEN_UINT_MAX(UINT16_MAX) )
#define IO_U16_MASK		 UINT16_MAX

#define IO_U32_GET_U16_HI(u32_value_in)  ( (uint16_t)( ( (u32_value_in) >> IO_U32_SHIFT_HI ) & IO_U16_MASK ) )
#define IO_U32_GET_U16_LO(u32_value_in)  ( (uint16_t)( ( (u32_value_in) >> IO_U32_SHIFT_LO ) & IO_U16_MASK ) )

#define IO_U32_SET_U16_HI(var_io,u16_value_in)  ( (uint32_t)( ( ( ( (uint32_t) u16_value_in ) & IO_U16_MASK ) << IO_U32_SHIFT_HI ) | ( (uint32_t) var_io ) ) )
#define IO_U32_SET_U16_LO(var_io,u16_value_in)  ( (uint32_t)( ( ( ( (uint32_t) u16_value_in ) & IO_U16_MASK ) << IO_U32_SHIFT_LO ) | ( (uint32_t) var_io ) ) )

#define IO_U32_SET_U16_LO_HI(u16_lo,u16_hi)									\
	( (uint32_t)(															\
		( ( ( (uint32_t) u16_lo ) & IO_U16_MASK ) << IO_U32_SHIFT_LO )  |	\
		( ( ( (uint32_t) u16_hi ) & IO_U16_MASK ) << IO_U32_SHIFT_HI )  )	)


/*******************************************************************************/
#define IO_DEFINITION_ACRONYM_SIZE  16
/*******************************************************************************/
struct io_definition
{
	/* physical_id
		The physical id is both the logical_id along with the instance.  
	*/
	IOC_T			physical_id;
	/* minor number
		Same as board number within driver level.  
	*/
	size_t			minor_number;
	size_t			region_offset;
	size_t			region_bytes;
	size_t			region_restrict_8bit;  /* this is a bus width restriction */
	size_t			interrupt_number;
	/* acronym
		Same name given to file nodes.  For example, if we have two tracere boards,
		then the acronym will be "tracere0" and "tracere1" where the minor number
		is appended to the generic acronym.  If only a generic acronym is available
		and sent to the driver to retrieve information, it will pull the information
		for the device that is not active.  For example, if we send "tracere" and
      we have "tracer0" and "tracer1", and "tracer0" has already been open, it will
      then simply return the information for "tracer1" and update the acronym to
		support it.  If we had sent "tracer0" and it was open and we try to open with
		it, it will return an error (i.e. IO_Definition_Get() ).
		
		<acronym><minor_number>
	 */
	char			acronym[IO_DEFINITION_ACRONYM_SIZE];
};
typedef struct io_definition io_definition_type;
/*******************************************************************************/
typedef void ( * io_int_handler )( void * private_data );
/*******************************************************************************/
struct io_interrupt_info
{
	size_t				interrupt_number;
	io_int_handler		handler;
	void *				private_data;
};
typedef struct io_interrupt_info io_interrupt_info_type;
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************/
#define IO_XFR_BITS_TYPE		3
#define IO_XFR_BITS_DIR			2
#define IO_XFR_BITS_REPEAT		1
/*---------------------------------------------*/
#define IO_XFR_MASK_TYPE		( (1 << IO_XFR_BITS_TYPE	) -1 )
#define IO_XFR_MASK_DIR			( (1 << IO_XFR_BITS_DIR		) -1 )
#define IO_XFR_MASK_REPEAT		( (1 << IO_XFR_BITS_REPEAT	) -1 )
/*---------------------------------------------*/
#define IO_XFR_SHIFT_TYPE		0
#define IO_XFR_SHIFT_DIR		( IO_XFR_SHIFT_TYPE		+ IO_XFR_BITS_TYPE	)
#define IO_XFR_SHIFT_REPEAT		( IO_XFR_SHIFT_DIR		+ IO_XFR_BITS_DIR	)
/*---------------------------------------------*/
#define IO_XFR_TYPE_U8			sizeof( uint8_t  )
#define IO_XFR_TYPE_U16			sizeof( uint16_t )
#define IO_XFR_TYPE_U32			sizeof( uint32_t )
/*---------------------------------------------*/
#define IO_XFR_U8				( ( IO_XFR_TYPE_U8   & IO_XFR_MASK_TYPE ) << IO_XFR_SHIFT_TYPE )
#define IO_XFR_U16				( ( IO_XFR_TYPE_U16  & IO_XFR_MASK_TYPE ) << IO_XFR_SHIFT_TYPE )
#define IO_XFR_U32				( ( IO_XFR_TYPE_U32  & IO_XFR_MASK_TYPE ) << IO_XFR_SHIFT_TYPE )
/*---------------------------------------------*/
#define IO_XFR_DIR_READ			0x01
#define IO_XFR_DIR_WRITE		0x02
/*---------------------------------------------*/
#define IO_XFR_READ				( ( IO_XFR_DIR_READ   & IO_XFR_MASK_DIR ) << IO_XFR_SHIFT_DIR )
#define IO_XFR_WRITE			( ( IO_XFR_DIR_WRITE  & IO_XFR_MASK_DIR ) << IO_XFR_SHIFT_DIR )
/*---------------------------------------------*/
#define IO_XFR_REPEAT			( ( (0x01) & IO_XFR_MASK_REPEAT ) << IO_XFR_SHIFT_REPEAT )
/*---------------------------------------------*/
#define IO_XFR_GET_TYPE(cmd)	( ( (cmd) >> IO_XFR_SHIFT_TYPE ) & IO_XFR_MASK_TYPE )
/*---------------------------------------------*/
enum io_xfr_command
{ 
	IO_XFR_NONE				= 0,
	IO_XFR_WRITE_U8			= IO_XFR_WRITE | IO_XFR_U8,
	IO_XFR_WRITE_U16		= IO_XFR_WRITE | IO_XFR_U16,
	IO_XFR_WRITE_U32		= IO_XFR_WRITE | IO_XFR_U32,
	IO_XFR_READ_U8			= IO_XFR_READ  | IO_XFR_U8,
	IO_XFR_READ_U16			= IO_XFR_READ  | IO_XFR_U16,
	IO_XFR_READ_U32			= IO_XFR_READ  | IO_XFR_U32,
	/*---------------------------------------------*/
	IO_XFR_WRITE_U8_REPEAT	= IO_XFR_REPEAT | IO_XFR_WRITE | IO_XFR_U8,
	IO_XFR_WRITE_U16_REPEAT	= IO_XFR_REPEAT | IO_XFR_WRITE | IO_XFR_U16,
	IO_XFR_WRITE_U32_REPEAT	= IO_XFR_REPEAT | IO_XFR_WRITE | IO_XFR_U32,
	IO_XFR_READ_U8_REPEAT	= IO_XFR_REPEAT | IO_XFR_READ  | IO_XFR_U8,
	IO_XFR_READ_U16_REPEAT	= IO_XFR_REPEAT | IO_XFR_READ  | IO_XFR_U16,
	IO_XFR_READ_U32_REPEAT	= IO_XFR_REPEAT | IO_XFR_READ  | IO_XFR_U32
};
typedef enum io_xfr_command IO_XFR_COMMAND_ENUM;

/*******************************************************************************/
#define IO_XFR_CREATE(repeat,dir,type)    \
	( (enum io_xfr_command)												\
	(	( ((repeat) & IO_XFR_MASK_REPEAT ) << IO_XFR_SHIFT_REPEAT )  |	\
		( ((dir)    & IO_XFR_MASK_DIR    ) << IO_XFR_SHIFT_DIR    )  |	\
		( ((type)   & IO_XFR_MASK_TYPE   ) << IO_XFR_SHIFT_TYPE   )  )  )
/*******************************************************************************/
struct io_xfr
{
	enum io_xfr_command	command;

	size_t		offset;
	int			reg_id;

	size_t		buf_len;
	union
	{
		uint8_t		u8;
		int8_t		i8;
		uint16_t 	u16;
		int16_t		i16;
		uint32_t	u32;
		int32_t  	i32;
		char *		buf;
	} data;

	int	error_code;
};
typedef struct io_xfr io_xfr_type;

/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/

/******************************************************************************
Description

Purpose
Provides upper layers of code an idea of how this interface layer will function.
For example: upper layer STX104 code may split certain tasks with the specific
STX104 driver.

TODO
1. is there an easier way??
*/
/*---------------------------------------------*/
#define IO_BEHAVIOR_SIMULATOR  	0
#define IO_BEHAVIOR_GENERIC	 	1		/* generic driver layer				*/
#define IO_BEHAVIOR_GEN_X86     2		/* generic driver layer for x86		*/
#define IO_BEHAVIOR_GEN_ARM	    3		/* generic driver layer for ARM		*/
#define IO_BEHAVIOR_STX104		4		/* STX104 driver layer				*/
#define IO_BEHAVIOR_SUMMIT		5		/* Summit driver layer				*/
#define IO_BEHAVIOR_TRACERE	 	6		/* Tracer driver layer				*/
/*---------------------------------------------*/
#define IO_LAYER_LIBRARY		0		/* resides in application or library layer */
#define IO_LAYER_KERNEL			1		/* resides in driver or kernel layer		 */
/*---------------------------------------------*/
#define IO_OS_DOS				0		/* DOS							*/
#define IO_OS_WIN32				1		/* XP or Win7 32-bit			*/
#define IO_OS_LINUX				2		/* Linux						*/
/*---------------------------------------------*/
#define IO_ID_BITS_BEHAVIOR   	4
#define IO_ID_BITS_LAYER      	4
#define IO_ID_BITS_OS			4
/*---------------------------------------------*/
#define IO_ID_MASK_BEHAVIOR		( (1 << IO_ID_BITS_BEHAVIOR		) -1 )
#define IO_ID_MASK_LAYER		( (1 << IO_ID_BITS_LAYER		) -1 )
#define IO_ID_MASK_OS			( (1 << IO_ID_BITS_OS			) -1 )
/*---------------------------------------------*/
#define IO_ID_SHIFT_BEHAVIOR	0
#define IO_ID_SHIFT_LAYER		( IO_ID_SHIFT_BEHAVIOR	+ IO_ID_BITS_BEHAVIOR  )
#define IO_ID_SHIFT_OS			( IO_ID_SHIFT_LAYER		+ IO_ID_BITS_LAYER  )
/*---------------------------------------------*/
#define IO_ID_CREATE(os,layer,behavior) \
	(	    ( ( ((uint32_t)(layer))     & IO_ID_MASK_LAYER )    << IO_ID_SHIFT_LAYER    )  |   \
			 ( ( ((uint32_t)(behavior)) & IO_ID_MASK_BEHAVIOR ) << IO_ID_SHIFT_BEHAVIOR )  |   \
			 ( ( ((uint32_t)(os))		& IO_ID_MASK_OS )       << IO_ID_SHIFT_OS       )      )

/*---------------------------------------------*/
#define IO_ID_GET_BEHAVIOR(id)	( ( (id) >> IO_ID_SHIFT_BEHAVIOR )	& IO_ID_MASK_BEHAVIOR	)	
#define IO_ID_GET_LAYER(id)		( ( (id) >> IO_ID_SHIFT_LAYER    )	& IO_ID_MASK_LAYER		)
#define IO_ID_GET_OS(id)		( ( (id) >> IO_ID_SHIFT_OS       )	& IO_ID_MASK_OS			)
/*---------------------------------------------*/
enum io_id
{
 IO_ID_DOS_LIB_SIMULATOR	=  IO_ID_CREATE( IO_OS_DOS,		IO_LAYER_LIBRARY,	IO_BEHAVIOR_SIMULATOR	),
 IO_ID_WIN32_LIB_SIMULATOR	=  IO_ID_CREATE( IO_OS_WIN32,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_SIMULATOR	),
 IO_ID_LINUX_LIB_SIMULATOR	=  IO_ID_CREATE( IO_OS_LINUX,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_SIMULATOR	),
 IO_ID_DOS_LIB_GENERIC		=  IO_ID_CREATE( IO_OS_DOS,		IO_LAYER_LIBRARY,	IO_BEHAVIOR_GENERIC		),
 IO_ID_WIN32_LIB_GENERIC	=  IO_ID_CREATE( IO_OS_WIN32,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_GENERIC		),
 IO_ID_LINUX_LIB_GENERIC	=  IO_ID_CREATE( IO_OS_LINUX,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_GENERIC		),
 IO_ID_DOS_LIB_SUMMIT		=  IO_ID_CREATE( IO_OS_DOS,		IO_LAYER_LIBRARY,	IO_BEHAVIOR_SUMMIT		),
 IO_ID_WIN32_LIB_SUMMIT		=  IO_ID_CREATE( IO_OS_WIN32,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_SUMMIT		),
 IO_ID_LINUX_LIB_SUMMIT		=  IO_ID_CREATE( IO_OS_LINUX,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_SUMMIT		),
 IO_ID_DOS_LIB_TRACERE		=  IO_ID_CREATE( IO_OS_DOS,		IO_LAYER_LIBRARY,	IO_BEHAVIOR_TRACERE		),
 IO_ID_WIN32_LIB_TRACERE	=  IO_ID_CREATE( IO_OS_WIN32,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_TRACERE		),
 IO_ID_LINUX_LIB_TRACERE	=  IO_ID_CREATE( IO_OS_LINUX,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_TRACERE		),
 IO_ID_DOS_LIB_STX104		=  IO_ID_CREATE( IO_OS_DOS,		IO_LAYER_LIBRARY,	IO_BEHAVIOR_STX104		),
 IO_ID_WIN32_LIB_STX104		=  IO_ID_CREATE( IO_OS_WIN32,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_STX104		),
 IO_ID_LINUX_LIB_STX104		=  IO_ID_CREATE( IO_OS_LINUX,	IO_LAYER_LIBRARY,	IO_BEHAVIOR_STX104		),
 IO_ID_LINUX_DRV_GEN_X86	=  IO_ID_CREATE( IO_OS_LINUX, 	IO_LAYER_KERNEL,  	IO_BEHAVIOR_GEN_X86		),
 IO_ID_LINUX_DRV_GEN_ARM	=  IO_ID_CREATE( IO_OS_LINUX, 	IO_LAYER_KERNEL,  	IO_BEHAVIOR_GEN_ARM		)
};
typedef enum io_id IO_ID_ENUM;


/*******************************************************************************/

#endif /* IO_SHARED_H__ */

