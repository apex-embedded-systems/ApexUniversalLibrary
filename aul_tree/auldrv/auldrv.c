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

	COPYRIGHT NOTICE Copyright (c) 2012-15 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded
	Systems Any reproduction or dissemination is prohibited unless specifically
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:

1. interrupts.
2. poll function
3. documentation

*********************************************************************************/
/*
Helpful Linux commands
		dmesg   - retrieve printk() messages.  -c flag clears the buffer.
		modinfo - retrieve information regarding the compiled .ko module
		insmod  - install a module
		lsmod   - list modules loaded in the kernel
		rmmod   - remove module
		modprobe - insmod plus dependencies		


The driver is composed of a controller or manager and then all devices specified
in the insmod command line.

All functions related to IOCTL operations will have a "AulDrv_Ioctl_" prefix.

All functions related to Initialization operations will have a "AulDrv_Initialize_"
prefix.

All functions related to termination or exit operations will have a
"AulDrv_Terminate_" prefix.
*/

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*   < < < INCLUDES  > > >					 																					*/
/******************************************************************************/



/*********************************************************************************/
#include <linux/version.h> 		/* LINUX_VERSION_CODE, KERNEL_VERSION	*/
#include <linux/init.h>			/* module_init, module_exit				*/
#include <linux/kernel.h>
#include <linux/module.h>		/* for modules							*/
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/fs.h>			/* file_operations						*/
#include <linux/uaccess.h>		/* copy_(to,from)_user and access_ok()	*/
#include <linux/device.h>		/* sysfs class							*/
//#include <linux/slab.h>		/* kmalloc								*/
#include <linux/errno.h>		/* return error codes  					*/

#if(0)
#include "../aul_tree/shared/compiler.h"
#include "../aul_tree/shared/device.h"
#include "../aul_tree/shared/status.h"
#include "../aul_tree/shared/cmd.h"
#include "../aul_tree/shared/module_macros.h"
#include "../aul_tree/str_safe/str_safe.h"
#include "../aul_tree/io/io_shared.h"
#endif

#include "../str_safe/str_safe.h"
#include "../shared/cmd_macros.h"
#include "../shared/cmd.h"
#include "../shared/device_list.h"   /* struct device_list */
#include "../shared/device.h"        /* DEVICE_QTY         */

#include "../shared/status.h"
#include "../io/io_shared.h"
#include "../io/io_private.h"  /* IO_COPY_TO/FROM_USER() */

#include "../io/io.h"
#include "../shared/ts_svn.h"     /* extract subversion date code */

#include "../devices/dev_avail_module.h"  /* DEVICE_STX104, DEVICE_TRACERE */

/*********************************************************************************/
#define AULDRV_MODULE_REV  "$Date: 2016-09-29 16:57:39 -0500 (Thu, 29 Sep 2016) $"
/*********************************************************************************/
#ifndef MODULE_AULDRV
# define MODULE_AULDRV      7 				/* module ID number */
# define MODULE_AULDRV_NAME "auldrv"//DRIVER_NAME    	/* must be 'auldrv' */
#endif /* MODULE_AULDRV */

#define DBG_DRV_NAME  MODULE_AULDRV_NAME

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/*              < < < DEBUG PARAMS > > >                                         */
/*********************************************************************************/

/*********************************************************************************/
/* if DEBUG then allow AULDRV_DEBUG */
#define AULDRV_DEBUG		  _DEBUG
/*********************************************************************************/
/* comment out the line below if you wish to remove
   debug output from this module											*/

#define AULDRV_DEBUG_LOG   DEBUG_OUT




/*********************************************************************************/
/* DO NOT USE */
/* #define AULDRV_DEVICE_DYNAMIC 1 */
/*********************************************************************************/

enum { AULDRV_DRIVER_MINOR_NUMBER = 0    };
enum { AULDRV_DRIVER_MINOR_INVALID = 100 }; /* TODO: use max unsigned value */

/*********************************************************************************/
/* TODO: merge with debug.c and debug.h */
#undef PDEBUG             /* undef it, just in case */
#if ( defined( __KERNEL__ ) && defined( _DEBUG ) )
  /* This one if debugging is on, and kernel space */
//#  include <linux/kernel.h>
#  ifdef DRIVER_NAME
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG DBG_DRV_NAME " " fmt, ## args )
#    pragma message "PDEBUG: __KERNEL__ and DRIVER_NAME=" DRIVER_NAME "."
#  else
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "" fmt, ## args )
#    pragma message "PDEBUG: __KERNEL__ and no DRIVER_NAME"
#  endif
#elif ( DEBUG_APPLICATION )
   /* This one for user space */
#  include <stdio.h>
#  ifdef  DRIVER_NAME
#    define PDEBUG(fmt, args...) fprintf( stderr, DBG_DRV_NAME fmt, ## args )
#    pragma message "PDEBUG: no __KERNEL__, DRIVER_NAME=" DRIVER_NAME "."
#  else
#    define PDEBUG(fmt, args...) fprintf( stderr, fmt, ## args )
#    pragma message "PDEBUG: DEBUG_APPLICATION and no DRIVER_NAME"
#  endif
#else
#  define PDEBUG( fmt, args... ) /* not debugging: nothing */
#  pragma message "PDEBUG: no debug reporting."
#endif
/*********************************************************************************/


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*   < < < MODULE PARAMETERS > > > 		  									  */
/******************************************************************************/
#if !defined( MODULE_MACROS_H_ )
# pragma message ( "aul_tree: auldrv.c  MODULE_MACRO_H_ not defined" )
/*********************************************************************************/
typedef int  ( * module_fnc     )( void );
/*******************************************************************************/
struct module_definition
{
//	const SYS_TYPE		type;

