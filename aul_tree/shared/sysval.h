#ifndef  __SYSVAL_H__
#define  __SYSVAL_H__
/********************************************************************************
  _____________________________________________________________________________
  Description:

  _____________________________________________________________________________
  Remarks:
    _____________________________________________________________________________
  Example:
    _____________________________________________________________________________
  Copyright Notice:
    COPYRIGHT NOTICE Copyright (c) 2010 by Apex Embedded
    Systems. All rights reserved. This document is the
    confidential property of Apex Embedded Systems Any
    reproduction or dissemination is prohibited unless
    specifically authorized in writing.
  _____________________________________________________________________________
  TODO:

  ********************************************************************************/

/*******************************************************************/
#define SYSVAL_H__REV  "$Date: 2015-04-17 19:12:24 -0500 (Fri, 17 Apr 2015) $"
/*******************************************************************/


#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/ioctl.h"  /* linux version -- apex annotated. */
#include "../shared/cmd_macros.h"




/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*
http://stackoverflow.com/questions/3957252/is-there-any-way-to-compute-the-width-of-an-integer-type-at-compile-time
Number of bits in inttype_MAX, or in any (1<<k)-1 where 0 <= k < 2040 */
#define IOC_IMAX_BITS(m) ( (m)/((m)%255+1) / 255%255*8 + 7-86/((m)%255+12) )


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


/*******************************************************************/
/* commands are now unique to the SYStem or system wide 
   
   These commands are shared among all drivers so that the
   SYStem level functions can pass to the underlying portion
   of the driver the same command to each driver to perform
   same tasks.  For example, the SYS_CMD_OPEN is passed 
   to the underlying driver function to perform an open operation
   and is the same for all drivers.

   SYStem provides the top end interface between application and
   driver in much the same way Linux does this, however, our
   implementation is much simpler, but parallels many of the Linux
   behavior where possible.
*/
#define SYS_CMD_NULL 0
#define SYS_IOC_TYPE  IOC_MASK_TYPE
#define SYS_IOC_XFR  9
typedef enum
{
	SYS_CMD_NOP				= IOC_CREATE_CMD(0,			IOC_PHYSICAL_ID_NULL),
	SYS_CMD_INITIALIZE		= IOC_CREATE_CMD(1,			IOC_PHYSICAL_ID_NULL),
	SYS_CMD_TERMINATE		= IOC_CREATE_CMD(2,			IOC_PHYSICAL_ID_NULL),
	SYS_CMD_POWER_UP		= IOC_CREATE_CMD(3,			IOC_PHYSICAL_ID_NULL),
	SYS_CMD_POWER_DOWN		= IOC_CREATE_CMD(4,			IOC_PHYSICAL_ID_NULL),
	SYS_CMD_OPEN			= IOC_CREATE_CMD(5,			IOC_PHYSICAL_ID_NULL),
	SYS_CMD_CLOSE			= IOC_CREATE_CMD(6,			IOC_PHYSICAL_ID_NULL),
	SYS_CMD_WRITE			= IOC_CREATE_CMD(7,			IOC_PHYSICAL_ID_NULL),
	SYS_CMD_READ			= IOC_CREATE_CMD(8,			IOC_PHYSICAL_ID_NULL),
	SYS_CMD_XFR				= IOC_CREATE_CMD(SYS_IOC_XFR,IOC_PHYSICAL_ID_NULL),
	SYS_CMD_CLI				= IOC_CREATE_CMD(10,		IOC_PHYSICAL_ID_NULL),
	SYS_CMD_RESTART			= IOC_CREATE_CMD(11,		IOC_PHYSICAL_ID_NULL),
	SYS_CMD_EVENT_ENABLE	= IOC_CREATE_CMD(12,		IOC_PHYSICAL_ID_NULL)
} SYS_CMD_ENUM;

#define SYS_CMD_LAST			12
#define SYS_CMD_TOTAL_QTY		( SYS_CMD_LAST + 1 ) 

/*******************************************************************/
/*******************************************************************/

