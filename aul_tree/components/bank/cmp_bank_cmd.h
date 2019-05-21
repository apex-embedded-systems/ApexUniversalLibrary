#ifndef COMPONENT_BANK_COMMAND
#define COMPONENT_BANK_COMMAND 31 /* component ID number */
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
#define BANK_COMMAND_REV  "$Date: 2015-02-04 20:27:51 -0600 (Wed, 04 Feb 2015) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define BANK_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_BANK_COMMAND)


#define COMPONENT_BANK_CMD_DEFINITIONS(_) \
	_( BANK_LOGICAL_ID, 	Bank_CMD_Restart,	CMD_BANK_RESTART,		CMD_CMP_RESTART		)\
	_( BANK_LOGICAL_ID, 	Bank_CMD_Update,	CMD_BANK_UPDATE,		CMD_CMP_UPDATE		)\
	_( BANK_LOGICAL_ID, 	Bank_CMD_Power_Up,	CMD_BANK_POWER_UP,		CMD_CMP_POWER_UP	)\
	_( BANK_LOGICAL_ID, 	Bank_CMD_Power_Down,CMD_BANK_POWER_DOWN,	CMD_CMP_POWER_DOWN	)\
	_( BANK_LOGICAL_ID, 	Bank_CMD_Cfg_Set,	CMD_BANK_CONFIG_SET,	CMD_CMP_CONFIG_SET	)\
	_( BANK_LOGICAL_ID, 	Bank_CMD_Cfg_Get,	CMD_BANK_CONFIG_GET,	CMD_CMP_CONFIG_GET	)\
	_( BANK_LOGICAL_ID, 	Bank_CMD_Read,		CMD_BANK_READ,			CMD_CMP_READ		)\
	_( BANK_LOGICAL_ID, 	Bank_CMD_Write,		CMD_BANK_WRITE,			CMD_CMP_WRITE		)\
	_( BANK_LOGICAL_ID, 	NULL,				CMD_BANK_PRESENT,		CMD_CMP_PRESENT		)

enum {  BANK_JUMP_TABLE_QTY = (CMD_CMP_LAST + 0) - CMD_CMP_RESTART + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* COMPONENT_BANK_COMMAND */
