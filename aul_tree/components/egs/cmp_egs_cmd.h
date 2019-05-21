#ifndef COMPONENT_EGS_COMMAND
#define COMPONENT_EGS_COMMAND 30 /* component ID number */
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
#define EGS_COMMAND_REV  "$Date: 2015-02-03 06:18:04 -0600 (Tue, 03 Feb 2015) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define EGS_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_EGS_COMMAND)


#define COMPONENT_EGS_CMD_DEFINITIONS(_) \
	_( EGS_LOGICAL_ID, 	EGS_CMD_Restart,	CMD_EGS_RESTART,	CMD_CMP_RESTART		)\
	_( EGS_LOGICAL_ID, 	EGS_CMD_Update,		CMD_EGS_UPDATE,		CMD_CMP_UPDATE		)\
	_( EGS_LOGICAL_ID, 	EGS_CMD_Power_Up,	CMD_EGS_POWER_UP,	CMD_CMP_POWER_UP	)\
	_( EGS_LOGICAL_ID, 	EGS_CMD_Power_Down,	CMD_EGS_POWER_DOWN,	CMD_CMP_POWER_DOWN	)\
	_( EGS_LOGICAL_ID, 	EGS_CMD_Cfg_Set,	CMD_EGS_CONFIG_SET,	CMD_CMP_CONFIG_SET	)\
	_( EGS_LOGICAL_ID, 	EGS_CMD_Cfg_Get,	CMD_EGS_CONFIG_GET,	CMD_CMP_CONFIG_GET	)\
	_( EGS_LOGICAL_ID, 	NULL,				CMD_EGS_READ,		CMD_CMP_READ		)\
	_( EGS_LOGICAL_ID, 	NULL,				CMD_EGS_WRITE,		CMD_CMP_WRITE		)\
	_( EGS_LOGICAL_ID, 	NULL,				CMD_EGS_PRESENT,	CMD_CMP_PRESENT		)

enum {  EGS_JUMP_TABLE_QTY = (CMD_CMP_LAST + 0) - CMD_CMP_RESTART + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* COMPONENT_EGS_COMMAND */
