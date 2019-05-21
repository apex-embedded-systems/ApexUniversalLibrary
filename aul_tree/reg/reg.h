#ifndef MODULE_REG
#define MODULE_REG 9 /* module ID number */
#define MODULE_REG_NAME "reg"
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

#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/target.h"
#include "../shared/debug.h"

#include "../shared/module_macros.h"

/*****************************************************************************/	
/*****************************************************************************/


/*********************************************************************************/
/* if DEBUG then allow REG_DEBUG */
#define REG_DEBUG		   _DEBUG



/******************************************************************
Description
Parameter values within the info record.

Notes:
The following macros sole purpose is to aid in readability only:
	REG_BYTES, 
	REG_AUTO_INCREMENT, 
	REG_SHADOW, 
	REG_APERTURE

*/
/* used to compute element size and register aperture, since both are offset by one.  1 <= x <= 8 */
#define REG_BYTES_LESS_ONE(x)  ((x) - 1)
#define REG_WIDTH_BYTES(x)	    (x)

/* register aperture width in bytes */
#define REG_APERTURE_BYTES(x)   (x)
/* direction - these are OR'd together */
enum
{
	REG_DIR_NONE			= 0,
	REG_DIR_READ			= 1,
	REG_DIR_WRITE			= 2,
	REG_DIR_READ_WRITE		= 3
};
/* register shadow, parameter either 0 or 1 */
#define REG_SHADOW(x)			(x)
/* register width */
enum
{
REG_SIZE_UINT8		=	sizeof( uint8_t ),
REG_SIZE_UINT16		=	sizeof( uint16_t ),
#if defined( __MSDOS__ )
REG_SIZE_UINT32		=	sizeof( uint32_t ),
#else
REG_SIZE_UINT32		=	sizeof( uint32_t ),
REG_SIZE_UINT64		=	sizeof( uint64_t )
#endif
};
/* auto increment, parameter either 0 or 1 */
#define REG_AUTO_INCREMENT(x)  (x)

/******************************************************************
Description
Info bit fields

TODO: adjust for more optimal arrangement??

	Info_valid[15:11]
	Auto_incr[10]
	Shadow_copy[9]
	Direction[8:7]
	Element_size[6:3]
	Aperture[2:0]
*/
/* bit width of each parameter */
#define REG_BITS_REGISTER_APERTURE			3
#define REG_BITS_ELEMENT_SIZE				4
#define REG_BITS_DIRECTION					2
#define REG_BITS_SHADOW_COPY				1
#define REG_BITS_AUTO_INCREMENT				1
#define REG_BITS_INFO_VALID					5

/* mask of each parameter */
#define REG_MASK_REGISTER_APERTURE			( (1 << REG_BITS_REGISTER_APERTURE        ) -1 )
#define REG_MASK_ELEMENT_SIZE					( (1 << REG_BITS_ELEMENT_SIZE         ) -1 )
#define REG_MASK_DIRECTION						( (1 << REG_BITS_DIRECTION            ) -1 )
#define REG_MASK_SHADOW_COPY					( (1 << REG_BITS_SHADOW_COPY          ) -1 )
#define REG_MASK_AUTO_INCREMENT				( (1 << REG_BITS_AUTO_INCREMENT           ) -1 )
#define REG_MASK_INFO_VALID					( (1 << REG_BITS_INFO_VALID		          ) -1 )

/* bit shift for each paremeter */
#define REG_SHIFT_REGISTER_APERTURE			0
#define REG_SHIFT_ELEMENT_SIZE				( REG_SHIFT_REGISTER_APERTURE	+ REG_BITS_REGISTER_APERTURE	)
#define REG_SHIFT_DIRECTION					( REG_SHIFT_ELEMENT_SIZE		+ REG_BITS_ELEMENT_SIZE			)	
#define REG_SHIFT_SHADOW_COPY					( REG_SHIFT_DIRECTION			+ REG_BITS_DIRECTION			)
#define REG_SHIFT_AUTO_INCREMENT				( REG_SHIFT_SHADOW_COPY			+ REG_BITS_SHADOW_COPY			)
#define REG_SHIFT_INFO_VALID					( REG_SHIFT_AUTO_INCREMENT		+ REG_MASK_AUTO_INCREMENT		)
	