/******************************************************************
*/
/******************************************************************
  DESCRIPTION
  This section defines complex data types used within the driver.
  The data types supported also include arrays.  Array functions
  are augmented with the array.c file.
  bit field		bit width		Description
  [10: 0]			11		Element size
  [18:11]			 8		Element quantity
  [28:19]			10		Type
  [30:29]			 2		Position
  [31:31]			 1		User bit				

  Global.h handles the simple data types.
******************************************************************/
/******************************************************************
Description
*/
typedef uint32_t SYS_TYPE;
/******************************************************************
Description
*/
typedef enum
{
	SYS_TYPE__NONE							=   0,

	SYS_TYPE__INT8							=   1,
	SYS_TYPE__INT16							=   2,
	SYS_TYPE__INT32							=   4,
	SYS_TYPE__INT64							=   8,
	SYS_TYPE__FLOAT							=  20,
	SYS_TYPE__DOUBLE						=  24,

	SYS_TYPE__IOKERN_TYPE					=  32,
	SYS_TYPE__IOKERN_REGION_TYPE			=  33,
	SYS_TYPE__IOKERN_IRQ_TYPE				=  34,
	SYS_TYPE__IOKERN_TASK_TYPE				=  35,
	SYS_TYPE__devmgr_device_type			=  36,
	SYS_TYPE__SYS_DRIVER_TYPE				=  37,
	SYS_TYPE__SYS_DESCRIPTOR_TYPE			=  38,
	SYS_TYPE__REGISTER_TYPE					=  39,
	SYS_TYPE__REGISTER_SET_TYPE				=  40,
	SYS_TYPE__SYS_XFR_TYPE					=  41,
	SYS_TYPE__FIFO_TYPE						=  42,
	SYS_TYPE__component_type				=  43,
	SYS_TYPE__FILTER_TYPE					=  44,
	SYS_TYPE__SYS_TIMER_PERIODIC_TASK_TYPE	=  45,

	SYS_TYPE__SYS_COMPONENT_CALLBACK_TYPE   =  46,
	SYS_TYPE__SYS_SERVICE_CFG_DO_TYPE		=  47,
	SYS_TYPE__SYS_SERVICE_DATA_SET_LED_TYPE =  48,
	SYS_TYPE__device_definition_type		=  49,
	SYS_TYPE__module_definition_type		=  50,
	SYS_TYPE__component_definition_type		=  51,
	SYS_TYPE__io_target_type				=  52,
	SYS_TYPE__device_available_type			=  53,
	SYS_TYPE__device_list_type				=  54,

	SYS_TYPE__CMP_HW_SCAN					=  64,
	SYS_TYPE__CMP_READ_WRITE_TYPE			=  65,
	SYS_TYPE__CMP_LPT_DATA_SET				=  66,
	SYS_TYPE__i8255m0_dataset_type			=  67,
	SYS_TYPE__CMP_TEGC_DATA_SET				=  68,
	SYS_TYPE__CMP_SER_006_DATA_SET			=  69,
	SYS_TYPE__CMP_CDB_010_DATA_SET			=  70,
	SYS_TYPE__CMP_TSC_011_DATA_SET			=  71,
	SYS_TYPE__CMP_WDT_DATA_SET				=  72,
	SYS_TYPE__i8254_dataset_type			=  73,
	SYS_TYPE__bank_cfg_type					=  74,

	SYS_TYPE__LPT_CFG_TYPE					=   80,
	SYS_TYPE__i8255m0_cfg_type				=   81,
	SYS_TYPE__TEGC_CFG_TYPE					=   82,
	SYS_TYPE__SER_CFG_TYPE					=   83,
	SYS_TYPE__CDB_CFG_TYPE					=   84,
	SYS_TYPE__TSC_CFG_TYPE					=   85,
	SYS_TYPE__WDT_CFG_TYPE					=   86,
	SYS_TYPE__i8254_cfg_type				=   87,
/*	SYS_TYPE__i8254_ct_type					=  88, */
	SYS_TYPE__i8254_ct_xfr_array_type		=   88,
	SYS_TYPE__i8254_ct_xfr_type				=   89,
	
	SYS_TYPE__tracere_cfg_type				=   90,
	SYS_TYPE__tracere_dio_cfg_type			=   91,
	SYS_TYPE__tracere_intr_cfg_type			=   92,
	SYS_TYPE__tracere_ct_cfg_type			=   93,
	SYS_TYPE__tracere_dataset_type			=   94,
	SYS_TYPE__stx104_dataset_type			=   95,
	SYS_TYPE__stx104_ai_cfg_type			=   96,
	SYS_TYPE__stx104_ct_cfg_type			=	97,

	SYS_TYPE__SYS_DEVICE_TYPE__DEV_TST		=  112,
	SYS_TYPE__SYS_DEVICE_TYPE__DEV_LPT		=  113,
	SYS_TYPE__SYS_DEVICE_TYPE__DEV_TRACERE	=  114,
	SYS_TYPE__SYS_DEVICE_TYPE__DEV_YKTS		=  115,
	SYS_TYPE__SYS_DEVICE_TYPE__DEV_YKIO		=  116,
	SYS_TYPE__SYS_DEVICE_TYPE__DEV_WDT		=  117,

	SYS_TYPE__SYS_DRIVER_TYPE__DRV_TST		=  128,
	SYS_TYPE__SYS_DRIVER_TYPE__DRV_LPT		=  129,
	SYS_TYPE__SYS_DRIVER_TYPE__DRV_TRACERE	=  130,
	SYS_TYPE__SYS_DRIVER_TYPE__DRV_YBASE	=  131,
	SYS_TYPE__SYS_DRIVER_TYPE__DRV_WDT		=  132,

	SYS_TYPE__indirect_dataset_type			=  133,
	SYS_TYPE__indirect_cfg_type				=  134,
/* <EGS_BEGIN>  */
	SYS_TYPE__egs_dataset_type				=  135,
	SYS_TYPE__egs_cfg_type					=  136,
/* <EGS_END> */

/* application layer stuff */
	SYS_TYPE__CDB_CAL_TYPE					=  252,
	SYS_TYPE__CDB_CAL_COLLECT_TYPE			=  253,

	SYS_TYPE__TEST_TYPE						=  254,
	SYS_TYPE__UNUSED						=  255
} SYS_BASE_TYPE;