	module_fnc		initialize;
	module_fnc		terminate;
	/* Purpose of revision information:
			1. Module management.
			2. Determine if application and module status/commands are in sync.
	*/
	const char *	name;
	const char *	svn_module_date;
	const char *    svn_command_date;
	const char *	svn_status_date;
};
typedef struct module_definition auldrv_module_definition_type;
#endif

/*******************************************************************************/
#define IO_DEFINITION_ACRONYM_SIZE  16
/*******************************************************************************/
#if !defined( IO_SHARED_H__ )
# pragma message ( "aul_tree: auldrv.c  IO_SHARED_H__ not defined" )
struct io_definition
{
	/* physical_id
		The physical id is both the logical_id along with the instance.
	*/
	IOC_T				physical_id;
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
	char				acronym[IO_DEFINITION_ACRONYM_SIZE];
};
typedef struct io_definition io_definition_type;
#endif
/*  NOTE: in this particular case "region" means the same as "device"		*/
/******************************************************************************/
/*#define AULDRV_REGION_QTY		2*/
#define AULDRV_REGION_QTY       DEVICE_QTY
/**********************************************************************************************************//**
@ingroup auldrv
@brief
*/
struct auldrv_region
{
	int			 io_descriptor;
	size_t		 open_and_active;
	size_t		 offset;
	size_t		 bytes;
	size_t		 restrict_8bits;
	char *		 name;
};
/**********************************************************************************************************//**
@ingroup auldrv
@brief
Data structure of parameters specific to this driver.
*/
struct auldrv_driver_params
{
	unsigned					major;	/* major number				*/
	unsigned					minor;	/* minor number				*/
	dev_t						number; /* device number			*/
	char *						name;	/* name of this driver		*/
	struct cdev					cdev;	/* driver character device	*/
	unsigned					minor_count; /* quantity of minor numbers */
	int							open_and_active;
	int							error_code_last;
	struct module_definition	module_aul; /* our driver definition of a module */
	struct class *				class;	/* driver class				*/
};

/**********************************************************************************************************//**
@ingroup auldrv
@brief
Data structure of device parameters.
*/
struct auldrv_device_params
{
	unsigned				major;	/* major number				*/
	unsigned				minor;	/* minor number				*/
	dev_t					number; /* device number			*/
	char *					name;	/* name of this device		*/
	struct cdev				cdev;	/* driver character device	*/
	int						open_and_active;
	int						error_code_last;
	int						io_descriptor;
	struct io_definition	region;
};
/**********************************************************************************************************//**
@ingroup auldrv
@brief
This is the "top" layer data structure for this driver.  This data structure
points to driver parameters and all device parameters.
*/
struct auldrv_dataset
{
	struct auldrv_driver_params		driver; /* the driver details */
	unsigned						map_minor_to_device[AULDRV_REGION_QTY + 1 /* driver/controller */];
	unsigned						device_count;				/* how many devices in our list */
	struct auldrv_device_params		device[AULDRV_REGION_QTY];  /* list of devices */
};
typedef struct auldrv_dataset_type;

/*********************************************************************************/


/*********************************************************************************/
/*********************************************************************************/
/*                 < < < VARIABLES > > >                                         */
/*********************************************************************************/

static const char * auldrv_module_name = MODULE_AULDRV_NAME;
/* Used to report back the name of this kernel module */
static const char auldrv_ioctl_module_name[] = "mod_"  MODULE_AULDRV_NAME;
static struct auldrv_dataset auldrv = { { 0 } };


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*   < < < MODULE PARAMETERS > > > 		  													*/
/******************************************************************************/

/*******************************************************************************
Module parameters passed in by insmod or modprobe.

TODO:
1. improve flexibility.  For now, these MUST be defined.
2. move all parameters into a data structure for easy modifications.

Note: These parameters will take priority over any library parameters.

	  What are these parameters?  They describe the hardware base address,
	  I/O space to allocate, the device name and whether or not to restrict
	  I/O transactions to byte width.   These parameters describe an
	  ISA bus interface.

	offset   - sets the base address of each region.
	bytes    - sets the contiguous bytes at a given I/O base address.
	restrict - restricts data transactions to 8-bit bus width.
	acronym  - name of the region (or device).
*/

static int auldrv_requested_driver_major_number = 0;
module_param_named( major, auldrv_requested_driver_major_number, int, S_IRUGO );
MODULE_PARM_DESC( major, "This driver ID. Dynamic assignment when major=0 (default)." );

/* NOTE: double {} is a GCC work around */
static unsigned int auldrv_requested_region_offset[AULDRV_REGION_QTY] = { { 0 } };
static int auldrv_requested_region_offset_count = 0;
module_param_array_named(offset, auldrv_requested_region_offset, uint, &auldrv_requested_region_offset_count, S_IRUGO );
MODULE_PARM_DESC( offset, "region offset array. Ex: offset=0x300 or offset=0x300,0x310" );

static unsigned int auldrv_requested_region_bytes[AULDRV_REGION_QTY] = {  0, };
static int auldrv_requested_region_bytes_count = 0;
module_param_array_named( bytes, auldrv_requested_region_bytes, uint, &auldrv_requested_region_bytes_count, S_IRUGO );
MODULE_PARM_DESC( bytes, "region bytes array. Ex: bytes=0x10 or bytes=0x10,0x10" );

static unsigned int auldrv_requested_region_restrict_8bit[AULDRV_REGION_QTY] = { { 0 } };
static int auldrv_requested_region_restrict_8bit_count = 0;
module_param_array_named( restrict, auldrv_requested_region_restrict_8bit, uint, &auldrv_requested_region_restrict_8bit_count, S_IRUGO );
MODULE_PARM_DESC( bit8, "region restrict 8-bits. Ex: restrict=0 or restrict=0,1" );