/* decode/get for each parameter */
#define REG_GET_REGISTER_APERTURE(x)	( ( (x) >> REG_SHIFT_REGISTER_APERTURE )         & REG_MASK_REGISTER_APERTURE         )
#define REG_GET_ELEMENT_SIZE(x)			( ( (x) >> REG_SHIFT_ELEMENT_SIZE )              & REG_MASK_ELEMENT_SIZE              )
#define REG_GET_DIRECTION(x)				( ( (x) >> REG_SHIFT_DIRECTION )                 & REG_MASK_DIRECTION                 )
#define REG_GET_SHADOW_COPY(x)			( ( (x) >> REG_SHIFT_SHADOW_COPY )               & REG_MASK_SHADOW_COPY               )
#define REG_GET_AUTO_INCREMENT(x)		( ( (x) >> REG_SHIFT_AUTO_INCREMENT )            & REG_MASK_AUTO_INCREMENT            )

/* encode/set for each parameter */
#define REG_SET_REGISTER_APERTURE(info,apr)	( ( ( (apr)  & REG_MASK_REGISTER_APERTURE )	<< REG_SHIFT_REGISTER_APERTURE ) |  \
																( (info) & ~( REG_MASK_REGISTER_APERTURE	<< REG_SHIFT_REGISTER_APERTURE )		)  )
#define REG_SET_ELEMENT_SIZE(info,size)		( ( ( (size) & REG_MASK_ELEMENT_SIZE )			<< REG_SHIFT_ELEMENT_SIZE )	|  \
																( (info) & ~( REG_MASK_ELEMENT_SIZE			<< REG_SHIFT_ELEMENT_SIZE )		)  )
#define REG_SET_DIRECTION(info,dir)				( ( ( (dir)  & REG_MASK_DIRECTION )				<< REG_SHIFT_DIRECTION )		|  \
																( (info) & ~( REG_MASK_DIRECTION				<< REG_SHIFT_DIRECTION )			)  )
#define REG_SET_SHADOW_COPY(info,sc)			( ( ( (sc)   & REG_MASK_SHADOW_COPY )			<< REG_SHIFT_SHADOW_COPY )		|  \
																( (info) & ~( REG_MASK_SHADOW_COPY			<< REG_SHIFT_SHADOW_COPY )			)  )
#define REG_SET_AUTO_INCREMENT(info,ai)		( ( ( (ai)   & REG_MASK_AUTO_INCREMENT )		<< REG_SHIFT_AUTO_INCREMENT )	|  \
																( (info) & ~( REG_MASK_AUTO_INCREMENT		<< REG_SHIFT_AUTO_INCREMENT )	 )  )

/* quick value tests -- eliminating shifts */
#define REG_TEST_DIRECTION_WRITE(x)		( (x) & ( (REG_MASK_DIRECTION & REG_DIR_WRITE)	<< REG_SHIFT_DIRECTION      )   ) 
#define REG_TEST_DIRECTION_READ(x)		( (x) & ( (REG_MASK_DIRECTION & REG_DIR_READ)	<< REG_SHIFT_DIRECTION      )   )
#define REG_TEST_SHADOW_COPY(x)			( (x) & ( REG_MASK_SHADOW_COPY						<< REG_SHIFT_SHADOW_COPY    )   )
#define REG_TEST_AUTO_INCREMENT(x)		( (x) & ( REG_MASK_AUTO_INCREMENT					<< REG_SHIFT_AUTO_INCREMENT )   )