/******************************************************************
Alias SYS_BASE_TYPEs
*/
#define SYS_TYPE__IO_CFG_TYPE     SYS_TYPE__CDB_CFG_TYPE
#define SYS_TYPE__YBASE_CFG_TYPE  SYS_TYPE__CDB_CFG_TYPE

/******************************************************************
Description
*/

/* NOTE: element quantity ought to include any null elements */
/* grey encoding */
typedef enum 
{
	SYS_TYPE_POSITION_ELEMENT_SINGLE	= 0,
	SYS_TYPE_POSITION_ELEMENT_FIRST		= 1,
	SYS_TYPE_POSITION_ELEMENT_LAST		= 2,
	SYS_TYPE_POSITION_ELEMENT_MIDDLE	= 3
} SYS_TYPE_POSITION_ENUM;

typedef enum
{
	SYS_TYPE_DEALLOCATE	= 0,
	SYS_TYPE_ALLOCATE	= 1
} SYS_TYPE_USER_FLAG_ENUM;

/*
TODO: need to test each parameter assigned and would be great to resize the values below.

NOTE:
1. The element size had to be increased from 11 to 14 to accommodate 
   the large CDB_CFG_TYPE due mostly to the CDB_010_CAL_TYPE data structure!

*/


/* bit width of each parameter */
#define SYS_TYPE_BITS_USER_FLAG				 1
#define SYS_TYPE_BITS_BASE_TYPE			    10				/* 1023 types max			*/
#define SYS_TYPE_BITS_ELEMENT_SIZE			14				/* 16383 bytes max			*/
#define SYS_TYPE_BITS_ELEMENT_QTY			 5				/*   32 elements maximum	*/
#define SYS_TYPE_BITS_POSITION				 2				/* null, last, other, first */
#define SYS_TYPE_BITS_TOTAL					( SYS_TYPE_BITS_USER_FLAG		+ \
											  SYS_TYPE_BITS_POSITION		+ \
											  SYS_TYPE_BITS_BASE_TYPE		+ \
											  SYS_TYPE_BITS_ELEMENT_QTY		+ \
											  SYS_TYPE_BITS_ELEMENT_SIZE	)
