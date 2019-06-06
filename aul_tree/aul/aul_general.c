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

#include "aul.h"
#include "aul_general.h"
#include "aul_shared.h"
#include "../shared/debug.h"
#include "../shared/device.h"
#include "../shared/status.h"
#include "../shared/error.h"
#include "../shared/mem.h"
#include "../shared/cmd.h"
#include "../shared/module_shared.h"

/*****************************************************************************/
#define AUL_GEN_MODULE_REV  "$Date: 2015-01-29 13:59:44 -0600 (Thu, 29 Jan 2015) $"

/*****************************************************************************/
/* if DEBUG then allow IO_DEBUG */
#define AUL_GEN_DEBUG			_DEBUG
/*****************************************************************************/
#define AUL_GEN_DEBUG_LOG		DEBUG_OUT





/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/* EITHER LIBRARY or DRIVER LAYER */

/*****************************************************************************/


#if(0)
/*****************************************************************************/
static int Tracere_CFG_DIO_Set( devmgr_device_type * device, tracere_dio_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;
	component_type *		component;
	i8255m0_cfg_type *		cfg_i8255m0;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_DIO_Set_Error; }
	/* make sure data structure is of correct type */
	if ( SYS_TYPE_MAKE( tracere_dio_cfg_type ) != cfg->type )
	{ error_code = EC_TRACERE_DATA_TYPE_ERROR;  goto Tracere_CFG_DIO_Set_Error;		}
	/* Config for i8255m0 #1 -- get latest */
	component   = &(device->component_table[TRACERE_COMPONENT_I8255M0_0]);
	cfg_i8255m0 = &(cfg->i8255m0_cfg[0]);
	error_code = CmpMgr_Command( component, CMD_I8255M0_CONFIG_SET_BASE, 0, (char *) cfg_i8255m0, sizeof( i8255m0_cfg_type ) );
	if ( error_code ) goto Tracere_CFG_DIO_Set_Error;
	/* Config for i8255m0 #2 -- get latest */
	component   = &(device->component_table[TRACERE_COMPONENT_I8255M0_1]);
	cfg_i8255m0 = &(cfg->i8255m0_cfg[1]);
	error_code = CmpMgr_Command( component, CMD_I8255M0_CONFIG_SET_BASE, 1, (char *) cfg_i8255m0, sizeof( i8255m0_cfg_type ) );
	if ( error_code ) goto Tracere_CFG_DIO_Set_Error;
	/* copy to tracere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( &(ds->cfg.dio), cfg, sizeof( tracere_dio_cfg_type ) );
	return SUCCESS;
Tracere_CFG_DIO_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/*****************************************************************************/
static int Tracere_CFG_DIO_Get( devmgr_device_type * device, tracere_dio_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;
	component_type *		component;
	i8255m0_cfg_type *		cfg_i8255m0;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_DIO_Get_Error; }
	/* pull config from trancere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( cfg, &(ds->cfg.dio), sizeof( ds->cfg.dio ) ); 
	/* Config for i8255m0 #1 -- get the latest */
	component   = &(device->component_table[TRACERE_COMPONENT_I8255M0_0]);
	cfg_i8255m0 = &(cfg->i8255m0_cfg[0]);
	error_code = CmpMgr_Command( component, CMD_I8255M0_CONFIG_GET_BASE, 0, (char *) cfg_i8255m0, sizeof( *cfg_i8255m0 ) );
	if ( error_code ) goto Tracere_CFG_DIO_Get_Error;
	/* Config for i8255m0 #2 -- get the latest */
	component   = &(device->component_table[TRACERE_COMPONENT_I8255M0_1]);
	cfg_i8255m0 = &(cfg->i8255m0_cfg[1]);
	error_code = CmpMgr_Command( component, CMD_I8255M0_CONFIG_GET_BASE, 1, (char *) cfg_i8255m0, sizeof( *cfg_i8255m0 ) );
	if ( error_code ) goto Tracere_CFG_DIO_Get_Error;

	cfg->type = SYS_TYPE_MAKE( tracere_dio_cfg_type );
	return SUCCESS;
