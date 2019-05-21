#ifndef COMPONENT_<TEMPLATE>_COMMAND
#define COMPONENT_<TEMPLATE>_COMMAND 16 /* component ID number */
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
#define <TEMPLATE>_COMMAND_REV  "$Date: 2015-02-03 06:18:04 -0600 (Tue, 03 Feb 2015) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define <TEMPLATE>_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_<TEMPLATE>_COMMAND)


#define COMPONENT_<TEMPLATE>_CMD_DEFINITIONS(_) \
	_( <TEMPLATE>_LOGICAL_ID, 	<Template>_CMD_Restart,		CMD_<TEMPLATE>_RESTART,		CMD_CMP_RESTART		)\
	_( <TEMPLATE>_LOGICAL_ID, 	<Template>_CMD_Update,		CMD_<TEMPLATE>_UPDATE,		CMD_CMP_UPDATE		)\
	_( <TEMPLATE>_LOGICAL_ID, 	<Template>_CMD_Power_Up,	CMD_<TEMPLATE>_POWER_UP,	CMD_CMP_POWER_UP	)\
	_( <TEMPLATE>_LOGICAL_ID, 	<Template>_CMD_Power_Down,	CMD_<TEMPLATE>_POWER_DOWN,	CMD_CMP_POWER_DOWN	)\
	_( <TEMPLATE>_LOGICAL_ID, 	<Template>_CMD_Cfg_Set,		CMD_<TEMPLATE>_CONFIG_SET,	CMD_CMP_CONFIG_SET	)\
	_( <TEMPLATE>_LOGICAL_ID, 	<Template>_CMD_Cfg_Get,		CMD_<TEMPLATE>_CONFIG_GET,	CMD_CMP_CONFIG_GET	)\
	_( <TEMPLATE>_LOGICAL_ID, 	NULL,						CMD_<TEMPLATE>_READ,		CMD_CMP_READ		)\
	_( <TEMPLATE>_LOGICAL_ID, 	NULL,						CMD_<TEMPLATE>_WRITE,		CMD_CMP_WRITE		)\
	_( <TEMPLATE>_LOGICAL_ID, 	NULL,						CMD_<TEMPLATE>_PRESENT,		CMD_CMP_PRESENT		)

enum {  <TEMPLATE>_JUMP_TABLE_QTY = (CMD_CMP_LAST + 0) - CMD_CMP_RESTART + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* COMPONENT_<TEMPLATE>_COMMAND */
