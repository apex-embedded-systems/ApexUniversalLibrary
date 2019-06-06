#ifndef  TARGET_H_
#define  TARGET_H_
/********************************************************************************
_____________________________________________________________________________  
Description:

Target specific details.  This will likely vary from CPU to CPU.
_____________________________________________________________________________
Purpose:

_____________________________________________________________________________
Remark:


_____________________________________________________________________________
Example:

_____________________________________________________________________________
Copyright Notice:
    
	COPYRIGHT NOTICE Copyright (c) 2014-15 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded 
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


*********************************************************************************/
#if(0)
/* WIN32;_DEBUG;_WINDOWS;_USRDLL;AUL_LIB_DLL_EXPORTS;%(PreprocessorDefinitions) */
/* uncomment for WIN32 DLL interface  */
/* #if defined( AUL_LIB_DLL_EXPORTS ) */
/* # define AUL_EXPORTS  1           */
/* #else                             */
/* # define AUL_EXPORTS              */
/* #endif                           */
#endif

/******************************************************************************
Select how the I/O transaction will be delivered to the hardware or OS.
*/
/* uncomment for I/O simulation */
/* Depreciate -- done by make file */
#define TARGET_IO_SIMULATION  1

/* CPU driven:  must be either 1 or 2  */
/* #define TARGET_BUS_APERTURE_WIDTH_BYTES  1 */
#define TARGET_BUS_APERTURE_WIDTH_BYTES  2

/* defines default address space that will be used 
	IO_DIRECT  = 0  (x86 I/O space)
	MEM_DIRECT = 1  (not implemented, use for ARM)
	If commented out it will default to IO_DIRECT (x86 I/O)
*/
#define TARGET_DEFAULT_ADDRESS_SPACE  0

/******************************************************************************
Description
If defined, then we include all string acronyms and struct types (at the 
beginning of each struct.  

The goal is that once a check build has been verified, most of the debugging stuff
can be removed and code size reduced.
*/
#define TARGET_CHECK_BUILD 1

/******************************************************************************
Description
Not fully implemented
Removes reporting and auto-code generation.
*/
#ifdef __KERNEL__
# ifdef TARGET_AUTO_CODE_GEN_REPORTS
#  undef TARGET_AUTO_CODE_GEN_REPORTS
# endif
#else
# define TARGET_AUTO_CODE_GEN_REPORTS 1
#endif

/*********************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* TARGET_H_ */

