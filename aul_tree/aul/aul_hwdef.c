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

#include "aul_hwdef.h"
#include "../shared/typebool.h"
/* #include "../shared/error.h"  */
/* #include "../shared/status.h"  */   /* uses aul error codes */
#include "../str_safe/str_safe.h"
#include "../shared/module.h"     /* status.h and error.h */
#include "../shared/device.h"   /* number of devices supported */
#include "../devices/dev_avail.h" /* */
#include "../shared/cmd_macros.h"
/*********************************************************************************/
#define AULHWDEF_MODULE_REV  "$Date: 2015-01-28 06:21:36 -0600 (Wed, 28 Jan 2015) $"

/*******************************************************************************/
#if ( defined( __KERNEL__ ) && defined( __linux__ ) )
# error ( "COMPILATION ERROR: AUL Hardware Definition Requires stdio.h which is not supported in the kernel" );
#else
# include <stdio.h>  /* fopen(), fclose(), fgets(), feof() */
# include <stdlib.h> /* getenv() */
#endif

/*******************************************************************************/
#define AUL_HDF_COLUMN_QUANTITY  6
/*******************************************************************************/
typedef enum
{
	AUL_HDF_COLUMN_NONE					=  0,
	AUL_HDF_COLUMN_MNEMONIC				=  1,
	AUL_HDF_COLUMN_TYPE					=  2,
	AUL_HDF_COLUMN_BASE_ADDRESS			=  3,
	AUL_HDF_COLUMN_IRQ					=  4,
	AUL_HDF_COLUMN_BUS_RESTRICT_8BIT	=  5,
	AUL_HDF_COLUMN_UNKNOWN				= 99
} AUL_HDF_COLUMN_ENUM;
#if(0)
/*********************************************************************************/
/* TODO: duplicate in AULDRV */
/*NOTE:  These are really of the IOC_T type!!*/
typedef enum
{
	AUL_DEVICE_UNDEFINED          	= 0,
#if defined( DEVICE_TRACERE )
	AUL_DEVICE_TRACERE				= IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_TRACERE ),
#endif
#if defined( DEVICE_STX104ND )
	AUL_DEVICE_STX104ND				= IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_STX104ND ),
#endif
#if defined( DEVICE_STX104 )
	AUL_DEVICE_STX104				= IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_STX104 ),
#endif
#if defined( DEVICE_SUMMIT )
	AUL_DEVICE_SUMMIT				= IOC_CREATE_LOGICAL_ID_DEVICE( DEVICE_SUMMIT ),
#endif
} AUL_DEVICE_ENUM;
#endif
/*****************************************************/
typedef enum
{
	AUL_IRQ_NONE	= 0,
	AUL_IRQ_2		= 2,
	AUL_IRQ_3		= 3,
	AUL_IRQ_4		= 4,
	AUL_IRQ_5		= 5,
	AUL_IRQ_6		= 6,
	AUL_IRQ_7		= 7,
	AUL_IRQ_9		= 9,
	AUL_IRQ_10		= 10,
	AUL_IRQ_11		= 11,
	AUL_IRQ_12		= 12,
	AUL_IRQ_14		= 14,
	AUL_IRQ_15		= 15
} AUL_INT_IRQS_ENUM;
/*******************************************************************************/
#define AULHWDEF_DEFINITION_ACRONYM_LENGTH 16
/********************************************************************************
Description
This is either filled in by using the aul_hwdef.txt or by retrieving from 
Linux driver as defined in insmod.  There is a way to do this in WIN32, but
it is really messy.

This will become an opaque interface (abstract data type or ADT).  
Reason:  it keeps this totally separate from any other form of 
device definition information.
*/
struct aulhwdef_device_definition
{
	SYS_TYPE			type;   /* SYS_TYPE__aul_device_type */
	DEVICE_LOGICAL_ENUM	device_logical_id;
	size_t				io_region_offset; /* base address			*/
	size_t				io_region_bytes;  /* BAR contiguous bytes */
	size_t				io_interrupt_number;
/*	NOTE: TARGET_BUS_APERTURE_WIDTH_BYTES.  We might have to add something into
	      target (i.e. global or something) to provide over-riding restriction.*/
	size_t				io_restrict_8bit;	/* bus width restriction */
	char 				acronym[AULHWDEF_DEFINITION_ACRONYM_LENGTH];
};
typedef struct aulhwdef_device_definition aulhwdef_device_definition_type;

