 /*
 * idi.c
 *
 *  Created on: Feb 25, 2015
 *      Author: Mike
 */


/* TODO:
 * 1. review function names for consistency in naming.
 * 2. comments above function definitions and any other definitions not complete.
 * 3. software review.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#if defined( __MSDOS__ )
# include <mem.h>
# include <dos.h>  /* outportb() and inportb() */
#else
# ifndef strcmpi
#  define strcmpi strcasecmp
# endif
# include <stdint.h>
#endif


#define IDI_REV  "$Date: 2015-04-03 21:04:55 -0500 (Fri, 03 Apr 2015) $"


/**********************************************************************************************************//**
 * @ingroup idi
 * \brief
 */
#if defined( __MSDOS__ )
 typedef unsigned char uint8_t;
 typedef unsigned int  uint16_t;  /* DOS only */
 typedef unsigned long uint32_t;  /* DOS only */

 typedef char int8_t;
 typedef int  int16_t;  /* DOS only */
 typedef long int32_t;  /* DOS only */
#endif
/**********************************************************************************************************//**
 * @ingroup idi
 * \brief
 */
typedef int BOOL;
#define false 0
#define true  1
enum { FALSE = 0, TRUE = 1 };

/**********************************************************************************************************//**
 * @ingroup idi
 * \brief
 * Board clock period
 */
#define CLOCK_PERIOD_SEC   20.0e-9

/**********************************************************************************************************//**
 * @ingroup idi
 * \brief
 * These are the unique IDs assigned to the hardware components.  If there is a firmware revision within
 * any of these components within the board, a new ID will be assigned.  The philosophy behind the ID scheme
 * is that it embodies both a unique ID and revision information in a purely arbitrary scheme.  It assumes
 * that all components defined within the system will never have the same ID numbers.  We maintain a unique
 * list of those ID numbers.  We will provide a list to customers as required.
 *
 * Software uses these ID numbers to determine which portion of the driver/library to apply to that portion
 * of the hardware. For example, if we end up having DINs with different ID numbers, this simply means that
 * the each will require (potentially) a unique portion of driver/library in order to properly use that
 * portion of the hardware.  This requires that the library/driver has some knowledge related to the ID numbers
 * assigned.
 *
 * At this point, an ID=0x8012 implies an Isolated Digital Input component that has a register map identical
 * to the WinSystems Opto48 card.  The actual DIN48 component has a more straight forward register mapping, but within the
 * STD-bus space we have re-arranged it to appear like the WinSystems Opto48 card.
 *
 * Similarly with the SPI component, we have mangled the register set in order that it fit within the constrained
 * register space.
 *
 * One could argue that there ought to be a nibble for revision and an upper nibble for the component ID itself,
 * but quickly you find out the limitation of that implementation.  If we simply use an arbitrary list from 1 to 65535
 * we can fit it within 16-bits and it will be a fairly long time until it is filled.  In a couple years we will
 * likely move to 32-bit and simply continue where we left off.  The software really does not care, so long as the
 * numbers are always unique.  We can use the uniqueness of the IDs along with some implied intelligence
 * (i.e. what board is this, what bus are we on, and perhaps a little bit of knowledge of the component itself)
 * we can always build and grow software to accommodate new revisions and still support older revision hardware.
 *
 */
enum
{
	ID_DIN = 0x8012,  /**< DIN48 ID   */
	ID_SPI = 0x8013   /**< SPI   ID   */
};

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/



/**********************************************************************************************************//**
 * @ingroup idi
 * \brief
 */
#define IDI_ERROR_CODES(_) \
	/*    enum_symbol             		   code		human readable        	   */ \
		_( SUCCESS,							0,		""							) \
		_( EC_BUFFER_TOO_LARGE, 			1,      "buffer too large"			) \
		_( EC_DIRECTION,        			2,      "direction"					) \
		_( EC_PARAMETER,	    			3,      "parameter"					) \
		_( EC_NOT_FOUND,        			4,		"not found"					) \
		_( EC_PARAMETER_MISSING,			5,		"missing parameter"			) \
		_( EC_SPI_ECD_OUT_OF_RANGE, 	   20,  	"SPI ECD range"				) \
		_( EC_SPI_HALF_CLOCK_OUT_OF_RANGE, 21, 		"SPI half clock range"		) \
		_( EC_SPI_CSB_OUT_OF_RANGE,  	   22, 		"SPI CSB range"				) \
		_( EC_SPI_NOT_FOUND, 		 	   23, 		"SPI not found"				) \


/*******************************************************************/
#define EC_EXTRACT_ENUM(symbol,code,message) symbol = code,

//#define EC_EXTRACT_DEFINITION(symbol,code,message) { read_write, bank, offset, name },
//#define EC_DEFINITION_NONE { SUCCESS, 0, "" }


typedef enum
{
	IDI_ERROR_CODES( EC_EXTRACT_ENUM )
} EC_ENUM;  /* EC = Error Code */

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************//**
 * @ingroup idi
 * \brief
 */
typedef enum
{
	IDI_BANK_0			= 0x00,
	IDI_BANK_1			= 0x40,
	IDI_BANK_2			= 0x80,
	IDI_BANK_3			= 0xC0,
	IDI_BANK_4			= 0x20,
	IDI_BANK_5			= 0x60,
	IDI_BANK_6			= 0xA0,
	IDI_BANK_7			= 0xE0,
	IDI_BANK_NONE   	= 0xFE, /**< indicates exclusive of bank address        */
	IDI_BANK_UNDEFINED  = 0xFF  /**< indicates that no banking has been defined */
} IDI_BANK_ENUM;

typedef enum
{
	REG_DIR_NONE 		= 0x00,
	REG_DIR_READ 		= 0x01,
	REG_DIR_WRITE 		= 0x02,
	REG_DIR_READ_WRITE	= 0x03
} REG_DIR_ENUM;



/**********************************************************************************************************//**
 * @ingroup idi
 * \brief
 */
#define IDI_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol          	 logical   physical  byte   bus byte                                               */ \
/*    enum_symbol          	 address   offset    width  aperture   read/write      acronym	      bank	       */ \
	/** DATA REGISTERS ***/ \
	_( IDI_DI_GROUP0,			 0,		0,		1,		1,		REG_DIR_READ,		"dig0",		IDI_BANK_NONE	) \
	_( IDI_DI_GROUP1,			 1,		1,		1,		1,		REG_DIR_READ,		"dig1",		IDI_BANK_NONE	) \
	_( IDI_DI_GROUP2,			 2,		2,		1,		1,		REG_DIR_READ,		"dig2",		IDI_BANK_NONE	) \
	_( IDI_DI_GROUP3,			 3,		3,		1,		1,		REG_DIR_READ,		"dig3",		IDI_BANK_NONE	) \
	_( IDI_DI_GROUP4,			 4,		4,		1,		1,		REG_DIR_READ,		"dig4",		IDI_BANK_NONE	) \
	_( IDI_DI_GROUP5,			 5,		5,		1,		1,		REG_DIR_READ,		"dig5",		IDI_BANK_NONE	) \
	/** STATUS REGISTER ***/ \
	_( IDI_INTR_BY_GROUP,		 6,		6,		1,		1,		REG_DIR_READ,		"isbg",		IDI_BANK_NONE	) \
	/** DATA / CONTROL REGISTERS ***/ \
	_( IDI_PEND_GROUP0,	 		 7,		8,		1,		1,		REG_DIR_READ,		"p0",		IDI_BANK_3		) \
	_( IDI_PEND_GROUP1,	 		 8,		9,		1,		1,		REG_DIR_READ,		"p1",		IDI_BANK_3		) \
	_( IDI_PEND_GROUP2,	 		 9,	   10,		1,		1,		REG_DIR_READ,		"p2",		IDI_BANK_3		) \
	_( IDI_PEND_GROUP3,			10,	   11,		1,		1,		REG_DIR_READ,		"p3",		IDI_BANK_3		) \
	_( IDI_PEND_GROUP4,			11,	   12,		1,		1,		REG_DIR_READ,		"p4",		IDI_BANK_3		) \
	_( IDI_PEND_GROUP5,			12,	   13,		1,		1,		REG_DIR_READ,		"p5",		IDI_BANK_3		) \
	_( IDI_CLEAR_GROUP0,	 	13,		8,		1,		1,		REG_DIR_WRITE,		"c0",		IDI_BANK_3		) \
	_( IDI_CLEAR_GROUP1,	 	14,		9,		1,		1,		REG_DIR_WRITE,		"c1",		IDI_BANK_3		) \
	_( IDI_CLEAR_GROUP2,		15,	   10,		1,		1,		REG_DIR_WRITE,		"c2",		IDI_BANK_3		) \
	_( IDI_CLEAR_GROUP3,		16,	   11,		1,		1,		REG_DIR_WRITE,		"c3",		IDI_BANK_3		) \
	_( IDI_CLEAR_GROUP4,		17,	   12,		1,		1,		REG_DIR_WRITE,		"c4",		IDI_BANK_3		) \
	_( IDI_CLEAR_GROUP5,		18,	   13,		1,		1,		REG_DIR_WRITE,		"c5",		IDI_BANK_3		) \
	/** CONTROL REGISTERS ***/ \
	_( IDI_BANK,				19,		7,		1,		1,		REG_DIR_READ_WRITE,	"bank",		IDI_BANK_NONE	) \
	_( IDI_ID_LSB,				20,    14,		1,		1,		REG_DIR_READ,		"idlsb",	IDI_BANK_NONE	) \
	_( IDI_ID_MSB,				21,    15,		1,		1,		REG_DIR_READ,		"idmsb",	IDI_BANK_NONE	) \
	_( IDI_ZERO0,				22,		8,		1,		1,		REG_DIR_READ,		"zb0",		IDI_BANK_0		) \
	_( IDI_ZERO1,				23,		9,		1,		1,		REG_DIR_READ,		"zb1",		IDI_BANK_0		) \
	_( IDI_ZERO2,				24,	   10,		1,		1,		REG_DIR_READ,		"zb2",		IDI_BANK_0		) \
	_( IDI_ZERO3,				25,	   11,		1,		1,		REG_DIR_READ,		"zb3",		IDI_BANK_0		) \
	_( IDI_ZERO4,				26,	   12,		1,		1,		REG_DIR_READ,		"zb4",		IDI_BANK_0		) \
	_( IDI_ZERO5,				27,	   13,		1,		1,		REG_DIR_READ,		"zb5",		IDI_BANK_0		) \
	/** CONFIG REGISTERS ***/ \
	_( IDI_EDGE_GROUP0,			28,	    8,		1,		1,		REG_DIR_READ_WRITE,	"ep0",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP1,			29,	    9,		1,		1,		REG_DIR_READ_WRITE,	"ep1",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP2,			30,	   10,		1,		1,		REG_DIR_READ_WRITE,	"ep2",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP3,			31,	   11,		1,		1,		REG_DIR_READ_WRITE,	"ep3",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP4,			32,	   12,		1,		1,		REG_DIR_READ_WRITE,	"ep4",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP5,			33,	   13,		1,		1,		REG_DIR_READ_WRITE,	"ep5",		IDI_BANK_1		) \
	_( IDI_INTR_BIT_GROUP0,		34,		8,		1,		1,		REG_DIR_READ_WRITE,	"ibe0",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP1,		35,		9,		1,		1,		REG_DIR_READ_WRITE,	"ibe1",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP2,		36,	   10,		1,		1,		REG_DIR_READ_WRITE,	"ibe2",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP3,		37,	   11,		1,		1,		REG_DIR_READ_WRITE,	"ibe3",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP4,		38,	   12,		1,		1,		REG_DIR_READ_WRITE,	"ibe4",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP5,		39,	   13,		1,		1,		REG_DIR_READ_WRITE,	"ibe5",		IDI_BANK_2		) \
	/** SPI REGISTERS ***/\
	_( SPI_ID_LSB,			40,	    8,		1,		1,		REG_DIR_READ,		"sidlsb",	IDI_BANK_6		) \
	_( SPI_ID_MSB,			41,	    9,		1,		1,		REG_DIR_READ,		"sidmsb",	IDI_BANK_6		) \
	_( SPI_CONFIG,			42,	   10,		1,		1,		REG_DIR_READ_WRITE,	"scfg",		IDI_BANK_6		) \
	_( SPI_ECD,				43,	   11,		1,		1,		REG_DIR_READ_WRITE,	"secd",		IDI_BANK_6		) \
	_( SPI_HCI_LSB,			44,	   12,		1,		1,		REG_DIR_READ_WRITE,	"shclsb",	IDI_BANK_6		) \
	_( SPI_HCI_MSB,			45,	   13,		1,		1,		REG_DIR_READ_WRITE,	"shcmsb",	IDI_BANK_6		) \
	_( SPI_DATA,			46,	    8,		1,		1,		REG_DIR_READ_WRITE,	"sdata",	IDI_BANK_7		) \
	_( SPI_TX_STATUS,		47,	    9,		1,		1,		REG_DIR_READ,		"stxf",		IDI_BANK_7		) \
	_( SPI_RX_STATUS,		48,	   10,		1,		1,		REG_DIR_READ,		"srxf",		IDI_BANK_7		) \
	_( SPI_COMMIT,			49,	   11,		1,		1,		REG_DIR_READ_WRITE,	"scmt",		IDI_BANK_7		) \
	_( IDI_UNDEFINED,			50,	    0,		0,		0,		REG_DIR_NONE,		"",			IDI_BANK_UNDEFINED	)


