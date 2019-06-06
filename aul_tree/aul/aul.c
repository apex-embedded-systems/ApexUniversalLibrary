/********************************************************************************
_____________________________________________________________________________  
Description:

Library layer API Interface.
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

#include "aul.h"
#include "aul_shared.h"
#include "aul_hwdef.h"
#include "../shared/debug.h"
#include "../shared/device.h"
#include "../shared/status.h"
#include "../shared/error.h"
#include "../shared/mem.h"
#include "../shared/cmd.h"
#include "../shared/module_shared.h"
#include "../shared/ts_ct.h"
#include "../str_safe/str_safe.h"
#include "../io/io.h"

#ifdef _WIN32
# include <windows.h>  /* GetProcessID */
#else
# include <sys/types.h>
# include <unistd.h>	/* getpid()		*/
#endif


/*******************************************************************/
#define AUL_MODULE_REV  "$Date: 2015-08-04 13:59:08 -0500 (Tue, 04 Aug 2015) $"

/*********************************************************************************/
/* if DEBUG then allow AUL_DBG.   */
#define AUL_DBG		   _DEBUG
/*********************************************************************************/
#define AUL_DEBUG_LOG   DEBUG_OUT

/*********************************************************************************/
/*store definition here?
retrieve details from driver, if avail*/

static devmgr_device_type * aul_device[DEVICE_QTY];
/*for linux we would be retaining the file descriptor.*/

/*********************************************************************************/
static int AUL_Device_Available( void )
{
	int index;
	for ( index = 0; index < DEVICE_QTY; index++ )
	{
		if ( NULL == aul_device[index] ) return index;
	}
	return -1;
}
/*********************************************************************************/
static int AUL_Device_IsDuplicate( io_definition_type * io_definition )
{
	int index;
	for ( index = 0; index < DEVICE_QTY; index++ )
	{
		if ( NULL != aul_device[index] )
		{
			if ( ( io_definition->physical_id  == aul_device[index]->physical_id  ) &&
				  ( io_definition->minor_number == aul_device[index]->minor_number )  ) 
				return 1;
		}
	}
	return 0;
}
/*********************************************************************************/
#if defined( _WIN32 )
	static char aul_lib_full_name[256] = MODULE_AUL_NAME "_lib_dll.dll\0\0\0\0";
#else
# if   defined( AUL_LIBRARY_SHARED_DRIVER_DEBUG		)
	static const char aul_lib_full_name[] = "lib" MODULE_AUL_NAME ".so";
# elif defined( AUL_LIBRARY_SHARED_SIMULATOR_DEBUG	)
	static const char aul_lib_full_name[] = "lib" MODULE_AUL_NAME ".so";
# elif defined( AUL_LIBRARY_SHARED_DRIVER_RELEASE    )
	static const char aul_lib_full_name[] = "lib" MODULE_AUL_NAME ".so";
# elif defined( AUL_LIBRARY_SHARED_SIMULATOR_RELEASE )
	static const char aul_lib_full_name[] = "lib" MODULE_AUL_NAME ".so";
# elif defined( AUL_LIBRARY_STATIC_DEBUG				)
	static const char aul_lib_full_name[] = "lib" MODULE_AUL_NAME "debug.a";
# elif defined( AUL_LIBRARY_STATIC_RELEASE			)
	static const char aul_lib_full_name[] = "lib" MODULE_AUL_NAME ".a";
# else
	static const char aul_lib_full_name[] = "lib" MODULE_AUL_NAME ".a";
# endif
#endif
/*********************************************************************************/
#define AUL_DRV_FULL_NAME_LENGTH 32
/*TODO: this ought to be from auldrv.h */
static char aul_drv_full_name[AUL_DRV_FULL_NAME_LENGTH] = "unknown";