Tracere_CFG_DIO_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/*****************************************************************************/
static int Tracere_CFG_CT_Set( devmgr_device_type * device, tracere_ct_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;
	component_type *		component;
	i8254_cfg_type *		cfg_i8254;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_CT_Set_Error; }
	/* */
	if ( SYS_TYPE_MAKE( tracere_ct_cfg_type ) != cfg->type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto Tracere_CFG_CT_Set_Error;	}
	/* make sure data structure is of correct type */
	if ( SYS_TYPE_MAKE( i8254_cfg_type ) != cfg->i8254_cfg.type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto Tracere_CFG_CT_Set_Error;	}
	/* pass configuration down to i8254 */
	component = &(device->component_table[TRACERE_COMPONENT_I8254]);
	cfg_i8254 = &(cfg->i8254_cfg);
	error_code = CmpMgr_Command( component, CMD_I8254_CONFIG_SET_BASE, 0, (char *) cfg_i8254, sizeof( i8254_cfg_type ) );
	if ( error_code ) goto Tracere_CFG_CT_Set_Error;
	/* copy to tracere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( &(ds->cfg.ct), cfg, sizeof( tracere_ct_cfg_type ) );
Tracere_CFG_CT_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/*****************************************************************************/
static int Tracere_CFG_CT_Get( devmgr_device_type * device, tracere_ct_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;
	component_type *		component;
	i8254_cfg_type *		cfg_i8254;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_CT_Get_Error; }
	/* pull config from trancere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( cfg, &(ds->cfg.ct), sizeof( ds->cfg.ct ) ); 
	/* Config for i8254 -- get the latest */
	component = &(device->component_table[TRACERE_COMPONENT_I8254]);
	cfg_i8254 = &(cfg->i8254_cfg);
	error_code = CmpMgr_Command( component, CMD_I8254_CONFIG_GET_BASE, 0, (char *) cfg_i8254, sizeof( *cfg_i8254 ) );
	if ( error_code ) goto Tracere_CFG_CT_Get_Error;

	cfg->type = SYS_TYPE_MAKE( tracere_ct_cfg_type );
	return SUCCESS;
Tracere_CFG_CT_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}