/* mask of each parameter */
#define SYS_TYPE_MASK_USER_FLAG			((SYS_TYPE)( (1 << SYS_TYPE_BITS_USER_FLAG		) -1 ))
#define SYS_TYPE_MASK_POSITION			((SYS_TYPE)( (1 << SYS_TYPE_BITS_POSITION		) -1 ))
#define SYS_TYPE_MASK_BASE_TYPE			((SYS_TYPE)( (1 << SYS_TYPE_BITS_BASE_TYPE		) -1 ))
#define SYS_TYPE_MASK_ELEMENT_QTY		((SYS_TYPE)( (1 << SYS_TYPE_BITS_ELEMENT_QTY	) -1 ))
#define SYS_TYPE_MASK_ELEMENT_SIZE		((SYS_TYPE)( (1 << SYS_TYPE_BITS_ELEMENT_SIZE	) -1 ))

/* bit shift for each paremeter */
#define SYS_TYPE_SHIFT_ELEMENT_SIZE		0	
#define SYS_TYPE_SHIFT_ELEMENT_QTY		( SYS_TYPE_SHIFT_ELEMENT_SIZE	+ SYS_TYPE_BITS_ELEMENT_SIZE	)
#define SYS_TYPE_SHIFT_BASE_TYPE		( SYS_TYPE_SHIFT_ELEMENT_QTY	+ SYS_TYPE_BITS_ELEMENT_QTY		)
#define SYS_TYPE_SHIFT_POSITION			( SYS_TYPE_SHIFT_BASE_TYPE		+ SYS_TYPE_BITS_BASE_TYPE		)
#define SYS_TYPE_SHIFT_USER_FLAG		( SYS_TYPE_SHIFT_POSITION		+ SYS_TYPE_BITS_POSITION		)

/* decode/get for each parameter */
#define TYPE_GET_ELEMENT_SIZE(x)	(size_t)( ( (x) >> SYS_TYPE_SHIFT_ELEMENT_SIZE	)	& SYS_TYPE_MASK_ELEMENT_SIZE	)		
#define TYPE_GET_ELEMENT_QTY(x)		(size_t)( ( (x) >> SYS_TYPE_SHIFT_ELEMENT_QTY	)	& SYS_TYPE_MASK_ELEMENT_QTY		)	
#define TYPE_GET_BASE_TYPE(x)		(SYS_BASE_TYPE) ( ( (x) >> SYS_TYPE_SHIFT_BASE_TYPE		)	& SYS_TYPE_MASK_BASE_TYPE		)
#define TYPE_GET_POSITION(x)		(size_t)( ( (x) >> SYS_TYPE_SHIFT_POSITION		)	& SYS_TYPE_MASK_POSITION		)
#define TYPE_GET_USER_FLAG(x)		(size_t)( ( (x) >> SYS_TYPE_SHIFT_USER_FLAG		)	& SYS_TYPE_MASK_USER_FLAG		)

/* test position */
#define SYS_TYPE_MASK_IN_PLACE_POSITION  ((SYS_TYPE)( SYS_TYPE_MASK_POSITION  << SYS_TYPE_SHIFT_POSITION  ))
#define SYS_TYPE_MASK_IN_PLACE_USER_FLAG ((SYS_TYPE)( SYS_TYPE_MASK_USER_FLAG << SYS_TYPE_SHIFT_USER_FLAG ))
#define SYS_TYPE_IS_ELEMENT_FIRST(x)	 ( ( (x) & SYS_TYPE_MASK_IN_PLACE_POSITION  ) == \
											( SYS_TYPE_MASK_IN_PLACE_POSITION & ( ((SYS_TYPE)SYS_TYPE_POSITION_ELEMENT_FIRST) << SYS_TYPE_SHIFT_POSITION ) ) )
