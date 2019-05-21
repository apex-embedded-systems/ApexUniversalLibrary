#ifndef COMPONENT_INDIRECT_COMMAND
#define COMPONENT_INDIRECT_COMMAND 21 /* component ID number */
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
#define INDIRECT_COMMAND_REV  "$Date: 2015-02-02 20:10:11 -0600 (Mon, 02 Feb 2015) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define INDIRECT_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_INDIRECT_COMMAND)


#define COMPONENT_INDIRECT_CMD_DEFINITIONS(_) \
	_( INDIRECT_LOGICAL_ID, 	Indirect_CMD_Restart,	CMD_INDIRECT_RESTART,		CMD_CMP_RESTART		)\
	_( INDIRECT_LOGICAL_ID, 	Indirect_CMD_Update,	CMD_INDIRECT_UPDATE,		CMD_CMP_UPDATE		)\
	_( INDIRECT_LOGICAL_ID, 	Indirect_CMD_Power_Up,	CMD_INDIRECT_POWER_UP,		CMD_CMP_POWER_UP	)\
	_( INDIRECT_LOGICAL_ID, 	Indirect_CMD_Power_Down,CMD_INDIRECT_POWER_DOWN,	CMD_CMP_POWER_DOWN	)\
	_( INDIRECT_LOGICAL_ID, 	Indirect_CMD_Cfg_Set,	CMD_INDIRECT_CONFIG_SET,	CMD_CMP_CONFIG_SET	)\
	_( INDIRECT_LOGICAL_ID, 	Indirect_CMD_Cfg_Get,	CMD_INDIRECT_CONFIG_GET,	CMD_CMP_CONFIG_GET	)\
	_( INDIRECT_LOGICAL_ID, 	Indirect_CMD_Read,		CMD_INDIRECT_READ,			CMD_CMP_READ		)\
	_( INDIRECT_LOGICAL_ID, 	Indirect_CMD_Write,		CMD_INDIRECT_WRITE,			CMD_CMP_WRITE		)\
	_( INDIRECT_LOGICAL_ID, 	NULL,					CMD_INDIRECT_PRESENT,		CMD_CMP_PRESENT		)

enum {  INDIRECT_JUMP_TABLE_QTY = (CMD_CMP_LAST + 0) - CMD_CMP_RESTART + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* COMPONENT_INDIRECT_COMMAND */