/******************************************************************
Description
Info -- Encode/populate with all parameters

   Re-organized within the macro so that it is easier to enter the information into the register table.  
   My preferred order is the following:
		Address mode, size, direction, auto-increment, shadow, register-aperture, element-size-div-aperture 
*/
#define REG_CREATE_INFO(size,dir,ai,shadow,aperture)													   \
(																														   \
	( ( (REG_BYTES_LESS_ONE(aperture))	& REG_MASK_REGISTER_APERTURE   ) << REG_SHIFT_REGISTER_APERTURE ) |  \
	( ( (dir)			& REG_MASK_DIRECTION                 ) << REG_SHIFT_DIRECTION                 ) |  \
	( ( (shadow)		& REG_MASK_SHADOW_COPY               ) << REG_SHIFT_SHADOW_COPY               ) |  \
	( ( (size)			& REG_MASK_ELEMENT_SIZE              ) << REG_SHIFT_ELEMENT_SIZE              ) |  \
	( ( (ai)				& REG_MASK_AUTO_INCREMENT				 ) << REG_SHIFT_AUTO_INCREMENT            ) |  \
    ( ( REG_MASK_INFO_VALID << REG_SHIFT_INFO_VALID                                                ) )  )


/*****************************************************************************/
#define REG_BANK_NONE  ( -1 )

/*****************************************************************************/
/*****************************************************************************/
enum { REG_INFO_NONE = 0 };

/******************************************************************************
*/
/*
typedef void (* REGISTER_READ_WRITE_TYPE )( REGISTER_TYPE * reg, void* buffer, int count );
*/
/******************************************************************
Description
Register definition used to create a register table for specific hardware.
*/
typedef struct REGISTER_TYPE
{
#if defined( TARGET_CHECK_BUILD )
	/* SYS_TYPE__REGISTER_TYPE */
#endif
	/* info to include:
		x	- aperture ( bus and register aperture.  precomputed at initialization, fixup ).  -- can be at any bit location
		x	- element size ( known at compile time ) 
		x	- element_size_div_aperture  ( calculated at initialization, fixup) -- prefer at LSB location
		x	- auto_increment ( known at compile time )  -- can be at any bit location
		x	- address mode   ( known at compile time )  -- prefer at LSB location since used to index a table.
			? direction access ( known at compile time ) -- can be at any bit location
			? shadow copy	   ( known at compile time ) -- can be at any bit location
			? array index      ( known at compile time ) <-- no longer required, due to auto_increment!

		Functions/macros supporting info:
			REG_INFO_ADDRESS_MODE( info )
			REG_INFO_SIZE_DIV_APERTURE( info )
			REG_INFO_AUTO_INCREMENT( info )
			REG_INFO_APERTURE( info )

		Bit structure:  keep to 16-bits so this will run on anything.

			location	bit count	description
			info[1:0]	 2				address mode   --- DEPRECATED
										  COMPLE_TIME: set value

			info[4:2]	 3				aperture - number of bytes, 0=1-byte, 7=8-byte.  Register aperture!  (limited to direct registers)
										  NOTE: If you have a transaction that is limited to 8-bits wide, due to a port issue, this is where
										        you restrict the transaction width, otherwise it be limited by bus aperature width.
										  COMPLE_TIME: set value
										  FIXUP: if bus aperature is less, then restrict to bus aperature width.

			info[6:5]	 2				direction 
										  COMPLE_TIME: set value  
											00 - no access
											01 - read
											10 - write
											11 - write and read

			info[7]		 1				shadow copy
										  COMPILE_TIME: set value


			info[11:8]	 3				element size  - number of bytes, 0=1-byte, 7=8-byte
										  COMPILE_TIME: set value

			info[12]	 1				auto-increment  (NOTE: this can be used to pass an array of values)
										  COMPILE_TIME: set value

			info[15:13]	 4				non-zero marker (to distinguish between null or end of register list 8/


			DO NOT USE UNIONS, THEY BREAK ENDIANNESS!!!!
			LET THE COMPILER FIGURE OUT THE OPTIMIZATION STRATEGY!

	*/
	size_t						info;  
	/* NOTE:
	    For misc parameters, specific to a register.
		 Example: used in the serial UART definition to indicate that it is a banked register.

		 Typically used to determine the value associated with a bank register.  It is assumed
		 that if the banking component is used that the banking pattern is predefined within
		 this state value.

		 Can also be used to hold temporary data, if not used for banking purposes.
		 This parameter is not used within the reg.c functions.
	*/
	int							state;  /* typically used to set the bank register */

/*TODO: temporary -- need to remove.
	size_t						param2;*/
	/*
		Direct:		offset
		Indirect:	offset
	*/
	size_t						virtual_address;

#if defined( REG_DEBUG )
	const char * name;
#endif 
/*	char			name[REG_NAME_SIZE];  only needed if we have a CLI    */
} REGISTER_TYPE;