int Dll_Filename_Get( char * file_name, size_t size );
/*********************************************************************************/
int AUL_Board( int board, devmgr_device_type ** device )
{
	int error_code;
	if ( ( board >= DEVICE_QTY			) ||
		 ( board < 0					)  )
	{	error_code = EC_AUL_BOARD_RANGE;  goto AUL_Board_IsNot_Valid_Error;	}

	if ( NULL == aul_device[board]	)
	{	error_code = EC_AUL_BOARD_DATASET_NULL;  goto AUL_Board_IsNot_Valid_Error;	}

	if ( aul_device[board]->open_and_active )
	{
		if ( NULL != device ) *device = aul_device[board];
	}
	else
	{	error_code = EC_AUL_BOARD_NOT_OPEN;  goto AUL_Board_IsNot_Valid_Error;	}
	return SUCCESS;
AUL_Board_IsNot_Valid_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*********************************************************************************/
devmgr_device_type * AUL_Device_Get( int board )
{
	return aul_device[board];
}
/*********************************************************************************/
int AUL_Device_To_Index( devmgr_device_type * device, int * board )
{
	int index;
	for ( index = 0; index < DEVICE_QTY; index++ )
	{
		if ( device == aul_device[index] )
		{
			*board = index;
			return SUCCESS;
		}
	}
	return -EC_AUL_DEVICE_NOT_LISTED;
}

/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function to close access to an already opened device (board).

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Device_Close( int board )
{
	int error_code;

	/* if board is not in range post error */
	if ( ( board >= DEVICE_QTY			) ||
	     ( board < 0					)  )
	{	error_code = EC_AUL_BOARD_RANGE;  goto AUL_Device_Close_Error;	}
	/* nothing to do, already done */
	if ( NULL == aul_device[board])  return SUCCESS;

	error_code = AUL_Board( board, NULL );
	if ( error_code ) goto AUL_Device_Close_Error;
	/* close the device */
	error_code = DevMgr_Close( &(aul_device[board]) );
	if ( error_code ) goto AUL_Device_Close_Error;
	return SUCCESS;
AUL_Device_Close_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function to allow access to a device (board). Up to four devices can be opened at any
given time. The function initializes and configures all underlying information with respect to the open request.
The 'device_acronym_name' must match the node name (see /dev folder in Linux) in the file system.

\param[in] device_mnemonic_name  This is the device's (board's) acronym defined during the driver installation (i.e. insmod).
                                 The same name as the node name found in /dev in Linux folder.
\param[out] board If successful, the value returned is a descriptor used for the purpose of subsequent access to the device. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Device_Open( const char * device_acronym_name, int * board )
{
	int		error_code;
	int		board_index;
/*	void *	device;
	IOC_T		device_logical_id;*/
	int		driver_override;
	device_definition_type * device_definition;
	io_definition_type   io_definition_hwdef;
	io_definition_type   io_definition_driver;
#if defined( MODULE_AULHWDEF )
	void * hwdef;
#endif
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	error_code  = SUCCESS;
	board_index = AUL_Device_Available();
	if ( board_index < 0 )
	{ error_code = EC_AUL_DEVICE_POINTER;	goto AUL_Device_Open_Error; }

	Mem_Set( &io_definition_hwdef,  0, sizeof( io_definition_hwdef  ) );
	Mem_Set( &io_definition_driver, 0, sizeof( io_definition_driver ) );
	/* Must copy the string into place so that the io_definition can be copied.
		The error_code returns actual number of characters copied.
	*/
	error_code = String_Copy( io_definition_driver.acronym, device_acronym_name, IO_DEFINITION_ACRONYM_SIZE );
	if ( ( IO_DEFINITION_ACRONYM_SIZE == error_code ) || ( 1 >= error_code ) )
	{	error_code = EC_AUL_ACRONYM_SIZE;  goto AUL_Device_Open_Error; }

	/* highest priority */
	driver_override = 0;
	if ( !IO_Is_Definition_Available() )
	{ /* Try to acquire from driver -- driver will use acronym 
			followed by minor_number which will just default to zero.
			At this point all we have to work with is the acronym.
		*/
		error_code = IO_Definition_Get( &io_definition_driver ); 
		if ( error_code ) goto AUL_Device_Open_Error;

		if ( 0 == String_Compare( io_definition_driver.acronym, device_acronym_name, IO_DEFINITION_ACRONYM_SIZE ) )
		{
			driver_override = 1;
		}
	}

	if ( AULHwDef_List_Is_Populated() )
	{ /* Use I/O parameters found in the hardware definition file. 
			We will use acronym to find.  The acronym must match the
			file node name as well.  We want that to be the case so
			that things are consistent across all platforms.
	  */
		error_code = AULHwDef_Acronym_Find( device_acronym_name, &(io_definition_hwdef.minor_number), &hwdef );
		if ( error_code ) goto AUL_Device_Open_Error;
		/* we must have a match, so copy it to destination */
		String_Copy( io_definition_hwdef.acronym, device_acronym_name, IO_DEFINITION_ACRONYM_SIZE );
		error_code = AULHwDef_Parameters_Get(	hwdef,
												&(io_definition_hwdef.physical_id),  /* actually logical id */
												&(io_definition_hwdef.region_offset),
												&(io_definition_hwdef.region_bytes),
												&(io_definition_hwdef.interrupt_number),
												&(io_definition_hwdef.region_restrict_8bit)
											);
		if ( error_code ) goto AUL_Device_Open_Error;
#ifdef _WIN32 
		/* reconcile the driver and hardware definition list */
		if ( driver_override )
		{
			if ( 0 == io_definition_driver.region_offset         ) 
			{	io_definition_driver.region_offset			= io_definition_hwdef.region_offset;		}
			if ( 0 == io_definition_driver.region_bytes          ) 
			{	io_definition_driver.region_bytes			= io_definition_hwdef.region_bytes;			}
			if ( 0 == io_definition_driver.region_restrict_8bit  ) 
			{	io_definition_driver.region_restrict_8bit	= io_definition_hwdef.region_restrict_8bit;	}
			if ( 0 == io_definition_driver.interrupt_number  ) 
			{	io_definition_driver.interrupt_number		= io_definition_hwdef.interrupt_number;		}
			if ( 0 == io_definition_driver.physical_id  ) 
			{	io_definition_driver.physical_id			= io_definition_hwdef.physical_id;			}
		}
		else
		{ /* hardware definition file is all we got to go on now.  */
			Mem_Copy( &io_definition_driver, &io_definition_hwdef, sizeof( io_definition_driver ) );
		}
#else
		error_code = EC_AUL_INCOMPLETE_CODE;	goto AUL_Device_Open_Error;
#endif
	}

#if(0)
#if defined( MODULE_AULHWDEF )
	if ( !AULHwDef_List_Is_Populated() )
	{ /* attempt to obtain I/O parameters from driver */
		if ( !IO_Is_Definition_Available() )
		{ /* Try to acquire from driver -- driver will use acronym 
				followed by minor_number which will just default to zero.
				At this point all we have to work with is the acronym.
		  */
			error_code = IO_Definition_Get( &io_definition_local ); 
			if ( error_code ) goto AUL_Device_Open_Error;
		}
	}
	else
	{ /* Use I/O parameters found in the hardware definition file. 
			We will use acronym to find.  The acronym must match the
			file node name as well.  We want that to be the case so
			that things are consistent across all platforms.
	  */
		error_code = AULHwDef_Acronym_Find( device_acronym_name, &(io_definition_local.minor_number), &hwdef );
		if ( error_code ) goto AUL_Device_Open_Error;

		error_code = AULHwDef_Parameters_Get(	hwdef,
												&(io_definition_local.physical_id),  /* actually logical id */
												&(io_definition_local.region_offset),
												&(io_definition_local.region_bytes),
												&(io_definition_local.interrupt_number),
												&(io_definition_local.region_restrict_8bit)
											);
		if ( error_code ) goto AUL_Device_Open_Error;
	}
#else
	if ( !IO_Is_Definition_Available() )
	{ /* Try to acquire from driver -- driver will use acronym 
			followed by minor_number which will just default to zero.
			At this point all we have to work with is the acronym.

			This must return physical_id, if it is zero.
		*/
		error_code = IO_Definition_Get( &io_definition_local ); 
		if ( error_code ) goto AUL_Device_Open_Error;
	}
#endif
#endif
/*
	 fix up to include instance -- may or may not be done already, just to be sure.
	io_definition_local.physical_id = IOC_LOGICAL_TO_PHYSICAL_ID( io_definition_local.physical_id, io_definition_local.minor_number );
*/

	/* determine if the device is supported in our device list */
	error_code = Device_Available_Find( io_definition_driver.physical_id, &device_definition );
	if ( error_code )
	{	error_code = EC_AUL_DEVICE_NOT_LISTED;	goto AUL_Device_Open_Error;	}

	/* look for any duplicates.  */
	if ( AUL_Device_IsDuplicate( &io_definition_driver ) )
	{	error_code = EC_AUL_DUPLICATE_DEVICE;	goto AUL_Device_Open_Error;	}
	
#if(0)
	/* fix up any items that are not defined -- using device definition defaults */
	if ( 0 == io_definition_driver.region_bytes )
	{
		io_definition_local.region_bytes = device_definition->io_definition.region_bytes;
	}
	if ( 0 == io_definition_local.region_offset )
	{
		io_definition_local.region_offset = device_definition->io_definition.region_offset;
	}
#endif

	/* Open the device allocating and assigning parameters */
	error_code = DevMgr_Open(   &(aul_device[board_index]),
								&io_definition_driver,
								device_definition
						    );
	if ( error_code ) goto AUL_Device_Open_Error;

	*board = board_index;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
AUL_Device_Open_Error:
	if ( board_index >= 0 ) aul_device[board_index] = NULL;
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function that returns a string describing the last error that was produced.

\return Human readable string describing last error encountered.
*/
AUL_DLL_API const char * AULCALLCONV AUL_Error_Last_Get( void )
{
	int          error_code;
	const char * message;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	Error_Last( &error_code, &message );
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return message;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function to retrieve human readable string associated with passed error code.

\param[in] error_code The error code used to look up error string.  Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
\return Human readable string for the passed in error code
*/
AUL_DLL_API const char * AULCALLCONV AUL_Error_String( int error_code )
{
	return Status_EC_String( error_code );
}
/**********************************************************************************************************//**
 * @ingroup general_libarary
 * \brief
 * Apex Universal Library function used to initialize the library and establish connectivity with the kernel driver.
 *
 * The hardware definition file is only used for special situations and normally is passed as a NULL.
 *
 * \param[in] hardware_definition_file_name  The value passed shall be a NULL.
 * \return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Initialization( const char* hardware_definition_file_name )
{
/*
TODO: Build a list of devices along with parameters.
		If Linux, then obtain list from driver.
		If Jungo (Win32), then IO_Initialize() 
*/
	int error_code;
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "//--------------------------------------------\n" );
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif

	if ( NULL != hardware_definition_file_name )
	{
		if ( String_Length( hardware_definition_file_name ) > 0 )
		{
			error_code = AULHwDef_Device_Populate_Definitions( hardware_definition_file_name );
			if ( error_code ) goto AUL_Initialization_Error;
		}
	}

	error_code = Module_Initialize();
	if ( error_code ) goto AUL_Initialization_Error;

#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
AUL_Initialization_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function to test for the type of logical device the 'board' is.

\param[in] board   The descriptor obtained from the function ::AUL_Device_Open.
\param[in] logical The logical device ID
\return An error code of type int is returned. If the board IS the same logical device then a '1' is returned, otherwise '0'.
        If an error has occurred, then a value less than zero is returned.
*/
AUL_DLL_API int AUL_Is_Logical_Device( int board, DEVICE_LOGICAL_ENUM logical )
{
	int					 error_code;
	devmgr_device_type * d; 	/* short hand for device */

	d = NULL;
	if ( SUCCESS != ( error_code = AUL_Board( board, &d ) ) ) goto AUL_Is_Logical_Device_Error;
	/* test validity of the device */
	if ( NULL == d )
	{	error_code = EC_AUL_DEVICE_NULL_POINTER;	goto AUL_Is_Logical_Device_Error;	}

	if ( IOC_GET_LOGICAL_ID_DEVICE( d->physical_id ) == logical ) return 1;

	return 0;
AUL_Is_Logical_Device_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief


\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] command
\param[in] argument

\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Ioctl( int				board, 
									   unsigned int		command, 
									   unsigned long	argument 
								     )
{
	int		error_code;
/*	size_t	type;*/
	size_t	cmd;
	size_t	cmd_set;
	size_t	index; /* or channel */
	/*size_t	instance;*/
	IOC_T		physical_id; /* NOTE: modules and devices: logical_id is the same as physical_id */
	devmgr_device_type * d; 	/* short hand for device */

#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
/*
Order of communication:	
1. lib		
2. driver	
3. device	
4. component
*/
	/* If intended for driver top level, then pass it along. */
	if ( AULDRV_LOGICAL_ID == IOC_GET_LOGICAL_ID_MODULE( command ) )
	{
		error_code = IO_Ioctl( board, command, argument );
		goto AUL_Ioctl_Finish;
	}

	if ( SUCCESS != ( error_code = AUL_Board( board, &d ) ) ) goto AUL_Ioctl_Error;
	cmd_set = 0;
	cmd     = 0;
	/* test for validity of the command */
	if ( IOC_MAGIC_IS_VALID_LIB(command) ) 
	{
		cmd_set = 1;
		cmd	  = IOC_GET_COMMAND( command );
	}
	else if ( IOC_MAGIC_IS_VALID( command ) )
	{ /* pass onto driver */
		error_code = IO_Ioctl( aul_device[board]->io_device, command, argument );
		if ( error_code ) goto AUL_Ioctl_Error;
	}
	else
	{	error_code = EC_AUL_IOCTL_COMMAND;	goto AUL_Ioctl_Error;	}
	/* test validity of the device */
	if ( NULL == d )
	{	error_code = EC_AUL_DEVICE_NULL_POINTER;	goto AUL_Ioctl_Error;	}
	if ( 0 == d->open_and_active ) 
	{	error_code = EC_AUL_DEVICE_NOT_OPEN;		goto AUL_Ioctl_Error;	}
	/* obtain information from command */
	physical_id	= IOC_GET_PHYSICAL_ID( command );
	index       = IOC_GET_INDEX( command );
	/*instance    = IOC_GET_INSTANCE( command );*/

	if ( IOC_VALID_LOGICAL_ID_MODULE( physical_id /* OK since only important parts tested */ ) )
	{ /* this is a module command */
		if ( IO_LOGICAL_ID == physical_id )
		{
			error_code = IO_Ioctl( aul_device[board]->io_device, command, argument );
			if ( error_code ) goto AUL_Ioctl_Error;
		}
		else
		{	error_code = EC_AUL_IOCTL_COMMAND;	goto AUL_Ioctl_Error;	}
	}
	else if ( IOC_VALID_LOGICAL_ID_DEVICE( physical_id /* OK since only import parts tested */ ) )
	{ /* this is a device command */
		if ( !cmd_set ) {	error_code = EC_AUL_IOCTL_COMMAND;	goto AUL_Ioctl_Error;	}
		/* error_code = ( * d->definition->jump_table[cmd] )( d, index, (char *) argument, 0 ); */ 
		error_code = DevMgr_Command( d, cmd, index, ( char *) argument, 0 );
	}
	else
	{ /* component command -- assume so, anyway */
		int					cmp_table_index;
		component_type * 	component; /* short hand */
		component_type * 	cmp_table;
/*TODO: add debugging macro around this: #if defined()...*/
		if ( !cmd_set )						{	error_code = EC_AUL_IOCTL_COMMAND;		goto AUL_Ioctl_Error;	}
		if ( NULL == d->component_table )	{ error_code = EC_AUL_IOCTL_COMPONENT;		goto AUL_Ioctl_Error;	}
/*end*/
		cmp_table = d->component_table;
		/* must search to find the component */
		cmp_table_index = CmpMgr_Component_Match_Physical_ID( cmp_table, physical_id );	
		if ( CMPMGR_EC_FAILURE == cmp_table_index )
		{ error_code = EC_AUL_IOCTL_COMPONENT;		goto AUL_Ioctl_Error;	}
		component	= &( cmp_table[cmp_table_index] );
		error_code  = CmpMgr_Command( component,  /* info about the component			*/
									  cmd,			/* component command for jump table	*/
									  index,		/* channel within component			*/
									  (char *) argument, 
									  0 
									);
	}
AUL_Ioctl_Finish:
	if ( error_code ) goto AUL_Ioctl_Error;  /* resulting from device or component error */
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
AUL_Ioctl_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}


/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function that returns a string describing the last error that was produced.

\deprecated For best results use ::AUL_Error_String( int error_code ) instead. As an alternative ::AUL_Error_Last_Get can also be used.

\return Human readable string describing last error encountered
*/
/* Deprecate */
AUL_DLL_API const char * AULCALLCONV AUL_Last_Error_Get( void )
{
	int          error_code;
	const char * message;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	Error_Last( &error_code, &message );
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return message;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function that returns a string describing the file name of the AUL library

\return Human readable string describing AUL library name
*/
AUL_DLL_API const char * AULCALLCONV AUL_Library_Name( void )
{
#if defined( _WIN32 )
	size_t length;
	length = Dll_Filename_Get( aul_lib_full_name, sizeof( aul_lib_full_name ) );
	return aul_lib_full_name;
#else
	return aul_lib_full_name;
#endif
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function that returns a string describing the driver name


\return Human readable string describing driver name
*/
AUL_DLL_API const char * AULCALLCONV AUL_Driver_Name( void )
{
	int error_code;
	error_code = AUL_Ioctl( -1 /* controller */, CMD_AULDRV_NAME, (unsigned long) aul_drv_full_name );
	if ( error_code ) return NULL;
	return( (const char *) aul_drv_full_name);
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function returns the compilation revision of the library. The revision has now been upgraded to be the latest compiled date in the form YYMMDDHHmm, where:<br />
YY = year<br />
MM = month<br />
DD = day of month<br />
HH = hour of the day<br />
mm = minute of the hour.<br />
The compiled date is now effectively a unique serial number. 

\param[out] revision  returns a long value indicating the revision of the library.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Library_Revision( unsigned long * revision )
{
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	*revision = (unsigned long) CTTS_YYMMDDHH_DEC;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function returns the SVN revision of the underlying driver Apex Universal Library.SYS file.  In this
case the revision returned is the SVN date-time stamp.

\param[out] revision  returns a long value indicating the revision of the library.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Driver_Revision( unsigned long * revision )
{
	uint32_t rev;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	IO_Version( &rev );
	*revision = (unsigned long) rev;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function returns the revision of the underlying driver Apex Universal Library.SYS file.

\deprecated Use ::AUL_Driver_Revision instead

\param[out] revision  returns a long value indicating the revision of the library.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
/* Deprecate */
AUL_DLL_API int AULCALLCONV AUL_Revision_Sys_File( unsigned long * revision )
{
	uint32_t rev;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	IO_Version( &rev );
	*revision = (unsigned long) rev;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function that returns

\param[out] debug_is_active  returns a zero if driver debug is not active, use this to warn users of potential reduced throughput resulting from excessive debug posting.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Driver_Is_Debug_Active( int * debug_is_active )
{
	int error_code;
	unsigned long active;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	error_code = AUL_Ioctl( -1 /* controller */, CMD_AULDRV_DEBUG_ACTIVE_GET, (unsigned long) &active );
	if ( error_code ) goto AUL_Driver_Is_Debug_Active_Error;
	*debug_is_active = (int) active;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
AUL_Driver_Is_Debug_Active_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function that returns

\param[out] debug_is_active  returns a zero if driver debug is not active, use this to warn users of potential reduced throughput resulting from excessive debug posting.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Library_Is_Debug_Active( int * debug_is_active )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
#ifdef _DEBUG
	*debug_is_active = 1;
#else
	*debug_is_active = 0;
#endif
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
}
AUL_DLL_API int AULCALLCONV AUL_Library_Process_Id_Get( unsigned long * pid )
{
#ifdef _WIN32
	*pid = (unsigned long) GetCurrentProcessId();
#else
	*pid = (unsigned long) getpid();
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup general_libarary
\brief
Apex Universal Library function that returns

\param[out] device_list
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Device_List_Get( device_list_type * device_list )
{
	int error_code;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	error_code = AUL_Ioctl( -1 /* controller */, CMD_AULDRV_DEVICE_LIST_GET, (unsigned long) device_list );
	if ( error_code < 0 ) error_code = -error_code;
	if ( EC_IO_WARN_DEVICE_LIST_UNKNOWN == error_code )
	{ /* retrieve the information from hardware definition file */
		size_t index;
		Mem_Set( device_list, 0, sizeof( *device_list ) );
		device_list->type = SYS_TYPE_MAKE( device_list_type );
		for ( index = 0; index < DEVICE_QTY; index++ )
		{ /* add in the open and active status */
			error_code = AULHwDef_Parameters_Using_Index_Get(	index,
										device_list->region[index].acronym,
										&(device_list->region[index].physical_id),  /* actually logical id */
										&(device_list->region[index].region_offset),
										&(device_list->region[index].region_bytes),
										&(device_list->region[index].interrupt_number),
										&(device_list->region[index].region_restrict_8bit)
									);
			if ( error_code ) break;
			device_list->region_count++;
		}
		error_code = EC_IO_WARN_DEVICE_LIST_PARTIAL; /* to perform subsequent if-then statement below */
	}
	if ( SYS_TYPE_MAKE( device_list_type ) != device_list->type )
	{	error_code = EC_AUL_DATA_TYPE_ERROR;	goto AUL_Device_List_Get_Error;	}
	if ( EC_IO_WARN_DEVICE_LIST_PARTIAL == error_code )
	{ /* need to add open status information */
		int index, count;
		for ( index = 0; index < DEVICE_QTY; index++ )
		{ /* add in the open and active status */
			if ( NULL != aul_device[index] )
			{ /* loop through io_definitions to find a match */
				for ( count = 0; count < device_list->region_count; count++ )
				{
					if ( aul_device[index]->physical_id == device_list->region[count].physical_id )
					{ /* we have a match -- now assign library open status */
						device_list->open_and_active[count] = aul_device[index]->open_and_active;
						break;
					}
				}
			}
		}
		error_code = SUCCESS;
	}
	if ( error_code ) goto AUL_Device_List_Get_Error;
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return error_code;
AUL_Device_List_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
AUL_DLL_API int AULCALLCONV AUL_Device_Logical_Get( const char * device_acronym_name, DEVICE_LOGICAL_ENUM * logical_id )
{ /* NOTICE: this code is EXTREMELY similar to the AUL_Device_Open().  TODO: merge/reduce */
	int					error_code;
	device_definition_type * device_definition;
	io_definition_type	io_definition;
#if defined( MODULE_AULHWDEF )
	void * hwdef;
#endif
#if defined( IO_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif
	Mem_Set( &io_definition,  0, sizeof( io_definition ) );
	error_code = String_Copy( io_definition.acronym, device_acronym_name, IO_DEFINITION_ACRONYM_SIZE );
	if ( ( IO_DEFINITION_ACRONYM_SIZE == error_code ) || ( 1 >= error_code ) )
	{	error_code = EC_AUL_ACRONYM_SIZE;  goto AUL_Device_Logical_Get_Error; }

	if ( !IO_Is_Definition_Available() )
	{
		error_code = IO_Definition_Get( &io_definition ); /* returns data based on acronym */
		if ( error_code ) goto AUL_Device_Logical_Get_Error;

		if ( 0 == String_Compare( io_definition.acronym, device_acronym_name, IO_DEFINITION_ACRONYM_SIZE ) )
		{
			goto AUL_Device_Logical_Get_Finish;
		}
	}
	else if ( AULHwDef_List_Is_Populated() )
	{ /* Use I/O parameters found in the hardware definition file. 
			We will use acronym to find.  The acronym must match the
			file node name as well.  We want that to be the case so
			that things are consistent across all platforms.
	  */
		error_code = AULHwDef_Acronym_Find( device_acronym_name, &(io_definition.minor_number), &hwdef );
		if ( error_code ) goto AUL_Device_Logical_Get_Error;
		/* we must have a match, so copy it to destination */
		error_code = AULHwDef_Parameters_Get(	hwdef,
												&(io_definition.physical_id),  /* actually logical id */
												&(io_definition.region_offset),
												&(io_definition.region_bytes),
												&(io_definition.interrupt_number),
												&(io_definition.region_restrict_8bit)
											);
		if ( error_code ) goto AUL_Device_Logical_Get_Error;
/*		error_code = EC_AUL_INCOMPLETE_CODE;	goto AUL_Device_Logical_Get_Error;  */
	}
	else
	{	error_code = EC_AUL_DEVICE_NOT_LISTED;	goto AUL_Device_Logical_Get_Error;	}
AUL_Device_Logical_Get_Finish:
	/* determine if the device is supported in our device list */
	error_code = Device_Available_Find( io_definition.physical_id, &device_definition );
	if ( error_code )
	{	error_code = EC_AUL_DEVICE_NOT_LISTED;	goto AUL_Device_Logical_Get_Error;	}
	/* look for any duplicates.  */
	if ( AUL_Device_IsDuplicate( &io_definition ) )
	{	error_code = EC_AUL_DUPLICATE_DEVICE;	goto AUL_Device_Logical_Get_Error;	}
	*logical_id = (DEVICE_LOGICAL_ENUM)( IOC_GET_LOGICAL_ID_DEVICE( io_definition.physical_id ) );
	return error_code;
AUL_Device_Logical_Get_Error:
	*logical_id = DEVICE_LOGICAL_UNDEFINED;
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}

/*********************************************************************************/
AUL_DLL_API int AULCALLCONV AUL_Termination( void )
{
	int error_code;
	int board;
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: entry\n", __func__ );
#endif

	/* Close out any open devices here */
	for ( board = 0; board < DEVICE_QTY; board++ )
	{
		if ( NULL != aul_device[board] )
		{
			if ( aul_device[board]->open_and_active )
			{
				error_code = AUL_Device_Close( board );
			}
		}
	}
	error_code = Module_Terminate();
	if ( error_code ) goto AUL_Termination_Error;

#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit success\n", __func__ );
#endif
	return SUCCESS;
AUL_Termination_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_NAME, __func__, __LINE__, error_code );
#if defined( AUL_DEBUG_LOG )
	Debug_Log( AUL_DEBUG_LOG "%s: exit ec=%s\n", __func__, Status_EC_String( error_code ) );
#endif
	return -error_code;
}
/*********************************************************************************/
module_definition_type module_aul_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	NULL,
	NULL,
	MODULE_AUL_NAME,
	AUL_MODULE_REV,
	NULL,
	AUL_STATUS_REV
};

