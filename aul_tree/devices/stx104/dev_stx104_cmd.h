#ifndef DEVICE_STX104_COMMAND
#define DEVICE_STX104_COMMAND 18 /* component ID number */
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
#define STX104_COMMAND_REV  "$Date: 2014-06-25 10:29:34 -0500 (Wed, 25 Jun 2014) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define STX104_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_DEVICE(DEVICE_STX104_COMMAND)


#define DEVICE_STX104_CMD_DEFINITIONS(_) \
	_( STX104_LOGICAL_ID, 	STX104_CMD_NOP,				CMD_STX104_NOP,				CMD_DEV_NOP					)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Initialize,		CMD_STX104_INITIALIZE,		CMD_DEV_INITIALIZE			)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Terminate,		CMD_STX104_TERMINATE,		CMD_DEV_TERMINATE			)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Power_Up,		CMD_STX104_POWER_UP,		CMD_DEV_POWER_UP			)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Power_Down,		CMD_STX104_POWER_DOWN,		CMD_DEV_POWER_DOWN			)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Open,			CMD_STX104_OPEN,			CMD_DEV_OPEN				)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Close,			CMD_STX104_CLOSE,			CMD_DEV_CLOSE				)\
	_( STX104_LOGICAL_ID, 	NULL,						CMD_STX104_CONFIG_SET,		CMD_DEV_CONFIG_SET			)\
	_( STX104_LOGICAL_ID, 	NULL,						CMD_STX104_CONFIG_GET,		CMD_DEV_CONFIG_GET			)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Write,			CMD_STX104_WRITE,			CMD_DEV_WRITE				)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Read,			CMD_STX104_READ,			CMD_DEV_READ				)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Reg_Write,		CMD_STX104_REG_WRITE,		CMD_DEV_REG_WRITE			)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Reg_Read,		CMD_STX104_REG_READ,		CMD_DEV_REG_READ			)\
	_( STX104_LOGICAL_ID, 	STX104_CMD_Restart,			CMD_STX104_RESTART,			CMD_DEV_RESTART				)\
	_( STX104_LOGICAL_ID, 	NULL,						CMD_STX104_DOUT_BIT_SET,	CMD_DEV_DOUT_BIT_SET		)\
	_( STX104_LOGICAL_ID, 	NULL,						CMD_STX104_DIN_BIT_GET,		CMD_DEV_DIN_BIT_GET			)\
	_( STX104_LOGICAL_ID, 	NULL,						CMD_STX104_EVENT_ENABLE,	CMD_DEV_EVENT_ENABLE		)\
	_( STX104_LOGICAL_ID, 	NULL,						CMD_STX104_EVENT_DISABLE,	CMD_DEV_EVENT_DISABLE		)\
	_( STX104_LOGICAL_ID, 	NULL,						CMD_STX104_DIO_PORT_SET,	CMD_DEV_EVENT_DISABLE +  1	)\
	_( STX104_LOGICAL_ID, 	NULL,						CMD_STX104_DIO_PORT_GET,	CMD_DEV_EVENT_DISABLE +  2	)\
	_( STX104_LOGICAL_ID, 	NULL,						CMD_STX104_DIO_UPDATE,		CMD_DEV_EVENT_DISABLE +  3	)


enum {  STX104_JUMP_TABLE_QTY = ( CMD_DEV_EVENT_DISABLE +  3 ) - CMD_DEV_NOP + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* DEVICE_STX104_COMMAND */
