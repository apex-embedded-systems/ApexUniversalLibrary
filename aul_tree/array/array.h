#ifndef MODULE_ARRAY
#define MODULE_ARRAY       10   /* module ID number */
#define MODULE_ARRAY_NAME  "array"
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

#include "../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../shared/sysval.h"
#include "../shared/module_macros.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************************
Description
Verifies that the element is valid and that the array is valid.

Return
Returns negative value if an error has occurred, otherwise it returns zero.
*/
int Array_Is_Valid( void * array_element );

/*****************************************************************************************
Description
Searches for first element within the array.  Note that this algorithm must be smart enough
to know when it is out of bounds by checking the type field.

The element passed must have a valid 'type'.

Return
Null if not found or type is not valid.
*/
void *	Array_Find_First_Get( void * array_element );

/*****************************************************************************************
Description
Searches for first element within the array.  Note that this algorithm must be smart enough
to know when it is out of bounds by checking the type field.

The element passed must have a valid 'type'.

Return
Null if not found or type is not valid.
*/
void *	Array_Find_Last_Get( void * array_element );

/*****************************************************************************************
Description
The element passed must have a valid 'type'.

Returns
The size of the element in bytes
*/
int  Array_Element_Size_Get( void * array_element );

/*****************************************************************************************
Description
The element passed must have a valid 'type'.

Returns
The base type of the element.
*/
SYS_BASE_TYPE Array_Element_Base_Type_Get( void * array_element );

/*****************************************************************************************
Description
The element passed must have a valid 'type'.

Returns
The the number of elements within the array by inspecting the type information.
*/
int Array_Element_Quantity_Get( void * array_element );

/*****************************************************************************************
Description
Mark all elements as required.  This is an internal function only.
It will also fix-up any partially populated.  Basically, modifying the element position
and array size information within the 'type' field.
*/
int Array_Element_Mark_All( void *	array_base,		/* pointer to the array					*/
							size_t	element_size,	/* size of each element in the array	*/
							size_t	element_qty,	/* number of elements within the array	*/
							size_t	base_type,		/* base type							*/
							size_t	user_flag
					        );

/*****************************************************************************************
Description
This is used to mark the position of an element within the array.
*/
int Array_Mark_Element_As( SYS_TYPE_POSITION_ENUM position, void * array_element );

/*****************************************************************************************
Description
*/
int Array_Terminate( void );

/*****************************************************************************************
Description
*/
int Array_Initialize( void );

/*****************************************************************************************
*/
extern module_definition_type module_array_definition;

/*****************************************************************************************
*/
#ifdef __cplusplus
}
#endif

#endif /* MODULE_ARRAY */