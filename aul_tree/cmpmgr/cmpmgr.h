#ifndef MODULE_CMP_MGR
#define MODULE_CMP_MGR       11   /* module ID number */
#define MODULE_CMP_MGR_NAME  "cmp_mgr"
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
#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/target.h"
#include "../shared/sysval.h"
#include "../shared/cmd_macros.h"
#include "../reg/reg.h"


/*****************************************************************************/	
/*****************************************************************************/	
/*****************************************************************************/



/*******************************************************************/
/*******************************************************************/
/******************************************************************
Description
Used to indicate status of the component fixups at startup.

STS = STatuS
FXU = FiXedUp
*/
#define CMP_STS_OFFSET_FXU_ON    1
#define CMP_STS_OFFSET_FXU_OFF   0

#define CMP_STS_PRIVATE_DATA_FXU_ON    1
#define CMP_STS_PRIVATE_DATA_FXU_OFF   0

#define CMP_STS_ESTABLISH_LINK_ON    1
#define CMP_STS_ESTABLISH_LINK_OFF   0

#define CMP_STS_EVENT_PARAMETERS_ON  1
#define CMP_STS_EVENT_PARAMETERS_OFF 0

#define CMP_STS_EVENT_ENABLE_ON		 1
#define CMP_STS_EVENT_ENABLE_OFF	 0

/* bit width of each parameter */
#define CMP_STS_BITS_OFFSET_FXU			1
#define CMP_STS_BITS_PRIVATE_DATA_FXU	1
#define CMP_STS_BITS_ESTABLISH_LINK		1
#define CMP_STS_BITS_EVENT_PARAMETERS   1
#define CMP_STS_BITS_EVENT_ENABLE       1

/* mask of each parameter */
#define CMP_STS_MASK_OFFSET_FXU			( (1 << CMP_STS_BITS_OFFSET_FXU	        ) -1 ) 
#define CMP_STS_MASK_PRIVATE_DATA_FXU	( (1 << CMP_STS_BITS_PRIVATE_DATA_FXU	) -1 ) 
#define CMP_STS_MASK_ESTABLISH_LINK		( (1 << CMP_STS_BITS_ESTABLISH_LINK		) -1 ) 
#define CMP_STS_MASK_EVENT_PARAMETERS	( (1 << CMP_STS_BITS_EVENT_PARAMETERS	) -1 ) 
#define CMP_STS_MASK_EVENT_ENABLE		( (1 << CMP_STS_BITS_EVENT_ENABLE	) -1 )

/* bit shift for each parameter */
#define CMP_STS_SHIFT_OFFSET_FXU		0
#define CMP_STS_SHIFT_PRIVATE_DATA_FXU	( CMP_STS_SHIFT_OFFSET_FXU			+ CMP_STS_BITS_OFFSET_FXU		)
#define CMP_STS_SHIFT_ESTABLISH_LINK	( CMP_STS_SHIFT_PRIVATE_DATA_FXU	+ CMP_STS_BITS_PRIVATE_DATA_FXU	)
#define CMP_STS_SHIFT_EVENT_PARAMETERS	( CMP_STS_SHIFT_ESTABLISH_LINK    	+ CMP_STS_BITS_ESTABLISH_LINK	)
#define CMP_STS_SHIFT_EVENT_ENABLE   	( CMP_STS_SHIFT_EVENT_PARAMETERS    + CMP_STS_BITS_EVENT_PARAMETERS	)

/* decode/get for each parameter */
#define CMP_STS_GET_OFFSET_FXU(x)		( ( (x) >> CMP_STS_SHIFT_OFFSET_FXU			) & CMP_STS_MASK_OFFSET_FXU			)
#define CMP_STS_GET_PRIVATE_DATA_FXU(x)	( ( (x) >> CMP_STS_SHIFT_PRIVATE_DATA_FXU	) & CMP_STS_MASK_PRIVATE_DATA_FXU	)
#define CMP_STS_GET_ESTABLISH_LINK(x)	( ( (x) >> CMP_STS_SHIFT_ESTABLISH_LINK		) & CMP_STS_MASK_ESTABLISH_LINK		)
#define CMP_STS_GET_EVENT_PARAMETERS(x)	( ( (x) >> CMP_STS_SHIFT_EVENT_PARAMETERS	) & CMP_STS_MASK_EVENT_PARAMETERS	)
#define CMP_STS_GET_EVENT_ENABLE(x)		( ( (x) >> CMP_STS_SHIFT_EVENT_ENABLE		) & CMP_STS_MASK_EVENT_ENABLE		)

