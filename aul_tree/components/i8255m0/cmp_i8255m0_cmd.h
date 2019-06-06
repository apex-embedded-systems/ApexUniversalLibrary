#ifndef COMPONENT_I8255M0_COMMAND
#define COMPONENT_I8255M0_COMMAND 16 /* component ID number */
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
#define I8255M0_COMMAND_REV  "$Date: 2015-02-02 20:10:11 -0600 (Mon, 02 Feb 2015) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define I8255M0_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_I8255M0_COMMAND)


#define COMPONENT_I8255M0_CMD_DEFINITIONS(_) \
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_Restart,	CMD_I8255M0_RESTART,	CMD_CMP_RESTART		)\
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_Update,		CMD_I8255M0_UPDATE,		CMD_CMP_UPDATE		)\
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_Power_Up,	CMD_I8255M0_POWER_UP,	CMD_CMP_POWER_UP	)\
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_Power_Down,	CMD_I8255M0_POWER_DOWN,	CMD_CMP_POWER_DOWN	)\
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_Cfg_Set,	CMD_I8255M0_CONFIG_SET,	CMD_CMP_CONFIG_SET	)\
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_Cfg_Get,	CMD_I8255M0_CONFIG_GET,	CMD_CMP_CONFIG_GET	)\
	_( I8255M0_LOGICAL_ID, 	NULL,					CMD_I8255M0_READ,		CMD_CMP_READ		)\
	_( I8255M0_LOGICAL_ID, 	NULL,					CMD_I8255M0_WRITE,		CMD_CMP_WRITE		)\
	_( I8255M0_LOGICAL_ID, 	NULL,					CMD_I8255M0_PRESENT,	CMD_CMP_PRESENT		)\
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_DI_Get,		CMD_I8255M0_DI_GET,		CMD_CMP_LAST + 1	)\
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_DO_Set,		CMD_I8255M0_DO_SET,		CMD_CMP_LAST + 2	)\
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_Port_Get,	CMD_I8255M0_PORT_GET,	CMD_CMP_LAST + 3	)\
	_( I8255M0_LOGICAL_ID, 	I8255M0_CMD_Port_Set,	CMD_I8255M0_PORT_SET,	CMD_CMP_LAST + 4	)

enum {  I8255M0_JUMP_TABLE_QTY = (CMD_CMP_LAST + 4) - CMD_CMP_RESTART + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* COMPONENT_I8255M0_COMMAND */
