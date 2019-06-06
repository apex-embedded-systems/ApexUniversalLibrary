#ifndef CMD_MACROS_H_
#define CMD_MACROS_H_
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


/********************************************************************************/
#include "../shared/typebase.h"	 /* uint32_t		*/
#include "../shared/ioctl.h"      /* _IOC_TYPEBITS */
#include "../shared/target.h"

/*********************************************************************************/
typedef enum
{
	COMMAND_CSV_COLUMN_NONE						=  0,
	COMMAND_CSV_COLUMN_ENUM_VALUE				=  1,
	COMMAND_CSV_COLUMN_ENUM_SYMBOL				=  2,
	COMMAND_CSV_COLUMN_UNKNOWN					=  3
} COMMAND_CSV_COLUMN_ENUM;

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/* http://stackoverflow.com/questions/3957252/is-there-any-way-to-compute-the-width-of-an-integer-type-at-compile-time */
/* Number of bits in inttype_MAX, or in any (1<<k)-1 where 0 <= k < 2040 */
#define IOC_IMAX_BITS(m) ( (m)/((m)%255+1) / 255%255*8 + 7-86/((m)%255+12) )

#if(0)
/* Number of bits in inttype_MAX, or in any (1<<b)-1 where 0 <= b < 3E+10 */
#define IOC_IMAX_BITS(m) ( ((m + 1) /((m)%0x3fffffffL+1) /0x3fffffffL %0x3fffffffL *30 \
                  + (m + 1)%0x3fffffffL /((m + 1)%31+1)/31%31*5 + 4-12/((m + 1)%31+3)) )
#endif
/*
IOC_IMAX_BITS(255) ( (255)/((255)%255+1) / 255%255*8 + 7-86/((255)%255+12) )
IOC_IMAX_BITS(255) ( (255)/(0+1) / 255%255*8 + 7-86/(0+12) )
IOC_IMAX_BITS(255) ( 255 / 255 %255 * 8 + 7-86/12 )
IOC_IMAX_BITS(255) ( 1 %255 * 8 + 7-7 )
IOC_IMAX_BITS(255) ( 1 * 8 + 0 )

IOC_IMAX_BITS(65535) ( (65535)/((65535)%255+1) / 255%255*8 + 7-86/((65535)%255+12) )
IOC_IMAX_BITS(65535) ( (65535)/((65535)%255+1) / 255%255*8 + 7-86/((65535)%255+12) )
*/
/******************************************************************
Description
IO Control commands are composed of four bit fields shown below.
o type					- The type of driver or a default SYStem level command 
						  that is shared by all drivers.
o command				- The command to be done at the driver level, in other words
						  which function to vector too within the driver jump table.
o component physical id - The component to which this command is directed, if any.
o index					- index passed to the component which which channel (i.e. DIO[index], AI[index])


NOTE: there are some potential conflicts we need to deal with or verify that 
they are not conflicts in Linux.  It appears that these will not be in conflict
because the type will be determined to be SYS_TYPE__NONE.  The exceptions are:


refer to ioctls.h:   TIOCLINUX = 0x541C



	format:
	bits			width			description
	05:00			6				index 		Used to provide channel information 
														0 <= channel <= 63.				
	07:06			2				instance    The instance of the device or component or module.
														No more than 4 instances of a component or device.
	15:08			8				type			Magic number used to ID the driver commands basically.
														We will use this as a mechanism to filter out 
														undesired commands in/out of the driver and application.  
														So long as it is placed in the correct location, it ought 
														be compatible with Linux ioctl.h 
	31:16			8				command		Same as ordinal number in Linux ioctl.h, except in 
														a different bit string location.
	31:24			8				base_id		Identifies which module, device or component in which
														the command is intended to be delivered too.

Note: physical_id or logical_id is the combination of base_id and instance.  This excludes 
all other information within the command.


*/

/*******************************************************************/
typedef uint32_t IOC_T;

/*******************************************************************/
/* 
Description
AULDRV magic number.  

Assumption
It is an even number.
*/
#define IOC_MAGIC_NUMBER			0xEC 
	

/*************************************************
* IO Control Command Structure				     *
*************************************************/

