#ifndef MODULE_MEM
#define MODULE_MEM 4 /* module ID number */
#define MODULE_MEM_NAME "mem"
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
    
	COPYRIGHT NOTICE Copyright (c) 2013 by Apex Embedded Systems. All rights 
	reserved. This document is the confidential property of Apex Embedded 
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


*********************************************************************************/

/********************************************************************************/

#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
/* #include "../rtmp/rtmp.h" */

#include "../shared/module_macros.h"

/********************************************************************************/


/********************************************************************************/
#ifdef _DEBUG
# define MEM_ALIGNMENT_TEST( ptr, bytes, message )   Mem_Alignment_Test_Function( ptr, bytes, message )
#else
# define MEM_ALIGNMENT_TEST( ptr, bytes )
#endif


/********************************************************************************/
#ifdef __RTMP_H__
	enum { MEM_NEW_USE_PARAMS       =  0 };
#endif

enum { MEM_NEW_USE_OS_POOL      =  1 };

#ifdef __RTMP_H__
	enum { MEM_NEW_USE_GLOBAL_RTMP  =  2 };
#endif

/********************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/********************************************************************************/


int Mem_Compare( const void * buf1, const void * buf2, size_t count );

void * Mem_Copy( void * dst, const void * src, size_t dst_count );

void * Mem_Set( void * dst, uint8_t value, size_t dst_count );

/********************************************************************************/

void * Mem_Allocate( void * mem_pool, size_t byte_quantity );

#ifndef __KERNEL__
 void * Mem_Reallocate( void * mem_pool, void ** memory, size_t byte_quantity );
#endif

void Mem_Free( void * mem_pool, void ** memory );

#ifdef __RTMP_H__MODULE_MEM_NAME
rtmp_t Mem_Create( size_t mem_count, void * mem, size_t mp_count, rtmp_m_t * mp  );
#endif

size_t Mem_Block_Available_Smallest( void * mem_pool );

void Mem_Alignment_Test_Function( void * ptr, size_t byte_alignment, const char * message );

void * Mem_New( size_t mem_count,		            
				void * mem, 
				size_t parition_list_row_quantity,  
				void * partition_list, 
				size_t flags  /* 1 = use malloc() functionality, 2 = use global rtmp pool, 0 = defaults */
			  );

int Mem_Terminate( void );

int Mem_Initialize( void );

extern module_definition_type module_mem_definition;

/********************************************************************************/
#ifdef __cplusplus
}
#endif
/********************************************************************************/

#endif /* MODULE_MEM */
