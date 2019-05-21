#ifndef MODULE_MODULE
#define MODULE_MODULE       1   /* module ID number */
#define MODULE_MODULE_NAME  "module"
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

/* List all modules to be included in this project. */
#include "../shared/status.h"     /* error codes and APIs		*/
#include "../shared/cmd.h"
#include "../shared/error.h"
#include "../shared/mem.h"
#include "../shared/debug.h"      /* debugging support			*/
#include "../io/io_shared.h"
#include "../reg/reg.h"
#include "../cmpmgr/cmpmgr.h"
#include "../devmgr/devmgr.h"

/* TEMPORARY */
/* #include "../auldrv/auldrv.h" */

/*TODO: merge to a top level interface??  */
#ifndef __KERNEL__
	#include "../components/cmp_avail_module.h"
	#include "../devices/dev_avail_module.h"
	#include "../acgr/acgr.h"
	#include "../aul/aul.h"
	#include "../aul/aul_shared.h"
	#include "../aul/aul_hwdef.h"
	#include "../shared/report.h"
#endif
/*******************************************************************************/

#include "../shared/module_macros.h"


/*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/


#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )

int Module_CSV_Cell_Get( size_t row, MODULE_CSV_COLUMN_ENUM column, char * cell_buf_io, size_t cell_buf_size );

int Module_CSV_Column_Name( MODULE_CSV_COLUMN_ENUM column, char * buf_io, size_t buf_size );

int Module_ID_Row_Get( size_t module_id );

#endif

/*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* MODULE_MODULE */

