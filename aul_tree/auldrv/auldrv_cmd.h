#ifndef MODULE_AULDRV_COMMAND
#define MODULE_AULDRV_COMMAND 7 /* component ID number */
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

#include "../shared/cmd_macros.h"

/*******************************************************************/
#define AULDRV_COMMAND_REV  "$Date: 2014-06-09 12:51:14 -0500 (Mon, 09 Jun 2014) $"


/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define AULDRV_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_MODULE( MODULE_AULDRV_COMMAND )

/* NOTE: All functions are NULL in this list because there is no jump table.  
		 We are using a large switch-case statement.							*/
#define MODULE_AULDRV_CMD_DEFINITIONS(_) \
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_NOP,							CMD_MOD_NOP					)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_ERROR_LAST,					CMD_MOD_ERROR_LAST			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_INITIALIZE,					CMD_MOD_INITIALIZE			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_TERMINATE,					CMD_MOD_TERMINATE			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_VERSION,						CMD_MOD_VERSION				)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_ID,							CMD_MOD_ID					)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_DEVICE_OPEN,					CMD_MOD_OPEN				)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_DEVICE_CLOSE,				CMD_MOD_CLOSE				)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_DEVICE_INTERRUPT_ENABLE,		CMD_MOD_CLOSE +  4			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_DEVICE_INTERRUPT_DISABLE,	CMD_MOD_CLOSE +  5			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_DEVICE_DEFINITION_AVAILABLE, CMD_MOD_CLOSE +  6			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_DEVICE_DEFINITION_GET,		CMD_MOD_CLOSE +  7			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_DEVICE_DEFINITION_SET,		CMD_MOD_CLOSE +  8			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_NAME,						CMD_MOD_CLOSE +  9			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_DEVICE_LIST_GET,				CMD_MOD_CLOSE +  10			)\
	_( AULDRV_LOGICAL_ID, 	NULL,		CMD_AULDRV_DEBUG_ACTIVE_GET,			CMD_MOD_CLOSE +  11			)


enum {  AULDRV_JUMP_TABLE_QTY = (CMD_MOD_CLOSE +  11) - CMD_MOD_NOP + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* MODULE_AULDRV_COMMAND */