/*---------------------------------------------*/
/* bit width of each parameter */
#define IOC_BITS_INDEX						6     			/* up to 64-channels 						*/
#define IOC_BITS_INSTANCE					2				/* number of same type components typical 	*/
#define IOC_BITS_TYPE						_IOC_TYPEBITS	/* typically 8-bits							*/
#define IOC_BITS_COMMAND					8
#define IOC_BITS_BASE_ID					8
/*---------------------------------------------*/
#define IOC_BITS_TOTAL						(IOC_BITS_INDEX + IOC_BITS_BASE_ID + IOC_BITS_INSTANCE + IOC_BITS_COMMAND + IOC_BITS_TYPE )

#if(0) /* temporary - GCC gives round off error? */
/* verifies that we are at exactly UINT32 bit width */
#if ( IOC_BITS_TOTAL > IOC_IMAX_BITS( UINT32_MAX ) )
#error  "ERROR: IOC bit count error within sysval.h"
#endif
#if ( IOC_BITS_TOTAL < IOC_IMAX_BITS( UINT32_MAX ) )
#error  "ERROR: IOC bit count error within sysval.h"
#endif
#endif

/*---------------------------------------------*/
/* mask of each parameter */
#define IOC_MASK_INDEX						((IOC_T)( (1 << IOC_BITS_INDEX			) -1 ))
#define IOC_MASK_INSTANCE					((IOC_T)( (1 << IOC_BITS_INSTANCE		) -1 ))
#define IOC_MASK_TYPE						((IOC_T)( (1 << IOC_BITS_TYPE			) -1 ))
#define IOC_MASK_COMMAND					((IOC_T)( (1 << IOC_BITS_COMMAND		) -1 ))
#define IOC_MASK_BASE_ID					((IOC_T)( (1 << IOC_BITS_BASE_ID		) -1 ))
#define IOC_DEVICE_ALL_SELECT				IOC_MASK_TYPE

/*---------------------------------------------*/
/* bit shift for each paremeter */
#define IOC_SHIFT_INDEX						0
#define IOC_SHIFT_INSTANCE					( IOC_SHIFT_INDEX			+ IOC_BITS_INDEX		)
#define IOC_SHIFT_TYPE						( IOC_SHIFT_INSTANCE		+ IOC_BITS_INSTANCE		)			
#define IOC_SHIFT_COMMAND					( IOC_SHIFT_TYPE			+ IOC_BITS_TYPE			)
#define IOC_SHIFT_BASE_ID					( IOC_SHIFT_COMMAND			+ IOC_BITS_COMMAND		)

/*---------------------------------------------*/
/* decode/get for each parameter */
#define IOC_GET_INDEX(iocmd)				((size_t)( ( ((IOC_T)(iocmd)) >> IOC_SHIFT_INDEX )		& IOC_MASK_INDEX		))
#define IOC_GET_INSTANCE(iocmd)				((size_t)( ( ((IOC_T)(iocmd)) >> IOC_SHIFT_INSTANCE )	& IOC_MASK_INSTANCE		))
#define IOC_GET_TYPE(iocmd)					((size_t)( ( ((IOC_T)(iocmd)) >> IOC_SHIFT_TYPE )		& IOC_MASK_TYPE			))
#define IOC_GET_COMMAND(iocmd)				((size_t)( ( ((IOC_T)(iocmd)) >> IOC_SHIFT_COMMAND )	& IOC_MASK_COMMAND		))
#define IOC_GET_BASE_ID(iocmd)				((size_t)( ( ((IOC_T)(iocmd)) >> IOC_SHIFT_BASE_ID )	& IOC_MASK_BASE_ID		))

/*---------------------------------------------*/
/* encode/set for each parameter */
#define IOC_SET_INDEX(iocmd,index)		( ( ( ((IOC_T)(index)) & IOC_MASK_INDEX )	<< IOC_SHIFT_INDEX		)	|  \
											( ((IOC_T)(iocmd)) & ~( IOC_MASK_INDEX	<< IOC_SHIFT_INDEX		)	)  )

#define IOC_SET_INSTANCE(iocmd,instance)	( ( ( ((IOC_T)(instance)) & IOC_MASK_INSTANCE )<< IOC_SHIFT_INSTANCE		)	|  \
												( ((IOC_T)(iocmd)) & ~( IOC_MASK_INSTANCE  << IOC_SHIFT_INSTANCE	)	)  )

