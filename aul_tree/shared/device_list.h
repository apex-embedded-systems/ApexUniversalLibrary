#ifndef _DEVICE_LIST_H_
#define _DEVICE_LIST_H_
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


*********************************************************************************/

#include "../shared/sysval.h"
#include "../shared/device.h"
#include "../io/io_shared.h"
#include "../shared/cmd_macros.h"  /* IOC_T */

/*******************************************************************************/
#define _DEVICE_LIST_H_REV  "$Date: 2014-06-09 12:46:56 -0500 (Mon, 09 Jun 2014) $"
/*******************************************************************************/

struct device_list
{
	SYS_TYPE				type;
	size_t					region_count;   /* = device count */
	int						open_and_active[DEVICE_QTY + 1];
	struct 	io_definition	region[DEVICE_QTY + 1];  /* = device details */
};
typedef struct device_list device_list_type;

#endif  /* _DEVICE_LIST_H_ */

