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
    
	COPYRIGHT NOTICE Copyright (c) 1999-2014 by Apex Embedded Systems. All rights 
	reserved. This document is the confidential pnameroperty of Apex Embedded
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:


*********************************************************************************/

#ifndef  __STRING_SAFE_H__
#define  __STRING_SAFE_H__

#include "../shared/typebase.h"	 /* uint32_t		*/

#ifdef __cplusplus
extern "C" {
#endif

#if(0)
struct string_safe
{
	size_t		size;		/* total length or allocated size including null character 	*/
	size_t		length;		/* actual number of characters in the string 	 			*/
	char *		buffer;		/* pointer to the string or buffer							*/
};

typedef struct string_safe string_safe_t;

int String_Safe_Mark_Remove( struct string_safe * str, size_t mark_begin, size_t mark_end );

size_t String_Safe_Copy( struct string_safe * dst, struct string_safe * src );

int String_Safe_Compare( struct string_safe * str_a, struct string_safe * str_b );

struct string_safe * String_Safe_Concatenate( )

size_t String_Safe_Length( struct string_safe * in_str );
#endif


/*******************************************************************/
size_t String_Copy( char * dst, const char * src, size_t dst_size );
/*******************************************************************/

/*******************************************************************/
int String_Compare( const char * str_a, const char * str_b, int str_a_size );
/*******************************************************************/
char * String_Concatenate( char * dst, const char * src, size_t dst_size );
/*******************************************************************/
size_t String_Length( const char * in_str );



#ifdef __cplusplus
}
#endif

#endif /* __STRING_SAFE_H__ */