#define SYS_TYPE_IS_ELEMENT_LAST(x)		 ( ( (x) & SYS_TYPE_MASK_IN_PLACE_POSITION  ) == \
											( SYS_TYPE_MASK_IN_PLACE_POSITION & ( ((SYS_TYPE)SYS_TYPE_POSITION_ELEMENT_LAST) << SYS_TYPE_SHIFT_POSITION ) ) )
#define SYS_TYPE_IS_ELEMENT_SINGLE(x)	 ( ( (x) & SYS_TYPE_MASK_IN_PLACE_POSITION  ) == \
											( SYS_TYPE_MASK_IN_PLACE_POSITION & ( ((SYS_TYPE)SYS_TYPE_POSITION_ELEMENT_SINGLE) << SYS_TYPE_SHIFT_POSITION ) ) )
#define SYS_TYPE_IS_USER_FLAG(x)		 ( ( (x) & SYS_TYPE_MASK_IN_PLACE_USER_FLAG ) == SYS_TYPE_MASK_IN_PLACE_USER_FLAG )

/* encode/set for each parameter */
#define SYS_TYPE_SET_USER_FLAG(var,flag)		( ( ( ((SYS_TYPE)flag)  & SYS_TYPE_MASK_USER_FLAG )	<< SYS_TYPE_SHIFT_USER_FLAG )		|  \
													( ((SYS_TYPE)var)   & ~( SYS_TYPE_MASK_USER_FLAG	<< SYS_TYPE_SHIFT_USER_FLAG )		)  )
#define SYS_TYPE_SET_POSITION(var,pos)			( ( ( ((SYS_TYPE)pos)   & SYS_TYPE_MASK_POSITION )	<< SYS_TYPE_SHIFT_POSITION )		|  \
													( ((SYS_TYPE)var)	& ~( SYS_TYPE_MASK_POSITION	<< SYS_TYPE_SHIFT_POSITION )		)  )
#define SYS_TYPE_SET_BASE_TYPE(var,base_type)	( ( ( ((SYS_TYPE)base_type) & SYS_TYPE_MASK_BASE_TYPE )<< SYS_TYPE_SHIFT_BASE_TYPE )		|  \
													( ((SYS_TYPE)var)   & ~( SYS_TYPE_MASK_BASE_TYPE	<< SYS_TYPE_SHIFT_BASE_TYPE )		)  )
#define SYS_TYPE_SET_ELEMENT_QTY(var,qty)		( ( ( ((SYS_TYPE)qty)   & SYS_TYPE_MASK_ELEMENT_QTY )	<< SYS_TYPE_SHIFT_ELEMENT_QTY )		|  \
													( ((SYS_TYPE)var)   & ~( SYS_TYPE_MASK_ELEMENT_QTY<< SYS_TYPE_SHIFT_ELEMENT_QTY )		)  )
#define SYS_TYPE_SET_ELEMENT_SIZE(var,es)		( ( ( ((SYS_TYPE)es)    & SYS_TYPE_MASK_ELEMENT_SIZE )<< SYS_TYPE_SHIFT_ELEMENT_SIZE )	|  \
													( ((SYS_TYPE)var)   & ~( SYS_TYPE_MASK_ELEMENT_SIZE << SYS_TYPE_SHIFT_ELEMENT_SIZE )	)  )

/*************************************************
 Define a complete data type
**************************************************/
#define SYS_TYPE_CREATE(base_type,user,position,element_qty,element_size)									\
	(																										\
	  ( ( ((SYS_TYPE) user)			& SYS_TYPE_MASK_USER_FLAG		) << SYS_TYPE_SHIFT_USER_FLAG		) |	\
	  ( ( ((SYS_TYPE) position)		& SYS_TYPE_MASK_POSITION		) << SYS_TYPE_SHIFT_POSITION		) |	\
	  ( ( ((SYS_TYPE) base_type)	& SYS_TYPE_MASK_BASE_TYPE		) << SYS_TYPE_SHIFT_BASE_TYPE		) |	\
	  ( ( ((SYS_TYPE) element_qty)	& SYS_TYPE_MASK_ELEMENT_QTY		) << SYS_TYPE_SHIFT_ELEMENT_QTY		) |	\
	  ( ( ((SYS_TYPE) element_size)	& SYS_TYPE_MASK_ELEMENT_SIZE	) << SYS_TYPE_SHIFT_ELEMENT_SIZE	) )