static char * auldrv_requested_region_acronym[AULDRV_REGION_QTY] = { NULL, };
static int auldrv_requested_region_acronym_count = 0;
module_param_array_named( acronym, auldrv_requested_region_acronym, charp, &auldrv_requested_region_acronym_count, S_IRUGO );
MODULE_PARM_DESC( acronym, "region name array. Ex: acronym=\"stx104\" or acronym=\"stx104\",\"tracer\"" );

static char * auldrv_requested_region_type[AULDRV_REGION_QTY] = { NULL, };
static int auldrv_requested_region_type_count = 0;
module_param_array_named( type, auldrv_requested_region_type, charp, &auldrv_requested_region_type_count, S_IRUGO );
MODULE_PARM_DESC( type, "type name array. Ex: type=\"sxt104\" or type=\"stx104\",\"tracer\"" );

/*********************************************************************************/
/*          < < < IOCTL() SUPPORT FUNCTIONS > > >                                */
/*********************************************************************************/

const struct file_operations auldrv_file_operations;

static int AulDrv_Initialize_Parameters_Device( struct auldrv_device_params *	dp,
												unsigned						index_device,
												struct class *					driver_class,
												dev_t							driver_number,
												struct file_operations * 		fops
											  );

static void AulDrv_Terminate_Device( struct auldrv_device_params *  dp,
									 struct class * 				driver_class
								   );

