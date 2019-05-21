#ifndef STATUS_MACROS_H_
#define STATUS_MACROS_H_
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

/********************************************************************************/
#define SUCCESS  		0
/* alias success values		 */
#define EC_SUCCESS  	SUCCESS
#define AUL_EC_SUCCESS	SUCCESS

/********************************************************************************/

#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/target.h"

/*********************************************************************************/
typedef enum
{
	STATUS_CSV_COLUMN_NONE						=  0,
	STATUS_CSV_COLUMN_ENUM_VALUE				=  1,
	STATUS_CSV_COLUMN_ENUM_SYMBOL				=  2,
	STATUS_CSV_COLUMN_MESSAGE					=  3,
	STATUS_CSV_COLUMN_MODULE_NAME				=  4,
	STATUS_CSV_COLUMN_UNKNOWN					=  5
} STATUS_CSV_COLUMN_ENUM;
/********************************************************************************/

/* Attempting to avoid other kernel error codes */
#define EC_OFFSET 2048

/*
http://stackoverflow.com/questions/3957252/is-there-any-way-to-compute-the-width-of-an-integer-type-at-compile-time
Number of bits in inttype_MAX, or in any (1<<k)-1 where 0 <= k < 2040
#define IMAX_BITS(m) ( (m)/((m)%255+1) / 255%255*8 + 7-86/((m)%255+12) )

#define STATUS_INT_WIDTH(x) IMAX_BITS(x)

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#pragma message( "STATUS_INT_WIDTH = " VALUE( STATUS_INT_WIDTH( INT_MAX ) ) )

#if ( STATUS_INT_WIDTH(x) >= -2 )
#pragma message( " status integer width greater than 30 " )
#endif
#pragma message( "STATUS_MACROS_H_" )
*/

/* Error code build */
/* TODO: bit width assumed! */
#define EC_BIT_CODE   16
#define EC_BIT_ID     16

#define EC_MASK_CODE  ( ( 1 << EC_BIT_CODE ) - 1 )
#define EC_MASK_ID    ( ( 1 << EC_BIT_ID   ) - 1 )

#define EC_SHIFT_CODE  0
#define EC_SHIFT_ID    ( EC_SHIFT_CODE + EC_BIT_CODE )

#define EC_BUILD(id,ec)											 	\
 ( ( ( (id)				& EC_MASK_ID		) << EC_SHIFT_ID	) | \
   ( ( (ec + EC_OFFSET)	& EC_MASK_CODE		) << EC_SHIFT_CODE	) )

#define EC_GET_ID(x)   (   ( (x) >> EC_SHIFT_ID   ) & EC_MASK_ID   )
#define EC_GET_CODE(x) ( ( ( (x) >> EC_SHIFT_CODE ) & EC_MASK_CODE ) - EC_OFFSET )

#define EC_BUILD_ID0(ec) EC_BUILD(0,ec)

/*
extern unsigned int __STATUS_MACROS_H_LINE_101__EC_BUILD_OS_error_code_out_of_range;

#define EC_BUILD_OS(ec)  (ec) < UINT16_MAX						?			\
								EC_BUILD_ID0(ec)				:			\
								__STATUS_MACROS_H_LINE_101__EC_BUILD_OS_error_code_out_of_range
*/

#define EC_ID_IS_OS(ec) ( !( (ec) & ( EC_MASK_ID << EC_SHIFT_ID ) ) )

/********************************************************************************/
/*** Macros used to extract the column(s) of interest from ERROR_DEFINITIONS macro */
#define ERROR_CODE_EXTRACTOR( id, error_name, error_no, error_str ) error_name = EC_BUILD(id, error_no),

#if ( TARGET_AUTO_CODE_GEN_REPORTS == 1 )
# define ERROR_DS_EXTRACTOR(  id, error_name, error_no, error_str ) { error_name, #error_name, error_str },
#else
# define ERROR_DS_EXTRACTOR(  id, error_name, error_no, error_str ) { error_name, error_str },
#endif

/********************************************************************************/

#endif /* STATUS_MACROS_H_ */
