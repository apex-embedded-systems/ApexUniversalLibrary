#ifndef _CALLCONV_H_
#define _CALLCONV_H_
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

#include "target.h"

/*********************************************************************************/
/*********************************************************************************/
/*                    < < <   CALLING CONVENTIONS   > > >                        */
/*********************************************************************************/
/*********************************************************************************/

#ifndef AUL_LIB_DLL_EXPORTS
# define AUL_LIB_DLL_EXPORTS  1
#endif

/*******************************************************************************/
#define _CALLCONV_H_REV  "$Date: 2015-02-05 05:44:04 -0600 (Thu, 05 Feb 2015) $"
/*******************************************************************************/

/******* Calling conventions for DLL or library interfaces  */
/* NOTE: Changed from CALLCONV to AULCALLCONV to avoid conflict in oaidl.h */
#if defined( __BORLANDC__ )
 #ifdef AUL_EXPORTS
  #define AUL_DLL_API __declspec(dllexport)
  #define AULCALLCONV __stdcall
  /* #define AULCALLCONV */
  #pragma message ( "Borland DLL export...." )
 #else
  #define AUL_DLL_API __declspec(dllimport)
  #define AULCALLCONV __stdcall
  /* #define AULCALLCONV */
  #pragma message ( "Borland DLL import...." )
 #endif
#elif defined( __linux__ )
# define AUL_DLL_API
# define AULCALLCONV __attribute__((stdcall))
#elif defined( __GNUC__ )
# define AUL_DLL_API
# define AULCALLCONV __attribute__((stdcall))
#elif defined( _WIN32 )
# ifdef AUL_LIB_DLL_EXPORTS
#  define AUL_DLL_API __declspec(dllexport)
#  define AULCALLCONV __stdcall
#  pragma message ( "MSVS DLL export...." )
# else
#  define AUL_DLL_API __declspec(dllimport)
#  define AULCALLCONV __stdcall
#  pragma message ( "MSVS DLL import...." )
# endif
#else
 #pragma message ( "WARNING: Unknown compilation condition" )
 #define AUL_DLL_API
 #define AULCALLCONV
#endif

#endif  /* _CALLCONV_H_ */

