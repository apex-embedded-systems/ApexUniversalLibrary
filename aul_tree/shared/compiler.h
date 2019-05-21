#ifndef _COMPILER_H_
#define _COMPILER_H_
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

/*  NOTES:

_WIN32 versus WIN32
http://stackoverflow.com/questions/662084/whats-the-difference-between-the-win32-and-win32-defines-in-c

*/


/*********************************************************************************/
/*********************************************************************************/
/*                     < < <   COMPILER SPECIFICS   > > >                        */
/*********************************************************************************/
/*********************************************************************************/
/*  #define __BORLANDC__ 0x500  */

/*********************************************************************************/
/******* Specific compiler issues */
#ifdef __BORLANDC__
 #pragma message ( "Borland C detected...." )
#elif defined( __GNUC__ )
/*   #pragma message ( "Assuming GCC...." )  **/
#elif defined( __linux__ )
 #pragma message ( "Assuming GCC...." )
#else
 #pragma message ( "Assuming Microsoft Visual Studio (MSVC)...." )
#endif

/*********************************************************************************/
/* If COMPILER_APPLICATION then we are assuming application/library 
	layer and not kernel or driver space.
*/
#if ( defined( _WIN32 ) && defined( _MSC_VER ) )
# define COMPILER_APPLICATION 1
#elif defined( __MSDOS__ )
# define COMPILER_APPLICATION 1
#elif defined( __linux__ )
# ifdef __KERNEL__
#  pragma message ( "aul_tree: compiler.h __KERNEL__ kernel module layer" )
#  ifdef COMPILER_APPLICATION
#   undef COMPILER_APPLICATION
#  endif
# else
#  pragma message ( "aul_tree: compiler.h __linux__ application layer" )
#  define COMPILER_APPLICATION 1
# endif
#elif defined( __GNUC__ )
# ifdef __KERNEL__
#  pragma message ( "aul_tree: compiler.h __KERNEL__ kernel module layer" )
#  ifdef COMPILER_APPLICATION
#   undef COMPILER_APPLICATION
#  endif
# else
#  pragma message ( "aul_tree: compiler.h __linux__ application layer" )
#  define COMPILER_APPLICATION 1
# endif
#else
# error Compiler error: No compatible OS or target
#endif

/*********************************************************************************/
/* function inlining */
#ifndef INLINE
	#if defined(  __MSDOS__	)
	 #define INLINE inline
	#elif defined( __GNUC__ )
	 #define INLINE inline
	#elif defined( __linux__ )
	 #define INLINE inline
	#elif defined( _WIN32 )
	 /* use __forceinline (VC++ specific) */
	 #define INLINE __forceinline 
	#endif
#endif

/*******************************************************************************/
/* TODO: */
#if ( defined( WIN32 ) && defined( _MSC_VER ) )
# define __func__  __FUNCTION__
#endif 



#endif  /* _COMPILER_H_ */
