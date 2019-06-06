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

1. rtmp within kernel space.


*********************************************************************************/
/* #include <assert.h> */
#include "mem.h"
/* #include "../shared/error.h" */
/* #include "../shared/status_macros.h"  */
#include "../shared/status.h"     /* error codes and APIs	*/
#include "../shared/error.h"

#ifdef __KERNEL__
	#include <linux/slab.h>  /* kmalloc() kfree() */
#else
	#ifndef __RTMP_H__
	# include <memory.h>
	# include <malloc.h>
	#endif
#endif

#include "../shared/cmd_macros.h"
/*********************************************************************************/
#define MEM_MODULE_REV  "$Date: 2014-06-08 05:52:51 -0500 (Sun, 08 Jun 2014) $"

/********************************************************************************/
int Mem_Compare( const void * buf1, const void * buf2, size_t count )
{
	int              difference  = 0;
	const uint8_t *  p1          = (uint8_t *) buf1;
	const uint8_t *  p2          = (uint8_t *) buf2;

	while ( ( count-- > 0 ) && ( 0 == difference ) )
	{
		difference = ((int) *( p1++ )) - ((int) *( p2++ ));
	}
	return difference;
}
/********************************************************************************/
void * Mem_Copy( void * dst, const void * src, size_t dst_count )
{
	uint8_t * d;
	uint8_t * s;
	if ( NULL == dst    ) return dst;
	if ( NULL == src    ) return dst;
	if ( 0 == dst_count ) return dst;
	if ( src == dst     ) return dst;
	d = (uint8_t *) dst;
	s = (uint8_t *) src;
	if ( dst < src )
	{ /* overlap where destination is below source */
		do {	*d++ = *s++;	dst_count--;	} while ( dst_count != 0 );
	}
	else
	{ /* overlap where destination is above source */
		d = &(d[dst_count - 1]);	s = &(s[dst_count - 1]);
		do {	*d-- = *s--;	dst_count--;	} while ( dst_count != 0 );
	}
	return dst;
}
/********************************************************************************/
void * Mem_Set( void * dst, uint8_t value, size_t dst_count )
{ 
	uint8_t * d = (uint8_t *) dst;
	if ( NULL == dst    ) return dst;
	if ( 0 == dst_count ) return dst;
	do {	*d++ = value;	dst_count--;	} while ( dst_count != 0 );
	return dst;
}
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

#define TODO_MIN_VALUE  8

/********************************************************************************/
#ifdef __RTMP_H__
	enum{ MEM_POOL_SIZE = 5 * 1024 };
	static rtmp_t  mem_pool_rtmp = { 0 };
	static uint8_t mem_pool[MEM_POOL_SIZE];

	enum{ MEM_POOL_DEFINITION_QTY = 7 };
	/*TODO: must be aligned*/
	static rtmp_m_t mem_pool_definition[MEM_POOL_DEFINITION_QTY] = 
	{ /*  block_qty    block_size   */
		{	    32,		   16		},   /* =   512 bytes		*/
		{	    16,		   32		},   /* =   512 bytes		*/
		{	     8,		   64		},   /* =   512 bytes		*/
		{	     4,		   128		},   /* =   512 bytes		*/
		{	     4,		   256		},   /* =  1024 bytes		*/
		{	     2,		   512		},   /* =  1024 bytes		*/
		{	     1,		   1024		},   /* =  1024 bytes		*/
										 /*    5120 bytes total	*/
	};
