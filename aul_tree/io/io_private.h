#ifndef IO_PRIVATE_H_
#define IO_PRIVATE_H_
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
#include "io.h"
#include "../shared/mem.h"
#include "../shared/debug.h"      /* debugging support						*/
#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/cmd_macros.h"  /* IOC_T */
#include "../shared/compiler.h"
#include "../shared/module_macros.h"

/*******************************************************************************/
#define IO_PRIVATE_H_REV  "$Date: 2014-06-11 12:50:10 -0500 (Wed, 11 Jun 2014) $"
/********************************************************************************
unsigned long copy_to_user (void __user * to, const void * from, unsigned long n);
NOTE - The "( Mem_Copy(), 0 )" means that it will return a zero or in other words
       the last item in the comma list is returned.
*/
#ifdef __KERNEL__
# include <linux/uaccess.h>		/* copy_(to,from)_user and access_ok()	*/
/*#include <asm-generic/uaccess.h>*/
# define IO_COPY_TO_USER(to,from,n)  copy_to_user( (void __user *) to, (void *) from, n )
# define IO_COPY_FROM_USER(to,from,n)  copy_from_user( to, from, n )
# pragma message ( "aul_tree: io_private.h  using kernel copy_to/from_user" )
#else
# define IO_COPY_TO_USER(to,from,n)   ( Mem_Copy( (void *) (to), (void *) (from), (n) ), 0 )
# define IO_COPY_FROM_USER(to,from,n) ( Mem_Copy( (void *) (to), (void *) (from), (n) ), 0 )
# pragma message ( "aul_tree: io_private.h  using Mem_Copy" )
#endif

/*********************************************************************************/
#define IO_TARGET_NAME_EXPAND(name) name
#define IO_TARGET_NAME_SUFFIX(name,suffix) { IO_TARGET_NAME_EXPAND(name) "_" suffix }
/*********************************************************************************/
struct io_device_dataset 
{
	io_definition_type		io_definition;
	size_t					active;
	size_t					bus_restrict_8bit;
	void *					private_data; /* reserved for specific io_?? file targets */
	int						iod;
	int						error_last;
};
/*******************************************************************************/

#if defined( IO_DEBUG )
typedef int ( * io_validity_fnc)( int iod, size_t virtual_address, size_t data_bytes ); 
#endif
typedef int ( * io_open_fnc  )( struct io_device_dataset * dataset );
typedef int ( * io_close_fnc )( struct io_device_dataset * dataset );
typedef int ( * io_intr_en_fnc )( struct io_device_dataset * dataset, io_int_handler handler, void * private_data );
typedef int ( * io_intr_dis_fnc )( struct io_device_dataset * dataset );
typedef int ( * io_ioctl_fnc )( struct io_device_dataset * dataset, unsigned int command, unsigned long argument );
typedef int ( * io_def_avail_fnc )( void );
typedef int ( * io_def_get_fnc )( struct io_definition * io_definition );
/*******************************************************************************/
struct io_target
{
	const SYS_TYPE		type;
	module_fnc			initialize;
	module_fnc			terminate;
	io_open_fnc			open;
	io_close_fnc		close;
	io_ioctl_fnc		ioctl;
#ifdef IO_CAPTURE_RELEASE
	io_capture_fnc		device_capture;
	io_release_fnc		device_release;
#endif
	io_intr_en_fnc		interrupt_enable;
	io_intr_dis_fnc		interrupt_disable;
	io_def_avail_fnc	definition_available;
	io_def_get_fnc		definition_get;
#if defined( IO_DEBUG )
	io_validity_fnc	valid_not;
#endif
	const char *		name;
	const char *		svn_module_date;
	const char *		svn_command_date;
	const char *		svn_status_date;
};
typedef struct io_target io_target_type;
/*******************************************************************************/

/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************/

/** Internal use only */
int IO_Is_Restrict_8Bit( int iod );

void IO_Error_Set( int error_code );

#if defined( IO_DEBUG )
 int IO_IsNotValid( int iod, size_t virtual_address, size_t data_bytes );
#endif

extern io_target_type			io_target_definition;

/*******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* IO_PRIVATE_H_ */

