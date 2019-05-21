/********************************************************************************
  _____________________________________________________________________________
  Description:

  _____________________________________________________________________________
  Remarks:
    _____________________________________________________________________________
  Example:
    _____________________________________________________________________________
  Copyright Notice:
    \COPYRIGHT NOTICE Copyright (c) 2012 by Apex Embedded
    Systems. All rights reserved. This document is the
    confidential property of Apex Embedded Systems Any
    reproduction or dissemination is prohibited unless
    specifically authorized in writing.
  _____________________________________________________________________________
  TODO:

  ********************************************************************************/

#include "array.h"
#include "../shared/mem.h"			 /* memory allocation      */
#include "../shared/status.h"     /* error codes and APIs	*/
#include "../shared/error.h"      /* error handling			*/
#include "../shared/device.h"		 /* device qty					*/
#include "../shared/debug.h"      /* debugging support		*/
#include <assert.h>

#include "../shared/module_macros.h"
#include "../shared/cmd_macros.h"
#include "../shared/sysval.h"

//#if defined( WIN32 )
//	#include <assert.h>
//	#include "../../target/target.h"
//	#include "../../driver/global/global.h"
//	#include "../sys/sysval.h"
//   #include "array.h"
//#elif defined( __MSDOS__ )
//	#include <assert.h>
// #if defined ( __FILE_STRUCTURE__FLAT__ )
//	#include "target.h"
//	#include "global.h"
//	#include "sysval.h"
//	#include "array.h"
// #else
//	#include "../target/target.h"
//	#include "../driver/global/global.h"
//	#include "../driver/sys/sysval.h"
//	#include "../driver/array/array.h"
// #endif
//#elif defined( __linux )
//	#include <assert.h>
//	#include "../../target/target.h"
//	#include "../../driver/global/global.h"
//	#include "../sys/sysval.h"
//   #include "array.h"
//#else
////create an error here
//#endif

/*******************************************************************/
#define ARRAY_MODULE_REV  "$Date: 2014-03-31 11:04:05 -0500 (Mon, 31 Mar 2014) $"


/*****************************************************************************************
*/

/*****************************************************************************************
Description
Verifies that the element is valid and that the array is valid.

Return
Returns negative value if an error has occurred, otherwise it returns zero.
*/
int Array_Is_Valid( void * array_element )
{
	SYS_TYPE  type;

	assert( array_element );
	type = *((SYS_TYPE *) array_element);

	if ( SYS_TYPE__NONE == TYPE_GET_BASE_TYPE( type ) ) return -EC_ARRAY_ELEMENT_TYPE;
	if ( 0 == TYPE_GET_ELEMENT_QTY( type )			  ) return -EC_ARRAY_ELEMENT_QTY;
	if ( TYPE_GET_ELEMENT_SIZE( type )	< sizeof( SYS_TYPE ) ) return -EC_ARRAY_ELEMENT_SIZE;	

	if ( NULL == Array_Find_First_Get( array_element ) ) return -EC_ARRAY_VALIDITY;
	
	if ( NULL == Array_Find_Last_Get( array_element ) ) return -EC_ARRAY_VALIDITY;

	return SUCCESS;
}

/*****************************************************************************************
Description
Searches for first element within the array.  Note that this algorithm must be smart enough
to know when it is out of bounds by checking the type field.

The element passed must have a valid 'type'.

Return
Null if not found or type is not valid.
*/
void *	Array_Find_First_Get( void * array_element )
{
	SYS_TYPE		type;
	SYS_BASE_TYPE	base_type;
	size_t			size;
	char *			cptr;

	assert( array_element );
	type = *((SYS_TYPE *) array_element);

#if defined( TARGET_CHECK_BUILD )
	if ( SYS_TYPE__NONE == TYPE_GET_BASE_TYPE( type ) ) return NULL;  //TODO: SYS_EC_ARRAY_ELEMENT_TYPE
#endif
	base_type	= TYPE_GET_BASE_TYPE( type );
	size		= TYPE_GET_ELEMENT_SIZE( type );
	cptr		= (char *) array_element;
	while ( !( ( SYS_TYPE_IS_ELEMENT_FIRST( type ) ) || ( SYS_TYPE_IS_ELEMENT_SINGLE( type ) ) ) )
	{
		cptr = cptr - size;
		type = *((SYS_TYPE *) cptr);
		if ( base_type != ((SYS_BASE_TYPE) TYPE_GET_BASE_TYPE( type )) ) return NULL;
	}
	return( (void *) cptr );
}

/*****************************************************************************************
Description
Searches for first element within the array.  Note that this algorithm must be smart enough
to know when it is out of bounds by checking the type field.  

The element passed must have a valid 'type'.

Return
Null if not found or type is not valid.
*/
void *	Array_Find_Last_Get( void * array_element )
{
	SYS_TYPE	type;
	size_t		base_type;
	size_t		size;
	char *		cptr;

	assert( array_element );
	type = *((SYS_TYPE *) array_element);
#if defined( TARGET_CHECK_BUILD )
	if ( SYS_TYPE__NONE == TYPE_GET_BASE_TYPE( type ) ) return NULL;  //TODO: SYS_EC_ARRAY_ELEMENT_TYPE
#endif
	base_type	= TYPE_GET_BASE_TYPE( type );
	size		= TYPE_GET_ELEMENT_SIZE( type );
	cptr		= (char *) array_element;
	while ( !( ( SYS_TYPE_IS_ELEMENT_LAST( type ) ) || ( SYS_TYPE_IS_ELEMENT_SINGLE( type ) ) ) )
	{
		cptr = cptr + size;
		type = *((SYS_TYPE *) cptr);
		if ( base_type != TYPE_GET_BASE_TYPE( type ) ) return NULL;
	}
	return( (void *) cptr );
}