#define IOC_SET_TYPE(iocmd,type)			( ( ( ((IOC_T)(type)) & IOC_MASK_TYPE )			<< IOC_SHIFT_TYPE		)	|  \
												( ((IOC_T)(iocmd)) & ~( IOC_MASK_TYPE		<< IOC_SHIFT_TYPE		)	)  )

#define IOC_SET_COMMAND(iocmd,cmd)		( ( ( ((IOC_T)(cmd)) & IOC_MASK_COMMAND )		<< IOC_SHIFT_COMMAND		)	|  \
											( ((IOC_T)(iocmd)) & ~( IOC_MASK_COMMAND	<< IOC_SHIFT_COMMAND		)	)  )

#define IOC_SET_BASE_ID(iocmd,cmd)		( ( ( ((IOC_T)(cmd)) & IOC_MASK_BASE_ID )		<< IOC_SHIFT_BASE_ID		)	|  \
											( ((IOC_T)(iocmd)) & ~( IOC_MASK_BASE_ID	<< IOC_SHIFT_BASE_ID		)	)  )

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
/*        MODULE, DEVICE and COMPONENT 
		LOGICAL_ID_, PHYSICAL_ID_, BASE_ID_
		     ABSTRACTED DATA FROM ABOVE
*/
/*******************************************************************/

/* For future reference:
	To obtain more IDs we would then need to utilize a different IOC_MAGIC_NUMBER  
   
	BASE_IDs in the case of MODULE and DEVICE will include instance bits since they
	are not used.  Last two bits of base_id for MODULE and DEVICE shall use the 
	instance values that are simply not used for these objects.

	To determine object type we simply need to mask upper bits of base_id bit string.

	BASE_ID
						7654 3210    <-- base_id bit string
	               9876 5432 10 <-- concatenated base_id & instance bit string
	MODULE MIN:		0000 0000 00    BASE_ID[7:0]  = MODULE_ID[9:2] and		Offset = 0
	MODULE MAX:    0000 1111 11    INSTANCE[1:0] = MODULE_ID[1:0].			Base_id range: 0 to 63.

	DEVICE MIN:		0001 0000 00	BASE_ID[7:0]  = DEVICE_ID[9:2] and		Offset = 16.   16 to 191
	DEVICE MAX:    0011 1111 11   INSTANCE[1:0] = DEVICE_ID[1:0].			Base_id range: 0 to 175.

	COMPONENT MIN:	0100 0000 00   Last two bits are instance information.
	COMPONENT MAX: 1111 1111 11   BASE_ID[7:0] = COMPONENT_ID[7:0] and INSTANCE[1:0] = instance of component.
																								Base_id range: 0 to 191.
																								Offset = 64

TODO: 
1. Testing ranges of base_ids.
*/
/*---------------------------------------------*/
#define IOC_LOGICAL_ID_NULL				0
#define IOC_PHYSICAL_ID_NULL			0
/*---------------------------------------------*/
#define IOC_OFFSET_MODULE					0x00  /*  0 */
#define IOC_OFFSET_DEVICE					0x10  /* 16 */
#define IOC_OFFSET_COMPONENT				0x40  /* 64 */
/*---------------------------------------------*/
#define IOC_MASK_OFFSET_MODULE			(  IOC_MASK_BASE_ID       & ( ~( (1 << IOC_IMAX_BITS( IOC_OFFSET_DEVICE    )	) -1 ) )	 )
#define IOC_MASK_OFFSET_DEVICE			(  IOC_MASK_OFFSET_MODULE & (  ( (1 << IOC_IMAX_BITS( IOC_OFFSET_COMPONENT )	) -1 ) )	 )
#define IOC_MASK_OFFSET_COMPONENT		(  IOC_MASK_BASE_ID       & ( ~( (1 << IOC_IMAX_BITS( IOC_OFFSET_COMPONENT )	) -1 ) )	 )	
/*---------------------------------------------*/
#define IOC_ADD_OFFSET_MODULE(base_id)			(  (IOC_T)( (base_id) + ( IOC_OFFSET_MODULE << IOC_BITS_INSTANCE ) )  )
#define IOC_ADD_OFFSET_DEVICE(base_id)			(  (IOC_T)( (base_id) + ( IOC_OFFSET_DEVICE << IOC_BITS_INSTANCE ) )  )
#define IOC_ADD_OFFSET_COMPONENT(base_id)		(  (IOC_T)( (base_id) + IOC_OFFSET_COMPONENT )  )
/*---------------------------------------------*/
#define IOC_SUB_OFFSET_MODULE(raw_id)			(  (IOC_T)( (raw_id) - ( IOC_OFFSET_MODULE << IOC_BITS_INSTANCE )    )	 )
#define IOC_SUB_OFFSET_DEVICE(raw_id)			(  (IOC_T)( (raw_id) - ( IOC_OFFSET_DEVICE << IOC_BITS_INSTANCE )    )	 )
#define IOC_SUB_OFFSET_COMPONENT(raw_id)		(  (IOC_T)( (raw_id) - IOC_OFFSET_COMPONENT )	 )
/*---------------------------------------------*/
#define IOC_CREATE_LOGICAL_ID_MODULE(base_id)																										\
	(   ( ( ( (IOC_ADD_OFFSET_MODULE(base_id)) >> IOC_BITS_INSTANCE ) & IOC_MASK_BASE_ID  ) << IOC_SHIFT_BASE_ID  )		|	\
	    ( ( ( (IOC_ADD_OFFSET_MODULE(base_id))                      ) & IOC_MASK_INSTANCE ) << IOC_SHIFT_INSTANCE )		)