/*******************************************************************/
#define REG_BANK_SET(bank)          ( bank   <<   8 )
#define REG_OFFSET_SET(offset)      ( offset & 0xff )
#define REG_LOCATION_BANK_GET(location)      ( ( location >> 8 ) & 0xFF )
#define REG_LOCATION_OFFSET_GET(location)    ( location & 0xff )


#define REG_EXTRACT_ENUM(symbol,index,offset,register_bytes,aperture_bytes,read_write,name,bank) symbol = ( REG_BANK_SET(bank) | REG_OFFSET_SET(offset) ),

#define REG_EXTRACT_DEFINITION(symbol,index,offset,register_bytes,aperture_bytes,read_write,name,bank) { ( (IDI_REG_ENUM) (REG_BANK_SET(bank) | REG_OFFSET_SET(offset)) ), read_write, bank, offset, #symbol, name },

/*******************************************************************/
typedef enum
{
	IDI_REGISTER_SET_DEFINITION( REG_EXTRACT_ENUM )
} IDI_REG_ENUM;

/*******************************************************************/
struct reg_definition
{
	IDI_REG_ENUM	symbol;
	REG_DIR_ENUM	direction;
	IDI_BANK_ENUM	bank;
	uint16_t		physical_offset;
	char * 			symbol_name;
	char *			acronym;
};



/*******************************************************************/
typedef enum
{
	IDI_CSB_SOFTWARE	= 0,
	IDI_CSB_BUFFER		= 1,
	IDI_CSB_UINT8		= 2,
	IDI_CSB_UINT16		= 3
} SPI_CSB_ENUM;

/*******************************************************************/
enum { SPI_FIFO_SIZE = 16 };

/*******************************************************************/
struct spi_cfg
{
	BOOL				sdio_wrap;				/**< SDIO_WRAP		*/
	BOOL				sdo_polarity;			/**< SDO_POL		*/
	BOOL				sdi_polarity;			/**< SDI_POL		*/
	BOOL				sclk_phase;				/**< SCLK_PHA		*/
	BOOL				sclk_polarity;			/**< SCLK_POL		*/
	SPI_CSB_ENUM		chip_select_behavior; 	/**< CSB[2:0]		*/
	uint8_t				end_cycle_delay;		/**< ECD[7:0]		*/
	uint16_t			half_clock_interval;	/**< HCI[11:0]		*/

	double				clock_hz;				/**< if nonzero, code will compute half_clock_interval */
	double				end_delay_ns;			/**< if nonzero, code will compute end_cycle_dealy     */
};
/*******************************************************************/
struct spi_status
{
	BOOL	tx_status;  /* meaning this is specific to TX status, not RX status */
	BOOL	full;
	BOOL	empty;
	int		fifo_size;
	int		fifo_count;
};


/*******************************************************************/
#define IDI_DIN_GROUP_SIZE  8
#define IDI_DIN_SHIFT_RIGHT 3
#define IDI_DIN_GROUP_QTY   6
#define IDI_DIN_QTY			( IDI_DIN_GROUP_SIZE * IDI_DIN_GROUP_QTY )


struct din_cfg
{
	struct
	{
		BOOL	falling_edge;
		BOOL	interrupt_enable;
	} chan[IDI_DIN_QTY];
};


/*******************************************************************/
#define FRAM_BLOCK_SIZE   	 256
#define IDI_MESSAGE_SIZE     256
#define SPI_BLOCK_SIZE       256

struct idi_dataset
{
	struct din_cfg		din_cfg;
	struct spi_cfg 		spi_cfg;
	uint8_t				bank_previous;
	uint16_t			base_address;
	uint16_t			spi_id;

	/* this is a scratch pad for FRAM transactions */
	uint8_t				fram_block[FRAM_BLOCK_SIZE];

	/* this is a scratch pad for SPI transactions */
	uint8_t				spi_block[SPI_BLOCK_SIZE];

	/* this is a scratch pad area for report generation */
	char *  			message[IDI_MESSAGE_SIZE];
};





/**********************************************************************************************************//**
@ingroup idi
\brief
 Apex Universal Library Board independent digital port write function.

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/

/* globals */


struct reg_definition  definitions[] =
{
		IDI_REGISTER_SET_DEFINITION( REG_EXTRACT_DEFINITION )
};

struct idi_dataset  idi_dataset;

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/

//TODO: roll bank information into X-Macro and then auto-generate all this stuff.

static const char * idi_bank_symbol_names[] =
{
	"IDI_BANK_0",
	"IDI_BANK_1",
	"IDI_BANK_2",
	"IDI_BANK_3",
	"IDI_BANK_4",
	"IDI_BANK_5",
	"IDI_BANK_6",
	"IDI_BANK_7",
	"IDI_BANK_NONE",
	"IDI_BANK_UNDEFINED"
};

