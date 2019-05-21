/********************************************************************************
_____________________________________________________________________________  
Description:

_____________________________________________________________________________
Purpose:

_____________________________________________________________________________
Remark:

Used to provide WIN32 DLL support.

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

/*******************************************************************************/
#if defined( __linux__ )  /*---------------------------------------------------*/


	#pragma message ( "COMPILATION WARNING: Windows DLL main entry excluded." )


#elif defined( _WIN32 )   /*---------------------------------------------------*/




	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
	#include <windows.h>
	#include <malloc.h>  /* _alloca */
	#include <stdlib.h>  /* wcstombs */
	#include "../str_safe/str_safe.h"
	
	/******************************************************************/
	static HMODULE dll_module_handle = NULL;
	/******************************************************************/
	int Dll_Filename_Get( char * file_name, size_t size )
	{
		LPWSTR file_name_wide_string;
		size_t index;
		size_t length;
		size_t cp_start;
		size_t cp;

		file_name_wide_string = (LPWSTR) _alloca( sizeof( *file_name_wide_string ) * size );
		if ( NULL == file_name_wide_string ) return -1;

		size = GetModuleFileName( dll_module_handle, file_name_wide_string, size );

		if ( 0 == size ) return -1;

		wcstombs( file_name, file_name_wide_string, size );
		length = String_Length( file_name );
		index = length;
		cp_start = 0;
		do
		{
			index--;
			if (  ( '\\' == file_name[index] ) || 
				  ( '/'  == file_name[index] ) ||
				  ( ':'  == file_name[index] )  )	
			{
				cp_start = index + 1;
				index = 0;
			}
		} while ( index != 0 );

		if ( cp_start )
		{ /* Note: if cp_start is zero then we are done */
			index = 0;
			for ( cp = cp_start; cp <= length; cp++ )
			{ /* note: up to length in order to include null at end */
				file_name[index] = file_name[cp];
				index++;
			}
		}
		return size;
/*	Dll_Filename_Get_Error: */
		return 0;
	}

	/******************************************************************/
	BOOL APIENTRY DllMain( HMODULE hModule,
									DWORD  ul_reason_for_call,
									LPVOID lpReserved
							)
	{
		(void) lpReserved;

		dll_module_handle = hModule;

		switch (ul_reason_for_call)
		{
			case DLL_PROCESS_ATTACH: // A process is loading the DLL.
			case DLL_THREAD_ATTACH:  // A process is creating a new thread.
			case DLL_THREAD_DETACH:  // A thread exits normally.
			case DLL_PROCESS_DETACH: // A process unloads the DLL.
				break;
		}
		return TRUE;
	}

#endif     /*---------------------------------------------------*/
