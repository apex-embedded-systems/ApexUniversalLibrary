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

#include "dev_stx104.h"
#include "../../shared/module.h"
#include "../../shared/status.h"
#include "../../shared/compiler.h"  /* inline */
#include "../../shared/callconv.h"
#include "../../aul/aul_shared.h"
#include "../../io/io.h"
#include "../../io/io_private.h"
#include "../../io/io_shared.h"
#include "dev_stx104_index.h"
#include "../../components/indirect/cmp_indirect.h"


/*******************************************************************/
#define STX104_MODULE_REV "$Date: 2016-09-30 14:48:43 -0500 (Fri, 30 Sep 2016) $"

/*******************************************************************/
const char * STX104_MODULE_SVN_Time_Stamp( void ) { return STX104_MODULE_REV; }
/*********************************************************************************/
/* if DEBUG then allow IO_DEBUG */
#define STX104_DEBUG			_DEBUG
/*********************************************************************************/
#define STX104_DEBUG_LOG	DEBUG_OUT

/*****************************************************************************/






/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
static int stx104_component_list[STX104_COMPONENT_QTY] = { { 0 } };
static int stx104_component_count = 0;

/*****************************************************************************/
static int STX104_CMD_NOP( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
#if defined( STX104_DEBUG_LOG )
	Debug_Log( STX104_DEBUG_LOG "%s\n", __func__ );
#endif
	return SUCCESS;
}
/*****************************************************************************/
static int STX104_CMD_Initialize( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
/*
acquire io-space maps and such.
allocate device data sets?

	device->definition->jump_table[CMD_STX104_INITIALIZE]( device, index, buf, len );
*/

	return SUCCESS;
}
/*****************************************************************************/
static int STX104_CMD_Terminate( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}
/*****************************************************************************/
static int STX104_CMD_Power_Up( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}
/*****************************************************************************/
static int STX104_CMD_Power_Down( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}
/*****************************************************************************/
static void STX104_Free_Memory(devmgr_device_type * device )
{
	/* NOTE: Mem_Free will null the pointer when it is done */
	device->component_table_qty = 0;
	if ( NULL != device->component_table ) Mem_Free( NULL, (void **) &(device->component_table) );
	if ( NULL != device->private_data    ) Mem_Free( NULL, (void **) &(device->private_data)    );
}
/*****************************************************************************/
static int STX104_CMD_Open( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
/*<INTERNAL_BEGIN>*/
/* TODO:
	AUL_Device_Open() has collected all I/O range and base address from the
	driver and then calls DevMgr_Open() which then establishes I/O and then
	calls the actual device open.

	Perhaps the opening of a device ought to be broken up into two parts?
	a) pre-allocation of components to determine board byte range ( refer
	   to io_definition_type ).
	b) verification of components
	c) final data structure

	Easiest thing to do is to implement a new device type:
		existing: 	stx104, tracere,
		new:		stx104egs
	This would be found in the driver and then the bytes= used during insmod
	can then be built into the driver.  It will then all work seamless and
	we will not require a pre- post- open functions!

	Now in the case

*/
/*<INTERNAL_END>*/
	int						error_code;
	indirect_cfg_type		cfg_indirect;
	void *					mem;
	component_type *		component_table;
	component_type *		cmp;
	component_type *		cmp_indirect;
	int						component_count;
	int						component_list[STX104_COMPONENT_QTY];
	stx104_dataset_type *	ds;      			/* ds = data set */
	size_t					mem_size;
	uint16_t				firmware_revision;
	(void)					index;
	(void)					buf;
	(void)					buf_len;

/* Obtain firmware revision information
 * TODO: this assumes board is open and active
 */
	error_code = STX104_Gen_Device_Revision( device, &firmware_revision );
	if ( error_code ) goto STX104_CMD_Open_Error;

/* Allocate memory for the components list. */
	mem_size = sizeof( component_type ) * (STX104_COMPONENT_QTY /* +  NULL */);
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_STX104_COMPONENT_ALLOCATE;  goto STX104_CMD_Open_Error; }
	Mem_Set( mem, 0, mem_size );
	component_table = (component_type *) mem;

	component_count = 0;
	if ( firmware_revision >= STX104_REVISION_090115 )
	{
		/* Create the indirect addressing component. */
		component_list[component_count] = STX104_COMPONENT_INDIRECT;
		cmp = &(component_table[component_count++]);
		error_code = Indirect_Create( cmp, 0 /* instance */ );
		if ( error_code ) goto STX104_CMD_Open_Error;
		cmp->component_link = NULL;	/* using direct I/O, no indirect addressing */
		cmp->offset			= device->io_region_offset + 0;
		cmp->iod			= device->io_device;
		/* Offset assigned. Note that other parameters could have been assigned. */
		cmp->status			= cmp->status | CMP_STS_BUILD(1,0,0,0,0);
		error_code = Indirect_Config_Initialize( &cfg_indirect ); /* any probing done here */
		cfg_indirect.address.offset		= STX104_INDEX_POINTER;
		cfg_indirect.address.offset_lsb = STX104_INDEX_POINTER;
		cfg_indirect.address.size       = sizeof( uint8_t );
		cfg_indirect.address.aperture	= sizeof( uint8_t );
		cfg_indirect.data.offset		= STX104_INDEX_DATA;
		cfg_indirect.data.offset_lsb	= STX104_INDEXED_DATA_LSB;
		cfg_indirect.data.offset_msb	= STX104_INDEXED_DATA_MSB;
		cfg_indirect.data.size			= sizeof( uint16_t );
		cfg_indirect.data.aperture		= sizeof( uint16_t );
		error_code = CmpMgr_Command( cmp, CMD_INDIRECT_CONFIG_SET_BASE, 0, (char *) &cfg_indirect, 0 );
		if ( error_code ) goto STX104_CMD_Open_Error;
		//TODO: restart??
		/* open up the index space */
		STX104_Index_Bank_Set( device, 1 );
		cmp_indirect = cmp;

		/* Create i8254 component and link */
		component_list[component_count] = STX104_COMPONENT_I854;
		cmp = &(component_table[component_count++]);
		error_code = I8254_Create( cmp, 0 /* instance */ );  /* any probing done here */
		if ( error_code ) goto STX104_CMD_Open_Error;
		cmp->component_link = cmp_indirect;	/* using indirect addressing */
		cmp->offset			= STX104_I8254_CT0_DATA /* index location */;
		cmp->iod			= device->io_device;
		/* Offset assigned. Note that other parameters could have been assigned. */
		cmp->status			= cmp->status | CMP_STS_BUILD(1,0,0,0,0);

/* <EGS_BEGIN>*/
		/* Create EGS component and link */
		component_list[component_count] = STX104_COMPONENT_EGS;
		cmp = &(component_table[component_count++]);
		cmp->component_link = NULL;	/* using direct I/O, no indirect addressing */
		cmp->offset			= device->io_region_offset + 0x010;
		cmp->iod			= device->io_device;
		error_code = EGS_Create( cmp, 0 /* instance */ );  /* any probing done here */
		if ( -EC_EGS_PROBE == error_code )
		{
/* TODO: would really like a warning subsystem */
			Debug_Log( DEBUG_OUT "Warning EGS Probe failed.  EGS hardware not detected\n" );
			Mem_Set( cmp, 0, sizeof( component_type ) );
			component_list[component_count] = STX104_COMPONENT_NONE;
			component_count--;
			error_code = SUCCESS;
		}
		else if ( error_code )
		{
			goto STX104_CMD_Open_Error;
		}
		else
		{ /* complete the process */
			/* Offset assigned. Note that other parameters could have been assigned. */
			cmp->status		= cmp->status | CMP_STS_BUILD(1,0,0,0,0);
		}
/* <EGS_END>*/
	}
	else if ( firmware_revision == STX104_REVISION_071604 )
	{
		/* Create i8254 component and link */
		component_list[component_count] = STX104_COMPONENT_I854;
		cmp = &(component_table[component_count++]);
		error_code = I8254_Create( cmp, 0 /* instance */ );
		if ( error_code ) goto STX104_CMD_Open_Error;
		cmp->component_link = NULL;	/* using direct I/O, no indirect addressing */
		cmp->offset			= device->io_region_offset + 0x0C;
		cmp->iod			= device->io_device;
		/* Offset assigned. Note that other parameters could have been assigned. */
		cmp->status			= cmp->status | CMP_STS_BUILD(1,0,0,0,0);
/* <EGS_BEGIN>*/
		/* Create EGS component and link */
		component_list[component_count] = STX104_COMPONENT_EGS;
		cmp = &(component_table[component_count++]);
		cmp->component_link = NULL;	/* using direct I/O, no indirect addressing */
		cmp->offset			= device->io_region_offset + 0x010;
		cmp->iod			= device->io_device;
		error_code = EGS_Create( cmp, 0 /* instance */ );  /* any probing done here */
		if ( -EC_EGS_PROBE == error_code )
		{
/* TODO: would really like a warning subsystem */
			Debug_Log( DEBUG_OUT "Warning EGS Probe failed.  EGS hardware not detected\n" );
			Mem_Set( cmp, 0, sizeof( component_type ) );
			component_list[--component_count] = STX104_COMPONENT_NONE;
			error_code = SUCCESS;
		}
		else if ( error_code )
		{
			goto STX104_CMD_Open_Error;
		}
		else
		{ /* complete the process */
			/* Offset assigned. Note that other parameters could have been assigned. */
			cmp->status		= cmp->status | CMP_STS_BUILD(1,0,0,0,0);
		}
/* <EGS_END>*/
	}
	else
	{ /* firmware no mans land.  Need to obtain upgrade */
		error_code = EC_STX104_FIRMWARE_REVISION;	goto STX104_CMD_Open_Error;
	}