/* encode/set for each parameter */
#define CMP_STS_SET_OFFSET_FXU(var,os)			( ( ( (os) & CMP_STS_MASK_OFFSET_FXU )	<< CMP_STS_SHIFT_OFFSET_FXU	)	 |  \
												  ( (var) & ~( CMP_STS_MASK_OFFSET_FXU	<< CMP_STS_SHIFT_OFFSET_FXU	)	 )  )
#define CMP_STS_SET_PRIVATE_DATA_FXU(var,pd)	( ( ( (pd) & CMP_STS_MASK_PRIVATE_DATA_FXU )	<< CMP_STS_SHIFT_PRIVATE_DATA_FXU )	 |  \
												  ( (var) & ~( CMP_STS_MASK_PRIVATE_DATA_FXU	<< CMP_STS_SHIFT_PRIVATE_DATA_FXU )	 )  )
#define CMP_STS_SET_ESTABLISH_LINK(var,el)		( ( ( (el) & CMP_STS_MASK_ESTABLISH_LINK )	<< CMP_STS_SHIFT_ESTABLISH_LINK )	 |  \
												  ( (var) & ~( CMP_STS_MASK_ESTABLISH_LINK	<< CMP_STS_SHIFT_ESTABLISH_LINK )	 )  )
#define CMP_STS_SET_EVENT_PARAMETERS(var,prm)	( ( ( (prm) & CMP_STS_MASK_EVENT_PARAMETERS )	<< CMP_STS_SHIFT_EVENT_PARAMETERS )	 |  \
												  ( (var) & ~( CMP_STS_MASK_EVENT_PARAMETERS	<< CMP_STS_SHIFT_EVENT_PARAMETERS )	 )  )
#define CMP_STS_SET_EVENT_ENABLE(var,enable)	( ( ( (enable) & CMP_STS_MASK_EVENT_ENABLE )	<< CMP_STS_SHIFT_EVENT_ENABLE )	 |  \
												  ( (var) & ~( CMP_STS_MASK_EVENT_ENABLE		<< CMP_STS_SHIFT_EVENT_ENABLE )	 )  )

/******************************************************************
* TODO: Need a thorough definition
*/
#define CMP_STS_BUILD(offset,private_data,establish_link,event_params,event_enable)							\
	(																										\
		( ( (offset)			& CMP_STS_MASK_OFFSET_FXU		) << CMP_STS_SHIFT_OFFSET_FXU			) |	\
		( ( (private_data)	& CMP_STS_MASK_PRIVATE_DATA_FXU		) << CMP_STS_SHIFT_PRIVATE_DATA_FXU		) |	\
		( ( (establish_link)	& CMP_STS_MASK_ESTABLISH_LINK	) << CMP_STS_SHIFT_ESTABLISH_LINK		) |	\
		( ( (event_params)	& CMP_STS_MASK_EVENT_PARAMETERS		) << CMP_STS_SHIFT_EVENT_PARAMETERS		) |	\
		( ( (event_enable)	& CMP_STS_MASK_EVENT_ENABLE			) << CMP_STS_SHIFT_EVENT_ENABLE			) )

/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
struct component;  /* forward declarations */
typedef struct component component_type;
/*******************************************************************/
typedef int (* cmp_fnc )(  component_type *	component,
									size_t	index,
									char *	buf,
									size_t	buf_len
						);
/******************************************************************/
struct component_definition
{
	/*  Data structure information, includes:
		 o type magic number (type)
		 o size of this data structure (element_size)
	*/
	SYS_TYPE			type;
	REGISTER_TYPE *		register_table;
	cmp_fnc *			jump_table;
	size_t				jump_table_qty;
	const char *		name;
	const char *		svn_module_date;
	const char *		svn_command_date;
	const char *		svn_status_date;
};
typedef struct component_definition component_definition_type;
/*****************************************************************
Description
Used by driver to build an inventory of hardware to be assigned.
Used by device to predefine parameters for components, especially
for components that cannot be easily "discovered".
*/
struct component_inventory
{
	SYS_TYPE		type;
	IOC_T			logical_id;
	size_t			instance;
	size_t			offset;
	IOC_T			address_mode_physical_id;  /* if 0, then direct addressing, otherwise use to find matching component */
	void *			private_data;
};
typedef struct component_inventory component_inventory_type;
/******************************************************************/
struct component
{
	/* definition of the component...fixed or constant information */
	component_definition_type *	definition;