/*******************************************************************************
Description
Hardware definition list or table
*/
static aulhwdef_device_definition_type aulhwdef_device_definition[DEVICE_QTY] = { { 0 } };
/*******************************************************************************
*/
static void AULHwDef_String_Trim_Left( char* string )
{
	int src;
	int dst;
	BOOL non_space_copy;

	non_space_copy = false;	
	src = 0;
	dst = 0;
	while( string[src] )
	{
		if ( non_space_copy )
		{
			string[dst] = string[src];
			dst++;
		}
		else if ( ( string[src] != ' ' ) && ( string[src] != '\t' /* tab */ ) )
		{
			string[dst] = string[src];
			dst++;
			non_space_copy = true;
		}
		src++;
	}
	string[dst] = '\0';
}
/*******************************************************************************
*/
static void AULHwDef_String_Trim_Right( char* string )
{

	int src;
	int cnt;
	BOOL non_space_copy;

	non_space_copy = false;
	cnt = String_Length( string );
	src = cnt - 1;
	while( cnt )
	{
		if ( non_space_copy )
		{
			/* Do nothing */
		}
		else if ( ( string[src] != ' ' ) && ( string[src] != '\t' /* tab */ ) )
		{
			non_space_copy = true;
		}
		else
		{
			string[src] = '\0';
		}
		src--; cnt--;
	}
}
/*******************************************************************************
*/
static void AULHwDef_String_Trim_Left_Right( char* string )
{
	AULHwDef_String_Trim_Right( string );
	AULHwDef_String_Trim_Left( string );
}
/*******************************************************************************
*/
static void AULHwDef_String_Trim_End_CRLF( char* str_name )
{
	int str_len;
	
	str_len = String_Length( str_name );
	
	if ( str_len > 1 )
	{
		if ( str_name[str_len - 1] == '\r' /* carriage return */ ) 
		{
			str_name[str_len - 1] = '\0';
		}
		if ( str_name[str_len - 1] == '\n' /* line feed */ ) 
		{
			str_name[str_len - 1] = '\0';
		}
	}
	if ( str_len > 2 )
	{
		if ( str_name[str_len - 2] == '\r' /* carriage return */ ) 
		{
			str_name[str_len - 2] = '\0';
		}
		if ( str_name[str_len - 2] == '\n' /* line feed */  ) 
		{
			str_name[str_len - 2] = '\0';
		}
	}		 
}
/*******************************************************************************
*/
static unsigned char AULHwDef_String_Is_Comment( const char* str_name )
{
	unsigned char flag;
	int str_len;
	int str_cp;
	
/*TODO: this was used to decode register names, but not strings in general.*/

	flag = false;
	str_len = String_Length( str_name );
	
	if ( str_len == 0 ) 
	{
		flag = true;
	}
	else
	{
		for ( str_cp=0; str_cp < str_len; str_cp++ )
		{
/*TODO: does not make any sense.*/
			if ( (str_name[str_cp] != ' ') && (str_name[str_cp]=='#') ) 
			{
				flag = true;
				break;
			}
		}
	}
	return flag;	
}
/*******************************************************************************
*/
struct aulhwdef_list
{
	size_t				id;
	const char *      name;
};
#if(0)
/*************************************************************/
/* TODO: duplicate in AULDRV */
static const struct aulhwdef_list aulhwdef_type[] =
{
#if defined( DEVICE_STX104ND )
	{ DEVICE_LOGICAL_STX104ND,		DEVICE_STX104ND_NAME },
#endif
#if defined( DEVICE_STX104 )
	{ DEVICE_LOGICAL_STX104,		DEVICE_STX104_NAME	},
#endif
#if defined( DEVICE_TRACERE )
	{ DEVICE_LOGICAL_TRACERE,		DEVICE_TRACERE_NAME	},
#endif
#if defined( DEVICE_SUMMIT )
	{ AUL_DEVICE_SUMMIT,			DEVICE_SUMMIT_NAME	},
#endif
	{ DEVICE_LOGICAL_UNDEFINED,	NULL					}
};
#endif
/*************************************************************/
static const struct aulhwdef_list aulhwdef_column[] =
{ /* NOTE: mnemonoic or acronym can be used interchangably */
	{ AUL_HDF_COLUMN_MNEMONIC,				"mnemonic"		},  
	{ AUL_HDF_COLUMN_MNEMONIC,				"acronym"		},
	{ AUL_HDF_COLUMN_TYPE,					"type"			},
	{ AUL_HDF_COLUMN_BASE_ADDRESS,		"base_address"	},
	{ AUL_HDF_COLUMN_IRQ,					"irq"				},
	{ AUL_HDF_COLUMN_BUS_RESTRICT_8BIT,	"bus_8bit"		},
	{ AUL_HDF_COLUMN_UNKNOWN,				NULL				}
};