/* Support for NULL element at the end of an array.  */
#define SYS_TYPE_NULL			 SYS_TYPE_SET_BASE_TYPE(0,SYS_TYPE__NONE)

/******************************************************************
*/
#define SYS_TYPE_OBJ(base,obj)		SYS_TYPE__##base ##__ ##obj
#define SYS_TYPE_OF(base)			SYS_TYPE__##base

#define SYS_TYPE_MAKE(type) SYS_TYPE_CREATE(SYS_TYPE_OF(type),0,0,0,sizeof(type))
#define SYS_TYPE_OBJECT(base,obj) SYS_TYPE_CREATE(SYS_TYPE_OBJ(base,obj),0,0,0,sizeof(base))


/*******************************************************************/
/*******************************************************************/


/********************************************************/
enum { RESTART_COLD = 0, RESTART_WARM = 1 };


#if(0)
/******************************************************************
*/
/******************************************************************
*/
/********************************************************/
#define SYS_DRIVER_QTY			8
#define SYS_DEVICE_QTY			4
#define SYS_DESCRIPTOR_QTY      (SYS_DRIVER_QTY * (SYS_DEVICE_QTY + 1))

/* TODO:
#define SYS_IO_RESOURCE_QTY		SYS_DESCRIPTOR_QTY */


#define SYS_DRIVER_ID_NONE		-1
#define SYS_DRIVER_TYPE_NONE	0
#define SYS_DRIVER_NAME_LENGTH	16
#define SYS_DEVICE_NAME_LENGTH  SYS_DRIVER_NAME_LENGTH
/* selects all devices within the driver or just the driver */
#define SYS_DEVICE_ALL_SELECT   IOC_DEVICE_ALL_SELECT