static const char * IDI_Symbol_Name_Bank( IDI_BANK_ENUM bank )
{
	int index;

	switch( bank )
	{
		case IDI_BANK_0:			index = 0;	break;
		case IDI_BANK_1:			index = 1;	break;
		case IDI_BANK_2:			index = 2;	break;
		case IDI_BANK_3:			index = 3;	break;
		case IDI_BANK_4:			index = 4;	break;
		case IDI_BANK_5:			index = 5;	break;
		case IDI_BANK_6:			index = 6;	break;
		case IDI_BANK_7:			index = 7;	break;
		case IDI_BANK_NONE:			index = 8;	break;
		case IDI_BANK_UNDEFINED:	index = 9;	break;
	}
	return idi_bank_symbol_names[index];
}

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
BOOL String_To_Bool( const char * str )
{
	switch( str[0] )
	{
	case '0':
	case 'f':
	case 'F':
		return false;
	case '1':
	case 't':
	case 'T':
		return true;
	}
	return false;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int Hex_Dump_Line( uint16_t address, uint8_t count, uint8_t * buffer, FILE * out )
{
	int 	index;
	char 	str_temp[8];
	char    str_ascii[20];
	char  	str_hex_list[64];

	//if ( count > 16 ) return -EC_BUFFER_TOO_LARGE;

	sprintf( str_hex_list, "%04X: ", ((int) address) );
	strcpy( str_ascii,    "" );
	for ( index = 0; index < ((int) count); index++ )
	{ 	/* append/build hex list */
		sprintf( str_temp, "%02X", buffer[index] );
		strcat( str_hex_list, str_temp );
		/* output spacer in the middle and end */
		if ( 0x07 == ( index & 0x07 ) ) strcat( str_hex_list, "  " );
		/* add a space after hex value and spacers */
		strcat( str_hex_list, " " );
		/* append/build ASCII list */
		if ( ( buffer[index] < ' ' ) || ( buffer[index] > '~' ) )
		{ /* since these characters will not display replace them with a period */
			strcat( str_ascii, "." );
		}
		else
		{ /* print the character as is */
			sprintf( str_temp, "%c", buffer[index] );
			strcat( str_ascii, str_temp );
		}
	}
	/* compute any remaining filler required to properly align ASCII portion */
	index = strlen( str_hex_list );
	/* total = ( 6 ) 'address characters' + ( 16 * 3 ) 'hex characters' + ( 2 * 3 ) 'spacers'  = 60 */
	count = 60 - index;
	while ( count > 0 )
	{ /* add sufficient characters so that the ASCII portion is in the proper columns */
		strcat( str_hex_list, " " );
		count--;
	}
	/* output the results */
	fprintf( out, "%s%s\n", str_hex_list, str_ascii );
	return SUCCESS;
}

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/

#define IDI_IO_DEBUG            1
#define IDI_IO_DIRECTION_TEST   1

/**********************************************************************************************************//**
@ingroup idi
\brief

Assumption:  single threaded application.

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
#if defined( IDI_IO_DEBUG )
static char * IO_Get_Symbol_Name( IDI_REG_ENUM location )
{
	int index;
	index =  0;
	while ( definitions[index].direction != REG_DIR_NONE )
	{
		if ( location == definitions[index].symbol  )
		{
			return definitions[index].symbol_name;
		}
		index++;
	}
	return NULL;
}
#endif

/**********************************************************************************************************//**
@ingroup idi
\brief

Assumption:  single threaded application.

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
#if defined( IDI_IO_DIRECTION_TEST )
static int IO_Direction_IsNotValid( IDI_REG_ENUM location, REG_DIR_ENUM direction )
{
	int index;
	index =  0;
	while ( definitions[index].direction != REG_DIR_NONE )
	{
		if ( location == definitions[index].symbol  )
		{
			if ( definitions[index].direction & direction ) return 0;
			else                                            return 1;
		}
		index++;
	}
	return 1;
}
#endif

/**********************************************************************************************************//**
@ingroup idi
\brief

Assumption:  single threaded application.

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int IO_Write_U8( IDI_REG_ENUM location, uint8_t value )
{
	uint8_t bank;
	uint8_t offset;
	uint16_t address;

#if defined( IDI_IO_DIRECTION_TEST )
	if ( IO_Direction_IsNotValid( location, REG_DIR_WRITE ) )
	{
# if defined( IDI_IO_DEBUG )
		printf( "IO_Write_U8: %s, error in direction\n", IO_Get_Symbol_Name( location ) );
# else
		printf( "IO_Write_U8: error in direction\n" );
# endif
		return -EC_DIRECTION;
	}
#endif

    bank   = (uint8_t) REG_LOCATION_BANK_GET( location );
	if ( ( IDI_BANK_NONE != bank ) && ( bank != idi_dataset.bank_previous ) )
	{   /* write to bank register only if different -- don't bother even checking it, will take too much time. */
		offset = (uint8_t) REG_LOCATION_OFFSET_GET( IDI_BANK );
		address = idi_dataset.base_address + offset;
		idi_dataset.bank_previous = bank;
#if defined( __MSDOS__ ) && !defined( IDI_IO_DEBUG )
		outportb( address, bank );
#else
		printf( "IO_Write_U8: %s, address = 0x%04X, bank = %s\n", IO_Get_Symbol_Name( IDI_BANK ), address, IDI_Symbol_Name_Bank( bank ) );
#endif
	}
	offset = (uint8_t) REG_LOCATION_OFFSET_GET(location);
	address = idi_dataset.base_address + offset;
#if defined( __MSDOS__ ) && !defined( IDI_IO_DEBUG )
	outportb( address, value );