/*****************************************************************************/	
/*****************************************************************************/
/*****************************************************************************/

/* <INTERNAL_BEGIN> */
/******************************************************************************
Description
Register ID bit fields
*/
#if(0)
/* bit width of each parameter */	
#define REG_BITS_ID_REG_OFFSET				8
#define REG_BITS_ID_GENERIC_COMPONENT		8

/* mask of each parameter */
#define REG_MASK_ID_REG_OFFSET		
#define REG_MASK_ID_GENERIC_COMPONENT

/* bit shift for each parameter */
#define REG_SHIFT_ID_REG_OFFSET		
#define REG_SHIFT_ID_GENERIC_COMPONENT

/* decode/get for each parameter */
#define REG_GET_ID_REG_OFFSET		
#define REG_GET_ID_GENERIC_COMPONENT

/* encode/set for each parameter */
#define REG_SET_ID_REG_OFFSET		
#define REG_SET_ID_GENERIC_COMPONENT
#endif

/* <INTERNAL_END> */


/******************************************************************
Description
Used to create a generic register ID that is composed of the generic component and
the register offset within that component.

Example:
#define YF_AI_STATUS  REG_CREATE_ID( COMP_AI24SE8DI4 | AI24SE8DI4_STATUS ) 

In this example we are creating a generic register that can be used by top level software
irrespective of the underlying component revision.  This is built at compile time.  The
reason we can build this at compile time is because we are restricting the component register
table within the device register set by way of a specific component ID to a generic component ID.
This component ID becomes the index into the register set table.

REGISTER_SET_TYPE yf_dev[] =
{
	{ <offset>, <specific component register table> } The first entry will be restricted to be any "AI24SE8DI4" type components.
	...
}

*/
#define REGISTER_CREATE_ID(component,reg_id)																 \
(																														 \
   ( ( (component)& REG_MASK_ID_GENERIC_COMPONENT	) << REG_SHIFT_ID_GENERIC_COMPONENT	) | \
	( ( (reg_id)	& REG_MASK_REG_OFFSET				) << REG_SHIFT_REG_OFFSET				)   ) 

/*******************************************************************/
typedef struct REGISTER_SET_TYPE_
{
#if defined( TARGET_CHECK_BUILD )
	/* SYS_TYPE__REGISTER_SET_TYPE */
#endif
	size_t			offset;		/* base address for this register table		*/
	REGISTER_TYPE *	table;		/* register table										*/
} REGISTER_SET_TYPE;

/*****************************************************************************/	
/*****************************************************************************/
/*****************************************************************************/

#define REG_SPACE_IO_DIRECT		2
#define REG_SPACE_MEM_DIRECT	3

#if defined( TARGET_DEFAULT_ADDRESS_SPACE )
	#if 0 == TARGET_DEFAULT_ADDRESS_SPACE
	 #define REG_DEFAULT_ADDRESS_SPACE  REG_SPACE_IO_DIRECT
	#endif 
	/* TODO: add other default address spaces here */
#else
	#define REG_DEFAULT_ADDRESS_SPACE  REG_MODE_DIRECT
#endif
/*****************************************************************************/	
/*****************************************************************************/
/*****************************************************************************/

#define REG_EXTRACT_ENUM(symbol,index,offset,register_bytes,aperture_bytes,read_write,name,state) symbol = (index),