#define IOC_CREATE_LOGICAL_ID_DEVICE(base_id)																										\
	(   ( ( ( (IOC_ADD_OFFSET_DEVICE(base_id)) >> IOC_BITS_INSTANCE ) & IOC_MASK_BASE_ID  ) << IOC_SHIFT_BASE_ID  )		|	\
	    ( ( ( (IOC_ADD_OFFSET_DEVICE(base_id))                      ) & IOC_MASK_INSTANCE ) << IOC_SHIFT_INSTANCE )		)

#define IOC_CREATE_LOGICAL_ID_COMPONENT(base_id)   ( ( (IOC_ADD_OFFSET_COMPONENT(base_id))	& IOC_MASK_BASE_ID  ) << IOC_SHIFT_BASE_ID )
/*---------------------------------------------*/
#define IOC_CREATE_PHYSICAL_ID_MODULE(base_id,instance)		IOC_CREATE_LOGICAL_ID_MODULE(base_id)
#define IOC_CREATE_PHYSICAL_ID_DEVICE(base_id,instance)		IOC_CREATE_LOGICAL_ID_DEVICE(base_id)

#define IOC_CREATE_PHYSICAL_ID_COMPONENT(base_id,instance)											\
	(																								\
		( ( (IOC_ADD_OFFSET_COMPONENT(base_id))	& IOC_MASK_BASE_ID  ) << IOC_SHIFT_BASE_ID	)  |	\
		( ( ((IOC_T)(instance))					& IOC_MASK_INSTANCE ) << IOC_SHIFT_INSTANCE	)  )
/*---------------------------------------------*/
#define IOC_MASK_LOGICAL_ID_MODULE		( ( IOC_MASK_BASE_ID << IOC_SHIFT_BASE_ID ) | ( IOC_MASK_INSTANCE << IOC_SHIFT_INSTANCE ) )
#define IOC_MASK_LOGICAL_ID_DEVICE		( ( IOC_MASK_BASE_ID << IOC_SHIFT_BASE_ID ) | ( IOC_MASK_INSTANCE << IOC_SHIFT_INSTANCE ) )
#define IOC_MASK_LOGICAL_ID_COMPONENT	(   IOC_MASK_BASE_ID << IOC_SHIFT_BASE_ID )
/*---------------------------------------------*/
#define IOC_MASK_PHYSICAL_ID           ( ( IOC_MASK_BASE_ID << IOC_SHIFT_BASE_ID ) | ( IOC_MASK_INSTANCE << IOC_SHIFT_INSTANCE ) )
#define IOC_MASK_PHYSICAL_ID_MODULE		IOC_MASK_PHYSICAL_ID
#define IOC_MASK_PHYSICAL_ID_DEVICE		IOC_MASK_PHYSICAL_ID
#define IOC_MASK_PHYSICAL_ID_COMPONENT	IOC_MASK_PHYSICAL_ID
/*---------------------------------------------*/
#define IOC_GET_LOGICAL_ID_MODULE(iocmd)		( (IOC_T)( ((IOC_T)(iocmd)) & IOC_MASK_LOGICAL_ID_MODULE    ) )
#define IOC_GET_LOGICAL_ID_DEVICE(iocmd)		( (IOC_T)( ((IOC_T)(iocmd)) & IOC_MASK_LOGICAL_ID_DEVICE    ) )
#define IOC_GET_LOGICAL_ID_COMPONENT(iocmd)	( (IOC_T)( ((IOC_T)(iocmd)) & IOC_MASK_LOGICAL_ID_COMPONENT ) )
/*---------------------------------------------*/
#define IOC_GET_PHYSICAL_ID(iocmd)					( (IOC_T)( ((IOC_T)(iocmd)) & IOC_MASK_PHYSICAL_ID ) )
#define IOC_GET_PHYSICAL_ID_MODULE(iocmd)			IOC_GET_PHYSICAL_ID(iocmd)
#define IOC_GET_PHYSICAL_ID_DEVICE(iocmd)			IOC_GET_PHYSICAL_ID(iocmd)
#define IOC_GET_PHYSICAL_ID_COMPONENT(iocmd)		IOC_GET_PHYSICAL_ID(iocmd)	
/*---------------------------------------------*/
#define IOC_GET_BASE_ID_MD(iocmd)																					\
			(		( IOC_GET_BASE_ID(iocmd) << IOC_BITS_INSTANCE )  | ( IOC_GET_INSTANCE(iocmd) )	)