/* Allocate and create our internal data set */
	mem_size = sizeof( stx104_dataset_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_STX104_PRIVATE_DATA_ALLOCATE;  goto STX104_CMD_Open_Error; }
	/* initialize the dataset */
	Mem_Set( mem, 0, mem_size );
	ds = (stx104_dataset_type *) mem;
	ds->type					= SYS_TYPE_MAKE( stx104_dataset_type );
	//ds->component_count			= component_count;
	Mem_Copy(ds->component_list, component_list, sizeof( int ) * STX104_COMPONENT_QTY );

	device->component_table     = component_table;
	device->component_table_qty = component_count;
	device->private_data        = mem;

	if ( error_code ) goto STX104_CMD_Open_Error;
	return SUCCESS;
STX104_CMD_Open_Error:
	STX104_Free_Memory( device );
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int STX104_CMD_Close( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int					error_code;
	component_type *	component_table;
	component_type *	cmp;
	stx104_dataset_type *	ds;      /* ds = data set */
	int					i;
	(void) index;
	(void) buf;
	(void) buf_len;

	ds = (stx104_dataset_type *) device->private_data;
	component_table = device->component_table;
	/* NOTE: <component>_Destroy will null the pointer when complete
	 *       Removing only the ones originally created.
	 */
	for ( i = ( device->component_table_qty - 1 ); i >= 0; i-- )
	{
		cmp = &(component_table[i]);
		switch( ds->component_list[i] )
		{
			case STX104_COMPONENT_I854:			I8254_Destroy( 	  cmp );		break;
			case STX104_COMPONENT_INDIRECT:		Indirect_Destroy( cmp );		break;
/* <EGS_BEGIN>*/
			case STX104_COMPONENT_EGS:			EGS_Destroy(  	  cmp );		break;
/* <EGS_END>*/
		}
	}
	STX104_Free_Memory( device );
	return SUCCESS;
/* STX104_Close_Error: */
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#if(0)
/*****************************************************************************/
static int STX104_Connectivity( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;

/* TODO:
1. Build a report indicating which layers we are able to communicate.
2. What do we want to report.
2.a. Can we communicate with the board in question?
2.b. Can we communicate with expected driver?
2.c. Provide some positive feedback to the application that we are indeed
     able to communicate with hardware and all software layers.
*/

	return SUCCESS;
}
#endif
/*****************************************************************************/
static int STX104_CMD_Write( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}
/*****************************************************************************/
static int STX104_CMD_Read( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)   device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}
/*****************************************************************************/
static int STX104_Reg_Read_Write( devmgr_device_type * device, io_xfr_type * xfr )
{
	int	error_code;
	/* verify that command data width matches requested data width transfer */
	if ( STX104_REG_GET_TYPE( xfr->reg_id ) != IO_XFR_GET_TYPE( xfr->command ) )
	{ error_code = EC_STX104_TRANSFER_DATA_TYPE_MISMATCH;  goto STX104_Transfer_Error;	 }

	if ( STX104_REG_IS_INDEXED( xfr->reg_id ) )
	{ /* indexed register */
		uint8_t index = (uint8_t)( STX104_REG_GET_OFFSET( xfr->reg_id ) );
		error_code    = SUCCESS;
		switch( xfr->command )
		{
			case IO_XFR_WRITE_U8:
				STX104_Index_Device_Write_U8( device, index, xfr->data.u8  );
				break;
			case IO_XFR_WRITE_U16:
				STX104_Index_Device_Write_U16( device, index, xfr->data.u16 );
				break;
			case IO_XFR_WRITE_U32:
				STX104_Index_Device_Write_U32( device, index, xfr->data.u32 );
				break;
			case IO_XFR_READ_U8:
				STX104_Index_Device_Read_U8( device, index, &(xfr->data.u8)  );
				break;
			case IO_XFR_READ_U16:
				STX104_Index_Device_Read_U16( device, index, &(xfr->data.u16)  );
				break;
			case IO_XFR_READ_U32:
				STX104_Index_Device_Read_U32( device, index, &(xfr->data.u32)  );
				break;
/* TODO: move repeat functions to dev_stx104_index.c */
			case IO_XFR_WRITE_U8_REPEAT:
				{
					size_t  element;
					size_t	count_element_local = xfr->buf_len / sizeof( uint8_t );
					for ( element = 0; element < count_element_local; element++ )
					{
						STX104_Index_Device_Write_U8( device, index, ((uint8_t *) xfr->data.buf)[element] );
						if ( error_code ) goto STX104_Transfer_Error;
					}
				}
				break;
			case IO_XFR_WRITE_U16_REPEAT:
				{
					size_t  element;
					size_t	count_element_local = xfr->buf_len / sizeof( uint16_t );
					for ( element = 0; element < count_element_local; element++ )
					{
						STX104_Index_Device_Write_U16( device, index, ((uint16_t *) xfr->data.buf)[element] );
						if ( error_code ) goto STX104_Transfer_Error;
					}
				}
				break;
			case IO_XFR_WRITE_U32_REPEAT:
				{
					size_t  element;
					size_t	count_element_local = xfr->buf_len / sizeof( uint32_t );
					for ( element = 0; element < count_element_local; element++ )
					{
						STX104_Index_Device_Write_U32( device, index, ((uint32_t *) xfr->data.buf)[element] );
						if ( error_code ) goto STX104_Transfer_Error;
					}
				}
				break;
			case IO_XFR_READ_U8_REPEAT:
				{
					size_t  element;
					size_t	count_element_local = xfr->buf_len / sizeof( uint8_t );
					for ( element = 0; element < count_element_local; element++ )
					{
						STX104_Index_Device_Read_U8( device, index, &( ((uint8_t *) xfr->data.buf)[element] ) );
						if ( error_code ) goto STX104_Transfer_Error;
					}
				}
				break;
			case IO_XFR_READ_U16_REPEAT:
				{
					size_t  element;
					size_t	count_element_local = xfr->buf_len / sizeof( uint16_t );
					for ( element = 0; element < count_element_local; element++ )
					{
						STX104_Index_Device_Read_U16( device, index, &( ((uint16_t *) xfr->data.buf)[element] ) );
						if ( error_code ) goto STX104_Transfer_Error;
					}
				}
				break;
			case IO_XFR_READ_U32_REPEAT:
				{
					size_t  element;
					size_t	count_element_local = xfr->buf_len / sizeof( uint32_t );
					for ( element = 0; element < count_element_local; element++ )
					{
						STX104_Index_Device_Read_U32( device, index, &( ((uint32_t *) xfr->data.buf)[element] ) );
						if ( error_code ) goto STX104_Transfer_Error;
					}
				}
				break;
			default:
				break;
		}
		goto STX104_Transfer_Continue;
	}
	else
	{ /* direct address */
		xfr->offset = STX104_REG_GET_OFFSET( xfr->reg_id ) + device->io_region_offset;
	}
	error_code = IO_Ioctl( device->io_device, CMD_IO_XFR, (unsigned long)((void *) xfr) );
STX104_Transfer_Continue:
	if ( error_code ) goto STX104_Transfer_Error;
	return SUCCESS;
STX104_Transfer_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int STX104_CMD_Reg_Write( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int				error_code;
	io_xfr_type		xfr;
	(void)	index;
	(void)	buf_len;
	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &xfr, buf, sizeof( xfr ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto STX104_Transfer_Error;	 }

/* TODO: need to verify that the xfr is what we actually have! */

	if ( IO_XFR_NONE == xfr.command )
	{ /* auto-create the XFR command */
		int repeat;
		if ( xfr.buf_len > 0 ) repeat = 1;
		else                   repeat = 0;
		xfr.command = IO_XFR_CREATE(repeat,IO_XFR_DIR_WRITE,STX104_REG_GET_TYPE(xfr.reg_id));
	}
	error_code = STX104_Reg_Read_Write( device, &xfr );
	if ( error_code ) goto STX104_Transfer_Error;
	return SUCCESS;
STX104_Transfer_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
static int STX104_CMD_Reg_Read( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int				error_code;
	io_xfr_type		xfr;
	(void)	index;
	(void)	buf_len;
	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &xfr, buf, sizeof( xfr ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto STX104_Transfer_Error;	 }

/* TODO: need a way to verify xfr integrity -- add type? */

/* TODO: need to verify that the xfr is what we actually have! */

	if ( IO_XFR_NONE == xfr.command )
	{ /* auto-create the XFR command */
		int repeat;
		if ( xfr.buf_len > 0 ) repeat = 1;
		else                   repeat = 0;
		xfr.command = IO_XFR_CREATE(repeat,IO_XFR_DIR_READ,STX104_REG_GET_TYPE(xfr.reg_id));
	}
	error_code = STX104_Reg_Read_Write( device, &xfr );
	if ( error_code ) goto STX104_Transfer_Error;
	return SUCCESS;
STX104_Transfer_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************************/
int STX104_CMD_Restart( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}


#if(0)
/*****************************************************************************/
struct aul_time_specification /* same as struct timespec */
{
	time_t	tv_sec;
	long	tv_nsec;
};
/*****************************************************************************/
/*
Retrieve the granularity of the clock.
*/
int STX104_CMD_Clock_Get_Resolution( devmgr_device_type * device, size_t index, char * buf, size_t buf_len )
{
	int		error_code;
	(void)  device;
	(void)	index;
	(void)	buf;
	(void)	buf_len;


	struct aul_time_specification	ts;

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &ts, buf, sizeof( ts ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto STX104_CMD_Clock_Get_Resolution_Error;	 }


	ts.tv_nsec = (long) STX104_CLOCK_RESOLUTION_NS;

	return SUCCESS;
STX104_CMD_Clock_Get_Resolution_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;	
}
#endif


/*****************************************************************************/
/*****************************************************************************/
/* This functions as a header file without the header file and mess that 
   comes about in this particular case
*/
#include "../../shared/ts_svn.h"

const char * STX104_AI_SVN_Time_Stamp( void );
const char * STX104_AO_SVN_Time_Stamp( void );
const char * STX104_DI_SVN_Time_Stamp( void );
const char * STX104_DO_SVN_Time_Stamp( void );
const char * STX104_Gen_SVN_Time_Stamp( void );
const char * STX104_Index_SVN_Time_Stamp( void );
const char * STX104_Intr_SVN_Time_Stamp( void );
const char * STX104_SPI_SVN_Time_Stamp( void );
const char * STX104_Trg_SVN_Time_Stamp( void );
/*****************************************************************************/
enum stx104_svn
{
	STX104_SVN_DEV			=	0,
	STX104_SVN_AI			=   1,
	STX104_SVN_AO			=	2,
	STX104_SVN_DI			=   3,
	STX104_SVN_DO			=	4,
	STX104_SVN_GEN			=   5,
	STX104_SVN_INDEX		=	6,
	STX104_SVN_INTR			=   7,
	STX104_SVN_SPI			=	8,
	STX104_SVN_TRG			=	9,
	STX104_SVN_QTY			=  10
};
/*****************************************************************************/
/* implemented to reduce code complexity within STX104_SVN_Time_Stamp_Merge() */
typedef const char * ( * stx104_svn_ts_fnc )( void );
static stx104_svn_ts_fnc  stx104_ts_fnc_list[] =
{
	STX104_MODULE_SVN_Time_Stamp,
	STX104_AI_SVN_Time_Stamp,
	STX104_AO_SVN_Time_Stamp,
	STX104_DI_SVN_Time_Stamp,
	STX104_DO_SVN_Time_Stamp,
	STX104_Gen_SVN_Time_Stamp,
	STX104_Index_SVN_Time_Stamp,
	STX104_Intr_SVN_Time_Stamp,
	STX104_SPI_SVN_Time_Stamp,
	STX104_Trg_SVN_Time_Stamp
};
/*****************************************************************************/
static void STX104_SVN_Time_Stamp_Merge( void )
{
	uint32_t		version;
	const char *	version_str;
	uint32_t		max_version;
	size_t			index;
	const char *    max_str;
	/* determine which time stamp is the latest */
	index = 0;
	max_version = 0;
	max_str  = NULL;
	do
	{
		version_str = ( * stx104_ts_fnc_list[index] )();
		version = TSSVN_YYMMDDHH_DEC( version_str );
		if ( version > max_version )
		{	
			max_version = version;
			max_str     = ( * stx104_ts_fnc_list[index] )();
		}
		index++;
	} while ( index < STX104_SVN_QTY );
	device_stx104_definition.svn_module_date = max_str;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
dev_fnc stx104_jump_table[] =
{
	DEVICE_STX104_CMD_DEFINITIONS( CMD_DEV_EXTRACT_JUMP_TABLE )
};
/****************************************************************************
Description
Application - Run during application initialization
Kernel		- Run during kernel initialization

Acronym and base_address are pulled from driver or insmod.
*/
int STX104_Initialization( void )
{
	STX104_SVN_Time_Stamp_Merge();
	return SUCCESS;
}

int STX104_Termination( void )
{
#if defined( STX104_DEBUG_LOG )
	Debug_Log( STX104_DEBUG_LOG "%s\n", __func__ );
#endif
	return SUCCESS;
}
/*****************************************************************/
enum { STX104_DEFAULT_IO_BASE_ADDRESS		= 0x0300 };
enum { STX104_DEFAULT_IO_BYTES				= 16     };
enum { STX104_DEFAULT_BUS_8BIT_RESTRICT     =  0	 }; /* no restriction */
enum { STX104_DEFAULT_MINOR_NUMBER			=  0     };
enum { STX104_DEFAULT_INTERRUPT_NUMBER		=  0     };

/*****************************************************************/
device_definition_type device_stx104_definition =
{
	SYS_TYPE_MAKE(device_definition_type),
	stx104_jump_table,
	STX104_JUMP_TABLE_QTY,
	STX104_Initialization,
	STX104_Termination,
	/* io_definition: logical_id, minor, base, width, irq, acronym */
	{	IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_STX104 ),	
		STX104_DEFAULT_MINOR_NUMBER,
		STX104_DEFAULT_IO_BASE_ADDRESS,					
		STX104_DEFAULT_IO_BYTES,
		STX104_DEFAULT_BUS_8BIT_RESTRICT,/* over-ridden by TARGET_BUS_APERTURE_WIDTH_BYTES */
		STX104_DEFAULT_INTERRUPT_NUMBER, 
		DEVICE_STX104_NAME 
	},
	DEVICE_STX104_NAME,
/*
TODO: need a way to grab the latest version file from all the dev_stx104_?.c files.
*/
	STX104_MODULE_REV,
	STX104_COMMAND_REV,
	STX104_STATUS_REV
};