/*****************************************************************************************
Description
The element passed must have a valid 'type'.

Returns
The size of the element in bytes
*/
int  Array_Element_Size_Get( void * array_element )
{
	SYS_TYPE  type;

	assert( array_element );
	type = *((SYS_TYPE *) array_element);
#if defined( TARGET_CHECK_BUILD )
	if ( SYS_TYPE__NONE == TYPE_GET_BASE_TYPE( type ) ) return -EC_ARRAY_ELEMENT_TYPE;
#endif
	return( TYPE_GET_ELEMENT_SIZE( type ) );
}

/*****************************************************************************************
Description
The element passed must have a valid 'type'.

Returns
The base type of the element.
*/
SYS_BASE_TYPE Array_Element_Base_Type_Get( void * array_element )
{
	SYS_TYPE  type;

	assert( array_element );
	type = *((SYS_TYPE *) array_element);
#if defined( TARGET_CHECK_BUILD )
	if ( SYS_TYPE__NONE == TYPE_GET_BASE_TYPE( type ) ) return -EC_ARRAY_ELEMENT_TYPE;
#endif
	return( TYPE_GET_BASE_TYPE( type ) );
}

/*****************************************************************************************
Description
Determine the array size, given a valid element.
*/
int Array_Element_Quantity_Get( void * array_element )
{
	SYS_TYPE  type;

	assert( array_element );
	type = *((SYS_TYPE *) array_element);
#if defined( TARGET_CHECK_BUILD )
	if ( SYS_TYPE__NONE == TYPE_GET_BASE_TYPE( type ) ) return -EC_ARRAY_ELEMENT_TYPE;
#endif
	return( TYPE_GET_ELEMENT_QTY( type ) );
}

/*****************************************************************************************
Description
Mark all elements as required.  This is an internal function only.
It will also fix-up any partially populated.  Basically, modifying the element position
and array size information within the 'type' field.

If a null element is required at the last location (i.e. null_is_last = true) then we make
sure that the type is mark as NULL (all zero values).  Why null at the end of an array?  
Three reasons: (1) support existing code, (2) implementation can be smaller than link lists, and
(3) we treat the array in a similar way as null terminated strings are treated.
*/
int Array_Element_Mark_All( void *	array_base,		/* pointer to the array					*/
							size_t	element_size,	/* size of each element in the array	*/
							size_t	element_qty,	/* number of elements within the array	*/
							size_t	base_type,		/* base type							*/
							size_t	user_flag
					      )
{
	size_t					index;
	SYS_TYPE *				type_ptr;
	char *					ptr;
	char *					ptr_base;
	SYS_TYPE_POSITION_ENUM	position;
	size_t					pos_last;

	if ( element_size < 2 ) return -EC_ARRAY_ELEMENT_SIZE;
	if ( element_qty < 1  ) return -EC_ARRAY_ELEMENT_QTY;

	ptr_base  = (char *) array_base;

	pos_last = element_qty - 1;

	if ( 1 == element_qty ) /* if only single element */
	{
		position = SYS_TYPE_POSITION_ELEMENT_SINGLE;
		type_ptr  = (SYS_TYPE *) ptr_base;
		*type_ptr = SYS_TYPE_CREATE( base_type, user_flag, position, element_qty, element_size );
	}
	else
	{
		for ( index = 0; index < element_qty; index++ )
		{
			ptr = ptr_base + ( element_size * index ); 
			if      ( pos_last == index ) position = SYS_TYPE_POSITION_ELEMENT_LAST;
			else if ( 0        == index ) position = SYS_TYPE_POSITION_ELEMENT_FIRST;
			else                          position = SYS_TYPE_POSITION_ELEMENT_MIDDLE;
		
			type_ptr = (SYS_TYPE *) ptr;
			*type_ptr = SYS_TYPE_CREATE( base_type, user_flag, position, element_qty, element_size );
		}
	}
	return SUCCESS;
}

/*****************************************************************************************
Description
This can only be used internally within the array functions.
*/
int Array_Mark_Element_As( SYS_TYPE_POSITION_ENUM position, void * array_element )
{
	SYS_TYPE  type;

	assert( array_element );
	type = *((SYS_TYPE *) array_element);
#if defined( TARGET_CHECK_BUILD )
	if ( SYS_TYPE__NONE == TYPE_GET_BASE_TYPE( type ) ) return -EC_ARRAY_ELEMENT_TYPE;
#endif
	
	*((SYS_TYPE *) array_element) = SYS_TYPE_SET_POSITION( type, position );

	return SUCCESS;
}

/*****************************************************************************************
Description
*/
int Array_Terminate( void )
{
	return SUCCESS;
}

/*****************************************************************************************
Description
*/
int Array_Initialize( void )
{
	return SUCCESS;
}
/*****************************************************************************************
*/
module_definition_type module_array_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	Array_Initialize,
	Array_Terminate,
	MODULE_ARRAY_NAME,
	ARRAY_MODULE_REV,
	NULL,
	ARRAY_STATUS_REV
};

