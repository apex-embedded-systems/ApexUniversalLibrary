#ifndef MODULE_IO_COMMAND
#define MODULE_IO_COMMAND 8 /* component ID number */
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
#define IO_COMMAND_REV  "$Date: 2014-05-17 11:01:01 -0500 (Sat, 17 May 2014) $"


/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define IO_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_MODULE( MODULE_IO_COMMAND )

/* NOTE: All functions are NULL because there is no jump table.  
		 We are using a large switch-case statement.							*/
#define MODULE_IO_CMD_DEFINITIONS(_) \
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_NOP,					CMD_MOD_NOP					)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_ERROR_LAST,			CMD_MOD_ERROR_LAST			)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_INITIALIZE,			CMD_MOD_INITIALIZE			)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_TERMINATE,			CMD_MOD_TERMINATE			)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_VERSION,				CMD_MOD_VERSION				)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_ID,					CMD_MOD_ID					)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_OPEN,				CMD_MOD_OPEN				)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_CLOSE,				CMD_MOD_CLOSE				)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_XFR,	/* IO rd/wr */	CMD_MOD_CLOSE +  1			)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_INTERRUPT_ENABLE,	CMD_MOD_CLOSE +  2			)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_INTERRUPT_DISABLE,	CMD_MOD_CLOSE +  3			)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_DEFINITION_AVAILABLE,CMD_MOD_CLOSE +  4			)\
	_( IO_LOGICAL_ID, 	NULL,		CMD_IO_DEFINITION_GET,		CMD_MOD_CLOSE +  5			)


enum {  IO_JUMP_TABLE_QTY = (CMD_MOD_CLOSE +  5) - CMD_MOD_NOP + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* MODULE_IO_COMMAND */
