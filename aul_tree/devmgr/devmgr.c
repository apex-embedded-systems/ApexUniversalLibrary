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

#include "devmgr.h"
#include "../devices/dev_avail.h"
#include "../shared/module.h"
#include "../shared/device.h"		 /* device qty					*/
#include "../shared/typebool.h"
#include "../io/io.h"

#include "../shared/cmd_macros.h"
/*******************************************************************/
#define DEVMGR_MODULE_REV  "$Date: 2015-02-02 17:41:44 -0600 (Mon, 02 Feb 2015) $"

/*********************************************************************************/
/* if DEBUG then allow IO_DEBUG */
#define DEVMGR_DEBUG			_DEBUG
/*********************************************************************************/
#define DEVMGR_DEBUG_LOG	DEBUG_OUT

/******************************************************************
*/
static devmgr_device_type devmgr_device[DEVICE_QTY] = {{ 0 }};

/******************************************************************
*/
static int DevMgr_Device_Available( void )
{
	int index;
	for ( index = 0; index < DEVICE_QTY; index++ )
	{
		if ( NULL == devmgr_device[index].definition ) return index;
	}
	return -1;
}
/******************************************************************
*/
/*
static int DevMgr_Device_IsDuplicate( IOC_T physical_id )
{
	int index;
	for ( index = 0; index < DEVICE_QTY; index++ )
	{
		if ( physical_id == devmgr_device[index].physical_id ) return 1;
	}
	return 0;
}
*/

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/******************************************************************
*/
int DevMgr_Open(	devmgr_device_type **		device_ptr,
					io_definition_type *		io_definition, /* if null, try to pull from driver */
					device_definition_type *	device_definition
				)
{
	int						error_code;
	int						index;
/*	int						minor_number;  */ /* same as board number within driver level...not same at app level. */
/*	void *					mem;*/
	devmgr_device_type *	device;

	device = NULL;

	if ( NULL == io_definition ) 
	{	error_code = EC_DEVMGR_IO_DEFINITION;	goto DevMgr_Open_Error; }

	if ( NULL == device_ptr )
	{	error_code = EC_DEVMGR_DEVICE_NULL;		goto DevMgr_Open_Error; }

/* NOTE: don't need to because we are pre-allocating above. */
	/* allocate memory for device level information */
/*	mem = Mem_Allocate( NULL, sizeof( devmgr_device_type ) );
	if ( NULL == mem )
	{ error_code = EC_DEVMGR_ALLOCATE_MEMORY;	goto DevMgr_Open_Error; }*/

	/* Find an available device[] position in which to store device information.
	    Since we have already allocated DEVICE_QTY of them, there is no need 
		 to memory allocate them.
	*/
	index  = DevMgr_Device_Available();
	if ( index < 0 )
	{ error_code = EC_DEVMGR_ALLOCATE_DEVICE;	goto DevMgr_Open_Error; }
	device = &( devmgr_device[index] );


	/* open the IO which subsequently requests I/O region and interrupts as needed */
	error_code = IO_Open( &(device->io_device), io_definition );
	if ( error_code ) goto DevMgr_Open_Error;

	/* now populate with IO related information */
	device->io_region_offset = io_definition->region_offset;
	device->io_region_bytes  = io_definition->region_bytes;
	device->io_irq           = io_definition->interrupt_number;
	device->physical_id		 = io_definition->physical_id;
	device->minor_number     = io_definition->minor_number;
	device->definition       = device_definition;
/*
TODO:
1. determine components and attach.
2. Create and attach any private device data. 
*/
	error_code = DevMgr_Command( device, CMD_DEV_OPEN, io_definition->minor_number, (char *) io_definition, sizeof( io_definition_type ) );
	if ( error_code ) goto DevMgr_Open_Error;

	*device_ptr = device;
	device->open_and_active = TRUE;
	return SUCCESS;
DevMgr_Open_Error:
	Mem_Set( device, 0, sizeof( devmgr_device_type ) );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_DEV_MGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/******************************************************************
*/
int DevMgr_Close( devmgr_device_type ** device_ptr )
{
/* TODO
0. test pointers and validity of things
1. disable interrupt, if active.
2. device close, deallocate components and so on.
3. io_close 
4. Free device from array (i.e. clear it)
5. Null device handle on return.
*/
	int						error_code;
	devmgr_device_type *	device;

	device = NULL;
	if ( NULL == device_ptr )
	{	error_code = EC_DEVMGR_DEVICE_NULL;		goto DevMgr_Close_Error; }

	if ( NULL == *device_ptr )
	{	error_code = EC_DEVMGR_DEVICE_NULL;		goto DevMgr_Close_Error; }

	device = *device_ptr;
	/* close the device itself - release any memory */
	error_code = DevMgr_Command( device, CMD_DEV_CLOSE, 0, NULL, 0 );
	if ( error_code ) goto DevMgr_Close_Error;
	/* close the IO  */
	error_code = IO_Close( &(device->io_device) );
	if ( error_code ) goto DevMgr_Close_Error;
	/* deallocate */
	Mem_Set( device, 0, sizeof( devmgr_device_type ) );
	/* same as Mem_Set() above: device->open_and_active = FALSE; */
	*device_ptr = NULL;
	return SUCCESS;
DevMgr_Close_Error:
	Mem_Set( device, 0, sizeof( devmgr_device_type ) );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_DEV_MGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/******************************************************************
*/
int DevMgr_Command(	devmgr_device_type *	device,		/* device from a list				*/
							uint32_t		command,	/* component command, vector		*/
							size_t			index,		/* index/channel within device		*/
							char *			buf, 
							size_t			buf_len 
					   )
{
	int			error_code;
	dev_fnc *	table;

	if ( command >= device->definition->jump_table_qty ) 
	{	error_code = EC_DEVMGR_COMMAND_INVALID;	goto DevMgr_Command_Error;		}
	
	table = device->definition->jump_table;
	if ( NULL == table[command] )
	{	error_code = EC_DEVMGR_COMMAND_INVALID;	goto DevMgr_Command_Error;		}

	error_code = ( * table[command] )(	device,
										index,
										buf,
										buf_len
									 );
	if ( error_code < 0 ) goto DevMgr_Command_Error;
	return SUCCESS;
DevMgr_Command_Error:
	Error_Log( MODULE_DEV_MGR_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************
*/
int DevMgr_Terminate( void )
{
	return SUCCESS;
}
/****************************************************************
*/
int DevMgr_Initialize( void )
{
/*here we run device initializations???*/
	return SUCCESS;
}
/*****************************************************************************************
*/
module_definition_type module_devmgr_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	DevMgr_Initialize,
	DevMgr_Terminate,
	MODULE_DEV_MGR_NAME,
	DEVMGR_MODULE_REV,
	NULL,
	DEVMGR_STATUS_REV
};