/*******************************************************************************
*/
static IOC_T AULHwDef_Board_Get_Type_From_String( const char * str_in )
{
	return (IOC_T) Device_Available_Find_Logical_Type_Using_Logical_Name( str_in );
#if(0)
	size_t			 index;
	const char *    name;  /* short hand */
	index = 0;
	do
	{	name = aulhwdef_type[index].name;
		if ( String_Compare( name, str_in, String_Length( name ) ) == 0 )
		{	return ( (IOC_T) aulhwdef_type[index].id );		}
		index++;
	} while ( DEVICE_LOGICAL_UNDEFINED != aulhwdef_type[index].id );
	return DEVICE_LOGICAL_UNDEFINED;
#endif
}
/*******************************************************************************
*/
static AUL_HDF_COLUMN_ENUM AULHwDef_Board_Get_Column_From_String( const char * str_in )
{
	size_t			 index;
	const char *    name;  /* short hand */
	index = 0;
	do
	{	name = aulhwdef_column[index].name;
		if ( String_Compare( name, str_in, String_Length( name ) ) == 0 )
		{	return ( (AUL_HDF_COLUMN_ENUM) aulhwdef_column[index].id );		}
		index++;
	} while ( AUL_HDF_COLUMN_UNKNOWN != ( (AUL_HDF_COLUMN_ENUM) aulhwdef_column[index].id ) );
	return AUL_HDF_COLUMN_UNKNOWN;
}
/*******************************************************************************
*/
static void AULHwDef_Split_File_Name(	const char *	name, 
													char *			directory, 
													int				dir_length, 
													char *			file_name, 
													int				file_name_length 
										      )
{
	int length;
	int cp;
	int cpd;
	int dir_last_char;

	length = String_Length( name );
	dir_last_char = -1;
	for( cp=length-1; cp > -1; cp-- )
	{
		if ( (name[cp] == '\\') || (name[cp] == '/') || (name[cp] == ':') )
		{
			dir_last_char = cp;
			cp = -1;
		}
	}
	/* directory portion of string */
	cpd = 0;
	if ( dir_last_char >= 0 )
	{
		cpd = dir_last_char + 1;
		if ( cpd >= (dir_length - 1) ) 
		{ /* not enough room to fit the directory string - return a null string */
			cpd = 0;
		}
		else
		{
			String_Copy( directory, name, cpd );
		}
	}
	directory[cpd] = '\0';
	/* filename portion of string */
	cp = 0;
	if ( length >= 0 )
	{
		cp = length - cpd;
		if ( cp > (file_name_length - 1) ) 
		{ /* not enough room to fit the filename string - return a null string */
			cp = 0;
		}
		else
		{
			String_Copy( file_name, name + cpd, cp );
		}
	}
	file_name[cp] = '\0';
}
/*******************************************************************************
*/
char * String_Tokenize( char * str, char ** token_state )
{
	char * cp;

	if ( NULL == str )
	{ /* return next token, if any */
		str = *token_state;
		if ( NULL == str ) return NULL;  /* no more tokens */
	}
	else
	{ /* find the start of current token */
		cp = str;
		while ( ( *cp == ',' ) && ( *cp != '\0' ) ) cp++;
		if ( *cp != '\0' )   str = cp;
	}
	/* find the next token position and put null character there */
	cp = str;
	while ( ( *cp != ',' ) && ( *cp != '\0' ) ) cp++;
	if ( cp != '\0' )
	{ /* found another delimiter */
		*cp = '\0';
		*token_state = cp + 1;  /* next token position */
	}
	else
	{ /* no more tokens */
		*token_state = NULL;
	}
	return str;
}
/*******************************************************************************
*/
enum {  AULHWDEF_DEVICE_MAX_STRING_SIZE = 256 };
/*******************************************************************************
*/
int AULHwDef_Device_Populate_Definitions( const char * device_definition_file_name )
{

#define AULDRV_FILENAME_LENGTH_MAX  256
/*DONE: TODO:  HARDWARE DEFINITION FILE HAS BEEN REORGANIZED AND THE COLUMNS ARE SWITCHED AROUND.  NEED TO UPDATE HERE.*/

/*DONE: TODO:  Parse directory and file name from device_definition_filename*/

	FILE * 	def_file;
	int 		device_count;
	int		error_code;
	aulhwdef_device_definition_type * 	dd;  		/* short for device_allocation */
	char 		temp_str[AULHWDEF_DEVICE_MAX_STRING_SIZE];
	char * 	str_ptr;
 	char * 	token_state;
	char * 	auldrv_dir_str;
	BOOL 		flag;
	int 		col_index;
	AUL_HDF_COLUMN_ENUM column[AUL_HDF_COLUMN_QUANTITY];
	char 		dir_str[AULDRV_FILENAME_LENGTH_MAX*2];
	char 		file_str[AULDRV_FILENAME_LENGTH_MAX];
	

	error_code = SUCCESS;
	def_file   = NULL;

	AULHwDef_Split_File_Name( device_definition_file_name, dir_str, AULDRV_FILENAME_LENGTH_MAX, file_str, AULDRV_FILENAME_LENGTH_MAX );


/*TODO:  THE DEVICE_DEFINITION DATA STRUCTURE IS NOT INITIALIZED!!!!!!!!
       NEED TO MAKE THAT HAPPEN!!


printf( "AUL_Device_Populate_Definitions: \n" );
printf( "  full=%s\n", device_definition_file_name );
printf( "  dir =%s\n", dir_str );
printf( "  file=%s\n", file_str );*/


	if ( (String_Length( dir_str ) == 0) && (String_Length( file_str ) == 0) )
	{ /* cannot split them apart - revert to original */
		def_file = fopen( device_definition_file_name, "rt" );

/*printf( "original directory and file name, use file name because string too long - hwd: <%s>\n", device_definition_file_name );*/
	}
	else
	{
		if ( String_Length( dir_str ) == 0 )
		{
			/* see if file is in current directory.  If not use one found by environment variable.*/
			def_file = fopen( device_definition_file_name, "rt" );
/*printf( "try original directory and file name - hwd: <%s>\n", device_definition_file_name );*/
			if ( def_file == NULL )
			{
				auldrv_dir_str = getenv( "AULDRV_DIR" );
/*printf( "[Y]\n" );*/
				if ( auldrv_dir_str == NULL ) 
				{
/*printf( "file not found\n" );*/
				}
				else if ( String_Length( auldrv_dir_str ) < (AULDRV_FILENAME_LENGTH_MAX - 1) )
				{

/*printf( "[X]\n" );*/
					String_Copy( dir_str, auldrv_dir_str, AULDRV_FILENAME_LENGTH_MAX*2	);
					String_Concatenate( dir_str, "\\", AULDRV_FILENAME_LENGTH_MAX*2		);
					String_Concatenate( dir_str, file_str, AULDRV_FILENAME_LENGTH_MAX*2  );

/*printf( "environment - hwd: <%s>\n", dir_str );*/

					def_file = fopen( dir_str, "rt" );
				}
				else
				{
/*printf( "strings are just too long\n" );*/
				}
			}
			else
			{
/*printf( "original file name - hwd: <%s>\n", device_definition_file_name );*/
			}
		}
		else
		{ 
			def_file = fopen( device_definition_file_name, "rt" );
/*printf( "same name - hwd: <%s>\n", device_definition_file_name );*/
		}
	}

	
	if ( def_file == NULL )
	{
		error_code = EC_AULHWDEF_CANNOT_FIND_DEFINITION_FILE;
		Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
/*		DRV_Error_Log( "AUL_Initialization: Error - cannot open definition file.\n" );
printf( "file not found\n" );
		result = EC_AULHWDEF_CANNOT_FIND_DEFINITION_FILE;*/
	}
/*	result = AUL_EC_SUCCESS;
	if ( ( def_file = fopen( device_definition_file_name, "rt" ) ) == NULL )
	{
		DRV_Error_Log( "AUL_Initialization: Error - cannot open definition file.\n" );
		result = AUL_EC_INIT_HW_CANNOT_FIND_DEFINITION_FILE;
	}*/
	else
	{
		/* first row is the column names */
/*DONE: TODO: link column names with decoding below.*/
		device_count = 0;
		flag = false;
		while( (!feof( def_file )) && (flag == false) )
		{
			fgets( temp_str, AULHWDEF_DEVICE_MAX_STRING_SIZE, def_file );
			AULHwDef_String_Trim_End_CRLF( temp_str );
			if ( AULHwDef_String_Is_Comment( temp_str ) || (String_Length( temp_str ) < 12) ) 
			{
				flag = false; /* assumed to be a comment, the AULHwDef_String_Is_Comment() not so good */
			}
			else
			{
				flag = true;  /* not a comment line */
				AULHwDef_String_Trim_End_CRLF( temp_str );
				for( col_index=0; col_index < AUL_HDF_COLUMN_QUANTITY; col_index++ )
				{ /* clear columns */
					column[col_index] = AUL_HDF_COLUMN_NONE;
				}
				token_state = NULL;
				for( col_index=0; col_index < AUL_HDF_COLUMN_QUANTITY; col_index++ )
				{
					if ( col_index == 0 ) str_ptr = String_Tokenize( temp_str, &token_state ); 
					else                  str_ptr = String_Tokenize( NULL,     &token_state );

					if ( NULL == str_ptr  ) break;
					if ( '\0' == *str_ptr ) break;

					AULHwDef_String_Trim_Left_Right( str_ptr );

					if ( str_ptr ) 
					{
						column[col_index] = AULHwDef_Board_Get_Column_From_String( str_ptr );
					}
					else
					{
/*						DRV_Error_Log( "AUL_Initialization: Error - definition file column name missing.\n" );
						result = EC_AULHWDEF_COLUMN_NAME_MISSING;*/
						error_code = EC_AULHWDEF_COLUMN_NAME_MISSING;
						Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
					}

					if ( column[col_index] == AUL_HDF_COLUMN_UNKNOWN )
					{
/*						DRV_Error_Log( "AUL_Initialization: Error - definition file column name unknown.\n" );
						result = EC_AULHWDEF_COLUMN_NAME_UNKNOWN;*/
						error_code = EC_AULHWDEF_COLUMN_NAME_UNKNOWN;
						Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
					}

				}
			}
		}

		if ( error_code ) return -error_code;

		if ( flag == false ) 
		{
/*			DRV_Error_Log( "AUL_Initialization: Error - definition file hit end-of-file.\n" );
			result = EC_AULHWDEF_FILE_LOAD; */
			error_code = EC_AULHWDEF_FILE_LOAD;  /* hit end-of-file */
			Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
			
		}	 
		else if ( SUCCESS == error_code )
		{
			/* read in the rest of the rows */
			while( (!feof( def_file )) && (device_count < DEVICE_QTY) )
			{
  				String_Copy( temp_str, "", AULHWDEF_DEVICE_MAX_STRING_SIZE ); 
				fgets( temp_str, AULHWDEF_DEVICE_MAX_STRING_SIZE, def_file );
				AULHwDef_String_Trim_End_CRLF( temp_str );
				/* skip line if too short or comment */
				if ( AULHwDef_String_Is_Comment( temp_str ) || (String_Length( temp_str ) < 12) )
				{
					/* skip the current line of text */
				}
				else
				{
					token_state = NULL;
					for( col_index=0; col_index < AUL_HDF_COLUMN_QUANTITY; col_index++ )
					{
						if ( AUL_HDF_COLUMN_NONE == column[col_index] ) break;

						if ( col_index == 0 ) str_ptr = String_Tokenize( temp_str, &token_state ); 
						else                  str_ptr = String_Tokenize( NULL,     &token_state );
						
						AULHwDef_String_Trim_Left_Right( str_ptr );
						/*dd = DRV_Device_Definition( device_count );*/
						dd = &(aulhwdef_device_definition[device_count]);
						switch( column[col_index] )
						{
							case AUL_HDF_COLUMN_MNEMONIC:
								/* acronym - fix to maximum of 16 characters including null string */
								if ( String_Length( str_ptr ) > AULHWDEF_DEFINITION_ACRONYM_LENGTH ) str_ptr[AULHWDEF_DEFINITION_ACRONYM_LENGTH - 1] = '\0';
								if ( str_ptr ) String_Copy( dd->acronym, str_ptr, AULHWDEF_DEFINITION_ACRONYM_LENGTH );
/*printf( "acronym=<%s>\n", dd->acronym );*/
								break;
							case AUL_HDF_COLUMN_TYPE:
								if ( str_ptr ) 
								{
									dd->device_logical_id = AULHwDef_Board_Get_Type_From_String( str_ptr );
/*printf( "type=%d\n", dd->device_logical_id );*/
								}
								break;
							case AUL_HDF_COLUMN_BASE_ADDRESS:
								if ( str_ptr ) dd->io_region_offset = (size_t) strtol(str_ptr, NULL, 0);
								else           dd->io_region_offset = 0;
/*printf( "io_region_offset=0x%04X\n", dd->io_region_offset );*/
								break;
							case AUL_HDF_COLUMN_BUS_RESTRICT_8BIT:
								if ( str_ptr ) dd->io_restrict_8bit = (size_t) strtol(str_ptr, NULL, 0);
								else           dd->io_restrict_8bit = 0;
/*printf( "io_restrict_8bit=%d\n", dd->io_restrict_8bit );*/
								break;
							case AUL_HDF_COLUMN_IRQ:
								if ( str_ptr ) dd->io_interrupt_number = (size_t) strtol(str_ptr, NULL, 0);
								else           dd->io_interrupt_number = 0;
/*printf( "IRQ=%d\n", dd->io_interrupt_number );*/
								break;
							default: break;
						}
					}

					/* acronym */
/*					str_ptr = strtok( temp_str, "," );
					AULHwDef_String_Trim_Left_Right( str_ptr );
					 mnemonic - fix to maximum of 16 characters including null string
					if ( String_Length( str_ptr ) > AULHWDEF_DEFINITION_ACRONYM_LENGTH ) str_ptr[AUL_DEVICE_DEFINITION_MNEMONIC_LENGTH - 1] = '\0';

					if ( str_ptr ) strcpy( dd->mnemonic, str_ptr );

					 board type
					str_ptr = strtok( NULL, "," );
					AULHwDef_String_Trim_Left_Right( str_ptr );
					if ( str_ptr )
					{
						dd->device_logical_id = AULHwDef_Board_Get_Type_From_String( str_ptr );
					}

					 base address
					str_ptr = strtok( NULL, "," );
					AULHwDef_String_Trim_Left_Right( str_ptr );
					if ( str_ptr ) dd->io_region_offset = (UINT16) strtol(str_ptr, NULL, 0);

					 irq
					str_ptr = strtok( NULL, "," );
					AULHwDef_String_Trim_Left_Right( str_ptr );
					if ( str_ptr ) dd->io_interrupt_number = (UINT16) strtol(str_ptr, NULL, 0);

	*/
					/* error checking left to right */
					switch( dd->io_interrupt_number )
					{
						case AUL_IRQ_NONE:
						case AUL_IRQ_2:
						case AUL_IRQ_3:
						case AUL_IRQ_4:
						case AUL_IRQ_5:
						case AUL_IRQ_6:
						case AUL_IRQ_7:
						case AUL_IRQ_9:
						case AUL_IRQ_10:
						case AUL_IRQ_11:
						case AUL_IRQ_12:
						case AUL_IRQ_14:
						case AUL_IRQ_15:
							break;
						default:
/*							DRV_Error_Log( "AUL_Initialization: Error - definition IRQ out of range.\n" );
							result = EC_AULHWDEF_IRQ_OUT_OF_RANGE;*/
							error_code = EC_AULHWDEF_IRQ_OUT_OF_RANGE;
							Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
							break;
					}
					if ( dd->io_region_offset < 0x100 )
					{ /* ISA region from 0x0000 to 0x0100 for motherboard typically */
/*						DRV_Error_Log( "AUL_Initialization: Error - definition base address out of range.\n" );
						result = EC_AULHWDEF_BASE_ADDRESS_OUT_OF_RANGE;*/
						error_code = EC_AULHWDEF_BASE_ADDRESS_OUT_OF_RANGE;
						Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
					}

					if ( dd->device_logical_id == DEVICE_LOGICAL_UNDEFINED )
					{
/*						DRV_Error_Log( "AUL_Initialization: Error - definition type undefined.\n" );
						result = EC_AULHWDEF_TYPE_UNDEFINED;*/
						error_code = EC_AULHWDEF_TYPE_UNDEFINED;
						Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
					}
					if ( String_Length(dd->acronym) == 0 ) 
					{
/*						DRV_Error_Log( "AUL_Initialization: Error - definition acronym length.\n" );
						result = EC_AULHWDEF_MNEMONIC_ERROR;*/
						error_code = EC_AULHWDEF_MNEMONIC_ERROR;
						Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
					}			

					switch( dd->device_logical_id )
					{
#if defined( DEVICE_STX104ND )
						case DEVICE_LOGICAL_STX104ND:
							dd->io_region_bytes  = 16;
/*
							dd->bar_number = 0;
							dd->interrupt_options = 0;  //edge triggered - not sure if correct.
*/
							break;
#endif
#if defined( DEVICE_STX104 )
						case DEVICE_LOGICAL_STX104:
							dd->io_region_bytes = 16;
/*
							dd->bar_number = 0;
							dd->interrupt_options = 0;  //edge triggered - not sure if correct.
*/
							break;
#endif
#if defined( DEVICE_TRACERE )
						case DEVICE_LOGICAL_TRACERE:
							dd->io_region_bytes = 16;
/*
							dd->bar_number = 0;
							dd->interrupt_options = 0;  //edge triggered - not sure if correct.
*/
							break;
#endif
#if defined( DEVICE_SUMMIT )
						case AUL_DEVICE_SUMMIT:
							dd->io_region_bytes = 16;
/*
							dd->bar_number = 0;
							dd->interrupt_options = 0;  //edge triggered - not sure if correct.
*/
							break;
#endif
						default:
							dd->io_region_bytes =  0;
/*
							dd->bar_number = 0;
							dd->interrupt_options = 0;  //edge triggered - not sure if correct.
*/
							break;
					}
					device_count++;
				}								
			} /* while */
			
			if ( device_count == 0 )  
			{
/*				DRV_Error_Log( "AUL_Initialization: Error - definition no devices found.\n" );
				result = EC_AULHWDEF_FILE_LOAD;*/
				error_code = EC_AULHWDEF_FILE_LOAD;
				Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
			}
		}
		fclose( def_file );
	}
	return -error_code;
}
/*******************************************************************************
*/
int AULHwDef_IsValid( void * hwdef )
{
	int    result = 0;
	aulhwdef_device_definition_type * d = (aulhwdef_device_definition_type * ) hwdef;
	if (  ( DEVICE_LOGICAL_UNDEFINED != d->device_logical_id	)  && 
			( 0             != d->io_region_offset				)  &&
			( 0             != d->io_region_bytes				)  &&
			( 0             != String_Length( d->acronym )  )   )
	{
			result = 1;
	}
	return result;
}
/*******************************************************************************
Description
Returns zero if the list is empty.  Returns non-zero if there is at least
one entry in the list.
*/
int AULHwDef_List_Is_Populated( void )
{
	size_t index;
	int    result;
	aulhwdef_device_definition_type * d; /* short hand */

	result = 0;
	for ( index = 0; index < DEVICE_QTY; index++ )
	{
		d = &(aulhwdef_device_definition[index]); 
		if ( AULHwDef_IsValid( d ) ) result++;
	}
	return result;
}
/*******************************************************************************
Returns an opaque pointer to the dataset.  If not found or not valid, returns
a null.
*/
int AULHwDef_Acronym_Find( const char * acronym, size_t * minor_number, void ** hwdef )
{
	size_t index;
	int	 error_code;
	aulhwdef_device_definition_type * d;  /* short-hand */

	for ( index = 0; index < DEVICE_QTY; index++ )
	{
		d = &(aulhwdef_device_definition[index]);
		if (  AULHwDef_IsValid( d ) )
		{
			if ( 0 == String_Compare( d->acronym, acronym, AULHWDEF_DEFINITION_ACRONYM_LENGTH ) )
			{
				char character;
				size_t len = String_Length( d->acronym );
				*hwdef = d;

				/* Extract the minor number.
				   MUST be from the last character of the acronym. 
					Examples: 
						summit0 ==> minor_number = 0
						summit1 ==> minor_number = 1
						summit  ==> minor_number = 0
				*/
				if ( len > 1 )
				{
					character = d->acronym[len - 1];
					if ( ( character >= '0' ) && ( character <= '3' ) ) *minor_number = (size_t)( character - '0' );
					else                                                *minor_number = 0;
				}
				else
				{
					*minor_number = 0;
				}
				return SUCCESS;
			}
		}
	}
	*hwdef = NULL;
	error_code = EC_AULHWDEF_ACRONYM_NOT_FOUND; 
	Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************************
*/
int AULHwDef_Parameters_Using_Index_Get(	size_t	 index,
								char *   acronym,
								IOC_T *	 logical_id_out,
								size_t * region_offset_out,
								size_t * region_bytes_out,
								size_t * interrupt_number_out,
								size_t * region_restrict_8bit_out
							)
{
	int    result;
	aulhwdef_device_definition_type * d; /* short hand */

	if ( index >= DEVICE_QTY ) goto AULHwDef_Parameters_Using_Index_Get_Error; 
	d = &(aulhwdef_device_definition[index]); 
	if ( AULHwDef_IsValid( d ) )
	{
		String_Copy( acronym, d->acronym, IO_DEFINITION_ACRONYM_SIZE );
		*logical_id_out				= d->device_logical_id;
		*region_offset_out			= d->io_region_offset;
		*region_bytes_out			= d->io_region_bytes;
		*interrupt_number_out		= d->io_interrupt_number;
		*region_restrict_8bit_out	= d->io_restrict_8bit;
		return SUCCESS;
	}
AULHwDef_Parameters_Using_Index_Get_Error:
	return -EC_AULHWDEF_ACRONYM_NOT_FOUND;
}
/*******************************************************************************
*/
int AULHwDef_Parameters_Get(	void *	 hwdef,
								IOC_T *	 logical_id_out,
								size_t * region_offset_out,
								size_t * region_bytes_out,
								size_t * interrupt_number_out,
								size_t * region_restrict_8bit_out
							)
{
	int error_code;
	aulhwdef_device_definition_type * d = (aulhwdef_device_definition_type * ) hwdef;

	if ( AULHwDef_IsValid( d ) )
	{
/*		if ( logical_id_in == d->device_logical_id )
		{*/
			*logical_id_out				= d->device_logical_id;
			*region_offset_out			= d->io_region_offset;
			*region_bytes_out			= d->io_region_bytes;
			*interrupt_number_out		= d->io_interrupt_number;
			*region_restrict_8bit_out	= d->io_restrict_8bit;
			error_code = SUCCESS;
/*		}
		else
		{
			error_code = EC_AULHWDEF_LOGICAL_ID;
			Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
		}*/
	}
	else
	{
		error_code = EC_AULHWDEF_INVALID_HWDEF; 
		Error_Log( MODULE_AULHWDEF_NAME, __func__, __LINE__, error_code );
	}
	return -error_code;
}

/*******************************************************************************
*/
int AULHwDef_Terminate( void )
{
	return SUCCESS;
}
/*******************************************************************************
*/
int AULHwDef_Initialize( void )
{
	return SUCCESS;
}
/*******************************************************************************
*/
module_definition_type module_aulhwdef_definition =
{
	SYS_TYPE_MAKE(module_definition_type),
	AULHwDef_Initialize,
	AULHwDef_Terminate,
	MODULE_AULHWDEF_NAME,
	AULHWDEF_MODULE_REV,
	NULL,
	AULHWDEF_STATUS_REV
};
