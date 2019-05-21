/********************************************************************************
_____________________________________________________________________________
Description:
	Safe string functions.
	These are portable safe string functions.  We wrote these based on standard
	library functions.  The Naming is more explicit (i.e. "strincmp" is now
	"String_Compare") to make code more readable.  Older versions of this code
	have been tested on many targets.
_____________________________________________________________________________
Purpose:

_____________________________________________________________________________
Remark:


_____________________________________________________________________________
Example:

_____________________________________________________________________________
Copyright Notice:

	COPYRIGHT NOTICE Copyright (c) 1999-2014 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded
	Systems Any reproduction or dissemination is prohibited unless specifically
	authorized in writing.

_____________________________________________________________________________
License:


*********************************************************************************/

#include "str_safe.h"

/*******************************************************************/
#define STRING_MODULE_REV  "$Date: 2015-12-01 09:20:47 -0600 (Tue, 01 Dec 2015) $"

/******************************************************************/
#define ASCII_NULL				'\0'
#define ASCII_LOWER_CASE_MASK	0x20

/******************************************************************/
enum { STRING_STR_A_NULL_TERMINATED = -1 };


/**********************************************************************************************************//**
 * @ingroup safe_string
 * @brief
 * The dst_size is the maximum amount that can be written to the
 * destination including null character, assuming that the dst is the beginning!
 * If dst_size is zero, then copies nothing.
 * Returns the number of characters copied.
 */
size_t String_Copy( char * dst, const char * src, size_t dst_size )
{
	size_t i;
   unsigned char character;

	if ( NULL == dst ) return 0;
	if ( 0 == dst_size ) return 0;
	if ( 1 == dst_size ) { *dst = ASCII_NULL; return 1; }

    i = 0;
	while( i < dst_size )
    {
    	character = src[i];
    	dst[i] = character;
        i++;
        if ( ASCII_NULL == character ) return( i );
    }
	if ( i > 0 ) i--;
    dst[i] = ASCII_NULL;
    return i;
}
/**********************************************************************************************************//**
 * @ingroup safe_string
 * @brief
 * Non-case sensitive string compare. Requires valid string pointers.
 * the str_a_size is the maximum storage at str_a including null character.
 * This compare is case independent.
 */
int String_Compare( const char * str_a, const char * str_b, int str_a_size )
{
	int   index;
	char  char_a, char_b;
	int   difference;
	int   term_a;  /* terminate (worse case) on str_a null */

	if ( 0 == str_a_size ) return 0;
	if ( ( NULL == str_a ) && ( NULL == str_b ) )  return 0;
	if ( NULL == str_a ) return -str_a_size; /* not sure best number */
	if ( NULL == str_b ) return str_a_size;  /* not sure best number */

	if ( STRING_STR_A_NULL_TERMINATED == str_a_size )  term_a = 1;
	else                                               term_a = 0;

	index = 0;
	do
	{
		char_a = str_a[index];
		char_b = str_b[index];

		if ( char_a ) char_a = char_a | ASCII_LOWER_CASE_MASK;
		if ( char_b ) char_b = char_b | ASCII_LOWER_CASE_MASK;

		difference = ((unsigned char) char_a) - ((unsigned char) char_b);
		index++;
	} while ( ( 0 == difference       )				&& 
		      ( ASCII_NULL != char_a  )				&& 
			  ( ASCII_NULL != char_b  )				&&
			  (( index <  str_a_size  ) || term_a) );  /* was index <=  str_a_size */
  		
	return difference;	
}
/**********************************************************************************************************//**
 * @ingroup safe_string
 * @brief
 * string concatenation
 * Copies no more than the size of the destination.  This assumes that the
 * dst pointer is at the beginning of the string (which is typically most
 * always is).  The size must include the null character
 */
char * String_Concatenate( char * dst, const char * src, size_t dst_size )
{
	char *ret = dst;

	if ( NULL == dst      ) return dst;
	if ( 0    == dst_size ) return dst;

	if ( 1 == dst_size ) { *dst = ASCII_NULL; return ret; }

	/* get to the end of the destination string */
	while ( *dst && dst_size ) { dst++;  dst_size--; }

	/* Copy character by character until 
	   null encountered at source or count 
	   is zero. 
	*/
	if ( dst_size ) dst_size--;
	while( dst_size )
	{
		if ( !( *dst++ = *src++ ) ) return ret;
		dst_size--;
	}
 
	/* apply null character at the end of destination string */
	*dst = 0;

	return ret;
}
/**********************************************************************************************************//**
 * @ingroup safe_string
 * @brief
 */
size_t String_Length( const char * in_str )
{
	const char * char_ptr;
	if ( NULL == in_str ) return 0;
	for ( char_ptr = in_str; *char_ptr != ASCII_NULL; char_ptr++ ){ }
	return( char_ptr - in_str );
}