/* <INTERNAL_BEGiN> */
#if(0)
#if defined( REG_DEBUG )
# define REG_LIST_NULL { 0, 0, 0, NULL }
# define REG_EXTRACT_PARAMS(symbol,index,offset,register_bytes,aperture_bytes,read_write,name) 	\
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),									 		\
					  read_write,																\
					  REG_AUTO_INCREMENT(0),													\
					  REG_SHADOW(0),															\
					  REG_APERTURE_BYTES(aperture_bytes) 										\
				),						/* info					*/								\
	  0,								/* param1				*/								\
	  offset,							/* virtual address		*/								\
	  name,								/* name					*/								\
	 },
#else
# define REG_LIST_NULL { 0, 0, 0 }
# define REG_EXTRACT_PARAMS(symbol,index,offset,register_bytes,aperture_bytes,read_write,name) 	\
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),									 		\
					  read_write,																\
					  REG_AUTO_INCREMENT(0),													\
					  REG_SHADOW(0),															\
					  REG_APERTURE_BYTES(aperture_bytes) 										\
				),						/* info					*/								\
	  0,								/* param1				*/								\
	  offset							/* virtual address	*/									\
	 },
#endif
#endif
/* <INTERNAL_END> */


#if defined( REG_DEBUG )
# define REG_LIST_NULL { 0, 0, 0, 0, NULL }
# define REG_EXTRACT_PARAMS(symbol,index,offset,register_bytes,aperture_bytes,read_write,name,state) 	\
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),									 		\
					  read_write,																\
					  REG_AUTO_INCREMENT(0),													\
					  REG_SHADOW(0),															\
					  REG_APERTURE_BYTES(aperture_bytes) 										\
				),						/* info					*/								\
	  0,								/* param1				*/								\
	  offset,							/* virtual address		*/								\
	  state,							/* state or bank		*/								\
	  name,								/* name					*/								\
	 },
#else
# define REG_LIST_NULL { 0, 0, 0, 0 }
# define REG_EXTRACT_PARAMS(symbol,index,offset,register_bytes,aperture_bytes,read_write,name,state) 	\
	{ REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),									 		\
					  read_write,																\
					  REG_AUTO_INCREMENT(0),													\
					  REG_SHADOW(0),															\
					  REG_APERTURE_BYTES(aperture_bytes) 										\
				),						/* info					*/								\
	  0,								/* param1				*/								\
	  offset,							/* virtual address	*/									\
	  state	 							/* state or bank		*/								\
	 },
#endif


/*****************************************************************************/	
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/

void Reg_Write( REGISTER_TYPE   register_table[],	/* the register table to be referenced for this particular device */
				size_t          offset,				/* to support multiple devices with same register table, but at different physical offsets */
				int				iod,				/* IO descriptor */
				size_t          reg_id,				/* register row */
				char *			buf,				/* buffer passed as byte string, can always be recast as required		*/ 
				size_t			buf_len 			/* length of buffer in bytes == sizeof() units							*/
			  );

void Reg_Read( REGISTER_TYPE  	register_table[],	/* the register table to be referenced for this particular device */
			   size_t			offset,				/* to support multiple devices with same register table, but at different physical offsets */
			   int				iod,				/* IO descriptor */
			   size_t			reg_id,				/* register row */
			   char *			buf,				/* buffer passed as byte string, can always be recast as required		*/ 
			   size_t			buf_len				/* length of buffer in bytes == sizeof() units							*/
			  );

void Reg_Table_Offset_Range( REGISTER_TYPE  register_table[],	/* register table to test								*/
							 size_t *		start,				/* returns the start address, virtual address in table	*/
							 size_t *		end,				/* returns the end address, virtual address in table	*/
							 size_t *		length				/* returns the contiguous length from start to end		*/
						   );

int Reg_Table_Initialize( REGISTER_TYPE table[] );

/*******************************************************************/

int Reg_Terminate( void );

int Reg_Initialize( void );

extern module_definition_type module_reg_definition;

/*******************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* MODULE_REG */
