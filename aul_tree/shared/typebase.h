#ifndef _TYPEBASE_H_
#define _TYPEBASE_H_
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



/*********************************************************************************/
/*********************************************************************************/
/*                            < < <   BASIC TYPES   > > >                        */
/*********************************************************************************/
/*********************************************************************************/

#if defined( __KERNEL__ )

	#include <linux/types.h>
	#include "../shared/pstdint.h"

#else /* __KERNEL__ */

	#include <limits.h>
	#include <stddef.h>
	#include "../shared/pstdint.h"

	#if defined( __linux__ )
		#include <stdint.h>
	#else
		#if defined( _PSTDINT_H_INCLUDED )
			# ifndef _BASETSD_H_
				typedef uint8_t  UINT8;
				typedef uint16_t UINT16;
				typedef uint32_t UINT32;
			# endif

		#else /* _PSTDINT_H_INCLUDED */
			/******* DEFINITIONS WHEN pstdint.h is not defined or included */
			/******* Legacy support */
			/* unsigned char type or 8-bit unsigned quantity. */
			# ifndef UINT8
			   typedef unsigned char UINT8;
			# endif

			# ifndef UINT8_MAX
			 /* Maximum unsigned char (8-bit value) or 255 or 0xFF. */
			#  define UINT8_MAX 0xFF
			# endif

			# ifndef UINT8_MIN
			 /* Minimum unsigned char (8-bit value) or 0 or 0x00. */
			#  define UINT8_MIN 0
			# endif

			# ifndef UINT16
			 /* unsigned short type or 16-bit unsigned quantity. */
			#  pragma message ( "UINT16 defined as unsigned short" )
			   typedef unsigned short UINT16;
			# endif

			# ifndef UINT16_MAX
			 /* Minimum unsigned short (16-bit value) or 65,535 or 0xFFFF. */
			#  define UINT16_MAX 0xFFFF
			# endif

			# ifndef UINT16_MIN
			 /* Minimum unsigned short (16-bit value) or 0 or 0x0000. */
			#  define UINT16_MIN 0
			# endif

			# ifndef UINT32
			 /* unsigned long type or 32-bit unsigned quantity. */
			#  pragma message ( "UINT32 defined as unsigned int" )
			   typedef unsigned int UINT32;
			# endif

			# ifndef UINT32_MAX
			 /* Maximum unsigned long (32-bit value) or 4,294,967,295 or
				0xFFFFFFFF.                                              */
			#  define UINT32_MAX 0xFFFFFFFFUL
			# endif

			# ifndef UINT32_MIN
			 /* Minimum unsigned long (32-bit value) or 0 or 0x00000000. */
			#  define UINT32_MIN 0
			# endif

		#endif  /* _PSTDINT_H_INCLUDED */

	#endif /* __linux__ */

#endif /* __KERNEL__ */

#endif  /* _TYPEBASE_H_ */