#define IOC_GET_BASE_ID_MODULE(iocmd)		IOC_SUB_OFFSET_MODULE( IOC_GET_BASE_ID_MD(iocmd) )
#define IOC_GET_BASE_ID_DEVICE(iocmd)		IOC_SUB_OFFSET_DEVICE( IOC_GET_BASE_ID_MD(iocmd) )
#define IOC_GET_BASE_ID_COMPONENT(iocmd)	IOC_GET_BASE_ID(iocmd)	
/*---------------------------------------------*/
#define IOC_VALID_LOGICAL_ID_MODULE(logical_id)     ( !!!( (logical_id) & ( IOC_MASK_OFFSET_MODULE    << IOC_SHIFT_BASE_ID ) )  )
#define IOC_VALID_LOGICAL_ID_COMPONENT(logical_id)  (  !!( (logical_id) & ( IOC_MASK_OFFSET_COMPONENT << IOC_SHIFT_BASE_ID ) )  )
#define IOC_VALID_LOGICAL_ID_DEVICE(logical_id)     ( !!!( IOC_VALID_LOGICAL_ID_MODULE(logical_id) | IOC_VALID_LOGICAL_ID_COMPONENT(logical_id) ) )
/*---------------------------------------------*/
#define IOC_INVALID_LOGICAL_ID_MODULE(logical_id)     ( !IOC_VALID_LOGICAL_ID_MODULE(logical_id)    )
#define IOC_INVALID_LOGICAL_ID_COMPONENT(logical_id)  ( !IOC_VALID_LOGICAL_ID_COMPONENT(logical_id) )
#define IOC_INVALID_LOGICAL_ID_DEVICE(logical_id)		( !IOC_VALID_LOGICAL_ID_DEVICE(logical_id)    )
/*---------------------------------------------*/

/* NOTE, FUTURE:

	Possible remapping scheme to increase density.
	REMAPPING:
	device base id:	if 17 then map to 1
							if 231 then map to 2
	get base id:		if 1 then map to 17
							if 2 then map to 231

	#define IOC_MAP_DEVICE_TO_ID(x) \
	    ( (x) == DEVICE_TRACERE ? (1) \
		 : (x) == DEVICE_STX104  ? (2) \
		 : 0                           )
	#define IOC_MAP_DEVICE_FROM_ID(x) \
	    ( (x) == (1) ? (DEVICE_TRACERE) \
		 : (x) == (2) ? (DEVICE_STX104)  \
		 : 0                             )

	To build: logical_id = bit_shift_in( map_in( base_id ) )
	To Get:   base_id    = map_out( bit_shift_out( logical_id ) )

DONE: 1. need a way to test the three above to make sure the base_id is within proper boundaries.
See AUL_Ioctl() in aul.c.

INSTANCE - only used by components.  Does not make sense to have more instances of a device, you could but they 
are treated differently anyway.  There is typically only one instance of a module as these are typcial support
objects and only one is required.

INDEX - used by module, device and component typically as a channel index.

TYPE  - used to determine validity of the command and is preserved in order to match up with Linux IOCTL command filtering requirements.

COMMAND - used by module, device and component.
*/


