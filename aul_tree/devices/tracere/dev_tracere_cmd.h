#ifndef DEVICE_TRACERE_COMMAND
#define DEVICE_TRACERE_COMMAND 17 /* component ID number */
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

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
#define TRACERE_COMMAND_REV  "$Date: 2014-06-25 10:29:34 -0500 (Wed, 25 Jun 2014) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define TRACERE_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_DEVICE(DEVICE_TRACERE_COMMAND)


#define DEVICE_TRACERE_CMD_DEFINITIONS(_) \
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_NOP,			CMD_TRACERE_NOP,			CMD_DEV_NOP					)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Initialize,		CMD_TRACERE_INITIALIZE,		CMD_DEV_INITIALIZE			)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Terminate,		CMD_TRACERE_TERMINATE,		CMD_DEV_TERMINATE			)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Power_Up,		CMD_TRACERE_POWER_UP,		CMD_DEV_POWER_UP			)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Power_Down,		CMD_TRACERE_POWER_DOWN,		CMD_DEV_POWER_DOWN			)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Open,			CMD_TRACERE_OPEN,			CMD_DEV_OPEN				)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Close,			CMD_TRACERE_CLOSE,			CMD_DEV_CLOSE				)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Config_Set,		CMD_TRACERE_CONFIG_SET,		CMD_DEV_CONFIG_SET			)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Config_Get,		CMD_TRACERE_CONFIG_GET,		CMD_DEV_CONFIG_GET			)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Write,			CMD_TRACERE_WRITE,			CMD_DEV_WRITE				)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Read,			CMD_TRACERE_READ,			CMD_DEV_READ				)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Reg_Write,		CMD_TRACERE_REG_WRITE,		CMD_DEV_REG_WRITE			)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Reg_Read,		CMD_TRACERE_REG_READ,		CMD_DEV_REG_READ			)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_Restart,		CMD_TRACERE_RESTART,		CMD_DEV_RESTART				)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_DIO_Bit_Set,	CMD_TRACERE_DIO_BIT_SET,	CMD_DEV_DOUT_BIT_SET		)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_DIO_Bit_Get,	CMD_TRACERE_DIO_BIT_GET,	CMD_DEV_DIN_BIT_GET			)\
	_( TRACERE_LOGICAL_ID, 	NULL,						CMD_TRACERE_EVENT_ENABLE,	CMD_DEV_EVENT_ENABLE		)\
	_( TRACERE_LOGICAL_ID, 	NULL,						CMD_TRACERE_EVENT_DISABLE,	CMD_DEV_EVENT_DISABLE		)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_DIO_Port_Set,	CMD_TRACERE_DIO_PORT_SET,	CMD_DEV_EVENT_DISABLE +  1	)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_DIO_Port_Get,	CMD_TRACERE_DIO_PORT_GET,	CMD_DEV_EVENT_DISABLE +  2	)\
	_( TRACERE_LOGICAL_ID, 	NULL,						CMD_TRACERE_DIO_DIRECTION,	CMD_DEV_EVENT_DISABLE +  3	)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_DIO_Update,		CMD_TRACERE_DIO_UPDATE,		CMD_DEV_EVENT_DISABLE +  4	)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_LED_Set,		CMD_TRACERE_LED_SET,		CMD_DEV_EVENT_DISABLE +  5	)\
	_( TRACERE_LOGICAL_ID, 	Tracere_CMD_LED_Get,		CMD_TRACERE_LED_GET,		CMD_DEV_EVENT_DISABLE +  6	)\


enum {  TRACERE_JUMP_TABLE_QTY = ( CMD_DEV_EVENT_DISABLE +  6 ) - CMD_DEV_NOP + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* DEVICE_TRACERE_COMMAND */