/*******************************************************
TODO:
1. Move to error handler
2. Create an X-Macro to include IDs, code, and human readable
*/
typedef enum
{
	SYS_EC_OK									=  0,
	SYS_EC_INITIALIZE_DRIVER_FAILED				=  1,
	SYS_EC_INITIALIZE_DRIVERS_NONE				=  2,
	SYS_EC_DRIVER_ID_OUT_OF_RANGE				=  3,
	SYS_EC_NAME_NOT_FOUND						=  4,
	SYS_EC_DESCRIPTOR_BAD						=  5,
	SYS_EC_CANNOT_ALLOCATE_DRIVER				=  6,
	SYS_EC_CANNOT_ALLOCATE_DESCRIPTOR			=  7,
	SYS_EC_CANNOT_FIND_DESCRIPTOR				=  8,
	SYS_EC_COMMAND_INVALID						=  9,

	SYS_EC_TIMER_PERIODIC_INTERVAL_OUT_OF_RANGE = 10,
	SYS_EC_TIMER_PERIODIC_OUT_OF_RANGE			= 11,
	SYS_EC_TIMER_PERIODIC_CANNOT_ALLOCATE_TASK  = 12,
	SYS_EC_TIMER_PERIODIC_RANGE_ERROR			= 13,
	SYS_EC_TIMER_PERIODIC_TIMER_CONFIG_ERROR    = 14,

	SYS_EC_EBUSY								= 16,  /* corresponds to Linux EBUSY  */
	SYS_EC_EINVAL								= 22,  /* corresponds to Linux EINVAL */

	SYS_EC_SERVICE_INVALID						= 32,
	SYS_EC_SERVICE_ATTACHMENT_IN_USE			= 33,

	SYS_EC_CLARGS_REQUEST_NOT_FOUND				= 34,
	SYS_EC_CLARGS_REQUEST_CANNOT_ALLOCATE		= 35,

	SYS_EC_NULL_POINTER							= 38,
	SYS_EC_TYPE_INCORRECT						= 39,

	SYS_EC_ARRAY_ELEMENT_SIZE					= 48,
	SYS_EC_ARRAY_ELEMENT_QTY					= 49,
	SYS_EC_ARRAY_ELEMENT_TYPE					= 50,
	SYS_EC_ARRAY_NULL							= 51,
	SYS_EC_ARRAY_VALIDITY_ERROR					= 52,

	SYS_EC_CANNOT_ALLOCATE_IO_RESOURCE			= 56,
	SYS_EC_ALLOCATE_RESOURCE_ADR_SPACE			= 57,
	SYS_EC_ALLOCATE_RESOURCE_INTERRUPT			= 58,
	SYS_EC_DEVICE_OUT_OF_RANGE					= 59,
	SYS_EC_DEVICE_RANGE							= 60,

	SYS_EC_XFR_DEVICE_RANGE						= 64,
	SYS_EC_XFR_REG_SIZE_ERROR					= 65,
	SYS_EC_XFR_CMP_OUT_OF_RANGE					= 66,
	SYS_EC_XFR_REG_OUT_OF_RANGE					= 67,
	
	SYS_EC_CMP_CANNOT_ADD_TO_AVAILABLE_LISTING	= 72,
	SYS_EC_CMP_CANNOT_ADD_TO_INVENTORY			= 73,
	SYS_EC_CMP_NOT_IN_AVAILABLE_LISTING			= 74,
	SYS_EC_CMP_NOT_FOUND						= 75,
	SYS_EC_CMP_MANAGER_ERROR					= 76, 
	SYS_EC_CMP_BUF_NULL							= 77, /* at component level */
	SYS_EC_CMP_PRIVATE_DATA_NULL				= 78,
	SYS_EC_CMP_INDEX_OUT_OF_RANGE				= 79,
	SYS_EC_CMP_ILLEGAL_REQUEST					= 80,
	SYS_EC_CMP_TYPE_TYPES_DO_NOT_MATCH			= 81,
	SYS_EC_CMP_PARAMETER_OUT_OF_RANGE			= 82,
	SYS_EC_CMP_CANNOT_ESTABLISH_LINK			= 83,
	SYS_EC_CMP_DATA_SIZING_BUF_LEN_ERROR		= 84,
	SYS_EC_CMP_CANNOT_COMMUNICATE_WITH_HARDWARE = 85,
	SYS_EC_CMP_IO_NOT_ENABLED					= 86,
	SYS_EC_COMPONENT_INVENTORY_NULL				= 87,
	SYS_EC_CMP_DATA_LENGTH_ERROR				= 88,
	SYS_EC_CMP_PACKET_ERROR						= 89,
	
	SYS_EC_FIFO_TYPE_ERROR						= 92,
	SYS_EC_FIFO_BUFFER_TOO_SMALL				= 93,


	SYS_EC_CAL_CHANNEL_QTY_ZERO					= 123,
/* used by either application space or driver level  */
	SYS_EC_XDR_METHOD_UNKNOWN					= 124,
	SYS_EC_XDR_NOTHING_TO_DO					= 125,
	SYS_EC_XDR_WRITE							= 126,
	SYS_EC_XDR_READ								= 127,

/* used by application space and not used by driver level */
	SYS_EC_CANNOT_OPEN_FILE						= 128,
	SYS_EC_CANNOT_OPEN_FILE_DUE_TO_BLOCK		= 129,
	SYS_EC_CANNOT_OPEN_DEVICE					= 130,
	SYS_EC_INDEX_OUT_OF_RANGE					= 131,
	SYS_EC_DATA_OR_POINTER_INVALID				= 132,
	SYS_EC_ARRAY_TOO_SMALL						= 133,
	SYS_EC_MULTIPLER_ERROR						= 134,

	SYS_EC_CALIBRATION_CANDIDATE_ERROR			= 144,
	SYS_EC_CALIBRATION_MASTER_ERROR				= 145,
	SYS_EC_CALIBRATION_INVALID					= 146,
	SYS_EC_CALIBRATION_MASTER_FILE_NOT_FOUND	= 147
} SYS_EC_ENUM;


#endif





/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/


/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/


#endif /* __SYSVAL_H__ */