	/*  Component ID
		Includes both the logical ID and the instantiation, typically
		straight from hardware, or it might be predefined for some 
		components (i.e. i8255) that does not have an ID register.

		<ID: n-bits><instantiation: m-bits >
	*/
	IOC_T						physical_id;	
	/*  Location within the component inventory in which component to be used.
		If NULL, then revert to default read/write address space.

		If not null, then use the component specified at this location within
		the devices component inventory.  You will need to use the supplementary
		private_data to point to a data set to process.

		For direct addressing, location will be null.  For other addressing modes
		this will point to one of the components within the device list that is
		responsible for the particular addressing mode.
	*/
	struct component *	component_link;
	/* Private data that is specific to this component.  Typically allocated
	   at time of inventory/fixup.
		Must have specific structure: type followed by cfg, then anything else.
	*/
	void *				private_data;
	/*  The offset address of the component with respect to the device, or
	    the base_address of the device plus the components offset. 
	
		For manual instantiation, the offset is the offset from the devices
		base address plus the devices base address.

		For IAC automatic instantiation, the offset is set to that found within
		its address space (i.e. indirect address space).
	*/
	size_t				offset;
	/*  The IO descriptor pointing to the underlying io block that has been
		allocated to the device and all its components.
	*/
	int					iod;

	/*  Indicates that the component has been fixed up at initialization time. 
		   CMP_FIXUP_NULL             0x00
		   CMP_FIXUP_OFFSET_DATA_SET  0x01 
		   CMP_FIXUP_PRIVATE_DATA_SET 0x02
		This can also be used to indicate information regarding the private data type, size, etc.
	*/
	size_t				status;
};
/* typedef struct component component_type; */
/******************************************************************/
#define COMPONENT_LIST_NULL 								\
	{	/* definition   */		NULL,						\
		/* physical_id  */		IOC_PHYSICAL_ID_NULL,		\
		/* link_rw		 */		NULL,						\
		/* private_data */		0,							\
		/* offset       */		0,							\
		/* status		 */		CMP_STS_BUILD(0,0,0,0,0)	\
	}, 


/******************************************************************
Description
This supports special register read/write by way of a component.
Used by IAC component.
*/
typedef void (* cmp_rw_fnc )( component_type *	component,
							  size_t			reg_id,
							  char *			buf,
							  size_t			buf_len
							);
/*******************************************************************/
struct cmp_rw_cfg
{
	SYS_TYPE	type;
};
typedef struct cmp_rw_cfg cmp_rw_cfg_type;

/*******************************************************************/
/* This data structure serves as a short-cut for indirect addressing.
   In other words, we can skip a function call and call the read/write
   functions directly.
*/
struct cmp_link_rw
{
	SYS_TYPE			type;			/* SYS_TYPE__CMP_READ_WRITE_TYPE */
	cmp_rw_cfg_type		cfg;
	cmp_rw_fnc			reg_write;		/* register write function */
	cmp_rw_fnc			reg_read;		/* register read function  */
};
typedef struct cmp_link_rw cmp_link_rw_type; 

/*******************************************************************/

#define CMPMGR_EC_FAILURE -1


/******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************/

void CmpMgr_Reg_Write(  component_type * component,	/* the component whos register is to be written	*/
						size_t			  reg_id,		/* the specific register, i.e. register row		*/
						char *			  buf,
						size_t		  	  buf_len
							);

void CmpMgr_Reg_Read(  component_type * component,	/* the component whos register is to be read		*/
						  size_t				 reg_id,		/* the specific register, i.e. register row		*/
						  char *				 buf,
						  size_t				 buf_len
				     );

/******************************************************************/

int CmpMgr_Config_Set(	component_type *	component, 
						size_t				index,
						char *				buf,
						size_t				buf_len
				     );

int CmpMgr_Config_Get(  component_type *	component,  
						size_t				index,
						char *				buf,
						size_t				buf_len
					);

/******************************************************************/

void CmpMgr_Offset_Range(	component_type *	component,	/* component to test												*/
							size_t *				start,		/* returns the start address, virtual address in table*/
							size_t *				end,			/* returns the end address, virtual address in table	*/
							size_t *				length		/* returns the contiguous length from start to end		*/
								 );

int CmpMgr_Component_Match_Physical_ID( component_type * list, IOC_T physical_id );

int CmpMgr_Command(	component_type *	component,	/* component from a device		*/
					uint32_t				command,		/* component command, vector	*/
					size_t				index,		/* index within component		*/
					char *				buf,
					size_t				buf_len
					   );

int CmpMgr_List_Setup(	component_inventory_type	inventory_list[],  	/* INPUT -- enumerated inventory found in h/w or predefined for device */
						size_t						direct_offset,		/* INPUT -- base address of device													*/
						int							component_qty,	 	/* INPUT  -- output quantity limit													*/
						component_type				component_list[]   	/* OUTPUT -- device component list, some optionally defined.				*/
							);

int CmpMgr_Terminate( void );

int CmpMgr_Initialize( void );


extern module_definition_type module_cmpmgr_definition;


/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/

#endif /* MODULE_CMP_MGR */

