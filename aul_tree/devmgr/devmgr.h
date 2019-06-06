#ifndef MODULE_DEV_MGR
#define MODULE_DEV_MGR       12   /* module ID number */
#define MODULE_DEV_MGR_NAME  "dev_mgr"
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
TODO:								size_t			index,
								char *			buf,
								size_t			buf_len
						   );


*********************************************************************************/


/*******************************************************************************/
#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/target.h"
#include "../shared/sysval.h"
#include "../cmpmgr/cmpmgr.h"  /* component_type */
#include "../io/io_shared.h"   /* io_definition_type */

#include "../shared/module_macros.h"

/***************************************************************************/
#define DEVICE_MANAGER_H_REV  "$Date: 2015-02-02 17:41:44 -0600 (Mon, 02 Feb 2015) $"
/***************************************************************************/



/*****************************************************************************/	
/*****************************************************************************/	
/*****************************************************************************/




/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
struct devmgr_device;  /* forward declarations */
typedef struct devmgr_device devmgr_device_type;
/*******************************************************************/
typedef int (* dev_fnc )(   devmgr_device_type *	device,
							size_t			index,
							char *			buf,
							size_t			buf_len
					   );
typedef int ( * dev_init_fnc      )( void );
typedef int ( * dev_terminate_fnc )( void );
/******************************************************************/
struct device_definition
{
	/*  Data structure information, includes:
		 o type magic number (type)
		 o size of this data structure (element_size)
	*/
	SYS_TYPE			type;
	dev_fnc *			jump_table;
	size_t				jump_table_qty;
	dev_init_fnc		initialize;
	dev_terminate_fnc	terminate;
	io_definition_type	io_definition;
	const char *		name;
	const char *		svn_module_date;
	const char *        svn_command_date;
	const char *		svn_status_date;
};
typedef struct device_definition device_definition_type;
/******************************************************************/
/* struct devmgr_device renamed to struct devmgr_device to remove linux conflict */
struct devmgr_device
{
	/* definition of the component...fixed or constant information */
	const device_definition_type *	definition;

	/*  Component ID
		Includes both the logical ID and the instantiation, typically
		straight from hardware, or it might be predefined for some 
		components (i.e. i8255) that does not have an ID register.

		<ID: n-bits><instantiation: m-bits >
	*/
	IOC_T					physical_id;	
	/* Private data that is specific to this component.  Typically allocated
	   at time of inventory/fixup.
		Must have specific structure: type followed by cfg, then anything else.
	*/
	void *					private_data;
	/* minor number of the device */
	size_t					minor_number;
	/* component list or table -- can be static or dynamically built
		usually during driver initialization.
	*/
	component_type  *		component_table;
	size_t					component_table_qty;
	/*------------------------------------------------------------------*/
	/*  The offset address of the component with respect to the device, or
	    the base_address of the component.  
	
		For manual instantiation, the offset is the offset from the devices
		base address.

		For IAC automatic instantiation, the offset is set to that found within
		its address space (i.e. indirect address space).
	*/
	size_t				io_region_offset;

	/*	Contiguous number of bytes requested in the I/O region.
	*/
	size_t				io_region_bytes; 
	/* interrupt requested */
	size_t				io_irq;
	size_t				io_irq_active;
	/* io-reference -- io descriptor */
	int					io_device;
	/* marking to know whether or not device is actually open. */
	size_t				open_and_active;
};
/* typedef struct devmgr_device devmgr_device_type; */
/******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************/

int DevMgr_Open(	devmgr_device_type **			device_ptr,
					io_definition_type * io_definition, /* if null, try to pull from driver */
					device_definition_type * device_definition
				);

int DevMgr_Close( devmgr_device_type ** device );


int DevMgr_Command(	devmgr_device_type *	device,		/* device from a list			*/
					uint32_t		command,	/* device command, vector		*/
					size_t			index,		/* index within component		*/
					char *			buf,
					size_t			buf_len
				  );
	
int DevMgr_Terminate( void );

int DevMgr_Initialize( void );

extern module_definition_type module_devmgr_definition;


/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/

#endif /* MODULE_DEV_MGR */