/*******************************************************************/
/* 
	Description
	This magic number (ioc type) is accepted by library (application
	layer) or driver (kernel layer).  Thus, if a command is sent to 
	the application, it is attempted to be processed their first and 
	if not it is then passed onto the driver.  

	Why do this?  This is a way for the code to be tested at the 
	application layer as well as the kernel level.  It allows the 
	command to be passed into the library and propagate to the driver
	as required.

	NOTE
	Magic number is the same as "type" (i.e. IOC_BITS_TYPE).
*/
#define IOC_MASK_MAGIC_DRV		( (IOC_T)( (   0x01             & IOC_MASK_TYPE ) << IOC_SHIFT_TYPE ) )
#define IOC_MAGIC_DRV_LIB		( (IOC_T)( (   IOC_MAGIC_NUMBER & IOC_MASK_TYPE ) << IOC_SHIFT_TYPE ) )
#define IOC_MAGIC_DRV			( (IOC_T)( IOC_MAGIC_DRV_LIB | IOC_MASK_MAGIC_DRV ) ) 

#if(0)
#if ( IOC_MAGIC_DRV_LIB == IOC_MAGIC_DRV )
# error "IOC_MAGIC_NUMBER is odd and ought to be even"
#endif
#endif

#define IOC_GET_MAGIC(cmd)		( (size_t)( ((IOC_T)cmd) & ( IOC_MASK_TYPE << IOC_SHIFT_TYPE ) ) ) 
#define IOC_MAGIC_IS_VALID(cmd)			!!( ( IOC_GET_MAGIC(cmd) & ~IOC_MASK_MAGIC_DRV ) == IOC_MAGIC_DRV_LIB ) 
#define IOC_MAGIC_IS_VALID_LIB(cmd)		!!( IOC_GET_MAGIC(cmd) == IOC_MAGIC_DRV_LIB )

#define IOC_SET_DRV(cmd) ( (IOC_T)( ((IOC_T)cmd) | IOC_MASK_MAGIC_DRV   ) )

/*******************************************************************/
/* Recognized hardware driver commands.		     					*/

#define IOC_CREATE_CMD(cmd,physical_id)															\
	(																							\
	  ( physical_id	/* includes base_id and instance */									)	|	\
	  ( IOC_MAGIC_DRV_LIB	/* this is type already shifted into place */			)	|	\
	  ( ( ((IOC_T)cmd)			& IOC_MASK_COMMAND		) << IOC_SHIFT_COMMAND		)  )


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/********************************************************************************/
typedef enum
{
	CMD_MOD_NOP					= 0,
	CMD_MOD_ERROR_LAST			= 1,
	CMD_MOD_INITIALIZE			= 2,
	CMD_MOD_TERMINATE			= 3,
	CMD_MOD_VERSION				= 4,
	CMD_MOD_ID					= 5,
	CMD_MOD_OPEN				= 6,
	CMD_MOD_CLOSE				= 7,
	CMD_MOD_CUSTOM_LAST			= 31 /*  */
} CMD_MOD_BASE_COMMON_ENUM; 

/********************************************************************************/

#define CMD_MOD_LAST				CMD_MOD_CUSTOM_LAST
#define CMD_COMMON_MOD_TOTAL_QTY	( CMD_MOD_LAST + 1 ) 

/********************************************************************************/

#define CMD_MOD_BASE  0

#define CMD_MOD(lid,number) IOC_CREATE_CMD( (CMD_MOD_BASE + (number)), lid )

#define CMD_MOD_EXTRACT_ENUM(lid,fnc,symbol,number) symbol = CMD_MOD(lid,number), 

#define CMD_MOD_EXTRACT_JUMP_TABLE(lid,fnc,name,number) fnc,

#define CMD_MOD_EXTRACT_BASE_ENUM(lid,fnc,symbol,number) symbol ## _BASE = number, 

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
#define CMD_DATASET_EXTRACTOR(lid,fnc,symbol,number) { CMD_MOD(lid,number), #symbol }, 
#else
#define CMD_DATASET_EXTRACTOR(lid,fnc,symbol,number)
#endif


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/


