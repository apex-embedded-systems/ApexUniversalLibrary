#ifndef COMPONENT_SYSCON_COMMAND
#define COMPONENT_SYSCON_COMMAND 20 /* component ID number */
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
#define SYSCON_COMMAND_REV  "$Date: 2015-02-03 06:18:04 -0600 (Tue, 03 Feb 2015) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define SYSCON_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_SYSCON_COMMAND)


#define COMPONENT_SYSCON_CMD_DEFINITIONS(_) \
	_( SYSCON_LOGICAL_ID, 	SysCon_CMD_Restart,		CMD_SYSCON_RESTART,			CMD_CMP_RESTART		)\
	_( SYSCON_LOGICAL_ID, 	SysCon_CMD_Update,		CMD_SYSCON_UPDATE,			CMD_CMP_UPDATE		)\
	_( SYSCON_LOGICAL_ID, 	SysCon_CMD_Power_Up,	CMD_SYSCON_POWER_UP,		CMD_CMP_POWER_UP	)\
	_( SYSCON_LOGICAL_ID, 	SysCon_CMD_Power_Down,	CMD_SYSCON_POWER_DOWN,		CMD_CMP_POWER_DOWN	)\
	_( SYSCON_LOGICAL_ID, 	SysCon_CMD_Cfg_Set,		CMD_SYSCON_CONFIG_SET,		CMD_CMP_CONFIG_SET	)\
	_( SYSCON_LOGICAL_ID, 	SysCon_CMD_Cfg_Get,		CMD_SYSCON_CONFIG_GET,		CMD_CMP_CONFIG_GET	)\
	_( SYSCON_LOGICAL_ID, 	NULL,					CMD_SYSCON_READ,			CMD_CMP_READ		)\
	_( SYSCON_LOGICAL_ID, 	NULL,					CMD_SYSCON_WRITE,			CMD_CMP_WRITE		)\
	_( SYSCON_LOGICAL_ID, 	NULL,					CMD_SYSCON_PRESENT,			CMD_CMP_PRESENT		)\
	_( SYSCON_LOGICAL_ID, 	SysCon_CMD_Run,			CMD_SYSCON_RUN,				CMD_CMP_LAST + 1	)

enum {  SYSCON_JUMP_TABLE_QTY = (CMD_CMP_LAST + 1) - CMD_CMP_RESTART + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* COMPONENT_SYSCON_COMMAND */