#endif
/********************************************************************************/
void * Mem_Allocate( void * mem_pool, size_t byte_quantity )
{	
	void * m;
#ifdef __RTMP_H__
	if ( NULL == mem_pool )  
	{
		if ( mem_pool_rtmp.ready )  m = RTMP_Allocate( &mem_pool_rtmp, byte_quantity );
		else                        m = calloc( byte_quantity, 1 );
	}
	else            
	{
		m = RTMP_Allocate( (rtmp_t *) mem_pool, byte_quantity );
	}
#else
	(void) mem_pool;
 #ifdef __KERNEL__
	m = kmalloc( byte_quantity, GFP_KERNEL );
	Mem_Set( m, 0, byte_quantity );
 #else
	m = calloc( byte_quantity, 1 );
 #endif
#endif
	return m;
}
/********************************************************************************/
#ifndef __KERNEL__
void * Mem_Reallocate( void * mem_pool, void ** memory, size_t byte_quantity )
{	
	void * m;
#ifdef __RTMP_H__
	if ( NULL == mem_pool )  
	{
		if ( mem_pool_rtmp.ready )  m = RTMP_Reallocate( &mem_pool_rtmp, memory, byte_quantity );
		else                        m = realloc( *memory, byte_quantity );
	}
	else            
	{
		m = RTMP_Reallocate( (rtmp_t *) mem_pool, memory, byte_quantity );
	}
#else
	(void) mem_pool;
	m = realloc( *memory, byte_quantity );
#endif
	return m;
}
#endif /* __KERNEL__ */
/********************************************************************************/
void Mem_Free( void * mem_pool, void ** memory )
{
#ifdef __RTMP_H__
	if ( NULL == mem_pool )  
	{
		if ( mem_pool_rtmp.ready )  RTMP_Free( (rtmp_t *) mem_pool, memory );
		else                        free( *memory );
	}
	else            
	{
		RTMP_Free( (rtmp_t *) mem_pool, memory );
	}
#else
	(void) mem_pool;
	if ( NULL != memory )
	{
		if ( NULL != *memory )
		{
 #ifdef __KERNEL__
			kfree( *memory );
 #else
			free( *memory );
 #endif
			*memory = NULL;
		}
	}
#endif
}
/*******************************************************************************
Description

*/
void * Mem_New( size_t mem_count,		            
				void * mem, 
				size_t parition_list_row_quantity,  
				void * partition_list, 
				size_t flags 
			  )
{
#ifndef __RTMP_H__
	(void) mem_count;
	(void) mem;
	(void) parition_list_row_quantity;
	(void) partition_list;
#endif
	if ( MEM_NEW_USE_OS_POOL == flags )
	{ /* use compiler/RTOS defined malloc */
		/* implementation, target specific.    */
		return NULL;
	}
#ifdef __RTMP_H__
	else if ( MEM_NEW_USE_GLOBAL_RTMP == flags ) /* global rtmp pool */
	{ /* utilize global rtmp pool and either external or internal definition */
		rtmp_m_t * _partition_list;

		if ( NULL != partition_list ) 
		{
			if ( 0 == parition_list_row_quantity ) { /* error */ }
			_partition_list = (rtmp_m_t *) partition_list;
		}
		else
		{
			_partition_list            = mem_pool_definition;
			parition_list_row_quantity = MEM_POOL_DEFINITION_QTY;
		}

		mem_pool_rtmp = RTMP_Create( sizeof( mem_pool ),			/* memory pool size in bytes	*/
									 mem_pool,						/* memory pool pointer			*/
									 parition_list_row_quantity,	/* partition list row quantity	*/
									 _partition_list,				/* parition list definition		*/
									 "Mem_New_Create"				/* debug name					*/
								   );
		return &mem_pool_rtmp;
	}
	else 
	{ /* MEM_NEW_USE_PARAMS */
		if ( 0 == mem_count ) { /* error */ }
		if ( 0 == parition_list_row_quantity ) { /* error */ }
		if ( NULL == mem ) { /* error */ }
		if ( NULL == partition_list ) { /* error */ }

		return RTMP_New( mem_count, 
						 mem, 
						 parition_list_row_quantity, 
						 (rtmp_m_t *) partition_list,
						 "Mem_New_New"				/* debug name					*/
					   );
	}
#else
	return NULL;
#endif
}
#ifdef __RTMP_H__
/*******************************************************************************
Description
mp     mem      pool
null   null     malloc pool
value  null     "global" rtmp pool
value  value    local rtmp pool
*/
rtmp_t Mem_Create( size_t mem_count, void * mem, size_t mp_count, rtmp_m_t * mp  )
{
	if ( NULL == mem )  
	{
	}
	else            
	{
	}
	return RTMP_Create( mem_count, mem, mp_count, mp, NULL );
}
#endif
/********************************************************************************/
size_t Mem_Block_Available_Smallest( void * mem_pool )
{
#ifdef __RTMP_H__
	if ( NULL == mem_pool )  
	{
		if ( mem_pool_rtmp.ready )  return RTMP_Block_Available_Smallest( (rtmp_t *) mem_pool );
		else                        return TODO_MIN_VALUE;
	}
	else            
	{
		return RTMP_Block_Available_Smallest( (rtmp_t *) mem_pool );
	}
#else
	(void) mem_pool;
	return TODO_MIN_VALUE;
#endif
}
/*******************************************************************/
int Mem_Terminate( void )
{
	return SUCCESS;
}
/*******************************************************************/
int Mem_Initialize( void )
{
	return SUCCESS;
}
/*********************************************************************************/
module_definition_type module_mem_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	Mem_Initialize,
	Mem_Terminate,
	MODULE_MEM_NAME,
	MEM_MODULE_REV,
	NULL,
	MEM_STATUS_REV
};

#ifdef _DEBUG
/********************************************************************************/
void Mem_Alignment_Test_Function( void * ptr, size_t byte_alignment, const char * message )
{
/* Test to make sure that the buf[] is aligned to 8-byte boundaries.
	TODO:  better way to do this???
*/
	size_t mask;
	int p = (int) ptr;
	(void) message;

	switch( byte_alignment )
	{
		case 0:
			mask = 4 - 1;
			break;
		case 2:
			mask = 2 - 1;
			break;
		case 4:
			mask = 4 - 1;
			break;
		case 8:
			mask = 8 - 1;
			break;
		default:
			mask = 4 - 1;
			break;
	}
	p = p & mask;
	if ( 0 != p )
	{
		Error_Log( MODULE_MEM_NAME, __func__, __LINE__, EC_MEM_MISALIGNMENT );
	}
}
#endif /* _DEBUG */