/**********************************************************************************************************//**
@ingroup auldrv
@brief
Based on the incoming io_definition, it will return all the details for the named
device.  In other words, the acronym indicated by AUL_Device_Open() will be received
here and compared to all the ones listed from the insmod command line and compared
until a match is found.  Once a match is found, this code will return the complete
io_definition of the device back to the library (i.e. user space).

The "AulDrv_Ioctl_" prefix indicates that this is a helper function used by Ioctl
function.

@return
If a successful match is found, then SUCCESS is returned, otherwise an -error code.
*/
/* TODO: ought to become IO_Definition_Get_Command()??? */
static int AulDrv_Ioctl_Device_Definition_Get( unsigned long argument )
{
	int						error_code;
	int						valid;
	unsigned				index;
	io_definition_type   	io_definition_local;

	PDEBUG( "%s entry.\n", __func__ );

	/* get the io_definition from user space */
	if ( IO_COPY_FROM_USER( &io_definition_local, argument, sizeof( io_definition_local ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto AulDrv_Ioctl_Device_Definition_Get_Error;	 }

	/* search for definition by comparing acronyms */
	valid = -1; /* assume invalid */
	for ( index = 0; index < auldrv.device_count; index++ )
	{
		if ( 0 == String_Compare( auldrv.device[index].region.acronym,
				                  io_definition_local.acronym,
				                  IO_DEFINITION_ACRONYM_SIZE          )      )
		{
			valid = index;
			break;
		}
	}
	if ( -1 != valid )
	{	/* pass it back to the user with additional information */
		if ( IO_COPY_TO_USER( argument, &(auldrv.device[valid].region), sizeof( io_definition_type ) ) )
		{ error_code = EC_IO_COPY_TO_USER;	goto AulDrv_Ioctl_Device_Definition_Get_Error;  }
	}
	else
	{ error_code = EC_AULDRV_IOCTL_DEFINITION;  goto AulDrv_Ioctl_Device_Definition_Get_Error;  }

	PDEBUG( "%s exit success.\n", __func__ );
	return SUCCESS;
AulDrv_Ioctl_Device_Definition_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	PDEBUG( "%s exit fail, error = %d.\n", __func__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
Returns a list of devices to the caller.

The "AulDrv_Ioctl_" prefix indicates that this is a helper function used by Ioctl
function.

@return
If successful SUCCESS is returned, otherwise an error code is returned.
*/
static int AulDrv_Ioctl_Device_List_Get( unsigned long argument )
{
	int						error_code;
	unsigned				index;
	/* NOTE: struct device_list same as device_list_type */
	device_list_type		device_list;  /* null terminated list */

	Mem_Set( &device_list, 0, sizeof( device_list ) );
	device_list.type = SYS_TYPE_MAKE( device_list_type );
	device_list.region_count = (size_t) auldrv.device_count;

	index = 0;
	while( index < auldrv.device_count )
	{
		Mem_Copy( &(device_list.region[index]), &(auldrv.device[index].region), sizeof( device_list.region[index] ) );
		device_list.open_and_active[index] 	= auldrv.device[index].open_and_active;
		index++;
	}
	if ( IO_COPY_TO_USER( argument, &(device_list), sizeof( device_list ) ) )
	{ error_code = EC_IO_COPY_TO_USER;	goto AulDrv_Ioctl_Device_List_Get_Error;  }
	PDEBUG( "%s exit success.\n", __func__ );
	return SUCCESS;
AulDrv_Ioctl_Device_List_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	PDEBUG( "%s exit fail, error = %d.\n", __func__, error_code );
	return -error_code;
}
/*********************************************************************************/



/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/*          < < < IOCTL() FUNCTION > > >                                         */
/*********************************************************************************/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int AulDrv_Ioctl(	struct inode * inode,
							struct file * 	file,
							unsigned int 	command,
							unsigned long 	argument
					   )
{
#else

static long AulDrv_Ioctl(	struct file *	file,
							unsigned int 	command,
							unsigned long	argument
						)
{
	struct inode *	inode		= file->f_dentry->d_inode;
#endif
	int			error_code;
	unsigned    index_device;
	IOC_T		module_logical_id;
	unsigned 	minor = MINOR( inode->i_rdev );

	PDEBUG( "%s entry.\n", __func__ );

	if ( !IOC_MAGIC_IS_VALID( command )  ) 
	{	error_code = EC_AULDRV_IOCTL_MAGIC_NUMBER;	goto AulDrv_Ioctl_Error;	}

	module_logical_id = IOC_GET_LOGICAL_ID_MODULE( command );
	error_code = SUCCESS;
	if ( ( AULDRV_LOGICAL_ID == module_logical_id ) && ( AULDRV_DRIVER_MINOR_NUMBER == minor ) )
	{ /* controller */

		switch ( command )
		{
			case CMD_AULDRV_VERSION:
				{ /* returns the SVN date-time stamp */
					uint32_t version;
					version = TSSVN_YYMMDDHH_DEC( AULDRV_MODULE_REV );
					if ( IO_COPY_TO_USER( argument, &version, sizeof( version ) ) )
					{ error_code = EC_AULDRV_COPY_TO_USER;	goto AulDrv_Ioctl_Error;  }
				}
				break;
			case CMD_AULDRV_NAME:
				{
					if ( IO_COPY_TO_USER( argument, auldrv_ioctl_module_name, String_Length( auldrv_ioctl_module_name ) + 1 ) )
					{ error_code = EC_AULDRV_COPY_TO_USER;	goto AulDrv_Ioctl_Error;  }
				}
				break;
			case CMD_AULDRV_DEVICE_DEFINITION_AVAILABLE:
				/* TODO: ought to become IO_Definition_Available_Command()??? */
				error_code = SUCCESS;
				break;
			case CMD_AULDRV_DEVICE_DEFINITION_GET:
				/* TODO: ought to become IO_Definition_Get_Command()??? */
				error_code = AulDrv_Ioctl_Device_Definition_Get( argument );
				break;
			case CMD_AULDRV_DEVICE_LIST_GET:
				/* retrieve all devices information (mostly generated at insmod)  */
				error_code = AulDrv_Ioctl_Device_List_Get( argument );
				break;
			case CMD_AULDRV_DEBUG_ACTIVE_GET:
				{ /* return zero if _DEBUG is not defined, otherwise non-zero */
					int debug_active;
#ifdef _DEBUG
					debug_active = 1;
#else
					debug_active = 0;
#endif
					if ( IO_COPY_TO_USER( argument, &debug_active, sizeof( debug_active ) ) )
					{ error_code = EC_AULDRV_COPY_TO_USER;	goto AulDrv_Ioctl_Error;  }
				}
				break;
#ifdef AULDRV_DEVICE_DYNAMIC
			case 1033: /* create stx104 */
				{
					unsigned index_device = argument;
					/* TODO: test for argument validity */
					error_code = AulDrv_Initialize_Parameters_Device( &(auldrv.device[index_device]),
																	  index_device,
																	  auldrv.driver.class,
																	  auldrv.driver.number,
																	  &auldrv_file_operations
																	);
					if ( error_code ) goto AulDrv_Ioctl_Error;
					auldrv.device_count++;
					minor = index_device + 1;
					auldrv.map_minor_to_device[minor] = index_device;

					error_code = IO_Open( &(auldrv.device[index_device].io_descriptor), &(auldrv.device[index_device].region) );
					if ( error_code ) goto AulDrv_Ioctl_Error;
					error_code = IO_Capture( auldrv.device[index_device].io_descriptor );
					if ( error_code ) goto AulDrv_Ioctl_Error;
				}
				break;
			case 1099: /* destroy stx104 */
				{
					unsigned index_device = argument;
					/* TODO: test for argument validity */
					AulDrv_Terminate_Device( &(auldrv.device[index_device]), auldrv.driver.class );

					minor = index_device + 1;
					auldrv.map_minor_to_device[minor] = AULDRV_DRIVER_MINOR_INVALID;
					auldrv.device_count--;

					error_code = IO_Release( auldrv.device[index_device].io_descriptor, 1 /* test_for_device_ready */ );
					if ( error_code ) goto AulDrv_Ioctl_Error;
					error_code = IO_Close( &(auldrv.device[index_device].io_descriptor) );
					if ( error_code ) goto AulDrv_Ioctl_Error;
				}
				break;
#endif
			default:
				error_code = EC_AULDRV_IOCTL_COMMAND_UNKNOWN; /* command invalid */
		}
	}
	else if ( IO_LOGICAL_ID == module_logical_id )
	{
		if ( !auldrv.driver.open_and_active )
		{ error_code = EC_AULDRV_DRIVER_OPEN_NOT; goto AulDrv_Ioctl_Error; }
		/* TODO: test for argument validity */
		index_device = auldrv.map_minor_to_device[minor];
		/* perform the actual I/O command requested */
		error_code   = IO_Ioctl( auldrv.device[index_device].io_descriptor, command, argument ); 
		if ( error_code ) goto AulDrv_Ioctl_Error;
	}
	else
	{
		error_code = EC_AULDRV_IOCTL_COMMAND_UNKNOWN; /* command invalid */
		goto AulDrv_Ioctl_Error;
	}
	if ( error_code ) goto AulDrv_Ioctl_Error;
	PDEBUG( "%s exit success.\n", __func__ );
	return SUCCESS;

AulDrv_Ioctl_Error:
	if ( error_code < 0 ) error_code = -error_code;
	PDEBUG( "%s exit fail, error = %d.\n", __func__, error_code );
	return -error_code;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*   < < < OPEN() AND CLOSE() FUNCTION > > > 																	*/
/******************************************************************************/

/**********************************************************************************************************//**
@ingroup auldrv
@brief
*/
static atomic_t auldrv_driver_available = ATOMIC_INIT(1);

/**********************************************************************************************************//**
@ingroup auldrv
@brief

@return
*/
static int AulDrv_Open( struct inode * inode, struct file * file )
{
	int			error_code = SUCCESS;
	unsigned 	minor = MINOR( inode->i_rdev );
	unsigned    index_device;

	PDEBUG( "%s entry.\n", __func__ );

	if ( AULDRV_DRIVER_MINOR_NUMBER == minor )
	{ /* controller */

		if ( ! atomic_dec_and_test( &auldrv_driver_available ) )
		{
			atomic_inc( &auldrv_driver_available );
			return -EBUSY; 
		}

		PDEBUG( " %s:  minor = %d, controller = %s\n", __func__, minor, auldrv_module_name );
		if ( auldrv.driver.open_and_active ) /* already open */
		{ error_code = EC_AULDRV_DRIVER_ALREADY_OPEN; goto AulDrv_Open_Error; }
		//file->private_data = &(auldrv.device);
		auldrv.driver.open_and_active = 1;
	}
	else
	{ /* devices or boards */
		index_device = auldrv.map_minor_to_device[minor];
		if ( AULDRV_DRIVER_MINOR_INVALID == index_device )
		{ error_code = EC_AULDRV_OPEN_MINOR_INVALID;   goto AulDrv_Open_Error;  }

		if ( auldrv.device[index_device].open_and_active )
		{ error_code = EC_AULDRV_DRIVER_ALREADY_OPEN; goto AulDrv_Open_Error; }

		PDEBUG( " %s: minor = %d, device = %s\n", __func__, minor, auldrv.device[index_device].region.acronym );
		/* get I/O space and interrupts allocated */
		error_code = IO_Open( &(auldrv.device[index_device].io_descriptor), &(auldrv.device[index_device].region) );
		if ( error_code ) goto AulDrv_Open_Error;

		auldrv.device[index_device].open_and_active = 1;
	}
	PDEBUG( "%s exit success.\n", __func__ );
	return SUCCESS;
AulDrv_Open_Error:
	if ( error_code < 0 ) error_code = -error_code;
	PDEBUG( "%s exit fail, error = %d.\n", __func__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief

@return
*/
static int AulDrv_Close( struct inode * inode, struct file * file )
{
	int			error_code;
	unsigned 	minor = MINOR( inode->i_rdev );
	unsigned    index_device;

	PDEBUG( "%s entry.\n", __func__ );
	if ( AULDRV_DRIVER_MINOR_NUMBER == minor )
	{ /* controller */

		atomic_inc( &auldrv_driver_available );

		PDEBUG( " %s: minor = %d, controller = %s\n", __func__, minor, auldrv_module_name );
		auldrv.driver.open_and_active = 0;
	}
	else
	{
		index_device = auldrv.map_minor_to_device[minor];
		PDEBUG( " %s: minor = %d, device = %s\n", __func__, minor, auldrv.device[index_device].region.acronym );
#if(0)
/* Ioctl() -> IO_Ioctl() by library */
		error_code = IO_Release( auldrv.device[index_device].io_descriptor, 0 /* test_for_device_ready */ );
		if ( error_code ) goto AulDrv_Close_Error;
#endif

		error_code = IO_Close( &(auldrv.device[index_device].io_descriptor) );
		if ( error_code ) goto AulDrv_Close_Error;

		auldrv.device[index_device].open_and_active = 0;
	}
	PDEBUG( "%s exit success.\n", __func__ );
	return SUCCESS;
AulDrv_Close_Error:
	if ( error_code < 0 ) error_code = -error_code;
	PDEBUG( "%s exit fail, error = %d.\n", __func__, error_code );
	return -error_code;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*   < < < FILE OPERATIONS DEFINITION > > > 																	*/
/******************************************************************************/
const struct file_operations auldrv_file_operations =
{
	.owner						= THIS_MODULE, 		/* owner 							*/
	.open						= AulDrv_Open,		/* open method						*/
	.release					= AulDrv_Close,		/* release or close method			*/
//	.read						= AulDrv_Read,		/* read method						*/
//	.write						= AulDrv_Write,		/* write method						*/
//	.llseek						= device_seek,		/* seek method						*/
//  .poll
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	.ioctl 						= AulDrv_Ioctl
#else
	.unlocked_ioctl 			= AulDrv_Ioctl		/* Note: unlocked_ioctl avoids Big Kernel Locking (BKL)	*/
#endif
};

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*   < < < MODULE INITIALIZE AND EXIT FUNCTIONS > > > 												*/
/******************************************************************************/

/**********************************************************************************************************//**
@ingroup auldrv
@brief
Removes the device from the system.

@return
*/
static void AulDrv_Terminate_Device( struct auldrv_device_params *  dp,
									 struct class * 				driver_class
								   )
{
	if ( NULL != driver_class ) device_destroy( driver_class, dp->number );
	if ( NULL != dp->cdev.ops ) cdev_del( &(dp->cdev) );
	/* TODO: clear all device parameters */
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
Walks through all devices and removes them.

@return nothing
*/
static void AulDrv_Terminate_Devices( struct auldrv_device_params * dp,
									  unsigned 						count,
									  struct class * 				driver_class
								   )
{
	unsigned	index_device;

	for ( index_device = 0; index_device < count; index_device++ )
	{
		AulDrv_Terminate_Device( &(dp[index_device]), driver_class );
	}
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
Removes the manager or controller portion of the driver.

@return nothing
*/
static void AulDrv_Terminate_Driver( struct auldrv_driver_params * dp )
{
	/* release, if any, device class associated with driver */
	if ( NULL != dp->class )
	{
		device_destroy( dp->class, dp->number );
	}
	/* release, if any, the cdev associated with driver */
	if ( NULL != dp->cdev.ops )
	{
		cdev_del( &(dp->cdev) );
	}
	/* release, if any, driver class */
	if ( NULL != dp->class )
	{
		class_unregister( dp->class );
		class_destroy(    dp->class );
	}
	/* release, if any, the major/minor numbers */
	if ( dp->number )
	{
		unregister_chrdev_region( dp->number, dp->minor_count );
	}
	/* TODO: clear all driver parameters */
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
Top level function that removes/terminates all devices and the manager.

@return nothing
*/
static void AulDrv_Terminate_AulDrv_Dataset( struct auldrv_dataset * ds )
{

	/* terminate/remove devices specified in insmod command line */
	AulDrv_Terminate_Devices( ds->device, ds->device_count, ds->driver.class );
	/* terminate the controller portion of the driver */
	AulDrv_Terminate_Driver( &(ds->driver) );
	/* TODO: clear ds->driver */
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
Allocate the character device to the kernel.

@return
returns OS type error code.
*/
static int AulDrv_Initialize_Device_Add( struct cdev * 				cdev,
										 dev_t   				   	number,
										 struct file_operations * 	fops
							)
{
	int error_code;
	/* connects driver methods with associated cdev */
	cdev_init( cdev, fops );
	cdev->owner = THIS_MODULE;
	cdev->ops   = fops;
	error_code  = cdev_add( cdev, number, 1 );
	return error_code;
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
Return a driver number which includes both major and minor numbers.  Register the
driver controller device in this case (auldrv).

@return OS type error code.
*/
static int AulDrv_Initialize_Driver_Number(	unsigned	major_number,
											dev_t *		number,
											size_t		minor_quantity,
											char *		driver_name
										  )
{
	int		error_code;
	dev_t	driver_number;

PDEBUG( "%s entry.\n", __func__ );

	if ( major_number )
	{
		driver_number = MKDEV( major_number, AULDRV_DRIVER_MINOR_NUMBER );
		error_code  = register_chrdev_region( driver_number, minor_quantity, driver_name);
		if ( !error_code ) *number = driver_number;
	}
	else
	{	/* request dynamic allocation of a device major number */
		error_code	= alloc_chrdev_region( number, AULDRV_DRIVER_MINOR_NUMBER, minor_quantity, driver_name );
	}
	return error_code;
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
The module definition of auldrv.  Currently, the name and revision are used for purposes of reporting
and used with udev to produce the device "auldrv" as seen in /dev.

@return Nothing
*/
static void AulDrv_Initialize_Definition_Module( struct module_definition * d )
{
PDEBUG( "%s entry.\n", __func__ );

	d->initialize		= NULL;
	d->terminate		= NULL;
	d->name				= auldrv_module_name;
	d->svn_module_date	= AULDRV_MODULE_REV;
	d->svn_command_date = NULL;
	d->svn_status_date  = NULL;
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
This is an internal initialization function.  Initializes driver functions
and creates the driver controller "auldrv" within /dev.  This node is always
created and its purpose is for sending and receiving driver management
information between library and driver.

@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
static int AulDrv_Initialize_Driver( struct auldrv_driver_params *	dp,
									 struct class *					driver_class,
									 dev_t							driver_number,
									 struct file_operations * 		fops
								   )
{
	int				error_code;
	struct device * dev_ret;
PDEBUG( "%s entry.\n", __func__ );
	dp->major			= MAJOR( driver_number );
	dp->minor			= MINOR( driver_number );
	dp->number			= driver_number;
	dp->name			= dp->module_aul.name;
	dp->minor_count		= AULDRV_REGION_QTY + 1;
	dp->open_and_active	= 0;
	dp->error_code_last	= SUCCESS;
	dp->class			= driver_class;

PDEBUG( "%s:  major = %d.\n", __func__, dp->major );
PDEBUG( "%s:  minor = %d.\n", __func__, dp->minor );

	/* add the device "auldrv" to cdev */
	error_code = AulDrv_Initialize_Device_Add( &(dp->cdev), dp->number, fops );
	if ( error_code ) return error_code;

PDEBUG( "%s AulDrv_Initialize_Device_Add complete.\n", __func__ );

	/* send uevents to udev, so it will create /dev/auldrv node */
	dev_ret = device_create( dp->class, 	/* class  */
							 NULL,   		/* parent */
							 dp->number,
							 NULL,
							 "%s",
							 dp->name
						   );
	if ( NULL == dev_ret )
	{
		error_code = ENODEV;
	}
PDEBUG( "%s device_create, error=%d.\n", __func__, error_code );
	return error_code;
}
/*******************************************************************************/
/* TODO: duplicate in aul_hwdef.c - merge to a central location */
#if defined( DEVICE_TRACERE )
 static const char auldrv_tracere[] = DEVICE_TRACERE_NAME;
#endif
#if defined( DEVICE_STX104ND )
 static const char auldrv_stx104nd[] = DEVICE_STX104ND_NAME;
#endif
#if defined( DEVICE_STX104 )
 static const char auldrv_stx104[]  = DEVICE_STX104_NAME;
#endif
#if defined( DEVICE_SUMMIT )
 static const char auldrv_summit[]  = DEVICE_SUMMIT_NAME;
#endif

 /**********************************************************************************************************//**
 @ingroup auldrv
 @brief
 This is an internal initialization function.  Populates the io_definition based on the
 insmod command line parameters entered.  In our case the device_index is the same as the
 minor number.

 @param  d				I/O definition of the device that will be populated.
 @param index_device 	The device index or minor number
 @return none
 */
static void AulDrv_Initialize_Definition_Device( struct io_definition * d, unsigned index_device )
{
PDEBUG( "%s entry using index_device = %d\n", __func__, index_device );
	d->region_offset        = auldrv_requested_region_offset[index_device];
	d->region_bytes         = auldrv_requested_region_bytes[index_device];
	d->region_restrict_8bit = auldrv_requested_region_restrict_8bit[index_device];
	d->interrupt_number		= 0; /*TODO: */
	d->minor_number         = index_device;
	if ( NULL != auldrv_requested_region_acronym[index_device] )
	{
		strncpy( d->acronym, auldrv_requested_region_acronym[index_device], IO_DEFINITION_ACRONYM_SIZE );
	}
	else
	{
		d->acronym[0] = '\0';
	}
PDEBUG( "  d->acronym = %s, ", d->acronym );
	/* assume none */
	d->physical_id = IOC_LOGICAL_ID_NULL;
#if defined( DEVICE_TRACERE )
	if ( 0 == String_Compare( auldrv_tracere, auldrv_requested_region_type[index_device],
	    					  String_Length( auldrv_tracere )                         ) )
	{
		d->physical_id = IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_TRACERE );
PDEBUG( "TRACERE: d->physical_id = %d\n", d->physical_id );
	}
#endif
#if defined( DEVICE_STX104ND )
	if( 0 == String_Compare( auldrv_stx104nd, auldrv_requested_region_type[index_device],
							 String_Length( auldrv_stx104nd )                                ) )
	{
		d->physical_id = IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_STX104ND );
PDEBUG( "STX104ND: d->physical_id = %d\n", d->physical_id );
	}
#endif
#if defined( DEVICE_STX104 )
	if( 0 == String_Compare( auldrv_stx104, auldrv_requested_region_type[index_device],
							 String_Length( auldrv_stx104 )                                ) )
	{
		d->physical_id = IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_STX104 );
PDEBUG( "STX104: d->physical_id = %d\n", d->physical_id );
	}
#endif
#if defined( DEVICE_SUMMIT )
	if ( 0 == String_Compare( auldrv_summit, auldrv_requested_region_type[index_device],
							  String_Length( auldrv_summit )                          ) )
	{
		d->physical_id = IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_SUMMIT );
PDEBUG( "SUMMIT: d->physical_id = %d\n", d->physical_id );
	}
#endif
PDEBUG( "%s exit success.\n", __func__ );
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
The main initialization function used during installation of the driver.  This section
makes the /dev nodes for each of the devices specified in the insmod command line.

@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
static int AulDrv_Initialize_Parameters_Device( struct auldrv_device_params *	dp,
												unsigned						index_device,
												struct class *					driver_class,
												dev_t							driver_number,
												struct file_operations * 		fops
											  )
{
	int 			error_code;
	struct device * dev_ret;
PDEBUG( "%s entry.\n", __func__ );
	AulDrv_Initialize_Definition_Device( &(dp->region), index_device );
	dp->major			= MAJOR( driver_number );
	dp->minor			= MINOR( driver_number ) + index_device + 1;
	dp->number			= MKDEV( dp->major, dp->minor );
	dp->name			= dp->region.acronym;
	dp->open_and_active = 0;

PDEBUG( "%s: index = %d, major = %d.\n", __func__, index_device, dp->major );
PDEBUG( "%s: index = %d, minor = %d.\n", __func__, index_device, dp->minor );
	/* allocate device within kernel */
	error_code = AulDrv_Initialize_Device_Add( &(dp->cdev), dp->number, fops );
	if ( error_code ) return error_code;

PDEBUG( "%s AulDrv_Initialize_Device_Add complete.\n", __func__ );

	/* send uevents to udev, so it will create /dev node */
	dev_ret = device_create( driver_class, 				/* class  */
							 NULL,   					/* parent */
							 dp->number,
							 NULL,
							 "%s",
							 dp->name
						   );
	if ( NULL == dev_ret )
	{
		error_code = ENODEV;
	}
	return error_code;
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
The main initialization function used during installation of the driver.  This initializes
all of the devices defined in the insmod command line.

@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
static int AulDrv_Initialize_Devices( struct auldrv_device_params *	dp,
									  unsigned *					device_count_inout,
									  struct class *				driver_class,
									  dev_t							driver_number,
									  struct file_operations * 		fops,
									  unsigned *					map_minor_to_device
									)
{
	int			error_code;
	unsigned	index_device;
	unsigned	minor;
	unsigned	count;
	unsigned    quantity;
PDEBUG( "%s entry.\n", __func__ );
	quantity = *device_count_inout;
	count = 0;

PDEBUG( "%s: quantity = %d.\n", __func__, quantity );

	for ( index_device = 0; index_device < quantity; index_device++ )
	{
		error_code = AulDrv_Initialize_Parameters_Device( &(dp[index_device]),
														  index_device,
														  driver_class,
														  driver_number,
														  fops
														);
		if ( error_code ) return error_code;
		minor = dp[index_device].minor;
		map_minor_to_device[minor] = index_device;
		count++;
		*device_count_inout = count;
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup auldrv
@brief
The main initialization function used during installation of the driver.

@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
static int AulDrv_Initialize_Dataset( struct auldrv_dataset *	ds,
									  struct class *			driver_class,
									  dev_t						driver_number,
									  struct file_operations * 	fops
								    )
{
	int error_code;
PDEBUG( "%s entry.\n", __func__ );
	/* Initialize the drivers module related parameters.  In the overall
	 * design of both the library and driver we consider various chunks
	 * of code to represent a module (not very precise description).  In
	 * this case the controller portion of the driver "auldrv" is considered
	 * a software module.
	 */
	AulDrv_Initialize_Definition_Module( &(ds->driver.module_aul) );
	/* Initialize the controller "auldrv" portion of the driver. */
	error_code = AulDrv_Initialize_Driver( 	&(ds->driver),
											driver_class,
											driver_number,
											fops
										 );
	if ( error_code ) return error_code;

#ifdef AULDRV_DEVICE_DYNAMIC
	ds->device_count = 0;
PDEBUG( "%s: device_count = %d.\n", __func__, ds->device_count );
#else
/* NOTE: device_count will never be larger than AULDRV_REGION_QTY, see
 *       AulDrv_Initialize_Driver() above.
 */
	ds->device_count = ds->driver.minor_count - 1;

PDEBUG( "%s: device_count = %d.\n", __func__, ds->device_count );
	/* initialize all the devices specified in the insmod command line */
	error_code = AulDrv_Initialize_Devices( ds->device,
											&(ds->device_count),
											driver_class,
											driver_number,
											fops,
											ds->map_minor_to_device
										  );
#endif
	return error_code;
}

/*******************************************************************************
TODO:  __init required?

group is "staff"
*/
/**********************************************************************************************************//**
@ingroup auldrv
@brief
The main initialization function used during installation of the driver as specified by module_init().

@return
An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it
returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
/**************************************************************************//**
 * @brief
 * The main initialization function used during installation of the driver.
 **/
int AulDrv_Initialize( void )
{
	int				error_code;
	/* Total number of minor numbers we need for this driver.
	   We will need a total of AULDRV_REGION_QTY + 1 where the
	   additional one is for the driver and the AULDRV_REGION_QTY
	   is for all the regions/devices
	*/
	unsigned		minor_number_count = AULDRV_REGION_QTY + 1;

	dev_t			driver_number;
	struct class *  driver_class;

	unsigned		minor;
	unsigned		index_device;

	struct device * dev_ret;

	PDEBUG( "%s entry.\n", __func__ );

	/* Acquire driver major number and allocate minor numbers.  The auldrv_requested_driver_major_number
	 *  is typically set to zero indicating that we are requesting a major number be assigned.
	 */
	error_code = AulDrv_Initialize_Driver_Number(	auldrv_requested_driver_major_number,  /* typically 0 */
													&driver_number,		 /* out-bound driver number */
													minor_number_count,  /* number of boards + 1    */
													auldrv_module_name   /* the name of this driver */
												);
	if ( error_code ) goto AulDrv_Initialize_Error;

/*
PDEBUG( "%s: CMD_AULDRV_DEVICE_DEFINITION_AVAILABLE   = %d.\n", __func__, CMD_AULDRV_DEVICE_DEFINITION_AVAILABLE );
PDEBUG( "%s: CMD_AULDRV_DEVICE_DEFINITION_GET = %d.\n", __func__, CMD_AULDRV_DEVICE_DEFINITION_GET );
PDEBUG( "%s: CMD_AULDRV_VERSION              = %d.\n", __func__, CMD_AULDRV_VERSION );
*/

PDEBUG( "%s: AulDrv_Initialize_Driver_Number complete.\n", __func__ );
PDEBUG( "%s:  major = %d.\n", __func__, MAJOR(driver_number) );
PDEBUG( "%s:  minor = %d.\n", __func__, MINOR(driver_number) );

	/* create the driver controller ("auldrv") class  */
	driver_class = class_create( THIS_MODULE, auldrv_module_name );
	if ( NULL == driver_class )
	{	error_code = ENODEV;		goto AulDrv_Initialize_Error;	}

PDEBUG( "%s: class_create complete.\n", __func__ );

	/* Initialize the driver/controller and the devices/regions */
	error_code = AulDrv_Initialize_Dataset( &auldrv, driver_class, driver_number, &auldrv_file_operations );
	if ( error_code ) goto AulDrv_Initialize_Error;

PDEBUG( "%s: AulDrv_Initialize_Dataset complete.\n", __func__ );

	/* Initialize I/O behavior.  For Linux this will be either io_linux_drv_gen_x86 or
	 *  io_linux_lib_sim, depending on what is compiled based on the make file.  This
	 *  allows us great flexibility in terms of how data will flow to/from the hardware.
	 */
	error_code = IO_Initialize();
	if ( error_code ) goto AulDrv_Initialize_Error;

	PDEBUG( "%s exit success.\n", __func__ );

	return SUCCESS;
AulDrv_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	PDEBUG( "%s fail, error = %d.\n", __func__, error_code );
	AulDrv_Terminate_AulDrv_Dataset( &auldrv );
	return -error_code;
}

/***************************************************************
* The cleanup_module()
*/
/**************************************************************************//**
@ingroup auldrv
@brief
The main termination function used during installation of the driver as specified by module_exit().

@return nothing
 **/
void AulDrv_Terminate( void )
{
	PDEBUG( "%s entry.\n", __func__ );
	AulDrv_Terminate_AulDrv_Dataset( &auldrv );
	IO_Terminate();
	PDEBUG( "%s exit success.\n", __func__ );
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*   < < < MODULE DEFINITIONS > > > 	  													*/
/******************************************************************************/

module_init(AulDrv_Initialize);
module_exit(AulDrv_Terminate);

MODULE_AUTHOR( "customer.service@apexembeddedsystems.com" );
MODULE_DESCRIPTION("AES Generic Kernel Driver");
//TODO: MODULE_VERSION();
MODULE_LICENSE("GPL");  /* for now */