/********************************************************************************/
typedef enum
{
	CMD_DEV_NOP				=  0,
	CMD_DEV_INITIALIZE		=  1,
	CMD_DEV_TERMINATE		=  2,
	CMD_DEV_POWER_UP		=  3,
	CMD_DEV_POWER_DOWN		=  4,
	CMD_DEV_OPEN			=  5,
	CMD_DEV_CLOSE			=  6,
	CMD_DEV_CONFIG_SET		=  7,
	CMD_DEV_CONFIG_GET		=  8,
	CMD_DEV_WRITE			=  9,
	CMD_DEV_READ			= 10,
	CMD_DEV_REG_WRITE		= 11,
	CMD_DEV_REG_READ		= 12,
	CMD_DEV_RESTART			= 13,
	CMD_DEV_DOUT_BIT_SET	= 14,
	CMD_DEV_DIN_BIT_GET		= 15,
	CMD_DEV_EVENT_ENABLE	= 16,
	CMD_DEV_EVENT_DISABLE	= 17,
/*
	CMD_DEV_CONNECTIVITY	= 14,
	CMD_DEV_CLI				= 15,
*/
	CMD_DEV_CUSTOM_LAST		= 31 /* we can do this because components will use */
} CMD_DEV_BASE_COMMON_ENUM; 

/********************************************************************************/

#define CMD_DEV_LAST					CMD_DEV_CUSTOM_LAST
#define CMD_COMMON_DEV_TOTAL_QTY		( CMD_DEV_LAST + 1 ) 

/********************************************************************************/
#define CMD_DEV_BASE   0

#define CMD_DEV(lid,number) IOC_CREATE_CMD( (CMD_DEV_BASE + (number)), lid )

#define CMD_DEV_EXTRACT_ENUM(lid,fnc,symbol,number) symbol = CMD_DEV(lid,number), 

#define CMD_DEV_EXTRACT_JUMP_TABLE(lid,fnc,name,number) fnc,

#define CMD_DEV_EXTRACT_BASE_ENUM(lid,fnc,symbol,number) symbol ## _BASE = number, 


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/******************************************************************

TODO TODO TODO  ??????????????
NONE OF THIS REALLY MATTERS ANY MORE?????????????????????

Description
Component level commands that come from device commands.  These are the 
default locations in the component jump table.  These commands are 
just above the device commands (starts at SYS_CMD_TOTAL_QTY).

These commands are dedicated and if a component has a jump table it must
have these commands, use NULLs where not applicable.

There is no other way to do this that is practical.  We could use a second
table to remap to the component table, with each component, but since this
driver will reside in 32-bit world that would actually cost more!  For an
8-bit or 16-bit it might save some space, but almost a toss up. 

The only other way is to have two jump tables, where one is for dedicated 
commands below and a second one for any commands above these.

*/
typedef enum
{
	CMD_CMP_RESTART			= 0, /**< restart the component							*/
	CMD_CMP_UPDATE			= 1, /**< update the component							*/
	CMD_CMP_POWER_UP		= 2, /**< power up the component 						*/
	CMD_CMP_POWER_DOWN		= 3, /**< power down the component 						*/
	CMD_CMP_CONFIG_SET		= 4, /**< set configuration information					*/
	CMD_CMP_CONFIG_GET		= 5, /**< retrieve configuration information			*/
	CMD_CMP_READ			= 6, /**< data read  									*/
	CMD_CMP_WRITE			= 7, /**< data write 									*/
	CMD_CMP_PRESENT			= 8  /**< Probe the component to verify its presence 	*/
} CMD_CMP_BASE_COMMON_ENUM; 
/********************************************************************************/

#define CMD_CMP_LAST					( CMD_CMP_PRESENT + 0 )
#define CMD_COMMON_CMP_TOTAL_QTY		( CMD_CMP_LAST    + 1 )

/********************************************************************************/
#define CMD_CMP_BASE   0

#define CMD_CMP(lid,number) IOC_CREATE_CMD( (CMD_CMP_BASE + (number)), lid )

#define CMD_CMP_EXTRACT_ENUM(lid,fnc,symbol,number) symbol = CMD_CMP(lid,number), 

#define CMD_CMP_EXTRACT_JUMP_TABLE(lid,fnc,name,number) fnc,

#define CMD_CMP_EXTRACT_BASE_ENUM(lid,fnc,symbol,number) symbol ## _BASE = number, 

/********************************************************************************/
#endif /* CMD_MACROS_H_ */
