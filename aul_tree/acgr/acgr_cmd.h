#ifndef MODULE_ACGR_COMMAND
#define MODULE_ACGR_COMMAND 15


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
#define ACGR_COMMAND_REV  "$Date: 2014-04-23 14:50:08 -0500 (Wed, 23 Apr 2014) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define ACGR_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_MODULE( MODULE_ACGR_COMMAND )

/*******************************************************************/
/* NOTE: this module does not use any commands.  This is a "place holder"
   within the cmd.c file for future commands for this module.                        */
#define MODULE_ACGR_CMD_DEFINITIONS(_) \
	_( ACGR_LOGICAL_ID, 	NULL,		CMD_ACGR_NOP,					CMD_MOD_NOP					)


enum {  ACGR_JUMP_TABLE_QTY = (CMD_MOD_ERROR_LAST) - CMD_MOD_NOP + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* MODULE_ACGR_COMMAND */