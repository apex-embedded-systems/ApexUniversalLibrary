#ifndef _TYPEBOOL_H_
#define _TYPEBOOL_H_
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

/*
http://stackoverflow.com/questions/2254075/using-true-and-false-in-c
http://stackoverflow.com/questions/8548521/trying-to-use-include-stdbool-h-in-vs-2010
*/

/*********************************************************************************/
/*********************************************************************************/
/*                          < < <   BOOLEAN TYPES   > > >                        */
/*********************************************************************************/
/*********************************************************************************/
/*
 * Complete rewrite conforming to C99 stdbool.h.
 */
/*********************************************************************************/

#if ( defined( __GNUC__ ) || defined( __linux__ ) )

	#ifndef __cplusplus		

	#define false 0
	#define true  1
	#define FALSE 0
	#define TRUE  1

	/* BEGIN - C99 non-conforming */
	/* typedef enum { FALSE = 0, TRUE = 1 } BOOL;  */
	typedef int BOOL;
/*	const BOOL TRUE  = (BOOL) true;
	const BOOL FALSE = (BOOL) false;*/
	/* END   - C99 non-conforming */

	#define bool  _Bool

	#if __STDC_VERSION__ < 199901L && __GNUC__ < 3
	typedef	int	_Bool;
	#endif

	#define	__bool_true_false_are_defined	1

	#endif  /* __cplusplus */


#elif ( defined( _WIN32 ) && defined( _MSC_VER ) )

	/* BEGIN - C99 non-conforming */
	typedef int BOOL;
	/* typedef enum { FALSE = 0, TRUE = 1 } BOOL;  */

	#define false 0
	#define true  1

	/* #define FALSE 0		*/
	/* #define TRUE  1	*/
	/* enum { false = 0, true = 1 };	*/
	enum { FALSE = 0, TRUE = 1 };

	/*  #ifndef __cplusplus	*/
	/* 	const BOOL false = 0;	*/
	/* 	const BOOL true  = 1;	*/

	/* #endif	*/

	/* const BOOL TRUE  = 1;	*/
	/* const BOOL FALSE = 0;	*/

	/* END   - C99 non-conforming */

	#define bool  _Bool

	#if __STDC_VERSION__ < 199901L && __GNUC__ < 3
	typedef	int	_Bool;
	#endif

	#define	__bool_true_false_are_defined	1

#else
/* POSSIBLE TODO: */
	# error Compiler error: No compatible OS or target
#endif	

/*********************************************************************************/
#endif  /* _TYPEBOOL_H_ */