#else
	printf( "IO_Write_U8: %s, address = 0x%04X, value = 0x%02X\n", IO_Get_Symbol_Name( location ), address, value );
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Assumption:  single threaded application.

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int IO_Read_U8( IDI_REG_ENUM location, uint8_t * value )
{
	uint8_t bank;
	uint8_t offset;
	uint16_t address;

#if defined( IDI_IO_DIRECTION_TEST )
	if ( IO_Direction_IsNotValid( location, REG_DIR_READ ) )
	{
# if defined( IDI_IO_DEBUG )
		printf( "IO_Read_U8: %s, error in direction\n", IO_Get_Symbol_Name( location ) );
# else
		printf( "IO_Read_U8: error in direction\n" );
# endif
		return -EC_DIRECTION;
	}
#endif

    bank   = (uint8_t) REG_LOCATION_BANK_GET( location );
	if ( ( IDI_BANK_NONE != bank ) && ( bank != idi_dataset.bank_previous ) )
	{   /* write to bank register only if different -- don't bother even checking it, will take too much time. */
		offset = (uint8_t) REG_LOCATION_OFFSET_GET( IDI_BANK );
		address = idi_dataset.base_address + offset;
		idi_dataset.bank_previous = bank;
#if defined( __MSDOS__ ) && !defined( IDI_IO_DEBUG )
		outportb( address, bank );
#else
		printf( "IO_Read_U8: %s, address = 0x%04X, bank = %s\n", IO_Get_Symbol_Name( IDI_BANK ), address, IDI_Symbol_Name_Bank( bank ) );
#endif
	}
	offset = (uint8_t) REG_LOCATION_OFFSET_GET(location);
	address = idi_dataset.base_address + offset;
#if defined( __MSDOS__ ) && !defined( IDI_IO_DEBUG )
	*value = inportb( address );
#else
	printf( "IO_Read_U8: %s, address = 0x%04X\n", IO_Get_Symbol_Name( location ), address );
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Assumption:  single threaded application.

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
void IO_Write_U16( IDI_REG_ENUM location, uint16_t value )
{
//TODO: assumes little endian.
	IO_Write_U8( ((int) location) + 0, (uint8_t)(     value      & 0xFF ) );
	IO_Write_U8( ((int) location) + 1, (uint8_t)( ( value >> 8 ) & 0xFF ) );
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Assumption:  single threaded application.

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
void IO_Read_U16( IDI_REG_ENUM location, uint16_t * value )
{
//TODO: assumes little endian.
	uint8_t lsb, msb;
	IO_Read_U8( ((int) location) + 0, &lsb );
	IO_Read_U8( ((int) location) + 1, &msb );
	*value = ( ((uint16_t) msb) << 8 ) | ( ((uint16_t) lsb) & 0xFF );
}



/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_DIN_Channel_Get( int channel, BOOL * value )
{
	int 	group;
	int 	bit;
	uint8_t reg_value;

	group = channel >> IDI_DIN_SHIFT_RIGHT;
	bit   = channel - group * IDI_DIN_GROUP_SIZE;

	IO_Read( IDI_DI_GROUP0 + group, &reg_value );

	if ( 0 != ( reg_value & ( 0x01 << bit ) ) ) *value = true;
	else                                        *value = false;

	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_DIN_Group_Get( int group, uint8_t * value )
{
	IO_Read( IDI_DI_GROUP0 + group, value );
	return SUCCESS;
}




/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int SPI_ID_Get( uint16_t * id )
{
	uint8_t   	scratch;
	uint16_t	id_temp;

	IO_Read( SPI_ID_LSB, &scratch );
	id_temp = (uint16_t) scratch;
	IO_Read( SPI_ID_MSB, &scratch );
	id_temp = ( id_temp << 8 ) | ((uint16_t) scratch);
	*id = id_temp;
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int SPI_IsNotPresent( void )
{
	uint16_t id;
	SPI_ID_Get( &id );
	if (ID_SPI == id ) return 0;
	return 1;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero negative error code.
*/
int SPI_Calculate_Half_Clock( double     half_clock_request_sec, /* requested half clock interval         	*/
							  double *   half_clock_actual_sec,  /*  computed actual half clock interval  	*/
							  double *   error,                  /* error between actual and desired   		*/
							  uint16_t * hci                     /* computed count                     		*/
							)
{
	double    	scratch;
	int			hci_temp;

	/* spi_half_clock_interval_sec = CLOCK_PERIOD_SEC * ( 4.0 + ( (double) hci ) ) */
	scratch = ( half_clock_request_sec / CLOCK_PERIOD_SEC ) - 4.0;
	hci_temp = (int) scratch;

	if ( ( hci_temp > 4095 ) || ( hci_temp < 0 ) ) return -EC_SPI_HALF_CLOCK_OUT_OF_RANGE;

	/* compute actual */
	scratch = CLOCK_PERIOD_SEC * ( 4.0 + ((double) hci_temp) );
	if ( NULL != error                 ) *error                 = ( scratch - half_clock_request_sec ) / half_clock_request_sec;
	if ( NULL != half_clock_actual_sec ) *half_clock_actual_sec = scratch;
	if ( NULL != hci                   ) *hci                   = (uint16_t) hci_temp;
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero negative error code.
*/
int SPI_Calculate_Clock( double     clock_request_hz, 	/* requested spi clock frequency         	*/
						 double *   clock_actual_hz,	/* computed actual clock frequency  		*/
						 double *   error,       		/* error between actual and desired   		*/
						 uint16_t * hci          		/* computed count                     		*/
						)
{
	int 	 error_code;
	double 	 half_clock_request_sec;
	double	 half_clock_actual_sec;
	double   error_internal;
	double	 scratch;
	uint16_t hci_internal;

	half_clock_request_sec = 1.0 / ( 2.0 * clock_request_hz );

	error_code = SPI_Calculate_Half_Clock( half_clock_request_sec,
										   &half_clock_actual_sec,
										   &error_internal,
										   &hci_internal
										  );
	if ( error_code ) return error_code;

	/* compute actual frequency */
	scratch = 1.0 / ( 2.0 * half_clock_actual_sec );
	if ( NULL != error                 ) *error           = ( scratch - clock_request_hz ) / clock_request_hz;
	if ( NULL != half_clock_actual_sec ) *clock_actual_hz = scratch;
	if ( NULL != hci                   ) *hci             = (uint16_t) hci_internal;
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief


\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero negative error code.
*/
int SPI_Calculate_End_Cycle_Delay( double    spi_half_clock_interval_sec, /* calculated half-clock interval     */
							       double    delay_request_sec,           /* requested end-delay interval       */
							       double *  delay_actual_sec,            /*  computed actual delay             */
							       double *  error,                       /* error between actual and desired   */
							       uint8_t * ecd                          /* computed count                     */
								)
{
	double    	delay_between_words_sec;
	double    	scratch;
	int			ecd_temp;

	/* delay_sec = CLOCK_PERIOD_SEC * 4 + ECD * spi_half_clock_interval_sec  */
	scratch = ( delay_request_sec - 4.0 * CLOCK_PERIOD_SEC ) / spi_half_clock_interval_sec;
	ecd_temp = (int) scratch;

	if ( ( ecd_temp > 255 ) || ( ecd_temp < 0 ) ) return -EC_SPI_ECD_OUT_OF_RANGE;

	/* compute actual */
	scratch = CLOCK_PERIOD_SEC * 4.0 + ((double) ecd_temp) * spi_half_clock_interval_sec;
	if ( NULL != error            ) *error            = ( scratch - delay_request_sec ) / delay_request_sec;
	if ( NULL != delay_actual_sec ) *delay_actual_sec = scratch;
	if ( NULL != ecd              ) *ecd              = (uint8_t) ecd_temp;
	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int SPI_Configuration_Set( struct spi_cfg * cfg )
{
	int			error_code;
	double 		scratch;
	double      hci_sec;     /* half clock interval in seconds */
	uint8_t   	config;

	if ( SPI_IsNotPresent() ) return -EC_SPI_NOT_FOUND;

	config = (uint8_t) ( ( cfg->chip_select_behavior & 0x07 ) << 4 );
	if ( cfg->sclk_polarity ) config |= 0x01;
	if ( cfg->sclk_phase    ) config |= 0x02;
	if ( cfg->sdi_polarity  ) config |= 0x04;
	if ( cfg->sdo_polarity  ) config |= 0x08;
	if ( cfg->sdio_wrap     ) config |= 0x80;

	IO_Write_U8( SPI_CONFIG, config );

	if ( cfg->clock_hz > 0 )
	{ /* compute half_clock_interval */
		//scratch = ( 1.0 - ( 8.0 * CLOCK_PERIOD_SEC * cfg->clock_hz ) ) / (  2.0 * CLOCK_PERIOD_SEC * cfg->clock_hz );
		error_code = SPI_Clock_Calculate( cfg->clock_hz, NULL, NULL, &(cfg->half_clock_interval) );
		if ( error_code ) return error_code;
	}
	hci_sec = CLOCK_PERIOD_SEC * ( 4 + cfg->half_clock_interval );

	if ( cfg->end_delay_ns > 0 )
	{
		scratch    = cfg->end_delay_ns * 1.0e-9;
		error_code = SPI_Calculate_End_Cycle_Delay( hci_sec, 				/* calculated half-clock interval     */
			       	   	   	   	   	   	   	   	    scratch,           		/* requested end-delay interval       */
			       	   	   	   	   	   	   	   	    NULL,            		/*  computed actual delay             */
			       	   	   	   	   	   	   	   	    NULL,                   /* error between actual and desired   */
			       	   	   	   	   	   	   	   	    &(cfg->end_cycle_delay) /* computed count                     */
			       	   	   	   	   	   	   	   	   );
		if ( error_code ) return error_code;
	}

	IO_Write_U8( SPI_HCI_LSB, (uint8_t)( cfg->half_clock_interval & 0xFF ) );
	IO_Write_U8( SPI_HCI_MSB, (uint8_t)( cfg->half_clock_interval >> 8   ) );
	IO_Write_U8( SPI_ECD, cfg->end_cycle_delay );

	memcpy( &(idi_dataset.spi_cfg), &cfg, sizeof( struct spi_cfg ) );

	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int SPI_Configuration_Get( struct spi_cfg * cfg )
{
	uint8_t   	scratch;

	if ( SPI_IsNotPresent() ) return -EC_SPI_NOT_FOUND;

	IO_Read( SPI_CONFIG, &scratch );
	cfg->chip_select_behavior 	= ( scratch >> 4 ) & 0x07;
    cfg->sclk_polarity		 	= (BOOL) ( scratch & 0x01 );
    cfg->sclk_phase   			= (BOOL) ( scratch & 0x02 );
	cfg->sdi_polarity 			= (BOOL) ( scratch & 0x04 );
	cfg->sdo_polarity 			= (BOOL) ( scratch & 0x08 );
	cfg->sdio_wrap    			= (BOOL) ( scratch & 0x80 );

	IO_Read( SPI_HCI_LSB, &scratch );
	cfg->half_clock_interval = (uint16_t) scratch;
	IO_Read( SPI_HCI_MSB, &scratch );
	cfg->half_clock_interval |= ( (uint16_t) scratch) << 8;

	IO_Read( SPI_ECD, &(cfg->end_cycle_delay) );

	cfg->clock_hz = 1.0 / ( 2.0 * CLOCK_PERIOD_SEC * ( 4.0 + ((double) cfg->half_clock_interval) ) );
	cfg->end_delay_ns = CLOCK_PERIOD_SEC * 4.0 + 0.5 * ((double) cfg->end_cycle_delay) / cfg->clock_hz;

	memcpy( &(idi_dataset.spi_cfg), &cfg, sizeof( struct spi_cfg ) );
	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int SPI_Report_Configuration_Text( struct spi_cfg * cfg, FILE * out )
{
	fprintf( out, "################################### SPI Configuration:\n" );
	fprintf( out, "sdio_wrap             = %s\n", cfg->sclk_polarity 	? "true" : "false" );
	fprintf( out, "sdo_polarity          = %s\n", cfg->sclk_polarity 	? "true" : "false" );
	fprintf( out, "sdi_polarity          = %s\n", cfg->sdi_polarity 	? "true" : "false" );
	fprintf( out, "sclk_phase            = %s\n", cfg->sclk_phase 		? "true" : "false" );
	fprintf( out, "sclk_polarity         = %s\n", cfg->sclk_polarity 	? "true" : "false" );
	fprintf( out, "chip_select_behavior  = " );

	switch( cfg->chip_select_behavior )
	{
		case IDI_CSB_SOFTWARE:		fprintf( out, "IDI_CSB_SOFTWARE" );			break;
		case IDI_CSB_BUFFER:		fprintf( out, "IDI_CSB_BUFFER" );			break;
		case IDI_CSB_UINT8:			fprintf( out, "IDI_CSB_UINT8" );			break;
		case IDI_CSB_UINT16:		fprintf( out, "IDI_CSB_UINT16" );			break;
	}
	fprintf( out, "\n" );

	fprintf( out, "end_cycle_delay       = 0x%02X  (%d)\n", cfg->end_cycle_delay,     cfg->end_cycle_delay     );
	fprintf( out, "half_clock_interval   = 0x%04X  (%d)\n", cfg->half_clock_interval, cfg->half_clock_interval );

	fprintf( out, "clock_hz              = %f Hz\n", cfg->clock_hz );
	fprintf( out, "end_delay_ns          = %f ns\n", cfg->end_delay_ns );
	fprintf( out, "\n" );
	return SUCCESS;
}


/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int SPI_Report_Status_Text( struct spi_status * status, FILE * out )
{
	fprintf( out, "################################### SPI" );
	if ( status->tx_status ) fprintf( out, "TX" );
	else                     fprintf( out, "RX" );

	fprintf( out, " Status:\n" );

	fprintf( out, "full       = %s\n", status->full  ? "true" : "false" );
	fprintf( out, "empty      = %s\n", status->empty ? "true" : "false" );
	fprintf( out, "fifo size  = %d\n", status->fifo_size                );
	fprintf( out, "fifo count = %s\n", status->fifo_count               );

	return SUCCESS;
}


/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int SPI_Configuration_Initialize( struct spi_cfg * cfg )
{
	cfg->sdio_wrap 				= false;
	cfg->sdo_polarity 			= false;
	cfg->sdi_polarity 			= false;
	/*  Mode 	CPOL 	CPHA
	 *	 0 	     0 	     0
	 *	 1 	     0 	     1
	 *	 2 	     1 	     0
	 *	 3 	     1 	     1
	 */
	cfg->sclk_phase 			= false;   /* the FRAM uses SPI Mode 0 or 3 */
	cfg->sclk_polarity 			= false;

	cfg->chip_select_behavior 	= false;
	cfg->end_cycle_delay 		= 0;		/* shortest delay possible 		*/
	cfg->half_clock_interval 	= 0;		/* shortest interval possible 	*/

	cfg->clock_hz 				= 0.0;
	cfg->end_delay_ns 			= 0.0;

	return SUCCESS;
}



/**********************************************************************************************************//**
@ingroup idi
\brief

Status format:
status[7]   = FIFO full
status[6]   = FIFO empty
status[5]   = zero
status[4:0] = bytes in FIFO. SPI_FIFO_SIZE is total length.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int SPI_Status_Write( struct spi_status * status )
{
	uint8_t reg_value;

	if ( SPI_IsNotPresent() ) return -EC_SPI_NOT_FOUND;

	IO_Read_U8( SPI_TX_STATUS, &reg_value );
	*status->fifo_count = reg_value & 0x1F;
	*status->full       = reg_value & 0x80;
	*status->fifo_size  = SPI_FIFO_SIZE;
	*status->empty      = reg_value & 0x40;
	*status->tx_status  = true;
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Status format:
status[7]   = FIFO full
status[6]   = FIFO empty
status[5]   = zero
status[4:0] = bytes in FIFO. SPI_FIFO_SIZE is total length.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
BOOL SPI_Status_Write_FIFO_Is_Full( void )
{
	uint8_t reg_value;
	IO_Read_U8( SPI_TX_STATUS, &reg_value );
	if ( reg_value & 0x80 ) return true;
	return false;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int SPI_Status_Read( struct spi_status * status )
{
	uint8_t reg_value;

	if ( SPI_IsNotPresent() ) return -EC_SPI_NOT_FOUND;

	IO_Read_U8( SPI_RX_STATUS, &reg_value );
	*status->fifo_count = reg_value & 0x1F;
	*status->full       = reg_value & 0x80;
	*status->fifo_size  = SPI_FIFO_SIZE;
	*status->empty      = reg_value & 0x40;
	*status->tx_status  = false;
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int SPI_Commit( uint8_t chip_select )
{
	if ( SPI_IsNotPresent() ) return -EC_SPI_NOT_FOUND;
	IO_Write_U8( SPI_COMMIT, chip_select );
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Assumption: assumes software is dealing with chip select external to this function.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int SPI_Data_Write( struct spi_status * 	status,
				  	int	 					buffer_size,
				  	int * 					count_actual,
				  	uint8_t * 				buffer
				  )
{
	int					error_code;
	int					write_level;
	int					count;
	struct spi_cfg		cfg;

	if ( SPI_IsNotPresent() ) return -EC_SPI_NOT_FOUND;

	error_code = SPI_Configuration_Get( &cfg );
	if ( error_code ) return error_code;

	error_code = SPI_Status_Write( status );
	if ( error_code ) return error_code;

	// test commit mode of SPI port
	write_level = 0;
	count       = buffer_size;
	switch ( cfg.chip_select_behavior )
	{
		case IDI_CSB_SOFTWARE:
			write_level = 0;  /* a blocking write all */
			break;
		case IDI_CSB_BUFFER:
			count = SPI_FIFO_SIZE - ((int) status->fifo_count);
			write_level = 1;  /* just fill the buffer */
			break;
		case IDI_CSB_UINT8:
			count = SPI_FIFO_SIZE - ((int) status->fifo_count);
			if ( 1 == ( count & 0x0001 ) )
			write_level = 0;  /* a blocking write all */
			break;
		case IDI_CSB_UINT16:
			//write only even numbers

			break;
	}

	error_code = SPI_Status_Write( status );
	if ( error_code ) return error_code;

	// write all buffer until done
	if ( 0 == write_level )
	{ /* a blocking write all */
		for ( index = 0; index < count; index++ )
		{
			while( SPI_Status_Write_FIFO_Is_Full() )
			{
				// wait - do nothing for now - not a good implementation.
			}
			IO_Write_U8( SPI_DATA, buffer[index] );
		}
		*count_actual = count;
	}
	else if ( 1 == write_level )
	{ /* just fill the buffer */
		for ( index = 0; index < count; index++ )
		{
			IO_Write_U8( SPI_DATA, buffer[index] );
		}
		*count_actual = count;
	}
	else if ( 2 == write_level )
	{ /* fill out the buffer and that is all */

	}

	error_code = SPI_Status_Write( status );
	if ( error_code ) return error_code;

	//write only

	// stuff them into buffer.

	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
//static SPI_Ddata_Read()
/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int SPI_Data_Read( struct spi_status * 	status,
		  	  	   int	 				buffer_size,
		  	  	   int * 				count_actual,
		  	  	   uint8_t * 			buffer
				 )
{
	int					error_code;
	int					length;
	uint8_t				byte_value;

	if ( SPI_IsNotPresent() ) return -EC_SPI_NOT_FOUND;

	error_code = SPI_Status_Read( status );
	if ( error_code ) return error_code;

	length = status->fifo_count;
	if ( length > buffer_size ) length = buffer_size;
	for ( index = 0; index < length; index++ )
	{
		IO_Read_U8( SPI_DATA, &byte_value );
		buffer[index] = byte_value;
	}
	error_code = SPI_Status_Read( status );
	return error_code;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int SPI_Data_Write_Read( 	int 		tx_count,
							uint8_t * 	tx_buf,
							int			rx_size,
							uint8_t * 	rx_buf,
						)
{

	// save commit mode

	// if tx_count = 1, then byte commit mode else word commit mode or buffer mode

	// if software control commit then ...

	if ( SPI_IsNotPresent() ) return -EC_SPI_NOT_FOUND;
	return SUCCESS;
}

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/


#define FRAM_DENSITY_BYTES   8192

/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM__Write_Enable_Latch_Set( void )
{
	uint8_t tx_buf[1] = { 0x06 }; /* opcode: WREN = 0x06 */
	return SPI_Transceiver( 1, tx_buf, 0, NULL );
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM__Write_Disable( void )
{
	uint8_t tx_buf[1] = { 0x04 }; /* opcode: WRDI = 0x04 */
	return SPI_Transceiver( 1, tx_buf, 0, NULL );
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM__Read_Status_Register( uint8_t * status )
{
	int 	error_code;
	uint8_t tx_buf[2] = { 0x05, 0x00 }; /* opcode: RDSR = 0x04 */
	uint8_t rx_buf[2];

	error_code = SPI_Transceiver( 2, tx_buf, 2, rx_buf );
	if ( error_code ) return error_code;
	*status = rx_buf[1];
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM__Write_Status_Register( uint8_t status )
{
	int 	error_code;
	uint8_t tx_buf[2] = { 0x05, 0x00 }; /* opcode: RDSR = 0x04 */
	uint8_t rx_buf[2];

	tx_buf[1] = status;
	error_code = SPI_Transceiver( 2, tx_buf, 2, rx_buf );
	if ( error_code ) return error_code;
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM__Memory_Read( uint16_t address, uint8_t count, uint8_t * buffer )
{
	int 	error_code;
	uint8_t tx_buf[2] = { 0x03, 0x00, 0x00 }; /* opcode: READ = 0x03 */
	uint8_t rx_buf[2];

	tx_buf[1] = (uint8_t)( address & 0xFF );
	tx_buf[2] = (uint8_t)( address >>   8 );

	// save spi commit mode
	// change to software control

	SPI_Commit( 1 );

	error_code = SPI_Transceiver( 3, tx_buf, 0, NULL );
	if ( error_code ) return error_code;

	error_code = SPI_Transceiver( count, NULL, count, buffer );
	if ( error_code ) return error_code;

	while ( SPI_Transmit_Is_Not_Empty() ); /* wait for buffer to empty */
	SPI_Commit( 0 );

	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM__Memory_Write( uint16_t address, uint8_t count, uint8_t * buffer )
{
	int error_code;
	uint8_t tx_buf[3] = { 0x02, 0x00, 0x00 }; /* opcode: WRITE = 0x02 */

	tx_buf[1] = (uint8_t)( address & 0xFF );
	tx_buf[2] = (uint8_t)( address >>   8 );

	// save spi commit mode
	// change to software control

	SPI_Commit( 1 );

	error_code = SPI_Transmit_Stuff( 3, tx_buf );
	if ( error_code ) return error_code;

	error_code = SPI_Transmit_Stuff( count, buffer );
	if ( error_code ) return error_code;

	while ( SPI_Transmit_Is_Not_Empty() ); /* wait for buffer to empty */
	SPI_Commit( 0 );

	// restore spi commit mode
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM__Read_ID( uint32_t * id )
{
	int 		error_code;
	uint8_t 	tx_buf[5] = { 0x9F, 0x00, 0x00, 0x00, 0x00 }; /* opcode: RDID = 0x9F */
	uint8_t 	rx_buf[5];

	error_code = SPI_Transceiver( 5, tx_buf, 5, rx_buf );
	if ( error_code ) return error_code;

	{
		int id_scratch = 0;
		int index;

		for ( index = 4; index > 0; index-- )
		{
			id_scratch = ( id_scratch << 8 ) | ( (uint32_t) rx_buf[index] );
		}
		*id = id_scratch;
	}

	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

This function will be used when creating a memory pool so that as blocks are
allocated one can determine if we have an issue outside of any allocated space
(i.e. overflows and so on).

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM_Set( uint8_t count, uint8_t * buffer )
{
	int				error_code;
	uint16_t		block_count;
	uint16_t		block_remainder;
	uint16_t		address;
	uint32_t		id;

	error_code = FRAM__Read_ID( &id );
	if ( error_code ) return error_code;

	address = 0;

	if ( count > 1 )
	{ /* */
		block_count     = FRAM_DENSITY_BYTES / ((uint16_t) count );
		block_remainder = FRAM_DENSITY_BYTES - ( block_count * ((uint16_t) FRAM_BLOCK_SIZE) );
	}
	else
	{ /* only one fill character, so we create a buffer of it to make things a bit faster */
		int				index;
		const uint16_t  block_size = FRAM_BLOCK_SIZE;
		/* prefill */
		if ( NULL == buffer )
		{
			for ( index = 0; index < block_size; index++ ) idi_dataset.fram_block[index]= 0;
		}
		else
		{
			for ( index = 0; index < block_size; index++ ) idi_dataset.fram_block[index]= buffer[0];
		}
		block_count     = FRAM_DENSITY_BYTES / block_size;
		block_remainder = FRAM_DENSITY_BYTES - ( block_count * block_size );
		buffer = idi_dataset.fram_block;
	}

	while ( block_count > 0 )
	{
		error_code = FRAM__Memory_Write( address, ((uint16_t) count), buffer );
		address += (uint16_t) count;
		block_count--;
	}

	if ( block_remainder > 0 )
	{
		error_code = FRAM__Memory_Write( address, block_remainder, buffer );
		if ( error_code ) return error_code;
	}
	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM_Report( uint16_t address, uint16_t length, FILE * out )
{
	int				error_code;
	const uint16_t  block_size = 16;
	uint16_t		block_count;
	uint16_t		block_remainder;

	block_count = length / block_size;
	block_remainder = length - ( block_count * block_size );

	while ( block_count > 0 )
	{ /* output a line at a time */
		error_code = FRAM__Memory_Read( address, block_size, idi_dataset.fram_block );
		error_code = Hex_Dump_Line( address, block_size, idi_dataset.fram_block, out );
		address += block_size;
		block_count--;
	}

	if ( block_remainder > 0 )
	{ /* output any remaining portion */
		error_code = FRAM__Memory_Read( address, block_remainder, idi_dataset.fram_block );
		error_code = Hex_Dump_Line( address, block_remainder, idi_dataset.fram_block, out );
		if ( error_code ) return error_code;
	}

	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

Currently limited to 16-bytes at a time.

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM_Memory_To_File( uint16_t address, uint16_t length, FILE * binary )
{
	int				error_code;
	uint16_t		block_count;
	uint16_t		block_remainder;

	block_count = length / ((uint16_t) FRAM_BLOCK_SIZE);
	block_remainder = length - ( block_count * ((uint16_t) FRAM_BLOCK_SIZE) );

	while ( block_count > 0 )
	{
		error_code = FRAM__Memory_Read( address, FRAM_BLOCK_SIZE, idi_dataset.fram_block );
		fwrite( idi_dataset.fram_block, FRAM_BLOCK_SIZE, binary );
		address += FRAM_BLOCK_SIZE;
		block_count--;
	}

	if ( block_remainder > 0 )
	{
		error_code = FRAM__Memory_Read( address, block_remainder, idi_dataset.fram_block );
		fwrite( idi_dataset.fram_block, block_remainder, binary );
		if ( error_code ) return error_code;
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

TODO: may need to rework so that it is the size of the file that sets the length

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
int FRAM_File_To_Memory( uint16_t address, uint16_t length, FILE * binary )
{
	int				error_code;
	uint16_t		block_count;
	uint16_t		block_remainder;

	block_count = length / ((uint16_t) SPI_BLOCK_SIZE);
	block_remainder = length - ( block_count * ((uint16_t) FRAM_BLOCK_SIZE) );

	while ( block_count > 0 )
	{
		fread( idi_dataset.fram_block, SPI_BLOCK_SIZE, binary );
		error_code = FRAM__Memory_Write( address, FRAM_BLOCK_SIZE, idi_dataset.fram_block );
		address += FRAM_BLOCK_SIZE;
		block_count--;
	}

	if ( block_remainder > 0 )
	{
		fread( idi_dataset.fram_block, block_remainder, binary );
		error_code = FRAM__Memory_Write( address, block_remainder, idi_dataset.fram_block );
		if ( error_code ) return error_code;
	}
	return SUCCESS;
}


/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/


/**********************************************************************************************************//**
@ingroup idi
\brief
*/
struct command_line
{
	int ( * cmd_fnc )( int argc, char * argv[] );
	char *  name;
	char *  help;
};
/**********************************************************************************************************/



/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_ID( int argc, char * argv[] )
{ /*  idi spi id */
	uint16_t id;
	SPI_ID_Get( &id );
	printf( "SPI ID: 0x%08lX\n", id );
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Config_Get( int argc, char * argv[] )
{ /*  idi spi cfg */
	int 	error_code;
	struct 	spi_cfg cfg;

	error_code = SPI_Configuration_Get( &cfg );
	if ( error_code ) return error_code;

	error_code = SPI_Report_Configuration_Text( &cfg, stdout );
	if ( error_code ) return error_code;

	printf( "\n" );
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Config_Clock_Hz( int argc, char * argv[] )
{ /*  idi spi clk [<freq_hz>] */
	int 	 error_code;
	double 	 clock_request_hz;
	double   clock_actual_hz;
	double   error;
	uint16_t hci;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( &cfg );
	if ( error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI CLK: %f hz\n", cfg->clock_hz );
	}
	else
	{ /* write */
		clock_request_hz = atof( argv[0] );
		error_code = SPI_Calculate_Clock( clock_request_hz, &clock_actual_hz, &error, &hci );
		if ( error_code ) return error_code;
		cfg.half_clock_interval = hci;
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( &cfg );
		if ( error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Config_End_Cycle_Delay_Sec( int argc, char * argv[] )
{ /*  idi spi ecd [<time sec>] */
	int 	 error_code;
	double 	 request_sec;
	double   actual_sec;
	double   error;
	uint8_t  ecd;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( &cfg );
	if ( error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI ECD: %f sec\n", ( cfg->end_delay_ns * 1.0e-9 ) );
	}
	else
	{ /* write */
		request_sec = atof( argv[0] );
		error_code = SPI_Calculate_End_Cycle_Delay( request_sec, &actual_sec, &error, &ecd );
		if ( error_code ) return error_code;
		cfg.end_delay_ns = actual_sec * 1.0e9;
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( &cfg );
		if ( error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
   		CPOL 	CPHA	MODE
	 	 0 	     0 	     0
	 	 1 	     0 	     1
	 	 2 	     1 	     0
	 	 3 	     1 	     1
*/
static int IDI_CMD__SPI_Config_Mode( int argc, char * argv[] )
{ /*  idi spi mode [0/1/2/3] */
	int 	 error_code;
	int		 mode;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( &cfg );
	if ( error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		if      ( (false == cfg.sclk_polarity ) && (false == cfg.sclk_phase) ) mode = 0;
		else if ( (false == cfg.sclk_polarity ) && (true  == cfg.sclk_phase) ) mode = 1;
		else if ( (true  == cfg.sclk_polarity ) && (false == cfg.sclk_phase) ) mode = 2;
		else if ( (true  == cfg.sclk_polarity ) && (true  == cfg.sclk_phase) ) mode = 3;
		printf( "SPI MODE: %d\n", mode );
	}
	else
	{ /* write */
		mode = (int) strtol( argv[0], NULL, 0 );
		switch ( mode )
		{
			case 0: cfg.sclk_polarity = false;	cfg.sclk_phase = false;		break;
			case 1: cfg.sclk_polarity = false;	cfg.sclk_phase = true;		break;
			case 2: cfg.sclk_polarity = true;	cfg.sclk_phase = false;		break;
			case 3: cfg.sclk_polarity = true;	cfg.sclk_phase = true;		break;
		}
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( &cfg );
		if ( error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Config_SDI_Polarity( int argc, char * argv[] )
{ /*  idi spi sdi [<true/1/false/0>] */
	int 	 error_code;
	BOOL	 value;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( &cfg );
	if ( error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI SDI POLARITY: %s\n", cfg.sdi_polarity ? "true" : "false" );
	}
	else
	{ /* write */
		cfg.sdi_polarity = String_To_Bool( argv[0] );
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( &cfg );
		if ( error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Config_SDO_Polarity( int argc, char * argv[] )
{ /*  idi spi sdo [<true/1/false/0>] */
	int 	 error_code;
	BOOL	 value;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( &cfg );
	if ( error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI SDO POLARITY: %s\n", cfg.sdo_polarity ? "true" : "false" );
	}
	else
	{ /* write */
		cfg.sdo_polarity = String_To_Bool( argv[0] );
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( &cfg );
		if ( error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Config_SDIO_Wrap( int argc, char * argv[] )
{ /*  idi spi wrap [<true/1/false/0>] */
	int 	 error_code;
	BOOL	 value;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( &cfg );
	if ( error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI wrap: %s\n", cfg.sdio_wrap ? "true" : "false" );
	}
	else
	{ /* write */
		cfg.sdio_wrap = String_To_Bool( argv[0] );
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( &cfg );
		if ( error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Config_Chip_Select_Behavior( int argc, char * argv[] )
{ /*  idi spi mode [0/1/2/3/software/buffer/uint8/uint16] */
	int 	 error_code;
	int		 csb;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( &cfg );
	if ( error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI CSB: ");
		switch ( cfg.chip_select_behavior )
		{
			case IDI_CSB_SOFTWARE:	printf( "software" );	break;
			case IDI_CSB_BUFFER:	printf( "buffer"   );	break;
			case IDI_CSB_UINT8:		printf( "uint8"    );	break;
			case IDI_CSB_UINT16:	printf( "uint16"   );	break;
		}
		printf( "\n" );
	}
	else
	{ /* write */
		if      ( 0 == strcmpi( "software", argv[0] ) ) cfg.chip_select_behavior = 0;
		else if ( 0 == strcmpi( "buffer",   argv[0] ) ) cfg.chip_select_behavior = 1;
		else if ( 0 == strcmpi( "uint8",    argv[0] ) ) cfg.chip_select_behavior = 2;
		else if ( 0 == strcmpi( "uint16",   argv[0] ) ) cfg.chip_select_behavior = 3;
		else
		{
			cfg.chip_select_behavior = (SPI_CSB_ENUM) strtol( argv[0], NULL, 0 );
			switch ( cfg.chip_select_behavior )
			{
				case IDI_CSB_SOFTWARE:
				case IDI_CSB_BUFFER:
				case IDI_CSB_UINT8:
				case IDI_CSB_UINT16:
					break;
				default:
					error_code = -EC_SPI_CSB_OUT_OF_RANGE;
					break;
			}
		}
		 if ( error_code ) return error_code;
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( &cfg );
		if ( error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Status( int argc, char * argv[] )
{ /*  idi spi status [rx] [tx] ...  */
	int 	 			error_code;
	int					index;
	struct spi_status 	status;

	if ( argc < 1 )
	{
		error_code = SPI_Status_Read( &status );
		if ( error_code ) return error_code;
		SPI_Report_Status_Text( &status, stdout );
	}
	else
	{
		for ( index = 0; index < argc; index++ )
		{
			if ( 0 == strcmpi( "rx", argv[index] ) )
			{
				error_code = SPI_Status_Read( &status );
				if ( error_code ) return error_code;
				SPI_Report_Status_Text( &status, stdout );
			}
			else if ( 0 == strcmpi( "tx", argv[index] ) )
			{
				error_code = SPI_Status_Write( &status );
				if ( error_code ) return error_code;
				SPI_Report_Status_Text( &status, stdout );
			}
		}
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Data( int argc, char * argv[] )
{ /*  idi spi data [rx] [all] [byte] [character] ... */
	int 	 			error_code;
	int					index;
	int					count;
	int					read_count; /* */
	struct spi_status	status;
	uint8_t				data_temp;
	uint8_t				rx_buffer[SPI_FIFO_SIZE];

	read_count = 0;
	if ( argc < 1 )
	{
		read_count = SPI_FIFO_SIZE;
	}
	else
	{
		if      ( 0 == strcmpi( "rx", argv[0] ) )
		{
			read_count = SPI_FIFO_SIZE;
		}
		else if ( 0 == strcmpi( "rx", argv[0] ) )
		{
			if ( argc > 1)
			{
				if ( 0 == strcmpi( "all", argv[1] ) ) read_count = SPI_FIFO_SIZE;
				else  read_count = (int) strtol( argv[1], NULL, 0 );
			}
		}
		else if ( 0 == strcmpi( "tx", argv[0] ) )
		{
			read_count = 0;
			index = argc - 1;
			SPI_Status_Write( &status );
			while ( ( index > 0 ) && ( true != status.full ) )
			{
				data_temp = (uint8_t) strtol( argv[index], NULL, 0 );
				error_code = SPI_Data_Write( &status, 1, &count, &data_temp );
				if ( error_code ) return error_code;
				index--;
			}
			if ( index > 0 ) printf( "Warning: ignored %d values\n", argc - 1 - index );
		}
	}

	if ( read_count > 0 )
	{
		error_code = SPI_Data_Write( &status, SPI_FIFO_SIZE, &count, &rx_buffer );
		if ( error_code ) return error_code;
		if ( count > 0 ) Hex_Dump_Line( 0, count, rx_buffer, stdout );
		else             printf( "Empty\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__SPI_Commit( int argc, char * argv[] )
{ /*  idi spi commit [<true/1/false/0>] */
	int		 error_code;
	uint8_t  chip_select;

	if      ( argc < 1                  ) chip_select = 0x01;
	else if ( String_To_Bool( argv[0] ) ) chip_select = 0x01;
	else                                  chip_select = 0x00;

	error_code = SPI_Commit( chip_select );
	if ( error_code ) return error_code;

	printf( "OK\n" );
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static struct command_line  idi_cmd_spi[] =
{
	{	IDI_CMD__SPI_ID, 							"id",		"wishbone id: params: none"								},
	{	IDI_CMD__SPI_Config_Get, 		  			"cfg",		"config dump: params: none"								},
	{   IDI_CMD__SPI_Config_Clock_Hz, 				"clk",		"clk:         params: [<clock freq in hertz>]"			},
	{   IDI_CMD__SPI_Config_End_Cycle_Delay_Sec,	"ecd",		"end delay:   params: [<time in seconds>]"				},
	{   IDI_CMD__SPI_Config_Mode,					"mode",		"mode:        params: [<0/1/2/3>]"						},
	{   IDI_CMD__SPI_Config_SDI_Polarity,			"sdi",		"sdi pol:     params: [<true/1/false/0>]"				},
	{   IDI_CMD__SPI_Config_SDO_Polarity,			"sdo",		"sdo pol:     params: [<true/1/false/0>]"				},
	{   IDI_CMD__SPI_Config_SDIO_Wrap,				"wrap",		"sdo-->sdi:   params: [<true/1/false/0>]"				},
	{   IDI_CMD__SPI_Config_Chip_Select_Behavior,	"csb",		"chip select behavior: params: [0/1/2/3/software/buffer/uint8/uint16]"	},
	{   IDI_CMD__SPI_Status,						"status",	"status of both TX and RX buffers"						},
	{   IDI_CMD__SPI_Data,							"data",		"read/write:  params: [one or more bytes or characters]"},
	{   IDI_CMD__SPI_Commit,						"commit"	"causes spi transactions to start"						},
	{	NULL,									NULL,		NULL													}
};
/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int IDI_Command_Line_SPI( int argc, char* argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	index = 0;
	while ( NULL != idi_cmd_spi[index].cmd_fnc )
	{
		if ( 0 == strcmpi( idi_cmd_spi[index].name, argv[0] )  )
		{
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			error_code = (* idi_cmd_spi[index].cmd_fnc )( argc_new, argv_new );
			break;
		}
		index++;
	}
	return error_code;
}
/**********************************************************************************************************/


/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__FRAM_Dump( int argc, char * argv[] )
{ /* idi fram dump <address> <length>  */
	uint16_t address;
	uint16_t length;

	if ( argc < 2 ) return -EC_PARAMETER;

	address = (uint16_t) strtol( argv[0], NULL, 0 );
	length  = (uint16_t) strtol( argv[1], NULL, 0 );

	return FRAM_Report( address, length, stdout );
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__FRAM_Save( int argc, char * argv[] )
{ /* idi fram save <address> <length> <destination_file> */
	int			error_code;
	uint16_t 	address;
	uint16_t 	length;
	FILE *   	out;

	if ( argc < 3 ) return -EC_PARAMETER;

	address = (uint16_t) strtol( argv[0], NULL, 0 );
	length  = (uint16_t) strtol( argv[1], NULL, 0 );
	out = fopen( argv[2], "w" );
	error_code = FRAM_Memory_To_File( address, length, out );
	fclose( out );
	return error_code;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__FRAM_Load( int argc, char * argv[] )
{ /* idi fram load <address> <source_file> */
	int			error_code;
	uint16_t 	address;
	uint16_t 	length;
	FILE *   	out;

	if ( argc < 2 ) return -EC_PARAMETER;

	address = (uint16_t) strtol( argv[0], NULL, 0 );
	out = fopen( argv[2], "r" );
	error_code = FRAM_File_To_Memory( address, out );
	fclose( out );
	return error_code;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__FRAM_Init( int argc, char * argv[] )
{ /*  idi fram init <pattern list: 0x55 0x33 '3' '5' 'q' >	*/
	int			error_code;
	int			index;
	uint8_t		buf[16];
	if ( argc < 1 )
	{ /* initialize all zeros */
		error_code = FRAM_Set( 0, NULL );
	}
	else
	{
		if ( argc > 16 ) argc = 16;
		for ( index = 0; index < argc; index++ )
		{
			buf[index] = (uint8_t) strtol( argv[index], NULL, 0 );
		}
		error_code = FRAM_Set( argc, buf );
	}
	return error_code;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__FRAM_WREN( int argc, char * argv[] )
{ /* idi fram WREN */
	return FRAM__Write_Enable_Latch_Set();
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__FRAM_WRDI( int argc, char * argv[] )
{ /*  idi fram WRDI	 */
	return FRAM__Write_Disable();
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__FRAM_RDSR( int argc, char * argv[] )
{ /* idi fram RDSR  */
	int 	error_code;
	uint8_t status;
	error_code = FRAM__Read_Status_Register( &status );
	printf( "FRAM STATUS: 0x%02X\n", ((int) status) );
	return error_code;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__FRAM_WRSR( int argc, char * argv[] )
{ /*  idi fram WRSR <value>	 */
	int 	error_code;
	uint8_t status;

	if ( argc < 1 ) return -EC_PARAMETER;
	status = (uint8_t) strtol( argv[0], NULL, 0 );
	return FRAM__Write_Status_Register( status );
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__FRAM_RDID( int argc, char * argv[] )
{ /*  idi fram RDID	 */
	uint32_t id;
	FRAM__Read_ID( &id );
	printf( "FRAM ID: 0x%08lX\n", id );
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static struct command_line  idi_cmd_fram[] =
{
	{	IDI_CMD__FRAM_Dump, 	"dump",		"params: <address> <length>"							},
	{	IDI_CMD__FRAM_Save,		"save" 		"params: <address> <length> <binary destination file>"	},
	{	IDI_CMD__FRAM_Load,		"load" 		"params: <address> <binary source file name"			},
	{	IDI_CMD__FRAM_Init,		"init" 		"params: [byte/character] [byte/character] ..."		 	},
	{	IDI_CMD__FRAM_WREN,		"wren" 		"WRite Enable Latch Set"								},
	{	IDI_CMD__FRAM_WRDI,		"wrdi" 		"WRite DIsable"											},
	{	IDI_CMD__FRAM_RDSR,		"rdsr" 		"ReaD Status Register"									},
	{	IDI_CMD__FRAM_WRSR,		"wrsr" 		"WRite Status Register.  Params: <status>"				},
	{	IDI_CMD__FRAM_RDID,		"rdid" 		"ReaD ID Register"										},
	{	NULL,					NULL,		NULL													},
};
/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int IDI_Command_Line_FRAM( int argc, char* argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	index = 0;
	while ( NULL != idi_cmd_fram[index].cmd_fnc )
	{
		if ( 0 == strcmpi( idi_cmd_fram[index].name, argv[0] )  )
		{
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			error_code = (* idi_cmd_fram[index].cmd_fnc )( argc_new, argv_new );
			break;
		}
		index++;
	}
	return error_code;
}
/**********************************************************************************************************/


/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__DIN_All( int argc, char * argv[] )
{ /* idi fram dump <address> <length>  */
	int 	error_code;
	int 	channel;
	BOOL	value;
	int		cp;
	int		group;
	char  	message[64];
	uint8_t din_grp[6];
	uint8_t mask;

	/* build in binary format */
	cp    = 0;
	group = 0;
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		error_code = IDI_DIN_Group_Get( group, &(din_grp[group]) );
		mask = 0x01;
		for ( channel = 0; channel < 8; channel++ )
		{
			message[cp++] = !!(din_grp[group] & mask) ? '1' : '0';
			mask = mask << 1;
		}
		message[cp++] = ' ';
	}
	message[cp] = '\0';
	printf( "DIN: %s\n", message );
	printf( "DIN:" );
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ ) printf( " %02X", din_grp[group] );
	printf( "\n" );

	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__DIN_Channel( int argc, char * argv[] )
{
	int 	error_code;
	int 	channel;
	char  	message[8];
	BOOL	value;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	channel = (int) strtol( argv[0], NULL, 0 );
	error_code = IDI_DIN_Channel_Get( channel, &value );
	message[0] = value ? '1' : '0';
	message[1] = '\0';

	printf( "DIN%02d: %s\n", message );
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static int IDI_CMD__DIN_Group( int argc, char * argv[] )
{
	int 	error_code;
	int		index;
	int 	group;
	int		group_count;
	BOOL	do_all;
	uint8_t din_grp[6];

	if ( argc < 1 ) do_all = true;
	else            do_all = false;

	if ( 0 == strcmpi( "all", argv[0] ) ) do_all = true;

	if ( do_all )
	{ /* all */
		for ( group = 0; group < IDI_DIN_GROUP_QTY; group ++ )
		{
			error_code = IDI_DIN_Group_Get( group, &(din_grp[group]) );
		}
		group_count = IDI_DIN_GROUP_QTY;
	}
	else
	{
		group = (int) strtol( argv[0], NULL, 0 );
		error_code = IDI_DIN_Group_Get( group, &(din_grp[0]) );
		group_count = 1;
	}

	printf( "DIN_GROUP:" );
	for ( group = 0; group < group_count; group++ )
	{
		printf( " 0x%02X", ((int) din_grp[group]) );
	}
	printf( "\n" );
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief
*/
static struct command_line  idi_cmd_din[] =
{
	{	IDI_CMD__DIN_Channel,	"chan",		"params: <channel>"										},
	{   IDI_CMD__DIN_Group,		"group",	"params: [<group_channel | all>]"						},
	{	IDI_CMD__DIN_All,	 	"all",		"reports all digital inputs in binary and hex"			},
	{	NULL,					NULL,		NULL													},
};
/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int IDI_Command_Line_Digital_Input( int argc, char* argv[] )
{ /*  idi din <channel/all>  */
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;
    char *      endptr;
    int			channel;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	channel = (int) strtol( argv[0], &endptr, 0 );
	if ( argv[0] != endptr )
	{  /* assume channel number */
		error_code = (* idi_cmd_din[0].cmd_fnc )( argc_new, argv_new );
	}
	else
	{ /* otherwise a normal command */
		while ( NULL != idi_cmd_din[index].cmd_fnc )
		{
			if ( 0 == strcmpi( idi_cmd_din[index].name, argv[0] )  )
			{
				argv_new = &(argv[1]);
				argc_new = argc - 1;
				error_code = (* idi_cmd_din[index].cmd_fnc )( argc_new, argv_new );
				break;
			}
			index++;
		}
	}
	return error_code;
}
/**********************************************************************************************************/


/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int IDI_Command_Line_Register_Transaction( int argc, char* argv[] )
{
	int error_code;
	int index;
	int found;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	found = -1;
	index =  0;
	while ( definitions[index].direction != REG_DIR_NONE )
	{
		if ( 0 == strcmpi( definitions[index].acronym, argv[0] )  )
		{
			found = index;
			break;
		}
		index++;
	}

	if ( found < 0 )
	{
		printf( "ER: \n" );
		return  -EC_NOT_FOUND;
	}

	if ( argc < 2 )
	{ /* read operation */
		uint8_t value;
		error_code = IO_Read_U8( definitions[index].symbol, &value );
		if ( SUCCESS == error_code )
		{
			printf( "RD: %s=0x%02X\n",  definitions[index].acronym, value );
		}
	}
	else
	{
		uint8_t value;
		value = (uint8_t) strtol( argv[1], NULL, 0 );
		error_code = IO_Write_U8( definitions[index].symbol, value );
		if ( SUCCESS == error_code )
		{
			printf( "WR: %s=0x%02X\n", definitions[index].acronym, value );
		}
	}

    return SUCCESS;
}

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/


/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int IDI_Register_Report_CSV( struct reg_definition * table, FILE * out )
{
	int index = 0;

	fprintf( out, "\"acronym\",\"symbol\",\"bank\",\"direction\",\"physical_offset\"\n" );
	do
	{
		fprintf( out, "\"%s\",", table[index].acronym     );
		fprintf( out, "\"%s\",", table[index].symbol_name );

		switch( table[index].bank )
		{
		case IDI_BANK_0:			fprintf( out, "IDI_BANK_0" );			break;
		case IDI_BANK_1:			fprintf( out, "IDI_BANK_1" );			break;
		case IDI_BANK_2:			fprintf( out, "IDI_BANK_2" );			break;
		case IDI_BANK_3:			fprintf( out, "IDI_BANK_3" );			break;
		case IDI_BANK_4:			fprintf( out, "IDI_BANK_4" );			break;
		case IDI_BANK_5:			fprintf( out, "IDI_BANK_5" );			break;
		case IDI_BANK_6:			fprintf( out, "IDI_BANK_6" );			break;
		case IDI_BANK_7:			fprintf( out, "IDI_BANK_7" );			break;
		case IDI_BANK_NONE:			fprintf( out, "IDI_BANK_NONE" );		break;
		case IDI_BANK_UNDEFINED:	fprintf( out, "IDI_BANK_UNDEFINED" );	break;
		}
		fprintf( out, "," );

		switch( table[index].direction )
		{
			case REG_DIR_NONE:			fprintf( out, "REG_DIR_NONE" );			break;
			case REG_DIR_READ:			fprintf( out, "REG_DIR_READ" );			break;
			case REG_DIR_WRITE:			fprintf( out, "REG_DIR_WRITE" );		break;
			case REG_DIR_READ_WRITE:	fprintf( out, "REG_DIR_READ_WRITE" );	break;
		}
		fprintf( out, "," );

		fprintf( out, "\"%d\"", table[index].physical_offset );
		fprintf( out, "\n\r" ); /* separate so we have flexibility to re-organize columns */
		index++;
	} while ( definitions[index].direction != REG_DIR_NONE );

	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
void IDI_Help( FILE * out )
{
	int index;

	fprintf( out, "\n" );
	fprintf( out, "help - outputs help information\n" );
	fprintf( out, "dump - outputs register information in a comma delimited format\n" );
	fprintf( out, "spi  - SPI related functions\n" );
	index = 0;
	while ( NULL != idi_cmd_spi[index].help )
	{
	fprintf( out, "       --> %s\n", idi_cmd_spi[index].help );
	index++;
	}
	fprintf( out, "fram - FRAM related functions\n" );
	index = 0;
	while ( NULL != idi_cmd_fram[index].help )
	{
	fprintf( out, "       --> %s\n", idi_cmd_fram[index].help );
	index++;
	}
	fprintf( out, "din  - Digital input related functions\n" );
	index = 0;
	while ( NULL != idi_cmd_din[index].help )
	{
	fprintf( out, "       --> %s\n", idi_cmd_din[index].help );
	index++;
	}

}

/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int IDI_Termination( void )
{
	return SUCCESS;
}
/**********************************************************************************************************//**
@ingroup idi
\brief

\param[in] cfg pass in the configuration to be written to hardware.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns
a non-zero error code.
*/
int IDI_Initialization( void )
{
	memset( &idi_dataset, 0, sizeof(struct idi_dataset) );

	idi_dataset.bank_previous = IDI_BANK_0;
//	IO_Write_U8( IDI_BANK, idi_dataset.idi_bank_previous );

	idi_dataset.base_address  = 0x300;

    return SUCCESS;
}


/**********************************************************************//**
 * @brief
 *
 * @param argc  not currently used
 * @param argv  not currently used
 * @return SUCCESS (0) if no errors encountered, otherwise errors are reported
 * as a negative value.
 */
int main( int argc, char* argv[] )
{
	//int index;
    //int count;
	int 	error_code;
    int     argc_new;
    char ** argv_new;

/* used only for Win7 debugging sessions with cygwin */
#if(1)
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
#endif

   	error_code = IDI_Initialization();
   	if ( error_code ) goto Main_Termination;
//   	printf( "Hello\n");
//   	return SUCCESS;

#if(0)
   	count = argc;
    index = 0;     /* OK, zero value is name of file. */
   	while ( count > 0 )
   	{
   		printf( "index = %d, str = <%s>\n", index, argv[index] );
        index++;
        count--;
   	}
#endif

   	if ( argc > 1 )
    {
   		if      ( 0 == strcmpi( "help", argv[1]) )
   		{
   			IDI_Help( stdout );
   		}
   		else if ( 0 == strcmpi( "dump", argv[1] ) )
    	{ /* idi dump .... */
        	error_code = IDI_Register_Report_CSV( definitions, stdout );
        	if ( error_code ) goto Main_Termination_Error_Codes;
        }
   		else if ( 0 == strcmpi( "spi", argv[1] ) )
   		{ /* idi spi .... */
   			// idi spi id
   			// idi spi
   			argv_new = &(argv[1]);
   			argc_new = argc - 1;
   			error_code = IDI_Command_Line_SPI( argc_new, argv_new );
   			if ( error_code ) goto Main_Termination_Error_Codes;
   		}
   		else if ( 0 == strcmpi( "fram", argv[1] ) )
   		{ /* idi fram .... */
   			argv_new = &(argv[1]);
   			argc_new = argc - 1;
   			error_code = IDI_Command_Line_FRAM( argc_new, argv_new );
   			if ( error_code ) goto Main_Termination_Error_Codes;
   		}
   		else if ( 0 == strcmpi( "din", argv[1] ) )
   		{ /* idi din .... */
   			argv_new = &(argv[1]);
   			argc_new = argc - 1;
   			error_code = IDI_Command_Line_Digital_Input( argc_new, argv_new );
   			if ( error_code ) goto Main_Termination_Error_Codes;
   		}
   		else
   		{
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			error_code = IDI_Command_Line_Register_Transaction( argc_new, argv_new );
			if ( error_code ) goto Main_Termination_Error_Codes;
   		}
    }


Main_Termination:
   	IDI_Termination();

   	return error_code;

Main_Termination_Error_Codes:
	IDI_Termination();
	printf( "ERROR: %d\n",  error_code );
	return error_code;

#if(0)
	IDI_Register_Report_CSV( definitions, stdout );


	if ( IDI_DIN_IsPresent() )
	{ /* perform DIN tests */

	}

	if ( SPI_IsPresent() )
	{ /* perform SPI tests */

	}
#endif
}



