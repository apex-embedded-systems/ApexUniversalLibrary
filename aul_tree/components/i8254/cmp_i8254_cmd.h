#ifndef COMPONENT_I8254_COMMAND
#define COMPONENT_I8254_COMMAND 19 /* component ID number */
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
#define I8254_COMMAND_REV  "$Date: 2015-02-02 20:10:11 -0600 (Mon, 02 Feb 2015) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define I8254_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_I8254_COMMAND)


#define COMPONENT_I8254_CMD_DEFINITIONS(_) \
	_( I8254_LOGICAL_ID, 	I8254_Restart,			CMD_I8254_RESTART,		CMD_CMP_RESTART		)\
	_( I8254_LOGICAL_ID, 	I8254_CMD_Update,		CMD_I8254_UPDATE,		CMD_CMP_UPDATE		)\
	_( I8254_LOGICAL_ID, 	I8254_CMD_Power_Up,		CMD_I8254_POWER_UP,		CMD_CMP_POWER_UP	)\
	_( I8254_LOGICAL_ID, 	I8254_CMD_Power_Down,	CMD_I8254_POWER_DOWN,	CMD_CMP_POWER_DOWN	)\
	_( I8254_LOGICAL_ID, 	I8254_CMD_Cfg_Set,		CMD_I8254_CONFIG_SET,	CMD_CMP_CONFIG_SET	)\
	_( I8254_LOGICAL_ID, 	I8254_CMD_Cfg_Get,		CMD_I8254_CONFIG_GET,	CMD_CMP_CONFIG_GET	)\
	_( I8254_LOGICAL_ID, 	NULL,					CMD_I8254_READ,			CMD_CMP_READ		)\
	_( I8254_LOGICAL_ID, 	NULL,					CMD_I8254_WRITE,		CMD_CMP_WRITE		)\
	_( I8254_LOGICAL_ID, 	NULL,					CMD_I8254_PRESENT,		CMD_CMP_PRESENT		)\
	_( I8254_LOGICAL_ID, 	I8254_CMD_Xfr,			CMD_I8254_CT_XFR,		CMD_CMP_LAST + 1	)\
	_( I8254_LOGICAL_ID, 	I8254_CMD_Xfr_Array,	CMD_I8254_CT_XFR_ARRAY,	CMD_CMP_LAST + 2	)


enum {  I8254_JUMP_TABLE_QTY = (CMD_CMP_LAST + 2) - CMD_CMP_RESTART + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* COMPONENT_I8254_COMMAND */