#endif
#if(0)
/*****************************************************************************/
static int Tracere_CFG_Intr_Set( devmgr_device_type * device, tracere_intr_cfg_type * cfg )
{
	int	error_code = EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
	(void) device;
	(void) cfg;

/*Tracere_CFG_Intr_Set_Error:*/
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/*****************************************************************************/
static int Tracere_CFG_Intr_Get( devmgr_device_type * device, tracere_intr_cfg_type * cfg )
{
	int	error_code = EC_TRACERE_COMMAND_NOT_IMPLEMENTED;
	(void) device;
	(void) cfg;

/*Tracere_CFG_Intr_Get_Error:*/
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/*****************************************************************************/
static int Tracere_CFG_Set( devmgr_device_type * device, tracere_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_Set_Error; }

	if ( SYS_TYPE_MAKE( tracere_cfg_type ) != cfg->type )
	{	error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;	goto Tracere_CFG_Set_Error;	}

	error_code = Tracere_CFG_DIO_Set( device, &(cfg->dio) );
	if ( error_code ) goto Tracere_CFG_Set_Error;

	error_code = Tracere_CFG_CT_Set( device, &(cfg->ct) );
	if ( error_code ) goto Tracere_CFG_Set_Error;

	ds = (tracere_dataset_type *) device->private_data;
	ds->cfg.led = cfg->led; 
	return SUCCESS;
Tracere_CFG_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/*****************************************************************************/
static int Tracere_CFG_Get( devmgr_device_type * device, tracere_cfg_type * cfg )
{
	int						error_code;
	tracere_dataset_type * 	ds;

	if ( NULL == cfg )
	{  error_code = EC_TRACERE_NULL_POINTER;  goto Tracere_CFG_Get_Error; }
	/* pull config from trancere dataset */
	ds = (tracere_dataset_type *) device->private_data;
	Mem_Copy( cfg, &(ds->cfg), sizeof( tracere_cfg_type ) );

	error_code = Tracere_CFG_DIO_Get( device, &(cfg->dio) );
	if ( error_code ) goto Tracere_CFG_Get_Error;

	error_code = Tracere_CFG_CT_Get( device, &(cfg->ct) );
	if ( error_code ) goto Tracere_CFG_Get_Error;

	cfg->type = SYS_TYPE_MAKE( tracere_cfg_type );
	return SUCCESS;
Tracere_CFG_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

#endif










/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* LIBRARY LAYER ONLY */
/*****************************************************************************/
#ifndef __KERNEL__

#if(0)
/**********************************************************************************************************//**
@ingroup aul_i8255
\brief
 Apex Universal Library Tracere specific function that sets the Tracere DIO configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] config A pointer to the ::tracere_dio_cfg_type structure that will be used to set the DIO configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Config_Set( int board, tracere_dio_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Config_Set_Error;
	error_code = Tracere_CFG_DIO_Set( device, config );
	if ( !error_code ) return error_code;
Tracere_DIO_Config_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup aul_i8255
\brief
 Apex Universal Library Tracere specific function that retrieves the Tracere DIO configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] config A pointer to the ::tracere_dio_cfg_type structure that will be used to retrieve the DIO configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Config_Get( int board, tracere_dio_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Config_Get_Error;
	error_code = Tracere_CFG_DIO_Get( device, config );
	if ( !error_code ) return error_code;
Tracere_DIO_Config_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup tracere_counter_timer
\brief
 Apex Universal Library Tracere specific function that sets the Tracere Counter/Timer configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] config A pointer to the ::tracere_ct_cfg_type structure that will be used to set the Counter/Timer configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_CT_Config_Set( int board, tracere_ct_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_CT_Config_Set_Error;
	error_code = Tracere_CFG_CT_Set( device, config );
	if ( !error_code ) return error_code;
Tracere_CT_Config_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup tracere_counter_timer
\brief
 Apex Universal Library Tracere specific function that retrieves the Tracere Counter/Timer configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] config A pointer to the ::tracere_ct_cfg_type structure that will be used to retrieve the Counter/Timer configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_CT_Config_Get( int board, tracere_ct_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_CT_Config_Get_Error;
	error_code = Tracere_CFG_CT_Get( device, config );
	if ( !error_code ) return error_code;
Tracere_CT_Config_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup tracere_general
\brief
 Apex Universal Library Tracere specific function that sets the Tracere general configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] config A pointer to the ::tracere_dio_cfg_type structure that will be used to set the general configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_Config_Set( int board, tracere_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_Config_Set_Error;
	error_code = Tracere_CFG_Set( device, config );
	if ( !error_code ) return error_code;
Tracere_Config_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup tracere_general
\brief
 Apex Universal Library Tracere specific function that retrieves the Tracere general configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] config A pointer to the ::tracere_dio_cfg_type structure that will be used to retrieve the general configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_Config_Get( int board, tracere_cfg_type * config )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_Config_Get_Error;
	error_code = Tracere_CFG_Get( device, config );
	if ( !error_code ) return error_code;
Tracere_Config_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup aul_i8255
\brief
 Apex Universal Library Tracere specific function that sets the Tracere DIO port value for the channel passed.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] channel The channel number that the data value will be set to. 
\param[in] value A pointer to an unsigned char that will be used to set the DIO port configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Port_Set( int board, int channel, uint8_t   value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Port_Set_Error;
	error_code = Tracere_CMD_DIO_Port_Set( device, channel, (char *) &value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_DIO_Port_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup aul_i8255
\brief
 Apex Universal Library Tracere specific function that retrieves the Tracere DIO port value for the channel passed.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] channel The channel number that the data value will be read from. 
\param[out] value A pointer to an unsigned char that will be used to retrieve the DIO port configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Port_Get( int board, int channel, uint8_t * value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Port_Get_Error;
	error_code = Tracere_CMD_DIO_Port_Get( device, channel, (char *) value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_DIO_Port_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup aul_dout
\brief
 Apex Universal Library Tracere specific function that sets the Tracere DIO bit value for the channel passed.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] channel The channel number that the data value will be set to. 
\param[in] value A pointer to the ::BOOL structure that will be used to set the DIO bit configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_DOUT_Bit_Set( int board, int channel, BOOL   value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto AUL_DOUT_Bit_Set_Error;
	error_code = AUL_Ioctl( board, CMD_DEV_DOUT_BIT_SET | channel, value );
	if ( !error_code ) return error_code;
AUL_DOUT_Bit_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup aul_i8255
\brief
 Apex Universal Library Tracere specific function that retrieves the Tracere DIO bit value for the channel passed.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] channel The channel number that the data value will be read from. 
\param[out] value A pointer to the ::BOOL structure that will be used to retrieve the DIO bit configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Bit_Get( int board, int channel, BOOL * value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_DIO_Bit_Get_Error;
	error_code = Tracere_CMD_DIO_Bit_Get( device, channel, (char *) value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_DIO_Bit_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**																											
*/
AUL_DLL_API int AULCALLCONV Tracere_DIO_Bit_Connector_Pin_Name_Get( int channel, const char ** pin_name )
{
	int error_code;

	if ( ( channel >= TRACERE_DIO_BIT_QTY ) || ( channel < 0 ) )
	{ error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;  goto Tracere_DIO_Bit_Connector_Pin_Name_Get_Error; }
	*pin_name = tracere_dio_connector_pin_name[channel];
	return SUCCESS;
Tracere_DIO_Bit_Connector_Pin_Name_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**																											
*/
AUL_DLL_API int AULCALLCONV Tracere_CT_CLock_Name_Get( tracere_ct_cfg_type * ct_cfg, int ct_channel, const char ** clock_name )
{
	int		error_code;
	int		clock_enum;

	if ( ( ct_channel >= TRACERE_CT_QTY ) || ( ct_channel < 0 ) )
	{ error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;  goto Tracere_CT_CLock_Name_Get_Error; }
	switch(ct_channel)
	{
		case 0:	clock_enum = ct_cfg->ct0.clock_source;	break;
		case 1: clock_enum = ct_cfg->ct1.clock_source;	break;
		case 2: clock_enum = ct_cfg->ct2.clock_source;  break;
		default:
			error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;
			goto Tracere_CT_CLock_Name_Get_Error;
	}
	*clock_name = tracere_ct_clk_src_name[ct_channel][clock_enum];
	return SUCCESS;
Tracere_CT_CLock_Name_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup tracere_general
\brief
 Apex Universal Library Tracere specific function that restarts the Tracere command.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] value A value of type ::TRACERE_RESTART_ENUM enum that will be used to set the Tracere command restart.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_Restart( int board, TRACERE_RESTART_ENUM value ) 
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_Restart_Error;
	error_code = Tracere_CMD_Restart( device, 0, (char *) &value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_Restart_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup tracere_general
\brief
 Apex Universal Library Tracere specific function that sets the Tracere LED configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] value A value of type ::TRACERE_LED_ENUM enum that will be used to set the LED information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_LED_Set( int board, TRACERE_LED_ENUM   value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_LED_Set_Error;
	/* error_code = DevMgr_Command( device, CMD_TRACERE_LED_SET, 0, (char *) &value, sizeof( value ) ); */
	error_code = Tracere_CMD_LED_Set( device, 0, (char *) &value, sizeof( value ) );
	if ( !error_code ) return error_code;
Tracere_LED_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup tracere_general
\brief
 Apex Universal Library Tracere specific function that retrieves the Tracere LED configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] value A pointer to the ::TRACERE_LED_ENUM enum that will be used to retrieve the LED information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_LED_Get( int board, TRACERE_LED_ENUM * value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_LED_Set_Error;
	/* error_code = DevMgr_Command( device, CMD_TRACERE_LED_GET, 0, (char *) &value, sizeof( value ) ); */
	error_code = Tracere_CMD_LED_Get( device, 0, (char *) value, sizeof( *value ) );
	if ( !error_code ) return error_code;
Tracere_LED_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif
#if(0)
/**********************************************************************************************************//**
@ingroup tracere_counter_timer
\brief
 Apex Universal Library Tracere specific function that retrieves the ??.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] value A pointer to the ??
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV Tracere_CT_Count_Get( int board, int channel, uint16_t * value )
{
	int						error_code;
	i8254_ct_xfr_type		cxfr;
/*	I8254_MODE_ENUM			mode;  */
	component_type *		component;
	devmgr_device_type *	device;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto Tracere_CT_Count_Get_Error;
	if ( channel >= I8254_CT_QTY )
	{ error_code = EC_TRACERE_CHANNEL_OUT_OF_RANGE;	goto Tracere_CT_Count_Get_Error;	}
	Mem_Set( &cxfr, 0, sizeof( cxfr ) );
	cxfr.type    = SYS_TYPE_MAKE( i8254_ct_xfr_type );
	cxfr.control = I8254_CTRL_CREATE( channel,I8254_RW_LATCH,0,0 );
	component    = &(device->component_table[TRACERE_COMPONENT_I8254]);
	error_code   = CmpMgr_Command( component,				/* info about the component			*/
								   CMD_I8254_CT_XFR_BASE,	/* component command for jump table	*/
								   channel,					/* channel			  				*/
								   (char *) &cxfr,	/* value we are passing along		*/
								   sizeof( cxfr )	/* buffer length */
								 );
	if ( error_code ) goto Tracere_CT_Count_Get_Error;
	*value = cxfr.count_read[channel];
	return SUCCESS;
Tracere_CT_Count_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
#endif

/**********************************************************************************************************//**
@ingroup aul_i8254
\brief
 Apex Universal Library Tracere specific function that retrieves the ??.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] value A pointer to the ??
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_I8254_Transfer( int board, i8254_ct_xfr_type * cxfr )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto AUL_I8254_Transfer_Error;
	/* error_code = DevMgr_Command( device, CMD_TRACERE_LED_GET, 0, (char *) &value, sizeof( value ) ); */
	error_code = AUL_Ioctl( board, CMD_I8254_CT_XFR, (unsigned long) cxfr );
	if ( !error_code ) return error_code;
AUL_I8254_Transfer_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_GEN_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup aul_i8254
\brief
 Apex Universal Library Tracere specific function that retrieves the ??.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] value A pointer to the ??
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_I8254_Transfer_Array( int board, i8254_ct_xfr_array_type * cxa )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto AUL_I8254_Transfer_Array_Error;
	/* error_code = DevMgr_Command( device, CMD_TRACERE_LED_GET, 0, (char *) &value, sizeof( value ) ); */
	error_code = AUL_Ioctl( board, CMD_I8254_CT_XFR_ARRAY, (unsigned long)( cxa ) );
	if ( !error_code ) return error_code;
AUL_I8254_Transfer_Array_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_GEN_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************//**
@ingroup aul_dio
\brief
 Apex Universal Library Board independent digital output bit (DO) function.
 This function creates a generic interface.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[in] channel The digital output channel
\param[in] value Digital Output (DO) value to be set.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_DO_Set( int board, int channel, BOOL value )
{
	DEVICE_LOGICAL_ENUM 	logical;
	int						error_code;
	devmgr_device_type *	device;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto AUL_DO_Set_Error;

	logical = IOC_GET_LOGICAL_ID_DEVICE( device->physical_id );

	if ( DEVICE_LOGICAL_STX104 == logical )
	{
		error_code = STX104_DO_Set( board, channel, value );
	}
	else if ( DEVICE_LOGICAL_TRACERE == logical )
	{
		error_code = Tracere_DIO_Bit_Set( board, channel, value );
/*	Here is a way to do the same thing:
 * 		error_code = AUL_Ioctl( board, CMD_TRACERE_DIO_BIT_SET | channel, &value );
 */

	}
	else
	{
		error_code = -EC_AUL_DEVICE_NOT_LISTED;
	}
	if ( error_code ) goto AUL_DO_Set_Error;
	return SUCCESS;
AUL_DO_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_GEN_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup aul_dio
\brief
 Apex Universal Library Board independent digital input bit (DI) function.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[in] channel The digital input channel
\param[out] value Digital Input (DI) value to be gotten.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_DI_Get( int board, int channel, BOOL * value )
{
	DEVICE_LOGICAL_ENUM 	logical;
	int						error_code;
	devmgr_device_type *	device;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto AUL_DI_Set_Error;

	logical = IOC_GET_LOGICAL_ID_DEVICE( device->physical_id );

	if ( DEVICE_LOGICAL_STX104 == logical )
	{
		error_code = STX104_DI_Get( board, channel, value );
	}
	else if ( DEVICE_LOGICAL_TRACERE == logical )
	{
		error_code = Tracere_DIO_Bit_Get( board, channel, value );
	}
	else
	{
		error_code = -EC_AUL_DEVICE_NOT_LISTED;
	}
	if ( error_code ) goto AUL_DI_Set_Error;
	return SUCCESS;
AUL_DI_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_GEN_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup aul_dio
\brief
 Apex Universal Library Board independent digital port write function.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[in] channel The port channel
\param[in] value Port value to be written.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_DIO_Port_Write( int board, int channel, uint8_t value )
{
	DEVICE_LOGICAL_ENUM 	logical;
	int						error_code;
	devmgr_device_type *	device;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto AUL_DIO_Port_Write_Error;

	logical = IOC_GET_LOGICAL_ID_DEVICE( device->physical_id );

	if ( DEVICE_LOGICAL_STX104 == logical )
	{
		error_code = STX104_DO_All_Set( board, value );
	}
	else if ( DEVICE_LOGICAL_TRACERE == logical )
	{
		error_code = Tracere_DIO_Port_Set( board, channel, value );
	}
	else
	{
		error_code = -EC_AUL_DEVICE_NOT_LISTED;
	}
	if ( error_code ) goto AUL_DIO_Port_Write_Error;
	return SUCCESS;
AUL_DIO_Port_Write_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_GEN_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup aul_dio
\brief
 Apex Universal Library Board independent digital port read function.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[in] channel The port channel
\param[in] value Port value read.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV AUL_DIO_Port_Read( int board, int channel, uint8_t * value )
{
	DEVICE_LOGICAL_ENUM 	logical;
	int						error_code;
	devmgr_device_type *	device;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto AUL_DIO_Port_Read_Error;

	logical = IOC_GET_LOGICAL_ID_DEVICE( device->physical_id );

	if ( DEVICE_LOGICAL_STX104 == logical )
	{
		error_code = STX104_DO_All_Get( board, value );
	}
	else if ( DEVICE_LOGICAL_TRACERE == logical )
	{
		error_code = Tracere_DIO_Port_Get( board, channel, value );
	}
	else
	{
		error_code = -EC_AUL_DEVICE_NOT_LISTED;
	}
	if ( error_code ) goto AUL_DIO_Port_Read_Error;
	return SUCCESS;
AUL_DIO_Port_Read_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( MODULE_AUL_GEN_NAME, __func__, __LINE__, error_code );
	return -error_code;
}




/*****************************************************************************/
#endif /* not __KERNEL__   (library only) */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

