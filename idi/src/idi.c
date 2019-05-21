/*
 * idi.c
 *
 *  Created on: Feb 25, 2015
 *      Author: Mike
 */

/*<INTERNAL_BEGIN>*/
/*
 * Distribution notes
 */
/*<INTERNAL_END>*/

/* TODO:
 * 1. Split file apart into final building blocks (i.e. SPI functions into a separate file)
 * 2. comments above function definitions and any other definitions not complete.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>  /* control-c termination */
#include <limits.h>  /* UINT_MAX */
#include <time.h>    /* asctime(), localtime(), time(), time_t */
#include <errno.h>   /* errno and strerror() */
#include <math.h>    /* sqrt() */


#if defined( __MSDOS__ )
# include <mem.h>
# include <dos.h>    /* outportb() and inportb() */
# include <conio.h>  /* kbhit() and getch()      */
#else
# ifndef strcmpi
#  define strcmpi strcasecmp
# endif
# include <stdint.h>
#endif

/*<INTERNAL_BEGIN>*/
#include "stopwatch.h"
/*<INTERNAL_END>*/

/**********************************************************************************************************//**
 * 
 * @brief
 * The Subversion (SVN) time/date marker which is updated during commit of the
 * source file to the repository.
 */
#define SVN_REV  "$Date: 2015-12-01 09:22:43 -0600 (Tue, 01 Dec 2015) $"


/**********************************************************************************************************//**
 * 
 * @brief
 * The C89 compiler is typically void of these definitions, so we include them here.
 * Defines specific data width information.  The idea is to make this target independent.
 */
#if defined( __MSDOS__ )
 typedef unsigned char uint8_t;
 typedef unsigned int  uint16_t;  /* DOS only */
 typedef unsigned long uint32_t;  /* DOS only */

 typedef char int8_t;
 typedef int  int16_t;  /* DOS only */
 typedef long int32_t;  /* DOS only */

# define UINT32_MAX  ULONG_MAX
# define INT32_MAX   LONG_MAX
# define INT32_MIN   LONG_MIN

#endif
/**********************************************************************************************************//**
 * 
 * @brief
 * Boolean logic definitions
 */
typedef int BOOL;
#define false 0
#define true  1
enum { FALSE = 0, TRUE = 1 };

/**********************************************************************************************************//**
 * 
 * @brief
 * Board clock period as defined by the on-board oscillator which is 50MHz
 */
#define CLOCK_PERIOD_SEC   20.0e-9

/**********************************************************************************************************//**
 * 
 * @brief
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
	ID_DIN  = 0x8012,  /**< component DIN48  ID   */
	ID_SPI  = 0x8013,  /**< component SPI    ID   */
	ID_DOUT = 0x8021,  /**< component DOUT48 ID   */
	ID_IAI  = 0x8030   /**< component AI16   ID   */
};

/**********************************************************************************************************//**
 *
 * @brief
 * Top level software board IDs.  These are used to multiplex to the appropriate functions within the software.
 */
enum
{
	ID_IDI48 = 0,			/**< Isolated Digital Input Board,  48-Channel */
	ID_IDO48 = 1,			/**< Isolated Digital Output Board, 48-channel */
	ID_IAI16 = 2,			/**< Isolated Analog Input Board,   16-channel */
	ID_ARM32 = 3,			/**< ARM based CPU board 					   */
	ID_BOARD_UNKNOWN = 99	/**< Unknown board ID						   */
};

/***
 * Work around to test hardware that may or may not be present...useful for
 * software debugging without any target hardware.
 */
/* #define ID_ALWAYS_REPORT_AS_GOOD  1  */


/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/



/**********************************************************************************************************//**
 * 
 * @brief
 * An organized error code listing.  This macro is used to build the complete error code
 * enumeration list.
 *
 */
#define ERROR_CODES(_) \
	/*    enum_symbol             		   code		human readable        	   */ \
		_( SUCCESS,							0,		""							) \
		_( EC_BUFFER_TOO_LARGE, 			1,      "buffer too large"			) \
		_( EC_DIRECTION,        			2,      "direction"					) \
		_( EC_PARAMETER,	    			3,      "parameter"					) \
		_( EC_NOT_FOUND,        			4,		"not found"					) \
		_( EC_PARAMETER_MISSING,			5,		"missing parameter"			) \
		_( EC_SYNTAX,						6,		"syntax error"				) \
		_( EC_HEX_DUMP_COUNT, 				7,		"hex dump count"			) \
		_( EC_INIT_FILE,                    8,		"write init file failed"    ) \
		_( EC_BANK,		                    9,		"bank not found"    		) \
		/* EC_BUSY emulates a similar Linux error */\
		_( EC_APP_TERMINATE,               10,      "terminated by user" 		) \
		_( EC_APP_TERMINATE_CTRLC,         11,      "terminated by ctrl-c" 		) \
		_( EC_NOT_IMPLEMENTED,			   12,		"function not implemented"  ) \
		_( EC_CHANNEL,                     13,      "channel range"             ) \
		_( EC_MISSING_DATA,		   		   14,		"missing data"				) \
		_( EC_BUSY,				   		   16,		"interrupt busy"			) \
		_( EC_INTERRUPT_UNAVAILABLE,	   21,		"interrupt not available"	) \
		/* was EC_INTR_ERROR was EC_EINVAL to emulate Linux behavior */\
		_( EC_INTR_ERROR,                  22,		"interrupt error"           ) \
		_( EC_SPI_ECD_OUT_OF_RANGE, 	   40,  	"SPI ECD range"				) \
		_( EC_SPI_HALF_CLOCK_OUT_OF_RANGE, 41, 		"SPI half clock range"		) \
		_( EC_SPI_CSB_OUT_OF_RANGE,  	   42, 		"SPI CSB range"				) \
		_( EC_SPI_NOT_FOUND, 		 	   43, 		"SPI not found"				) \
		_( EC_SPI_BUFFER_SIZE_ODD,         44,		"buffer size odd"			) \
		_( EC_SPI_BUFFER_SIZE,         	   45,		"buffer size out of range"	) \
		_( EC_SPI_OBJECT_SIZE,         	   46,		"spi tx/rx object size"		) \
		_( EC_SPI_BUFFER_MISMATCH,     	   47,		"spi buffer mismatch"		) \
		_( EC_SPI_BUFFERS_NULL,     	   48,		"spi tx/rx buffers null"	) \
		_( EC_FRAM_ADDRESS_RANGE,     	   49,		"fram address range"		) \
		_( EC_TEST_FAIL,                   50,		"test fail"           		) \
		_( EC_BOARD, 					   51,		"board invalid"				) \
		_( EC_BOARD_TYPE,				   52,		"board type invalid"		) \
		_( EC_REGISTER_ADDRESS_EXCEEDED,   53,		"register address exceeded" ) \
		_( EC_EXTRA_IGNORED,			   54,		"extra ignored" 			) \
		_( EC_FILE_ERROR,				   55,		"file open error" 			) \
		_( EC_FILE_ERROR_SIZE,			   56,		"file data size error" 		) \
		_( EC_MODE_LEGACY,				   57,		"S/W set to legacy mode"	) \
		_( EC_BUFFER_EXCEEDED,			   58,		"buffer exceeded"           ) \
		_( EC_RANGE,			   		   59,		"range exceeded"            )


/*******************************************************************/
#define EC_EXTRACT_ENUM(symbol,code,message)    symbol = code,
#define EC_EXTRACT_HUMAN_READABLE(symbol,code,message)  { code, message },
#define EC_HUMAN_READABLE_TERMINATE { 0, NULL }

//#define EC_EXTRACT_DEFINITION(symbol,code,message) { read_write, bank, offset, name },
//#define EC_DEFINITION_NONE { SUCCESS, 0, "" }

typedef enum
{
	ERROR_CODES( EC_EXTRACT_ENUM )
} EC_ENUM;  /* EC = Error Code */

struct ec_human_readable
{
	EC_ENUM			error_code;
	const char *	message;
};



/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/

/**********************************************************************************************************//**
 * 
 * @brief
 * The bank register mapping.  This mapping is upwardly compatible with the legacy hardware
 * banking register.  It also allows for future expansion utilizing the lower bits which are
 * currently unused.
 */
#define IDI_BANK_INFO_DEFINITION(_)       \
		_( IDI_BANK_0,			0x00	) \
		_( IDI_BANK_1,			0x40	) \
		_( IDI_BANK_2,			0x80	) \
		_( IDI_BANK_3,			0xC0	) \
		_( IDI_BANK_4,			0x20	) \
		_( IDI_BANK_5,			0x60	) \
		_( IDI_BANK_6,			0xA0	) \
		_( IDI_BANK_7,			0xE0	) \
		_( IDI_BANK_NONE,		0xFE	) \
		_( IDI_BANK_UNDEFINED,	0xFF	)


#define IDO_BANK_INFO_DEFINITION(_)       \
		_( IDO_BANK_0,			0x00	) \
		_( IDO_BANK_1,			0x40	) \
		_( IDO_BANK_2,			0x80	) \
		_( IDO_BANK_3,			0xC0	) \
		_( IDO_BANK_4,			0x20	) \
		_( IDO_BANK_5,			0x60	) \
		_( IDO_BANK_6,			0xA0	) \
		_( IDO_BANK_7,			0xE0	) \
		_( IDO_BANK_NONE,		0xFE	) \
		_( IDO_BANK_UNDEFINED,	0xFF	)


#define BANK_INFO_DEFINITION(_)       	  \
		_( BANK_0,				0x00	) \
		_( BANK_1,				0x40	) \
		_( BANK_2,				0x80	) \
		_( BANK_3,				0xC0	) \
		_( BANK_4,				0x20	) \
		_( BANK_5,				0x60	) \
		_( BANK_6,				0xA0	) \
		_( BANK_7,				0xE0	) \
		_( BANK_NONE,			0xFE	) \
		_( BANK_UNDEFINED,		0xFF	)


/* generic versions */
//#define BANK_NONE				0xFE
//#define BANK_UNDEFINED			0xFF

/*******************************************************************/
#define BANK_EXTRACT_ENUM(symbol,value)  symbol = value,

#define BANK_EXTRACT_DEFINITION(symbol,value) { symbol,             #symbol },


enum { IDI_BANK_ENUM_QTY = 10 };

enum { IDO_BANK_ENUM_QTY = 10 };

typedef enum
{
	IDI_BANK_INFO_DEFINITION( BANK_EXTRACT_ENUM )
} IDI_BANK_ENUM;

typedef enum
{
	IDO_BANK_INFO_DEFINITION( BANK_EXTRACT_ENUM )
} IDO_BANK_ENUM;

typedef enum
{
	BANK_INFO_DEFINITION( BANK_EXTRACT_ENUM )
} BANK_ENUM;




#define BANK_NULL_DEFINITION                  { BANK_UNDEFINED, 	NULL    }



struct idi_bank_info
{
	int				symbol;
	const char * 	name;
};

struct ido_bank_info
{
	IDI_BANK_ENUM	symbol;
	const char * 	name;
};

struct bank_info
{
	BANK_ENUM		symbol;
	const char * 	name;
};

/*******************************************************************/

typedef enum
{
	REG_DIR_NONE 		= 0x00,
	REG_DIR_READ 		= 0x01,
	REG_DIR_WRITE 		= 0x02,
	REG_DIR_READ_WRITE	= 0x03
} REG_DIR_ENUM;



/**********************************************************************************************************//**
 * 
 * @brief
 * Organized list of registers and associate attributes of each of the registers.  This macro does not
 * consume any memory of in itself, and is only 'consumed' or used to automatically build enumerations
 * and pre-built data structures.
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
	/** FPGA INFORMATION - overlaps zeros registers ***/\
	_( IDI_FPGA_DATA,			28,		8,		1,		1,		REG_DIR_READ,		"fd",		IDI_BANK_0		) \
	_( IDI_FPGA_INDEX,			29,		9,		1,		1,		REG_DIR_READ_WRITE,	"fi",		IDI_BANK_0		) \
	/** CONFIG REGISTERS ***/ \
	_( IDI_EDGE_GROUP0,			30,	    8,		1,		1,		REG_DIR_READ_WRITE,	"ep0",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP1,			31,	    9,		1,		1,		REG_DIR_READ_WRITE,	"ep1",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP2,			32,	   10,		1,		1,		REG_DIR_READ_WRITE,	"ep2",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP3,			33,	   11,		1,		1,		REG_DIR_READ_WRITE,	"ep3",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP4,			34,	   12,		1,		1,		REG_DIR_READ_WRITE,	"ep4",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP5,			35,	   13,		1,		1,		REG_DIR_READ_WRITE,	"ep5",		IDI_BANK_1		) \
	_( IDI_INTR_BIT_GROUP0,		36,		8,		1,		1,		REG_DIR_READ_WRITE,	"ibe0",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP1,		37,		9,		1,		1,		REG_DIR_READ_WRITE,	"ibe1",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP2,		38,	   10,		1,		1,		REG_DIR_READ_WRITE,	"ibe2",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP3,		39,	   11,		1,		1,		REG_DIR_READ_WRITE,	"ibe3",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP4,		40,	   12,		1,		1,		REG_DIR_READ_WRITE,	"ibe4",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP5,		41,	   13,		1,		1,		REG_DIR_READ_WRITE,	"ibe5",		IDI_BANK_2		) \
	/** SPI REGISTERS ***/\
	_( IDI_SPI_ID_LSB,			42,	    8,		1,		1,		REG_DIR_READ,		"sidlsb",	IDI_BANK_6		) \
	_( IDI_SPI_ID_MSB,			43,	    9,		1,		1,		REG_DIR_READ,		"sidmsb",	IDI_BANK_6		) \
	_( IDI_SPI_CONFIG,			44,	   10,		1,		1,		REG_DIR_READ_WRITE,	"scfg",		IDI_BANK_6		) \
	_( IDI_SPI_ECD,				45,	   11,		1,		1,		REG_DIR_READ_WRITE,	"secd",		IDI_BANK_6		) \
	_( IDI_SPI_HCI_LSB,			46,	   12,		1,		1,		REG_DIR_READ_WRITE,	"shclsb",	IDI_BANK_6		) \
	_( IDI_SPI_HCI_MSB,			47,	   13,		1,		1,		REG_DIR_READ_WRITE,	"shcmsb",	IDI_BANK_6		) \
	_( IDI_SPI_DATA,			48,	    8,		1,		1,		REG_DIR_READ_WRITE,	"sdata",	IDI_BANK_7		) \
	_( IDI_SPI_TX_STATUS,		49,	    9,		1,		1,		REG_DIR_READ,		"stxf",		IDI_BANK_7		) \
	_( IDI_SPI_RX_STATUS,		50,	   10,		1,		1,		REG_DIR_READ,		"srxf",		IDI_BANK_7		) \
	_( IDI_SPI_COMMIT,			51,	   11,		1,		1,		REG_DIR_READ_WRITE,	"scmt",		IDI_BANK_7		) \
	_( IDI_UNDEFINED,			52,	    0,		0,		0,		REG_DIR_NONE,		"",			IDI_BANK_UNDEFINED	)


#define IDO_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol          	 logical   physical  byte   bus byte                                               */ \
/*    enum_symbol          	 address   offset    width  aperture   read/write      acronym	      bank	       */ \
	/** DATA REGISTERS ***/ \
	_( IDO_DO_GROUP0,			 0,		0,		1,		1,		REG_DIR_READ_WRITE,	"dog0",		IDO_BANK_NONE	) \
	_( IDO_DO_GROUP1,			 1,		1,		1,		1,		REG_DIR_READ_WRITE,	"dog1",		IDO_BANK_NONE	) \
	_( IDO_DO_GROUP2,			 2,		2,		1,		1,		REG_DIR_READ_WRITE,	"dog2",		IDO_BANK_NONE	) \
	_( IDO_DO_GROUP3,			 3,		3,		1,		1,		REG_DIR_READ_WRITE,	"dog3",		IDO_BANK_NONE	) \
	_( IDO_DO_GROUP4,			 4,		4,		1,		1,		REG_DIR_READ_WRITE,	"dog4",		IDO_BANK_NONE	) \
	_( IDO_DO_GROUP5,			 5,		5,		1,		1,		REG_DIR_READ_WRITE,	"dog5",		IDO_BANK_NONE	) \
	/** CONTROL REGISTERS ***/ \
	_( IDO_BANK,				 6,		7,		1,		1,		REG_DIR_READ_WRITE,	"bank",		IDO_BANK_NONE	) \
	/** FPGA INFORMATION ***/\
	_( IDO_FPGA_DATA,			 7,		8,		1,		1,		REG_DIR_READ,		"fd",		IDI_BANK_0		) \
	_( IDO_FPGA_INDEX,			 8,		9,		1,		1,		REG_DIR_READ_WRITE,	"fi",		IDI_BANK_0		) \
	/** ***/\
	_( IDO_ID_LSB,				 9,    14,		1,		1,		REG_DIR_READ,		"idlsb",	IDO_BANK_NONE	) \
	_( IDO_ID_MSB,				10,    15,		1,		1,		REG_DIR_READ,		"idmsb",	IDO_BANK_NONE	) \
	/** SPI REGISTERS ***/\
	_( IDO_SPI_ID_LSB,			11,	    8,		1,		1,		REG_DIR_READ,		"sidlsb",	IDO_BANK_6		) \
	_( IDO_SPI_ID_MSB,			12,	    9,		1,		1,		REG_DIR_READ,		"sidmsb",	IDO_BANK_6		) \
	_( IDO_SPI_CONFIG,			13,	   10,		1,		1,		REG_DIR_READ_WRITE,	"scfg",		IDO_BANK_6		) \
	_( IDO_SPI_ECD,				14,	   11,		1,		1,		REG_DIR_READ_WRITE,	"secd",		IDO_BANK_6		) \
	_( IDO_SPI_HCI_LSB,			15,	   12,		1,		1,		REG_DIR_READ_WRITE,	"shclsb",	IDO_BANK_6		) \
	_( IDO_SPI_HCI_MSB,			16,	   13,		1,		1,		REG_DIR_READ_WRITE,	"shcmsb",	IDO_BANK_6		) \
	_( IDO_SPI_DATA,			17,	    8,		1,		1,		REG_DIR_READ_WRITE,	"sdata",	IDO_BANK_7		) \
	_( IDO_SPI_TX_STATUS,		18,	    9,		1,		1,		REG_DIR_READ,		"stxf",		IDO_BANK_7		) \
	_( IDO_SPI_RX_STATUS,		19,	   10,		1,		1,		REG_DIR_READ,		"srxf",		IDO_BANK_7		) \
	_( IDO_SPI_COMMIT,			20,	   11,		1,		1,		REG_DIR_READ_WRITE,	"scmt",		IDO_BANK_7		) \
	_( IDO_UNDEFINED,			21,	    0,		0,		0,		REG_DIR_NONE,		"",			IDO_BANK_UNDEFINED	)


/* TODO: incomplete IAI16 register set definition below ********************************************************/

#define IAI_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol          	 logical   physical  byte   bus byte                                               */ \
/*    enum_symbol          	 address   offset    width  aperture   read/write      acronym	      bank	       */ \
	/** DATA REGISTERS ***/ \
	_( IAI_AIN0,				 0,	 0x00,		2,		1,		REG_DIR_READ,		"ai1",		IAI_BANK_NONE	) \
	_( IAI_AIN1,				 1,	 0x02,		2,		1,		REG_DIR_READ,		"ai2",		IAI_BANK_NONE	) \
	_( IAI_AIN2,				 2,	 0x04,		2,		1,		REG_DIR_READ,		"ai3",		IAI_BANK_NONE	) \
	_( IAI_AIN3,				 3,	 0x06,		2,		1,		REG_DIR_READ,		"ai4",		IAI_BANK_NONE	) \
	_( IAI_AIN4,			  	 4,	 0x20,		2,		1,		REG_DIR_READ,		"ai5",		IAI_BANK_NONE	) \
	_( IAI_AIN5,				 5,	 0x22,		2,		1,		REG_DIR_READ,		"ai6",		IAI_BANK_NONE	) \
	_( IAI_AIN6,				 6,	 0x24,		2,		1,		REG_DIR_READ,		"ai7",		IAI_BANK_NONE	) \
	_( IAI_AIN7,				 7,	 0x26,		2,		1,		REG_DIR_READ,		"ai8",		IAI_BANK_NONE	) \
	_( IAI_AIN8,				 8,	 0x40,		2,		1,		REG_DIR_READ,		"ai9",		IAI_BANK_NONE	) \
	_( IAI_AIN9,				 9,	 0x42,		2,		1,		REG_DIR_READ,		"ai10",		IAI_BANK_NONE	) \
	_( IAI_AIN10,				10,	 0x44,		2,		1,		REG_DIR_READ,		"ai11",		IAI_BANK_NONE	) \
	_( IAI_AIN11,				11,	 0x46,		2,		1,		REG_DIR_READ,		"ai12",		IAI_BANK_NONE	) \
	_( IAI_AIN12,				12,	 0x60,		2,		1,		REG_DIR_READ,		"ai13",		IAI_BANK_NONE	) \
	_( IAI_AIN13,				13,	 0x62,		2,		1,		REG_DIR_READ,		"ai14",		IAI_BANK_NONE	) \
	_( IAI_AIN14,				14,	 0x64,		2,		1,		REG_DIR_READ,		"ai15",		IAI_BANK_NONE	) \
	_( IAI_AIN15,				15,	 0x66,		2,		1,		REG_DIR_READ,		"ai16",		IAI_BANK_NONE	) \
	/** FPGA INFORMATION ***/\
	_( IAI_FPGA_DATA,			 7,		8,		1,		1,		REG_DIR_READ,		"fd",		IDI_BANK_0		) \
	_( IAI_FPGA_INDEX,			 8,		9,		1,		1,		REG_DIR_READ_WRITE,	"fi",		IDI_BANK_0		) \
	/** OTHER REGISTERS ***/ \
	_( IAI_ID_LSB,				17,  0x6E,		1,		1,		REG_DIR_READ,		"idlsb",	IAI_BANK_NONE	) \
	_( IAI_ID_MSB,				18,  0x6F,		1,		1,		REG_DIR_READ,		"idmsb",	IAI_BANK_NONE	) \
	/** SPI REGISTERS ***/\
	_( IAI_SPI_ID_LSB,			19,	 0x70,		1,		1,		REG_DIR_READ,		"sidlsb",	IAI_BANK_NONE	) \
	_( IAI_SPI_ID_MSB,			20,	 0x71,		1,		1,		REG_DIR_READ,		"sidmsb",	IAI_BANK_NONE	) \
	_( IAI_SPI_CONFIG,			21,	 0x72,		1,		1,		REG_DIR_READ_WRITE,	"scfg",		IAI_BANK_NONE	) \
	_( IAI_SPI_ECD,				22,	 0x73,		1,		1,		REG_DIR_READ_WRITE,	"secd",		IAI_BANK_NONE	) \
	_( IAI_SPI_HCI_LSB,			23,	 0x74,		1,		1,		REG_DIR_READ_WRITE,	"shclsb",	IAI_BANK_NONE	) \
	_( IAI_SPI_HCI_MSB,			24,	 0x75,		1,		1,		REG_DIR_READ_WRITE,	"shcmsb",	IAI_BANK_NONE	) \
	_( IAI_SPI_DATA,			25,	 0x76,		1,		1,		REG_DIR_READ_WRITE,	"sdata",	IAI_BANK_NONE	) \
	_( IAI_SPI_TX_STATUS,		26,	 0x77,		1,		1,		REG_DIR_READ,		"stxf",		IAI_BANK_NONE	) \
	_( IAI_SPI_RX_STATUS,		27,	 0x78,		1,		1,		REG_DIR_READ,		"srxf",		IAI_BANK_NONE	) \
	_( IAI_SPI_COMMIT,			28,	 0x79,		1,		1,		REG_DIR_READ_WRITE,	"scmt",		IAI_BANK_NONE	) \
	_( IAI_UNDEFINED,			29,	    0,		0,		0,		REG_DIR_NONE,		"",			IAI_BANK_UNDEFINED	)

#if(0)
#define SPI_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol          	 logical   physical  byte   bus byte                                               */ \
/*    enum_symbol          	 address   offset    width  aperture   read/write      acronym	      bank	       */ \
	/** SPI REGISTERS ***/\
	_( SPI_ID_LSB,				 7,	    8,		1,		1,		REG_DIR_READ,		"sidlsb",	BANK_6			) \
	_( SPI_ID_MSB,				 8,	    9,		1,		1,		REG_DIR_READ,		"sidmsb",	BANK_6			) \
	_( SPI_CONFIG,				 9,	   10,		1,		1,		REG_DIR_READ_WRITE,	"scfg",		BANK_6			) \
	_( SPI_ECD,					10,	   11,		1,		1,		REG_DIR_READ_WRITE,	"secd",		BANK_6			) \
	_( SPI_HCI_LSB,				11,	   12,		1,		1,		REG_DIR_READ_WRITE,	"shclsb",	BANK_6			) \
	_( SPI_HCI_MSB,				12,	   13,		1,		1,		REG_DIR_READ_WRITE,	"shcmsb",	BANK_6			) \
	_( SPI_DATA,				13,	    8,		1,		1,		REG_DIR_READ_WRITE,	"sdata",	BANK_7			) \
	_( SPI_TX_STATUS,			14,	    9,		1,		1,		REG_DIR_READ,		"stxf",		BANK_7			) \
	_( SPI_RX_STATUS,			15,	   10,		1,		1,		REG_DIR_READ,		"srxf",		BANK_7			) \
	_( SPI_COMMIT,				16,	   11,		1,		1,		REG_DIR_READ_WRITE,	"scmt",		BANK_7			) \
	_( SPI_UNDEFINED,			17,	    0,		0,		0,		REG_DIR_NONE,		"",			BANK_UNDEFINED	)
#endif

/*******************************************************************/
#define REG_LOCATION_LOGICAL_GET(location)   ( ( (location) >> 8   ) & 0xFF )
#define REG_LOCATION_CHANNEL_GET(location)   (   (location)          & 0xFF )

/*********************************************
 * @brief
 * 'Index' is the row number, in this case it is the 'logical address' column
 * in the register definition x-macros.  The 'channel' is an index for channel
 * information which in this case we are setting to zero as it is not required
 * since that information is already embodied within the logical address for
 * the specific register definition.
 **/
#define REG_LOCATION_SET(index,channel)      ( ( ((index) & 0xFF) << 8 )  |  ((channel) & 0xFF) )

/*********************************************
 * @brief
 * Use to build the register enumerations of the IDI and IDO boards.
 **/
#define REG_EXTRACT_ENUM(symbol,index,offset,register_bytes,aperture_bytes,read_write,name,bank) symbol = REG_LOCATION_SET(index,0),

/*********************************************
 * @brief
 * Use to build the register definitions of the IDI and IDO boards.
 **/
#define REG_EXTRACT_DEFINITION(symbol,index,offset,register_bytes,aperture_bytes,read_write,name,bank) { REG_LOCATION_SET(index,0), read_write, bank, offset, #symbol, name },

/**********************************************************************************************************//**
 * 
 * @brief
 * Once the register location (i.e. logical_address or row) is known, then
 * all other parameters associated with that register can be looked up
 * including physical_offset, bank and legal read/write access.
 *
 */
/*******************************************************************/
typedef enum
{
	IDI_REGISTER_SET_DEFINITION( REG_EXTRACT_ENUM )
} IDI_REG_ENUM;

/*******************************************************************/
typedef enum
{
	IDO_REGISTER_SET_DEFINITION( REG_EXTRACT_ENUM )
} IDO_REG_ENUM;

/*******************************************************************/
struct idi_reg_definition
{
	IDI_REG_ENUM	symbol;
	REG_DIR_ENUM	direction;
	IDI_BANK_ENUM	bank;
	uint16_t		physical_offset;
	char * 			symbol_name;
	char *			acronym;
};

/*******************************************************************/
struct ido_reg_definition
{
	IDO_REG_ENUM	symbol;
	REG_DIR_ENUM	direction;
	IDO_BANK_ENUM	bank;
	uint16_t		physical_offset;
	char * 			symbol_name;
	char *			acronym;
};

/*******************************************************************/
/**< Generic definition used in IO_Write_U8 and IO_Read_U8 */
struct reg_definition
{
	int				symbol;
	REG_DIR_ENUM	direction;
	int				bank;
	uint16_t		physical_offset;
	char * 			symbol_name;
	char *			acronym;
};

/*******************************************************************/
typedef enum
{
	CSB_SOFTWARE	= 0,
	CSB_BUFFER		= 1,
	CSB_uint8_t		= 2,
	CSB_uint16_t	= 3
} SPI_CSB_ENUM;

/*******************************************************************/
//enum { SPI_FIFO_SIZE = 16 - 1 };
enum { SPI_FIFO_SIZE = 8 };

enum { SPI_FIFO_SIZE_BIT_WIDTH = 15 };


/*******************************************************************/
struct spi_cfg
{
	BOOL				sdio_wrap;				/**< SDIO_WRAP		*/
	BOOL				sdo_polarity;			/**< SDO_POL		*/
	BOOL				sdi_polarity;			/**< SDI_POL		*/
	BOOL				sclk_phase;				/**< SCLK_PHA		*/
	BOOL				sclk_polarity;			/**< SCLK_POL		*/
	BOOL				chip_select_route_ch1;  /**< CS_ROUTE		*/
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

/*******************************************************************/
#define IDO_DO_GROUP_SIZE  8
#define IDO_DO_SHIFT_RIGHT 3
#define IDO_DO_GROUP_QTY   6
#define IDO_DO_QTY		   ( IDO_DO_GROUP_SIZE * IDO_DO_GROUP_QTY )


struct din_cfg
{
	struct
	{
		BOOL	falling_edge;
		BOOL	interrupt_enable;
	} chan[IDI_DIN_QTY];
};

struct dout_cfg
{
	struct
	{
		BOOL	value;
	} chan[IDO_DO_QTY];
};



/**********************************************************************************************************//**
 * @brief
 */
/*******************************************************************/
#define FRAM_BLOCK_SIZE   	 256
#define SPI_BLOCK_SIZE       256
#define REGS_INIT_QTY         16
#define MESSAGE_SIZE         256

#define SPI_LOGGING  1

/*******************************************************************/
enum spi_log_enum
{
	SPI_LOG_NONE   		= 0,
	SPI_LOG_REQUEST		= 1,
	SPI_LOG_FILE		= 2,
	SPI_LOG_PAUSE   	= 4,
	SPI_LOG_ACTIVE  	= 5
};


/*******************************************************************/
struct spi_dataset
{
	uint16_t			id;
	struct spi_cfg 		cfg;

#if ( 1 == SPI_LOGGING )
	enum spi_log_enum	log_state;
	FILE *				log_file;
#endif

	/* this is a scratch pad for FRAM transactions */
	uint8_t				fram_block[FRAM_BLOCK_SIZE];

	/* used by:  IDI_CMD__SPI_Data()
	 *
	 * */
	uint8_t				tx_buffer[SPI_BLOCK_SIZE];
	uint8_t				rx_buffer[SPI_BLOCK_SIZE];
};

/*******************************************************************/
/* Trace triggers */
#define IO_TRACE_DEFINITION(_) \
/*    	enum_symbol     value  	 name	   		   */ \
	_( IO_TRACE_NULL,	0,		"NULL"				) \
	_( IO_TRACE_READ,	1,		"IO_Read_U8"		) \
	_( IO_TRACE_WRITE,	2,		"IO_Write_U8"		) \
	_( IO_TRACE_SPI,	3,		"SPI"				) \
	_( IO_TRACE_FRAM,	4,		"FRAM"				) \
	_( IO_TRACE_AS,		5,		"Analog_Stick"		) \
	_( IO_TRACE_OTHER,	6,		"Other"				) \

#define IO_TRACE_EXTRACT_ENUM(symbol,index,name) symbol = index,
#define IO_TRACE_EXTRACT_NAME(symbol,index,name) name,


//enum IO_TRACE_ENUM
//{
//	IO_TRACE_NULL	= 0,	/* nothing logged 					*/
//	IO_TRACE_READ	= 1,	/* IO_Read_U8()  					*/
//	IO_TRACE_WRITE  = 2,  	/* IO_Write_U8() 					*/
//	IO_TRACE_SPI    = 3,	/* spi transactions 				*/
//	IO_TRACE_FRAM   = 4,	/* FRAM transactions begin 			*/
//	IO_TRACE_AS		= 5,	/* Analog Stick transaction begin 	*/
//	IO_TRACE_OTHER  = 6
//};

/*******************************************************************/
/* @brief
 * Refer to IO_Trace_Log_Dataset() function for action details.  We put all this in the x-macro as a convenient summary.
 */
#define IO_TRACE_START_DEFINITION(_) \
/*    	enum_symbol          	 	value  	 name	   	IO_Trace_Log_Dataset() action  						         */ \
	_( IO_TRACE_START_DISABLE,		0,		"disable",	ds->trace.active = false;								      ) \
	_( IO_TRACE_START_ON,			1,		"on",		ds->trace.active = true;		    						  ) \
	_( IO_TRACE_START_ERROR,		2,		"error",	if ( SUCCESS       != error_code  ) ds->trace.active = true;  ) \
	_( IO_TRACE_START_SPI,			3,		"spi",		if ( IO_TRACE_SPI  == action      ) ds->trace.active = true;  ) \
	_( IO_TRACE_START_FRAM,			4,		"fram",		if ( IO_TRACE_FRAM == action      ) ds->trace.active = true;  ) \
	_( IO_TRACE_START_AS,			5,		"as",		if ( IO_TRACE_AS   == action      ) ds->trace.active = true;  )

#define IO_TRACE_STOP_DEFINITION(_) \
/*    	enum_symbol          	 	value  	name	   	IO_Trace_Log_Dataset() action  						         */ \
	_( IO_TRACE_STOP_NONE,			0,		"none",																	  ) \
	_( IO_TRACE_STOP_ERROR,			1,		"error",	if ( SUCCESS != error_code ) ds->trace.active = false;		  ) \
	_( IO_TRACE_STOP_CTRLC,			2,		"ctrlc",																  )

/* this will breakdown if we add more items to IO_TRACE_IGNORE_DEFINITION.  good enough for what we need right now. */
#define IO_TRACE_IGNORE_DEFINITION(_) \
/*    	enum_symbol          	 	value  	 name	   */ \
	_( IO_TRACE_IGNORE_NONE,		0,		"none"		) \
	_( IO_TRACE_IGNORE_SPI,         1,		"spi"		) \
	_( IO_TRACE_IGNORE_IO,          2,		"io"		)

/*******************************************************************/
#define IO_TRACE_START_STOP_EXTRACT_ENUM(symbol,index,name,action) symbol = index,
#define IO_TRACE_START_STOP_EXTRACT_NAME(symbol,index,name,action) name,
/* */
#define IO_TRACE_START_STOP_EXTRACT_NAME_HELP_LIST(symbol,index,name,action) name " "

#define IO_TRACE_START_STOP_EXTRACT_SWITCH_STATEMENT(symbol,index,name,action)  \
	case symbol:	\
		{ action }  \
		break;

#define IO_TRACE_IGNORE_EXTRACT_ENUM(symbol,value,name) symbol = value,
#define IO_TRACE_IGNORE_EXTRACT_NAME(symbol,value,name) name,
#define IO_TRACE_IGNORE_EXTRACT_NAME_HELP_LIST(symbol,value,name) name " "

/*******************************************************************/
enum IO_TRACE_ENUM
{
	IO_TRACE_DEFINITION( IO_TRACE_EXTRACT_ENUM )
};
/*******************************************************************/
enum IO_TRACE_START_ENUM
{
	IO_TRACE_START_DEFINITION( IO_TRACE_START_STOP_EXTRACT_ENUM )
};
/*******************************************************************/
enum IO_TRACE_STOP_ENUM
{
	IO_TRACE_STOP_DEFINITION( IO_TRACE_START_STOP_EXTRACT_ENUM )
};
/*******************************************************************/
enum IO_TRACE_IGNORE_ENUM
{
	IO_TRACE_IGNORE_DEFINITION( IO_TRACE_IGNORE_EXTRACT_ENUM )
};
/*******************************************************************/
#define IO_TRACE_SIZE  			1024
#define IO_TRACE_FILE_NAME_SIZE 32
#define IO_TRACE_DUMP_FILE_NAME "iotrace.csv"
/*******************************************************************/
#define IO_TRACE_USE_SPI_LOGGING  	1
#define IO_TRACE_USE_FRAM_LOGGING 	1
#define IO_TRACE_USE_AS_LOGGING 	1
/*******************************************************************/
struct io_trace
{
	int					error_code;
	enum IO_TRACE_ENUM 	action;
	int 				board_id;
	int 				location;
	size_t				line;		/* line in source code */
	size_t				posting;   /* larger numbers => later times */
	uint8_t 			value;
	const char * 		function_name;
};
/*******************************************************************/
struct io_trace_info
{
	size_t						begin;
	size_t						end;
	size_t						index;
	size_t						count;
	size_t						posting; /* larger number => later times */
	BOOL						wrap;
	enum IO_TRACE_START_ENUM	start;
	enum IO_TRACE_STOP_ENUM		stop;
	enum IO_TRACE_IGNORE_ENUM	ignore;
	BOOL						active;
	char  						filename[IO_TRACE_FILE_NAME_SIZE];
	struct io_trace *   		buf;
};

/*******************************************************************/
typedef enum
{
	MODE_JUMPERS_0		= 0,  /* legacy */
	MODE_JUMPERS_1 		= 1,
	MODE_JUMPERS_2		= 2,
	MODE_JUMPERS_3		= 3
} MODE_JUMPERS_ENUM;


/**********************************************************************************************************//**
 * @brief
 * Store parameters in this data structure and then can easily read/write them.
 */
#define AS_ADS1259_REGISTER_QTY  9

struct as_ads1259_registers
{
	uint8_t		reg[AS_ADS1259_REGISTER_QTY];
};

/*******************************************************************/
struct idi_dataset
{
	/* the following must always be at the beginning of this data structure and in the same order */
	int					board_id;
	BOOL				set__suppress_io_activity; /* used by SET */
//	BOOL				loop_command;
	BOOL				quit_application;
	uint16_t			base_address;
	IDI_BANK_ENUM		bank_previous;
//	int					bank_register_index;   /* needs to be initialized at startup!! */
	BOOL				io_simulate;
	BOOL				io_report;
	BOOL				spi_is_present;
//	BOOL				io_trace;
	BOOL				fram_cs_default;   /* when fram is used, this will temporarily be used */
	BOOL				fram_spi_cs_route_backup;
	BOOL				as_cs_default;   /* when analog stick is used, this will temporarily be used */
	BOOL				as_spi_cs_route_backup;
	SPI_CSB_ENUM		as_spi_chip_select_behavior;
	MODE_JUMPERS_ENUM				mode_jumpers;
	const char *					svn_revision_string;
	struct spi_dataset				spi;
	struct io_trace_info 			trace;
	struct as_ads1259_registers   	ads1259;

	/* this is a scratch pad area for report generation */
	char 	  			message[MESSAGE_SIZE];


	/* irq number */
	unsigned int 		irq_number;
	/* Passing messages from main() to the final function which wishes to pick up
	 * and install an interrupt handler can do so.
	 */
	BOOL				irq_please_install_handler_request;	/* MAIN */
	BOOL				irq_handler_active;					/* MAIN */
	size_t				irq_quantity;						/* MAIN */
	size_t volatile 	irq_count;          				/* ISR 	*/
	size_t				irq_count_previous;					/* MAIN */
	/* Isolated digital input specific interrupt parameters
	 * to be used only by the Interrupt Service Routine (ISR).
	 * TODO: modify hardware firmware so that this can be moved
	 * to mainline rather than at ISR level.
	 */
	uint8_t	volatile	isr_pending_list[IDI_DIN_GROUP_QTY]; /* ISR 	*/

	/* dataset portion specific to this particular board */

	struct din_cfg		din_cfg;

	struct
	{
		BOOL			used;
		int				address;
		IDI_REG_ENUM 	location;
		uint8_t 		value;
	} reg_init[REGS_INIT_QTY];
};

/*******************************************************************/
struct ido_dataset
{
	/* the following must always be at the beginning of this data structure and in the same order */
	int					board_id;
	BOOL				set__suppress_io_activity; /* used by SET */
//	BOOL				loop_command;
	BOOL				quit_application;
	uint16_t			base_address;
	IDI_BANK_ENUM		bank_previous;
//	int					bank_register_index;   /* needs to be initialized at startup!! */
	BOOL				io_simulate;
	BOOL				io_report;
	BOOL				spi_is_present;
//	BOOL				io_trace;
	BOOL				fram_cs_default;   /* when fram is used, this will temporarily be used */
	BOOL				fram_spi_cs_route_backup;
	BOOL				as_cs_default;   /* when analog stick is used, this will temporarily be used */
	BOOL				as_spi_cs_route_backup;
	SPI_CSB_ENUM		as_spi_chip_select_behavior;
	MODE_JUMPERS_ENUM				mode_jumpers;
	const char *					svn_revision_string;
	struct spi_dataset				spi;
	struct io_trace_info 			trace;
	struct as_ads1259_registers   	ads1259;

	/* this is a scratch pad area for report generation */
	char 	  			message[MESSAGE_SIZE];

	/* irq number */
	unsigned int 		irq_number;
	/* Passing messages from main() to the final function which wishes to pick up
	 * and install an interrupt handler can do so.
	 */
	BOOL				irq_please_install_handler_request;	/* MAIN */
	BOOL				irq_handler_active;					/* MAIN */
	size_t				irq_quantity;						/* MAIN */
	size_t volatile 	irq_count;          				/* ISR 	*/
	size_t				irq_count_previous;					/* MAIN */
	/* Isolated digital input specific interrupt parameters
	 * to be used only by the Interrupt Service Routine (ISR).
	 * TODO: modify hardware firmware so that this can be moved
	 * to mainline rather than at ISR level.
	 */
	uint8_t	volatile	isr_pending_list[IDI_DIN_GROUP_QTY]; /* ISR 	*/

	/* dataset portion specific to this particular board */

	struct dout_cfg		dout_cfg;

	struct
	{
		BOOL			used;
		int				address;
		IDO_REG_ENUM 	location;
		uint8_t 		value;
	} reg_init[REGS_INIT_QTY];

};
/*******************************************************************/
/**
 * @brief
 * Provides a generic interface to all board datasets.  This information is used
 * by several functions in order to determine how to read/write registers, etc.
 */
struct board_dataset
{
	/* the following must always be at the beginning of this data structure and in the same order */
	int					board_id;
	BOOL				set__suppress_io_activity; /* used by SET */
//	BOOL				loop_command;
	BOOL				quit_application;
	uint16_t			base_address;
	int					bank_previous;
	BOOL				io_simulate;
	BOOL				io_report;
	BOOL				spi_is_present;
//	BOOL				io_trace;
	BOOL				fram_cs_default;   /* when fram is used, this will temporarily be used */
	BOOL				fram_spi_cs_route_backup;
	BOOL				as_cs_default;   /* when analog stick is used, this will temporarily be used */
	BOOL				as_spi_cs_route_backup;
	SPI_CSB_ENUM		as_spi_chip_select_behavior;
	MODE_JUMPERS_ENUM				mode_jumpers;
	const char *					svn_revision_string;
	struct spi_dataset				spi;
	struct io_trace_info 			trace;
	struct as_ads1259_registers   	ads1259;
};

/**********************************************************************************************************//**
 * 
 * @brief
 * Global variables
 */

/*** Software revision as determined by subversion commits */
static const char svn_revision_string[] = { SVN_REV };

/*** Error code to human readable data structure */
const struct ec_human_readable ec_human_readable[] =
{
		ERROR_CODES( EC_EXTRACT_HUMAN_READABLE )
		EC_HUMAN_READABLE_TERMINATE
};


/*** list of "bank" names and associated enumerated symbol  */
static const struct idi_bank_info idi_bank_info[] =
{
		IDI_BANK_INFO_DEFINITION( BANK_EXTRACT_DEFINITION )
		BANK_NULL_DEFINITION
};

static const struct ido_bank_info ido_bank_info[] =
{
		IDO_BANK_INFO_DEFINITION( BANK_EXTRACT_DEFINITION )
		BANK_NULL_DEFINITION
};


/*** a read only list of register parameters */
static const struct idi_reg_definition idi_reg_definition[] =
{
		IDI_REGISTER_SET_DEFINITION( REG_EXTRACT_DEFINITION )
};

static const struct ido_reg_definition ido_reg_definition[] =
{
		IDO_REGISTER_SET_DEFINITION( REG_EXTRACT_DEFINITION )
};

/*** Global data structure which is restored during initialization and saved
 *   during application termination.
 */
static struct idi_dataset  idi_ds;

/*******************************************************************/
static struct ido_dataset  ido_ds;

/*******************************************************************/
static struct io_trace		global_io_trace_buf[IO_TRACE_SIZE];
static BOOL					global_io_trace_enable;
static const char * 		global_io_trace_start_name[]  = { 	IO_TRACE_START_DEFINITION( IO_TRACE_START_STOP_EXTRACT_NAME )
																NULL
														   };
static const char *			global_io_trace_stop_name[]   = { 	IO_TRACE_STOP_DEFINITION(  IO_TRACE_START_STOP_EXTRACT_NAME )
																NULL
														   };
static const char *			global_io_trace_ignore_name[] = { 	IO_TRACE_IGNORE_DEFINITION(  IO_TRACE_IGNORE_EXTRACT_NAME )
																NULL
														   };
static const char * 		global_io_trace_name[] 		  = {	IO_TRACE_DEFINITION(       IO_TRACE_EXTRACT_NAME )
																NULL
   	   	   	   	   	   	   	   	   	   	   	   	   	   	   };

/**********************************************************************************************************//**
 *
 * @brief
 * Board type information
 */

#define IDI_BOARD_INIT_FILE_NAME    "idi_init.bin"
#define IDO_BOARD_INIT_FILE_NAME    "ido_init.bin"

struct board_definition
{
	const void * bank_info;
	const void * definition;
	const void * dataset;
	const int    bank_register_symbol;
	const int	 fpga_data_symbol;
	const int	 fpga_index_symbol;
};

static struct board_definition  board_definition[] =
{
	{  	(const void *) idi_bank_info,	(const void *) idi_reg_definition, 	(const void *) &idi_ds,  	IDI_BANK,			IDI_FPGA_DATA,		IDI_FPGA_INDEX	},
	{	(const void *) ido_bank_info,	(const void *) ido_reg_definition,	(const void *) &ido_ds,		IDO_BANK,			IDO_FPGA_DATA,		IDO_FPGA_INDEX	},
	{	NULL,							NULL,								NULL,						IDO_BANK_NONE,		0,					0				}
};


static int  global_board_id      						= ID_IDI48;
static BOOL global_loop_command 						= false;
static int  global_loop_count                           = 0;
static int  global_loop_count_counter					= 0;
static int  global_loop_delay_ms						= 0;
static BOOL global_loop_space							= false;
static BOOL global_irq_please_install_handler_request 	= false;



/*
static size_t   global_internal_tx_byte_count = 0;
static size_t   global_internal_rx_byte_count = 0;
*/

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  R E G I S T E R  R E P O R T I N G  A N D  B A N K I N G  F U N C T I O N S  > > > >       */

/**********************************************************************//**
 * 
 * @brief
 * Outputs a human readable CSV to the desired output file or stdout.
 *
 * @param[in] bank  the bank enumerated value written to the bank register.
 * @return a string that describes the selected bank.
 */
static const char * IDI_Bank_Symbol_To_Name( struct idi_bank_info * info, IDI_BANK_ENUM bank )
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
	return info[index].name;
}

static const char * IDO_Bank_Symbol_To_Name( struct ido_bank_info * info, IDO_BANK_ENUM bank )
{
	int index;

	switch( bank )
	{
		case IDO_BANK_0:			index = 0;	break;
		case IDO_BANK_1:			index = 1;	break;
		case IDO_BANK_2:			index = 2;	break;
		case IDO_BANK_3:			index = 3;	break;
		case IDO_BANK_4:			index = 4;	break;
		case IDO_BANK_5:			index = 5;	break;
		case IDO_BANK_6:			index = 6;	break;
		case IDO_BANK_7:			index = 7;	break;
		case IDO_BANK_NONE:			index = 8;	break;
		case IDO_BANK_UNDEFINED:	index = 9;	break;
	}
	return info[index].name;
}


static const char * Bank_Symbol_To_Name( int board_id, int bank )
{
	switch ( board_id )
	{
		case ID_IDI48: return IDI_Bank_Symbol_To_Name( ((struct idi_bank_info *) board_definition[board_id].bank_info), (IDI_BANK_ENUM) bank );
		case ID_IDO48: return IDO_Bank_Symbol_To_Name( ((struct ido_bank_info *) board_definition[board_id].bank_info), (IDO_BANK_ENUM) bank );
	}
	return NULL;
}



/**********************************************************************//**
 *
 * @brief
 *
 * @param[in] bank  the bank enumerated value written to the bank register.
 * @return a string that describes the selected bank.
 */

static int IDI_Bank_Name_To_Symbol( int board_id, const char * name, IDI_BANK_ENUM * bank )
{
	int index;
	const struct bank_info * info = board_definition[board_id].bank_info;
	index = 0;
	while ( NULL != info[index].name  )
	{
		if ( 0 == strcmpi( info[index].name, name ) )
		{
			*bank = info[index].symbol;
			return SUCCESS;
		}
		index++;
	}
	return -EC_BANK;
}

static int IDO_Bank_Name_To_Symbol( int board_id, const char * name, IDO_BANK_ENUM * bank )
{
	int index;
	const struct bank_info * info = board_definition[board_id].bank_info;
	index = 0;
	while ( NULL != info[index].name  )
	{
		if ( 0 == strcmpi( info[index].name, name ) )
		{
			*bank = info[index].symbol;
			return SUCCESS;
		}
		index++;
	}
	return -EC_BANK;
}

static int Bank_Name_To_Symbol( int board_id, const char * name, int * bank )
{
	switch ( board_id )
	{
		case ID_IDI48: return IDI_Bank_Name_To_Symbol( board_id, name, (IDI_BANK_ENUM *) bank );
		case ID_IDO48: return IDO_Bank_Name_To_Symbol( board_id, name, (IDO_BANK_ENUM *) bank );
	}
	return -EC_BOARD;
}

/**********************************************************************//**
 * 
 * @brief
 *
 */
static int IDI_Register_Report_CSV( const struct idi_reg_definition * table, FILE * out )
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
	} while ( table[index].direction != REG_DIR_NONE );

	return SUCCESS;
}
/**********************************************************************//**
 *
 * @brief
 *
 */
static int IDO_Register_Report_CSV( const struct ido_reg_definition * table, FILE * out )
{
	int index = 0;

	fprintf( out, "\"acronym\",\"symbol\",\"bank\",\"direction\",\"physical_offset\"\n" );
	do
	{
		fprintf( out, "\"%s\",", table[index].acronym     );
		fprintf( out, "\"%s\",", table[index].symbol_name );

		switch( table[index].bank )
		{
		case IDO_BANK_0:			fprintf( out, "IDO_BANK_0" );			break;
		case IDO_BANK_1:			fprintf( out, "IDO_BANK_1" );			break;
		case IDO_BANK_2:			fprintf( out, "IDO_BANK_2" );			break;
		case IDO_BANK_3:			fprintf( out, "IDO_BANK_3" );			break;
		case IDO_BANK_4:			fprintf( out, "IDO_BANK_4" );			break;
		case IDO_BANK_5:			fprintf( out, "IDO_BANK_5" );			break;
		case IDO_BANK_6:			fprintf( out, "IDO_BANK_6" );			break;
		case IDO_BANK_7:			fprintf( out, "IDO_BANK_7" );			break;
		case IDO_BANK_NONE:			fprintf( out, "IDO_BANK_NONE" );		break;
		case IDO_BANK_UNDEFINED:	fprintf( out, "IDO_BANK_UNDEFINED" );	break;
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
	} while ( table[index].direction != REG_DIR_NONE );

	return SUCCESS;
}
/**********************************************************************//**
 *
 * @brief
 * Outputs a human readable CSV to the desired output file or stdout.
 *
 * @param[in] table  register definition table or data structure array
 * @param[in] out    destination of the human readable text to specified file or terminal.
 * @return SUCCESS (0) if no errors encountered, otherwise errors are reported
 * as a negative value.
 */
int Register_Report_CSV( int board_id, FILE * out )
{
	switch ( board_id )
	{
		case ID_IDI48:	return IDI_Register_Report_CSV( ((const struct idi_reg_definition *) board_definition[board_id].definition), out );
		case ID_IDO48:	return IDO_Register_Report_CSV( ((const struct ido_reg_definition *) board_definition[board_id].definition), out );
	}
	return -EC_BOARD;
}

/**********************************************************************************************************//**
 * @brief
 */
static int Register_Acronym_To_Row( int board_id, const char * acronym, int * row )
{
	int   index;
	const struct reg_definition * definition = (const struct reg_definition *) board_definition[board_id].definition;

	index =  0;
	while ( definition[index].direction != REG_DIR_NONE )
	{
		if ( 0 == strcmpi( definition[index].acronym, acronym )  )
		{
			*row = index;
			return SUCCESS;
		}
		index++;
	}
	return  -EC_NOT_FOUND;
}

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  M I S C E L L A N E O U S   F U N C T I O N S  > > > > 							          */


/**********************************************************************************************************//**
 * 
 * @brief
 * General function used to convert a string into a boolean equivalent value.
 *
 * @param[in] str  string input for conversion.
 * @return a BOOL is returned.  The default value returned is false.
 */
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
 * @brief
 * Dumps a hexadecimal and ASCII equivalent string to the desired output.
 * The format, illustrated below is a classic memory dump format.
 *
 * <address>: <hex> [<hex>].... <ascii list>
 *
 * @param[in] address  starting address.
 * @param[in] count    number of bytes in the buffer
 * @param[in] buffer   buffer containing the bytes to output
 * @param[out] out     desired output destination.
 * @return An error code is returned.
 */
enum { HEX_DUMP_BYTES_PER_LINE = 16 };

int Hex_Dump_Line( uint16_t address, size_t count, uint8_t * buffer, FILE * out )
{
	size_t 	index;
	char 	str_temp[8];
	char    str_ascii[20];
	char  	str_hex_list[64];

	//if ( count > 16 ) return -EC_BUFFER_TOO_LARGE;
	if ( count > HEX_DUMP_BYTES_PER_LINE ) return -EC_HEX_DUMP_COUNT;

	sprintf( str_hex_list, "%04X: ", ((int) address) );
	strcpy( str_ascii,    "" );
	for ( index = 0; index < count; index++ )
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
/**********************************************************************************************************//**
 * 
 * @brief
 * Translates an error code into a human readable message.
 *
 * Excerpt from AES advanced Linux library/driver.
 * COPYRIGHT NOTICE Copyright (c) 2012 by Apex Embedded Systems.
 *
 * @param[in] error_code   The error code to be translated into a human readable message
 * @return a human readable string representing a very brief description of the error code.
 */
static const char ec_unknown[] = "unknown error code";

const char * EC_Code_To_Human_Readable( EC_ENUM error_code )
{
	int index;

	if ( error_code < 0 ) error_code = -error_code;

	index = 0;
	while( NULL != ec_human_readable[index].message )
	{
		if ( ((EC_ENUM) error_code) == ec_human_readable[index].error_code )
		{
			return ec_human_readable[index].message;
		}
		index++;
	}
	return ec_unknown;
}

#ifdef __BORLANDC__
#elif _WIN32
#else
# include <termios.h>
#endif
/**********************************************************************************************************//**
 * 
 * @brief
 * Obtains a key from the keyboard in a non-blocking way.  This is exerpetted from
 * AES Universal Library/Driver.
 *
 * Excerpt from AES advanced Linux library/driver.
 * COPYRIGHT NOTICE Copyright (c) 2012 by Apex Embedded Systems.
 *
 * @param[out] character   Character from keyboard otherwise null character.
 * @return If true, then a valid character is available at the keyboard, otherwise false.
 */
BOOL Character_Get( int * character )
{
	int  char_temp;
	BOOL result = false;
#ifdef __BORLANDC__
	if ( kbhit() )
	{
		char_temp = getch();
		result    = true;
	}
	else
	{
		char_temp = '\0';
	}
#elif _WIN32
	if ( _kbhit() )
	{
		char_temp = getch();
		result    = true;
	}
	else
	{
		char_temp = '\0';
	}
#else
	struct termios orig_term_attr;
	struct termios new_term_attr;

	/* set the terminal to raw mode */
	tcgetattr( fileno(stdin), &orig_term_attr );
	memcpy( &new_term_attr, &orig_term_attr, sizeof(struct termios) );
	new_term_attr.c_lflag &= ~( ECHO|ICANON );
	new_term_attr.c_cc[VTIME] = 0;
	new_term_attr.c_cc[VMIN] = 0;
	tcsetattr( fileno( stdin ), TCSANOW, &new_term_attr );

	/* read a character from the stdin stream without blocking */
	/*   returns EOF (-1) if no character is available */
	char_temp = fgetc( stdin );

	/* restore the original terminal attributes */
	tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr );

	if ( EOF == char_temp )
	{
		char_temp = '\0';
		result    = false;
	}
	else
	{
		result    = true;
	}

#endif
	if ( NULL != character ) *character = char_temp;
	return result;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Obtains the current time and date.  This is exerpetted from AES Universal Library/Driver.
 *
 * Excerpt from AES advanced Linux library/driver.
 * COPYRIGHT NOTICE Copyright (c) 2012 by Apex Embedded Systems.
 *
 * @param[out] buf   		resulting date-time string.
 * @param[in]  buf_size  	the character size of the buffer including null terminating character.
 * @return SUCCESS always.
 */
int Time_Current_String( char * buf, size_t buf_size )
{
	struct tm * time_block;
	time_t		time_current;
	char *      time_ascii;
	size_t		index;

	time_current = time( NULL );
	/*TODO: replace localtime() with a reentrant version */
	time_block   = localtime( &time_current );
	/*
	TODO: replace asctime() with a reentrant version thereof  asctime_r() is a typical name.
	TODO: don't like the format!
	*/
	time_ascii = asctime( time_block );
	index = 0;
	/* copy the string out to the final buffer */
	while( ( time_ascii[index] != '\n' ) && ( index < buf_size ) )
	{
		buf[index] = time_ascii[index];
		index++;
	}
	if ( index >= buf_size ) index = buf_size - 1;
	buf[index] = '\0';
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int Print_Multiple( char * message, FILE * out )
{
	int error_code;

	error_code = fprintf( stdout, message );
	if ( out != stdout )
	{ /* output to a file too */
		error_code = fprintf( out, message );
	}
	return error_code;
}

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  I / O   P R O C E S S I N G   > > > > 											          */



/**********************************************************************************************************//**
 *
 * @brief
 */
int IO_Trace_Initialize( int board_id )
{
	size_t					index;
	struct io_trace *	 	trace;
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	dataset->trace.begin	= 0;
	dataset->trace.end		= IO_TRACE_SIZE - 1;
	dataset->trace.index	= 0;
	dataset->trace.count	= 0;
	dataset->trace.wrap		= false;
	dataset->trace.posting  = 0;

	if ( IO_TRACE_START_ON == dataset->trace.start ) dataset->trace.active = true;
	else                                             dataset->trace.active = false;

	dataset->trace.buf = &(global_io_trace_buf[0]);

	for ( index = 0; index < IO_TRACE_SIZE; index++ )
	{
		trace = &(dataset->trace.buf[index]);
		memset( trace, 0, sizeof( struct io_trace ) );
		dataset->trace.buf[index].action 		= IO_TRACE_NULL;
		dataset->trace.buf[index].function_name = NULL;
		dataset->trace.buf[index].board_id 		= ID_BOARD_UNKNOWN;
	}

	if ( strlen(dataset->trace.filename) == 0 )
	{
		strcpy( dataset->trace.filename, IO_TRACE_DUMP_FILE_NAME );
	}
	return SUCCESS;
}


/**********************************************************************************************************//**
 *
 * @brief
 */
static int IO_Trace_Log_Dataset_Begin( struct board_dataset * ds, size_t line, enum IO_TRACE_ENUM action, int location, int error_code, uint8_t value )
{
	(void) line;
	(void) location;
	(void) value;

	if ( false == global_io_trace_enable ) return 1; /* was SUCCESS */

	/* this will breakdown if we add more items to IO_TRACE_IGNORE_DEFINITION */
	if ( IO_TRACE_IGNORE_NONE != ds->trace.ignore )
	{
		if ( IO_TRACE_IGNORE_SPI  == ( ds->trace.ignore & IO_TRACE_IGNORE_SPI  ) )
		{
			if ( IO_TRACE_SPI == action ) return 4; /* ignore request */
		}
		if ( IO_TRACE_IGNORE_IO   == ( ds->trace.ignore & IO_TRACE_IGNORE_IO   ) )
		{
			if ( ( IO_TRACE_READ == action ) || ( IO_TRACE_WRITE == action ) ) return 3; /* ignore request */
		}
	}

	/* start trigger detect */
	switch( ds->trace.start )
	{
		IO_TRACE_START_DEFINITION( IO_TRACE_START_STOP_EXTRACT_SWITCH_STATEMENT )
	}

	if ( false == ds->trace.active ) return 2;  /* was SUCCESS */

	/* pre-index */
	if ( false == ds->trace.wrap ) ds->trace.count++;

	if ( ds->trace.index >= ds->trace.end )
	{
		ds->trace.index = ds->trace.begin;
		ds->trace.wrap  = true;
	}
	else
	{
		ds->trace.index++;
	}
	return 0;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static int IO_Trace_Log_Dataset_End( struct board_dataset * ds, size_t line, enum IO_TRACE_ENUM action, int location, int error_code, uint8_t value )
{
	(void) line;
	(void) action;
	(void) location;
	(void) value;

	/* stop trigger detect */
	switch( ds->trace.stop )
	{
		IO_TRACE_STOP_DEFINITION( IO_TRACE_START_STOP_EXTRACT_SWITCH_STATEMENT )
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
int IO_Trace_Log_Dataset( struct board_dataset * ds, size_t line, enum IO_TRACE_ENUM action, int location, int error_code, uint8_t value )
{
	struct io_trace * trace;

	if ( 0 != IO_Trace_Log_Dataset_Begin( ds, line, action, location, error_code, value ) ) return SUCCESS;  /* nothing to do */

	trace = &(ds->trace.buf[ds->trace.index]);
	trace->action  		= action;
	trace->board_id		= ds->board_id;
	trace->location		= location;
	trace->error_code 	= error_code;
	trace->value		= value;
	trace->posting		= ds->trace.posting;
	trace->line			= line;
	ds->trace.posting++;

	IO_Trace_Log_Dataset_End( ds, line, action, location, error_code, value  );

	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
int IO_Trace_Log( int board_id, size_t line, enum IO_TRACE_ENUM action, int location, int error_code, uint8_t value )
{
	int ec; /* short for error_code and to avoid naming conflict with incoming parameter */
	if ( true == global_io_trace_enable )
	{
		struct board_dataset *  dataset;
		dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

		/* this will breakdown if we add more items to IO_TRACE_IGNORE_DEFINITION */
		if ( IO_TRACE_IGNORE_NONE != dataset->trace.ignore )
		{
			if ( IO_TRACE_IGNORE_SPI  == ( dataset->trace.ignore & IO_TRACE_IGNORE_SPI  ) )
			{
				if ( IO_TRACE_SPI == action ) return 4; /* ignore request */
			}
			if ( IO_TRACE_IGNORE_IO   == ( dataset->trace.ignore & IO_TRACE_IGNORE_IO   ) )
			{
				if ( ( IO_TRACE_READ == action ) || ( IO_TRACE_WRITE == action ) ) return 3; /* ignore request */
			}
		}
		ec = IO_Trace_Log_Dataset( dataset, line, action, location, error_code, value );
	}
	else
	{
		ec = SUCCESS;
	}
	return ec;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
int IO_Trace_Log_Function( const char * function_name, size_t line, enum IO_TRACE_ENUM action, int board_id, int error_code )
{
	int ec; /* short for error_code and to avoid naming conflict with incoming parameter */
	if ( true == global_io_trace_enable )
	{
		struct io_trace * 		trace;
		struct board_dataset *  ds;
		ds = ( struct board_dataset * )  board_definition[board_id].dataset;

		if ( 0 != IO_Trace_Log_Dataset_Begin( ds, line, action, 0, error_code, 0  ) ) return SUCCESS; /* nothing to do */

		trace = &(ds->trace.buf[ds->trace.index]);
		trace->action  		 = action;
		trace->board_id		 = ds->board_id;
		trace->location		 = 0;
		trace->error_code 	 = error_code;
		trace->value		 = 0;
		trace->posting		 = ds->trace.posting;
		trace->line			 = line;
		trace->function_name = function_name;
		ds->trace.posting++;

		IO_Trace_Log_Dataset_End( ds, line, action, 0, error_code, 0  );
	}
	else
	{
		ec = SUCCESS;
	}
	return ec;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Dumps the trace buffer to a CSV file for further analysis.
 */
int IO_Trace_Dump( int board_id )
{
	int						error_code;
	FILE *					out;
	size_t					count, index, row;
	struct io_trace *  		trace;
	struct reg_definition * definition = ( struct reg_definition * ) board_definition[board_id].definition;
	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

	if ( false == global_io_trace_enable ) return SUCCESS;

	if ( strlen(dataset->trace.filename) == 0 ) out = fopen( IO_TRACE_DUMP_FILE_NAME, "wt" );
	else										out = fopen( dataset->trace.filename, "wt" );

	error_code = -EC_FILE_ERROR; /* assumed for the moment */
	if ( NULL == out ) return error_code;
	else               error_code = SUCCESS;

	index = dataset->trace.index;
	//if ( true == dataset->trace.wrap ) 	count = dataset->trace.index;
	//else									count = IO_TRACE_SIZE;
	// the above or alternatively below:
	count = dataset->trace.count;

	/* prefix information */
	//CMD__Main_Trace( board_id, 0, NULL );

	/* TODO: merge as this is a duplication from other functions */
	{
		char buf[64]={ 0 };
		Time_Current_String( buf, 64 );
		fprintf( out, "%s\n", buf );
	}
	fprintf( out, "trace.start        = " );
	fprintf( out, "%s\n", global_io_trace_start_name[dataset->trace.start] );
	fprintf( out, "trace.stop         = " );
	fprintf( out, "%s\n", global_io_trace_stop_name[dataset->trace.stop] );

	{
		size_t at_least_one = 0;
		fprintf( out, "trace.ignore       = " );
		/* this will breakdown if we add more items to IO_TRACE_IGNORE_DEFINITION */
		if ( IO_TRACE_IGNORE_SPI  == ( dataset->trace.ignore & IO_TRACE_IGNORE_SPI  ) )
		{
			if ( at_least_one > 0 ) printf( " " );
			fprintf( out, global_io_trace_ignore_name[IO_TRACE_IGNORE_SPI  ] );
			at_least_one++;
		}
		if ( IO_TRACE_IGNORE_IO   == ( dataset->trace.ignore & IO_TRACE_IGNORE_IO   ) )
		{
			if ( at_least_one > 0 ) printf( " " );
			fprintf( out, global_io_trace_ignore_name[IO_TRACE_IGNORE_IO   ] );
			at_least_one++;
		}
		if ( IO_TRACE_IGNORE_NONE == ( dataset->trace.ignore ) )
		{
			fprintf( out, global_io_trace_ignore_name[IO_TRACE_IGNORE_NONE ] );
		}
		fprintf( out, "\n" );
	}

	fprintf( out, "trace.file_name    = " );
	if ( 0 == strlen( dataset->trace.filename ) ) fprintf( out, "%s\n", IO_TRACE_DUMP_FILE_NAME );
	else                                          fprintf( out, "%s\n", dataset->trace.filename );


	/* row header */
	fprintf( out, "\"row\"" );
    fprintf( out, ", \"posting\"" );
    fprintf( out, ", \"board\"" );
    fprintf( out, ", \"action\"" );
    fprintf( out, ", \"src_line\"" );
    fprintf( out, ", \"register_or_function\"" );
    fprintf( out, ", \"channel\"" );
    fprintf( out, ", \"error\"" );
    fprintf( out, ",\"value\"" );
    fprintf( out, "\n" );
	/* all the rows */
	row = 0;
	while ( count > 0 )
	{
		trace = &(dataset->trace.buf[index]);

		/* row */
		fprintf( out, "%3u", (unsigned) row );

		/* posting */
		fprintf( out, ", %6u", (unsigned) trace->posting );

		/* board */
		/* TODO: create an x-macro for all board definitions */
		fprintf( out, ", " );
		switch( trace->board_id )
		{
			case ID_IDI48:	fprintf( out, "\"IDI48\"" );	break;
			case ID_IDO48:	fprintf( out, "\"IDO48\"" );	break;
			case ID_IAI16:	fprintf( out, "\"IAI16\"" );	break;
			case ID_ARM32:	fprintf( out, "\"ARM32\"" );	break;
		}
		/* action */
		fprintf( out, ", " );
		fprintf( out, "\"%s\"", global_io_trace_name[trace->action] );

		/* source line */
		fprintf( out, ", %6u", (unsigned) trace->line );

		/* register or symbol name */
		switch( trace->action )
		{
			case IO_TRACE_READ:
			case IO_TRACE_WRITE:
				fprintf( out, ", \"reg: %s\"", definition[REG_LOCATION_LOGICAL_GET(trace->location) + REG_LOCATION_CHANNEL_GET( trace->location )].symbol_name );
				break;
			default:
				if ( NULL != trace->function_name )
				{
				fprintf( out, ", \"fnc: %s\"", trace->function_name );
				}
				break;
		}

		/* channel */
		fprintf( out, ", %d", REG_LOCATION_CHANNEL_GET( trace->location ) );
		/* error code */
		fprintf( out, ", \"%s\"", EC_Code_To_Human_Readable( trace->error_code ) );
		/* value read/written */
		fprintf( out, ", 0x%02X", (int) trace->value );
		/* terminate row */
		fprintf( out, "\n" );

		/* post-index */
		if ( index == dataset->trace.begin )
		{
			index = dataset->trace.end;
		}
		else
		{
			index--;
		}
		count--;
		row++;
	}

	fclose( out );
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Produces an I/O direction error report.
 */
#define IDI_IO_DIRECTION_TEST   1

#if(0)
/**********************************************************************************************************//**
 *
 * @brief
 * Translates a register enumerated symbol into a string that is the same as the enumerated
 * symbol used throughout this code base.
 *
 * @param[in] location   The enumerated symbol representing the register.
 * @return a human readable string of the symbol.
 */
static char * IO_Get_Symbol_Name( IDI_REG_ENUM location )
{
	int index;
	index = REG_LOCATION_LOGICAL_GET( location );
	return idi_definitionsions[index].symbol_name;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Looks up in the register definitions list for the ports possible read/write directions.
 *
 * @param[in] location  the enumerated register symbol.  The enumerated symbol is
 *                      composed of offset and bank information used to determine
 *                      the final address information.
 * @param[in] direction The desired direction that is to run
 * @return A false is returned if the direction is valid, otherwise a true is returned
 */
#if defined( IDI_IO_DIRECTION_TEST )
static BOOL IO_Direction_IsNotValid( IDI_REG_ENUM location, REG_DIR_ENUM direction )
{
	int index;
	index = REG_LOCATION_LOGICAL_GET( location );
	if ( direction == (idi_definitions[index].direction & direction) ) return false;
	return true;
}
#endif
#endif


/**********************************************************************************************************//**
 * @brief
 */
static int IO_Write_U8_Port( struct board_dataset * dataset, size_t line, int address, uint8_t value )
{
	if ( NULL == dataset )
	{
#if defined( __MSDOS__ )
			outportb( address, value );
#else
			printf( "IO_Write_U8_Port: address = 0x%04X, value = 0x%02X\n", address, value );
#endif
	}
	else
	{
		if ( !dataset->io_simulate )
		{
#if defined( __MSDOS__ )
			outportb( address, value );
#endif
		}
		if ( ( dataset->io_report ) || ( dataset->io_simulate ) )
		{
		printf( "IO_Write_U8_Port: address = 0x%04X, value = 0x%02X\n", address, value );
		}
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int IO_Read_U8_Port( struct board_dataset * dataset, size_t line, int address,  uint8_t * value )
{
	if ( NULL == dataset )
	{
#if defined( __MSDOS__ )
			*value = inportb( address );
#else
		printf( "IO_Read_U8_Port: address = 0x%04X, value=unknown\n", address );
#endif
	}
	else
	{
		if ( !dataset->io_simulate )
		{
#if defined( __MSDOS__ )
			*value = inportb( address );
#endif
		}
		if ( ( dataset->io_report ) || ( dataset->io_simulate ) )
		{
		printf( "IO_Read_U8_Port: address = 0x%04X, ", address );
#if defined( __MSDOS__ )
		printf( "value = 0x%02X\n", *value );
#else
		printf( "value = unknown\n" );
#endif
		}
	}
	return SUCCESS;
}

/**********************************************************************************************************//**
 *
 * @brief
 * Writes uint8_t to I/O port.  Macros are used to guide the target implementation.
 *
 * @param[in] board_id  software level board selector or descriptor.
 * @param[in] location  the enumerated register symbol.  The enumerated symbol is
 *                      composed of offset and bank information used to determine
 *                      the final address information.
 * @param[in] value     The data to be written out.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IO_Write_U8( int board_id, size_t line, int location, uint8_t value )
{
	int error_code;
	int index;
	int offset;
	int channel;
	int	address;
	int	bank;

	struct reg_definition * definition = ( struct reg_definition * ) board_definition[board_id].definition;
	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

	error_code = SUCCESS;
	index   = REG_LOCATION_LOGICAL_GET( location );
#if defined( IDI_IO_DIRECTION_TEST )
	if ( !( REG_DIR_WRITE == ( definition[index].direction & REG_DIR_WRITE ) ) )
	{
		printf( "IO_Write_U8: %s, error in direction\n", definition[index].symbol_name );
		error_code = -EC_DIRECTION;
		goto IO_Write_U8_Exit;
	}
#endif
    bank   = definition[index].bank;
	if ( ( BANK_NONE != bank ) && ( bank != dataset->bank_previous ) )
	{   /* write to bank register only if different -- don't bother even checking it, will take too much time. */
		address = dataset->base_address + definition[REG_LOCATION_LOGICAL_GET( board_definition[board_id].bank_register_symbol )].physical_offset;
		dataset->bank_previous = bank;
		IO_Write_U8_Port( dataset, __LINE__, address, (uint8_t) bank );
		IO_Trace_Log( board_id, line, IO_TRACE_WRITE, board_definition[board_id].bank_register_symbol, error_code, (uint8_t) bank );
		if ( ( dataset->io_report ) || ( dataset->io_simulate ) )
		{
		printf( "IO_Write_U8 bank write: %s, address = 0x%04X, bank = %s (0x%02X)\n", 	definition[index].symbol_name,
																		    		address,
																		    		Bank_Symbol_To_Name( board_id, bank ),
																		    		bank
			  );
		}
	}
	channel = REG_LOCATION_CHANNEL_GET( location );
	offset  = definition[index].physical_offset;
	address = dataset->base_address + offset + channel;
	IO_Write_U8_Port( dataset, __LINE__, address, value );
IO_Write_U8_Exit:
	IO_Trace_Log( board_id, line, IO_TRACE_WRITE, location, error_code, value );
	return SUCCESS;
}

/**********************************************************************************************************//**
 *
 * @brief
 * Reads uint8_t from I/O port.  Macros are used to guide the target implementation.
 *
 * @param[in] location  the enumerated register symbol.  The enumerated symbol is
 *                      composed of offset and bank information used to determine
 *                      the final address information.
 * @param[in] value     The pointer to the destination of the read uint8_t value.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IO_Read_U8( int board_id, size_t line, int location,  uint8_t * value )
{
	int error_code;
	int index;
	int offset;
	int	channel;
	int address;
	int	bank;

	struct reg_definition * definition = ( struct reg_definition * ) board_definition[board_id].definition;
	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

	error_code = SUCCESS;
	index   = REG_LOCATION_LOGICAL_GET( location );
#if defined( IDI_IO_DIRECTION_TEST )
	if ( !( REG_DIR_READ == ( definition[index].direction & REG_DIR_READ ) ) )
	{
		printf( "IO_Read_U8: %s, error in direction\n", definition[index].symbol_name );
		error_code = -EC_DIRECTION;
		goto IO_Read_U8_Exit;
	}
#endif
	bank   = definition[index].bank;
	if ( ( BANK_NONE != bank ) && ( bank != dataset->bank_previous ) )
	{   /* write to bank register only if different -- don't bother even checking it, will take too much time. */
		address = dataset->base_address + definition[REG_LOCATION_LOGICAL_GET( board_definition[board_id].bank_register_symbol )].physical_offset;
		dataset->bank_previous = bank;
		IO_Write_U8_Port(  dataset, __LINE__, address, (uint8_t) bank );
		IO_Trace_Log( board_id, line, IO_TRACE_READ, board_definition[board_id].bank_register_symbol, error_code, (uint8_t) bank );
		if ( ( dataset->io_report ) || ( dataset->io_simulate ) )
		{
		printf( "IO_Read_U8 bank write: %s, address = 0x%04X, bank = %s (0x%02X)\n", 	definition[index].symbol_name,
																		    	address,
																		    	Bank_Symbol_To_Name( board_id, bank ),
																		    	bank
			  );
		}
	}
	channel = REG_LOCATION_CHANNEL_GET( location );
	offset  = definition[index].physical_offset;
	address = dataset->base_address + offset + channel;
	IO_Read_U8_Port( dataset, __LINE__, address, value );
IO_Read_U8_Exit:
	IO_Trace_Log( board_id, line, IO_TRACE_READ, location, error_code, *value );
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Writes uint16_t to I/O ports in a uint8_t succession incrementing the offset address.
 * Macros are used to guide the target implementation.  In this case, bus width (which
 * we typcially refer to the port width, which is different than register width) is
 * assumed to be byte (uint8_t) wide.
 *
 * @param[in] location  the enumerated register symbol.  The enumerated symbol is
 *                      composed of offset and bank information used to determine
 *                      the final address information.
 * @param[in] value     The uint16_t value to be written to the I/O register
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
void IO_Write_U16_Address_Increment( int board_id, size_t line, int location, uint16_t value )
{
//TODO: assumes little endian.


	IO_Write_U8( board_id, line, (int)(((int) location) + 0 /* channel */), (uint8_t)(   value        & 0xFF ) );
	IO_Write_U8( board_id, line, (int)(((int) location) + 1 /* channel */), (uint8_t)( ( value >> 8 ) & 0xFF ) );
}
/**********************************************************************************************************//**
 *
 * @brief
 * Reads uint16_t from I/O ports in a uint8_t succession incrementing the offset address.
 * Macros are used to guide the target implementation.  In this case, bus width (which
 * we typcially refer to the port width, which is different than register width) is
 * assumed to be byte (uint8_t) wide.
 *
 * @param[in] location  the enumerated register symbol.  The enumerated symbol is
 *                      composed of offset and bank information used to determine
 *                      the final address information.
 * @param[in] value     The pointer to the destination of the read uint16_t value.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
void IO_Read_U16_Address_Increment( int board_id, size_t line, int location,  uint16_t * value )
{
//TODO: assumes little endian.
	uint8_t lsb, msb;
	IO_Read_U8( board_id, line, (int)(((int) location) + 0 /* channel */), &lsb );
	IO_Read_U8( board_id, line, (int)(((int) location) + 1 /* channel */), &msb );
	*value = ( ((uint16_t) msb) << 8 ) | ( ((uint16_t) lsb) & 0xFF );
}
/**********************************************************************************************************//**
 *
 * @brief
 * Writes uint16_t to I/O ports in a uint8_t succession to the same address location.
 * Macros are used to guide the target implementation.  In this case, bus width (which
 * we typcially refer to the port width, which is different than register width) is
 * assumed to be byte (uint8_t) wide.
 *
 * @param[in] location  the enumerated register symbol.  The enumerated symbol is
 *                      composed of offset and bank information used to determine
 *                      the final address information.
 * @param[in] value     The uint16_t value to be written to the I/O register
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
void IO_Write_U16_Address_Fixed( int board_id, size_t line, int location, uint16_t value )
{
//TODO: assumes little endian.
	IO_Write_U8( board_id, line, (int)(((int) location) + 0), (uint8_t)(     value      & 0xFF ) );
	IO_Write_U8( board_id, line, (int)(((int) location) + 0), (uint8_t)( ( value >> 8 ) & 0xFF ) );
}
/**********************************************************************************************************//**
 *
 * @brief
 * Reads uint16_t from I/O ports in a uint8_t succession to the same address location.
 * Macros are used to guide the target implementation.  In this case, bus width (which
 * we typcially refer to the port width, which is different than register width) is
 * assumed to be byte (uint8_t) wide.
 *
 * @param[in] location  the enumerated register symbol.  The enumerated symbol is
 *                      composed of offset and bank information used to determine
 *                      the final address information.
 * @param[in] value     The pointer to the destination of the read uint16_t value.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
void IO_Read_U16_Address_Fixed( int board_id, size_t line, int location,  uint16_t * value )
{
//TODO: assumes little endian.
	uint8_t lsb, msb;
	IO_Read_U8( board_id, line, (int)(((int) location) + 0), &lsb );
	IO_Read_U8( board_id, line, (int)(((int) location) + 0), &msb );
	*value = ( ((uint16_t) msb) << 8 ) | ( ((uint16_t) lsb) & 0xFF );
}


/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*                   < < < <  P R I N T E R  P O R T  --  D E B U G   > > > >   						  */



#if defined( __MSDOS__ )

#define LPT_DEBUG_USE  1

#if ( 1 == LPT_DEBUG_USE )
enum
{
	LPT_OFFSET_DATA     = 0,
	LPT_OFFSET_STATUS   = 1,
	LPT_OFFSET_CONTROL	= 2,
	LPT_BASE_ADDRESS 	= 0x378,
};

static uint8_t lpt_control_register;

/**********************************************************************************************************//**
 * @brief
 */
void LPT_Initialize( void )
{
	lpt_control_register = 0;
	IO_Write_U8_Port( NULL, 0, LPT_BASE_ADDRESS + LPT_OFFSET_CONTROL, lpt_control_register );
}
/**********************************************************************************************************//**
 * @brief
 */
void LPT_SLTIN_Set( void )
{
	lpt_control_register |= 0x08;
	IO_Write_U8_Port( NULL, 0, LPT_BASE_ADDRESS + LPT_OFFSET_CONTROL, lpt_control_register );
}
/**********************************************************************************************************//**
 * @brief
 */
void LPT_SLTIN_Clear( void )
{
	lpt_control_register &= ~0x08;
	IO_Write_U8_Port( NULL, 0, LPT_BASE_ADDRESS + LPT_OFFSET_CONTROL, lpt_control_register );
}
#endif
#endif

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*                  < < < <  I N T E R R U P T  H A N D L I N G   > > > >                                 */
/*
 *
 * Excerpt from AES advanced library/driver for DOS with a Linux flavor.
 *
 * Admittedly, a whole lot of stuff for just a simple interrupt.  But it offers up
 * many options for this board as well as the others.
 *
 * COPYRIGHT NOTICE Copyright (c) 2012 by Apex Embedded Systems.
*/

/*-----------------------------------------------------------------------------*/
#if defined( __MSDOS__ )

#include <limits.h>   /* UINT_MAX */

#define TARGET_CPU_INTEL_386 1

/******************************************************************************/
/* These are the port addresses of the 8259 Programmable
   Interrupt Controller (PIC).
*/
#define IOKERN_DOS_PIC1_BASE_ADDRESS	0x20
#define IOKERN_DOS_PIC2_BASE_ADDRESS	0xA0
#define IOKERN_DOS_PIC1_CMD             IOKERN_DOS_PIC1_BASE_ADDRESS			/* PIC1 Command Port		*/
#define IOKERN_DOS_PIC1_IMR             ( IOKERN_DOS_PIC1_BASE_ADDRESS + 1 )	/* PIC1 interrupt mask port	*/
#define IOKERN_DOS_PIC2_CMD             IOKERN_DOS_PIC2_BASE_ADDRESS			/* PIC2 Command Port		*/
#define IOKERN_DOS_PIC2_IMR             ( IOKERN_DOS_PIC2_BASE_ADDRESS + 1 )	/* PIC2 interrupt mask port	*/

/* An end of interrupt needs to be sent to the Control Port of
	the 8259 when a hardware interrupt ends. */
#define IOKERN_DOS_NSEOI           0x20   /* End Of Interrupt */


#define IOKERN_IRQ_START  disable()
#define IOKERN_IRQ_ENABLE enable()
#define IOKERN_IRQ_END(irq) IOKern_PIC_EOI( irq ); enable()

#if defined( TARGET_CPU_INTEL_186 )
# pragma message "TARGET_CPU_INTEL_186 for interrupt chaining"

struct pt_regs
{
	uint16_t bp;
	uint16_t di;
	uint16_t si;
	uint16_t ds;
	uint16_t es;
	uint16_t dx;
	uint16_t cx;
	uint16_t bx;
	uint16_t ax;
	uint16_t ip;
	uint16_t cs;
	uint16_t flags;
};


# define IOKERN_IRQ_CHAIN_SELECT(old_fp,regs)		\
		_BX  =   regs.bx;    						\
		_CX  =   regs.ax;     						\
		regs.ax  =   FP_SEG((void far *)old_fp);	\
		regs.bx  =   FP_OFF((void far *)old_fp);	\
		_AX  =  _CX  ;								\
		__emit__(0x5D);								\
		__emit__(0x5F);								\
		__emit__(0x5E); 							\
		__emit__(0x1F); 							\
		__emit__(0x07);  							\
		__emit__(0x5A); 							\
		__emit__(0x59); 							\
		__emit__(0xCB);

#elif defined( TARGET_CPU_INTEL_386 )
# pragma message "TARGET_CPU_INTEL_386 for interrupt chaining"

struct pt_regs
{
	uint16_t bp;
	union { uint32_t edi; uint16_t di; };
	union { uint32_t esi; uint16_t si; };
	uint16_t ds;
	uint16_t es;
	union { uint32_t edx; uint16_t dx; struct { uint8_t l, h; } d; };
	union { uint32_t ecx; uint16_t cx; struct { uint8_t l, h; } c; };
		union { uint32_t ebx; uint16_t bx; struct { uint8_t l, h; } b; };
	/* aw is used to select top or bottom word -- used in ISR chaining */
	union { uint32_t eax; uint16_t ax; struct { uint16_t l, h; } aw; struct { uint8_t l, h; } a; };
	uint16_t ip;
	uint16_t cs;
	uint16_t flags;
};

# define IOKERN_IRQ_CHAIN_SELECT(old_fp,regs)	\
	_ECX  =   regs.eax;							\
	regs.aw.h  = FP_SEG((void far *)old_fp);	\
	regs.aw.l  = FP_OFF((void far *)old_fp);	\
	_EAX  =  _ECX ;								\
	__emit__(0x5D);								\
	__emit__(0x66);  __emit__(0x5F);			\
	__emit__(0x66);  __emit__(0x5E);			\
	__emit__(0x1F);								\
	__emit__(0x07);								\
	__emit__(0x66);  __emit__(0x5A);			\
	__emit__(0x66);  __emit__(0x59);			\
	__emit__(0x66);  __emit__(0x5B);			\
	__emit__(0xCB);
#endif

#define INTERRUPT interrupt
typedef void INTERRUPT ( * IOKERN_ISR_FP )( struct pt_regs r );

/******************************************************************************/
/* NOTE: Borland 5 requires semicolons - GCC will need something different */
#define IOKERN_LOCAL_IRQ_SAVE(flags)	\
	do {								\
			asm push bx				;	\
			asm pushf				;	\
			asm pop bx				;	\
			asm mov [flags], bx		;	\
			asm cli					;	\
			asm pop bx				;	\
	} while(0)

#define IOKERN_LOCAL_IRQ_RESTORE(flags)	\
	do {								\
			asm push bx				;	\
			asm mov bx, [flags]		;	\
			asm push bx				;	\
			asm popf				;	\
			asm pop bx				;	\
	} while(0)

/********************************************************/
typedef enum
{
	IOKERN_IRQ_NONE			= UINT_MAX,
	/* hardware interrupts */
	IOKERN_IRQ_0			=  0, /*         timer												*/
	IOKERN_IRQ_1			=  1, /*         keyboard or PS/2									*/
	IOKERN_IRQ_2			=  2, /*         reserved for 2nd 8259 (IRQ8-15) or IRQ2 for XT		*/
	IOKERN_IRQ_3			=  3, /* pc104 - typically COM2,4									*/
	IOKERN_IRQ_4			=  4, /* pc104 - typically COM1,3									*/
	IOKERN_IRQ_5			=  5, /* pc104 - typically LPT2 or sound							*/
	IOKERN_IRQ_6			=  6, /* pc104 - typically floppy									*/
	IOKERN_IRQ_7			=  7, /* pc104 - typically LPT1										*/
	IOKERN_IRQ_8			=  8, /*		 real-time clock									*/
	IOKERN_IRQ_9			=  9,
	IOKERN_IRQ_10			= 10, /* pc104 -  													*/
	IOKERN_IRQ_11			= 11, /* pc104 -  													*/
	IOKERN_IRQ_12			= 12, /* pc104 - typically, PS2 mouse								*/
	IOKERN_IRQ_13			= 13, /*         Numerical processor unit							*/
	IOKERN_IRQ_14			= 14, /* pc104 - primary IDE										*/
	IOKERN_IRQ_15			= 15, /* pc104 - secondary IDE										*/
	/* software interrupts */
	IOKERN_INT_33			= 0x33
} IOKERN_IRQ_ENUM;
/********************************************************/
typedef enum
{
	IOKERN_TASK_ID_NONE			= -1,
	IOKERN_TASK_ID_IRQ0			=  0,
	IOKERN_TASK_ID_IRQ1			=  1,
	IOKERN_TASK_ID_IRQ2			=  2,
	IOKERN_TASK_ID_IRQ3			=  3,
	IOKERN_TASK_ID_IRQ4			=  4,
	IOKERN_TASK_ID_IRQ5			=  5,
	IOKERN_TASK_ID_IRQ6			=  6,
	IOKERN_TASK_ID_IRQ7			=  7,
	IOKERN_TASK_ID_IRQ8			=  8,
	IOKERN_TASK_ID_IRQ9			=  9,
	IOKERN_TASK_ID_IRQ10		= 10,
	IOKERN_TASK_ID_IRQ11		= 11,
	IOKERN_TASK_ID_IRQ12		= 12,
	IOKERN_TASK_ID_IRQ13		= 13,
	IOKERN_TASK_ID_IRQ14		= 14,
	IOKERN_TASK_ID_IRQ15		= 15,
	IOKERN_TASK_ID_INT33		= 16  /* mouse function calls */
} IOKERN_TASK_ID_ENUM;

/********************************************************/
typedef enum
{
	IOKERN_CHAIN_TO_OLD_OFF		= 0,    /* if help_fp==NULL then this is always the case */
	IOKERN_CHAIN_TO_OLD_TIMER	= 1,	/* chain to the old timer, but only when
										   iokern_timer_periodic_chain_count reaches
										   zero.
										*/
	IOKERN_CHAIN_TO_OLD_NORMAL  = 2		/* chain to old interrupt */
} IOKERN_CHAIN_TO_OLD_ENUM;


/*******************************************************
Description
The following code is directly from irqreturn.h from the Linux kernel tree.
This is being used by the simulator and DOS to emulate Linux driver behavior
and specifically resource allocations.
*/
/**
 * enum irqreturn
 * @IRQ_NONE		interrupt was not from this device
 * @IRQ_HANDLED		interrupt was handled by this device
 * @IRQ_WAKE_THREAD	handler requests to wake the handler thread
 */
enum irqreturn {
	IRQ_NONE,
	IRQ_HANDLED,
	IRQ_WAKE_THREAD,
};
/* irqreturn_t is directly from Linux kernel */
typedef enum irqreturn irqreturn_t;

/********************************************************/
typedef irqreturn_t ( * IOKERN_TASK_FP )( int irq, void * dev_id, struct pt_regs * regs );
typedef void        ( * IOKERN_HELP_FP )( IOKERN_TASK_ID_ENUM id, IOKERN_IRQ_ENUM irq, struct pt_regs * regs );

/********************************************************/
typedef struct IOKERN_TASK_TYPE
{
	//SYS_TYPE		type;			/* SYS_TYPE__SYS_TASK_TYPE			*/
	IOKERN_TASK_FP	task_fp;		/* task to be called for the IRQ	*/
	IOKERN_HELP_FP	help_fp;		/* alternate interrupt routing -- timer functions	*/
	void *			dev_id;			/* task private data				*/
	const char *	name;			/* name of the task					*/
	size_t			speed;			/* irq/int fast or slow	(slow == push to main loop tasklet)	*/
	IOKERN_CHAIN_TO_OLD_ENUM chain_to_old;	/* See enum above */
	unsigned int	number;			/* irq/int that this task is mapped too */
	size_t			sw_int;			/* if software interrupt, then this value is set */
	irqreturn_t		result;			/* result of the task				*/
	unsigned long	count;			/* number of times the IRQ has run	*/
	IOKERN_ISR_FP	old_isr;		/* store old ISR here				*/
	unsigned char	old_state;		/* PICK MASK copy					*/
} IOKERN_TASK_TYPE;

/******************************************************************************
*/
static void IOKern_DOS_IRQ_Mask_Set( unsigned char irq, unsigned char state /* 0=On,1=Off */ )
{
	unsigned int  port;
	unsigned char value;

	port = IOKERN_DOS_PIC1_IMR;
	if ( irq >= 8 )
	{
		/* have to turn on/off IRQ2 at PIC1 (slave to master input) */
		if ( state ) value = inp( port ) |  ( 1 << 2 ); /* turn off PIC */
		else         value = inp( port ) & ~( 1 << 2 ); /* turn on PIC  */
		outp( port, value );

		irq = irq - 8;
		port = IOKERN_DOS_PIC2_IMR;
	}
	if ( state ) value = inp( port ) |  ( 1 << irq ); /* turn off PIC */
	else         value = inp( port ) & ~( 1 << irq ); /* turn on PIC  */
	outp( port, value );
}
/******************************************************************************
*/
static void IOKern_DOS_IRQ_Mask_Get( unsigned char irq, unsigned char * state )
{
	unsigned int  port;
	unsigned char value;
	if ( irq < 8 )
	{
		port = IOKERN_DOS_PIC1_IMR;
	}
	else
	{
		irq = irq - 8;
		port = IOKERN_DOS_PIC2_IMR;
	}
	value = inp( port ) & ( 1 << irq );
	*state = value;
}

/******************************************************************************
*/
typedef void interrupt (* IOKERN_DOS_VECTOR_TYPE )();

/******************************************************************************
*/
IOKERN_DOS_VECTOR_TYPE IOKern_DOS_Vector_Get( unsigned num )
{
	asm push es
	asm push bx
/* set ES to zero, so it points to the interrupt vector table at 0000:0000 */
	asm	xor bx,bx
	asm	mov es,bx
	asm	mov bx,[num]
/* x4 because each IVT entry (a far address) is 4 bytes long */
	asm	shl bx,1
	asm	shl bx,1
/* now ES:BX points to the vector we want; IVT[num].
Read the 32-bit vector (segment + offset) 'atomically',
without an interrupt happening in the middle of the read. */
	asm	les bx,es:[bx]
/* return 32-bit far address in DX:AX */
	asm	mov dx,es
	asm	mov ax,bx
	asm pop bx
	asm pop es
/* it DOES return a value...even though the compiler will complain! */
}

/******************************************************************************
*/
void IOKern_DOS_Vector_Set( unsigned num, IOKERN_DOS_VECTOR_TYPE h )
{
	asm push es
	asm push bx
/* set ES to zero, so it points to the interrupt vector table at 0000:0000 */
	asm	xor bx,bx
	asm	mov es,bx
	asm	mov bx,[num]
/* x4 because each IVT entry (a far address) is 4 bytes long */
	asm	shl bx,1
	asm	shl bx,1
/* I don't think 8088 or 80286 can do a 32-bit store,
so shut off interrupts and store 16 bits at a time. */
	asm	pushf
	asm	cli
	asm	mov ax,word ptr [h]
	asm	mov es:[bx],ax
	asm	mov ax,word ptr [h+2]
	asm	mov es:[bx+2],ax
	asm	popf
	asm pop bx
	asm pop es
}
/******************************************************************************
*/
#define	IOKERN_DOS_INT_GET(num)		IOKern_DOS_Vector_Get(num)
#define	IOKERN_DOS_INT_SET(num, fn)	IOKern_DOS_Vector_Set(num, (IOKERN_DOS_VECTOR_TYPE)fn)

#define IOKERN_TASK_QTY  17
IOKERN_TASK_TYPE  iokern_task[IOKERN_TASK_QTY];

/**********************************************************************************************************//**
 * 
 * @brief
 */
void IOKern_PIC_EOI( unsigned char irq )
{
	outp( IOKERN_DOS_PIC1_CMD, IOKERN_DOS_NSEOI );
	if ( irq >= 8 ) outp( IOKERN_DOS_PIC2_CMD, IOKERN_DOS_NSEOI );
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Supporting 'fast' interrupts
 */
void IOKern_Interrupt_Helper( IOKERN_TASK_ID_ENUM id, IOKERN_IRQ_ENUM irq, struct pt_regs * regs )
{
	iokern_task[id].result =
		( * iokern_task[id].task_fp )( irq, iokern_task[id].dev_id, regs );

	iokern_task[id].count++;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR0( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ1, IOKERN_IRQ_0, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ0, IOKERN_IRQ_0, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_0 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR1( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ1, IOKERN_IRQ_1, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ1, IOKERN_IRQ_1, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_1 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR2( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ2, IOKERN_IRQ_2, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ2, IOKERN_IRQ_2, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_2 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR3( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ3, IOKERN_IRQ_3, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ3, IOKERN_IRQ_3, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_3 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR4( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ4, IOKERN_IRQ_4, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ4, IOKERN_IRQ_4, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_4 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR5( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ5, IOKERN_IRQ_5, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ5, IOKERN_IRQ_5, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_5 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR6( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ6, IOKERN_IRQ_6, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ6, IOKERN_IRQ_6, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_6 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR7( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ7, IOKERN_IRQ_7, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ7, IOKERN_IRQ_7, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_7 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR8( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ8, IOKERN_IRQ_8, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ8, IOKERN_IRQ_8, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_8 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR9( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ9, IOKERN_IRQ_9, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ9, IOKERN_IRQ_9, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_9 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR10( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ10, IOKERN_IRQ_10, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ10, IOKERN_IRQ_10, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_10 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR11( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ11, IOKERN_IRQ_11, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ11, IOKERN_IRQ_11, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_11 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR12( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ12, IOKERN_IRQ_12, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ12, IOKERN_IRQ_12, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_12 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR13( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ13, IOKERN_IRQ_13, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ13, IOKERN_IRQ_13, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_13 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR14( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ14, IOKERN_IRQ_14, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ14, IOKERN_IRQ_14, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_14 );
#endif
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Interrupt service routine.  It will subsequently call the appropriate user function.
 * These are meant to be short operations.
 */
static void INTERRUPT IOKern_ISR15( struct pt_regs r )
{
#if defined( IOKERN_ISR_USE_THESE_GUTS )
	IOKERN_ISR_GUTS( IOKERN_TASK_ID_IRQ15, IOKERN_IRQ_15, r );
#else
	IOKERN_IRQ_START;
	IOKern_Interrupt_Helper( IOKERN_TASK_ID_IRQ15, IOKERN_IRQ_15, &r );
	IOKERN_IRQ_END( IOKERN_IRQ_15 );
#endif
}
/******************************************************************************
*/
IOKERN_ISR_FP iokern_isr_table[IOKERN_TASK_QTY] =
{
	IOKern_ISR0,
	IOKern_ISR1,
	IOKern_ISR2,
	IOKern_ISR3,
	IOKern_ISR4,
	IOKern_ISR5,
	IOKern_ISR6,
	IOKern_ISR7,
	IOKern_ISR8,
	IOKern_ISR9,
	IOKern_ISR10,
	IOKern_ISR11,
	IOKern_ISR12,
	IOKern_ISR13,
	IOKern_ISR14,
	IOKern_ISR15,
	NULL
};
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IOKern_IRQ_Test_Helper( unsigned int irq )
{
	int error_code = SUCCESS;
#if defined( IOKERN_CPU_REGION_MAP )
	int index;
	/* test for any CPU related conflicts */
	index = 0;
	while( SYS_TYPE__NONE != iokern_irq_map_list[index].type )
	{
		if ( irq == iokern_irq_map_list[index].irq )
		{ /* there is a possible conflict with other hardware */
			return( -((int)(index + 1)) );
		}
		index++;
	}
#endif

	switch( (IOKERN_IRQ_ENUM) irq )
	{
		case IOKERN_IRQ_NONE:
		//case IOKERN_IRQ_0:   -- don't want users to use the timer tick...
		case IOKERN_IRQ_1:
		//case IOKERN_IRQ_2:   -- rarely XT machines any more, must be IRQ9 instead.
		case IOKERN_IRQ_3:
		case IOKERN_IRQ_4:
		case IOKERN_IRQ_5:
		case IOKERN_IRQ_6:
		case IOKERN_IRQ_7:
		case IOKERN_IRQ_9:
		case IOKERN_IRQ_10:
		case IOKERN_IRQ_11:
		case IOKERN_IRQ_12:
		/* case IOKERN_IRQ_13: -- not allowed due NPU, not avail in PC104 */
		case IOKERN_IRQ_14:
		case IOKERN_IRQ_15:
		/* no longer usable case IOKERN_INT_33:  mouse function calls */
			break;
		default:
			error_code =  -EC_INTR_ERROR;
	}

	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IOKern_IRQ_Test( unsigned int irq )
{
	int error_code;

	error_code = IOKern_IRQ_Test_Helper( irq );
/*  TODO:  post error to error handler
	DEBUG: test error_code here to determine exact cause */
	if ( error_code ) return -EC_INTR_ERROR;

	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
void IOKern_IRQ_Invoke_ISR_Test( size_t irq )
{
	struct pt_regs r;

	memset( &r, 0, sizeof( struct pt_regs ) );
	( * iokern_isr_table[irq] )( r );
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
IOKERN_TASK_ID_ENUM IOKern_Task_ID_Get( unsigned int irq )
{
	IOKERN_TASK_ID_ENUM  task_id;

	if ( IOKERN_IRQ_NONE == irq )
	{
		task_id = IOKERN_TASK_ID_NONE;  /* nothing to do */
	}
	else if ( irq <= IOKERN_IRQ_15 )
	{
		task_id = (IOKERN_TASK_ID_ENUM) irq;
	}
	else
	{
		task_id = IOKERN_TASK_ID_NONE;
	}
	return task_id;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
IOKERN_TASK_TYPE * IOKern_Task_Handle_Get( unsigned int irq )
{
	size_t 	task_id;

	if ( SUCCESS != IOKern_IRQ_Test( irq ) ) return( NULL );

	task_id = 0;
	while( task_id < IOKERN_TASK_QTY )
	{
		if ( irq == iokern_task[task_id].number )
		{
			return( &iokern_task[task_id] );
		}
		task_id++;
	}
	/* not found */
	return NULL;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int IOKern_DOS_ISR_Install( unsigned char irq )
{
	IOKERN_TASK_ID_ENUM  task_id;
	int					 int_number;

	task_id = IOKern_Task_ID_Get( irq );
	if ( IOKERN_TASK_ID_NONE == task_id ) return SUCCESS; /* nothing to install */

	if ( ( IOKERN_TASK_ID_IRQ0 == task_id ) || ( IOKERN_TASK_ID_INT33 == task_id ) )
	{ /* avoid timer interrupt and mouse stuff for now */
		return -EC_INTERRUPT_UNAVAILABLE;
	}

//	if ( IOKERN_INT_33 == irq )
//	{
//		int_number = irq;
//	//	disable();
//		iokern_task[task_id].old_isr = (IOKERN_ISR_FP) IOKERN_DOS_INT_GET( int_number );
//		//IOKern_DOS_IRQ_Mask_Get( irq, &(iokern_task[task_id].old_state) );
//		IOKERN_DOS_INT_SET( int_number, iokern_isr_table[task_id] );
//		//IOKern_DOS_IRQ_Mask_Set( irq, 0 );  /* turn on PIC */
//	//	enable();
//	}
//	else
//	{


		if ( irq < 8 ) int_number = ( irq - 0 ) + 0x08;
		else           int_number = ( irq - 8 ) + 0x70;

	//	disable();
		iokern_task[task_id].old_isr = (IOKERN_ISR_FP) IOKERN_DOS_INT_GET( int_number );
		IOKern_DOS_IRQ_Mask_Get( irq, &(iokern_task[task_id].old_state) );
		IOKERN_DOS_INT_SET( int_number, iokern_isr_table[task_id] );
		IOKern_DOS_IRQ_Mask_Set( irq, 0 );  /* turn on PIC */
	//	enable();
//	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static void IOKern_DOS_ISR_Restore( unsigned char irq )
{
	IOKERN_TASK_ID_ENUM  task_id;
	int					 int_number;

	task_id = IOKern_Task_ID_Get( irq );
	if ( IOKERN_TASK_ID_NONE == task_id ) return; /* nothing to install */

//	if ( IOKERN_INT_33 == irq )
//	{
//		int_number = irq;
//		//IOKern_DOS_IRQ_Mask_Set( irq, iokern_task[task_id].old_pic_state );
//		IOKERN_DOS_INT_SET( int_number, iokern_task[task_id].old_isr );
//	}
//	else
//	{
		if ( irq < 8 ) int_number = ( irq - 0 ) + 0x08;
		else           int_number = ( irq - 8 ) + 0x70;

		IOKern_DOS_IRQ_Mask_Set( irq, iokern_task[task_id].old_state );
		IOKERN_DOS_INT_SET( int_number, iokern_task[task_id].old_isr );
//	}
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IOKern_DOS_IRQ_Request(	unsigned int	irq,
							IOKERN_TASK_FP	handler,
//							unsigned long	flags,
//							const char *	dev_name,
							void *			dev_id		/* private data */
							)
{
	int 					error_code;
	IOKERN_TASK_ID_ENUM  	task_id;
#define IOKERN_FUNCTION_NAME "IOKern_DOS_IRQ_Request"
 #if defined ( MSDOS_PRINTF_DEBUG_MESSAGES )
	Printf( "%s: ", IOKERN_FUNCTION_NAME );
	Printf( "irq=%d, ", irq );
 #endif
#undef IOKERN_FUNCTION_NAME
	if ( SUCCESS != IOKern_IRQ_Test( irq ) ) return -EC_INTR_ERROR; /* invalid irq */

	task_id = IOKern_Task_ID_Get( irq );
	if ( IOKERN_TASK_ID_NONE == task_id ) return SUCCESS; /* nothing to install */

	if ( NULL == handler ) return -EC_INTR_ERROR;						/* cannot have a null handler */
	if ( NULL != iokern_task[task_id].task_fp ) return -EC_BUSY;	/* already used */
//	iokern_task[task_id].type		= SYS_TYPE_MAKE(IOKERN_TASK_TYPE);
	iokern_task[task_id].task_fp	= handler;

//	if ( IRQF_TIMER & flags )
//	{
//		iokern_task[task_id].help_fp = IOKern_DOS_Timer_Periodic_Interrupt;
//		iokern_task[task_id].chain_to_old = IOKERN_CHAIN_TO_OLD_OFF;
//		switch( flags & IOKERN_CHAIN_TO_OLD__MASK )
//		{
//			case IOKERN_CHAIN_TO_OLD_OFF:
//				break;
//			case IOKERN_CHAIN_TO_OLD_TIMER:
//				if ( IOKERN_DOS_TIMER_PERIODIC_HW_IRQ == irq ) iokern_task[task_id].chain_to_old = IOKERN_CHAIN_TO_OLD_TIMER;
//				break;
//			case IOKERN_CHAIN_TO_OLD_NORMAL:
//				//if ( IOKERN_DOS_TIMER_PERIODIC_HW_IRQ != irq ) iokern_task[task_id].chain_to_old = IOKERN_CHAIN_TO_OLD_NORMAL;
//				break;
//		}
//	}
//	else
//	{
		iokern_task[task_id].help_fp		= NULL;
		iokern_task[task_id].chain_to_old	= IOKERN_CHAIN_TO_OLD_OFF;
//	}

	iokern_task[task_id].dev_id		= dev_id;
//	iokern_task[task_id].name		= dev_name;
	iokern_task[task_id].number		= irq;
	error_code = IOKern_DOS_ISR_Install( (unsigned char) irq );
 #if defined ( MSDOS_PRINTF_DEBUG_MESSAGES )
	Printf( "done\n" );
 #endif
	return error_code;
#undef IOKERN_FUNCTION_NAME
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
void IOKern_DOS_IRQ_Free( unsigned int 	irq
//						  void * 		dev_id
						)
{
	IOKERN_TASK_TYPE * 	task;

#define IOKERN_FUNCTION_NAME "IOKern_DOS_IRQ_Free"
 #if defined ( MSDOS_PRINTF_DEBUG_MESSAGES )
	Printf( "%s: ", IOKERN_FUNCTION_NAME );
	Printf( "irq=%d, ", irq );
 #endif
	task = IOKern_Task_Handle_Get( irq );
	if ( ( SUCCESS == IOKern_IRQ_Test( irq ) ) && ( NULL != task ) )
	{
		if ( task->task_fp )
		{
			IOKern_DOS_ISR_Restore( (unsigned char) irq );
			memset( task, 0, sizeof( IOKERN_TASK_TYPE ) );
			task->number = IOKERN_IRQ_NONE;
 #if defined ( MSDOS_PRINTF_DEBUG_MESSAGES )
			Printf( "success " );
 #endif
		}
	}
	else
	{
 #if defined ( MSDOS_PRINTF_DEBUG_MESSAGES )
	Printf( " " );
 #endif
	}
 #if defined ( MSDOS_PRINTF_DEBUG_MESSAGES )
	Printf( "done\n" );
 #endif
#undef IOKERN_FUNCTION_NAME
}
/******************************************************************************
*/
void IOKern_Resource_Termination( void )
{
	size_t				index;

	IOKERN_IRQ_START;
	for ( index = 0; index < IOKERN_TASK_QTY; index++ )
	{ /* restore any interrupts that are still mapped in */
		IOKern_DOS_IRQ_Free( iokern_task[index].number );
	}
	IOKERN_IRQ_ENABLE;
}
/******************************************************************************
*/
void IOKern_Resource_Initialization( void )
{
	size_t				index;

//	iokern_timer_periodic_chain_count = 0;
//	iokern_timer_peridic_chain_load   = 0;

//	memset( iokern_region_list, 0, IOKERN_REGION_QTY * sizeof( IOKERN_REGION_TYPE ) );
	memset( iokern_task, 0, IOKERN_TASK_QTY * sizeof( IOKERN_TASK_TYPE ) );

	for ( index = 0; index < IOKERN_TASK_QTY; index++ )
	{
		iokern_task[index].number = IOKERN_IRQ_NONE;
	}

#if defined( IOKERN_CPU_REGION_MAP )
	/* map region description to region list */
	index = 0;
	while( SYS_TYPE__NONE != iokern_cpu_region_list[index].type )
	{
		iokern_cpu_region_list[index].name = iokern_cpu_region_description[index];
		index++;
	}
	/* map irq/int description to irq/int list */
	index = 0;
	while( SYS_TYPE__NONE != iokern_irq_map_list[index].type )
	{
		iokern_irq_map_list[index].name = iokern_irq_map_description[index];
		index++;
	}
#endif
}


#endif /*------------------------------------------------- __MSDOS__ */

/**********************************************************************************************************/

#if(0)

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  A N A L O G  I N P U T   F U N C T I O N S   > > > > 	                                  */

/**********************************************************************************************************//**
 *
 * @brief
 * Obtains the analog input component (or board ID in this case) ID number.
 *
 * @param[out] id  The 16-bit ID number
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IAI_AI_ID_Get( int board_id, uint16_t * id )
{
	uint8_t   	lsb, msb;

	IO_Read_U8( board_id, IAI_ID_LSB, &lsb );
	IO_Read_U8( board_id, IAI_ID_MSB, &msb );
	*id = ( ((uint16_t) msb) << 8 ) | ((uint16_t) lsb);
#if defined( ID_ALWAYS_REPORT_AS_GOOD )
	*id = ID_IAI;
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 *
 */
int IAI16_AI_Channel_Get( int board_id, size_t channel, int * value )
{
	int			location;
	size_t		row;
	uint16_t 	scratch;
	struct reg_definition * definition = ( struct reg_definition * ) board_definition[board_id].definition;

	row = REG_LOCATION_LOGICAL_GET( IAI_AIN0 ) + channel;
	location = REG_LOCATION_SET( row, 0 ); /* since the registers are not contiguous */

	IO_Read_U16_Address_Increment( board_id, location, &scratch );
	/* TODO:  determine conversion */
	*value = (int) scratch;
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 *
 */
int IAI16_AI_Channel_Multiple_Get( 	int 	board_id,
									size_t 	channel_first,
									size_t 	channel_last,
									size_t 	table_offset,
									int * 	table
								 )
{
	int			location;
	size_t		row;
	size_t		index;
	uint16_t 	scratch;
	struct reg_definition * definition = ( struct reg_definition * ) board_definition[board_id].definition;

	index = 0;
	for ( channel = channel_first; channel <= channel_last; channel++ )
	{
		row = REG_LOCATION_LOGICAL_GET( IAI_AIN0 ) + channel;
		location = REG_LOCATION_SET( row, 0 ); /* since the registers are not contiguous */
		IO_Read_U16_Address_Increment( board_id, location, &scratch );
		/* TODO:  determine conversion */
		value[table_offset + index] = (int) scratch;
		index++;
	}
	return SUCCESS;
}




/**********************************************************************************************************//**
 *
 * @brief
 *
 */



/**********************************************************************************************************//**
 *
 * @brief
 *
 */


/**********************************************************************************************************//**
 *
 * @brief
 *
 */


/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IAI16_AI_ID( int board_id, int argc, char * argv[] )
{ /*  idi spi id */
	uint16_t 	id;
	(void) 		argc;
	(void) 		argv;
	IAI_AI_ID_Get( board_id, &id );
	printf( "IAI16 AI ID: 0x%04X\n", id );
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IAI16_AI_Channel( int board_id, int argc, char * argv[] )
{
	int 	error_code;		       /* used primarily for debug purposes */
	int 	channel;
	int		value;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	channel = (int) strtol( argv[0], NULL, 0 );

	if ( argc < 2 )
	{ /* get */
		error_code = IAI_AI_Channel_Get( board_id, channel, &value );
		printf( "IA%02d:  0x%04X  %d\n", channel, value, value );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IAI16_AI_All( int board_id, int argc, char * argv[] )
{
	int 	error_code;		       /* used primarily for debug purposes */
	int 	channel;
	int		value;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	channel = (int) strtol( argv[0], NULL, 0 );


	IAI16_AI_Channel_Multiple_Get( 	board_id,
									0,
									IAI16_CHANNEL_QTY - 1,
									0,
									table
								 );

	for ( channel = 0; channel < IAI16_CHANNEL_QTY; channel++ )
	{
		printf( "IA%02d:  0x%04X  %d\n", channel, table[channel], table[channel] );
	}



//	if ( argc < 2 )
//	{ /* get */
//		error_code = IAI_AI_Channel_Get( board_id, channel, &value );
//		printf( "IA%02d:  0x%04X  %d\n", channel, value, value );
//	}
	return SUCCESS;
}


/**********************************************************************************************************//**
 *
 * @brief
 */
static struct command_line_board  cmd_iai16[] =
{
		{	NULL,				CMD__IAI16_AI_ID,		"id",		"params: none.  Reports the AI board/component ID."		},
		{	NULL,				CMD__IAI16_AI_Channel,	"chan",		"params: <channel>"										},
		{	NULL,				CMD__IAI16_AI_All,	 	"all",		"gets all analog inputs"								},
		{   cmd_ido48_test,		CMD__IAI16_Test,		"test",		"semi-auto test functions"								},
	{	NULL,	NULL,				NULL,		NULL													},
};
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_IAI16( int board_id, int argc, char* argv[] )
{ /*  idi din <channel/all>  */
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;
    char *      endptr;
    //int			channel;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	if ( ID_IDO48 != board_id ) return -EC_BOARD_TYPE;

	error_code = -EC_SYNTAX;

	//channel = (int) strtol( argv[0], &endptr, 0 );
	strtol( argv[0], &endptr, 0 ); /* just want to know where it fails */
	if ( argv[0] != endptr )
	{  /* assume channel number */
		error_code = (* cmd_iai16[0].cmd_fnc )( board_id, argc, argv );
	}
	else
	{ /* otherwise a normal command */
		index = 0;
		while ( NULL != cmd_iai16[index].cmd_fnc )
		{
			if ( 0 == strcmpi( cmd_iai16[index].name, argv[0] )  )
			{
				argv_new = &(argv[1]);
				argc_new = argc - 1;
				if ( 0 == argc_new ) argv_new = NULL;
				error_code = (* cmd_iai16[index].cmd_fnc )( board_id, argc_new, argv_new );
				break;
			}
			index++;
		}
	}
	return error_code;
}






/**********************************************************************************************************/
#endif


/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  D I G I T A L  I N P U T   F U N C T I O N S   > > > > 	                                  */

/**********************************************************************************************************//**
 * 
 * @brief
 * Obtains the digital input component (or board ID in this case) ID number.
 *
 * @param[out] id  The 16-bit ID number
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IDI_DIN_ID_Get( int board_id, uint16_t * id )
{
	uint8_t   	lsb, msb;
	struct board_dataset *  dataset;

	dataset = (struct board_dataset *) board_definition[board_id].dataset;

	if ( MODE_JUMPERS_0 == dataset->mode_jumpers )
	{ /* we are in legacy mode */
		return -EC_MODE_LEGACY;
	}

	IO_Read_U8( board_id, __LINE__, IDI_ID_LSB, &lsb );
	IO_Read_U8( board_id, __LINE__, IDI_ID_MSB, &msb );
	*id = ( ((uint16_t) msb) << 8 ) | ((uint16_t) lsb);


#if defined( ID_ALWAYS_REPORT_AS_GOOD )
	*id = ID_DIN;
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Determines if the DIN component and/or board is present.  Returns true if not present
 * (i.e. error).
 *
 * @return A zero is returned if present, otherwise a 1 is returned.
 */
BOOL IDI_DIN_IsNotPresent( int board_id )
{
	uint16_t id;
	IDI_DIN_ID_Get( board_id, &id );
	if (ID_DIN == id ) return 0;
	return 1;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Obtains and reports a single digital input channel.
 *
 * @param[in]  channel channel to be read out.
 * @param[out] value   Pointer to the boolean value to be set based on the digital input value
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IDI_DIN_Channel_Get( int board_id, size_t channel, BOOL * value )
{
	size_t 	group;
	size_t 	bit;
	uint8_t reg_value;

	group = channel >> IDI_DIN_SHIFT_RIGHT;
	bit   = channel - group * IDI_DIN_GROUP_SIZE;

	IO_Read_U8( board_id, __LINE__, IDI_DI_GROUP0 + group /* channel */, &reg_value );

	if ( 0 != ( reg_value & ( 0x01 << bit ) ) ) *value = true;
	else                                        *value = false;

	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Reads the selected digital input port (8-bits).
 *
 * @param[in]  group  the group, range is 0 to 5.
 * @param[out] value  pointer to the destination for the data read out
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IDI_DIN_Group_Get( int board_id, size_t group, uint8_t * value )
{
	IO_Read_U8( board_id, __LINE__, IDI_DI_GROUP0 + group /* channel */, value );
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 *
 * @param[in]  channel channel to be cleared of its pending status.
 * @param[out] value  pointer to the destination for the data read out
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IDI_DIN_Pending_Clear( int board_id, size_t channel )
{
	size_t 	group;
	size_t 	bit;
	uint8_t reg_value;

	group = channel >> IDI_DIN_SHIFT_RIGHT;
	bit   = channel - group * IDI_DIN_GROUP_SIZE;

	if ( group >= IDI_DIN_GROUP_QTY ) return -EC_CHANNEL;

	reg_value = (uint8_t)( 0x01 << bit );
	IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group /* channel */, reg_value );
	return SUCCESS;
}

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  S P I   F U N C T I O N S    > > > > 													  */

/* #define SPI_PRINT_DEBUG 1
 */
/* #define SPI_PRINT_XFR_DEBUG 1
 */

/**********************************************************************************************************//**
 *
 * @brief
 * Retrieves the SPI ID register value.
 *
 * @param[out] id   The SPI component ID value.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int SPI_ID_Get_Helper( int board_id, int id_lsb, int id_msb, uint16_t * id )
{
	uint8_t   	lsb, msb;
	struct board_dataset *  dataset;

	dataset = (struct board_dataset *) board_definition[board_id].dataset;

#define FUNCTION_NAME__  "SPI_ID_Get_Helper"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( MODE_JUMPERS_0 == dataset->mode_jumpers )
	{ /* we are in legacy mode */
		return -EC_MODE_LEGACY;
	}

	IO_Read_U8( board_id, __LINE__, id_lsb, &lsb );
	IO_Read_U8( board_id, __LINE__, id_msb, &msb );
	*id = ( ((uint16_t) msb) << 8 ) | ((uint16_t) lsb);
#if defined( ID_ALWAYS_REPORT_AS_GOOD )
	*id = ID_SPI;
#endif
	return SUCCESS;
}

int SPI_ID_Get( int board_id, uint16_t * id )
{
#define FUNCTION_NAME__  "SPI_ID_Get"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	switch ( board_id )
	{
		case ID_IDI48:
			return SPI_ID_Get_Helper( board_id, IDI_SPI_ID_LSB, IDI_SPI_ID_MSB, id );
		case ID_IDO48:
			return SPI_ID_Get_Helper( board_id, IDO_SPI_ID_LSB, IDO_SPI_ID_MSB, id );
	}
	return -EC_BOARD;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Reports if the SPI component is available within the register space by matching a known ID.
 * The SPI register map is only enabled within the hardware if the hardware mode is not zero
 * (i.e. M1 and M0 jumpers on the board provide a nonzero value).
 *
 * @return A zero is returned if the SPI component ID is not found within the register space.
 */
int SPI_IsNotPresent( int board_id )
{
	int 	 				error_code;
	uint16_t 				id;
	struct board_dataset *  dataset;
	dataset = (struct board_dataset *) board_definition[board_id].dataset;

#define FUNCTION_NAME__  "SPI_IsNotPresent"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	/* This eliminates the need to continuously recheck the
	 * SPI_ID for every SPI Transaction.
	 */
	if ( true == dataset->spi_is_present ) return 0;

	error_code = SPI_ID_Get( board_id, &id );
	if ( error_code < 0 ) return error_code;
	if (ID_SPI == id )
	{
		dataset->spi_is_present	= true;
		return 0;
	}
	return 1;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Computes the half clock register value given a requested time interval.  It will also produce a
 * 'report' indicating the actual value (due to integer resolution) as well as a computed error between
 * requested and actual.  The error can be used to determine whether timing constraints are met.
 *
 * @param[in]  half_clock_request_sec  Request time interval in seconds.  Example: 20.0e-6 is 20uS.
 * @param[in]  half_clock_actual_sec   Actual computed time. If this pointer is NULL, then it is not output.
 * @param[out] error  Error between requested and actual.  If this pointer is NULL, then it is not output.
 * @param[out] hci    Half clock register value computed.  If this pointer is NULL, then it is not output.
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
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
 * 
 * @brief
 * Computes the SPI clock half clock register value given a requested SPI clock frequency.  It will also
 * produce a 'report' indicating the actual value (due to integer resolution) as well as a computed error
 * between requested and actual.  The eror can be used to determine whether timing constraints are met.
 *
 * @param[in]  clock_request_hz  Request clock frequency in Hertz.  Example: 1.0e6 is 1MHz.
 * @param[in]  clock_actual_hz   Actual computed frequency. If this pointer is NULL, then it is not output.
 * @param[out] error  Error between requested and actual.  If this pointer is NULL, then it is not output.
 * @param[out] hci    Half clock register value computed.  If this pointer is NULL, then it is not output.
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
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
	if ( SUCCESS != error_code ) return error_code;

	/* compute actual frequency */
	scratch = 1.0 / ( 2.0 * half_clock_actual_sec );
	if ( NULL != error           ) *error           = ( scratch - clock_request_hz ) / clock_request_hz;
	if ( NULL != clock_actual_hz ) *clock_actual_hz = scratch;
	if ( NULL != hci             ) *hci             = (uint16_t) hci_internal;
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Computes the half clock interval in seconds given the value from the half clock interval register.
 *
 * @param[in] half_clock_interval  Half clock interval register value
 * @return The time value as a double in units of seconds.
 */
double SPI_Calculate_Half_Clock_Interval_Sec( uint16_t half_clock_interval /* hci */ )
{
	double half_clock_interval_sec;
	half_clock_interval_sec = CLOCK_PERIOD_SEC * ( 4.0 + ((double) half_clock_interval ) );
	return half_clock_interval_sec;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Computes the time delay at the end of each byte transmitted.  It will only output the parameters
 * whose pointers are not NULL.
 *
 * @param[in]  spi_half_clock_interval_sec  Computed half clock interval in seconds
 * @param[in]  delay_request_sec            Requested time delay in seconds
 * @param[out] delay_actual_sec             Pointer to actual time delay computed, if not NULL.
 * @param[out] error                        Pointer to error value computed, if not NULL.
 * @param[out] ecd                          Pointer to the computed end-cycle-delay, if not NULL.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Calculate_End_Cycle_Delay( double    spi_half_clock_interval_sec, /* calculated half-clock interval     */
							       double    delay_request_sec,           /* requested end-delay interval       */
							       double *  delay_actual_sec,            /*  computed actual delay             */
							       double *  error,                       /* error between actual and desired   */
							       uint8_t * ecd                          /* computed count                     */
								)
{
	//double    	delay_between_words_sec;
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
 * 
 * @brief
 * Extracts the chip select behavior from the SPI configuration register.
 *
 * @param[out] chip_select_behavior   pointer to the destination of the value obtained.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Configuration_Chip_Select_Behavior_Get( int board_id, SPI_CSB_ENUM * chip_select_behavior )
{
	uint8_t   	scratch;

#define FUNCTION_NAME__  "SPI_Configuration_Chip_Select_Behavior_Get"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_CONFIG, &scratch );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_CONFIG, &scratch );	break;
	}

	*chip_select_behavior = (SPI_CSB_ENUM) ( scratch >> 4 ) & 0x03;
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Sets the chip select behavior to the SPI configuration register.
 *
 * @param[in] chip_select_behavior  enumerated value to be written to the register.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Configuration_Chip_Select_Behavior_Set( int board_id, SPI_CSB_ENUM chip_select_behavior )
{
	uint8_t   	scratch;
	int			spi_config_symbol;

#define FUNCTION_NAME__  "SPI_Configuration_Chip_Select_Behavior_Set"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	switch ( board_id )
	{
		case ID_IDI48:	spi_config_symbol = (int) IDI_SPI_CONFIG;	break;
		case ID_IDO48:	spi_config_symbol = (int) IDO_SPI_CONFIG;	break;
	}

	IO_Read_U8( board_id, __LINE__, spi_config_symbol, &scratch );

	scratch &= ~0x30;
	scratch |= (uint8_t) ( ( chip_select_behavior & 0x03 ) << 4 );

	IO_Write_U8( board_id, __LINE__, spi_config_symbol, scratch );
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Sets which hardware chip select line is to be used, either CS0 (channel 0) or CS1 (channel 1).
 *
 * @param[in] cs_to_channel1  true if CS to route to channel 1 (CS1), othwerwise channel 0 (CS0).
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Configuration_Chip_Select_Route_Set( int board_id, BOOL cs_to_channel1 )
{
	int			spi_config_symbol;
	uint8_t   	scratch;
	uint8_t		mask = 0x40;

#define FUNCTION_NAME__  "SPI_Configuration_Chip_Select_Route_Set"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	switch ( board_id )
	{
		case ID_IDI48:	spi_config_symbol = (int) IDI_SPI_CONFIG;	break;
		case ID_IDO48:	spi_config_symbol = (int) IDO_SPI_CONFIG;	break;
	}

	IO_Read_U8( board_id, __LINE__, spi_config_symbol, &scratch );

	if ( true == cs_to_channel1 ) scratch |= mask;
	else                          scratch &= ~mask;

	IO_Write_U8( board_id, __LINE__, spi_config_symbol, scratch );
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Sets which hardware chip select line is to be used, either CS0 (channel 0) or CS1 (channel 1).
 *
 * @param[out] chip_select_behavior   pointer to the destination of the value obtained.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Configuration_Chip_Select_Route_Get( int board_id, BOOL * cs_to_channel1 )
{
	uint8_t   	scratch;
	uint8_t		mask = 0x40;

#define FUNCTION_NAME__  "SPI_Configuration_Chip_Select_Route_Get"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_CONFIG, &scratch );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_CONFIG, &scratch );	break;
	}

	if ( scratch & mask ) *cs_to_channel1 = true;
	else                  *cs_to_channel1 = false;

	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Commits the configuration data structure to the hardware.
 *
 * @param[in] cfg  The software configuration data structure to be committed to hardware
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Configuration_Set( int board_id, struct spi_cfg * cfg )
{
	int			error_code;
	double 		scratch;
	double      hci_sec;     /* half clock interval in seconds */
	uint8_t   	config;
	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "SPI_Configuration_Set"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	config = (uint8_t) ( ( cfg->chip_select_behavior & 0x03 ) << 4 );
	if ( cfg->sclk_polarity         ) config |= 0x01;
	if ( cfg->sclk_phase            ) config |= 0x02;
	if ( cfg->sdi_polarity          ) config |= 0x04;
	if ( cfg->sdo_polarity          ) config |= 0x08;
	if ( cfg->sdio_wrap             ) config |= 0x80;
	if ( cfg->chip_select_route_ch1 ) config |= 0x40;

	switch ( board_id )
	{
		case ID_IDI48:	IO_Write_U8( board_id, __LINE__, IDI_SPI_CONFIG, config );	break;
		case ID_IDO48:	IO_Write_U8( board_id, __LINE__, IDO_SPI_CONFIG, config );	break;
	}
	//IO_Write_U8( board_id, SPI_CONFIG, config );

	if ( cfg->clock_hz > 0 )
	{ /* compute half_clock_interval */
		//scratch = ( 1.0 - ( 8.0 * CLOCK_PERIOD_SEC * cfg->clock_hz ) ) / (  2.0 * CLOCK_PERIOD_SEC * cfg->clock_hz );
		error_code = SPI_Calculate_Clock( cfg->clock_hz, NULL, NULL, &(cfg->half_clock_interval) );
		if ( SUCCESS != error_code ) return error_code;
	}
	hci_sec = SPI_Calculate_Half_Clock_Interval_Sec( cfg->half_clock_interval );

	if ( cfg->end_delay_ns > 0 )
	{
		scratch    = cfg->end_delay_ns * 1.0e-9;
		error_code = SPI_Calculate_End_Cycle_Delay( hci_sec, 				/* calculated half-clock interval     */
			       	   	   	   	   	   	   	   	    scratch,           		/* requested end-delay interval       */
			       	   	   	   	   	   	   	   	    NULL,            		/*  computed actual delay             */
			       	   	   	   	   	   	   	   	    NULL,                   /* error between actual and desired   */
			       	   	   	   	   	   	   	   	    &(cfg->end_cycle_delay) /* computed count                     */
			       	   	   	   	   	   	   	   	   );
		if ( SUCCESS != error_code ) return error_code;
	}

	switch ( board_id )
	{
		case ID_IDI48:	IO_Write_U8( board_id, __LINE__, IDI_SPI_HCI_LSB, (uint8_t)( cfg->half_clock_interval & 0xFF ) );	break;
		case ID_IDO48:	IO_Write_U8( board_id, __LINE__, IDO_SPI_HCI_LSB, (uint8_t)( cfg->half_clock_interval & 0xFF ) );	break;
	}
	//IO_Write_U8( board_id, SPI_HCI_LSB, (uint8_t)( cfg->half_clock_interval & 0xFF ) );


	switch ( board_id )
	{
		case ID_IDI48:	IO_Write_U8( board_id, __LINE__, IDI_SPI_HCI_MSB, (uint8_t)( cfg->half_clock_interval >> 8   ) );	break;
		case ID_IDO48:	IO_Write_U8( board_id, __LINE__, IDO_SPI_HCI_MSB, (uint8_t)( cfg->half_clock_interval >> 8   ) );	break;
	}
	//IO_Write_U8( board_id, SPI_HCI_MSB, (uint8_t)( cfg->half_clock_interval >> 8   ) );

	switch ( board_id )
	{
		case ID_IDI48:	IO_Write_U8( board_id, __LINE__, IDI_SPI_ECD, cfg->end_cycle_delay );	break;
		case ID_IDO48:	IO_Write_U8( board_id, __LINE__, IDO_SPI_ECD, cfg->end_cycle_delay );	break;
	}
	//IO_Write_U8( board_id, SPI_ECD, cfg->end_cycle_delay );

	if ( cfg != &(dataset->spi.cfg) )
	{
		memcpy( &(dataset->spi.cfg), &cfg, sizeof( struct spi_cfg ) );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Obtains the SPI configuration from the hardware.
 *
 * @param[out] cfg  SPI configuration data structure or data set
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Configuration_Get( int board_id, struct spi_cfg * cfg )
{
	uint8_t   	scratch;
	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "SPI_Configuration_Get"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_CONFIG, &scratch );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_CONFIG, &scratch );	break;
	}
	//IO_Read_U8( SPI_CONFIG, &scratch );

	cfg->chip_select_route_ch1  = ( scratch & 0x40 ) ? true : false;
	cfg->chip_select_behavior 	= (SPI_CSB_ENUM) ( scratch >> 4 ) & 0x03;
    cfg->sclk_polarity		 	= ( scratch & 0x01 ) ? true : false;
    cfg->sclk_phase   			= ( scratch & 0x02 ) ? true : false;
	cfg->sdi_polarity 			= ( scratch & 0x04 ) ? true : false;
	cfg->sdo_polarity 			= ( scratch & 0x08 ) ? true : false;
	cfg->sdio_wrap    			= ( scratch & 0x80 ) ? true : false;

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_HCI_LSB, &scratch );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_HCI_LSB, &scratch );	break;
	}
	//IO_Read_U8( board_id, SPI_HCI_LSB, &scratch );
	cfg->half_clock_interval = (uint16_t) scratch;
	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_HCI_MSB, &scratch );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_HCI_MSB, &scratch );	break;
	}
	//IO_Read_U8( board_id, SPI_HCI_MSB, &scratch );
	cfg->half_clock_interval |= ( (uint16_t) scratch) << 8;
	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_ECD, &(cfg->end_cycle_delay) );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_ECD, &(cfg->end_cycle_delay) );	break;
	}
	//IO_Read_U8( board_id, SPI_ECD, &(cfg->end_cycle_delay) );

	cfg->clock_hz = 1.0 / ( 2.0 * CLOCK_PERIOD_SEC * ( 4.0 + ((double) cfg->half_clock_interval) ) );
	cfg->end_delay_ns = 1.0e9 * CLOCK_PERIOD_SEC * 4.0 + 0.5 * ((double) cfg->end_cycle_delay) / cfg->clock_hz;

	if ( cfg != &(dataset->spi.cfg) )
	{
		memcpy( &(dataset->spi.cfg), &cfg, sizeof( struct spi_cfg ) );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Creates a human readable report of the SPI configuration data structure.
 *
 * @param[in] cfg  SPI configuration data structure pointer
 * @parm[out] out  File destination descriptor
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Report_Configuration_Text( struct spi_cfg * cfg, FILE * out )
{
	fprintf( out, "################################### SPI Configuration:\n" );
	fprintf( out, "sdio_wrap             = %s\n", cfg->sdio_wrap 	    	 ? "true" : "false" );
	fprintf( out, "sdo_polarity          = %s\n", cfg->sdo_polarity 		 ? "true" : "false" );
	fprintf( out, "sdi_polarity          = %s\n", cfg->sdi_polarity 		 ? "true" : "false" );
	fprintf( out, "sclk_phase            = %s\n", cfg->sclk_phase 			 ? "true" : "false" );
	fprintf( out, "sclk_polarity         = %s\n", cfg->sclk_polarity 		 ? "true" : "false" );
	fprintf( out, "chip_select_route_ch1 = %s\n", cfg->chip_select_route_ch1 ? "true" : "false" );
	fprintf( out, "chip_select_behavior  = " );

	switch( cfg->chip_select_behavior )
	{
		case CSB_SOFTWARE:		fprintf( out, "CSB_SOFTWARE" 	);	break;
		case CSB_BUFFER:		fprintf( out, "CSB_BUFFER" 		);	break;
		case CSB_uint8_t:		fprintf( out, "CSB_uint8_t" 	);	break;
		case CSB_uint16_t:		fprintf( out, "CSB_uint16_t" 	);	break;
		default:                fprintf( out, "undefined" 		);	break;
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
 * 
 * @brief
 * Produces a human readable report of the SPI status data structure.
 *
 * @param[in] status SPI status data structure pointer
 * @parm[out] out    File destination descriptor
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Report_Status_Text( struct spi_status * status, FILE * out )
{
	fprintf( out, "################################### SPI " );
	if ( status->tx_status ) fprintf( out, "TX" );
	else                     fprintf( out, "RX" );

	fprintf( out, " Status:\n" );

	fprintf( out, "full       = %s\n", status->full  ? "true" : "false" );
	fprintf( out, "empty      = %s\n", status->empty ? "true" : "false" );
	fprintf( out, "fifo size  = %d\n", status->fifo_size                );
	fprintf( out, "fifo count = %d\n", status->fifo_count               );

	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Initializes the SPI configuration data structure
 *
 * @parma[in] cfg  Pointer to the SPI configuration data structure to be initialized
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
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
	cfg->chip_select_route_ch1  = false;    /* default to channel 0 */
	cfg->chip_select_behavior 	= CSB_SOFTWARE;
	cfg->end_cycle_delay 		= 0;		/* shortest delay possible 		*/
	cfg->half_clock_interval 	= 0;		/* shortest interval possible 	*/

	cfg->clock_hz 				= 0.0;
	cfg->end_delay_ns 			= 0.0;

	return SUCCESS;
}

/**********************************************************************************************************//**
 * 
 * @brief
 * Builds a detailed status data structure of the transmit/write outgoing SPI data FIFO.
 * Reports the quantity of bytes currently in the transmit FIFO, full flag, empty flag,
 * the total size of the FIFO in bytes, and sets tx_status to true indicating that this
 * is status specific to the transmit FIFO.
 *
 * The status register has the following format:
 * status[7] 	full
 * status[6]    empty
 * status[5]    not used (future size expansion)
 * status[4:0]  number of bytes currently in the FIFO
 *
 * @param[out] status   Pointer to status data structure to be updated.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Status_Write( int board_id, struct spi_status * status )
{
	uint8_t reg_tx_status;

#define FUNCTION_NAME__  "SPI_Status_Write"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_TX_STATUS, &reg_tx_status );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_TX_STATUS, &reg_tx_status );	break;
	}

	status->fifo_count = (int)( reg_tx_status & SPI_FIFO_SIZE_BIT_WIDTH );
	status->full       = (BOOL)( reg_tx_status & 0x80 );
	status->fifo_size  = (int) SPI_FIFO_SIZE;
	status->empty      = (BOOL)( reg_tx_status & 0x40 );
	status->tx_status  = true;
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Returns the complete write/transmit FIFO status.
 *
 * The status register has the following format:
 * status[7] 	full
 * status[6]    empty
 * status[5]    not used (future size expansion)
 * status[4:0]  number of bytes currently in the FIFO
 *
 *
 * @param[out] full              FIFO full flag
 * @param[out] empty             FIFO empty flag
 * @param[out] bytes_in_fifo     a count of the number of bytes in the FIFO
 * @return     nothing
 */
void SPI_Status_Write_FIFO_Status( int board_id, BOOL * full,  BOOL * empty,  size_t * bytes_available )
{
#if defined( SPI_PRINT_XFR_DEBUG )
	uint8_t reg_buf[4];
	size_t  scratch;
	size_t	index;
	int     error_code;
#endif
	uint8_t	reg_tx_status;

#define FUNCTION_NAME__  "SPI_Status_Write_FIFO_Status"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__


#if defined( SPI_PRINT_XFR_DEBUG )
	switch ( board_id )
	{
		case ID_IDI48:
			for ( index = 0; index < 4; index++ )
				IO_Read_U8( board_id, __LINE__, IDI_SPI_TX_STATUS, &(reg_buf[index]) );
			break;
		case ID_IDO48:
			for ( index = 0; index < 4; index++ )
				IO_Read_U8( board_id, __LINE__, IDO_SPI_TX_STATUS, &(reg_buf[index]) );
			break;
	}

	error_code = 0;
	for ( index = 1; index < 4; index++ )
	{
		scratch = (size_t)( reg_buf[index] & SPI_FIFO_SIZE_BIT_WIDTH );
		if ( scratch > ( (size_t)( reg_buf[0] & SPI_FIFO_SIZE_BIT_WIDTH ) ) )
		{
			error_code = -1;
		}

		scratch = (size_t) ( reg_buf[index] & 0x40 );
		if ( scratch != ( (size_t) ( reg_buf[0] & 0x40 ) ) )
		{
			error_code = -1;
		}
	}

	if ( error_code < 0 )
	{
		printf( "###ERROR:  SPI_Status_Write_FIFO_Status: " );
		for ( index = 0; index < 4; index++ ) printf( ", 0x%02X", reg_buf[index] );
		printf( "\n" );
	}

	reg_tx_status = reg_buf[0];
#else
	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_TX_STATUS, &reg_tx_status );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_TX_STATUS, &reg_tx_status );	break;
	}
#endif

	switch( reg_tx_status & 0xC0 )
	{
		case 0x00: *full = false;	*empty = false;		break;
		case 0x40: *full = false;	*empty = true;		break;
		case 0x80: *full = true;	*empty = false;		break;
		case 0xC0: /* ought never happen */
			*full = true;
			*empty = true;
#define FUNCTION_NAME__  "SPI_Status_Write_FIFO_Status-FullEmpty"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

			break;
	}

	*bytes_available = (size_t) SPI_FIFO_SIZE_BIT_WIDTH - (size_t)( reg_tx_status & SPI_FIFO_SIZE_BIT_WIDTH );
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Returns the transmit/write FIFO full status flag.   It is preferable to use the SPI_Status_Write()
 * or SPI_Status_Write_FIFO_Status() because all status is retrieved at one time.
 *
 * @return Returns true if the transmit/write FIFO is full.
 */
BOOL SPI_Status_Write_FIFO_Is_Full( int board_id )
{
	uint8_t reg_tx_status;

#define FUNCTION_NAME__  "SPI_Status_Write_FIFO_Is_Full"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_TX_STATUS, &reg_tx_status );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_TX_STATUS, &reg_tx_status );	break;
	}
	if ( reg_tx_status & 0x80 ) return true;
	return false;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Returns the complete read/receive FIFO status.
 *
 * The status register has the following format:
 * status[7] 	full
 * status[6]    empty
 * status[5]    not used (future size expansion)
 * status[4:0]  number of bytes currently in the FIFO
 *
 *
 * @param[out] empty             FIFO empty flag
 * @param[out] bytes_available   a count of the number of bytes in the FIFO
 * @return     nothing
 */
void SPI_Status_Read_FIFO_Status( int board_id, BOOL * empty,  size_t * bytes_available )
{
#if defined( SPI_PRINT_XFR_DEBUG )
	uint8_t reg_value[4];
	size_t	index;
	size_t  scratch;
	int     error_code;
#endif
	uint8_t	reg_rx_status;

#define FUNCTION_NAME__  "SPI_Status_Read_FIFO_Status"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__


#if defined( SPI_PRINT_XFR_DEBUG )
	switch ( board_id )
	{
		case ID_IDI48:
			for ( index = 0; index < 4; index++ )
				IO_Read_U8( board_id, __LINE__, IDI_SPI_RX_STATUS, &(reg_value[index]) );
			break;
		case ID_IDO48:
			for ( index = 0; index < 4; index++ )
				IO_Read_U8( board_id, __LINE__, IDO_SPI_RX_STATUS, &(reg_value[index]) );
			break;
	}

	error_code = 0;
	for ( index = 1; index < 4; index++ )
	{
		scratch = (size_t)( reg_value[index] & SPI_FIFO_SIZE_BIT_WIDTH );
		if ( scratch < ( (size_t)( reg_value[0] & SPI_FIFO_SIZE_BIT_WIDTH ) ) )
		{
			error_code = -1;
		}

		scratch = (size_t) ( reg_value[index] & 0x40 );
		if ( scratch != ( (size_t) ( reg_value[0] & 0x40 ) ) )
		{
			error_code = -1;
		}
	}

	if ( error_code < 0 )
	{
		printf( "###ERROR:  SPI_Status_Read_FIFO_Status: " );
		for ( index = 0; index < 4; index++ ) printf( ", 0x%02X", reg_value[index] );
		printf( "\n" );
	}
	reg_rx_status = reg_value[0];
#else
	switch ( board_id )
	{
		case ID_IDI48:
			IO_Read_U8( board_id, __LINE__, IDI_SPI_RX_STATUS, &reg_rx_status );
			break;
		case ID_IDO48:
			IO_Read_U8( board_id, __LINE__, IDO_SPI_RX_STATUS, &reg_rx_status );
			break;
	}
#endif

	//IO_Read_U8( board_id, SPI_RX_STATUS, &reg_rx_status );
	*bytes_available = (size_t)( reg_rx_status & SPI_FIFO_SIZE_BIT_WIDTH );
	*empty           = (BOOL)( reg_rx_status & 0x40 );
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Builds a detailed status data structure of the receive/read incoming SPI data FIFO.
 * Reports the quantity of bytes currently in the receive FIFO, full flag, empty flag,
 * the total size of the FIFO in bytes, and sets tx_status to false indicating that this
 * is status specific to the receive FIFO.
 *
 * The status register has the following format:
 * status[7] 	full
 * status[6]    empty
 * status[5]    not used (future size expansion)
 * status[4:0]  number of bytes currently in the FIFO
 *
 * @param[out] status   Pointer to status data structure to be updated.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Status_Read( int board_id, struct spi_status * status )
{
	uint8_t reg_rx_status;

#define FUNCTION_NAME__  "SPI_Status_Read"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_RX_STATUS, &reg_rx_status );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_RX_STATUS, &reg_rx_status );	break;
	}

	status->fifo_count = (int)( reg_rx_status & 0x1F );
	status->full       = (BOOL)( reg_rx_status & 0x80 );
	status->fifo_size  = (int) SPI_FIFO_SIZE;
	status->empty      = (BOOL)( reg_rx_status & 0x40 );
	status->tx_status  = false;
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Returns the transmit/write FIFO empty status flag.  This function is typically used to wait for the
 * transmit/write FIFO to become empty.
 *
 * @return Returns true if the transmit/write FIFO is not empty.
 */
BOOL SPI_Status_Write_FIFO_Is_Not_Empty( int board_id )
{
	uint8_t reg_tx_status;

#define FUNCTION_NAME__  "SPI_Status_Write_FIFO_Is_Not_Empty"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_TX_STATUS, &reg_tx_status );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_TX_STATUS, &reg_tx_status );	break;
	}
	if ( reg_tx_status & 0x40 ) return false;
	return true;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Returns the receive/read FIFO empty status flag.  This function is typically used to determine
 * if the FIFO is empty.
 *
 * @return Returns true if the receive/read FIFO is not empty.
 */
BOOL SPI_Status_Read_FIFO_Is_Not_Empty( int board_id )
{
	uint8_t reg_rx_status;

#define FUNCTION_NAME__  "SPI_Status_Read_FIFO_Is_Not_Empty"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_RX_STATUS, &reg_rx_status );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_RX_STATUS, &reg_rx_status );	break;
	}
	if ( reg_rx_status & 0x40 ) return false;
	return true;
}
/**********************************************************************************************************//**
 * 
 * @brief
 *
 */
#define SPI_COMMIT_BIT_POSITION  0x01

/**********************************************************************************************************//**
 *
 * @brief
 * Sets/Clears the chip select or used to commit the transmit/write FIFO to the spi interface.  The mode
 * of operation is dependent on the chip_select_behavior.
 *
 * @param[in] commit   Used to write to the SCS_COMMIT bit.  Its behavior is dependent on the
 *                     chip_select_behavior.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Commit( int board_id, uint8_t commit )
{
#define FUNCTION_NAME__  "SPI_Commit"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;
	commit = commit & SPI_COMMIT_BIT_POSITION;
	switch ( board_id )
	{
		case ID_IDI48:	IO_Write_U8( board_id, __LINE__, IDI_SPI_COMMIT, commit );	break;
		case ID_IDO48:	IO_Write_U8( board_id, __LINE__, IDO_SPI_COMMIT, commit );	break;
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Sets/Clears the chip select or used to commit the transmit/write FIFO to the spi interface.  The mode
 * of operation is dependent on the chip_select_behavior.
 *
 * @param[out] commit   Used to write to the SCS_COMMIT bit.  Its behavior is dependent on the
 *                      chip_select_behavior.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Commit_Get( int board_id, uint8_t * commit )
{
	uint8_t value;

#define FUNCTION_NAME__  "SPI_Commit_Get"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__


	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;
	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_COMMIT, &value );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_COMMIT, &value );	break;
	}
	*commit = value & SPI_COMMIT_BIT_POSITION;
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 *
 * @return true if commit is inactive.  In other words, if the SPI state machine has completed the
 *   data transfer including any chip select changes that might occur at the end of buffer.
 */
BOOL SPI_Commit_Is_Inactive( int board_id )
{
	uint8_t value;

#define FUNCTION_NAME__  "SPI_Commit_Is_Inactive"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	switch ( board_id )
	{
		case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_COMMIT, &value );	break;
		case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_COMMIT, &value );	break;
	}
	//IO_Read_U8( board_id, SPI_COMMIT, &chip_select );
	if ( 0 == (value & SPI_COMMIT_BIT_POSITION) ) return true;
	return false;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Writes specifically to the SPI transmit/write data FIFO.  It does not attempt to correlate
 * the number of transmit bytes with receive bytes.  Its purpose is more for low level hardware
 * testing.  Note that this function has a signature identical to the fwrite() function (i.e.
 * make use of function pointers to guide destination of data).
 *
 * @param[in] buffer   Buffer containing the data to be written.
 * @param[in] size     Size of objects in bytes.
 * @param[in] count    Number of objects to be written
 * @param[out] fd_log  Optional log file to write the buffer too.  If NULL, then no logging.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned indicating an error.  In addition, a positive value is returned
 *         indicating the number of actual objects written.
 */
int SPI_FIFO_Write( int board_id, const void * buffer, size_t size, size_t count, FILE * fd_log )
{
	int 	error_code;       /* used primarily for debug purposes */
	size_t	bytes_available;
	BOOL	empty;
	BOOL	full;
	size_t  index;
	size_t  qty_objects;
	size_t 	qty_bytes;
	int		reg_symbol;

#define FUNCTION_NAME__  "SPI_FIFO_Write"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	error_code = SUCCESS;

	if ( (size * count) > SPI_FIFO_SIZE ) return -EC_PARAMETER;

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	SPI_Status_Write_FIFO_Status( board_id, &full, &empty, &bytes_available );

	qty_objects =  bytes_available / size; /* max number of objects that can be processed */
	if ( count < qty_objects ) qty_objects = count;

	qty_bytes = qty_objects * size;

//printf( "SPI_FIFO_Write: count = %u, size = %u\n", count, size );
//printf( "SPI_FIFO_Write: bytes_in_fifo = %u, qty_objects = %u\n", bytes_in_fifo, qty_objects );

	switch ( board_id )
	{
		case ID_IDI48:	reg_symbol = (int) IDI_SPI_DATA;	break;
		case ID_IDO48:	reg_symbol = (int) IDO_SPI_DATA;	break;
	}
	for ( index = 0; index < qty_bytes; index++ ) IO_Write_U8( board_id, __LINE__, reg_symbol, ((uint8_t *) buffer)[index] );

	if ( NULL != fd_log )
	{
		error_code = fwrite( buffer, size, qty_objects, fd_log );
	}

	if ( SUCCESS == error_code ) error_code = ( (int) qty_objects );
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Reads from the SPI receive/read data FIFO.  It does not attempt to correlate the number of
 * transmit bytes with receive bytes.  Its purpose is more for low level hardware
 * testing.  Note that this function has a signature identical to the fread() function (i.e.
 * make use of function pointers to guide sourcing of data).
 *
 * @param[in] buffer   Buffer for the data destination.
 * @param[in] size     Size of objects in bytes.
 * @param[in] count    Number of objects to be read
 * @param[out] fd_log  Optional log file to write the buffer too.  If NULL, then no logging.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned indicating an error.  In addition, a positive value is returned
 *         indicating the number of actual objects written.
 */
int SPI_FIFO_Read( int board_id, const void * buffer, size_t size, size_t count, FILE * fd_log )
{
	int 	error_code;       /* used primarily for debug purposes */
	size_t	bytes_available;
	BOOL	empty;
	size_t  index;
	size_t  qty_objects;
	size_t 	qty_bytes;
	int		reg_symbol;

#define FUNCTION_NAME__  "SPI_FIFO_Read"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	error_code = SUCCESS;

	if ( (size * count) > SPI_FIFO_SIZE ) return -EC_PARAMETER;

	if ( SPI_IsNotPresent( board_id ) ) return -EC_SPI_NOT_FOUND;

	SPI_Status_Read_FIFO_Status( board_id, &empty, &bytes_available );

	qty_objects =  bytes_available / size; /* max number of objects that can be processed */
	if ( count < qty_objects ) qty_objects = count;

	qty_bytes = qty_objects * size;

	switch ( board_id )
	{
		case ID_IDI48:	reg_symbol = (int) IDI_SPI_DATA;	break;
		case ID_IDO48:	reg_symbol = (int) IDO_SPI_DATA;	break;
	}
	for ( index = 0; index < qty_bytes; index++ ) IO_Read_U8( board_id, __LINE__, reg_symbol, &(((uint8_t *) buffer)[index]) );

	if ( NULL != fd_log )
	{
		error_code = fwrite( buffer, size, qty_objects, fd_log );
	}

	if ( SUCCESS == error_code ) error_code = ( (int) qty_objects );
	return error_code;
}

/**********************************************************************************************************//**
 * 
 * @brief
 */
static void SPI_Data_Write_Read_Helper_Read( int 			board_id,
											 size_t		 	object_size,
											 size_t			object_total_count,
											 BOOL			active_rx,
											 size_t *		index_rx,
											 const void * 	rx_buffer
										   )
{
	BOOL	rx_empty;
	int		reg_symbol;
	size_t	rx_bytes_available;
	size_t	object_qty;
	size_t	object_remaining;
	size_t	index_object;
	size_t	index;
	size_t  index_byte;
	uint8_t bit_bucket;   	/* dump bytes into no-where */

#if defined( SPI_PRINT_XFR_DEBUG )
	printf( "  SPI_Data_Write_Read_Helper_Read: " );
#endif

#define FUNCTION_NAME__  "SPI_Data_Write_Read_Helper_Read"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	/* get status simultaneously */
	SPI_Status_Read_FIFO_Status( board_id, &rx_empty, &rx_bytes_available );
	if ( rx_bytes_available >= object_size )
	{ /* read data */
		object_qty = rx_bytes_available / object_size;		/* number of objects to be read-in 											*/
		object_remaining = object_total_count - *index_rx;	/* total objects that can be potentially read and deposited to rx-buffer 	*/
		if ( object_qty > object_remaining )
		{ /* only process the remainder of objects */
			object_qty = object_total_count - *index_rx;	/* adjusted actual total objects that can deposited to rx-buffer 			*/
		}

		switch ( board_id )
		{
			case ID_IDI48:	reg_symbol = (int) IDI_SPI_DATA;	break;
			case ID_IDO48:	reg_symbol = (int) IDO_SPI_DATA;	break;
		}

		if ( active_rx )
		{
			for ( index_object = 0; index_object < object_qty; index_object++ )
			{
				index_byte = (*index_rx) * object_size;
				for ( index = 0; index < object_size; index++ )
				{
					IO_Read_U8( board_id, __LINE__, reg_symbol, &(((uint8_t *) rx_buffer)[index_byte + index]) );
#if defined( SPI_PRINT_XFR_DEBUG )
global_internal_rx_byte_count++;
#endif
				}
				*index_rx = *index_rx + 1;
			}
		}
		else
		{
			for ( index_object = 0; index_object < object_qty; index_object++ )
			{
				index_byte = (*index_rx) * object_size;
				for ( index = 0; index < object_size; index++ )
				{
					IO_Read_U8( board_id, __LINE__, reg_symbol, &bit_bucket ); /* toss */
#if defined( SPI_PRINT_XFR_DEBUG )
global_internal_rx_byte_count++;
#endif
				}
				*index_rx = *index_rx + 1;
			}
		}
#if defined( SPI_PRINT_XFR_DEBUG )
printf( "object_qty=%u, rx_bytes_available=%u, object_remaining=%u, object_size=%u", object_qty, rx_bytes_available, object_remaining, object_size );
#endif
	}
#if defined( SPI_PRINT_XFR_DEBUG )
printf( ", index_rx=%u\n", *index_rx );
#endif

}
/**********************************************************************************************************//**
 *
 * @brief
 *
 * returns true if user wishes to break.
 */
static void SPI_Data_Write_Read_Helper_Commit(  int				board_id,
												size_t			object_size,
												SPI_CSB_ENUM	csb,
												BOOL  			commit_ready,
												BOOL * 			commit_valid
											 )
{
//	BOOL 	tx_full;
//	BOOL 	tx_empty;
//	size_t 	tx_bytes_available;
	(void)	object_size;

#define FUNCTION_NAME__  "SPI_Data_Write_Read_Helper_Commit"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

//	/* get status simultaneously */
//	SPI_Status_Write_FIFO_Status( &tx_full, &tx_empty, &tx_bytes_available );
	if ( true == commit_ready )
	{
		if ( false == *commit_valid )
		{
			if ( CSB_SOFTWARE  != csb ) SPI_Commit( board_id, 0xFF );    /* does not matter what is written 	*/
		}
#if defined( SPI_PRINT_DEBUG )
printf( "SPI_Data_Write_Read_Helper_Commit:  commit_valid = true\n" );
#endif
		*commit_valid = true;
	}
	else
	{
		if ( CSB_BUFFER == csb )
		{	/* wait for the buffer to empty - so that Chip select can demarcate the block */
			if ( SPI_Commit_Is_Inactive( board_id ) )
			{
#if defined( SPI_PRINT_DEBUG )
printf( "SPI_Data_Write_Read_Helper_Commit:  commit_valid = false  (CSB = BUFFER)\n" );
#endif
				*commit_valid = false; /* while commit_valid is true we are blocking tx buffer filling 	*/
			}
		}
		else
		{
#if defined( SPI_PRINT_DEBUG )
printf( "SPI_Data_Write_Read_Helper_Commit:  commit_valid = false\n" );
#endif
			*commit_valid = false; /* go ahead with more TX buffer filling */
		}
	}
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static void SPI_Data_Write_Read_Helper_Write(	int				board_id,
												size_t		 	object_size,
												size_t			object_total_count,
												SPI_CSB_ENUM	csb,
												BOOL			active_tx,
												BOOL 			commit_valid,
												BOOL * 			commit_ready,
												size_t *		index_tx,
												const void * 	tx_buffer
										   )
{
	BOOL 			tx_full;
	BOOL 			tx_empty;
	int				reg_symbol;
	size_t 			tx_bytes_available;
	size_t			object_qty;
	size_t			object_remaining;
	size_t			index_object;
	size_t			index;
	size_t  		index_byte;

#if defined( SPI_PRINT_XFR_DEBUG )
	int				reg_tx_status, reg_rx_status;
	uint8_t			reg_tx_status_value, reg_rx_status_value;
#endif

	const uint8_t  	tx_dummy_value = 0x00;


#if defined( SPI_PRINT_XFR_DEBUG )
printf( "  SPI_Data_Write_Read_Helper_Write: " );
#endif

#define FUNCTION_NAME__  "SPI_Data_Write_Read_Helper_Write"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( false == commit_valid ) /* go ahead and fill the buffer hand shake */
	{
#if defined( SPI_PRINT_XFR_DEBUG )
printf( " GO" );
#endif
		if ( false == *commit_ready )
		{ /* fill buffer */
#if defined( SPI_PRINT_XFR_DEBUG )
printf( ", FILL" );
#endif
			/* get status simultaneously */
			SPI_Status_Write_FIFO_Status( board_id, &tx_full, &tx_empty, &tx_bytes_available );
			if ( tx_bytes_available >= object_size )
			{	/* write data */
				object_qty = tx_bytes_available / object_size; /* available to be written */
				object_remaining = object_total_count - *index_tx;
				if ( object_qty > object_remaining )
				{ /* only process the remainder of objects */
					object_qty = object_total_count - *index_tx;
				}

				if ( CSB_BUFFER   == csb )
				{ /* can only write one object at a time */
					if ( object_qty >= 1 ) object_qty = 1;
				}
#if defined( SPI_PRINT_DEBUG )
printf( "  SPI_Data_Write_Read_Helper_Write: TX object_qty = %d\n", (int) object_qty );
#endif
				switch ( board_id )
				{
					case ID_IDI48:
						reg_symbol = (int) IDI_SPI_DATA;
#if defined( SPI_PRINT_XFR_DEBUG )
						reg_tx_status = IDI_SPI_TX_STATUS;
						reg_rx_status = IDI_SPI_RX_STATUS;
#endif
						break;
					case ID_IDO48:
						reg_symbol = (int) IDO_SPI_DATA;
#if defined( SPI_PRINT_XFR_DEBUG )
						reg_tx_status = IDO_SPI_TX_STATUS;
						reg_rx_status = IDO_SPI_RX_STATUS;
#endif
						break;
				}

				if ( active_tx )
				{
#if defined( SPI_PRINT_DEBUG )
printf( "  SPI_Data_Write_Read_Helper_Write: TX active\n" );
#endif

#if defined( SPI_PRINT_XFR_DEBUG )
printf( ", DSTATUS:" );
#endif
					for ( index_object = 0; index_object < object_qty; index_object++ )
					{
#if defined( SPI_PRINT_DEBUG )
printf( "  SPI_Data_Write_Read_Helper_Write: TX bytes:" );
#endif
						index_byte = *index_tx * object_size;
						for ( index = 0; index < object_size; index++ )
						{
#if defined( SPI_PRINT_DEBUG )
printf( " 0x%02X", ((uint8_t *) tx_buffer)[index_byte + index] );
#endif
							IO_Write_U8( board_id, __LINE__, reg_symbol, ((uint8_t *) tx_buffer)[index_byte + index] );


#if defined( SPI_PRINT_XFR_DEBUG )
global_internal_tx_byte_count++;
# if defined( __MSDOS__ )
delay( 200 /* ms */ );  /* give readout time plenty of time to catch up */
#endif
IO_Read_U8( board_id, __LINE__, reg_tx_status, &reg_tx_status_value );
IO_Read_U8( board_id, __LINE__, reg_rx_status, &reg_rx_status_value );
printf( " [0x%02X.0x%02X]", reg_tx_status_value, reg_rx_status_value );
#endif


						}
						*index_tx = *index_tx + 1;
					}
#if defined( SPI_PRINT_DEBUG )
printf( "\n" );
#endif
				}
				else
				{
#if defined( SPI_PRINT_DEBUG )
printf( "  SPI_Data_Write_Read_Helper_Write: TX inactive\n" );
#endif


#if defined( SPI_PRINT_XFR_DEBUG )
printf( ", NSTATUS:" );
#endif
					for ( index_object = 0; index_object < object_qty; index_object++ )
					{
						//index_byte = *index_tx * object_size;
						for ( index = 0; index < object_size; index++ )
						{
							IO_Write_U8( board_id, __LINE__, reg_symbol, tx_dummy_value ); /* send anything */

#if defined( SPI_PRINT_XFR_DEBUG )
global_internal_tx_byte_count++;
# if defined( __MSDOS__ )
delay( 200 /* ms */ );  /* give readout time plenty of time to catch up */
#endif
IO_Read_U8( board_id, __LINE__, reg_tx_status, &reg_tx_status_value );
IO_Read_U8( board_id, __LINE__, reg_rx_status, &reg_rx_status_value );
printf( " [0x%02X.0x%02X]", reg_tx_status_value, reg_rx_status_value );
#endif

						}
						*index_tx = *index_tx + 1;
					}

				}
				*commit_ready = true;
#if defined( SPI_PRINT_XFR_DEBUG )
printf( ", object_qty=%u, tx_bytes_available=%u, object_remaining=%u, object_size=%u", object_qty, tx_bytes_available, object_remaining, object_size );
printf( ", index_tx=%u", *index_tx );
#endif

#if defined( SPI_PRINT_DEBUG )
printf( "SPI_Data_Write_Read_Helper_Write TX request\n" );
#endif
			}
			else
			{
				/* wait/skip - not enough room to add to buffer yet */
printf( ", FULL" );
			}
		}
		else
		{
			/* Already filled or there is no room to add more to buffer */
printf( ", FULL" );
		}
	}
	else
	{ /* hand shake complete -- acknowledgement */
#if defined( SPI_PRINT_DEBUG )
printf( "SPI_Data_Write_Read_Helper_Write TX ack\n" );
#endif
#if defined( SPI_PRINT_XFR_DEBUG )
printf( " ACK" );
#endif
		*commit_ready = false;
	}
#if defined( SPI_PRINT_XFR_DEBUG )
printf( "\n" );
#endif
}
/**********************************************************************************************************//**
 *
 * @brief
 *
 * If CSB = software.
 *   Stuff as fast as you can.
 *
 * If CSB == buffer.
 *   if object size = 1, then stuff as fast as you can.  The chip select will go active
 *      and only go inactive when done transmitting.  This is best used for objects
 *      whos size is less than SPI_FIFO_SIZE.
 *   if object size != 1, then stuff only that quantity and wait until buffers are
 *     empty in order to have the chip select wrap around the data.
 *
 * If CSB == uint8_t
 *   Stuff as fast as you can.
 *
 * If CSB == uint16_t
 *   Stuff two bytes at a time as fast as you can.  The object_size must be set to 2
 *   for this to work.
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Data_Write_Read( 	int 		 board_id,
							size_t		 object_size,  		/**< TX & RX object size: 1 <= object_size <= FIFO_SIZE */
							size_t		 tx_object_count, 	/**< object count */
							const void * tx_buffer,
							size_t		 rx_object_count,  	/**< object count */
							const void * rx_buffer
						)
{
	int				error_code;
	SPI_CSB_ENUM	csb;
	size_t			rx_bytes_available;
	BOOL			rx_empty;
	size_t			tx_bytes_available;
	BOOL			active_tx;
	BOOL			active_rx;
	BOOL			tx_empty;
	BOOL			tx_full;
	size_t 			index_tx;			/* object index 						*/
	size_t 			index_rx;
//	size_t			index_byte;			/* absolute byte index					*/
//	size_t			index;				/* byte index relative to object   		*/
//	size_t			index_object;
//	size_t			object_qty;
//	size_t			object_remaining;
	BOOL 			commit_valid;
	BOOL			commit_ready;
	uint8_t 		bit_bucket; /* tossed */
//	const uint8_t  	tx_dummy_value = 0x00;

#if defined( SPI_PRINT_DEBUG )
#if defined( __MSDOS__ )
	static size_t   internal_count = 0;
#endif
#endif

	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "SPI_Data_Write_Read"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

#if ( 1 == SPI_LOGGING )
	if ( SPI_LOG_ACTIVE == dataset->spi.log_state )
	{
		fprintf( dataset->spi.log_file, "SPI_Data_Write_Read:" );
	}
#endif

#if defined( SPI_PRINT_DEBUG )
	printf( "*** SPI_Data_Write_Read:  Entry.\n" );
#endif
#if defined( SPI_PRINT_XFR_DEBUG )
	printf( "*** SPI_Data_Write_Read:  Entry.\n" );
#endif

	index_tx = 0;
	index_rx = 0;


/* TEST FOR VALIDITY OF PARAMETERS */
	/* see if there is anything to do */
	if ( ( NULL == tx_buffer ) && ( NULL == rx_buffer ) )
	{ /* no buffers to read or write to, therefore, nothing to be done */
		error_code = SUCCESS;
		goto SPI_Data_Write_Read_Error_Exit;
	}
	if ( SPI_IsNotPresent( board_id ) )
	{ /* SPI module not present in hardware, therefore, nothing to be done */
		error_code = -EC_SPI_NOT_FOUND;
		goto SPI_Data_Write_Read_Error_Exit;
	}

	/* test for transmit and receive buffers */
	if ( ( NULL != tx_buffer ) && ( NULL != rx_buffer ) )
	{ /* if both tx/rx buffers are available then test for.... */
		if ( tx_object_count != rx_object_count )
		{ /* buffers not same size, error out */
			error_code = -EC_SPI_BUFFER_MISMATCH;
			goto SPI_Data_Write_Read_Error_Exit;
		}
	}
	if ( NULL != tx_buffer )
	{ /* transmit data from tx_buffer */
		active_tx = true;
	}
	else
	{ /* transmit dummy data */
		tx_object_count = rx_object_count;
		active_tx       = false; /* transmit dummy data of tx_object_count quantity */
	}
	if ( NULL != rx_buffer )
	{ /* capture received data to rx_buffer */
		active_rx        = true;
	}
	else
	{ /* toss any received data */
		rx_object_count = tx_object_count;
		active_rx       = false; /* toss out quantity of rx_object_count  */
	}

	/* obtain the hardware chip select behavior */
	SPI_Configuration_Chip_Select_Behavior_Get( board_id, &csb );

	if ( CSB_SOFTWARE == csb )
	{ /* software controlled chip select */
		uint8_t chip_select;
		if ( 0 == SPI_Commit_Get( board_id, &chip_select ) )
		{ /* turn on chip select now */
#if defined( SPI_PRINT_DEBUG )
			printf( "SPI_Data_Write_Read: Warning Chip select not active, setting it now\n" );
#endif
			SPI_Commit( board_id, 0xFF );
		}
	}

	if ( CSB_uint16_t == csb )
	{ /* Requires even number of bytes in transmit buffer in order for this to work  */
		if ( object_size & 0x01 )
		{ /* odd number of bytes detected for buffers */
			printf( "SPI_Data_Write_Read: Object Size is set to " );
#if defined( __MSDOS__ )
			printf( "%d", object_size );
#else
			printf( "%lu", object_size );
#endif
			printf( ", and ought to be set to 2\n" );
			error_code = -EC_SPI_BUFFER_SIZE_ODD;
			goto SPI_Data_Write_Read_Error_Exit;
		}
	}

#if defined( SPI_PRINT_DEBUG )
	printf( "SPI_Data_Write_Read:\n" );
	#if defined( __MSDOS__ )
	printf( "  object_size       = %d\n", object_size );
	printf( "  tx_object_count   = %d\n", tx_object_count );
	printf( "  rx_object_count   = %d\n", rx_object_count );
	printf( "  index_tx          = %d\n", index_tx );
	printf( "  index_rx          = %d\n", index_rx );
	printf( "  tx_buffer         = %p\n", tx_buffer );
	printf( "  rx_buffer         = %p\n", rx_buffer );
	#else
	printf( "  object_size       = %lu\n", object_size );
	printf( "  tx_object_count   = %lu\n", tx_object_count );
	printf( "  rx_object_count   = %lu\n", rx_object_count );
	printf( "  index_tx          = %lu\n", index_tx );
	printf( "  index_rx          = %lu\n", index_rx );
	printf( "  tx_buffer         = %p\n", tx_buffer );
	printf( "  rx_buffer         = %p\n", rx_buffer );
	#endif
	printf( "  active_tx         = %s\n", active_tx ? "true" : "false" );
	printf( "  active_rx         = %s\n", active_rx ? "true" : "false" );

	if ( NULL != tx_buffer )
	{ /* dump up to the first 16-bytes found in the transmit buffer */
		size_t count = object_size * tx_object_count;
		if ( count > HEX_DUMP_BYTES_PER_LINE ) count = HEX_DUMP_BYTES_PER_LINE;
		Hex_Dump_Line( 0, count, (uint8_t *) tx_buffer, stdout );
	}
#endif

//	csb_buffer_mode_override = false;
	if ( CSB_BUFFER == csb )
	{ /* In order for the chip select to wrap around a complete object, the maximum
	     size object we can handle is the SPI_FIFO_SIZE.
	  */
		if ( object_size > SPI_FIFO_SIZE )
		{
			error_code = -EC_SPI_OBJECT_SIZE;
			goto SPI_Data_Write_Read_Error_Exit;
		}
#if ( 1 == SPI_LOGGING )
		if ( SPI_LOG_ACTIVE == dataset->spi.log_state )
		{
			fprintf( dataset->spi.log_file, " CSB_BUFFER" );
		}
#endif
	}

/* BEGIN SPI TRANSACTIONS */
	/* initially need to make sure that both TX and RX are empty */
	/* wait for TX FIFO to be empty - most likely it will be */
	SPI_Status_Write_FIFO_Status( board_id, &tx_full, &tx_empty, &tx_bytes_available );
	if ( ( false == tx_empty ) && ( CSB_SOFTWARE != csb ) ) SPI_Commit( board_id, 0xFF );
	do
	{ //TODO: need a time out of some sort here and then return an error code.
		error_code = SUCCESS;
		if ( dataset->quit_application ) error_code = -EC_APP_TERMINATE_CTRLC;
		if ( Character_Get( NULL )     ) error_code = -EC_APP_TERMINATE;
		if ( SUCCESS != error_code     )
		{
			printf( "SPI_Data_Write_Read initial empty tx\n" );
			goto SPI_Data_Write_Read_Error_Exit;
		}
		SPI_Status_Write_FIFO_Status( board_id, &tx_full, &tx_empty, &tx_bytes_available );
	} while ( false == tx_empty );

	/* read out any remaining RX-FIFO data */
	do
	{ //TODO: need a time out of some sort here and then return an error code.
		error_code = SUCCESS;
		if ( dataset->quit_application ) error_code = -EC_APP_TERMINATE_CTRLC;
		if ( Character_Get( NULL )     ) error_code = -EC_APP_TERMINATE;
		if ( SUCCESS != error_code     )
		{
			printf( "SPI_Data_Write_Read initial empty rx\n" );
			goto SPI_Data_Write_Read_Error_Exit;
		}
		SPI_Status_Read_FIFO_Status( board_id, &rx_empty, &rx_bytes_available );
		if ( false == rx_empty )
		{
			switch ( board_id )
			{
				case ID_IDI48:	IO_Read_U8( board_id, __LINE__, IDI_SPI_DATA, &bit_bucket  ); /* toss */	break;
				case ID_IDO48:	IO_Read_U8( board_id, __LINE__, IDO_SPI_DATA, &bit_bucket  ); /* toss */	break;
			}
		}
	} while ( false == rx_empty );

	index_tx     = 0;
	index_rx     = 0;
	commit_valid = false;
	commit_ready = false;
	while ( index_tx < tx_object_count )
	{
		error_code = SUCCESS;
		if ( dataset->quit_application ) error_code = -EC_APP_TERMINATE_CTRLC;
		if ( Character_Get( NULL )     ) error_code = -EC_APP_TERMINATE;
		if ( SUCCESS != error_code     )
		{
			printf( "SPI_Data_Write_Read main loop\n" );
			goto SPI_Data_Write_Read_Error_Exit;
		}

#if defined( SPI_PRINT_XFR_DEBUG )
# if defined( __MSDOS__ )
		delay( 250 /* ms */ );  /* give readout time plenty of time to catch up */
		/* perform an extra read cycle to be sure we got it all */
		SPI_Data_Write_Read_Helper_Read( board_id, object_size, tx_object_count, active_rx, &index_rx, rx_buffer );
		delay( 250 /* ms */  );
# endif
#endif

#if defined( SPI_PRINT_DEBUG )
# if defined( __MSDOS__ )
		internal_count++;
		printf( "SPI_Data_Write_Read:  LOOP: internal_count = %u\n", internal_count );
		printf( "  object_size       = %d\n", object_size );
		printf( "  tx_object_count   = %d\n", tx_object_count );
		printf( "  rx_object_count   = %d\n", rx_object_count );
		printf( "  index_tx          = %d\n", index_tx );
		printf( "  index_rx          = %d\n", index_rx );
		printf( "  tx_buffer         = %p\n", tx_buffer );
		printf( "  rx_buffer         = %p\n", rx_buffer );
		delay( 1000 /* ms */ );
# endif
#endif

		/* Read Data ***********************************************************
		 *  This function will play catch up with respect to the transmit side.
		 */
		SPI_Data_Write_Read_Helper_Read( board_id, object_size, tx_object_count, active_rx, &index_rx, rx_buffer );


#if defined( SPI_PRINT_DEBUG )
# if defined( __MSDOS__ )
		if ( index_rx != index_tx )
		{
			error_code = -EC_SPI_BUFFER_MISMATCH;
			printf( "SPI_Data_Write_Read:  MISMATCH: internal_count = %u\n", internal_count );

			#if defined( __MSDOS__ )
			printf( "  object_size       = %d\n", object_size );
			printf( "  tx_object_count   = %d\n", tx_object_count );
			printf( "  rx_object_count   = %d\n", rx_object_count );
			printf( "  index_tx          = %d\n", index_tx );
			printf( "  index_rx          = %d\n", index_rx );
			printf( "  tx_buffer         = %p\n", tx_buffer );
			printf( "  rx_buffer         = %p\n", rx_buffer );
			#else
			printf( "  object_size       = %lu\n", object_size );
			printf( "  tx_object_count   = %lu\n", tx_object_count );
			printf( "  rx_object_count   = %lu\n", rx_object_count );
			printf( "  index_tx          = %lu\n", index_tx );
			printf( "  index_rx          = %lu\n", index_rx );
			printf( "  tx_buffer         = %p\n", tx_buffer );
			printf( "  rx_buffer         = %p\n", rx_buffer );
			#endif
			printf( "  active_tx         = %s\n", active_tx ? "true" : "false" );
			printf( "  active_rx         = %s\n", active_rx ? "true" : "false" );

			goto SPI_Data_Write_Read_Error_Exit;
		}
# endif
#endif

		/* status ***********************************************************/
		SPI_Data_Write_Read_Helper_Commit( board_id, object_size, csb, commit_ready, &commit_valid );

		/* Write Data  ***********************************************************
		 *
		 */
		SPI_Data_Write_Read_Helper_Write( 	board_id,
											object_size,
											tx_object_count,
											csb,
											active_tx,
											commit_valid,
											&commit_ready,
											&index_tx,
											tx_buffer
										);



		/* status ***********************************************************/
		SPI_Data_Write_Read_Helper_Commit( board_id, object_size, csb, commit_ready, &commit_valid );

	}
#if defined( SPI_PRINT_DEBUG )
printf( "  SPI_Data_Write_Read: Post write operations\n" );
#endif

	/* wait for any data to be written to be completed */
	do
	{
		error_code = SUCCESS;
		if ( dataset->quit_application ) error_code = -EC_APP_TERMINATE_CTRLC;
		if ( Character_Get( NULL )     ) error_code = -EC_APP_TERMINATE;
		if ( SUCCESS != error_code     )
		{
			printf( "SPI_Data_Write_Read status ending portion\n" );
			goto SPI_Data_Write_Read_Error_Exit;
		}
		SPI_Data_Write_Read_Helper_Commit( board_id, object_size, csb, commit_ready, &commit_valid );
		SPI_Status_Write_FIFO_Status( board_id, &tx_full, &tx_empty, &tx_bytes_available );
	} while ( false == tx_empty );

#if defined( SPI_PRINT_DEBUG )
printf( "  SPI_Data_Write_Read: Post Read Begin\n" );
#endif

	/* retrieve the remaining read data and don't return until we are done */
	while ( index_rx != index_tx )
	{ //TODO: timeout mechanism????
		error_code = SUCCESS;
		if ( dataset->quit_application ) error_code = -EC_APP_TERMINATE_CTRLC;
		if ( Character_Get( NULL )     ) error_code = -EC_APP_TERMINATE;
		if ( SUCCESS != error_code     )
		{
			printf( "SPI_Data_Write_Read status ending receive loop\n" );
			goto SPI_Data_Write_Read_Error_Exit;
		}
		SPI_Data_Write_Read_Helper_Read( board_id, object_size, tx_object_count, active_rx, &index_rx, rx_buffer );
	}
#if ( 1 == SPI_LOGGING )
	if ( SPI_LOG_ACTIVE == dataset->spi.log_state )
	{
		size_t i;
		fprintf( dataset->spi.log_file, " %3d:", error_code );
		if ( NULL != rx_buffer )
		{
			for ( i = 0; i < (object_size * rx_object_count); i++ )
			{
				fprintf( dataset->spi.log_file, " 0x%02X", ((uint8_t *) rx_buffer)[i] );
			}
		}
		fprintf( dataset->spi.log_file, "\n" );
	}
#endif
	return SUCCESS;

SPI_Data_Write_Read_Error_Exit:


#if defined( SPI_PRINT_XFR_DEBUG )
	printf( "SPI_Data_Write_Read:\n" );
	#if defined( __MSDOS__ )
	printf( "  object_size       = %d\n", object_size );
	printf( "  tx_object_count   = %d\n", tx_object_count );
	printf( "  rx_object_count   = %d\n", rx_object_count );
	printf( "  index_tx          = %d\n", index_tx );
	printf( "  index_rx          = %d\n", index_rx );
	printf( "  tx_buffer         = %p\n", tx_buffer );
	printf( "  rx_buffer         = %p\n", rx_buffer );
	#else
	printf( "  object_size       = %lu\n", object_size );
	printf( "  tx_object_count   = %lu\n", tx_object_count );
	printf( "  rx_object_count   = %lu\n", rx_object_count );
	printf( "  index_tx          = %lu\n", index_tx );
	printf( "  index_rx          = %lu\n", index_rx );
	printf( "  tx_buffer         = %p\n", tx_buffer );
	printf( "  rx_buffer         = %p\n", rx_buffer );
	#endif
	printf( "  active_tx         = %s\n", active_tx ? "true" : "false" );
	printf( "  active_rx         = %s\n", active_rx ? "true" : "false" );

	printf( "  global_internal_tx_byte_count = %u\n", global_internal_tx_byte_count );
	printf( "  global_internal_rx_byte_count = %u\n", global_internal_rx_byte_count );
#endif


#define FUNCTION_NAME__  "SPI_Data_Write_Read"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, error_code );
#endif
#undef FUNCTION_NAME__
#if ( 1 == SPI_LOGGING )
	if ( SPI_LOG_ACTIVE == dataset->spi.log_state )
	{
		fprintf( dataset->spi.log_file, "%d\n", error_code );
	}
#endif
	return error_code;
}
/**********************************************************************************************************//**

\brief

Special case of Write/Read that has a function signature same as fread() or fwrite().

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */

int SPI_Data_Write( int				board_id,
					const void *   	tx_buffer,
					size_t			tx_object_size,  	/**< object size  */
					size_t 			tx_object_count, 	/**< object count */
					FILE *			fd_log      		/**< set to NULL if no file logging */
	  	 	 	  )
{
	int error_code;

#define FUNCTION_NAME__  "SPI_Data_Write"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	error_code = SPI_Data_Write_Read(	board_id,
										tx_object_size,
										tx_object_count,
										tx_buffer,
										0,			/* nothing to receive */
										NULL		/* nothing to receive */
									);
	if ( SUCCESS != error_code ) goto SPI_Data_Write_Error_Exit;

	if ( NULL != fd_log )
	{
		error_code = fwrite( tx_buffer, tx_object_size, tx_object_count, fd_log );
	}
	if ( error_code < 0 ) goto SPI_Data_Write_Error_Exit;

	return SUCCESS;

SPI_Data_Write_Error_Exit:

#define FUNCTION_NAME__  "SPI_Data_Write"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, error_code );
#endif
#undef FUNCTION_NAME__

	return error_code;
}
/**********************************************************************************************************//**

\brief

Special case of Write/Read that has a function signature same as fread() or fwrite().

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int SPI_Data_Read( 	int				board_id,
					const void *   	rx_buffer,
					size_t			rx_object_size,  	/**< object size  */
					size_t 			rx_object_count, 	/**< object count */
					FILE *			fd_log      		/**< set to NULL if no file logging */
	  	  	  	 )
{
	int error_code;

#define FUNCTION_NAME__  "SPI_Data_Read"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	error_code = SPI_Data_Write_Read(	board_id,
										rx_object_size,
										0,		/* nothing to transmit */
										NULL,	/* nothing to transmit */
										rx_object_count,
										rx_buffer
									);
	if ( SUCCESS != error_code )  goto SPI_Data_Read_Error_Exit;;

	if ( NULL != fd_log )
	{
		error_code = fwrite( rx_buffer, rx_object_size, rx_object_count, fd_log );
	}
	if ( error_code < 0 ) goto SPI_Data_Read_Error_Exit;

	return SUCCESS;

SPI_Data_Read_Error_Exit:

#define FUNCTION_NAME__  "SPI_Data_Read"
#if ( IO_TRACE_USE_SPI_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_SPI, board_id, error_code );
#endif
#undef FUNCTION_NAME__

	return error_code;
}




/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  F R A M   F U N C T I O N S  > > > > 							                          */


//#define IDI_FRAM_PRINT_DEBUG 1

/**********************************************************************************************************//**
 * 
 * @brief
 * FRAM Density current in use.
 */
enum { FRAM_DENSITY_BYTES = 8192 };


/**********************************************************************************************************//**
 *
 * @brief
 * FRAM SPI chip select over-ride mechanism.  Temporarily used to set the
 * chip select channel to what we need based on "set framcs".
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int FRAM__Chip_Select_Route_Override( int board_id )
{
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "FRAM__Chip_Select_Route_Override"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__
	/* backup current routing */
	SPI_Configuration_Chip_Select_Route_Get( board_id, &(dataset->fram_spi_cs_route_backup) );
	/* install fram based chip select route (i.e. chan #1) */
	SPI_Configuration_Chip_Select_Route_Set( board_id, dataset->fram_cs_default );
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * FRAM SPI chip select restore mechanism.  Returns the chip select to original SPI configuration settings.
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int FRAM__Chip_Select_Route_Restore( int board_id )
{
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "FRAM__Chip_Select_Route_Restore"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__
	/* restore the backup */
	SPI_Configuration_Chip_Select_Route_Set( board_id, dataset->fram_spi_cs_route_backup );
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * FRAM Write Enable Latch Set command (WREN)
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM__Write_Enable_Latch_Set( int board_id )
{
	int error_code;
	uint8_t tx_buf[1] = { 0x06 }; /* opcode: WREN = 0x06 */

#define FUNCTION_NAME__  "FRAM__Write_Enable_Latch_Set"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	FRAM__Chip_Select_Route_Override( board_id );
	/* chip select to wrap around the command */
	SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_BUFFER );
	error_code = SPI_Data_Write_Read( board_id, 1, 1, tx_buf, 0, NULL );
	FRAM__Chip_Select_Route_Restore( board_id );
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * FRAM Write Latch disable (or clear) command (WRDI).
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM__Write_Disable( int board_id )
{
	int error_code;
	uint8_t tx_buf[1] = { 0x04 }; /* opcode: WRDI = 0x04 */

#define FUNCTION_NAME__  "FRAM__Write_Disable"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	FRAM__Chip_Select_Route_Override( board_id );
	/* chip select to wrap around the command */
	SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_BUFFER );
	error_code = SPI_Data_Write_Read( board_id, 1, 1, tx_buf, 0, NULL );
	FRAM__Chip_Select_Route_Restore( board_id );
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Read the FRAM status register and output the value.
 *
 * @param status A pointer to the destination location of the status data.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM__Read_Status_Register( int board_id, uint8_t * status )
{
	int 	error_code;
	uint8_t tx_buf[2] = { 0x05, 0x00 }; /* opcode: RDSR = 0x05 */
	uint8_t rx_buf[2];

#define FUNCTION_NAME__  "FRAM__Read_Status_Register"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	FRAM__Chip_Select_Route_Override( board_id );
	/* chip select to wrap around the command */
	SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_BUFFER );
	error_code = SPI_Data_Write_Read( board_id, 2, 1, tx_buf, 1, rx_buf );
	FRAM__Chip_Select_Route_Restore( board_id );
	if ( SUCCESS != error_code ) return error_code;
	*status = rx_buf[1];

#if defined( IDI_FRAM_PRINT_DEBUG )
{
	int index;
	printf( "FRAM__Read_Status_Register:" );
	for ( index = 0; index < 2; index++ )
	{
		printf( " 0x%02X", rx_buf[index] );
	}
	printf( "\n" );
}
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Write to the FRAM status register.
 *
 * @param[in]  FRAM status value to be written.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM__Write_Status_Register( int board_id, uint8_t status )
{
	int 	error_code;
	uint8_t tx_buf[2] = { 0x01, 0x00 }; /* opcode: WRSR = 0x01 */
	uint8_t rx_buf[2];

#define FUNCTION_NAME__  "FRAM__Write_Status_Register"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	FRAM__Write_Enable_Latch_Set( board_id );

	tx_buf[1] = status;
	FRAM__Chip_Select_Route_Override( board_id );
	/* chip select to wrap around the command */
	SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_BUFFER );
	error_code = SPI_Data_Write_Read( board_id, 2, 1, tx_buf, 1, rx_buf );
	FRAM__Chip_Select_Route_Restore( board_id );
	if ( SUCCESS != error_code ) return error_code;

	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Reads data from FRAM memory to the output buffer.
 *
 * @param[in] address   Starting FRAM address
 * @param[in] count     Number of bytes to transfer
 * @param[out] buffer   Destination buffer in which to store the data
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM__Memory_Read( int board_id, uint16_t address, size_t count, uint8_t * buffer )
{
	int 			error_code;
//	SPI_CSB_ENUM 	csb_copy;
	uint8_t 		tx_buf[3] = { 0x03, 0x00, 0x00 }; /* opcode: READ = 0x03 */
	//uint8_t rx_buf[3];

#define FUNCTION_NAME__  "FRAM__Memory_Read"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__


#if defined( SPI_PRINT_DEBUG )
	printf( "*** FRAM__Memory_Read:  Entry.\n" );
#endif

	tx_buf[1] = (uint8_t)( address >>   8 );
	tx_buf[2] = (uint8_t)( address & 0xFF ); /* LSB last */
	FRAM__Chip_Select_Route_Override( board_id );
//	/* retain an existing copy of the actual csb value */
//	error_code = SPI_Configuration_Chip_Select_Behavior_Get( &csb_copy );
//	if ( SUCCESS != error_code ) goto FRAM__Memory_Read_Error_Exit;
	/* over-ride and set it to what we wish it to be */
	error_code = SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_SOFTWARE );
	if ( SUCCESS != error_code ) goto FRAM__Memory_Read_Error_Exit;

	SPI_Commit( board_id, 0xFF );

#if defined( SPI_PRINT_DEBUG )
	printf( "*** FRAM__Memory_Read:  Send FRAM Write Command.\n" );
#endif

	error_code = SPI_Data_Write_Read( board_id, 3, sizeof( uint8_t ), tx_buf, 0, NULL );
	if ( SUCCESS != error_code ) goto FRAM__Memory_Read_Error_Exit;

#if defined( SPI_PRINT_DEBUG )
	printf( "*** FRAM__Memory_Read:  count=%u\n", count );
#endif

	error_code = SPI_Data_Write_Read( board_id, sizeof( uint8_t ), 0, NULL, count, buffer );
	if ( SUCCESS != error_code ) goto FRAM__Memory_Read_Error_Exit;

//	while ( SPI_Status_Write_FIFO_Is_Not_Empty() ); /* wait for buffer to empty */
	SPI_Commit( board_id, 0x00 );

//	/* restore the csb */
//	error_code = SPI_Configuration_Chip_Select_Behavior_Set( csb_copy );
//	if ( SUCCESS != error_code ) goto FRAM__Memory_Read_Error_Exit;
	FRAM__Chip_Select_Route_Restore( board_id );
	return SUCCESS;

FRAM__Memory_Read_Error_Exit:
	SPI_Commit( board_id, 0x00 );

#define FUNCTION_NAME__  "FRAM__Memory_Read"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, error_code );
#endif
#undef FUNCTION_NAME__

	FRAM__Chip_Select_Route_Restore( board_id );
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * Writes data from buffer to FRAM memory.
 *
 * @param[in] address   Starting FRAM address
 * @param[in] count     Number of bytes to transfer
 * @param[out] buffer   Source buffer from which data will be transfered to FRAM
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM__Memory_Write( int board_id, uint16_t address, size_t count, uint8_t * buffer )
{
	int 			error_code;
//	SPI_CSB_ENUM 	csb_copy;
	uint8_t 		tx_buf[3] = { 0x02, 0x00, 0x00 }; /* opcode: WRITE = 0x02 */

#define FUNCTION_NAME__  "FRAM__Memory_Write"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	tx_buf[1] = (uint8_t)( address >>   8 );
	tx_buf[2] = (uint8_t)( address & 0xFF ); /* LSB last */

	FRAM__Write_Enable_Latch_Set( board_id );
	FRAM__Chip_Select_Route_Override( board_id );
//	/* retain an existing copy of the actual csb value */
//	error_code = SPI_Configuration_Chip_Select_Behavior_Get( &csb_copy );
//	if ( SUCCESS != error_code ) goto FRAM__Memory_Write_Error_Exit;
	/* over-ride and set it to what we wish it to be */
	error_code = SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_SOFTWARE );
	if ( SUCCESS != error_code ) goto FRAM__Memory_Write_Error_Exit;

	SPI_Commit( board_id, 0xFF );

	error_code = SPI_Data_Write_Read( board_id, 3, sizeof( uint8_t ), tx_buf, 0, NULL );
	if ( SUCCESS != error_code ) goto FRAM__Memory_Write_Error_Exit;

	error_code = SPI_Data_Write_Read( board_id, sizeof( uint8_t ), count, buffer, 0, NULL );
	if ( SUCCESS != error_code ) goto FRAM__Memory_Write_Error_Exit;

//	while ( SPI_Status_Write_FIFO_Is_Not_Empty() ); /* wait for buffer to empty */
	SPI_Commit( board_id, 0x00 );

//	/* restore the csb */
//	error_code = SPI_Configuration_Chip_Select_Behavior_Set( csb_copy );
//	if ( SUCCESS != error_code ) return error_code;
	FRAM__Chip_Select_Route_Restore( board_id );
	return SUCCESS;

FRAM__Memory_Write_Error_Exit:
	SPI_Commit( board_id, 0x00 );

#define FUNCTION_NAME__  "FRAM__Memory_Write"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, error_code );
#endif
#undef FUNCTION_NAME__

	FRAM__Chip_Select_Route_Restore( board_id );
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 *
 * @param[out] id  The 32-bit ID register read from the FRAM.  This appears to be only available
 *                 with Fujistu parts.
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM__Read_ID( int board_id, uint32_t * id )
{
	int 		error_code;
	uint8_t 	tx_buf[5] = { 0x9F, 0x00, 0x00, 0x00, 0x00 }; /* opcode: RDID = 0x9F */
	uint8_t 	rx_buf[5];

#define FUNCTION_NAME__  "FRAM__Read_ID"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	FRAM__Chip_Select_Route_Override( board_id );
	SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_BUFFER );
	error_code = SPI_Data_Write_Read( board_id, 5, 1, tx_buf, 1, rx_buf );
	if ( SUCCESS != error_code ) goto FRAM__Read_ID_Error_Exit;

	{
		uint32_t id_scratch = 0;
		size_t	 index;

		for ( index = 1; index < 5; index++ )
		{ /* assuming MSB first */
			id_scratch = ( id_scratch << 8 ) | ( (uint32_t) rx_buf[index] );
		}
		*id = id_scratch;
	}
#if defined( IDI_FRAM_PRINT_DEBUG )
{
	int index;
	printf( "FRAM__Read_ID:" );
	for ( index = 0; index < 5; index++ )
	{ /*
	 	 0xFF   -- command
	 	 0x04  	-- Fujistu
	 	 0x7F 	-- continuation code
	 	 0x03 	-- Density
	 	 0x02 	-- Product ID
	 */
		printf( " 0x%02X", rx_buf[index] );
	}
	printf( "\n" );
}
#endif
	FRAM__Chip_Select_Route_Restore( board_id );
	return SUCCESS;
FRAM__Read_ID_Error_Exit:

#define FUNCTION_NAME__  "FRAM__Read_ID"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, error_code );
#endif
#undef FUNCTION_NAME__

	FRAM__Chip_Select_Route_Restore( board_id );
	return error_code;
}
/**********************************************************************************************************//**

\brief

This function will be used when creating a memory pool so that as blocks are
allocated one can determine if we have an issue outside of any allocated space
(i.e. overflows and so on).

\param[in] cfg pass in the configuration to be written to hardware.
\return a nonzero if successful, else return zero.
*/
/**********************************************************************************************************//**
 * 
 * @brief
 * Writes a repeating pattern to the entire FRAM memory array.  the pattern is obtained
 * from the buffer.  If the buffer is NULL, then all zeros are written to the FRAM.
 *
 * @param[in] count  Length in bytes of the pattern found within the buffer
 * @param[in] buffer input buffer containing the repeat pattern to be written to FRAM
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM_Set( int board_id, size_t count, uint8_t * buffer )
{
	int			error_code;
	size_t		block_count;
	size_t		block_remainder;
	uint16_t	address;
#if defined( IDI_FRAM_PRINT_DEBUG )
	size_t		index;
#endif
//	uint32_t	id;
	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "FRAM_Set"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

//	error_code = FRAM__Read_ID( &id );
//	if ( SUCCESS != error_code ) return error_code;

	address = 0;
	error_code = SUCCESS;

	if ( count > 1 )
	{ /* */
		block_count     = ((size_t) FRAM_DENSITY_BYTES) / count;
		block_remainder = ((size_t) FRAM_DENSITY_BYTES) - ( block_count * count );
	}
	else
	{ /* only one fill character, so we create a buffer of it to make things a bit faster */
		int			index;
		const int	block_size = FRAM_BLOCK_SIZE;
		/* prefill */
		if ( NULL == buffer )
		{
			for ( index = 0; index < block_size; index++ ) dataset->spi.fram_block[index]= 0;
		}
		else
		{
			for ( index = 0; index < block_size; index++ ) dataset->spi.fram_block[index]= buffer[0];
		}
		block_count     = ((size_t) FRAM_DENSITY_BYTES) / block_size;
		block_remainder = ((size_t) FRAM_DENSITY_BYTES) - ( block_count * block_size );
		buffer          = dataset->spi.fram_block;
		count           = block_size;
	}

#if defined( IDI_FRAM_PRINT_DEBUG )
	index = 0;
# if defined( __MSDOS__ )
   printf( "FRAM_Set: count=%d, block_count=%d, block_remainder=%d\n", count, block_count, block_remainder );
# else
   printf( "FRAM_Set: count=%lu, block_count=%lu, block_remainder=%lu\n", count, block_count, block_remainder );
# endif
#endif

	while ( block_count > 0 )
	{
		error_code = FRAM__Memory_Write( board_id, address, count, buffer );
		if ( SUCCESS != error_code ) goto FRAM_Set_Error_Exit;
		address += (uint16_t) count;
		block_count--;

#if defined( IDI_FRAM_PRINT_DEBUG )
	if ( index < 16 )
	{
# if defined( __MSDOS__ )
		printf( "FRAM_Set: block_count=%d, address=%d\n", block_count, address );
# else
		printf( "FRAM_Set: block_count=%lu, block_remainder=%d\n", block_count, address );
# endif
		index++;
	}
#endif

	}

	if ( block_remainder > 0 )
	{
		error_code = FRAM__Memory_Write( board_id, address, block_remainder, buffer );
		if ( SUCCESS != error_code ) goto FRAM_Set_Error_Exit;
	}


	return SUCCESS;

FRAM_Set_Error_Exit:

#define FUNCTION_NAME__  "FRAM_Set"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, error_code );
#endif
#undef FUNCTION_NAME__

	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM_Report( int board_id, uint16_t address, size_t length, FILE * out )
{
	int			error_code;
	const int	block_size = HEX_DUMP_BYTES_PER_LINE;
	size_t		block_count;
	size_t		block_remainder;
	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "FRAM_Report"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

#if defined( SPI_PRINT_XFR_DEBUG )
	fprintf( out, "FRAM using SPI CS channel %d\n", dataset->fram_cs_default ? 1 : 0 );
#endif

	block_count = ((size_t) length) / block_size;
	block_remainder = ((size_t) length) - ( block_count * block_size );

	while ( block_count > 0 )
	{ /* output a line at a time */
		error_code = FRAM__Memory_Read( board_id, address, block_size, dataset->spi.fram_block );
		if ( SUCCESS != error_code ) goto FRAM_Report_Error_Exit;
		error_code = Hex_Dump_Line( address, block_size, dataset->spi.fram_block, out );
		if ( SUCCESS != error_code ) goto FRAM_Report_Error_Exit;
		address += block_size;
		block_count--;
	}

	if ( block_remainder > 0 )
	{ /* output any remaining portion */
		error_code = FRAM__Memory_Read( board_id, address, block_remainder, dataset->spi.fram_block );
		if ( SUCCESS != error_code ) goto FRAM_Report_Error_Exit;
		error_code = Hex_Dump_Line( address, block_remainder, dataset->spi.fram_block, out );
		if ( SUCCESS != error_code ) goto FRAM_Report_Error_Exit;
	}

	return SUCCESS;

FRAM_Report_Error_Exit:

#define FUNCTION_NAME__  "FRAM_Report"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, error_code );
#endif
#undef FUNCTION_NAME__

	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM_Memory_To_File( int board_id, uint16_t address, size_t length, FILE * binary )
{
	int			error_code;
	size_t		block_count;
	size_t		block_remainder;
	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "FRAM_Memory_To_File"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	block_count = length / ((size_t) FRAM_BLOCK_SIZE);
	block_remainder = length - ( block_count * ((size_t) FRAM_BLOCK_SIZE) );

	while ( block_count > 0 )
	{
		error_code = FRAM__Memory_Read( board_id, address, ((size_t) FRAM_BLOCK_SIZE), dataset->spi.fram_block );
		if ( SUCCESS != error_code ) goto FRAM_Memory_To_File_Error_Exit;
		fwrite( dataset->spi.fram_block, 1, ((size_t) FRAM_BLOCK_SIZE), binary );
		address += FRAM_BLOCK_SIZE;
		block_count--;
	}

	if ( block_remainder > 0 )
	{
		error_code = FRAM__Memory_Read( board_id, address, block_remainder, dataset->spi.fram_block );
		if ( SUCCESS != error_code ) goto FRAM_Memory_To_File_Error_Exit;
		fwrite( dataset->spi.fram_block, 1, block_remainder, binary );
	}
	return SUCCESS;

FRAM_Memory_To_File_Error_Exit:

#define FUNCTION_NAME__  "FRAM_Memory_To_File"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, error_code );
#endif
#undef FUNCTION_NAME__

	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int FRAM_File_To_Memory( int board_id, uint16_t address, size_t length, FILE * binary )
{
	int		error_code;
	size_t	count_read;
	size_t  count_total;
	size_t  count_actual;
	struct board_dataset *  dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "FRAM_File_To_Memory"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	count_total = 0;
	count_read  = FRAM_BLOCK_SIZE;
	if ( 0 == length )
	{
		do
		{
			count_actual = fread( dataset->spi.fram_block, 1, count_read, binary );
			error_code   = FRAM__Memory_Write( board_id, address, count_read, dataset->spi.fram_block );
			if ( SUCCESS != error_code ) goto FRAM_File_To_Memory_Error_Exit;
			count_total += count_actual;
			if ( count_actual != count_read ) count_read = 0; /* must be at end of file */
		} while ( count_read > 0 );
	}
	else
	{
		if ( length < count_read ) count_read = length;
		do
		{
			count_actual = fread( dataset->spi.fram_block, 1, count_read, binary );
			error_code   = FRAM__Memory_Write( board_id, address, count_read, dataset->spi.fram_block );
			if ( SUCCESS != error_code ) goto FRAM_File_To_Memory_Error_Exit;
			count_total += count_actual;
			length      -= count_actual;
			if ( count_actual != count_read ) count_read = 0; /* must be at end of file */
			if ( length < count_read ) count_read = length;
		} while ( count_read > 0 );
	}
	return SUCCESS;

FRAM_File_To_Memory_Error_Exit:

#define FUNCTION_NAME__  "FRAM_File_To_Memory"
#if ( IO_TRACE_USE_FRAM_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_FRAM, board_id, error_code );
#endif
#undef FUNCTION_NAME__

	return error_code;
}


/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/


/**********************************************************************************************************//**
 * 
 * @brief
 * Data structure used to decode command line operation.
 */
struct command_line
{
	struct command_line * link;						/* link to next lower level data structure		  */
	int ( * cmd_fnc )( int argc, char * argv[] );   /* function to call to process arguments further  */
	char *  name;                                   /* name of argument/command word                  */
	char *  help;                                   /* very brief help string associated with command */
};

struct command_line_board   /* temporary until conversion complete */
{
	struct command_line_board * link;				/* link to next lower level data structure		  */
	int ( * cmd_fnc )( int board_id, int argc, char * argv[] );   /* function to call to process arguments further  */
	char *  name;                                   /* name of argument/command word                  */
	char *  help;                                   /* very brief help string associated with command */
};


/**********************************************************************************************************/


/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  S P I  C O M M A N D   F U N C T I O N S  > > > > 				                          */


/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_ID( int board_id, int argc, char * argv[] )
{ /*  idi spi id */
	uint16_t 	id;
	(void) 		argc;
	(void) 		argv;

	SPI_ID_Get( board_id, &id );
	printf( "SPI ID: 0x%04X\n", id );
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Config_Get( int board_id, int argc, char * argv[] )
{ /*  idi spi cfg */
	int 	error_code;
	struct 	spi_cfg cfg;
	(void) 	argc;
	(void) 	argv;

	error_code = SPI_Configuration_Get( board_id, &cfg );
	if ( SUCCESS != error_code ) return error_code;

	error_code = SPI_Report_Configuration_Text( &cfg, stdout );
	if ( SUCCESS != error_code ) return error_code;

	printf( "\n" );
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Config_Clock_Hz( int board_id, int argc, char * argv[] )
{ /*  idi spi clk [<freq_hz>] */
	int 	 error_code;
	double 	 clock_request_hz;
	double   clock_actual_hz;
	double   error;
	uint16_t hci;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( board_id, &cfg );
	if ( SUCCESS != error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI CLK: %f hz\n", cfg.clock_hz );
	}
	else
	{ /* write */
		clock_request_hz = atof( argv[0] );
		error_code = SPI_Calculate_Clock( clock_request_hz, &clock_actual_hz, &error, &hci );
		if ( SUCCESS != error_code ) return error_code;
		//cfg.half_clock_interval = hci;
		/* commit configuration to hardware */
		cfg.clock_hz = clock_actual_hz;
		error_code   = SPI_Configuration_Set( board_id, &cfg );
		if ( SUCCESS != error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Config_End_Cycle_Delay_Sec( int board_id, int argc, char * argv[] )
{ /*  idi spi ecd [<time sec>] */
	int 	 error_code;
	double 	 request_sec;
	double   actual_sec;
	double   error;
	uint8_t  ecd;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( board_id, &cfg );
	if ( SUCCESS != error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI ECD: %g sec\n", ( cfg.end_delay_ns * 1.0e-9 ) );
	}
	else
	{ /* write */
		request_sec = atof( argv[0] );
		error_code = SPI_Calculate_End_Cycle_Delay( SPI_Calculate_Half_Clock_Interval_Sec( cfg.half_clock_interval ),
													request_sec,
													&actual_sec,
													&error,
													&ecd
												  );
		if ( SUCCESS != error_code ) return error_code;
		cfg.end_delay_ns = actual_sec * 1.0e9;
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( board_id, &cfg );
		if ( SUCCESS != error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**

\brief

SPI Wiki definition
Note:  FE = Falling Edge, RE = Rising Edge, IMSSA = Immediate after Slave Select Active
			     CLOCK   CLOCK
   		MODE    POLARITY PHASE	  PROPAGATE   CAPTURE	FIRST BIT		TARGET
	 	 0 	     0 	     0			FE			RE      IMSSA			 FRAM
	 	 1 	     0 	     1			RE			FE		RE				ADS1259
	 	 2 	     1 	     0			RE			FE		IMSSA
	 	 3 	     1 	     1			FE			RE		FE				 FRAM

SPI Module 0x8013
			     CLOCK   CLOCK
   		MODE    POLARITY PHASE	  PROPAGATE   CAPTURE	FIRST BIT		PROP  			CAP   		SHIFT  			NOTE

	 	 0 	     0 	     0			FE			RE    	IMSSA			FE			1st-RE			OK, last shifted using CMPLT
	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	1st-skip

	 	 1 	     0 	     1			RE			FE		IMSSA   		RE			1st-FE			OK, last shifted using CMPLT
	 	 																1st-skip

	 	 2 	     1 	     0			RE			FE		IMSSA

	 	 3 	     1 	     1			FE			RE		IMSSA

*/
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Config_Mode( int board_id, int argc, char * argv[] )
{ /*  idi spi mode [0/1/2/3] */
	int 	 error_code;
	int		 mode;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( board_id, &cfg );
	if ( SUCCESS != error_code ) return error_code;

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

#if(0)
		uint16_t id;
		SPI_ID_Get( board_id, &id );
		if ( 0x8013 == id )
		{

		}
		else
		{

		}
#endif

		mode = (int) strtol( argv[0], NULL, 0 );
		switch ( mode )
		{
			case 0: cfg.sclk_polarity = false;	cfg.sclk_phase = false;		break;
			case 1: cfg.sclk_polarity = false;	cfg.sclk_phase = true;		break;
			case 2: cfg.sclk_polarity = true;	cfg.sclk_phase = false;		break;
			case 3: cfg.sclk_polarity = true;	cfg.sclk_phase = true;		break;
		}
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( ID_IDI48, &cfg );
		if ( SUCCESS != error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Config_SDI_Polarity( int board_id, int argc, char * argv[] )
{ /*  idi spi sdi [<true/1/false/0>] */
	int 	 error_code;
//	BOOL	 value;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( board_id, &cfg );
	if ( SUCCESS != error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI SDI POLARITY: %s\n", cfg.sdi_polarity ? "true" : "false" );
	}
	else
	{ /* write */
		cfg.sdi_polarity = String_To_Bool( argv[0] );
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( board_id, &cfg );
		if ( SUCCESS != error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Config_SDO_Polarity( int board_id, int argc, char * argv[] )
{ /*  idi spi sdo [<true/1/false/0>] */
	int 	 error_code;
//	BOOL	 value;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( board_id, &cfg );
	if ( SUCCESS != error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI SDO POLARITY: %s\n", cfg.sdo_polarity ? "true" : "false" );
	}
	else
	{ /* write */
		cfg.sdo_polarity = String_To_Bool( argv[0] );
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( board_id, &cfg );
		if ( SUCCESS != error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Config_SDIO_Wrap( int board_id, int argc, char * argv[] )
{ /*  idi spi wrap [<true/1/false/0>] */
	int 	 error_code;
//	BOOL	 value;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( board_id, &cfg );
	if ( SUCCESS != error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI wrap: %s\n", cfg.sdio_wrap ? "true" : "false" );
	}
	else
	{ /* write */
		cfg.sdio_wrap = String_To_Bool( argv[0] );
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( board_id, &cfg );
		if ( SUCCESS != error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Config_Chip_Select_Route( int board_id, int argc, char * argv[] )
{ /*  idi spi sdo [<true/1/false/0>] */
	int 	 error_code;
//	BOOL	 value;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( board_id, &cfg );
	if ( SUCCESS != error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI CS ROUTE: %s\n", cfg.chip_select_route_ch1 ? "1" : "0" );
	}
	else
	{ /* write */
		cfg.chip_select_route_ch1 = String_To_Bool( argv[0] );
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( board_id, &cfg );
		if ( SUCCESS != error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Config_Chip_Select_Behavior( int board_id, int argc, char * argv[] )
{ /*  idi spi mode [0/1/2/3/software/buffer/uint8_t/uint16_t] */
	int 	 error_code;
//	int		 csb;
	struct 	 spi_cfg cfg;

	/* pull current configuration from the hardware -- allows for warm restore so to speak */
	error_code = SPI_Configuration_Get( board_id, &cfg );
	if ( SUCCESS != error_code ) return error_code;

	if ( argc < 1 )
	{ /* read */
		printf( "SPI CSB: ");
		switch ( cfg.chip_select_behavior )
		{
			case CSB_SOFTWARE:	printf( "software" 	);	break;
			case CSB_BUFFER:	printf( "buffer"   	);	break;
			case CSB_uint8_t:	printf( "uint8_t"   );	break;
			case CSB_uint16_t:	printf( "uint16_t"  );	break;
			default:            printf( "undefined"	);	break;
		}
		printf( "\n" );
	}
	else
	{ /* write */
		if      ( 0 == strcmpi( "software",   argv[0] ) ) cfg.chip_select_behavior = 0;
		else if ( 0 == strcmpi( "buffer",     argv[0] ) ) cfg.chip_select_behavior = 1;
		else if ( 0 == strcmpi( "uint8_t",    argv[0] ) ) cfg.chip_select_behavior = 2;
		else if ( 0 == strcmpi( "uint16_t",   argv[0] ) ) cfg.chip_select_behavior = 3;
		else
		{
			cfg.chip_select_behavior = (SPI_CSB_ENUM) strtol( argv[0], NULL, 0 );
			switch ( cfg.chip_select_behavior )
			{
				case CSB_SOFTWARE:
				case CSB_BUFFER:
				case CSB_uint8_t:
				case CSB_uint16_t:
					break;
				default:
					error_code = -EC_SPI_CSB_OUT_OF_RANGE;
					break;
			}
		}
		 if ( SUCCESS != error_code ) return error_code;
		/* commit configuration to hardware */
		error_code = SPI_Configuration_Set( board_id, &cfg );
		if ( SUCCESS != error_code ) return error_code;
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Status( int board_id, int argc, char * argv[] )
{ /*  idi spi status [rx] [tx] ...  */
	int 	 			error_code;
	int					index;
	struct spi_status 	status;

	if ( argc < 1 )
	{
		error_code = SPI_Status_Read( board_id, &status );
		if ( SUCCESS != error_code ) return error_code;
		SPI_Report_Status_Text( &status, stdout );
	}
	else
	{
		for ( index = 0; index < argc; index++ )
		{
			if ( 0 == strcmpi( "rx", argv[index] ) )
			{
				error_code = SPI_Status_Read( board_id, &status );
				if ( SUCCESS != error_code ) return error_code;
				SPI_Report_Status_Text( &status, stdout );
			}
			else if ( 0 == strcmpi( "tx", argv[index] ) )
			{
				error_code = SPI_Status_Write( board_id, &status );
				if ( SUCCESS != error_code ) return error_code;
				SPI_Report_Status_Text( &status, stdout );
			}
		}
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Data_Interpreter( 	int 		arg_start,  /* [in] starting argument */
										int 		argc,
										char * 		argv[],
										size_t		tx_size,	/* [in] max size of out buffer in bytes */
										size_t *	tx_count,   /* [out] actual size  */
										uint8_t * 	tx_buffer   /* [out] output buffer */
									)
{
	size_t 	arg_qty;
	size_t	count;
	int 	index_arg;
	int		index_byte;
	uint8_t data_temp;
	char * 	endptr;
	char *  sp;

	arg_qty = (size_t)( argc - arg_start );

	sp         = NULL;
	index_arg  = arg_start;
	index_byte = 0;
	count      = arg_qty;
	while ( ( count != 0 ) && ( index_byte < tx_size ) )
	{
		data_temp = (uint8_t) strtol( argv[index_arg], &endptr, 0 );
		if ( endptr == argv[index_arg] )
		{
			sp = &(argv[index_arg][0]);
			while ( ( '\0' != *sp ) && ( index_byte < tx_size ) )
			{
				tx_buffer[index_byte] = (uint8_t)( *sp );
				sp++;	index_byte++;
			}
			if ( '\0' == *sp ) sp = NULL;
		}
		else
		{
			tx_buffer[index_byte] = data_temp;
			index_byte++;
			sp = NULL;
		}
		count--;	index_arg++;
	}

	if ( count > 0 )
	{
#if defined( __MSDOS__ )
		printf( "Warning: ignored %d arguments", count );
#else
		printf( "Warning: ignored %lu arguments", count );
#endif
		if ( NULL != sp )
		{
			printf( ", and portion of string argument" );
		}
		printf( "\n" );
	}

	*tx_count = index_byte;
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 *
 * TODO: add a command to allow different object sizes to be transmitted.  That would better demonstrate
 * the buffer mode in the SPI hardware.  Currently, if csb=buffer then only a byte per spi fifo buffer
 * will be transmitted.  All bytes will be transmitted, each byte will be chip-select wrapped and overall
 * SPI transations will be slow due to software and bus timing.
 */
static int CMD__SPI_Data( int board_id, int argc, char * argv[] )
{ /*  idi spi data [byte] [character] ... */
	int 	 			error_code;
	size_t				index;
	size_t				transfer_count; /* */
	size_t	  			lines;
	uint8_t *			bp; 			/* buffer pointer */

	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;



	if ( argc < 1 ) return -EC_PARAMETER;

	CMD__SPI_Data_Interpreter( 		0,  						/* [in] starting argument */
									argc,
									argv,
									SPI_BLOCK_SIZE,				/* [in] max size of out buffer in bytes */
									&transfer_count,   			/* [out] actual size in bytes  			*/
									dataset->spi.tx_buffer 	/* [out] output buffer 					*/
								 );

	error_code = SPI_Data_Write_Read( 	board_id,
										sizeof( uint8_t ),
										transfer_count,
										dataset->spi.tx_buffer,
										transfer_count,
										dataset->spi.rx_buffer
									);
	if ( SUCCESS != error_code ) return error_code;

	lines = transfer_count / HEX_DUMP_BYTES_PER_LINE;
	//if ( 0 == ( transfer_count - lines * HEX_DUMP_BYTES_PER_LINE ) ) lines = lines - 1;
	for ( index = 0; index <= lines; index++ )
	{
		bp = &(dataset->spi.rx_buffer[index * HEX_DUMP_BYTES_PER_LINE]);
		if ( transfer_count < HEX_DUMP_BYTES_PER_LINE )
		{
			Hex_Dump_Line( 0, transfer_count, bp, stdout );
		}
		else
		{
			Hex_Dump_Line( 0, HEX_DUMP_BYTES_PER_LINE, bp, stdout );
			transfer_count = transfer_count - HEX_DUMP_BYTES_PER_LINE;
		}
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_FIFO( int board_id, int argc, char * argv[] )
{ /*  idi spi data [byte] [character] ... */
	int 	 			error_code;
	int					index;
	size_t				count;
	SPI_CSB_ENUM		csb;
	int					read_count; /* */
//	uint8_t				data_temp;
	uint8_t				tx_buffer[SPI_FIFO_SIZE];
	uint8_t				rx_buffer[SPI_FIFO_SIZE];

	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	read_count = 0;
	if ( argc < 1 )
	{
		read_count = SPI_FIFO_SIZE;
	}
	else
	{
		if      ( 0 == strcmpi( "rx", argv[0] ) )
		{
			if ( argc > 1)
			{
				if ( 0 == strcmpi( "all", argv[1] ) ) read_count = SPI_FIFO_SIZE;
				else read_count = (int) strtol( argv[1], NULL, 0 );
			}
		}
		else if ( 0 == strcmpi( "tx", argv[0] ) )
		{
			CMD__SPI_Data_Interpreter( 		1,  				/* [in] starting argument */
											argc,
											argv,
											SPI_FIFO_SIZE,		/* [in] max size of out buffer */
											&count,   			/* [out] actual size  */
											tx_buffer   		/* [out] output buffer */
										 );

			error_code = SPI_FIFO_Write( board_id, (void *) tx_buffer, sizeof( uint8_t ), count, NULL );
			if ( error_code < 0 ) return error_code;

			printf( "OK, wrote %d objects or %d bytes\n", error_code, error_code * ((int) sizeof( uint8_t )) );
			return SUCCESS;
		}
		else if ( 0 == strcmpi( "commit", argv[0] ) )
		{
			if ( argc > 1 )
			{
				if ( String_To_Bool( argv[1] ) ) SPI_Commit( board_id, 0xFF );
				else                             SPI_Commit( board_id, 0x00 );

				printf( "OK\n" );
				return SUCCESS;
			}
			else
			{
				uint8_t commit_status;
				SPI_Commit_Get( board_id, &commit_status );
				printf( "commit status = %s\n", ( 0 == commit_status ) ? "false" : "true" );
				return SUCCESS;
			}
			//unreachable - return error_code;
		}
	}


	/* wait for transmit data to empty out */
	/* obtain the chip select behavior */
	error_code = SPI_Configuration_Chip_Select_Behavior_Get( board_id, &csb );
	if ( SUCCESS != error_code ) return error_code;

	if ( SPI_Status_Write_FIFO_Is_Not_Empty( board_id ) )
	{ /* go ahead and commit it */
		if ( CSB_SOFTWARE != csb ) SPI_Commit( board_id, 0xFF );
	}
	//TODO: need a time out of some sort here and then return an error code.
	while ( SPI_Status_Write_FIFO_Is_Not_Empty( board_id ) )
	{ /* wait */
		error_code = SUCCESS;
		if ( dataset->quit_application ) error_code = -EC_APP_TERMINATE_CTRLC;
		if ( Character_Get( NULL )     ) error_code = -EC_APP_TERMINATE;
		if ( SUCCESS != error_code     )
		{
			printf( "IDI_CMD__SPI_FIFO: waiting for tx empty\n" );
			return -EC_APP_TERMINATE;
		}
	}

	if ( read_count > 0 )
	{
		int 	  lines;
		uint8_t * bp; /* buffer pointer */

		if ( SPI_Status_Read_FIFO_Is_Not_Empty( board_id ) )
		{
			error_code = SPI_FIFO_Read( board_id, (void *) rx_buffer, sizeof( uint8_t ), read_count, NULL );
			if ( error_code < 0 ) return error_code;
			/* error code is number of object that have been read */
			read_count = error_code * sizeof( uint8_t );
			lines = read_count / HEX_DUMP_BYTES_PER_LINE;
			//if ( 0 == ( read_count - lines * HEX_DUMP_BYTES_PER_LINE ) ) lines = lines - 1;
			for ( index = 0; index <= lines; index++ )
			{
				bp = &(rx_buffer[index * HEX_DUMP_BYTES_PER_LINE]);
				if ( read_count < HEX_DUMP_BYTES_PER_LINE )
				{
					Hex_Dump_Line( 0, read_count, bp, stdout );
				}
				else
				{
					Hex_Dump_Line( 0, HEX_DUMP_BYTES_PER_LINE, bp, stdout );
					read_count = read_count - HEX_DUMP_BYTES_PER_LINE;
				}
			}
		}
		else
		{
			printf( "FIFO Empty\n" );
		}
	}
	return SUCCESS;




}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__SPI_Commit( int board_id, int argc, char * argv[] )
{ /*  idi spi commit [<true/1/false/0>] */
	int		 error_code;
	uint8_t  commit;

	if      ( argc < 1 )
	{
		uint8_t commit_status;
		SPI_Commit_Get( board_id, &commit_status );
		printf( "commit status = %s\n", ( 0 == commit_status ) ? "false" : "true" );
	}
	else if ( String_To_Bool( argv[0] ) ) commit = 0x01;
	else                                  commit = 0x00;

	error_code = SPI_Commit( board_id, commit );
	if ( SUCCESS != error_code ) return error_code;

	printf( "OK\n" );
	return SUCCESS;
}

/**********************************************************************************************************//**
 * 
 * @brief
 */
static struct command_line_board  cmd_spi[] =
{
	{	NULL,	CMD__SPI_ID, 							"id",		"wishbone id: params: none"								},
	{	NULL,	CMD__SPI_Config_Get, 		  			"cfg",		"config dump: params: none"								},
	{   NULL,	CMD__SPI_Config_Clock_Hz, 				"clk",		"clk:         params: [<clock freq in hertz>]"			},
	{   NULL,	CMD__SPI_Config_End_Cycle_Delay_Sec,	"ecd",		"end delay:   params: [<time in seconds>]"				},
	{   NULL,	CMD__SPI_Config_Mode,					"mode",		"mode:        params: [<0/1/2/3>]"						},
	{   NULL,	CMD__SPI_Config_SDI_Polarity,			"sdi",		"sdi pol:     params: [<true/1/false/0>]"				},
	{   NULL,	CMD__SPI_Config_SDO_Polarity,			"sdo",		"sdo pol:     params: [<true/1/false/0>]"				},
	{   NULL,	CMD__SPI_Config_SDIO_Wrap,				"wrap",		"sdo-->sdi:   params: [<true/1/false/0>]"				},
	{   NULL,	CMD__SPI_Config_Chip_Select_Behavior,	"csb",		"chip select behavior: params: [0/1/2/3/software/buffer/uint8_t/uint16_t]"	},
	{   NULL,	CMD__SPI_Config_Chip_Select_Route,		"csr",		"chip select routing: params: [<true/1/false/0>]"	},
	{   NULL,	CMD__SPI_Status,						"status",	"status of both TX and RX buffers"						},
	{   NULL,	CMD__SPI_Data,							"data",		"read/write:  params: <data list: bytes/characters/strings>" },
	{	NULL,	CMD__SPI_FIFO,							"fifo",		"fifo r/w:    params: <data list: bytes/characters/strings>" },
	{   NULL,	CMD__SPI_Commit,						"commit",	"causes spi transactions to start"						},
	{	NULL,	NULL,									NULL,		NULL													}
};
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_SPI( int board_id, int argc, char* argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;

    error_code = -EC_SYNTAX;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	index = 0;
	while ( NULL != cmd_spi[index].cmd_fnc )
	{
		if ( 0 == strcmpi( cmd_spi[index].name, argv[0] )  )
		{
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			if ( 0 == argc_new ) argv_new = NULL;

			{ /* set trace active if schedule to start upon SPI transactions */
				struct board_dataset *  dataset;
				dataset = ( struct board_dataset * )  board_definition[board_id].dataset;
				if ( IO_TRACE_START_SPI == dataset->trace.start )
				{
					dataset->trace.active = true;
				}
			}

			error_code = (* cmd_spi[index].cmd_fnc )( board_id, argc_new, argv_new );
			break;
		}
		index++;
	}
	return error_code;
}
/**********************************************************************************************************/



/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  A N A L O G   S T I C K   C O M M A N D   F U N C T I O N S  > > > > 				                          */

//static BOOL as_checksum = false;

const double ADS1259_CLOCK_MHZ       = 7.3728e6;
const double ADS1259_CLOCK_TOLERANCE = 0.02;  /* +/- 2% */

const int32_t ADS1259_OFC_MAX =  8388607L; /* 0x007FFFFF   */
const int32_t ADS1259_OFC_MIN = -8388607L; /* 0xFF800001   */

const int32_t ADS1259_FSC_MAX = 8388608L;  /* 0x00800000   => x2 */
const int32_t ADS1259_FSC_MIN = 0L;        /* 0x00000000   => x0 */



/**********************************************************************************************************//**
 * @brief
 */
enum
{   AS_ADS1259_CONFIG0		 = 0,
	AS_ADS1259_CONFIG1		 = 1,
	AS_ADS1259_CONFIG2		 = 2,
	AS_ADS1259_OFC0			 = 3,
	AS_ADS1259_OFC1			 = 4,
	AS_ADS1259_OFC2			 = 5,
	AS_ADS1259_FSC0			 = 6,
	AS_ADS1259_FSC1			 = 7,
	AS_ADS1259_FSC2			 = 8,
	AS_ADS1259_REG_QTY  	 = 9
};
/**********************************************************************************************************//**
 * @brief
 */
static char * as_register_name[] =
{
	"config0",  /* address = 0 */
	"config1",
	"config2",
	"ofc0",
	"ofc1",
	"ofc2",
	"fsc0",
	"fsc1",
	"fsc2",		/* address = 8 */
	NULL
};
/**********************************************************************************************************//**
 * @brief
 */
struct bit_string_info
{
	size_t 	register_offset;
	size_t	bit_offset;
	size_t	bit_width;
	char *  name;
};
/**********************************************************************************************************//**
 * @brief
 */
static struct bit_string_info as_bit_string[] =
{
		{ AS_ADS1259_CONFIG0, 		0,		1,	 	"spi"		},
		{ AS_ADS1259_CONFIG0,		2,		1,		"rbias"		},
		{ AS_ADS1259_CONFIG0,		4,		2,		"id"		},
		{ AS_ADS1259_CONFIG1,		0,		3,		"delay"		},
		{ AS_ADS1259_CONFIG1,		3,		1,		"extref"	},
		{ AS_ADS1259_CONFIG1,		4,		1,		"sinc2"		},
		{ AS_ADS1259_CONFIG1,		6,		1,		"chksum"	},
		{ AS_ADS1259_CONFIG1,		7,		1,		"flag"		},
		{ AS_ADS1259_CONFIG2,		0,		3,		"dr"		},
		{ AS_ADS1259_CONFIG2,		4,		1,		"pulse"		},
		{ AS_ADS1259_CONFIG2,		5,		1,		"syncout"	},
		{ AS_ADS1259_CONFIG2,		6,		1,		"extclk"	},
		{ AS_ADS1259_CONFIG2,		7,		1,		"drdy_b"	},
		{ AS_ADS1259_REG_QTY, 		0,		0,		NULL		}
};
/**********************************************************************************************************//**
 * @brief
 */
static int ADS1259_Calibration_Range_Test( int32_t * value, size_t address, size_t clamp_and_error_skip )
{
	int32_t		scratch;
	int 		error_code = SUCCESS;

	scratch = *value;
	if ( AS_ADS1259_OFC0 == address )
	{ /* offset */
		if ( scratch > ADS1259_OFC_MAX )
		{
			error_code = -EC_RANGE;
			scratch = ADS1259_OFC_MAX;
		}
		if ( scratch < ADS1259_OFC_MIN )
		{
			error_code = -EC_RANGE;
			scratch = ADS1259_OFC_MIN;
		}
	}
	else
	{ /* gain */
		if ( scratch > ADS1259_FSC_MAX )
		{
			error_code = -EC_RANGE;
			scratch = ADS1259_FSC_MAX;
		}
		if ( scratch < ADS1259_FSC_MIN )
		{
			error_code = -EC_RANGE;
			scratch = ADS1259_FSC_MIN;
		}
	}

	if ( 0 != clamp_and_error_skip )
	{
		*value     = scratch;
		error_code = SUCCESS;
	}
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
int AS_Register_Name_To_Offset( char * name )
{
	size_t index;

	index = 0;
	while ( NULL != as_register_name[index] )
	{
		if ( 0 == strcmpi( as_register_name[index], name ) )
		{
			return index;
		}
		index++;
	}
	return -EC_NOT_FOUND;
}
/**********************************************************************************************************//**
 * @brief
 */
int Bit_String_Report( struct bit_string_info * table, size_t address, uint8_t value, FILE * out )
{
	size_t	index;
	BOOL 	not_done;
	BOOL	initial;
	uint8_t	bit_string_value;

	initial  = true;
	not_done = true;
	index    = 0;
	while ( not_done )
	{
		if ( NULL == table[index].name )
		{
			not_done = false;
		}
		else if ( address == table[index].register_offset )
		{
			size_t  i;
			uint8_t	mask = 0;
			for ( i = 0; i < table[index].bit_width; i++ ) mask = ( mask << 1 ) | 0x01;
			bit_string_value = value >> table[index].bit_offset;
			bit_string_value = bit_string_value & mask;
			if ( initial )
			{
				fprintf( out, ": " );	initial = false;
			}
			fprintf( out, " %s=%d", table[index].name, (int) bit_string_value );
		}
		index++;
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
int Bit_String_Index_By_Name( struct bit_string_info * table, char * name )
{
	int index;
	index = 0;
	while ( NULL != table[index].name )
	{
		if ( 0 == strcmpi( table[index].name, name )  ) return index;
		index++;
	}
	return -EC_NOT_FOUND;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Analog Stick SPI chip select over-ride mechanism.  Temporarily used to set the
 * chip select channel to what we need based on "set framcs".
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int AS_Chip_Select_Route_Override( int board_id )
{
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "AS_Chip_Select_Route_Override"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	SPI_Configuration_Chip_Select_Route_Get( board_id, &(dataset->as_spi_cs_route_backup) );
	SPI_Configuration_Chip_Select_Route_Set( board_id, dataset->as_cs_default );
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Analog Stick SPI chip select restore mechanism.  Returns the chip select to original SPI configuration settings.
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int AS_Chip_Select_Route_Restore( int board_id )
{
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "AS_Chip_Select_Route_Restore"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	SPI_Configuration_Chip_Select_Route_Set( board_id, dataset->as_spi_cs_route_backup );
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
int AS_Opcode_Write( int board_id, uint8_t * tx_buf, size_t count )
{
	int error_code;
	struct board_dataset *  dataset;

	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	/* chip select wraps around buffer */

#define FUNCTION_NAME__  "AS_Opcode_Write"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__
#if ( 1 == SPI_LOGGING )
	if ( SPI_LOG_ACTIVE == dataset->spi.log_state )
	{
		size_t index;
		fprintf( dataset->spi.log_file, "AS_Opcode_Write: " );
		for ( index = 0; index < count; index++ ) fprintf( dataset->spi.log_file, " 0x%02X", tx_buf[index] );
		fprintf( dataset->spi.log_file, "\n" );
	}
#endif
	AS_Chip_Select_Route_Override( board_id );
	if ( CSB_SOFTWARE == dataset->as_spi_chip_select_behavior )
	{
		SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_SOFTWARE );
		SPI_Commit( board_id, 0xFF );
	}
	else
	{
		SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_BUFFER );
	}
	error_code = SPI_Data_Write_Read( 	board_id,	/* board type		*/
										1,			/* tx object size, always a byte in size 	*/
										count,		/* tx object count 	*/
										tx_buf, 	/* transmit buffer 	*/
										0, 			/* rx object count	*/
										NULL 		/* receive buffer 	*/
							  	    );
	if ( CSB_SOFTWARE == dataset->as_spi_chip_select_behavior )
	{
		SPI_Commit( board_id, 0x00 );
	}
	AS_Chip_Select_Route_Restore( board_id );
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Wakeup( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x03 };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Wakeup"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Sleep( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x05 };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Sleep"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Reset( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x07 };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Reset"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Start( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x09 };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Start"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
#if(0)
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Bogus( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x20 };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Bogus"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
#endif
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Stop( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x0B };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Stop"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Rdatac( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x10 };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Rdatac"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Sdatac( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x11 };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Sdatac"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
/**********************************************************************************************************//**
 * @brief
 */
static int AS_Registers_Read( int board_id, size_t address, size_t count, uint8_t * tx_buf, uint8_t * rx_buf )
{
	size_t					object_count;
	int						error_code;
	struct board_dataset *  dataset;

	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;
#define FUNCTION_NAME__  "AS_Registers_Read"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	/* command validity test */
	if ( ( address + count ) > AS_ADS1259_REGISTER_QTY ) return -EC_REGISTER_ADDRESS_EXCEEDED;
	/* op-code fix up */
	tx_buf[0] = 0x20 + ( (uint8_t) ( 0x0F & address ) );
	if ( 0 == count )
	{
		tx_buf[1]    = 0;
		object_count = 2;
		count        = 1;
	}
	else
	{
		tx_buf[1] = ( (uint8_t) ( 0x0F & count ) ) - 1;
		object_count = ( 0x0F & count ) + 2;
	}

	if ( true == dataset->set__suppress_io_activity )
	{
		size_t index;
		for ( index = 0; index < count; index++ )
		{
			rx_buf[index + 2] = dataset->ads1259.reg[address + index];
		}
		return SUCCESS;
	}

	/* chip select wraps around buffer */
	AS_Chip_Select_Route_Override( board_id );
	if ( CSB_SOFTWARE == dataset->as_spi_chip_select_behavior )
	{
		SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_SOFTWARE );
		SPI_Commit( board_id, 0xFF );
	}
	else
	{
		SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_BUFFER );
	}
	error_code =  SPI_Data_Write_Read(  board_id,		/* board type		*/
										1,				/* tx object size, always a byte in size 	*/
										object_count,	/* tx object count 	*/
										tx_buf, 		/* transmit buffer 	*/
										object_count,	/* rx object count	*/
										rx_buf 			/* receive buffer 	*/
							  	  	  );
	if ( CSB_SOFTWARE == dataset->as_spi_chip_select_behavior )
	{
		SPI_Commit( board_id, 0x00 );
	}
	AS_Chip_Select_Route_Restore( board_id );
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
static int AS_Registers_Write( int board_id, size_t address, size_t count, uint8_t * tx_buf, uint8_t * rx_buf )
{
	size_t					object_count;
	size_t					index;
	int						error_code;
	struct board_dataset *  dataset;

	error_code = SUCCESS;
	dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "AS_Registers_Write"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	/* command validity test */
	if ( ( address + count ) > AS_ADS1259_REGISTER_QTY ) return -EC_REGISTER_ADDRESS_EXCEEDED;

	/* op-code fix up */
	tx_buf[0] = 0x40 + ( (uint8_t) ( 0x0F & address ) );
	if ( 0 == count )
	{
		tx_buf[1]    = 0;
		count        = 1;
		object_count = 3;
	}
	else
	{
		tx_buf[1] = ( (uint8_t) ( 0x0F & count ) ) - 1;
		object_count = ( 0x0F & count ) + 2;
	}

	if ( true == dataset->set__suppress_io_activity )
	{
		/* save the register value(s) */
		for ( index = 0; index < count; index++ )
		{
			dataset->ads1259.reg[index + address] = tx_buf[index + 2];
		}
		return SUCCESS;
	}

	/* chip select wraps around buffer */
	AS_Chip_Select_Route_Override( board_id );
	if ( CSB_SOFTWARE == dataset->as_spi_chip_select_behavior )
	{
		SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_SOFTWARE );
		SPI_Commit( board_id, 0xFF );
	}
	else
	{
		SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_BUFFER );
	}
	error_code =  SPI_Data_Write_Read(  board_id,		/* board type		*/
										1,				/* tx object size, always a byte in size 	*/
										object_count,	/* tx object count 	*/
										tx_buf, 		/* transmit buffer 	*/
										object_count,	/* rx object count	*/
										rx_buf 			/* receive buffer 	*/
							  	  	  );
	if ( CSB_SOFTWARE == dataset->as_spi_chip_select_behavior )
	{
		SPI_Commit( board_id, 0x00 );
	}
	AS_Chip_Select_Route_Restore( board_id );
//AS_REGISTER_WRITE_END:
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_RReg( int board_id, int argc, char * argv[] )
{
	size_t		count;
	size_t		index;
	size_t		address;
	int			error_code;
	BOOL		dump_as_hex;
	FILE *		fd_text;
	char * 		file_name_text;
	size_t		file_request;  /* 0=no request, 1=text, 2=bin */
	size_t		file_valid;    /* bit 0 - text valid, bit 1 - binary valid */
	const size_t  FILE_TYPE_TEXT   			= 0x01;
	const size_t  FILE_TYPE_TEXT_APPEND   	= 0x04;
	uint8_t		tx_buf[12] = { 0x00 };
	uint8_t		rx_buf[12] = { 0x00 };

#define FUNCTION_NAME__  "CMD__AS_RReg"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__


#if(0)
	dump_as_hex = false;
	index = 0;
	if ( ( argc < 1 ) || ( NULL == argv ) )
	{
		count   = AS_ADS1259_REGISTER_QTY;
		address = AS_ADS1259_CONFIG0;
		index = 1;
	}
	else
	{
		if ( ( 'a' == argv[0][0] ) || ( 'A' == argv[0][0] ) ) /* for all */
		{
			count   = AS_ADS1259_REGISTER_QTY;
			address = AS_ADS1259_CONFIG0;
			index = 1;
		}
		else if ( 1 == argc )
		{
			count  = 1;
			error_code = AS_Register_Name_To_Offset( argv[0] );
			if ( error_code < 0 ) address = (size_t) strtol( argv[0], NULL, 0 );
			else                  address = (size_t) error_code;
			index  = 1;
		}
		else
		{
			/* obtain address/offset and register count */
			count   = (size_t) strtol( argv[1], NULL, 0 );
			error_code = AS_Register_Name_To_Offset( argv[0] );
			if ( error_code < 0 ) address = (size_t) strtol( argv[0], NULL, 0 );
			else 				  address = (size_t) error_code;
			index = 2;
		}

		if ( argc > index )
		{
			if ( ( 'h' == argv[index][0] ) || ( 'H' == argv[index][0] ) )
			{
				dump_as_hex = true;
			}
		}
	}
#endif

	count 		 = 0;
	dump_as_hex  = false;
	index 		 = 0;
	fd_text      = stdout;
	file_request = 0;
	file_valid   = 0;
	if ( ( argc < 1 ) || ( NULL == argv ) )
	{
		count   = AS_ADS1259_REGISTER_QTY;
		address = AS_ADS1259_CONFIG0;
	}
	else
	{
		while ( index < argc )
		{
			if ( ( FILE_TYPE_TEXT | FILE_TYPE_TEXT_APPEND ) & file_request )
			{
				file_name_text = argv[index];
				if ( FILE_TYPE_TEXT_APPEND & file_request )
				{	/* append existing file - we assume file open just works */
					fd_text      = fopen( file_name_text, "at" );
					file_request = file_request & ~FILE_TYPE_TEXT_APPEND;
					file_valid   = file_valid   |  FILE_TYPE_TEXT_APPEND;
				}
				else
				{	/* over-write existing file - we assume file open just works */
					fd_text      = fopen( file_name_text, "wt" );
					file_request = file_request & ~FILE_TYPE_TEXT;
					file_valid   = file_valid   |  FILE_TYPE_TEXT;
				}
			}
			else if ( 0 == strcmpi( "--at", argv[index] )  )
			{
				file_request = file_request | FILE_TYPE_TEXT_APPEND;
			}
			else if ( 0 == strcmpi( "--wt", argv[index] )  )
			{
				file_request = file_request | FILE_TYPE_TEXT;
			}
			else if ( 0 == strcmpi( "--hex", argv[index] )  )
			{ /* hex formatting request */
				dump_as_hex = true;
			}
			else
			{
				switch( index )
				{
					case 0: /* assume start address information */
						if ( ( 'a' == argv[0][0] ) || ( 'A' == argv[0][0] ) ) /* for all */
						{
							count   = AS_ADS1259_REGISTER_QTY;
							address = AS_ADS1259_CONFIG0;
						}
						else
						{
							count  = 1; /* assume */
							error_code = AS_Register_Name_To_Offset( argv[0] );
							if ( error_code < 0 ) address = (size_t) strtol( argv[0], NULL, 0 );
							else                  address = (size_t) error_code;
						}
						break;
					case 1: /* assume register count */
						count   = (size_t) strtol( argv[1], NULL, 0 );
						break;
					case 2: /* assume "hex formatting request */

						break;
					default:
						break; /* assume nothing */
				}
			}
			index++;
		}
	}

	error_code = AS_Registers_Read( board_id, address, count, tx_buf, rx_buf );
	if ( error_code < 0 ) return error_code;

	if ( dump_as_hex )
	{
		Hex_Dump_Line( (uint16_t) address, count, &(rx_buf[2]), fd_text );
	}
	else
	{
		for ( index = 0; index < count; index++ )
		{
			fprintf( fd_text, "%s=0x%02X", as_register_name[address + index], rx_buf[index + 2] );
			Bit_String_Report( as_bit_string, address + index, rx_buf[index + 2], fd_text );
			fprintf( fd_text, "\n" );
		}
	}

	if ( file_valid & ( FILE_TYPE_TEXT | FILE_TYPE_TEXT_APPEND ) ) fclose( fd_text );

	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
enum buffer_column { BUFFER_COLUMN_VALUE = 0, BUFFER_COLUMN_TAG1 = 1, BUFFER_COLUMN_TAG2 = 2 };
#define				BUFFER_COLUMN    3
#define             BUFFER_LENGTH 1024
static int32_t		buffer[BUFFER_COLUMN][BUFFER_LENGTH];
static size_t		buffer_index;
/**********************************************************************************************************//**
 * @brief
 */
static void Buffer_Init(void)
{
	size_t index;
	buffer_index = 0;
	for ( index = 0; index < BUFFER_LENGTH; index++ )
	{
		buffer[BUFFER_COLUMN_VALUE][index] = 0;
		buffer[BUFFER_COLUMN_TAG1][index]  = 0;
		buffer[BUFFER_COLUMN_TAG2][index]  = 0;
	}
}
/**********************************************************************************************************//**
 * @brief
 */
static int Buffer_Mean_Int32( int32_t (* buffer)[BUFFER_LENGTH], size_t length, double * mean )
{
	size_t		index;
	double		sum;
	int32_t * 	buf;

	if ( 0 == length ) return -EC_PARAMETER;
	sum = 0.0;
	buf = buffer[BUFFER_COLUMN_VALUE];
	for ( index = 0; index < length; index++ )
	{
		sum += (double) buf[index];
	}
	*mean = sum / ((double)length);
	return SUCCESS;
}
/*****************************************************
Uses Bessel's correction.
*/
static int Buffer_Standard_Deviation_Int32( int32_t (* buffer)[BUFFER_LENGTH], size_t length, double mean, double * stdev )
{
	size_t		index;
	double  	term;
	double		squared_sum;
	int32_t * 	buf;

	if ( length < 2 ) return -EC_PARAMETER;

	squared_sum = 0.0;
	buf = buffer[BUFFER_COLUMN_VALUE];
	for ( index = 0; index < length; index++ )
	{
		term = ((double) buf[index]) - mean;
		squared_sum += term * term;
	}
	squared_sum = squared_sum / ((double) ( length - 1));
	*stdev = sqrt( squared_sum );
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int Buffer_Minimum_Int32( int32_t (* buffer)[BUFFER_LENGTH], size_t length, double * minimum )
{
	int32_t 	min;
	size_t  	index;
	int32_t * 	buf;

	if ( 0 == length ) return -EC_PARAMETER;
	min = INT32_MAX;
	buf = buffer[BUFFER_COLUMN_VALUE];
	for ( index = 0; index < length; index++ )
	{
		if ( buf[index] < min ) min = buf[index];
	}
	*minimum = (double) min;
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int Buffer_Maximum_Int32( int32_t (* buffer)[BUFFER_LENGTH], size_t length, double * maximum )
{
	int32_t 	max;
	size_t  	index;
	int32_t * 	buf;

	if ( 0 == length ) return -EC_PARAMETER;
	max = INT32_MIN;
	buf = buffer[BUFFER_COLUMN_VALUE];
	for ( index = 0; index < length; index++ )
	{
		if ( buf[index] > max ) max = buf[index];
	}
	*maximum = (double) max;
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int Buffer_Peak_To_Peak_Int32( double min, double max, double * peak_to_peak )
{
	*peak_to_peak = max - min;
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int Buffer_Stuff( int32_t value, int32_t tag1, int32_t tag2 )
{
	if ( buffer_index >= BUFFER_LENGTH ) return -EC_BUFFER_EXCEEDED;
	buffer[BUFFER_COLUMN_VALUE][buffer_index] = value;
	buffer[BUFFER_COLUMN_TAG1][buffer_index]  = tag1;
	buffer[BUFFER_COLUMN_TAG2][buffer_index]  = tag2;
	buffer_index++;
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int Buffer_Length( size_t * length )
{
	*length = buffer_index;
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int Buffer_Save( FILE * out )
{
	size_t 		index;
	size_t		column;

	for ( index = 0; index < buffer_index; index++ )
	{
		for ( column = 0; column < BUFFER_COLUMN; column++ )
		{
#if defined( __MSDOS__ )
			fprintf( out, "%ld", buffer[column][index] );
#else
			fprintf( out, "%d",  buffer[column][index] );
#endif
			if ( column < (BUFFER_COLUMN - 1) ) fprintf( out, ","  );
			else                                fprintf( out, "\n" );
		}
	}
	return SUCCESS;
}

/**********************************************************************************************************//**
 * @brief
 */
static int Buffer_Save_Binary_Int32( const char * file_name )
{
	FILE * 	fd;
	size_t	index;
	size_t	column;

	if ( 0 == buffer_index ) return -SUCCESS;

	fd = fopen( file_name, "wb" );
	if ( NULL == fd ) return -EC_FILE_ERROR;

	for ( index = 0; index < buffer_index; index++ )
	{ /* have to write out each one individual to re-order properly */
		for ( column = 0; column < BUFFER_COLUMN; column++ )
		{
			fwrite( 	&(buffer[column][index]), 	/* data send to file 						*/
						sizeof( int32_t ),	   		/* size of each frame in bytes 				*/
						1,							/* number of frames in the buffer 			*/
						fd							/* data file that buffer will be copied to 	*/
				  );
		}
	}
#if(0)
	/* write the data to binary file - Use Audacity to view file */
	fwrite( 	buffer,								/* buffer to send to file 					*/
				BUFFER_COLUMN * sizeof( int32_t ),	/* size of each frame in bytes 				*/
				buffer_index,						/* number of frames in the buffer 			*/
				fd									/* data file that buffer will be copied to 	*/
		  );
#endif
	fclose( fd );

	return SUCCESS;
}

/**********************************************************************************************************//**
 * @brief
 */
static int AS_Rdata_Statistics( FILE * out )
{
	int 	error_code;
	size_t 	length;
	double	mean;
	double  maximum;
	double  minimum;
	double  peak_to_peak;
	double  stdev;

	Buffer_Length( &length );

	Buffer_Mean_Int32( buffer, length, &mean );
	Buffer_Minimum_Int32( buffer, length, &minimum );
	Buffer_Maximum_Int32( buffer, length, &maximum );
	Buffer_Peak_To_Peak_Int32( minimum, maximum, &peak_to_peak );
	error_code = Buffer_Standard_Deviation_Int32( buffer, length, mean, &stdev );
    fprintf( out, "Data Statistics:\n" );
	fprintf( out, "  sample_quantity         = %7d\n", (int) length   );
	fprintf( out, "  mean                    = %7.3f\n", mean         );
	if ( SUCCESS == error_code )
	{ /* have enough samples to compute standard deviation */
	fprintf( out, "  stdev                   = %7.3f\n", stdev        );
	}
	fprintf( out, "  minimum                 = %7.3f\n", minimum      );
	fprintf( out, "  maximum                 = %7.3f\n", maximum      );
	fprintf( out, "  peak-to-peak            = %7.3f\n", peak_to_peak );

	fprintf( out, "\n"               );
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 *
 * TODO: need a timeout mechanism.
 */
static void AS_Data_Ready_Wait( int board_id, size_t drdy_anticipated_active, size_t * loop_count_per_data_ready )
{
	size_t		test_count;
	size_t		valid_count;
	uint8_t		tx_buf[4] = { 0x00, 0x00, 0x00, 0x00 };
	uint8_t		rx_buf[4] = { 0x00, 0x00, 0x00, 0x00 };
	uint8_t		scratch;
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "AS_Data_Ready_Wait"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

#if ( 1 == SPI_LOGGING )
	if ( SPI_LOG_ACTIVE == dataset->spi.log_state )
	{
		fprintf( dataset->spi.log_file, "AS_Data_Ready_Wait: %d\n", drdy_anticipated_active ? 1 : 0 );
	}
#endif

	/* computes inverted value */
	if ( 0 == drdy_anticipated_active ) scratch = 0x00;
	else                                scratch = 0x80;

	test_count = 0;
	valid_count = 0;
	rx_buf[2] = scratch;
	do
	{
		if ( 0 == valid_count ) test_count++;

		if ( scratch != ( rx_buf[2] & 0x80 ) )
		{ /* must be valid for at least two cycles for a valid state */
			if ( 0 == drdy_anticipated_active ) valid_count = 2;
			else                                valid_count++;
		}

		AS_Registers_Read( board_id, 2, 1, tx_buf, rx_buf );
#if ( 1 == SPI_LOGGING )
		if ( SPI_LOG_ACTIVE == dataset->spi.log_state )
		{
			size_t index;
			fprintf( dataset->spi.log_file, "*AS_Data_Ready_Wait: %d:", (int) test_count );
			for ( index = 0; index < 3; index++ )
			{
			fprintf( dataset->spi.log_file, " 0x%02X", rx_buf[index] );
			}
			fprintf( dataset->spi.log_file, "\n" );
		}
#endif
	} while ( valid_count < 2 );




	if ( test_count > 1 )
	{ /* we have detected at least one time where data ready is not at desired state */
		if ( NULL != loop_count_per_data_ready )
		{
			*loop_count_per_data_ready = 0;
		}
	}
}
/**********************************************************************************************************//**
 * @brief
 * Rdata command
 */
static int AS_Data_Read( int board_id, int32_t * adc_value )
{
	int			error_code;
	uint32_t	adc_temp;
	size_t		byte_index;
	uint8_t		tx_buf[4] = { 0x12, 0x00, 0x00, 0x00 };
	uint8_t		rx_buf[4] = { 0x00, 0x00, 0x00, 0x00 };
	struct board_dataset *  dataset;

#define FUNCTION_NAME__  "AS_Data_Read"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	/* chip select wraps around buffer */
	AS_Chip_Select_Route_Override( board_id );
	if ( CSB_SOFTWARE == dataset->as_spi_chip_select_behavior )
	{
		SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_SOFTWARE );
		SPI_Commit( board_id, 0xFF );
	}
	else
	{
		SPI_Configuration_Chip_Select_Behavior_Set( board_id, CSB_BUFFER );
	}

	{ /* just to be absolutely sure - we will clear the rx_buffer and tx_buffer (again) */
		size_t index;
		for ( index = 0; index < 4; index++ )
		{
			rx_buf[index] = 0;
			tx_buf[index] = 0;
		}
		tx_buf[0] = 0x12;
	}

	error_code =  SPI_Data_Write_Read(  board_id,	/* board type		*/
										1,			/* tx object size, always a byte in size 	*/
										4,			/* tx object count 	*/
										tx_buf, 	/* transmit buffer 	*/
										4, 			/* rx object count	*/
										rx_buf 		/* receive buffer 	*/
							  	  	  );
#if ( 1 == SPI_LOGGING )
	if ( SPI_LOG_ACTIVE == dataset->spi.log_state )
	{
		size_t index;
		fprintf( dataset->spi.log_file, "AS_Data_Read:" );
		for ( index = 0; index < 4; index++ )
		{
			fprintf( dataset->spi.log_file, " 0x%02X", rx_buf[index] );
		}
		fprintf( dataset->spi.log_file, "\n" );
	}
#endif
	if ( CSB_SOFTWARE == dataset->as_spi_chip_select_behavior )
	{
		SPI_Commit( board_id, 0x00 );
	}
	AS_Chip_Select_Route_Restore( board_id );
	if ( error_code < 0 ) goto AS_Read_Data_Exit;

	adc_temp = 0;  /* OK, we clear, again, the destination of the information - no way to get a duplicate here.... */
	for ( byte_index = 1; byte_index < 4; byte_index++ )	adc_temp = (adc_temp << 8 ) + ( (uint32_t) rx_buf[byte_index] );

	/* sign extend first before converting */
	if ( 0 != ( 0xFF800000L & adc_temp ) )
	{
		adc_temp = 0xFF800000L | adc_temp;
	}
	*adc_value = (int32_t) adc_temp;

AS_Read_Data_Exit:
	return error_code;
}


/*<INTERNAL_BEGIN>*/
/**********************************************************************************************************//**
 * @brief
 */
static void AS_Time_Statistics(  STOPWATCH_HANDLE_TYPE * stopwatch_handle,
								 int					 time_overall_index,
								 int					 time_reading_index,
								 int					 time_writing_index,
								 FILE * 				 out
							  )
{
	STOPWATCH_TIMEVAL_TYPE * time_value;
	double time_result[STOPWATCH_QUANTITY];
	size_t sample_count;
	double dscratch;

	Buffer_Length( &sample_count );

	fprintf( out, "Timing Statistics:\n" );
	time_value = StopWatch_Value( stopwatch_handle[time_overall_index] );
	time_result[time_overall_index] = ( (double) time_value->tv_sec ) + ( (double) time_value->tv_usec ) * 0.000001;
	fprintf( out, "  Total time           = %7.3f sec, 100%%\n", time_result[time_overall_index] );

	if ( 0.0 == time_result[time_overall_index] ) return; /* nothing more to do */

	time_value = StopWatch_Value( stopwatch_handle[time_reading_index] );
	time_result[time_reading_index] = ( (double) time_value->tv_sec ) + ( (double) time_value->tv_usec ) * 0.000001;
	fprintf( out, "  Read timing          = %7.3f sec, %5.1f%%\n", time_result[time_reading_index], ( time_result[time_reading_index]* 100.0 / time_result[time_overall_index] ) );

	time_value = StopWatch_Value( stopwatch_handle[time_writing_index] );
	time_result[time_writing_index] = ( (double) time_value->tv_sec ) + ( (double) time_value->tv_usec ) * 0.000001;
	fprintf( out, "  Write timing         = %7.3f sec, %5.1f%%\n", time_result[time_writing_index], ( time_result[time_writing_index]* 100.0 / time_result[time_overall_index] ) );

	fprintf( out, "  Total Samples        = %7d\n", (int) sample_count );
	dscratch = ( (double) sample_count ) / time_result[time_overall_index];
	fprintf( out, "  Samples/Sec          = %7.2f Samples/Sec\n",  dscratch  );
	dscratch = 1000000.0 / dscratch;
	fprintf( out, "  Time per sample      = %7.3f uSec/Sample\n",  dscratch );
}
/*<INTERNAL_END>*/

/**********************************************************************************************************//**
 * @brief
 *
 *
 */
static int CMD__AS_Rdata( int board_id, int argc, char * argv[] )
{
	char        baton[4]  = { '/', '-', '\\', '|' };
	const size_t baton_max = 3;
	size_t		baton_index;
	int32_t		adc_value;
	int32_t		adc_value_previous;
//	uint32_t	adc_temp;
//	size_t		byte_index;
	size_t 		index;
	size_t		loop_count_per_data_ready;
	size_t		sample_qty;
	FILE *		fd_text;
	char * 		file_name_text;
	char * 		file_name_binary;
	size_t		file_request;  /* 0=no request, 1=text, 2=bin */
	size_t		file_valid;    /* bit 0 - text valid, bit 1 - binary valid */
#if ( 1 == LPT_DEBUG_USE )
	size_t		lpt_counter;
#endif
#if(0)
	size_t		start_count;
#endif
	BOOL		file_report_registers;
	BOOL		verbose;
	BOOL		use_environment_ads1259_registers;
	int			error_code;
	double time_result;
	double time_result_previous;
	struct board_dataset *  dataset;
	const size_t  FILE_TYPE_TEXT   = 0x01;
	const size_t  FILE_TYPE_BINARY = 0x02;
/*<INTERNAL_BEGIN>*/
//	STOPWATCH_TIMEVAL_TYPE * time_value;
//	double time_result[STOPWATCH_QUANTITY];
	STOPWATCH_HANDLE_TYPE stopwatch_handle[STOPWATCH_QUANTITY];
	enum { TIME_OVERALL = 0, TIME_READING = 1, TIME_WRITING = 2 };
/*<INTERNAL_END>*/
	(void)		argc;
	(void)		argv;

	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

#define FUNCTION_NAME__  "CMD__AS_Rdata"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	fd_text      = NULL;
	file_request = 0;
	file_valid   = 0;
	index        = 0;
	sample_qty   = 1;
	verbose      = false;
	file_report_registers = false;
	use_environment_ads1259_registers = false;
#if ( 1 == SPI_LOGGING )
	dataset->spi.log_state = SPI_LOG_NONE;
#endif
	/* determine parameters */
	while ( index < argc )
	{
		if ( ( FILE_TYPE_TEXT | FILE_TYPE_BINARY ) & file_request )
		{
			if ( FILE_TYPE_TEXT & file_request )
			{
				file_name_text = argv[index];
				file_request = file_request & ~FILE_TYPE_TEXT;
				file_valid   = file_valid   |  FILE_TYPE_TEXT;
			}
			if ( FILE_TYPE_BINARY & file_request )
			{
				file_name_binary = argv[index];
				file_request = file_request & ~FILE_TYPE_BINARY;
				file_valid   = file_valid   |  FILE_TYPE_BINARY;
			}
		}
		else if ( 0 == strcmpi( "--text", argv[index] )  )
		{
			file_request = file_request | FILE_TYPE_TEXT;
		}
		else if ( 0 == strcmpi( "--binary", argv[index] )  )
		{
			file_request = file_request | FILE_TYPE_BINARY;
		}
		else if ( 0 == strcmpi( "--v", argv[index] )  )
		{
			verbose = true;
		}
		else if ( 0 == strcmpi( "--r", argv[index] )  )
		{
			file_report_registers = true;
		}
		else if ( 0 == strcmpi( "--e", argv[index] )  )
		{
			use_environment_ads1259_registers = true;
		}
#if ( 1 == SPI_LOGGING )
		else if ( 0 == strcmpi( "--s", argv[index] )  )
		{
			dataset->spi.log_state = SPI_LOG_REQUEST;
		}
#endif
		else
		{
			sample_qty = (size_t) strtol( argv[index], NULL, 0 );
			if ( sample_qty > ( BUFFER_LENGTH - 1 )  ) sample_qty = BUFFER_LENGTH;
		}
		index++;
	}

	CMD__AS_Reset(  board_id, 0, NULL );
	Buffer_Init();

/*<INTERNAL_BEGIN>*/
	StopWatch_Initialization();
	/* stopwatch initialization */
	stopwatch_handle[TIME_OVERALL] = StopWatch_Open();
	StopWatch_Set( stopwatch_handle[TIME_OVERALL], STOPWATCH_RESET );
	stopwatch_handle[TIME_READING] = StopWatch_Open();
	StopWatch_Set( stopwatch_handle[TIME_READING], STOPWATCH_RESET );
	stopwatch_handle[TIME_WRITING] = StopWatch_Open();
	StopWatch_Set( stopwatch_handle[TIME_WRITING], STOPWATCH_RESET );
/*<INTERNAL_END>*/


#if defined( __MSDOS__ )
	{ /* estimate the delay and delay for that amount of timer + 10mS */
		double scratch;

		scratch = ADS1259_CLOCK_MHZ * ( 1.0 + ADS1259_CLOCK_TOLERANCE );
		scratch = 65536.0 / scratch;
		scratch = scratch * 1000.0;   /* convert to mS */
		scratch = scratch + 100.0;     /* add 100mS for a margin of safety */
		delay ( (size_t) scratch );
	}
#endif
#if ( 1 == LPT_DEBUG_USE )
	LPT_Initialize();
	lpt_counter = 0;
#endif
	CMD__AS_Sdatac( board_id, 0, NULL );
	CMD__AS_Stop( board_id, 0, NULL );

//since the hardware is reset we need a way to first pull the values and/or restore to
//most resent values.  For now --e is mandatory to get it to work correctly!!!

	loop_count_per_data_ready = 0;

	if ( use_environment_ads1259_registers )
	{
		uint8_t		tx_buf[12] = { 0x00 };
		uint8_t		rx_buf[12] = { 0x00 };

		for ( index = 0; index < AS_ADS1259_REGISTER_QTY; index ++ )
		{
			tx_buf[index + 2] = dataset->ads1259.reg[index];
		}
		//memcpy( &(tx_buf[2], dataset->ads1259.reg, AS_ADS1259_REGISTER_QTY );
		AS_Registers_Write( board_id, AS_ADS1259_CONFIG0, AS_ADS1259_REGISTER_QTY, tx_buf, rx_buf );
	}

	time_result_previous = 0.0;

	if ( ( true == verbose ) || ( true == file_report_registers ) )
	{
		char *       argv_aux[3] = { "a", "--wt", NULL };
		int    		 argc_aux    = 3;

		if ( file_valid & FILE_TYPE_TEXT )
		{
			argv_aux[2] = file_name_text;
		}
		else
		{
			argv_aux[1] = NULL;
			argc_aux    = 1;
		}

		CMD__AS_RReg( board_id, argc_aux, argv_aux );
	}

	if ( file_valid & FILE_TYPE_TEXT )
	{   /* we assume file open just works */

		if ( ( true == verbose ) || ( true == file_report_registers ) )
		{ /* append to whatever is there */
			fd_text = fopen( file_name_text, "at" );
			fprintf( fd_text, "\n" );
		}
		else
		{   /* over-write */
			fd_text = fopen( file_name_text, "wt" );
		}
	}

	if ( true == verbose )
	{
		baton_index = 0;
		printf( "\nActive:  " );
	}
#if ( 1 == SPI_LOGGING )
	if ( SPI_LOG_REQUEST == dataset->spi.log_state )
	{
		dataset->spi.log_file = fopen( "spilog.txt", "wt" );
		if ( NULL == dataset->spi.log_file ) dataset->spi.log_state = SPI_LOG_NONE;
		else                                 dataset->spi.log_state = SPI_LOG_ACTIVE;
	}
#endif

/*<INTERNAL_BEGIN>*/
	StopWatch_Set( stopwatch_handle[TIME_OVERALL], STOPWATCH_START );
/*<INTERNAL_END>*/
//	CMD__AS_Start( board_id, 0, NULL );

	//if ( 0 != AS_Data_Ready_NoWait( board_id, 1 /* active */ ) )
	//{ /* toss any samples ready to be read */
		/* again, just to be sure...we clear the adc_value */
		adc_value = 0;
		error_code = AS_Data_Read( board_id, &adc_value );
	//}

#if(0)
start_count = 0;
#endif

	adc_value_previous = 0;
	adc_value = 0;

	for ( index = 0; index < sample_qty; index++ )
	{
#if ( 1 == LPT_DEBUG_USE )
		LPT_SLTIN_Clear();
#endif
/*<INTERNAL_BEGIN>*/
		/* BEGIN: read block(s) of analog input data */
		StopWatch_Set( stopwatch_handle[TIME_READING], STOPWATCH_START );
/*<INTERNAL_END>*/

#if(0)
/* inject a bogus (not start) command every 5th sample
 *    -- this is not possible, otherwise it hangs!!
 *
 * We are also now waiting for two valid drdy_b before reading.
 * That has helped some I think.
 *
 * When data is missed, the subsequent timing of the analog data is preserved (OK).  The
 * symptom is just like it froze and read the previous value.  Timing before valid is about the same.  It is
 * clearly coming from the ADC!!! Has to be.
 *
 * No errors reported when duplicate samples occur.
 */
if ( start_count > 5 )
{
	CMD__AS_Bogus( board_id, 0, NULL );  /* simulates a register read command */
	start_count = 0;
}
else
{
	CMD__AS_Start( board_id, 0, NULL );
	start_count++;
}
#endif

		CMD__AS_Start( board_id, 0, NULL );
		CMD__AS_Start( board_id, 0, NULL );
		/* wait for data_ready to go inactive */
		AS_Data_Ready_Wait( board_id,
							0, 							/* data ready in inactive state							*/
							NULL
						  );
		loop_count_per_data_ready++;

		/* wait for the data ready to become active */
		AS_Data_Ready_Wait( board_id,
							1, 							/* data ready in active state 							*/
							&loop_count_per_data_ready  /* resets loop counter if we detect data ready inactive	*/
						  );

		/* read the ADC */
		adc_value_previous = adc_value;
		adc_value = 0; /* this occurs in the function below, but we will do it again, just to be absolutely sure */
#if ( 1 == LPT_DEBUG_USE )
		if ( 0 == lpt_counter ) LPT_SLTIN_Set();
#endif

#if defined( __MSDOS__ )
		delay( 10 /* mS */ );
#endif

		error_code = AS_Data_Read( board_id, &adc_value );




		if ( adc_value_previous == adc_value )
		{
#if ( 1 == LPT_DEBUG_USE )
			if ( 0 == lpt_counter )  lpt_counter++;
#endif
		}

		if ( error_code < 0 ) goto CMD__AS_Rdata_Error_Exit;

/*<INTERNAL_BEGIN>*/
		StopWatch_Set( stopwatch_handle[TIME_READING], STOPWATCH_PAUSE );
		StopWatch_Set( stopwatch_handle[TIME_WRITING], STOPWATCH_START );
/*<INTERNAL_END>*/

		{
			STOPWATCH_TIMEVAL_TYPE * time_value;
			time_value  = StopWatch_Value( stopwatch_handle[TIME_READING] );
			time_result = ( (double) time_value->tv_sec ) + ( (double) time_value->tv_usec ) * 0.000001; /* Sec */
		}



		Buffer_Stuff( adc_value, (int32_t)( ( time_result - time_result_previous ) * 1000.0 ) /* tag1 */, (int32_t) loop_count_per_data_ready /* tag2 */ );
		//Buffer_Stuff( adc_value, loop_count_per_data_ready /* tag */ );

		if ( true == verbose )
		{
			printf( "\b%c", baton[baton_index] );
			if ( baton_index == baton_max ) baton_index = 0;
			else                            baton_index++;

			if ( FILE_TYPE_TEXT & file_valid )
			{
#if defined( __MSDOS__ )
				fprintf( fd_text, "%ld, %7.3f\n", adc_value, ( time_result - time_result_previous ) );
#else
				fprintf( fd_text, "%d, %7.3f\n",  adc_value, ( time_result - time_result_previous ) );
#endif
			}
		}

		time_result_previous = time_result;

/*<INTERNAL_BEGIN>*/
		StopWatch_Set( stopwatch_handle[TIME_WRITING], STOPWATCH_PAUSE );
/*<INTERNAL_END>*/
	}
/*<INTERNAL_BEGIN>*/
	StopWatch_Set( stopwatch_handle[TIME_OVERALL], STOPWATCH_PAUSE );
/*<INTERNAL_END>*/
	if ( true == verbose )
	{
		printf( "\n" );
	}

	if ( FILE_TYPE_TEXT & file_valid )
	{
		if ( true == verbose ) fprintf( fd_text, "\n" );
		Buffer_Save( fd_text );
		AS_Rdata_Statistics( fd_text );
		AS_Time_Statistics( stopwatch_handle, TIME_OVERALL, TIME_READING, TIME_WRITING, fd_text );
		fclose( fd_text );
	}
	if ( FILE_TYPE_BINARY & file_valid )
	{
		Buffer_Save_Binary_Int32( file_name_binary );
	}

/*<INTERNAL_BEGIN>*/
	AS_Time_Statistics( stopwatch_handle, TIME_OVERALL, TIME_READING, TIME_WRITING, stdout );
/*<INTERNAL_END>*/

	AS_Rdata_Statistics( stdout );

//	CMD__AS_Stop( board_id, 0, NULL );
/*<INTERNAL_BEGIN>*/
	StopWatch_Termination();
/*<INTERNAL_END>*/
#if ( 1 == SPI_LOGGING )
	if ( dataset->spi.log_state >= SPI_LOG_FILE ) fclose( dataset->spi.log_file );
	dataset->spi.log_state = SPI_LOG_NONE;
#endif
	return SUCCESS;
CMD__AS_Rdata_Error_Exit:
	AS_Chip_Select_Route_Restore( board_id );
	if ( FILE_TYPE_TEXT & file_valid ) fclose( fd_text );
/*<INTERNAL_BEGIN>*/
	StopWatch_Termination();
/*<INTERNAL_END>*/
#if ( 1 == SPI_LOGGING )
	if ( dataset->spi.log_state >= SPI_LOG_FILE ) fclose( dataset->spi.log_file );
	dataset->spi.log_state = SPI_LOG_NONE;
#endif
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
static int AS_Calibration_Value_Write( int board_id, size_t address, int32_t value )
{
	int				error_code;
	uint32_t		scratch;
	size_t 			index;
	const size_t	byte_length = 3;
	uint8_t			tx_buf[12] = { 0x00 };
	uint8_t			rx_buf[12] = { 0x00 };

#define FUNCTION_NAME__  "AS_Calibration_Value_Write"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	/* clamp if necessary */
	ADS1259_Calibration_Range_Test( &value, address, 1 /* clamp and skip any errors */ );

	scratch = (uint32_t) value;
	for ( index = 0; index < byte_length; index++ )
	{ /* note: LSB to MSB */
		tx_buf[index + 2] = (uint8_t)( scratch & 0xFF );
		scratch = scratch >> 8;
	}
	/* op-code fix up occurs on the first two tx_buf bytes in the AS_Registers_Write()  */
	error_code = AS_Registers_Write( board_id, address, byte_length, tx_buf, rx_buf );
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
static int AS_Calibration_Value_Read( int board_id, size_t address, int32_t * value )
{
	int			error_code;
	uint32_t	value_temp;
	size_t 		index;
	const size_t	byte_length = 3;
	uint8_t		tx_buf[12] = { 0x00 };
	uint8_t		rx_buf[12] = { 0x00 };

#define FUNCTION_NAME__  "AS_Calibration_Value_Read"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__
	/* op-code fix up occurs on the first two tx_buf bytes in the AS_Registers_Read()  */
	error_code = AS_Registers_Read( board_id, address, byte_length, tx_buf, rx_buf );
	if ( error_code < 0 ) goto AS_Calibration_Value_Read_Error_Exit;
	value_temp = 0;
	/* note: MSB to LSB */
	for ( index = 0; index < byte_length; index++ ) value_temp = ( value_temp << 8 ) + ( (uint32_t) rx_buf[byte_length - index + 2 - 1] );

	/* sign extend first before converting */
	if ( 0 != ( 0xFF800000L & value_temp ) )
	{
		value_temp = 0xFF800000L | value_temp;
	}
	*value = (int32_t) value_temp;
	return SUCCESS;
AS_Calibration_Value_Read_Error_Exit:
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_WReg( int board_id, int argc, char * argv[] )
{
	size_t	count;
	size_t	index;
	size_t	address;
	int		error_code;
	BOOL	not_done;
	uint8_t	tx_buf[12] = { 0x00 };
	uint8_t	rx_buf[12] = { 0x00 };

#define FUNCTION_NAME__  "CMD__AS_WReg"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( argc < 2 ) return -EC_SYNTAX;   /* need at least an address and value to write */

	index = 0;
	if ( ( argv[index][0] >= '0' ) && ( argv[index][0] <= '9' ) )
	{ /* offset value */
		address  = (size_t) strtol( argv[index], NULL, 0 );
	}
	else
	{
		error_code = AS_Register_Name_To_Offset( argv[index] );
		if ( error_code < 0 ) return -EC_NOT_FOUND;
		address = (size_t) error_code;
	}
	index++;

	count = 0;
	not_done = true;
	do
	{
		tx_buf[count + 2]  = (uint8_t) strtol( argv[index], NULL, 0 );
		count++;
		index++;

		if ( count >= AS_ADS1259_REGISTER_QTY )
		{
			not_done   = false;
			error_code = -EC_EXTRA_IGNORED;
		}
		if ( index >= argc ) not_done = false;
	} while ( not_done );

	error_code = AS_Registers_Write( board_id, address, count, tx_buf, rx_buf );
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Reg_Write_By_Name( int board_id, int argc, char * argv[] )
{
	size_t	count;
	size_t	index;
	size_t	address;
	size_t	value;
	int		error_code;
	uint8_t	tx_buf[12] = { 0x00 };
	uint8_t	rx_buf[12] = { 0x00 };

#define FUNCTION_NAME__  "CMD__AS_Reg_Write_By_Name"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	if ( argc < 2 ) return -EC_SYNTAX;   /* need at least an address and value to write */

	index = 0;
	while ( ( argc - index ) > 1 )
	{
		if ( (argc - index) > 1 )
		{
			if ( ( argv[index][0] >= '0' ) && ( argv[index][0] <= '9' ) )
			{ /* offset value */
				address  = (size_t) strtol( argv[index], NULL, 0 );
			}
			else
			{
				error_code = AS_Register_Name_To_Offset( argv[index] );
				if ( error_code < 0 ) return -EC_NOT_FOUND;
				address = (size_t) error_code;
			}
			index++;

			if ( ( argc - index ) > 0 )
			{ /* register value to be written */
				value  = (size_t) strtol( argv[index], NULL, 0 );
				tx_buf[2] = (uint8_t) value;
				count = 1;
				error_code = AS_Registers_Write( board_id, address, count, tx_buf, rx_buf );
			}
			else
			{
				return -EC_SYNTAX;
			}
			index++;
		}
		else
		{
			error_code = -EC_SYNTAX;
		}
	} /* while loop */

	if ( (argc - index) > 0 ) error_code = -EC_EXTRA_IGNORED;
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Ofscal( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x18 };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Ofscal"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Gancal( int board_id, int argc, char * argv[] )
{
	uint8_t		tx_buf[1] = { 0x19 };
	(void)		argc;
	(void)		argv;

#define FUNCTION_NAME__  "CMD__AS_Gancal"
#if ( IO_TRACE_USE_AS_LOGGING == 1 )
	IO_Trace_Log_Function( FUNCTION_NAME__, __LINE__, IO_TRACE_AS, board_id, 0 );
#endif
#undef FUNCTION_NAME__

	return AS_Opcode_Write( board_id, tx_buf, 1 );
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Calibration_Gain( int board_id, int argc, char * argv[] )
{
	int32_t	fsc;
	int		error_code;

	if ( argc > 0 )
	{ /* write cycle */
		fsc = (int32_t) strtol( argv[0], NULL, 0 );
		error_code = AS_Calibration_Value_Write( board_id, AS_ADS1259_FSC0, fsc );
		if ( error_code < 0 ) return error_code;
	}
	else
	{ /* read cycle */
		error_code = AS_Calibration_Value_Read( board_id, AS_ADS1259_FSC0, &fsc );
		if ( error_code < 0 ) return error_code;
#if defined( __MSDOS__ )
		printf( "Full Scale Calibration (FSC) = 0x%08lX, %ld\n", fsc, fsc );
#else
		printf( "Full Scale Calibration (FSC) = 0x%08X, %d\n", fsc, fsc );
#endif
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Calibration_Offset( int board_id, int argc, char * argv[] )
{
	int						error_code;
	int32_t					ofc;

	if ( argc > 0 )
	{ /* write cycle */
		ofc = (int32_t) strtol( argv[0], NULL, 0 );
		error_code = AS_Calibration_Value_Write( board_id, AS_ADS1259_OFC0, ofc );
	}
	else
	{ /* read cycle */
		error_code = AS_Calibration_Value_Read( board_id, AS_ADS1259_OFC0, &ofc );
		if ( error_code < 0 ) return error_code;
#if defined( __MSDOS__ )
		printf( "Offset Calibration (OFC) = 0x%08lX, %ld\n", ofc, ofc );
#else
		printf( "Offset Calibration (OFC) = 0x%08X, %d\n", ofc, ofc );
#endif
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Register_Save( int board_id, int argc, char * argv[] )
{
	int		error_code;
	FILE * 	fd;
	uint8_t	tx_buf[12] = { 0x00 };
	uint8_t	rx_buf[12] = { 0x00 };

	if ( argc < 1 ) return -EC_SYNTAX;

	error_code = AS_Registers_Read( board_id, AS_ADS1259_CONFIG0, AS_ADS1259_REGISTER_QTY, tx_buf, rx_buf );
	if ( error_code < 0 ) return error_code;

	fd = fopen( argv[0], "w" );
	if ( NULL != fd )
	{
		void * data_pointer = (void *) &(rx_buf[2]);
		fwrite( data_pointer, AS_ADS1259_REGISTER_QTY, 1, fd );
		fclose( fd );
	}
	return SUCCESS;
}
#if(0)
/**********************************************************************************************************//**
 * @brief
 */
static void AS_Register_Defaults( uint8_t * buf, size_t count )
{
	if ( count < AS_ADS1259_REGISTER_QTY ) return;
	buf[AS_ADS1259_CONFIG0] = 0x85;
	buf[AS_ADS1259_CONFIG1] = 0x10;
	buf[AS_ADS1259_CONFIG2] = 0x00;
	buf[AS_ADS1259_OFC0] 	= 0x00;
	buf[AS_ADS1259_OFC1] 	= 0x00;
	buf[AS_ADS1259_OFC2] 	= 0x00;
	buf[AS_ADS1259_FSC0] 	= 0x00;
	buf[AS_ADS1259_FSC1] 	= 0x00;
	buf[AS_ADS1259_FSC2] 	= 0x40;
}
#endif
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Register_Load( int board_id, int argc, char * argv[] )
{
	int						error_code;
	FILE * 					fd;
	void *  				data_pointer = NULL;
	uint8_t					tx_buf[12]   = { 0x00 };
	uint8_t					rx_buf[12]   = { 0x00 };
	struct board_dataset *  dataset;

	dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;
	error_code = SUCCESS;

	if ( 0 == argc )
	{ /* load from environment */
		size_t index;
		for ( index = 0; index < AS_ADS1259_REGISTER_QTY; index ++ )
		{
			tx_buf[index + 2] = dataset->ads1259.reg[index];
		}
		//memcpy( &(tx_buf[2], dataset->ads1259.reg, AS_ADS1259_REGISTER_QTY );
		error_code = AS_Registers_Write( board_id, AS_ADS1259_CONFIG0, AS_ADS1259_REGISTER_QTY, tx_buf, rx_buf );
	}
	else
	{
		fd = fopen( argv[0], "r" );
		if ( NULL == fd )
		{
			error_code = -EC_FILE_ERROR;
		}
		else
		{ /* load from file */
			size_t file_size;
			/* determine file size - if data set has changed we must restart */
			fseek( fd, 0L, SEEK_END );
			file_size = (size_t) ftell( fd );
			fseek( fd, 0L, SEEK_SET );
			if ( file_size != AS_ADS1259_REGISTER_QTY )
			{
				error_code = -EC_FILE_ERROR_SIZE;
			}
			else
			{	/* read in registers */
				data_pointer = (void *) &(tx_buf[2]);
				fread( data_pointer, AS_ADS1259_REGISTER_QTY, 1, fd );
				error_code = AS_Registers_Write( board_id, AS_ADS1259_CONFIG0, AS_ADS1259_REGISTER_QTY, tx_buf, rx_buf );
			}
			fclose( fd );
		}
	}
	return error_code;
}

/**********************************************************************************************************//**
 * @brief
 */
static int CMD__AS_Parameter( int board_id, int argc, char * argv[] )
{
	int 					error_code = SUCCESS;
    int     				argc_new;
    char ** 				argv_new;
	int 					index;
	uint8_t					tx_buf[12]   = { 0x00 };
	uint8_t					rx_buf[12]   = { 0x00 };

	if ( ( argc < 1 ) || ( NULL == argv ) )
	{ /* read all parameters */
		argv_new = &(argv[1]);
		argc_new = argc - 1;
		printf( "Calibration offset: " );
		error_code = CMD__AS_Calibration_Offset( board_id, argc_new, argv_new );
		if ( error_code < 0 ) goto CMD__AS_PARAMETER_END;
		printf( "Calibration gain: " );
		error_code = CMD__AS_Calibration_Gain( board_id, argc_new, argv_new );
		if ( error_code < 0 ) goto CMD__AS_PARAMETER_END;
		printf( "Registers:\n" );
		for ( index = 0; index < AS_ADS1259_REGISTER_QTY; index++ )
		{
			error_code = CMD__AS_RReg( board_id, 1, &(as_register_name[index]) );
			if ( error_code < 0 ) goto CMD__AS_PARAMETER_END;
		}

		printf( "Bit String Parameters:\n" );
		index = 0;
		while ( NULL != as_bit_string[index].name )
		{
			struct 	bit_string_info * 	bsd; /* short for bit string definition */
			size_t  					i;
			uint8_t						mask = 0;
			bsd = &(as_bit_string[index]);
			error_code = AS_Registers_Read( board_id, bsd->register_offset, 1, tx_buf, rx_buf );
			if ( error_code < 0 ) goto CMD__AS_PARAMETER_END;
			/* build mask */
			for ( i = 0; i < bsd->bit_width; i++ ) mask = ( mask << 1 ) | 0x01;
			rx_buf[2] = rx_buf[2] >> bsd->bit_offset;
			rx_buf[2] = rx_buf[2] & mask;
			printf( " %8s = %d\n", bsd->name, (int) rx_buf[2] );

			index++;
		}
	}
	if ( argc >= 2 )
	{ /* write cycle */
		if ( 0 == strcmpi( "ofc", argv[0] )  )
		{ /* offset calibration param */
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			error_code = CMD__AS_Calibration_Offset( board_id, argc_new, argv_new );
		}
		else if ( 0 == strcmpi( "fsc", argv[0] )  )
		{ /* full-scale calibration */
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			error_code = CMD__AS_Calibration_Gain( board_id, argc_new, argv_new );
		}
		else if ( ( index = AS_Register_Name_To_Offset( argv[0] ) ) >= 0 )
		{ /* this is a register to be written */
			error_code = CMD__AS_Reg_Write_By_Name( board_id, argc, argv );
		}
		else if ( ( index = Bit_String_Index_By_Name( as_bit_string, argv[0] ) ) >= 0 )
		{ /* this is a bit string to be written */
			struct 	bit_string_info * 	bsd; /* short for bit string definition */
			size_t  					temp;
			uint8_t						mask = 0;
			bsd = &(as_bit_string[index]);
			error_code = AS_Registers_Read( board_id, bsd->register_offset, 1, tx_buf, rx_buf );
			/* build mask */
			for ( temp = 0; temp < bsd->bit_width; temp++ ) mask = ( mask << 1 ) | 0x01;
			temp = (size_t) strtol( argv[1], NULL, 0 );
			temp = temp & mask;
			temp = temp << bsd->bit_offset;
			rx_buf[2] = rx_buf[2] & ~( mask << bsd->bit_offset );
			tx_buf[2] = rx_buf[2] | ( (uint8_t) temp );
			error_code = AS_Registers_Write( board_id, bsd->register_offset, 1, tx_buf, rx_buf );
		}
		else
		{
			error_code = -EC_NOT_FOUND;
		}
	}
	else
	{ /* read cycle */
		if ( 0 == strcmpi( "ofc", argv[0] )  )
		{ /* offset calibration param */
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			error_code = CMD__AS_Calibration_Offset( board_id, argc_new, argv_new );
		}
		else if ( 0 == strcmpi( "fsc", argv[0] )  )
		{ /* full-scale calibration */
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			error_code = CMD__AS_Calibration_Gain( board_id, argc_new, argv_new );
		}
		else if ( ( index = AS_Register_Name_To_Offset( argv[0] ) ) >= 0 )
		{ /* this is a register to be read */
			error_code = CMD__AS_RReg( board_id, argc, argv );
		}
		else if ( ( index = Bit_String_Index_By_Name( as_bit_string, argv[0] ) ) >= 0 )
		{ /* this is a bit string to be read */
			struct 	bit_string_info * 	bsd; /* short for bit string definition */
			size_t  					i;
			uint8_t						mask = 0;
			bsd = &(as_bit_string[index]);
			error_code = AS_Registers_Read( board_id, bsd->register_offset, 1, tx_buf, rx_buf );
			if ( error_code < 0 ) goto CMD__AS_PARAMETER_END;
			/* build mask */
			for ( i = 0; i < bsd->bit_width; i++ ) mask = ( mask << 1 ) | 0x01;
			rx_buf[2] = rx_buf[2] >> bsd->bit_offset;
			rx_buf[2] = rx_buf[2] & mask;
			printf( "%8s = %d\n", bsd->name, (int) rx_buf[2] );
		}
		else
		{
			error_code = -EC_NOT_FOUND;
		}
	}
CMD__AS_PARAMETER_END:
	return error_code;
}

/**********************************************************************************************************//**
 *
 * @brief
 */
static struct command_line_board  cmd_as[] =
{
	{	NULL,	CMD__AS_Wakeup, 			"wakeup",		"ADS1259 exit sleep mode"									},
	{	NULL,	CMD__AS_Sleep,				"sleep",		"ADS1259 enter sleep mode"									},
	{	NULL,	CMD__AS_Reset,				"reset",		"ADS1259 register and filter reset"							},
	{	NULL,	CMD__AS_Start,				"start",		"ADS1259 start conversion(s)" 								},
	{	NULL,	CMD__AS_Stop,				"stop",			"ADS1259 stop conversions"		 							},
	{	NULL,	CMD__AS_Rdatac,				"rdatac",		"ADS1259 set read data continuous mode"						},
	{	NULL,	CMD__AS_Sdatac,				"sdatac",		"ADS1259 stop read data continuous mode"					},
	{	NULL,	CMD__AS_Rdata,				"rdata",		"ADS1259 read data in stop continuous mode"					},
	{	NULL,	CMD__AS_RReg,				"rreg",			"ADS1259 read N registers, rreg <address> <count>"			},
	{	NULL,	CMD__AS_WReg,				"wreg",			"ADS1259 write N registers, wreg <adress> <value> [<value>]"},
	{	NULL,	CMD__AS_Ofscal,				"ofscal",		"ADS1259 perform offset calibration, apply zero input"		},
	{	NULL,	CMD__AS_Gancal,				"gancal",		"ADS1259 perform full-scale calibration, apply full-scale"  },
	{	NULL,	CMD__AS_Reg_Write_By_Name,	"wrn",			"Write one or more registers by name, wrn <name> <value>"	},
	{	NULL,	CMD__AS_Calibration_Gain,	"fsc",			"read/write fullscale calibration"							},
	{	NULL,	CMD__AS_Calibration_Offset,	"ofc",			"read/write offset calibration"								},
	{	NULL,	CMD__AS_Register_Save,		"rsave",		"save all registers to a file"								},
	{	NULL,	CMD__AS_Register_Load,		"rload",		"restore all registers from file"							},
	{	NULL,	CMD__AS_Parameter,			"param",		"read/write parameter"										},
	{	NULL,	NULL,						NULL,			NULL														},
};
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_Analog_Stick( int board_id, int argc, char* argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;

    error_code = -EC_SYNTAX;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	index = 0;
	while ( NULL != cmd_as[index].cmd_fnc )
	{
		if ( 0 == strcmpi( cmd_as[index].name, argv[0] )  )
		{
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			if ( 0 == argc_new ) argv_new = NULL;

			{ /* set trace active if schedule to start upon Analog Stick transactions */
				struct board_dataset *  dataset;
				dataset = ( struct board_dataset * )  board_definition[board_id].dataset;
				if ( IO_TRACE_START_AS == dataset->trace.start )
				{
					dataset->trace.active = true;
				}
			}

			error_code = (* cmd_as[index].cmd_fnc )( board_id, argc_new, argv_new );
			break;
		}
		index++;
	}
	return error_code;
}
/**********************************************************************************************************/

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  F R A M  C O M M A N D   F U N C T I O N S  > > > > 				                          */

/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_Dump( int board_id, int argc, char * argv[] )
{ /* idi fram dump <address> <length>  */
	uint16_t address;
	uint16_t length;

	if ( argc < 1 ) return -EC_PARAMETER;

	address = (uint16_t) strtol( argv[0], NULL, 0 );
	if ( address > ( FRAM_DENSITY_BYTES - 1 ) ) return -EC_FRAM_ADDRESS_RANGE;

	if ( argc < 2 ) length = HEX_DUMP_BYTES_PER_LINE;
	else		    length  = (uint16_t) strtol( argv[1], NULL, 0 );

	if ( ( address + length ) > FRAM_DENSITY_BYTES ) length = FRAM_DENSITY_BYTES - address;

	return FRAM_Report( board_id, address, length, stdout );
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_Save( int board_id, int argc, char * argv[] )
{ /* idi fram save <address> <length> <destination_file> */
	int			error_code;
	uint16_t 	address;
	size_t 		length;
	FILE *   	out;

	if ( argc < 3 ) return -EC_PARAMETER;

	address = (uint16_t) strtol( argv[0], NULL, 0 );
	if ( address > ( FRAM_DENSITY_BYTES - 1 ) ) return -EC_FRAM_ADDRESS_RANGE;

	length  = (uint16_t) strtol( argv[1], NULL, 0 );
	if ( (address + length - 1) > ( FRAM_DENSITY_BYTES - 1 ) ) return -EC_FRAM_ADDRESS_RANGE;
	out = fopen( argv[2], "w" );
	error_code = FRAM_Memory_To_File( board_id, address, length, out );
	fclose( out );
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_Load( int board_id, int argc, char * argv[] )
{ /* idi fram load <address> <source_file> */
	int			error_code;
	uint16_t 	address;
//	uint16_t 	length;
	FILE *   	out;

	if ( argc < 2 ) return -EC_PARAMETER;

	address = (uint16_t) strtol( argv[0], NULL, 0 );
	if ( address > ( FRAM_DENSITY_BYTES - 1 ) ) return -EC_FRAM_ADDRESS_RANGE;
	out = fopen( argv[2], "r" );
	error_code = FRAM_File_To_Memory( board_id, address, 0 /* no length specified */, out );
	fclose( out );
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_Write( int board_id, int argc, char * argv[] )
{ /* idi fram load <address> <source_file> */
	int			error_code;
	size_t		count;
	uint16_t 	address;
	struct board_dataset *  dataset;
	dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

	if ( argc < 2 ) return -EC_PARAMETER;

	address = (uint16_t) strtol( argv[0], NULL, 0 );
	if ( address > ( FRAM_DENSITY_BYTES - 1 ) ) return -EC_FRAM_ADDRESS_RANGE;

	error_code = CMD__SPI_Data_Interpreter( 	1, /* starting argument */
												argc,
												argv,
												FRAM_BLOCK_SIZE,
												&count,
												dataset->spi.fram_block
											  );
	if ( SUCCESS != error_code ) goto CMD__FRAM_Write_Error_Exit;
	error_code = FRAM__Memory_Write( board_id, address, count, dataset->spi.fram_block );
CMD__FRAM_Write_Error_Exit:
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_Init( int board_id, int argc, char * argv[] )
{ /*  idi fram init <pattern list: 0x55 0x33 '3' '5' 'q' >	*/
	int			error_code;
//	int			index;
	size_t		tx_size_in_bytes;
	struct board_dataset *  dataset;
	dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

	//uint8_t		buf[16];
	if ( argc < 1 )
	{ /* initialize all zeros */
		error_code = FRAM_Set( board_id, 0, NULL );
	}
	else
	{
		CMD__SPI_Data_Interpreter(		0, 					/* arg_start */
										argc,
										argv,
										FRAM_BLOCK_SIZE,
										&tx_size_in_bytes,
										dataset->spi.fram_block
									 );
//		if ( argc > 16 ) argc = 16;
#if defined( IDI_FRAM_PRINT_DEBUG )
//just report the first 16 bytes.
{
	size_t display_count;

	if ( tx_size_in_bytes > HEX_DUMP_BYTES_PER_LINE ) display_count = HEX_DUMP_BYTES_PER_LINE;
	else                                              display_count = tx_size_in_bytes;

	Hex_Dump_Line( 0, display_count, dataset->spi.fram_block, stdout );
}
#endif
		error_code = FRAM_Set( board_id, tx_size_in_bytes, dataset->spi.fram_block );
	}
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_WREN( int board_id, int argc, char * argv[] )
{ /* idi fram WREN */
	(void) 		argc;
	(void) 		argv;
	return FRAM__Write_Enable_Latch_Set( board_id );
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_WRDI( int board_id, int argc, char * argv[] )
{ /*  idi fram WRDI	 */
	(void) 		argc;
	(void) 		argv;
	return FRAM__Write_Disable( board_id );
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_RDSR( int board_id, int argc, char * argv[] )
{ /* idi fram RDSR  */
	int 		error_code;
	uint8_t 	status;
	(void) 		argc;
	(void) 		argv;
	error_code = FRAM__Read_Status_Register( board_id, &status );
	printf( "FRAM STATUS: 0x%02X\n", ((int) status) );
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_WRSR( int board_id, int argc, char * argv[] )
{ /*  idi fram WRSR <value>	 */
//	int 	error_code;
	uint8_t status;

	if ( argc < 1 ) return -EC_PARAMETER;
	status = (uint8_t) strtol( argv[0], NULL, 0 );
	return FRAM__Write_Status_Register( board_id, status );
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__FRAM_RDID( int board_id, int argc, char * argv[] )
{ /*  idi fram RDID	 */
	uint32_t 	id;
	(void) 		argc;
	(void) 		argv;
	FRAM__Read_ID( board_id, &id );
#if defined( __MSDOS__ )
	printf( "FRAM ID: 0x%08lX\n", id );
#else
	printf( "FRAM ID: 0x%08X\n", id );
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 */
static struct command_line_board  cmd_fram[] =
{
	{	NULL,	CMD__FRAM_Dump, 	"dump",		"params: <address> <length>"							},
	{	NULL,	CMD__FRAM_Save,		"save",		"params: <address> <length> <binary destination file>"	},
	{	NULL,	CMD__FRAM_Load,		"load",		"params: <address> <binary source file name>"			},
	{	NULL,	CMD__FRAM_Write,	"write",	"params: <address> <data list: bytes/characters/strings>" },
	{	NULL,	CMD__FRAM_Init,		"init",		"params: [byte/character] [byte/character] ..."		 	},
	{	NULL,	CMD__FRAM_WREN,		"wren",		"WRite Enable Latch Set"								},
	{	NULL,	CMD__FRAM_WRDI,		"wrdi",		"WRite DIsable"											},
	{	NULL,	CMD__FRAM_RDSR,		"rdsr",		"ReaD Status Register"									},
	{	NULL,	CMD__FRAM_WRSR,		"wrsr",		"WRite Status Register.  Params: <status>"				},
	{	NULL,	CMD__FRAM_RDID,		"rdid",		"ReaD ID Register"										},
	{	NULL,	NULL,				NULL,		NULL													},
};
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_FRAM( int board_id, int argc, char* argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;

    error_code = -EC_SYNTAX;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	index = 0;
	while ( NULL != cmd_fram[index].cmd_fnc )
	{
		if ( 0 == strcmpi( cmd_fram[index].name, argv[0] )  )
		{
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			if ( 0 == argc_new ) argv_new = NULL;

			{ /* set trace active if schedule to start upon FRAM transactions */
				struct board_dataset *  dataset;
				dataset = ( struct board_dataset * )  board_definition[board_id].dataset;
				if ( IO_TRACE_START_FRAM == dataset->trace.start )
				{
					dataset->trace.active = true;
				}
			}

			error_code = (* cmd_fram[index].cmd_fnc )( board_id, argc_new, argv_new );
			break;
		}
		index++;
	}
	return error_code;
}
/**********************************************************************************************************/

typedef enum
{
	TEST_STATE_INIT 		= 0x000,
	TEST_STATE_READY		= 0x100,
	TEST_STATE_ACTIVE		= 0x200,
	TEST_STATE_COMPLETE		= 0xF00,
	TEST_STATE_QUIT_ERROR	= 0xF10,
	TEST_STATE_QUIT_USER	= 0xF20,
	TEST_STATE_QUIT			= 0xFFF
} TEST_STATE_ENUM;


/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  D I G I T A L  O U T P U T   C O M M A N D   F U N C T I O N S  > > > > 	                  */


/**********************************************************************************************************//**
 *
 * @brief
 * Obtains the digital output component (or board ID in this case) ID number.
 *
 * @param[out] id  The 16-bit ID number
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IDO_DOUT_ID_Get( int board_id, uint16_t * id )
{
	uint8_t   	lsb, msb;
	struct board_dataset *  dataset;
	dataset = (struct board_dataset *) board_definition[board_id].dataset;

	if ( MODE_JUMPERS_0 == dataset->mode_jumpers )
	{ /* we are in legacy mode */
		return -EC_MODE_LEGACY;
	}

	IO_Read_U8( board_id, __LINE__, IDO_ID_LSB, &lsb );
	IO_Read_U8( board_id, __LINE__, IDO_ID_MSB, &msb );
	*id = ( ((uint16_t) msb) << 8 ) | ((uint16_t) lsb);
#if defined( ID_ALWAYS_REPORT_AS_GOOD )
	*id = ID_DOUT;
#endif
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Determines if the DIN component and/or board is present.  Returns true if not present
 * (i.e. error).
 *
 * @return A zero is returned if present, otherwise a 1 is returned.
 */
BOOL IDO_DOUT_IsNotPresent( int board_id )
{
	uint16_t id;
	IDO_DOUT_ID_Get( board_id, &id );
	if (ID_DOUT == id ) return 0;
	return 1;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Obtains and reports a single digital output channel.
 *
 * @param[in]  channel channel to be read out.
 * @param[out] value   Pointer to the boolean value to be set based on the digital input value
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IDO_DO_Channel_Get( int board_id, size_t channel, BOOL * value )
{
	size_t 	group;
	size_t 	bit;
	uint8_t reg_value;

	group = channel >> IDO_DO_SHIFT_RIGHT;
	bit   = channel - group * IDO_DO_GROUP_SIZE;

	IO_Read_U8( board_id, __LINE__, IDO_DO_GROUP0 + group /* channel */, &reg_value );

	if ( 0 != ( reg_value & ( 0x01 << bit ) ) ) *value = true;
	else                                        *value = false;

	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Sets a single digital output channel.
 *
 * @param[in] channel to be written.
 * @param[in] value
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IDO_DO_Channel_Set( int board_id, size_t channel, BOOL value )
{
	size_t 	group;
	size_t 	bit;
	uint8_t reg_value;
	uint8_t mask;

	group = channel >> IDO_DO_SHIFT_RIGHT;
	bit   = channel - group * IDO_DO_GROUP_SIZE;

	IO_Read_U8( board_id, __LINE__, IDO_DO_GROUP0 + group /* channel */, &reg_value );
	mask = 0x01 << bit;
	if ( true == value ) reg_value |=  mask;
	else				 reg_value &= ~mask;

	IO_Write_U8( board_id, __LINE__, IDO_DO_GROUP0 + group /* channel */, reg_value );
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Reads the selected digital output port (8-bits).
 *
 * @param[in]  group  the group, range is 0 to 5.
 * @param[out] value  pointer to the destination for the data read out
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IDO_DO_Group_Get( int board_id, size_t group, uint8_t * value )
{
	IO_Read_U8( board_id, __LINE__, IDO_DO_GROUP0 + group /* channel */, value );
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Reads the selected digital output port (8-bits).
 *
 * @param[in]  group  the group, range is 0 to 5.
 * @param[in]  value  pointer to the destination for the data read out
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int IDO_DO_Group_Set( int board_id, size_t group, uint8_t value )
{
	IO_Write_U8( board_id, __LINE__, IDO_DO_GROUP0 + group /* channel */, value );
	return SUCCESS;
}

/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int Print_Byte_List(	const char * prefix_string,	/* beginning of line output 				*/
							int			 argc,			/* mode list count							*/
							char ** 	 argv,			/* "binary", "group" or "hex" is valid  	*/
							size_t 		 list_count, 	/* number of bytes in the list				*/
							uint8_t * 	 list, 			/* list of bytes to be output/displayed		*/
							FILE * 		 out			/* destination of the list					*/
						  )
{
	enum { MODE_NONE = 0, MODE_BINARY = 1, MODE_HEX = 2, MODE_ALL = 3 } mode_out;
	int		group;

	mode_out = MODE_ALL;
	if ( NULL == argv )
	{
		mode_out = MODE_ALL;
	}
	else
	{
		int index;
		for ( index = 0; index < argc; index++ )
		{
			if      ( 0    == strcmpi( "binary", argv[index] ) ) mode_out |= MODE_BINARY;
			else if ( 0    == strcmpi( "group",  argv[index] ) ) mode_out |= MODE_HEX;
			else if ( 0    == strcmpi( "hex",    argv[index] ) ) mode_out |= MODE_HEX;
			else                                                 mode_out |= MODE_ALL;
		}
	}

	if ( MODE_BINARY == ( mode_out & MODE_BINARY ) )
	{ /* build in binary format */
		int		cp;
		int 	channel;
		uint8_t mask;
		char  	message[64];

		cp    = 0;
		group = 0;
		for ( group = 0; group < list_count; group++ )
		{
			mask = 0x01;
			for ( channel = 0; channel < 8; channel++ )
			{
				message[cp++] = !!(list[group] & mask) ? '1' : '0';
				mask = mask << 1;
			}
			message[cp++] = ' ';
		}
		message[cp] = '\0';
		fprintf( out, "%s: %s\n", prefix_string, message );
	}

	if ( MODE_HEX == ( mode_out & MODE_HEX ) )
	{
		fprintf( out, "%s:", prefix_string );
		for ( group = 0; group < list_count; group++ ) fprintf( out, " %02X", list[group] );
		fprintf( out, "\n" );
	}

	return SUCCESS;
}

#if ( IDO_DO_GROUP_QTY == IDI_DIN_GROUP_QTY )
/* IDO-IDI LOOPBACK - BEGIN */

/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int IDO48_IDI48_Loopback_Test( int di_id, int do_id, uint8_t * di_list, uint8_t * do_list, size_t count )
{
	int group;
	for( group = 0; group < count; group++ )
	{
		IDO_DO_Group_Get(  do_id, group, &(do_list[group]) );
		IDI_DIN_Group_Get( di_id, group, &(di_list[group]) );
		if ( di_list[group] != do_list[group] ) return -group;
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static int CMD__IDO48_IDI48_Loopback( int board_id, int argc, char * argv[] )
{
	int				error_code;
	struct  ido_dataset *  dataset;
	FILE * 			out;
	char * 			file_name;
	TEST_STATE_ENUM state;
	size_t			group;
	size_t			channel;
	BOOL			di_value;
	BOOL			do_value;
#if defined( __MSDOS__ )
	size_t			wait_ms;
#endif
	uint8_t			do_list[IDO_DO_GROUP_QTY];
	uint8_t			di_list[IDI_DIN_GROUP_QTY];
	uint8_t			dog[IDO_DO_GROUP_QTY];
	char * 			mode_string[] = { "binary" };
	char 			out_default[] = { "stdout" };
	char *			msg; /* Short for message string.  Pointer to a string. */

	state = TEST_STATE_INIT;
	dataset = NULL;
	switch( board_id )
	{
		case ID_IDO48:
			dataset = ( struct ido_dataset * )  board_definition[board_id].dataset;
			break;
		default:
			return -EC_BOARD_TYPE;
	}

#if defined( __MSDOS__ )
	wait_ms = 100; /* msec - should be plenty of time for signal to propagate */
#endif
	out       = stdout;
	file_name = out_default;
	if ( argc > 0 )
	{
		size_t  wi;		/* short for word index        */
		char 	open_method[4];
		wi = 0;
		strcpy( open_method, "wt" );
		if (  ( 0 == strcmpi( "--a",      argv[wi] ) )  ||
			  ( 0 == strcmpi( "--append", argv[wi] ) )   )
		{
			wi++;
			strcpy( open_method, "at" );
		}

		if ( argc > wi )
		{
			out = fopen( argv[wi], open_method );
			file_name = argv[wi];
		}
	}

/* NOTES:
 * 1. snprintf() is not available in Borland DOS compiler
 * 2. sprintf() must be paired with a Print_Multiple() function.
 */

	msg = &(dataset->message[0]);
	sprintf( msg, "======================================= " );
	Print_Multiple( msg, out );
	Time_Current_String( msg, MESSAGE_SIZE );
	Print_Multiple( msg, out );

	sprintf( msg, "\nIDO48-IDI48 loopback test: %s", file_name );
	Print_Multiple( msg, out );

	/* initialize all to off */
	for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
	{
		do_list[group] = 0x00; /* all off */
		IDO_DO_Group_Set(  board_id, group, do_list[group]    );
#if defined( __MSDOS__ )
		delay( wait_ms );
#endif
		IDI_DIN_Group_Get( ID_IDI48, group, &(di_list[group]) );
	}
	error_code =  IDO48_IDI48_Loopback_Test( ID_IDI48, board_id, di_list, do_list, IDO_DO_GROUP_QTY );
	if ( error_code < 0 )
	{
		state = TEST_STATE_QUIT;
	}

	state = TEST_STATE_ACTIVE;
	channel = 0;
	while ( TEST_STATE_ACTIVE == state )
	{

		/* set channel */
		do_value = true;
		IDO_DO_Channel_Set(  board_id, channel, do_value  );
#if defined( __MSDOS__ )
		delay( wait_ms );
#endif
		IDI_DIN_Channel_Get( ID_IDI48, channel, &di_value );

		if ( do_value != di_value )
		{
			state = TEST_STATE_QUIT_ERROR;
		}
		error_code =  IDO48_IDI48_Loopback_Test( ID_IDI48, board_id, di_list, do_list, IDO_DO_GROUP_QTY );
		if ( error_code < 0 )
		{
			state = TEST_STATE_QUIT_ERROR;
		}

		/* clear channel */
		do_value = false;
		IDO_DO_Channel_Set(  board_id, channel, do_value  );
#if defined( __MSDOS__ )
		delay( wait_ms );
#endif
		IDI_DIN_Channel_Get( ID_IDI48, channel, &di_value );

		if ( do_value != di_value )
		{
			state = TEST_STATE_QUIT_ERROR;
		}
		error_code =  IDO48_IDI48_Loopback_Test( ID_IDI48, board_id, di_list, do_list, IDO_DO_GROUP_QTY );
		if ( error_code < 0 )
		{
			state = TEST_STATE_QUIT_ERROR;
		}


		{ /* set pattern all */
			size_t index;
			uint8_t	do_group[IDO_DO_GROUP_QTY];

			for ( index = 0; index < IDO_DO_GROUP_QTY; index++ )
			{
				do_group[index] = 0xFF;
				IDO_DO_Group_Set( board_id, index, do_group[index] );
			}
	#if defined( __MSDOS__ )
		delay( wait_ms );
	#endif
			error_code =  IDO48_IDI48_Loopback_Test( ID_IDI48, board_id, di_list, do_list, IDO_DO_GROUP_QTY );
			if ( error_code < 0 )
			{
				sprintf( msg, "All high fail" );
				Print_Multiple( msg, out );
				state = TEST_STATE_QUIT_ERROR;
			}
			else
			{
				for ( index = 0; index < IDO_DO_GROUP_QTY; index++ )
				{
					do_group[index] = 0x00;
					IDO_DO_Group_Set( board_id, index, do_group[index] );
				}
			}
		}


		if ( true == Character_Get( NULL ) ) state = TEST_STATE_QUIT_USER;

		if ( TEST_STATE_ACTIVE == state )
		{
			channel++;
			sprintf( msg, "." );
			Print_Multiple( msg, out );
			if ( channel >= IDO_DO_QTY ) state = TEST_STATE_COMPLETE;
		}
	}


	sprintf( msg, "[PATTERN_INVERT]" );
	Print_Multiple( msg, out );
	channel = 0;
	if ( TEST_STATE_COMPLETE == state )  state = TEST_STATE_ACTIVE;
	while ( TEST_STATE_ACTIVE == state )
	{
		{ /* set pattern all */
			size_t index;

			for ( index = 0; index < IDO_DO_GROUP_QTY; index++ )
			{
				IDO_DO_Group_Set( board_id, index, 0xFF );
			}
	#if defined( __MSDOS__ )
		delay( wait_ms );
	#endif
			error_code =  IDO48_IDI48_Loopback_Test( ID_IDI48, board_id, di_list, do_list, IDO_DO_GROUP_QTY );
			if ( error_code < 0 )
			{
				sprintf( msg, "All high fail" );
				Print_Multiple( msg, out );
				state = TEST_STATE_QUIT_ERROR;
			}
		}

		/* clear channel */
		do_value = false;
		IDO_DO_Channel_Set(  board_id, channel, do_value  );
#if defined( __MSDOS__ )
		delay( wait_ms );
#endif
		IDI_DIN_Channel_Get( ID_IDI48, channel, &di_value );

		if ( do_value != di_value )
		{
			state = TEST_STATE_QUIT_ERROR;
		}
		error_code =  IDO48_IDI48_Loopback_Test( ID_IDI48, board_id, di_list, do_list, IDO_DO_GROUP_QTY );
		if ( error_code < 0 )
		{
			state = TEST_STATE_QUIT_ERROR;
		}

		/* set channel */
		do_value = true;
		IDO_DO_Channel_Set(  board_id, channel, do_value  );
#if defined( __MSDOS__ )
		delay( wait_ms );
#endif
		IDI_DIN_Channel_Get( ID_IDI48, channel, &di_value );

		if ( do_value != di_value )
		{
			state = TEST_STATE_QUIT_ERROR;
		}
		error_code =  IDO48_IDI48_Loopback_Test( ID_IDI48, board_id, di_list, do_list, IDO_DO_GROUP_QTY );
		if ( error_code < 0 )
		{
			state = TEST_STATE_QUIT_ERROR;
		}

		if ( true == Character_Get( NULL ) ) state = TEST_STATE_QUIT_USER;

		if ( TEST_STATE_ACTIVE == state )
		{
			channel++;
			sprintf( msg, "." );
			Print_Multiple( msg, out );
			if ( channel >= IDO_DO_QTY ) state = TEST_STATE_COMPLETE;
		}
	}


	{ /* clear all */
		size_t index;
		for ( index = 0; index < IDO_DO_GROUP_QTY; index++ )
		{
			IDO_DO_Group_Set( board_id, index, 0x00 );
		}
#if defined( __MSDOS__ )
	delay( wait_ms );
#endif
	}


	sprintf( msg, "\n" );
	Print_Multiple( msg, out );
	switch( state )
	{
		case TEST_STATE_COMPLETE:
			sprintf( msg, "PASS\n" );
			Print_Multiple( msg, out );
			error_code = SUCCESS;
			break;
		case TEST_STATE_QUIT_ERROR:
			sprintf( msg, "FAIL at channel %d\n", (int) channel );
			Print_Multiple( msg, out );
			error_code = EC_TEST_FAIL;
			/* print it out */
			for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
			{
				IDO_DO_Group_Get( board_id, group, &(dog[group]) );
			}
			Print_Byte_List( "DO:",				/* prefix 										*/
							 1,					/* mode list count								*/
							 mode_string,		/* mode list									*/
							 IDO_DO_GROUP_QTY,	/* size or number of elements in the list/array */
							 dog,				/* list	or array								*/
							 stdout				/* output to terminal							*/
						   );

			if ( out != stdout )
			{
				Print_Byte_List( "DO:",				/* prefix 										*/
								 1,					/* mode list count								*/
								 mode_string,		/* mode list									*/
								 IDO_DO_GROUP_QTY,	/* size or number of elements in the list/array */
								 dog,				/* list	or array								*/
								 out				/* output to terminal							*/
							   );
			}
			break;
		case TEST_STATE_QUIT_USER:
			sprintf( msg, "CANCEL BY USER at channel %d\n", (int) channel );
			Print_Multiple( msg, out );
			error_code = EC_TEST_FAIL;
			break;
		default: /* other states ignored */
			break;
	}
	if ( out != stdout ) fclose( out );
	return error_code;
}
/* IDO-IDI LOOPBACK - END */
#endif
/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int CMD__IDO48_DOUT_Test_Alternate( int board_id, int argc, char * argv[] )
{
	TEST_STATE_ENUM state;
	size_t			group;
#if defined( __MSDOS__ )
	size_t			delay_normal_ms;
	size_t			delay_inverted_ms;
#endif
	uint8_t			pattern;
	uint8_t			dog[IDO_DO_GROUP_QTY];
	char *			mode_string[] = { "binary" };

	state = TEST_STATE_INIT;
#if defined( __MSDOS__ )
	delay_normal_ms  = 100;			/* default */
	delay_inverted_ms =  20;			/* default */
#endif
	pattern		 = 0x55;		/* default */

	if ( argc > 0 )
	{
		pattern = (uint8_t) strtol( argv[0], NULL, 0 );
	}

	if ( argc > 1 )
	{
		if ( 0 == strcmpi( "time", argv[1] ) )
		{
			if      ( argc > 2 )
			{
#if defined( __MSDOS__ )
				delay_normal_ms = (size_t) strtol( argv[2], NULL, 0 );
#endif
			}
			else if ( argc > 3 )
			{
#if defined( __MSDOS__ )
				delay_inverted_ms = (size_t) strtol( argv[3], NULL, 0 );
#endif
			}
		}
	}

	/* initialize all to off */
	for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
	{
		IDO_DO_Group_Set( board_id, group, 0x00 );
		dog[group] = 0x00;
	}

	state = TEST_STATE_ACTIVE;
	printf( "IDO48 Alternating sequence testing (press any key to quit):" );
	while ( TEST_STATE_ACTIVE == state )
	{   /* set all groups normal */
		for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
		{
			IDO_DO_Group_Set( board_id, group, pattern );
			dog[group] = pattern;
		}
		/* print it out */
		for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
		{
			IDO_DO_Group_Get( board_id, group, &(dog[group]) );
		}
		Print_Byte_List( "DO:",				/* prefix 										*/
						 1,					/* mode list count								*/
						 mode_string,		/* mode list									*/
						 IDO_DO_GROUP_QTY,	/* size or number of elements in the list/array */
						 dog,				/* list	or array								*/
						 stdout				/* output to terminal							*/
					   );
#if defined( __MSDOS__ )
		delay( delay_normal_ms );
#endif
		/* set all groups inverted */
		for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
		{
			IDO_DO_Group_Set( board_id, group, ~pattern );
			dog[group] = ~pattern;
		}
		/* print it out */
		for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
		{
			IDO_DO_Group_Get( board_id, group, &(dog[group]) );
		}
		Print_Byte_List( "DO:",				/* prefix 										*/
						 1,					/* mode list count								*/
						 mode_string,		/* mode list									*/
						 IDO_DO_GROUP_QTY,	/* size or number of elements in the list/array */
						 dog,				/* list	or array								*/
						 stdout				/* output to terminal							*/
					   );
#if defined( __MSDOS__ )
		delay( delay_inverted_ms );
#endif
		if ( true == Character_Get( NULL ) ) state = TEST_STATE_QUIT_USER;
	}

	/* initialize all to off */
	for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
	{
		IDO_DO_Group_Set( board_id, group, 0x00 /* all off */ );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int CMD__IDO48_DOUT_Test_One_Hot( int board_id, int argc, char * argv[] )
{
	TEST_STATE_ENUM state;
	size_t			group;
	size_t			channel;
#if defined( __MSDOS__ )
	size_t			delay_on_ms;
	size_t			delay_off_ms;
#endif
	uint8_t			dog[IDO_DO_GROUP_QTY];
	char *			mode_string[] = { "binary" };

	state = TEST_STATE_INIT;
#if defined( __MSDOS__ )
	delay_on_ms  = 100;			/* default */
	delay_off_ms =  20;			/* default */
#endif
	if ( argc > 0 )
	{
#if defined( __MSDOS__ )
		delay_on_ms = (size_t) strtol( argv[0], NULL, 0 );
#endif
	}
	else if ( argc > 1 )
	{
#if defined( __MSDOS__ )
		delay_off_ms = (size_t) strtol( argv[1], NULL, 0 );
#endif
	}

	/* initialize all to off */
	for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
	{
		dog[group] = 0x00; /* all off */
		IDO_DO_Group_Set( board_id, group, dog[group] );
	}

	state = TEST_STATE_ACTIVE;
	printf( "IDO48 One-hot sequence testing (press any key to quit):" );
	channel = 0;
	while ( TEST_STATE_ACTIVE == state )
	{   /* set channel */
		IDO_DO_Channel_Set( board_id, channel, true );
		/* print it out */
		for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
		{
			IDO_DO_Group_Get( board_id, group, &(dog[group]) );
		}
		Print_Byte_List( "DO:",				/* prefix 										*/
						 1,					/* mode list count								*/
						 mode_string,		/* mode list									*/
						 IDO_DO_GROUP_QTY,	/* size or number of elements in the list/array */
						 dog,				/* list	or array								*/
						 stdout				/* output to terminal							*/
					   );
#if defined( __MSDOS__ )
		delay( delay_on_ms );
#endif
		/* clear channel */
		IDO_DO_Channel_Set( board_id, channel, false );
#if defined( __MSDOS__ )
		delay( delay_off_ms );
#endif
		/* print it out */
		for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
		{
			IDO_DO_Group_Get( board_id, group, &(dog[group]) );
		}
		Print_Byte_List( "DO:",				/* prefix 										*/
						 1,					/* mode list count								*/
						 mode_string,		/* mode list									*/
						 IDO_DO_GROUP_QTY,	/* size or number of elements in the list/array */
						 dog,				/* list	or array								*/
						 stdout				/* output to terminal							*/
					   );

		if ( true == Character_Get( NULL ) ) state = TEST_STATE_QUIT_USER;
		channel++;
		if ( channel >= IDO_DO_QTY ) channel = 0;
	}

	/* initialize all to off */
	for( group = 0; group < IDO_DO_GROUP_QTY; group++ )
	{
		IDO_DO_Group_Set( board_id, group, 0x00 /* all off */ );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static struct command_line_board  cmd_ido48_test[] =
{
	{	NULL,	CMD__IDO48_DOUT_Test_One_Hot,		"onehot",		"toggling one at a time forever"			},
	{	NULL,	CMD__IDO48_DOUT_Test_Alternate,		"alternate",	"alternate pattern at all ports forever"	},
#if ( IDO_DO_GROUP_QTY == IDI_DIN_GROUP_QTY )
	{	NULL,	CMD__IDO48_IDI48_Loopback,			"loopback",		"IDI48 & IDO48 Loopback test"				},
#endif
	{	NULL,	NULL,								NULL,			NULL										},
};
/**********************************************************************************************************//**
 * @brief
 * Cycles through all channels one at a time in a one-hot situation.
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int CMD__IDO48_Test( int board_id, int argc, char * argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;
    char *      endptr;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	error_code = -EC_SYNTAX;

	strtol( argv[0], &endptr, 0 ); /* just want to know where it fails */
	if ( argv[0] != endptr )
	{  /* assume channel number */
		error_code = (* cmd_ido48_test[0].cmd_fnc )( board_id, argc, argv );
	}
	else
	{ /* otherwise a normal command */
		index = 0;
		while ( NULL != cmd_ido48_test[index].cmd_fnc )
		{
			if ( 0 == strcmpi( cmd_ido48_test[index].name, argv[0] )  )
			{
				argv_new = &(argv[1]);
				argc_new = argc - 1;
				if ( 0 == argc_new ) argv_new = NULL;
				error_code = (* cmd_ido48_test[index].cmd_fnc )( board_id, argc_new, argv_new );
				break;
			}
			index++;
		}
	}
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IDO48_DO_ID( int board_id, int argc, char * argv[] )
{
	uint16_t 	id;
	(void) 		argc;
	(void) 		argv;
	IDO_DOUT_ID_Get( board_id, &id );
	printf( "IDO48 DOUT ID: 0x%04X\n", id );
	return SUCCESS;
}/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IDO48_DO_All( int board_id, int argc, char * argv[] )
{ /* do all [<binary/hex/group>]  */
	int 	error_code;		       /* used primarily for debug purposes */
	size_t	group;
	uint8_t do_grp[6];
	//uint8_t mask;

	/* collect up the data */
	for ( group = 0; group < IDO_DO_GROUP_QTY; group++ )
	{
		error_code = IDO_DO_Group_Get( board_id, group, &(do_grp[group]) );
	}
	/* print it out */
	Print_Byte_List( "DO:",				/* prefix 										*/
					 argc,				/* mode list count								*/
					 argv,				/* mode list									*/
					 IDO_DO_GROUP_QTY,	/* size or number of elements in the list/array */
					 do_grp,			/* list	or array								*/
					 stdout				/* output to terminal							*/
				   );
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IDO48_DO_Channel( int board_id, int argc, char * argv[] )
{
	int 	error_code;		       /* used primarily for debug purposes */
	int 	channel;
	char  	message[8];
	BOOL	value;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	channel = (int) strtol( argv[0], NULL, 0 );

	if ( argc < 2 )
	{ /* get */
		error_code = IDO_DO_Channel_Get( board_id, channel, &value );
		message[0] = value ? '1' : '0';
		message[1] = '\0';
		printf( "DO%02d: %s\n", channel, message );
	}
	else
	{ /* set */
		value = String_To_Bool( argv[1] );
		error_code = IDO_DO_Channel_Set( board_id, channel, value );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IDO48_DO_Group( int board_id, int argc, char * argv[] )
{
	int 	error_code; /* used primarily for debugging */
	int		group;
	int		group_start;
	int		group_end;
	//int		group_count;
	BOOL	read_cycle;
	uint8_t do_grp[IDO_DO_GROUP_QTY];


	if ( argc < 1 )
	{ /* no arguments ==> read all groups */
		group_start = 0;
		group_end   = IDO_DO_GROUP_QTY - 1;
		//group_count = IDO_DO_GROUP_QTY;
		read_cycle  = true;
	}
	else if ( argc > 1 )
	{ /* at least two arguments ==> write at least one */
		group_start = (int) strtol( argv[0], NULL, 0 );
		if ( group_start >= IDO_DO_GROUP_QTY ) return -EC_CHANNEL;
		group_end   = IDO_DO_GROUP_QTY - 1;
		//group_count = 0;
		read_cycle  = false; /* writing data */
	}
	else
	{ /* one argument ==> read one */
		group_start = (int) strtol( argv[0], NULL, 0 );
		if ( group_start >= IDO_DO_GROUP_QTY ) return -EC_CHANNEL;
		group_end   = group_start + 1;
		//group_count = 1;
		read_cycle  = true;
	}

	if ( true == read_cycle )
	{ /* read cycle */
		for ( group = group_start; group < group_end; group++ )
		{
			error_code = IDO_DO_Group_Get( board_id, group, &(do_grp[group]) );
		}

		printf( "DO_GROUP:" );
		for ( group = group_start; group < group_end; group++ )
		{
			printf( " 0x%02X", ((int) do_grp[group]) );
		}
		printf( "\n" );
	}
	else
	{ /* write cycle */
		int index;
		group_end = group_start;
		group     = group_start;
		for ( index = 1; index < argc; index++ )
		{
			do_grp[group] = (int) strtol( argv[index], NULL, 0 );
			group++;
			group_end++;
			if ( group >= IDO_DO_GROUP_QTY ) break;
		}

		for ( group = group_start; group < group_end; group++ )
		{
			error_code = IDO_DO_Group_Set( board_id, group, do_grp[group] );
		}
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 */
static struct command_line_board  cmd_ido48[] =
{
	{	NULL,				CMD__IDO48_DO_ID,		"id",		"params: none.  Reports the DOUT board/component ID."	},
	{	NULL,				CMD__IDO48_DO_Channel,	"chan",		"params: <channel> <value>"								},
	{   NULL,				CMD__IDO48_DO_Group,	"group",	"params: [<group_channel | all>]"						},
	{	NULL,				CMD__IDO48_DO_All,	 	"all",		"gets all digital outputs in binary and hex"			},
	{   cmd_ido48_test,		CMD__IDO48_Test,		"test",		"semi-auto test functions"								},
	{	NULL,				NULL,					NULL,		NULL													}
};
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_IDO48( int board_id, int argc, char* argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;
    char *      endptr;
    //int			channel;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	if ( ID_IDO48 != board_id ) return -EC_BOARD_TYPE;

	error_code = -EC_SYNTAX;

	//channel = (int) strtol( argv[0], &endptr, 0 );
	strtol( argv[0], &endptr, 0 ); /* just want to know where it fails */
	if ( argv[0] != endptr )
	{  /* assume channel number */
		error_code = (* cmd_ido48[0].cmd_fnc )( board_id, argc, argv );
	}
	else
	{ /* otherwise a normal command */
		index = 0;
		while ( NULL != cmd_ido48[index].cmd_fnc )
		{
			if ( 0 == strcmpi( cmd_ido48[index].name, argv[0] )  )
			{
				argv_new = &(argv[1]);
				argc_new = argc - 1;
				if ( 0 == argc_new ) argv_new = NULL;
				error_code = (* cmd_ido48[index].cmd_fnc )( board_id, argc_new, argv_new );
				break;
			}
			index++;
		}
	}
	return error_code;
}
/**********************************************************************************************************/


/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  D I G I T A L  I N P U T   C O M M A N D   F U N C T I O N S  > > > > 	                  */


/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */

#define DIN_TEST_DEBUG_PRINT 1



/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static BOOL CMD__IDI48_DIN_Helper_All_Values_False( uint8_t * dg, size_t group_qty )
{
	size_t group;

	for ( group = 0; group < group_qty; group++ )
	{
		if ( 0x00 != dg[group] ) return false;
	}
	return true; /* all must be zero */
}


/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int CMD__IDI48_DIN_Test_Value( int board_id, int argc, char * argv[] )
{
	int 			error_code;
	TEST_STATE_ENUM state;
	size_t			group;
	size_t			bit;
	size_t			index;
	uint8_t			result;
	uint8_t			dig[IDI_DIN_GROUP_QTY];
	uint8_t			dig_prev[IDI_DIN_GROUP_QTY];
	uint8_t			group_change;
	uint8_t			mask;
	BOOL			din_cmplt[IDI_DIN_QTY];
	FILE * 			out;
	struct  idi_dataset *  dataset;
	char * 			file_name;
	char *			msg; /* Short for message string.  Pointer to a string. */
	char 			out_default[] = { "stdout" };
	(void)			argc;
	(void)  		argv;

	state = TEST_STATE_INIT;

	dataset = NULL;
	switch( board_id )
	{
		case ID_IDI48:
			dataset = ( struct idi_dataset * )  board_definition[board_id].dataset;
			break;
		default:
			return -EC_BOARD_TYPE;
	}


	msg   = &(dataset->message[0]);

	out       = stdout;
	file_name = out_default;
	if ( argc > 0 )
	{
		size_t  wi;		/* short for word index        */
		char 	open_method[4];
		wi = 0;
		strcpy( open_method, "wt" );
		if (  ( 0 == strcmpi( "--a",      argv[wi] ) )  ||
			  ( 0 == strcmpi( "--append", argv[wi] ) )   )
		{
			wi++;
			strcpy( open_method, "at" );
		}

		if ( argc > wi )
		{
			out = fopen( argv[wi], open_method );
			file_name = argv[wi];
		}
	}

	for( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
		dig_prev[group] = dig[group];
	}
	for ( bit = 0; bit < IDI_DIN_QTY; bit++ ) din_cmplt[bit] = false;

	sprintf( msg, "DIN_TEST_VALUE:" );		Print_Multiple( msg, out );
	while ( ( TEST_STATE_QUIT != state ) && ( TEST_STATE_COMPLETE != state ) )
	{ /* obtain new values and compare to previous.  When all inputs have changed, then complete. */
		for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ ) IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
		switch( state )
		{
			case TEST_STATE_INIT: /* wait for all inputs to be false */
				if ( true == CMD__IDI48_DIN_Helper_All_Values_False( dig, IDI_DIN_GROUP_QTY ) )
				{
					state = TEST_STATE_READY;
					sprintf( msg, "READY:" );	Print_Multiple( msg, out );
				}
				break;
			case TEST_STATE_READY:
				sprintf( msg, " " );	Print_Multiple( msg, out );
				state = TEST_STATE_ACTIVE;
				break;
			case TEST_STATE_ACTIVE:
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					group_change = dig[group] ^ dig_prev[group]; /* XOR */
					if ( 0x00 != group_change )
					{
						mask = 0x01;
						for ( bit = 0; bit < IDI_DIN_GROUP_SIZE; bit++ )
						{
							index = bit + group * IDI_DIN_GROUP_SIZE;
							if ( ( 0x00 != ( mask & group_change ) ) && ( false == din_cmplt[index] ) )
							{
								din_cmplt[index] = true;
								sprintf( msg, "." );	Print_Multiple( msg, out );
							}
							mask = mask << 1;
						}
					}
				}
				result = true;
				for ( bit = 0; bit < IDI_DIN_QTY; bit++ ) result = result & din_cmplt[bit];
				if ( true == result ) state = TEST_STATE_COMPLETE;
				break;
			case TEST_STATE_COMPLETE:
				break;
			case TEST_STATE_QUIT:
				break;
			default: /* all other states ignored */
				break;
		}
		if ( true == Character_Get( NULL ) )
		{
			state = TEST_STATE_QUIT;
		}
		for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ ) dig_prev[group] = dig[group];
	}

	{
		char * msg = &(dataset->message[0]);
		Time_Current_String( msg, MESSAGE_SIZE );	Print_Multiple( msg, out );
		sprintf( msg, ": " );						Print_Multiple( msg, out );
	}

	if ( TEST_STATE_QUIT == state )
	{
		sprintf( msg, "FAIL" );		error_code = EC_TEST_FAIL;
	}
	else
	{
		sprintf( msg, "PASS" );		error_code = SUCCESS;
	}
	Print_Multiple( msg, out );
	sprintf( msg,  "\n" );
	Print_Multiple( msg, out );
	if ( out != stdout ) fclose( out );
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int CMD__IDI48_DIN_Test_RE( int board_id, int argc, char * argv[] )
{
	int 			error_code;
	TEST_STATE_ENUM state;
	size_t			group;
	size_t			bit;
	size_t			index;
	uint8_t			result;
	uint8_t			dig[IDI_DIN_GROUP_QTY];
	uint8_t			pending[IDI_DIN_GROUP_QTY];
	uint8_t			mask;
	BOOL			din_cmplt[IDI_DIN_QTY];
#if defined( DIN_TEST_DEBUG_PRINT )
	char            message[64];
	int				cp;
	int				state_detect;
#endif
	(void)			argc;
	(void)  		argv;

	state = TEST_STATE_INIT;

	for( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
		IO_Write_U8( board_id, __LINE__, IDI_EDGE_GROUP0  + group,  0xFF ); /* initialize to all rising-edge, led perspective */
		IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group,  0xFF ); /* clear any pending */
		IO_Read_U8( board_id, __LINE__, IDI_PEND_GROUP0 + group, &(pending[group]) );
	}
	for ( bit = 0; bit < IDI_DIN_QTY; bit++ ) din_cmplt[bit] = false;

	printf( "DIN_TEST_RISING_EDGE:" );
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IO_Read_U8( board_id, __LINE__, IDI_PEND_GROUP0 + group, &(pending[group]) ); /* read pending first! */
		IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
		IO_Write_U8( board_id, __LINE__, IDI_INTR_BIT_GROUP0  + group,  0xFF ); /* enable edges - assumes we are not using any interrupts */
	}
	while ( ( TEST_STATE_QUIT != state ) && ( TEST_STATE_COMPLETE != state ) )
	{ /* obtain new values and compare to previous.  When all inputs have changed, then complete. */
		switch( state )
		{
			case TEST_STATE_INIT: /* wait for all inputs to be false */
				if ( true == CMD__IDI48_DIN_Helper_All_Values_False( dig, IDI_DIN_GROUP_QTY ) )
				{
					state = TEST_STATE_READY;
					printf( " READY" );
				}
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
					IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group, 0xFF );
				}
				break;
			case TEST_STATE_READY:
#if defined( DIN_TEST_DEBUG_PRINT )
				cp = 0;
				strcpy( message, "" );
				state_detect = 2;
				printf( "\nPENDING:" );
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					printf( " 0x%02X", pending[group] );
				}
#else
				printf( " " );
#endif
				state = TEST_STATE_ACTIVE;
				break;
			case TEST_STATE_ACTIVE:
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					if ( 0x00 != pending[group] )
					{ /* find the bit values within this group which have pending status */
						mask = 0x01;
						for ( bit = 0; bit < IDI_DIN_GROUP_SIZE; bit++ )
						{
							index = bit + group * IDI_DIN_GROUP_SIZE;
							if ( ( 0x00 != (mask & pending[group]) ) && ( 0x00 != (dig[group] & mask) ) )
							{ /* pending detected and indeed the digital input is now high */
								if ( false == din_cmplt[index] )
								{
									din_cmplt[index] = true;
									//IDI_DIN_Pending_Clear( index );
#if defined( DIN_TEST_DEBUG_PRINT )
cp += sprintf( &(message[cp]), " [%2d]", (int) index );
state_detect = 1;
#else
									printf( "." );
#endif
								}
							}
							mask = mask << 1;
						}
						IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group,  0xFF ); /* clear any pending */
					}
				}
#if defined( DIN_TEST_DEBUG_PRINT )
if ( state_detect < 3 )
{
	printf( "\nPENDING:" );
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		printf( " 0x%02X", pending[group] );
	}
	printf( "   " );
	printf( "%s", message );
	cp = 0;
	state_detect++; /* output change and no change and stop */
}
strcpy( message, "" );
#endif
				result = true;
				for ( bit = 0; bit < IDI_DIN_QTY; bit++ ) result = result & din_cmplt[bit];
				if ( true == result ) state = TEST_STATE_COMPLETE;
				/* get next round of pending values */
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					IO_Read_U8( board_id, __LINE__, IDI_PEND_GROUP0 + group, &(pending[group]) ); /* read pending first! */
					IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
				}
				break;
			case TEST_STATE_COMPLETE:
#if defined( DIN_TEST_DEBUG_PRINT )
				strcpy( message, "" );
				printf( "\nPENDING:" );
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					printf( " 0x%02X", pending[group] );
				}
				printf( "   " );
				printf( "\n" );
#endif
				break;
			case TEST_STATE_QUIT:
				break;
			default: /* all other states ignored */
				break;
		}
		if ( true == Character_Get( NULL ) )
		{
			state = TEST_STATE_QUIT;
		}
	}

	/* disable all interrupts */
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IO_Write_U8( board_id, __LINE__, IDI_INTR_BIT_GROUP0  + group,  0x00 );
	}

	if ( TEST_STATE_QUIT == state )
	{
		printf( " FAIL" );		error_code = EC_TEST_FAIL;
	}
	else
	{
		printf( " PASS" );		error_code = SUCCESS;
	}

	printf( "\n" );

	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int CMD__IDI48_DIN_Test_FE( int board_id, int argc, char * argv[] )
{
	int 			error_code;
	TEST_STATE_ENUM state;
	size_t			group;
	size_t			bit;
	size_t			index;
	uint8_t			result;
	uint8_t			dig[IDI_DIN_GROUP_QTY];
	uint8_t			pending[IDI_DIN_GROUP_QTY];
	uint8_t			mask;
	BOOL			din_cmplt[IDI_DIN_QTY];
#if defined( DIN_TEST_DEBUG_PRINT )
	char            message[64];
	int				cp;
	int				state_detect;
#endif
	(void)			argc;
	(void)  		argv;

	state = TEST_STATE_INIT;

	for( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
		IO_Write_U8( board_id, __LINE__, IDI_EDGE_GROUP0  + group,  0x00 ); /* initialize to all falling-edge, led perspective */
		IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group,  0xFF ); /* clear any pending */
		IO_Read_U8( board_id, __LINE__, IDI_PEND_GROUP0 + group, &(pending[group]) );
	}
	for ( bit = 0; bit < IDI_DIN_QTY; bit++ ) din_cmplt[bit] = false;

	printf( "DIN_TEST_FALLING_EDGE:" );
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IO_Read_U8( board_id, __LINE__, IDI_PEND_GROUP0 + group, &(pending[group]) ); /* read pending first! */
		IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
		IO_Write_U8( board_id, __LINE__, IDI_INTR_BIT_GROUP0  + group,  0xFF ); /* enable edges - assumes we are not using any interrupts */
	}
	while ( ( TEST_STATE_QUIT != state ) && ( TEST_STATE_COMPLETE != state ) )
	{ /* obtain new values and compare to previous.  When all inputs have changed, then complete. */
		switch( state )
		{
			case TEST_STATE_INIT: /* wait for all inputs to be false */
				if ( true == CMD__IDI48_DIN_Helper_All_Values_False( dig, IDI_DIN_GROUP_QTY ) )
				{
					state = TEST_STATE_READY;
					printf( " READY" );
				}
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
					IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group, 0xFF );
				}
				break;
			case TEST_STATE_READY:
#if defined( DIN_TEST_DEBUG_PRINT )
				cp = 0;
				strcpy( message, "" );
				state_detect = 2;
				printf( "\nPENDING:" );
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					printf( " 0x%02X", pending[group] );
				}
#else
				printf( " " );
#endif
				state = TEST_STATE_ACTIVE;
				break;
			case TEST_STATE_ACTIVE:
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					if ( 0x00 != pending[group] )
					{ /* find the bit values within this group which have pending status */
						mask = 0x01;
						for ( bit = 0; bit < IDI_DIN_GROUP_SIZE; bit++ )
						{
							index = bit + group * IDI_DIN_GROUP_SIZE;
							if ( ( 0x00 != (mask & pending[group]) ) && ( 0x00 == (dig[group] & mask) ) )
							{ /* pending detected and indeed the digital input is now low */
								if ( false == din_cmplt[index] )
								{
									din_cmplt[index] = true;
									//IDI_DIN_Pending_Clear( index );
#if defined( DIN_TEST_DEBUG_PRINT )
cp += sprintf( &(message[cp]), " [%2d]", (int) index );
state_detect = 1;
#else
									printf( "." );
#endif
								}
							}
							mask = mask << 1;
						}
						IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group,  0xFF ); /* clear any pending */
					}
				}
#if defined( DIN_TEST_DEBUG_PRINT )
if ( state_detect < 3 )
{
	printf( "\nPENDING:" );
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		printf( " 0x%02X", pending[group] );
	}
	printf( "   " );
	printf( "%s", message );
	cp = 0;
	state_detect++; /* output change and no change and stop */
}
strcpy( message, "" );
#endif
				result = true;
				for ( bit = 0; bit < IDI_DIN_QTY; bit++ ) result = result & din_cmplt[bit];
				if ( true == result ) state = TEST_STATE_COMPLETE;
				/* get next round of pending values */
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					IO_Read_U8( board_id, __LINE__, IDI_PEND_GROUP0 + group, &(pending[group]) ); /* read pending first! */
					IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
				}
				break;
			case TEST_STATE_COMPLETE:
#if defined( DIN_TEST_DEBUG_PRINT )
				strcpy( message, "" );
				printf( "\nPENDING:" );
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					printf( " 0x%02X", pending[group] );
				}
				printf( "   " );
				printf( "\n" );
#endif
				break;
			case TEST_STATE_QUIT:
				break;
			default: /* ignore other states */
				break;
		}
		if ( true == Character_Get( NULL ) )
		{
			state = TEST_STATE_QUIT;
		}
	}

	/* disable all interrupts */
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IO_Write_U8( board_id, __LINE__, IDI_INTR_BIT_GROUP0  + group,  0x00 );
	}

	if ( TEST_STATE_QUIT == state )
	{
		printf( " FAIL" );		error_code = EC_TEST_FAIL;
	}
	else
	{
		printf( " PASS" );		error_code = SUCCESS;
	}

	printf( "\n" );

	return error_code;
}



#if defined( __MSDOS__ )
/**********************************************************************************************************//**
 * @brief
 * Interrupt Service Routine (ISR).
 * This is a legacy implementation.
 *
 * TODO: upgrade hardware such that the loop in the ISR can be moved to normal main-line code.  Simply
 * reading the IDI_INTR_BY_GROUP ought to be the only hardware interrupt acknowledgement required.
 */
static irqreturn_t CMD__IDI48_DIN_Test_Interrupt_Handler( int irq, void * dev_id, struct pt_regs * regs )
{
	size_t	group;
	uint8_t	interrupt_status;
	uint8_t	mask_group;
	uint8_t pending;
	uint8_t bank_backup;
	uint8_t bank_previous_backup;
	int		board_id;
	struct  idi_dataset *  idi_dataset;
	(void)	irq;
	(void)	dev_id;
	(void)	regs;

	board_id = *((int *) dev_id);
	switch( board_id )
	{
		case ID_IDI48:
			idi_dataset = ( struct idi_dataset * )  board_definition[board_id].dataset;
			break;
		default: /* ignore for all other boards because it only makes sense for IDI48 board! */
			goto CMD__IDI48_DIN_Test_Interrupt_Handler_Completed;
	}

	/* save bank information at both software and hardware levels */
	bank_previous_backup = idi_dataset->bank_previous;
	IO_Read_U8( board_id, __LINE__, IDI_BANK,          &bank_backup      );
	/* obtain interrupt summary information */
	IO_Read_U8( board_id, __LINE__, IDI_INTR_BY_GROUP, &interrupt_status );
	mask_group = 0x01;
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{ /* determine which groups generated the interrupt */
		if ( 0 != ( mask_group & interrupt_status ) )
		{
			IO_Read_U8( board_id, __LINE__, IDI_PEND_GROUP0 + group, &pending );
			idi_dataset->isr_pending_list[group] = pending;
			IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group, pending );
		}
		else
		{
			idi_dataset->isr_pending_list[group] = 0;
		}
		mask_group = mask_group << 1;
	}
	/* restore bank information at both hardware and software levels */
	IO_Write_U8( board_id, __LINE__, IDI_BANK, bank_backup );
	idi_dataset->bank_previous = bank_previous_backup;
CMD__IDI48_DIN_Test_Interrupt_Handler_Completed:
	idi_dataset->irq_count++;
	return IRQ_HANDLED;
}
#endif

/**********************************************************************************************************//**
 * @brief
 * Assumption:  IRQs slower than main loop below.
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int CMD__IDI48_DIN_Test_Interrupt( int board_id, int argc, char * argv[] )
{
	struct idi_dataset *  idi_dataset;
	//struct reg_definition * definition;
	int 				error_code;
	TEST_STATE_ENUM 	state;
	size_t				group;
	size_t				bit;
	size_t				index;
	size_t				irq_count_local;
	uint8_t				result;
	uint8_t				irq_pending_local;
	uint8_t				dig[IDI_DIN_GROUP_QTY];
	uint8_t				pending[IDI_DIN_GROUP_QTY];
	uint8_t				mask;
	BOOL				io_report_backup, io_simulate_backup;
	BOOL				din_cmplt[IDI_DIN_QTY];
#if defined( DIN_TEST_DEBUG_PRINT )
	char            	message[64];
	int					cp;
	int					state_detect;
#endif
#if defined( __MSDOS__ )
	unsigned int    interrupt_number;
	switch( board_id )
	{
		case ID_IDI48:
			idi_dataset = ( struct idi_dataset * )  board_definition[board_id].dataset;
			break;
		default:
			return -EC_BOARD;
	}

	if ( argc > 0 )
	{
		interrupt_number = (unsigned int) strtol( argv[0], NULL, 0 );
	}
	else
	{
		interrupt_number = idi_dataset->irq_number;
	}
#else
	//definition = ( struct reg_definition * ) board_definition[board_id].definition;
	(void)			argc;
	(void)  		argv;

	switch( board_id )
	{
		case ID_IDI48:
			idi_dataset = ( struct idi_dataset * )  board_definition[board_id].dataset;
			break;
		default:
			return -EC_BOARD;
	}
#endif

	IO_Write_U8( board_id, __LINE__, IDI_SPI_CONFIG, 0 );
	io_report_backup   = idi_dataset->io_report;
	io_simulate_backup = idi_dataset->io_simulate;

	idi_dataset->irq_count_previous = 0;
	idi_dataset->irq_count          = 0;

	state             = TEST_STATE_INIT;
	irq_pending_local = 0;
	irq_count_local   = 0;


	for( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
		IO_Write_U8( board_id, __LINE__, IDI_EDGE_GROUP0  + group,  0xFF ); /* initialize to all rising-edge, led perspective */
		IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group,  0xFF ); /* clear any pending */
		IO_Read_U8( board_id, __LINE__, IDI_PEND_GROUP0 + group, &(pending[group]) );
	}
	for ( bit = 0; bit < IDI_DIN_QTY; bit++ ) din_cmplt[bit] = false;

	printf( "DIN_INTERRUPT:" );
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IO_Read_U8( board_id, __LINE__, IDI_PEND_GROUP0 + group, &(pending[group]) ); /* read pending first! */
		IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
	}

#if defined( __MSDOS__ )
	/* install interrupt */
	error_code = IOKern_DOS_IRQ_Request( interrupt_number,                		/* IRQ number */
										 CMD__IDI48_DIN_Test_Interrupt_Handler,   /* Interrrupt service routine (ISR) */
										 (void *) &board_id						/* private data */
									   );
	if ( SUCCESS != error_code )
	{
		state = TEST_STATE_QUIT;
		goto CMD__IDI48_DIN_Test_Interrupt_Terminate;
	}
#else
	state = TEST_STATE_QUIT;
	error_code = -EC_INTERRUPT_UNAVAILABLE;
	goto CMD__IDI48_DIN_Test_Interrupt_Terminate;
#endif

	/* enable all interrupts */
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IO_Write_U8( board_id, __LINE__, IDI_INTR_BIT_GROUP0  + group,  0xFF );
	}

	idi_dataset->irq_handler_active = true;

	while ( ( TEST_STATE_QUIT != state ) && ( TEST_STATE_COMPLETE != state ) )
	{ /* obtain new values and compare to previous.  When all inputs have changed, then complete. */

		switch( state )
		{
			case TEST_STATE_INIT: /* wait for all inputs to be false ---------------------------*/
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					IDI_DIN_Group_Get( board_id, group, &(dig[group]) );
				}
				if ( true == CMD__IDI48_DIN_Helper_All_Values_False( dig, IDI_DIN_GROUP_QTY ) )
				{
					state = TEST_STATE_READY;
					printf( " READY" );
				}
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ ) IO_Write_U8( board_id, __LINE__, IDI_CLEAR_GROUP0 + group, 0xFF );
				break;
			case TEST_STATE_READY:		/*-------------------------------------------------------*/
#if defined( DIN_TEST_DEBUG_PRINT )
				cp = 0;
				strcpy( message, "" );
				state_detect = 2;
				printf( "\nPENDING:" );
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					printf( " 0x%02X", pending[group] );
				}
#else
				printf( " " );
#endif
				state = TEST_STATE_ACTIVE;
				break;
			case TEST_STATE_ACTIVE:		/*-------------------------------------------------------*/

				if ( true == Character_Get( NULL ) ) state = TEST_STATE_QUIT;
//TODO: spinlock - SPIN_LOCK_IRQ_SAVE method
#if defined( __MSDOS__ )
				disable();  /* block interrupts...well, work towards spinlock */
#endif
				irq_count_local = idi_dataset->irq_count;
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					pending[group] = idi_dataset->isr_pending_list[group];
					if ( 0 != pending[group] ) irq_pending_local++;
				}
#if defined( __MSDOS__ )
				enable();
#endif
//TODO: release spinlock - SYS_SPIN_UNLOCK_IRQ_RESTORE
				if ( irq_count_local != idi_dataset->irq_count_previous )
				{
					if ( irq_count_local > idi_dataset->irq_count_previous )
					{
						index = irq_count_local - idi_dataset->irq_count_previous;
					}
					else
					{ /* must have wrapped */
						index = UINT_MAX - idi_dataset->irq_count_previous;
						index = index + irq_count_local + 1;
					}
					idi_dataset->irq_count_previous = irq_count_local;
					irq_count_local = index;
				}
				else
				{
					irq_count_local   = 0; /* still waiting for ISR to happen */
					irq_pending_local = 0;
				}

				if ( irq_count_local > 0 )
				{
#if defined( DIN_TEST_DEBUG_PRINT )
cp += sprintf( &(message[cp]), " {%2d}", (int) irq_count_local );
#endif
					for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
					{
						if ( 0x00 != pending[group] )
						{ /* find the bit values within this group which have pending status */
							mask = 0x01;
							for ( bit = 0; bit < IDI_DIN_GROUP_SIZE; bit++ )
							{
								index = bit + group * IDI_DIN_GROUP_SIZE;
								if ( 0x00 != (mask & pending[group]) )
								{ /* pending edge detected */
									if ( false == din_cmplt[index] )
									{
										din_cmplt[index] = true;
#if defined( DIN_TEST_DEBUG_PRINT )
cp += sprintf( &(message[cp]), " [%2d]", (int) index );
state_detect = 1;
#else
										printf( "." );
#endif
									}
								}
								mask = mask << 1;
							}
						}
					}
				}
#if defined( DIN_TEST_DEBUG_PRINT )
if ( state_detect < 3 )
{
	printf( "\nPENDING:" );
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		printf( " 0x%02X", pending[group] );
	}
	printf( "   " );
	printf( "%s", message );
	cp = 0;
	state_detect++; /* output change and no change and stop */
}
strcpy( message, "" );
#endif
				result = true;
				for ( bit = 0; bit < IDI_DIN_QTY; bit++ ) result = result & din_cmplt[bit];
				if ( true == result ) state = TEST_STATE_COMPLETE;
				break;
			case TEST_STATE_COMPLETE:		/*-------------------------------------------------------*/
#if defined( DIN_TEST_DEBUG_PRINT )
				strcpy( message, "" );
				printf( "\nPENDING:" );
				for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
				{
					printf( " 0x%02X", pending[group] );
				}
				printf( "   " );
				printf( "\n" );
#endif
				break;
			case TEST_STATE_QUIT:		/*-------------------------------------------------------*/
				break;
			default: /* ignore other states */
				break;
		}
		if ( true == Character_Get( NULL ) )
		{
			state = TEST_STATE_QUIT;
		}
	}

	/* disable all interrupts */
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		IO_Write_U8( board_id, __LINE__, IDI_INTR_BIT_GROUP0  + group,  0x00 );
	}

#if defined( __MSDOS__ )
	IOKern_DOS_IRQ_Free( idi_dataset->irq_number );
	idi_dataset->irq_handler_active = false;
#endif

CMD__IDI48_DIN_Test_Interrupt_Terminate:

	idi_dataset->io_report   = io_report_backup;
	idi_dataset->io_simulate = io_simulate_backup;

	if ( TEST_STATE_QUIT == state )
	{
		printf( " FAIL" );		error_code = EC_TEST_FAIL;
	}
	else
	{
		printf( " PASS" );		error_code = SUCCESS;
	}

	printf( "\n" );

	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 */
static struct command_line_board  cmd_idi48_test[] =
{
	{	NULL,	CMD__IDI48_DIN_Test_Value,		"value",	"loop, until all have toggled"					},
	{	NULL,	CMD__IDI48_DIN_Test_RE,			"re",		"loop, rising-edge detect  (led perspective)"	},
	{   NULL,	CMD__IDI48_DIN_Test_FE,			"fe",		"loop, falling-edge detect (led perspective)"	},
	{	NULL,	CMD__IDI48_DIN_Test_Interrupt,	"interrupt","loop, test interrupt on all inputs"			},
	{	NULL,	NULL,							NULL,		NULL											},
};
/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
static int CMD__IDI48_DIN_Test( int board_id, int argc, char * argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;
    char *      endptr;
    //int			channel;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	error_code = -EC_SYNTAX;

	//channel = (int) strtol( argv[0], &endptr, 0 );
	strtol( argv[0], &endptr, 0 ); /* just want to know where it fails */
	if ( argv[0] != endptr )
	{  /* assume channel number */
		error_code = (* cmd_idi48_test[0].cmd_fnc )( board_id, argc, argv );
	}
	else
	{ /* otherwise a normal command */
		index = 0;
		while ( NULL != cmd_idi48_test[index].cmd_fnc )
		{
			if ( 0 == strcmpi( cmd_idi48_test[index].name, argv[0] )  )
			{
				argv_new = &(argv[1]);
				argc_new = argc - 1;
				if ( 0 == argc_new ) argv_new = NULL;
				error_code = (* cmd_idi48_test[index].cmd_fnc )( board_id, argc_new, argv_new );
				break;
			}
			index++;
		}
	}
	return error_code;
}
/**********************************************************************************************************/
struct port_list
{
	int				address_start;
	int				address_end;
	BOOL			scan;
	const char * 	name;
};

static const struct port_list  lpm_lx800_port_list[] =
{
		{	0x000,  0x00F, 	0,	"8237DMA Controller #1"							},
		{ 	0x010,  0x01F,	1, 	"Free"											},
		{ 	0x020,  0x021,	0, 	"8259 PIC #1"									},
		{ 	0x022,  0x03F,	1, 	"Free"											},
		{ 	0x040,  0x043,	0, 	"8254 PIT"										},
		{ 	0x044,  0x04D,	1, 	"Free"											},
		{ 	0x04E,  0x04F,	0, 	"Reserved for on,  0xboard configuration"		},
		{ 	0x050,  0x05F,	1, 	"Free"											},
		{ 	0x060,  0x06F,	0, 	"8042 Keyboard / Mouse Controller"				},
		{ 	0x070,  0x07F,	0, 	"CMOS RAM, Clock / Calendar"					},
		{ 	0x080,  0x09F,	0, 	"DMA Page Registers"							},
		{ 	0x0A0,  0x0BF,	0, 	"8259 PIC #2"									},
		{ 	0x0C0,  0x0DF,	0, 	"8237 DMA Controller #2"						},
		{ 	0x0E0,  0x0EF,	1, 	"Free"											},
		{ 	0x0F0,  0x0F1,	0, 	"Math Co,  0xprocessor Control"					},
		{ 	0x0F2,  0x0F7,	1, 	"Free"											},
		{ 	0x0F8,  0x0FF,	0, 	"Math Co,  0xprocessor"							},
		{ 	0x100,  0x102,	0, 	"Video Controllers"								},
		{ 	0x103,  0x11F,	1, 	"Free"											},
		{ 	0x120,  0x12F,	0, 	"Digital I/O"									},
		{ 	0x130,  0x14F,	1, 	"Free"											},
		{ 	0x150, 	0x150,	0,	"Reserved for on,  0xboard configuration"		},
		{ 	0x151,  0x1CF,	1, 	"Free"											},
		{ 	0x1D0,  0x1DF,	0, 	"Legacy Watchdog (1D0,  0xEnabled; 1D8 ,  0x Pet)"		},
		{ 	0x1E8,  0x1EB,	0, 	"Reserved for on,  0xboard configuration"		},
		{ 	0x1EC, 	0x1EC,	0,	"Interrupt Status Register"						},
		{ 	0x1ED, 	0x1ED,	0,	"Status LED"									},
		{ 	0x1EE,  0x1EF,	0, 	"Watchdog Timer Control"						},
		{ 	0x1F0,  0x1FF,	0, 	"IDE Controller #1"								},
		{ 	0x200,  0x277,	1,	"Free"											},
		{ 	0x278,  0x27F,	1, 	"Free (Option for LPT)"							},
		{ 	0x280,  0x2A7,	1, 	"Free"											},
		{ 	0x2A8,  0x2AF,	1, 	"Free (Option for on,  0xboard serial ports)"	},
		{ 	0x2B0,  0x2DF,	0, 	"Video Controllers"								},
		{ 	0x2E0,  0x2E7,	1, 	"Free"											},
		{ 	0x2E8,  0x2EF,	0, 	"COM4 – (Default)"								},
		{ 	0x2F0,  0x2F7,	1, 	"Free"											},
		{ 	0x2F8,  0x2FF,	0, 	"COM2 – (Default)"								},
		{ 	0x300,  0x377,	1, 	"Free"											},
		{ 	0x378,  0x37B,	0, 	"LPT (Default)"									},
		{ 	0x37C,  0x3A7,	1, 	"Free"											},
		{ 	0x3A8,  0x3AF,	1, 	"Free (Option for on,  0xboard serial ports)"	},
		{ 	0x3B0,  0x3BB,	0, 	"Video Controllers"								},
		{ 	0x3BC,  0x3BF,	1, 	"Free (Option for LPT)"							},
		{ 	0x3C0,  0x3DF,	0, 	"Video Controllers"								},
		{ 	0x3E0,  0x3E7,	1, 	"Free"											},
		{ 	0x3E8,  0x3EF,	0, 	"COM3 – (Default)"								},
		{ 	0x3F0,  0x3F7,	1, 	"Free"											},
		{	0x000,	0x000,	0,	NULL											} /* terminator */
};



/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int IDI48_DIN_ID_Port_Scan( const struct port_list * pl, size_t * row, int * port )
{
	size_t 				row_index;
	int					pa;   /* short for port address */
	int					pi_start, pi_end;
	int					count;
	BOOL				not_done;
	uint16_t			id;
	uint8_t				lsb, msb;

#if defined( __MSDOS__ )
#else
	return -EC_NOT_IMPLEMENTED;
#endif

	not_done  = true;
	row_index = 0;
	do
	{
		if ( ( 0 == pl[row_index].address_start ) && ( 0 == pl[row_index].address_end ) )
		{
			not_done = false;
		}
		else
		{
			printf ( "." );
			count = pl[row_index].address_end - pl[row_index].address_start + 1;
			if ( count > 1 )
			{
				pi_start = pl[row_index].address_start      ;
				pi_end   = pl[row_index].address_end    + 1 ;
				if ( 1 == ( pi_start & 1 ) )
				{ /* odd address, skip */
					pi_start++;
					count--;
				}

				for ( pa = pi_start; pa < pi_end; pa = pa + 2 )
				{
#if defined( __MSDOS__ )
					lsb = inportb( ( pa + 0 )  );
					msb = inportb( ( pa + 1 )  );
#endif
					id  = ( ((uint16_t) msb) << 8 ) | ((uint16_t) lsb);

					if ( ID_DIN == id )
					{
						*row  = row_index;
						*port = pa;
						return SUCCESS;
					}
				}
			}
			row_index++;
		}
	} while ( true == not_done );

	return -EC_NOT_FOUND;
}


/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IDI48_DIN_ID( int board_id, int argc, char * argv[] )
{ /*  idi spi id */
	uint16_t 	id;
	(void) 		argc;
	(void) 		argv;

	if ( argc > 0 )
	{
		if ( 0 == strcmpi( "scan", argv[0] )  )
		{
			size_t  row;
			int 	port;
			int 	error_code = SUCCESS;

			printf ("Scanning: " );
			error_code = IDI48_DIN_ID_Port_Scan( lpm_lx800_port_list, &row, &port );
			if ( SUCCESS == error_code )
			{
				printf( "port found at row = %02d, id address = 0x%04X", (int) row, port );
			}
			else
			{
				printf( "not found" );
			}
			printf( "\n" );

		}
	}
	else
	{
		IDI_DIN_ID_Get( board_id, &id );
		printf( "IDI48 DIN ID: 0x%04X\n", id );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IDI48_DIN_All( int board_id, int argc, char * argv[] )
{ /* idi din all [<binary/hex/group>]  */
	int 	error_code;		       /* used primarily for debug purposes */
	int 	channel;
	//BOOL	value;
	int		cp;
	enum { MODE_NONE = 0, MODE_BINARY = 1, MODE_HEX = 2, MODE_ALL = 3 } mode_out;
	int		group;
	char  	message[64];
	uint8_t din_grp[IDI_DIN_GROUP_QTY];
	uint8_t mask;
	(void)	argc;
	(void)	argv;

	mode_out = MODE_ALL;
	if ( argc > 0 )
	{
		int index;
		mode_out = MODE_NONE;
		for ( index = 0; index < argc; index++ )
		{
			if      ( 0 == strcmpi( "binary", argv[index] ) ) mode_out |= MODE_BINARY;
			else if ( 0 == strcmpi( "group",  argv[index] ) ) mode_out |= MODE_HEX;
			else if ( 0 == strcmpi( "hex",    argv[index] ) ) mode_out |= MODE_HEX;
			else                                              mode_out |= MODE_ALL;
		}
	}
	/* build in binary format */
	cp    = 0;
	group = 0;
	for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
	{
		error_code = IDI_DIN_Group_Get( board_id, group, &(din_grp[group]) );
		mask = 0x01;
		for ( channel = 0; channel < 8; channel++ )
		{
			message[cp++] = !!(din_grp[group] & mask) ? '1' : '0';
			mask = mask << 1;
		}
		message[cp++] = ' ';
	}
	message[cp] = '\0';
	if ( MODE_BINARY == ( mode_out & MODE_BINARY ) )
	{
		printf( "DIN: %s\n", message );
	}
	if ( MODE_HEX == ( mode_out & MODE_HEX ) )
	{
		printf( "DIN:" );
		for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ ) printf( " %02X", din_grp[group] );
		printf( "\n" );
	}

	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IDI48_DIN_Channel( int board_id, int argc, char * argv[] )
{
	int 	error_code;		       /* used primarily for debug purposes */
	int 	channel;
	char  	message[IDI_DIN_GROUP_QTY + 2];
	BOOL	value;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	channel = (int) strtol( argv[0], NULL, 0 );
	error_code = IDI_DIN_Channel_Get( board_id, channel, &value );
	message[0] = value ? '1' : '0';
	message[1] = '\0';

	printf( "DIN%02d: %s\n", channel, message );
	return SUCCESS;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__IDI48_DIN_Group( int board_id, int argc, char * argv[] )
{
	int 	error_code; /* used primarily for debugging */
//	int		index;
	int 	group;
	int		group_count;
	BOOL	do_all;
	uint8_t din_grp[IDI_DIN_GROUP_QTY];

	if ( argc < 1 ) do_all = true;
	else            do_all = false;

	if ( 0 == strcmpi( "all", argv[0] ) ) do_all = true;

	if ( do_all )
	{ /* all */
		for ( group = 0; group < IDI_DIN_GROUP_QTY; group++ )
		{
			error_code = IDI_DIN_Group_Get( board_id, group, &(din_grp[group]) );
		}
		group_count = IDI_DIN_GROUP_QTY;
	}
	else
	{
		group = (int) strtol( argv[0], NULL, 0 );
		error_code = IDI_DIN_Group_Get( board_id, group, &(din_grp[0]) );
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
 * 
 * @brief
 */
static struct command_line_board  cmd_idi48[] =
{
	{	NULL,				CMD__IDI48_DIN_ID,		"id",		"params: none.  Reports the DIN board/component ID."	},
	{	NULL,				CMD__IDI48_DIN_Channel,	"chan",		"params: <channel>"										},
	{   NULL,				CMD__IDI48_DIN_Group,	"group",	"params: [<group_channel | all>]"						},
	{	NULL,				CMD__IDI48_DIN_All,	 	"all",		"reports all digital inputs in binary and hex"			},
	{   cmd_idi48_test,		CMD__IDI48_DIN_Test,	"test",		"semi-auto test functions"								},
	{	NULL,				NULL,					NULL,		NULL													},
};
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_IDI48( int board_id, int argc, char* argv[] )
{ /*  idi din <channel/all>  */
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;
    char *      endptr;
    //int			channel;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	if ( ID_IDI48 != board_id ) return -EC_BOARD_TYPE;

	error_code = -EC_SYNTAX;

	//channel = (int) strtol( argv[0], &endptr, 0 );
	strtol( argv[0], &endptr, 0 ); /* just want to know where it fails */
	if ( argv[0] != endptr )
	{  /* assume channel number */
		error_code = (* cmd_idi48[0].cmd_fnc )( board_id, argc, argv );
	}
	else
	{ /* otherwise a normal command */
		index = 0;
		while ( NULL != cmd_idi48[index].cmd_fnc )
		{
			if ( 0 == strcmpi( cmd_idi48[index].name, argv[0] )  )
			{
				argv_new = &(argv[1]);
				argc_new = argc - 1;
				if ( 0 == argc_new ) argv_new = NULL;
				error_code = (* cmd_idi48[index].cmd_fnc )( board_id, argc_new, argv_new );
				break;
			}
			index++;
		}
	}
	return error_code;
}
/**********************************************************************************************************/


/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*    < < < <  O T H E R   C O M M A N D   F U N C T I O N S  > > > > 	                                  */

/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Trace_File( int board_id, int argc, char * argv[] )
{ /*   */
	int 					error_code;
	size_t					index;
	BOOL					not_done;
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	error_code = SUCCESS;
	if ( argc < 1 )
	{ /* read */
		printf( "file_name    = " );
		if ( 0 == strlen( dataset->trace.filename ) ) printf( "%s\n", IO_TRACE_DUMP_FILE_NAME );
		else                                          printf( "%s\n", dataset->trace.filename );
	}
	else
	{ /* write */
		index    = 0;
		not_done = true;
		do
		{
			dataset->trace.filename[index] = argv[0][index];
			if ( '\0' == argv[0][index] )
			{
				not_done = false;
			}
			else if ( index >= ( IO_TRACE_FILE_NAME_SIZE - 2) )
			{
				not_done = false;
				index++;
				dataset->trace.filename[index] = '\0';
			}
			else
			{
				index++;
			}
		} while ( true == not_done );
	}
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Trace_Stop( int board_id, int argc, char * argv[] )
{ /*   */
	int 					error_code;
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	error_code = SUCCESS;
	if ( argc < 1 )
	{ /* read */
		printf( "stop         = " );
		printf( "%s\n", global_io_trace_stop_name[dataset->trace.stop] );
/*
		switch( dataset->trace.stop )
		{
			case IO_TRACE_STOP_NONE: 	printf( "none" 		 );	break;
			case IO_TRACE_STOP_ERROR:	printf( "error"      );	break;
			case IO_TRACE_STOP_CTRLC:   printf( "ctrlc"      ); break;
		}
		printf( "\n" );
*/
	}
	else
	{ /* write */
		int index = 0;
		error_code = -EC_PARAMETER;
		while ( NULL != global_io_trace_stop_name[index] )
		{
			if ( 0 == strcmpi( global_io_trace_stop_name[index], argv[0] )  )
			{
				dataset->trace.stop = (enum IO_TRACE_STOP_ENUM) index;
				error_code = SUCCESS;
				break;
			}
			index++;
		}
/*
		if      ( 0 == strcmpi( "none", argv[0] )  )
		{
			dataset->trace.stop = IO_TRACE_STOP_NONE;
		}
		else if ( 0 == strcmpi( "error",      argv[0] )  )
		{
			dataset->trace.stop = IO_TRACE_STOP_ERROR;
		}
		else if ( 0 == strcmpi( "ctrlc",      argv[0] )  )
		{
			dataset->trace.stop = IO_TRACE_STOP_CTRLC;
		}
		else
		{
			error_code = -EC_PARAMETER;
		}
*/
	}
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Trace_Start( int board_id, int argc, char * argv[] )
{ /*   */
	int 					error_code;
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	error_code = SUCCESS;
	if ( argc < 1 )
	{ /* read */
		printf( "start        = " );
		printf( "%s\n", global_io_trace_start_name[dataset->trace.start] );
/*
		switch( dataset->trace.start )
		{
			case IO_TRACE_START_DISABLE: 	printf( "disable" );	break;
			case IO_TRACE_START_ON:			printf( "on"      );	break;
			case IO_TRACE_START_ERROR:		printf( "error"	  );	break;
		}
		printf( "\n" );
*/
	}
	else
	{ /* write */
		int index = 0;
		error_code = -EC_PARAMETER;
		while ( NULL != global_io_trace_start_name[index] )
		{
			if ( 0 == strcmpi( global_io_trace_start_name[index], argv[0] )  )
			{
				dataset->trace.start = (enum IO_TRACE_START_ENUM) index;
				error_code = SUCCESS;
				break;
			}
			index++;
		}
/*
		if      ( 0 == strcmpi( "disable", argv[0] )  )
		{
			dataset->trace.start = IO_TRACE_START_DISABLE;
		}
		else if ( 0 == strcmpi( "on",      argv[0] )  )
		{
			dataset->trace.start = IO_TRACE_START_ON;
		}
		else if ( 0 == strcmpi( "disable", argv[0] )  )
		{
			dataset->trace.start = IO_TRACE_START_ERROR;
		}
		else
		{
			error_code = -EC_PARAMETER;
		}
*/
	}
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Trace_Ignore( int board_id, int argc, char * argv[] )
{ /*   */
	int 					error_code;
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	error_code = SUCCESS;
	if ( argc < 1 )
	{ /* read */
		/* this will breakdown if we add more items to IO_TRACE_IGNORE_DEFINITION */
		size_t at_least_one = 0;
		printf( "ignore       = " );
		if ( IO_TRACE_IGNORE_SPI  == ( dataset->trace.ignore & IO_TRACE_IGNORE_SPI  ) )
		{
			if ( at_least_one > 0 ) printf( " " );
			printf( global_io_trace_ignore_name[IO_TRACE_IGNORE_SPI  ] );
			at_least_one++;
		}
		if ( IO_TRACE_IGNORE_IO   == ( dataset->trace.ignore & IO_TRACE_IGNORE_IO   ) )
		{
			if ( at_least_one > 0 ) printf( " " );
			printf( global_io_trace_ignore_name[IO_TRACE_IGNORE_IO   ] );
			at_least_one++;
		}
		if ( IO_TRACE_IGNORE_NONE == ( dataset->trace.ignore ) )
		{
			printf( global_io_trace_ignore_name[IO_TRACE_IGNORE_NONE ] );
		}
		printf( "\n" );
	}
	else
	{ /* write */
		size_t argc_index, str_index;
		error_code = -EC_PARAMETER;
		dataset->trace.ignore = IO_TRACE_IGNORE_NONE;
		for ( argc_index = 0; argc_index < argc; argc_index++ )
		{
			str_index = 0;
			while ( NULL != global_io_trace_ignore_name[str_index] )
			{
				if ( 0 == strcmpi( global_io_trace_ignore_name[str_index], argv[argc_index] )  )
				{
					dataset->trace.ignore |= (enum IO_TRACE_IGNORE_ENUM) str_index;
					goto CMD_Trace_Ignore_TRY_ANOTHER;
				}
				str_index++;
			}
			return -EC_PARAMETER;
CMD_Trace_Ignore_TRY_ANOTHER:
			error_code = SUCCESS;
		}
	}
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static struct command_line_board  cmd_trace[] =
{
	{	NULL,	CMD__Trace_Start,			"start",	"begin trace on: < "  IO_TRACE_START_DEFINITION( IO_TRACE_START_STOP_EXTRACT_NAME_HELP_LIST ) ">" },
	{	NULL,	CMD__Trace_Stop,			"stop",		"stop trace on:  < "  IO_TRACE_STOP_DEFINITION(  IO_TRACE_START_STOP_EXTRACT_NAME_HELP_LIST ) ">" },
	{	NULL,	CMD__Trace_Ignore,			"ignore",	"ignore trace on:  < "  IO_TRACE_IGNORE_DEFINITION(  IO_TRACE_IGNORE_EXTRACT_NAME_HELP_LIST ) ">" },
	{	NULL,	CMD__Trace_File,			"file",		"file_name up to 32-characters"			},
	{	NULL,	NULL,						NULL,		NULL									},
};
/**********************************************************************************************************//**
 *
 * @brief
 */
static int CMD__Main_Trace( int board_id,  int argc,  char* argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;
    (void)		board_id;

    error_code = -EC_SYNTAX;

    if ( ( argc < 1 ) || ( NULL == argv ) )
	{
		index = 0;
		while ( NULL != cmd_trace[index].cmd_fnc )
		{
			printf( "trace." );
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			if ( 0 == argc_new ) argv_new = NULL;
			error_code = (* cmd_trace[index].cmd_fnc )( board_id, argc_new, argv_new );
			index++;
		}
	}
	else
	{
		index = 0;
		while ( NULL != cmd_trace[index].cmd_fnc )
		{
			if ( 0 == strcmpi( cmd_trace[index].name, argv[0] )  )
			{
				argv_new = &(argv[1]);
				argc_new = argc - 1;
				if ( 0 == argc_new ) argv_new = NULL;
				error_code = (* cmd_trace[index].cmd_fnc )( board_id, argc_new, argv_new );
				break;
			}
			index++;
		}
	}
	return error_code;
}
/**********************************************************************//**
 * 
 * @brief
 *
 * Either reads or writes a register using the form:
 * idi <register acronym> [<value>]
 *
 * If <value> is not include, then it is assumed to be a read.  If value is
 * included, then a write to the specified register is made.
 *
 * This function uses the definitions[] array which is global and built from
 * IDI_REGISTER_SET_DEFINITION macro which is a nicely organized register list.
 *
 * @param[in] argc  number of arguments including the executable file name
 * @param[in] argv  list of string arguments lex'd from the command line
 * @return SUCCESS (0) if no errors encountered, otherwise errors are reported
 * as a negative value.
 */
int Command_Line_Register_Transaction( int board_id, int argc, char* argv[] )
{
	int 					error_code;
	BOOL					valid_bank_reg;
	int 					index;
	struct reg_definition * definition;
	//struct board_dataset *  dataset;

	definition = ( struct reg_definition * ) board_definition[board_id].definition;
	//dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	error_code = Register_Acronym_To_Row( board_id, argv[0], &index );
	if ( SUCCESS != error_code ) return error_code;

	/* Bank register is dealt as a special case in that we can accept either a number
	 * or an enumerated value (i.e. the name of the enumerated symbol).
	 *
	 * TODO: generalize this for all parameters that are enumerated.
	 */
	valid_bank_reg = false;
	switch( board_id )
	{
		case ID_IDI48:  if ( IDI_BANK == definition[index].symbol )  valid_bank_reg = true;		break;
		case ID_IDO48:	if ( IDO_BANK == definition[index].symbol )  valid_bank_reg = true;		break;
	}
	if ( valid_bank_reg )
	{
		if ( argc < 2 )
		{ /* read operation */
			uint8_t value;
			error_code = IO_Read_U8( board_id, __LINE__, definition[index].symbol, &value );
			if ( SUCCESS == error_code )
			{
				printf( "RD: %s=%s  (0x%02X)\n",  definition[index].acronym, Bank_Symbol_To_Name( board_id, ((int) value) ), value );
			}
		}
		else
		{ /* write */
			uint8_t 	value;
			int		 	bank;

			error_code = Bank_Name_To_Symbol( board_id, argv[1], &bank );
			if ( SUCCESS != error_code )
			{ /* assume that it is just a number */
				value = (uint8_t) strtol( argv[1], NULL, 0 );
			}
			else
			{ /* this is an enumerated value */
				value = (uint8_t) bank;
			}
			error_code = IO_Write_U8( board_id, __LINE__, definition[index].symbol, value );
			if ( SUCCESS == error_code )
			{
				printf( "WR: %s=0x%02X\n", definition[index].acronym, value );
			}
		}
	}
	else
	{
		if ( argc < 2 )
		{ /* read operation */
			uint8_t value;
			error_code = IO_Read_U8( board_id, __LINE__, definition[index].symbol, &value );
			if ( SUCCESS == error_code )
			{
				printf( "RD: %s=0x%02X\n",  definition[index].acronym, value );
			}
		}
		else
		{
			uint8_t value;
			value = (uint8_t) strtol( argv[1], NULL, 0 );
			error_code = IO_Write_U8( board_id, __LINE__, definition[index].symbol, value );
			if ( SUCCESS == error_code )
			{
				printf( "WR: %s=0x%02X\n", definition[index].acronym, value );
			}
		}
	}

    return SUCCESS;
}

/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Main_IO_Behavior( int board_id, int argc, char * argv[] )
{ /*   */
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	if ( ( argc < 1 ) || ( NULL == argv ) )
	{ /* read */
		printf( "IO Simulate        = %s\n", dataset->io_simulate ? "true" : "false" );
		printf( "IO Report          = %s\n", dataset->io_report   ? "true" : "false" );
	}
	else if ( argc > 1 )
	{ /* write */
		if       ( 0 == strcmpi( "simulate", argv[0] ) )
		{
			dataset->io_simulate = String_To_Bool( argv[1] );
		}
		else if  ( 0 == strcmpi( "report", argv[0] ) )
		{
			dataset->io_report = String_To_Bool( argv[1] );
		}
		else
		{

		}
		printf( "OK\n" );
	}
	else
	{ /* read individual */
		if       ( 0 == strcmpi( "simulate", argv[0] ) )
		{
			printf( "IO Simulate        = %s\n", dataset->io_simulate ? "true" : "false" );
		}
		else if  ( 0 == strcmpi( "report", argv[0] ) )
		{
			printf( "IO Report          = %s\n", dataset->io_report   ? "true" : "false" );
		}
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Main_Base( int board_id, int argc, char * argv[] )
{ /*  idi spi ecd [<time sec>] */
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	if ( ( argc < 1 ) || ( NULL == argv ) )
	{ /* read */
		printf( "base               = 0x%04X\n", dataset->base_address );
	}
	else
	{ /* write */
		dataset->base_address = (uint16_t) strtol( argv[0], NULL, 0 );
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Main_Irq_Number( int board_id, int argc, char * argv[] )
{ /*  idi spi ecd [<time sec>] */
	struct idi_dataset *  idi_dataset = NULL;

	switch( board_id )
	{
		case ID_IDI48:
			idi_dataset = ( struct idi_dataset * )  board_definition[board_id].dataset;
			break;
	}

	if ( ( argc < 1 ) || ( NULL == argv ) )
	{ /* read */
		if ( NULL == idi_dataset ) return SUCCESS;
#if defined( __MSDOS__ )
		printf( "irq                = %u\n", idi_dataset->irq_number );
#else
		printf( "irq                = %u\n", idi_dataset->irq_number );
#endif
	}
	else
	{ /* write */
		if ( NULL == idi_dataset ) return -EC_BOARD_TYPE;
		idi_dataset->irq_number = (unsigned int) strtol( argv[0], NULL, 0 );
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Number of interrupt cycles.  If "loop" is used, then the count can be terminated by
 * pressing any key on the keyboard input.
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Main_I_Count( int board_id, int argc, char * argv[] )
{ /*  idi spi ecd [<time sec>] */
	struct idi_dataset *  idi_dataset = NULL;

	switch( board_id )
	{
		case ID_IDI48:
			idi_dataset = ( struct idi_dataset * )  board_definition[board_id].dataset;
			break;
	}

	if ( ( argc < 1 ) || ( NULL == argv ) )
	{ /* read */
		if ( NULL == idi_dataset ) return SUCCESS;
#if defined( __MSDOS__ )
		printf( "iqty               = %d\n", idi_dataset->irq_quantity );
#else
		printf( "iqty               = %lu\n", idi_dataset->irq_quantity );
#endif
	}
	else
	{ /* write */
		if ( NULL == idi_dataset ) return -EC_BOARD_TYPE;
		idi_dataset->irq_quantity = (size_t) strtol( argv[0], NULL, 0 );
		printf( "OK\n" );
	}
	return SUCCESS;
}

#if (0)
/**********************************************************************************************************//**
 *
 * @brief
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Main_Init_Reg( int board_id, int argc, char * argv[] )
{ /*  idi set reg <address/name> <value> */
	int index;
	int error_code;
	struct board_dataset *  dataset;
	struct reg_definition * definition;
	dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;
	definition = ( struct reg_definition * ) board_definition[board_id].definition;

	if ( argc < 1 )
	{ /* list all of them */
		for ( index = 0; index < REGS_INIT_QTY; index++ )
		{
			printf ( "reg_init[%2d]: ", index );
			if ( dataset->reg_init[index].used )
			{
				if ( IDI_UNDEFINED != dataset->reg_init[index].location )
				{
					row  = REG_LOCATION_LOGICAL_GET( location );
					printf( "reg = %s, value = 0x%02X", definition[row].acronym, value );
				}
				else
				{
					printf( "address  = 0x%04X, value = 0x%02X", address, value );
				}
			}
			else
			{
				printf( "unused" );
			}
		}
		printf( "\n" );
	}
	else if ( argc > 1 )
	{ /* write */
		for ( index = 0; index < REGS_INIT_QTY; index++ )
		{
			if ( IDI_UNDEFINED != dataset->reg_init[index].location )
			{
				if ( SUCCESS == Register_Acronym_To_Row( board_id, argv[0], &index ) )
				{ /* acronym found! */

				}
				else
				{

				}
			}
		}
	}

	if ( argc > 0 )
	{
		//search for name.

	}

	return SUCCESS;
}
#endif
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Main_FRAM_CS( int board_id, int argc, char * argv[] )
{ /* */
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	if ( ( argc < 1 ) || ( NULL == argv ) )
	{ /* read */
		printf( "framcs             = " );
		if ( true == dataset->fram_cs_default ) printf( "1" );
		else                                    printf( "0" );
		printf( "  (FRAM SPI Chip Select)\n" );
	}
	else
	{ /* write */
		dataset->fram_cs_default = String_To_Bool( argv[0] );
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Main_AnalogStick_CS( int board_id, int argc, char * argv[] )
{ /*  */
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	if ( ( argc < 1 ) || ( NULL == argv ) )
	{ /* read */
		printf( "aics               = " );
		if ( true == dataset->as_cs_default ) printf( "1" );
		else                                  printf( "0" );
		printf( "  (Analog Stick SPI Chip Select)\n" );
	}
	else
	{ /* write */
		dataset->as_cs_default = String_To_Bool( argv[0] );
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Main_AnalogStick_CS_Mode( int board_id, int argc, char * argv[] )
{ /*  */
	int						error_code;
	struct board_dataset *  dataset;

	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	error_code = SUCCESS;
	if ( ( argc < 1 ) || ( NULL == argv ) )
	{ /* read */
		printf( "aimode             = " );
		switch( dataset->as_spi_chip_select_behavior )
		{
		case CSB_SOFTWARE:
			printf( "SOFTWARE" );
			break;
		case CSB_BUFFER:
			printf( "BUFFER" );
			break;
		default:
			printf( "INVALID" );
			break;
		}
		printf( "\n" );
	}
	else
	{ /* write */
		if      ( ( argv[0][0] == 's' ) || ( argv[0][0] == 'S' ) )
		{
			dataset->as_spi_chip_select_behavior = CSB_SOFTWARE;
		}
		else if ( ( argv[0][0] == 'b' ) || ( argv[0][0] == 'B' ) )
		{
			dataset->as_spi_chip_select_behavior = CSB_BUFFER;
		}
		else
		{
			error_code = -EC_SYNTAX;
		}
	}
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Mode jumper settings.
 *  0  = 00  =
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
static int CMD__Main_Mode_Jumpers( int board_id, int argc, char * argv[] )
{ /*  */
	struct board_dataset * ds = NULL;

	switch( board_id )
	{
		case ID_IDI48:
			ds = ( struct board_dataset * )  board_definition[board_id].dataset;
			break;
		case ID_IDO48:
			ds = ( struct board_dataset * )  board_definition[board_id].dataset;
			break;
	}

	if ( ( argc < 1 ) || ( NULL == argv ) )
	{ /* read */
		if ( NULL == ds ) return SUCCESS;
		printf( "mode               = " );
		switch( ds->mode_jumpers )
		{
			case MODE_JUMPERS_0:	printf( "0 (legacy)" );		break;
			case MODE_JUMPERS_1:	printf( "1         " );		break;
			case MODE_JUMPERS_2:	printf( "2         " );		break;
			case MODE_JUMPERS_3:	printf( "3         " );		break;
		}
		printf( ". Assumed mode jumper settings.\n" );
	}
	else
	{ /* write */
		if ( NULL == ds ) return -EC_BOARD_TYPE;
		if ( 0 == strcmpi( "legacy", argv[0] ) )
		{
			ds->mode_jumpers = MODE_JUMPERS_0;
		}
		else
		{
			ds->mode_jumpers = (MODE_JUMPERS_ENUM) strtol( argv[0], NULL, 0 );
		}
		printf( "OK\n" );
	}
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static struct command_line_board  cmd_set[] =
{
	{	NULL,		CMD__Main_Base,				"base",		"params: [<address>]"										},
	{	NULL,		CMD__Main_IO_Behavior,		"io",		"params: [<simulate>/<report>]"								},
	{	NULL,   	CMD__Main_Irq_Number,	    "irq",      "params: [<irq_number 0 to 15>]"                       		},
	{	NULL,   	CMD__Main_I_Count,		    "iqty",     "params: [<number>].  Number of interrupts."				},
	{	NULL,   	CMD__Main_FRAM_CS,		    "framcs",   "params: [<0/1>].  FRAM SPI chip select channel."			},
	{	NULL,   	CMD__Main_AnalogStick_CS,   "aics",   	"params: [<0/1>].  Analog Stick SPI chip select channel."	},
	{	NULL,   	CMD__Main_AnalogStick_CS_Mode,"aimode",	"params: [<buf/soft>]. Analog Stick SPI chip select mode."	},
	{   NULL,		CMD__AS_Parameter,			"as",		"Analog Stick register or parameter read/write"				},
	{	cmd_trace,  CMD__Main_Trace,    		"trace",   	"params: [<start/stop/file>."								},
	{	NULL,  		CMD__Main_Mode_Jumpers,		"mode",   	"params: [<0/1/2/3].  Mode jumper settings"					},
	{	NULL,		NULL,						NULL,		NULL														},
};
/**********************************************************************************************************//**
 *
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_Set( int board_id, int argc, char* argv[] )
{
	int 		error_code;
	int 		index;
    int     	argc_new;
    char ** 	argv_new;
	struct board_dataset *  dataset;
	(void)		board_id;

	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;
    error_code = -EC_SYNTAX;

	if ( argc < 1 )
	{
		index = 0;
		while ( NULL != cmd_set[index].cmd_fnc )
		{
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			if ( 0 == argc_new ) argv_new = NULL;
			dataset->set__suppress_io_activity = true;
			error_code = (* cmd_set[index].cmd_fnc )( board_id, argc_new, argv_new );
			dataset->set__suppress_io_activity = false;
			index++;
		}
	}
	else
	{
		index = 0;
		while ( NULL != cmd_set[index].cmd_fnc )
		{
			if ( 0 == strcmpi( cmd_set[index].name, argv[0] )  )
			{
				argv_new = &(argv[1]);
				argc_new = argc - 1;
				if ( 0 == argc_new ) argv_new = NULL;
				dataset->set__suppress_io_activity = true;
				error_code = (* cmd_set[index].cmd_fnc )( board_id, argc_new, argv_new );
				dataset->set__suppress_io_activity = false;
				break;
			}
			index++;
		}
	}
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_Dump( int board_id, int argc, char* argv[] )
{
	int    error_code;
	FILE * fd_out;
	//struct reg_definition * definition;
	//definition = ( struct reg_definition * ) board_definition[board_id].definition;

	if ( argc > 0 )
	{
		fd_out = fopen( argv[0], "w" );
		if ( NULL == fd_out ) fd_out = stdout;
	}
	else
	{
		fd_out = stdout;
	}
	error_code = Register_Report_CSV( board_id, fd_out );

	if ( (argc > 0) && (NULL != fd_out) && (stdout != fd_out) )
	{
		fclose( fd_out );
	}
	return error_code;
}

/**********************************************************************************************************//**
 *
 * @brief
 * Used to write to one or more registers in sequence.  Can write values to the same register by using the 'x'
 * to toggle auto-indexing from initially on to the off position.
 *
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_IO_Write( int board_id, int argc, char* argv[] )
{
	size_t	index;
	BOOL	address_captured;
	int    	error_code;
	int    	address;
	uint8_t value;
	BOOL	auto_index;
	(void)	board_id;

	if ( argc < 2 ) return -EC_NOT_FOUND;

	address_captured = false;
	auto_index       = true;
	for ( index = 0; index < argc; index++ )
	{
		if ( ( 'x' == argv[index][0] ) || ( 'X' == argv[index][0] ) )
		{ /* x - toggle auto-index */
			auto_index = !auto_index;
		}
		else
		{
			if ( false == address_captured )
			{ /* set address */
				address = (int) strtol( argv[index], NULL, 0 );
				address_captured = true;
				//printf( "0x%04X:", address );
			}
			else
			{
				value   = (uint8_t) strtol( argv[index], NULL, 0 );
				error_code = IO_Write_U8_Port( NULL, __LINE__, address, value );
				if ( SUCCESS != error_code ) return error_code;
				if ( true == auto_index ) address++;
			}
		}
	}
	printf( "OK\n" );
	return error_code;
}
/**********************************************************************************************************//**
 * 
 * @brief
 * used to read one or more values from registers sequentially.
 *
 * b [prefix commands] <type> <address> [x] [c <count>]
 *
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error
 *         code is returned.
 */
int Command_Line_IO_Read( int board_id, int argc, char* argv[] )
{
	size_t	index;
	BOOL	count_capture;
	BOOL	address_captured;
	BOOL	auto_index;
	int    	error_code;
	int    	address;
	int		count;

	uint8_t value;
	(void)	board_id;

	if ( argc < 1 ) return -EC_NOT_FOUND;

	count			 = 1;
	error_code       = SUCCESS;
	count_capture    = false;
	address_captured = false;
	auto_index       = true;
	for ( index = 0; index < argc; index++ )
	{
		if ( ( 'c' == argv[index][0] ) || ( 'C' == argv[index][0] ) )
		{ /* c <count>  - number of reads to perform */
			count_capture = true;
		}
		else if ( ( 'x' == argv[index][0] ) || ( 'X' == argv[index][0] ) )
		{ /* x - toggle auto-index */
			auto_index = !auto_index;
		}
		else
		{
			if ( true == count_capture )
			{ /* determine the count */
				count = (int) strtol( argv[index], NULL, 0 );
				count_capture = false;
			}
			else
			{
				if ( false == address_captured )
				{ /* set address */
					address = (int) strtol( argv[index], NULL, 0 );
					address_captured = true;
					printf( "0x%04X:", address );
				}
				else
				{


				}
			}
		}
	}

	for ( index = 0; index < count; index++ )
	{
		error_code = IO_Read_U8_Port( NULL, __LINE__, address, &value );
		if ( SUCCESS != error_code ) return error_code;
		printf( " 0x%02X", value );
		if ( true == auto_index ) address++;
	}

	printf( "\n" );
	return error_code;
}
/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
int Command_Line_Wait( int board_id, int argc, char* argv[] )
{
	(void)  board_id;
	(void)	argc;
	(void)  argv;

	if ( global_loop_command ) return SUCCESS;
	global_loop_command = true;
	return SUCCESS;
}
/**********************************************************************/
/**********************************************************************//**
 *
 * @brief
 * Outputs a help listing to the user.
 */
static void Help_Pause_Helper( BOOL skip_pause, size_t * line )
{
	if ( true == skip_pause ) return;
	if ( *line > 20 )
	{
		printf( "Press any key for more....." );
		while ( !Character_Get( NULL ) ) { /* wait */ }
		printf( "\r" );
		*line = 0;
	}
}
/**********************************************************************//**
 *
 * @brief
 */
static void Help_Output( BOOL							skip_pause,
						 int							depth,
						 size_t *						line,
						 struct command_line_board * 	table,
						 FILE * 						out
					   )
{
	size_t 		 row;
	int			 index;
	const char * prefix = { "       " };

	row = 0;
	while ( NULL != table[row].name )
	{
		if ( 0 == depth )
		{
			fprintf( out, "%-4s", table[row].name );
		}
		else
		{
			for ( index = 0; index < depth; index++ ) fprintf( out, "%s", prefix );
			fprintf( out, "%-8s", table[row].name );
		}
		fprintf( out, " - %s\n", table[row].help );
		*line += 1;
#if defined( __MSDOS__ )
		Help_Pause_Helper( skip_pause, line );
#endif
		if ( NULL != table[row].link )
		{ /* recursive */
			Help_Output( skip_pause, depth + 1, line, table[row].link, out );
			fprintf( out, "\n" );
			*line += 1;
		}
		row++;
	}
}
/**********************************************************************//**
 *
 * @brief
 *
 */
static void IDI_Help( size_t * line, FILE * out )
{
	fprintf( out, "IDI48 board supported\n" );
	fprintf( out, " Examples:\n" );
	fprintf( out, "   idi dig0          <-- reports the digital input group 0 port.\n" );
	fprintf( out, "   idi loop dig0     <-- loops until key pressed\n" );
	fprintf( out, "   idi irq loop dig0 <-- loops while counting interrupts or until iqty reached\n" );
	fprintf( out, "   idi irq dig0      <-- does command once and uses interrupts for short time\n" );
	*line += 7;
}
/**********************************************************************//**
 *
 * @brief
 *
 */
static void IDO_Help( size_t * line, FILE * out )
{
	fprintf( out, "IDO48 board supported\n" );
	fprintf( out, " Examples:\n" );
	fprintf( out, "   ido dog0          <-- reports the digital output group 0 port.\n" );
	fprintf( out, "   ido loop dog0     <-- loops until key pressed\n" );
	*line += 5;
}

/* forward declarations */
static struct command_line_board    cmd_top[];
static struct command_line          cmd_prefix[];


/**********************************************************************//**
 *
 * @brief
 * Outputs a help listing to the user.
 */
static void Help( BOOL skip_pause, FILE * out )
{
	size_t  line;
	int		top;

	line = 0;
	fprintf( out, "\n" );
	fprintf( out, "STDBus General Test Code\n" );
	fprintf( out, "Apex Embedded Systems\n" );
	fprintf( out, "SVN software revision: " SVN_REV "\n"  );
	fprintf( out, "Supporting:\n" );
	line += 5;

	IDI_Help( &line, out );
	IDO_Help( &line, out );

	fprintf( out, "\n" );
	fprintf( out, "help - outputs help information\n" );
	line += 2;

	top = 0;
	while ( NULL != cmd_prefix[top].name )
	{
		fprintf( out, "\n" );
		fprintf( out, "%-4s - %s\n", cmd_prefix[top].name, cmd_prefix[top].help );
		top++;
		line += 2;
		Help_Pause_Helper( skip_pause, &line );
	}

	Help_Output( skip_pause, 0, &line, (struct command_line_board *) cmd_top /* removes the board type parameter */, out );
	fprintf( out, "\n" );
}
/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
int Command_Line_Help( int board_id, int argc, char* argv[] )
{
	FILE *  out;
	FILE *  fd = NULL;
	BOOL    skip_pause = false;
	(void)  board_id;

	out = stdout;
	if ( argc > 0 )
	{ /* attempt to open a file */
		fd = fopen( argv[0], "w" );
		if ( NULL != fd )
		{
			out = fd;	skip_pause = true;
		}
	}
	Help( skip_pause, out );
	if ( NULL != fd ) fclose( fd );

	return SUCCESS;
}
/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
int FPGA_Date_Time_Information( int board_id, uint32_t * date_svn, uint32_t * date_compilation )
{
	int 			location_data;	/* date-time information */
	int 			location_index; /* date-time information */
	int				state;
	size_t			index;
	size_t  		count;
	BOOL			fpga_data_available;
	const uint8_t 	index_test_list[] = { 0x05, 0x0A, 0x00 };
	uint8_t			scratch[16] = { 0x00 };
	uint8_t			block_offset;
	uint8_t 		block_type;
	uint32_t 		scratch_u32;
#define FPGA_DATE_SIZE 4
	struct board_dataset *  dataset;
	dataset = (struct board_dataset *) board_definition[board_id].dataset;

	if ( MODE_JUMPERS_0 == dataset->mode_jumpers )
	{ /* we are in legacy mode */
		return -EC_MODE_LEGACY;
	}

	/* initialize parameters */
	location_data  		= board_definition[board_id].fpga_data_symbol;
	location_index 		= board_definition[board_id].fpga_index_symbol;
	fpga_data_available = true; /* assumed */
	/* verify availability of the fpga index register */
	count 				= 0;
	while ( 0x00 != index_test_list[count] )
	{
		IO_Write_U8( board_id, __LINE__, location_index, index_test_list[count] );
		IO_Read_U8( board_id, __LINE__, location_index, &(scratch[count]) );
		count++;
	}
	for ( index = 0; index < count; index++ )
	{
		if ( index_test_list[index] != scratch[index] )
		{
			fpga_data_available = false;
			break;
		}
	}

	if ( false == fpga_data_available )
	{ /* legacy, FPGA date codes not supported */
		*date_svn = 0;
		*date_compilation = 0;
		return SUCCESS;
	}

	state        = 0;
	index        = 0;
	block_offset = 0;
	while( 3 != state )
	{
		switch ( state )
		{
			case 0:	/* start of block */
				IO_Write_U8( board_id, __LINE__, location_index, (uint8_t) index );
				index++;
				IO_Read_U8( board_id, __LINE__, location_data, &block_offset    );

				IO_Write_U8( board_id, __LINE__, location_index, (uint8_t) index 	);
				index++;
				IO_Read_U8( board_id, __LINE__, location_data, &block_type 		);

				switch( block_type )
				{
					case 's':
					case 'S':  /* subversion date-time code */
						state = 1;		count = 0;
						break;
					case 'c':
					case 'C':	/* compilation date-time code */
						state = 2;		count = 0;
						break;
					case 0x00:
					case 0xFF:	/* end or null block */
						state = 3;
						count = 0;
						break;
					default:
						break;
				}
				break;
			case 1: /* within 'SVN' block */
				IO_Write_U8( board_id, __LINE__, location_index, (uint8_t) index   );
				index++;	count++;
				IO_Read_U8( board_id,  __LINE__, location_data,  &(scratch[FPGA_DATE_SIZE - count]) );
				if ( FPGA_DATE_SIZE == count )
				{
					size_t i;
					scratch_u32 = 0;
					for ( i = 0; i < FPGA_DATE_SIZE; i++ )
					{
						scratch_u32 = ( scratch_u32 << 8 ) + ( (uint32_t) scratch[i] );
					}
					*date_svn = scratch_u32;
					state = 0;
				}
				break;
			case 2: /* within 'Compilation' block */
				IO_Write_U8( board_id, __LINE__, location_index, (uint8_t) index   );
				index++;	count++;
				IO_Read_U8( board_id,  __LINE__, location_data,  &(scratch[FPGA_DATE_SIZE - count]) );
				if ( FPGA_DATE_SIZE == count )
				{
					size_t i;
					scratch_u32 = 0;
					for ( i = 0; i < FPGA_DATE_SIZE; i++ )
					{
						scratch_u32 = ( scratch_u32 << 8 ) + ( (uint32_t) scratch[i] );
					}
					*date_compilation = scratch_u32;
					state = 0;
				}
				break;
		} /* switch */
	} /* while */
	return SUCCESS;
#undef FPGA_DATE_SIZE
}

/*
 * Notes:
 *  Read back compilation time-stamp as:  2812470617
 *
 *  b ido fram dump 0 100
 *  		index_tx	index_rx
 *  		16			14
 *  		16			14
 *
 *  b ido fram init "Apex Embedded "
 *          1			0
 *
 * Something is really screwed up, but cannot tell if it is CPU hardware or a real problem.
 *
 *
 *
 *
 */



/**********************************************************************************************************//**
 * @brief
 * @return A zero (SUCCESS) is returned if successful, otherwise a negative error code is returned.
 */
int Command_Line_FPGA( int board_id, int argc, char* argv[] )
{
	int			error_code;
	FILE *		out;
	uint32_t 	date_svn;
	uint32_t 	date_compilation;
	(void)		argc;
	(void)		argv;

	error_code = FPGA_Date_Time_Information( board_id, &date_svn, &date_compilation );
	if ( error_code < SUCCESS ) goto Command_Line_FPGA_Exit;

	out = stdout;

	fprintf( out, "FPGA: " );

	if ( 0 == date_svn )
	{
		fprintf( out, "SVN = unavailable" );
	}
	else
	{
#if defined( __MSDOS__ )
		fprintf( out, "SVN =    %08lu", date_svn );
#else
		fprintf( out, "SVN =    %08u", date_svn );
#endif
	}

	if ( 0 == date_compilation )
	{
		fprintf( out, ", Compilation = unavailable\n" );
	}
	else
	{
#if defined( __MSDOS__ )
		fprintf( out, ", Compilation = %08lu\n", date_compilation );
#else
		fprintf( out, ", Compilation = %08u\n", date_compilation );
#endif
	}

Command_Line_FPGA_Exit:
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 * Used to build out the command list data structure and the board type requirement list.  Implemented in this
 * fashion in order to keep all the data together.
 */
#define CMD_MAIN_LIST(_) \
	/* link			function						name		help description						board type required?  */ \
	_( NULL,		Command_Line_Dump,				"dump",		"Register information in CSV format",		true	) \
	_( cmd_set,		Command_Line_Set,				"set",		"Set/Get main parameters",					true	) \
	_( cmd_spi,		Command_Line_SPI,				"spi",		"SPI related functions",					true	) \
	_( cmd_as,		Command_Line_Analog_Stick,		"as",		"Analog Stick related functions",			true	) \
 	_( cmd_fram,	Command_Line_FRAM,				"fram",		"FRAM related functions",					true	) \
 	_( cmd_idi48,	Command_Line_IDI48,				"di",		"Digital input related functions",			false /* implied board ID */	) \
 	_( cmd_ido48,	Command_Line_IDO48,				"do",		"Digital output related functions",			false /* implied board ID */	) \
 	_( NULL,		Command_Line_IO_Write,			"o",		"unrestricted I/O write <address> <data>",	false	) \
 	_( NULL,		Command_Line_IO_Read,			"i",		"unrestricted I/O read  <address>",			false	) \
	_( NULL,		Command_Line_Wait,				"wait",		"wait for any key press",					false	) \
	_( NULL,		Command_Line_Help,				"help",		"very brief command summary",				false	) \
	_( NULL,		Command_Line_FPGA,				"fpga",		"fpga compilation and svn dates",			false	) \
 	_( NULL,		NULL,							NULL,		NULL,										false	)

#define CMD_MAIN_EXTRACT_COMMANDS(link,fnc,name,help,brdrqrd)  				{ link, fnc, name, help },
#define CMD_MAIN_EXTRACT_BOARD_TYPE_REQUIRED(link,fnc,name,help,brdrqrd)	brdrqrd,

/**********************************************************************************************************//**
 *
 * @brief
 */
static struct command_line_board  cmd_top[] =
{
	CMD_MAIN_LIST( CMD_MAIN_EXTRACT_COMMANDS )
};
static BOOL cmd_top__board_type_required[] =
{
		CMD_MAIN_LIST( CMD_MAIN_EXTRACT_BOARD_TYPE_REQUIRED )
};
/**********************************************************************//**
 *
 * @brief
 * Processes and dispatches the top level of the command and passes the remaining
 * string list onto specialized functions to further process arguments.
 * If no command is specified then a help output is produced.
 *
 * @param[in] argc  number of arguments including the executable file name
 * @param[in] argv  list of string arguments lex'd from the command line
 * @return SUCCESS (0) if no errors encountered, otherwise errors are reported
 * as a negative value.
 */
int Command_Line_Main( int board_id, int argc, char* argv[] )
{
	int 	error_code;
	int     index;
	BOOL	not_found;
    int     argc_new;
    char ** argv_new;


    error_code = -EC_SYNTAX;

    if ( argc < 1 ) return -EC_NOT_FOUND;

    /* there has to be at least one argument at this point */
	not_found = true;
	index     = 0;
	while ( NULL != cmd_top[index].cmd_fnc )
	{
		if ( 0 == strcmpi( cmd_top[index].name, argv[0] )  )
		{
			not_found = false;
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			if ( 0 == argc_new ) argv_new = NULL;
			error_code = (* cmd_top[index].cmd_fnc )( board_id, argc_new, argv_new );
			if ( SUCCESS != error_code ) goto COMMAND_LINE_MAIN_TERMINATE;
			break;
		}
		index++;
	}

	if ( not_found )
	{ /* assume that it is a register related transaction */
		argv_new = &(argv[0]);
		argc_new = argc - 0;
		error_code = Command_Line_Register_Transaction( board_id, argc_new, argv_new );
		if ( SUCCESS != error_code ) goto COMMAND_LINE_MAIN_TERMINATE;
	}
COMMAND_LINE_MAIN_TERMINATE:
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 *
 */
enum CLM_BTS   /* Command Line Main (CLM)  Board Type Status (BTS) */
{
	CLM_BTS_NORMAL  		= 0,
	CLM_BTS_NOT_REQUIRED	= 1,
	CLM_BTS_IMPLIED			= 2
};

/**********************************************************************************************************//**
 *
 * @brief
 * Determine if this command requires specification of board type or not, or if a board type can be implied.
 *
 * @return
 *   0 = normal
 *   1 = board type not required (i.e. 'board_id' parameter is a don't care)
 *   2 = board type implied and determined here.
 */
static enum CLM_BTS Command_Line_Main__Board_Type_Status( const char * cmd_string, int * board_id )
{
	int    	 		index;
	enum CLM_BTS	status;
	index     = 0;
	status    = CLM_BTS_NORMAL;
	*board_id = ID_BOARD_UNKNOWN;
	while ( NULL != cmd_top[index].cmd_fnc )
	{   /* two peices of logic are being combined here.  First if "do" or "di" commands issued, then
	       the "ido" or "idi" board type is optional as "do" implies IDO48 operation and "di" implies
	       IDI48 board operation.

	       The second part of the logic is looking at "i", "o" and "wait" commands.  These commands
	       are independent of any board type.
	    */

		/* short cuts -- board IDs implied */
		if      ( 0 == strcmpi( "do", cmd_string )  )
		{
			*board_id = ID_IDO48;	status = CLM_BTS_IMPLIED;
		}
		else if ( 0 == strcmpi( "di", cmd_string )  )
		{
			*board_id = ID_IDI48;	status = CLM_BTS_IMPLIED;
		}


		/* commands which bypass the need for board type */
		if ( 0 == strcmpi( cmd_top[index].name, cmd_string )  )
		{
			if ( false == cmd_top__board_type_required[index] )
			{
				if ( CLM_BTS_NORMAL == status ) status = CLM_BTS_NOT_REQUIRED;
			}
			return status;
		}
		index++;
	}
	return status;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static int Command_Line_Loop_Count( int   argc,    /* argument index in this case 	*/
									char* argv[]   /* always null in this case 		*/
							      )
{
	int error_code;

	if ( argc > 0 )
	{
		if ( argv[0][0] >= '0' && argv[0][0] <= '9' )
		{
			global_loop_count         = (int) strtol( argv[0], NULL, 0 );
			global_loop_count_counter = global_loop_count;
			error_code = 1;
		}
		else
		{
			error_code = -EC_SYNTAX;
		}
	}
	else
	{
		error_code = -EC_SYNTAX;
	}
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static int Command_Line_Loop_Delay( int   argc,    /* argument index in this case 	*/
									char* argv[] /* always null in this case 		*/
							      )
{
	int error_code;

	if ( argc > 0 )
	{
		if ( ( argv[0][0] >= '0' ) && ( argv[0][0] <= '9' ) )
		{
			global_loop_delay_ms = (int) strtol( argv[0], NULL, 0 );
			error_code = 1;
		}
		else
		{
			error_code = -EC_SYNTAX;
		}
	}
	else
	{
		error_code = -EC_SYNTAX;
	}
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static int Command_Line_Loop_Space( int   argc,    /* argument index in this case 	*/
									char* argv[]   /* always null in this case 		*/
							      )
{
	(void) 	argc;
	(void) 	argv;

	global_loop_space = true;
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static struct command_line  cmd_loop[] =
{
	{	NULL,	Command_Line_Loop_Count,			"count",	"loop for N counts"								},
	{	NULL,	Command_Line_Loop_Delay,			"delay",	"loop with delay <milliseconds>"				},
	{	NULL,	Command_Line_Loop_Space,			"space",	"loop each time space bar pressed"				},
	{	NULL,	NULL,								NULL,		NULL											},

};
/**********************************************************************************************************//**
 *
 * @brief
 */
static int Command_Line_Prefix_Loop( int   argc,   	/* argument index in this case */
									 char* argv[] 	/* always null in this case */
								   )
{
	int 	error_code;
	int     index;
	int		ai;
	//BOOL	not_found;
    int     argc_new;
    char ** argv_new;

    global_loop_command = true;
	//not_found = true;
	index     = 0;
	ai		  = 0;
	while ( NULL != cmd_loop[index].cmd_fnc )
	{
		if ( 0 == strcmpi( cmd_loop[index].name, argv[ai] )  )
		{
			//not_found = false;
			argv_new = &(argv[ai + 1]);
			argc_new = argc - (ai + 1);
			if ( 0 == argc_new ) argv_new = NULL;
			error_code = (* cmd_loop[index].cmd_fnc )( argc_new, argv_new );
			if ( error_code < 0 ) goto COMMAND_LINE_PREFIX_LOOP_TERMINATE;
			else ai += 1 + error_code;
			index = 0;
		}
		else
		{
			index++;
		}
	}
	return ai;
COMMAND_LINE_PREFIX_LOOP_TERMINATE:
	return error_code;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static int Command_Line_Prefix_Irq( int   argc,    /* argument index in this case 	*/
									char* argv[] /* always null in this case 		*/
							      )
{
	(void)  argc;
	(void)  argv;

	global_irq_please_install_handler_request = true;
#if(0)
	switch( board_id )
	{
		case ID_IDI48:
			( ( struct idi_dataset * ) board_definition[board_id].dataset)->irq_please_install_handler_request = true;
			break;		/* continue */
		default:
			return -EC_BOARD_TYPE;
	}
#endif
//	printf( "OK on irq_please_install_handler_request\n" );
	return 0;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static int Command_Line_Prefix_Trace( 	int   argc,    /* argument index in this case 	*/
							   	   	   char* argv[] /* always null in this case 		*/
							 	 	 )
{
	(void) argc;
	(void) argv;
	global_io_trace_enable = true;
	return SUCCESS;
}
/**********************************************************************************************************//**
 *
 * @brief
 */
static struct command_line  cmd_prefix[] =
{
	{	NULL,		Command_Line_Prefix_Irq,			"irq",		"allows commands to handle interrupts"				},
	{	cmd_loop,	Command_Line_Prefix_Loop,			"loop",		"any command loops until key pressed"				},
	{	NULL,		Command_Line_Prefix_Trace,			"trace",	"enable trace functionality"						},
	{	NULL,		NULL,								NULL,		NULL												},
};
/**********************************************************************************************************//**
 *
 * @brief
 * @return Index value to next argument in the list (i.e. updated index), or negative value
 *         indicating an error condition.
 */
int Command_Line_Prefix( int argc, char* argv[] )
{
	int 		error_code;
	int			index;
	int			ai;    	/* argument index 	*/
    int     	argc_new;
    char ** 	argv_new;

    if ( argc < 1 ) return 0; /* nothing to do here */

	index     = 0;
	ai        = 0;
	while ( NULL != cmd_prefix[index].cmd_fnc )
	{
		if ( 0 == strcmpi( cmd_prefix[index].name, argv[0] )  )
		{
			argv_new = &(argv[1]);
			argc_new = argc - 1;
			if ( 0 == argc_new ) argv_new = NULL;
			error_code = (* cmd_prefix[index].cmd_fnc )( argc_new, argv_new );
			if ( error_code < 0 ) goto COMMAND_LINE_PREFIX_TERMINATE;
			else ai = 1 + error_code; /* ok we found something */
			break;
		}
		index++;
	}
	return ai;
COMMAND_LINE_PREFIX_TERMINATE:
	return error_code;
}

/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/




/**********************************************************************************************************/
/**********************************************************************************************************/
/**********************************************************************************************************/
/*                  < < < <  M A I N    F U N C T I O N S  > > > > 	                                      */

/* #define MAIN_DEBUG 1
 */

/**********************************************************************//**
 *
 * @brief
 *
 */
static int IDI_Termination( struct idi_dataset * dataset )
{
	FILE * fd;

#if defined( MAIN_DEBUG )
printf( "Main:  IDI_Termination\n" );
#endif

#if defined( __MSDOS__ )
	IOKern_Resource_Termination();
#endif
	if ( dataset->irq_handler_active )
	{
#if defined( __MSDOS__ )
		printf( " --> irq %u: count = %d\n", dataset->irq_number, dataset->irq_count );
#else
		printf( " --> irq %u: count = %lu\n", dataset->irq_number, dataset->irq_count );
#endif
	}
	dataset->irq_handler_active = false;
	dataset->irq_please_install_handler_request = false;

	/* save the data set */
	fd = fopen( IDI_BOARD_INIT_FILE_NAME, "wb" );
	if ( NULL == fd )
	{ /* defaults */

#if defined( MAIN_DEBUG )
		printf("IDI_Termination: fopen() error: %d (%s)\n", errno, strerror(errno) );
#endif
		return -EC_INIT_FILE;
	}
	else
	{ /* write dataset */
#if defined( MAIN_DEBUG )
printf( "IDI_Termination:  sizeof( struct idi_dataset ) = " );
# if defined( __MSDOS__ )
   printf( "%u\n", sizeof( struct idi_dataset ) );
# else
   printf( "%lu\n", sizeof( struct idi_dataset ) );
# endif
#endif

   	    fwrite( dataset, sizeof( struct idi_dataset ), 1, fd );
		fclose( fd );
	}
	return SUCCESS;
}
/**********************************************************************//**
 *
 * @brief
 *
 */
static int IDO_Termination( struct ido_dataset * dataset  )
{
	FILE * fd;

#if defined( MAIN_DEBUG )
printf( "Main:  IDO_Termination\n" );
#endif

	/* save the data set */
	fd = fopen( IDO_BOARD_INIT_FILE_NAME, "wb" );
	if ( NULL == fd )
	{ /* defaults */
#if defined( MAIN_DEBUG )
		printf("IDO_Termination: fopen() error: %d (%s)\n", errno, strerror(errno) );
#endif
		return -EC_INIT_FILE;
	}
	else
	{ /* write dataset */
#if defined( MAIN_DEBUG )
printf( "IDO_Termination:  sizeof( struct ido_dataset ) = " );
# if defined( __MSDOS__ )
   printf( "%u\n", sizeof( struct ido_dataset ) );
# else
   printf( "%lu\n", sizeof( struct ido_dataset ) );
# endif
#endif
		fwrite( dataset, sizeof( struct ido_dataset ), 1, fd );
		fclose( fd );
	}
	return SUCCESS;
}
/**********************************************************************//**
 *
 * @brief
 * Runs upon application exit.  It saves the idi_dataset data structure.
 *
 * @return SUCCESS (0) if no errors encountered, otherwise errors are reported
 * as a negative value.
 */
int Termination( int board_id )
{
	int 					error_code;
//	struct board_dataset *  dataset;
//	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;
//	/* this is for temporary purposes only -- any needs to be stored as false.  */
//	dataset->set__suppress_io_activity = false;

#if defined( MAIN_DEBUG )
printf( "Main:  Termination\n" );
#endif

	error_code = IO_Trace_Dump( board_id );
	switch( board_id )
	{
		case ID_IDI48:
			error_code = IDI_Termination( (struct idi_dataset *) board_definition[board_id].dataset );
			break;
		case ID_IDO48:
			error_code = IDO_Termination( (struct ido_dataset *) board_definition[board_id].dataset );
			break;
	}
	return error_code;
}
/**********************************************************************//**
 *
 * @brief
 */
void IDI_Dataset_Defaults( struct idi_dataset * ds )
{
	int index;

	printf( "Warning - using main IDI48 dataset defaults\n" );
	/* clear all */
	memset( ds, 0, sizeof(struct idi_dataset) );
	/* set default values other than zeros */
	ds->base_address        = 0x0110;
	/* dataset->base_address        = 0xff00; */

	for ( index = 0; index < REGS_INIT_QTY; index++ )
	{
		ds->reg_init[index].used     = false;
		ds->reg_init[index].address  = 0;
		ds->reg_init[index].location = IDI_UNDEFINED;
		ds->reg_init[index].value    = 0x00;
	}
}
/**********************************************************************//**
 *
 * @brief
 */
void IDO_Dataset_Defaults( struct ido_dataset * ds )
{
	int index;

	printf( "Warning - using main IDO48 dataset defaults\n" );
	/* clear all */
	memset( ds, 0, sizeof(struct ido_dataset) );
	/* set default values other than zeros */
	ds->base_address        = 0x0130;
	/* dataset->base_address        = 0xff00; */

	for ( index = 0; index < REGS_INIT_QTY; index++ )
	{
		ds->reg_init[index].used     = false;
		ds->reg_init[index].address  = 0;
		ds->reg_init[index].location = IDO_UNDEFINED;
		ds->reg_init[index].value    = 0x00;
	}
}
/**********************************************************************//**
 *
 * @brief
 */
void Signal_Handler( int signal )
{
	struct board_dataset * dataset = ( struct board_dataset * )  board_definition[global_board_id].dataset;
	(void) signal;
	dataset->quit_application = true;
	if ( true == global_io_trace_enable )
	{
		if ( IO_TRACE_STOP_CTRLC == dataset->trace.stop ) dataset->trace.active = false;
	}
}

/**********************************************************************//**
 *
 * @brief
 *
 */
static void IDI_Initialization_Data_Structure_Load( struct idi_dataset * dataset )
{
	FILE * 	fd;
	size_t	file_size;
	/* restore the data set, if we can otherwise initialize with defaults */
	fd = fopen( IDI_BOARD_INIT_FILE_NAME, "rb" );
	if ( NULL == fd )
	{ /* use the defaults if file not found */
		IDI_Dataset_Defaults( dataset );
#if defined( MAIN_DEBUG )
printf( "IDI_Initialization_Data_Structure_Load - fd null -> using defaults\n" );
#endif
	}
	else
	{   /* determine file size - if data set has changed we must restart */
		fseek( fd, 0L, SEEK_END );
		file_size = (size_t) ftell( fd );
		fseek( fd, 0L, SEEK_SET );
		if ( file_size != sizeof( struct idi_dataset ) )
		{
			IDI_Dataset_Defaults( dataset );
#if defined( MAIN_DEBUG )
printf( "IDI_Initialization_Data_Structure_Load - file_size=" );
# if defined( __MSDOS__ )
   printf( "%u, sizeof(struct idi_dataset)=%u\n", file_size, sizeof( struct idi_dataset ) );
# else
   printf( "%lu, sizeof(struct idi_dataset)=%lu\n", file_size, sizeof( struct idi_dataset ) );
# endif
#endif
		}
		else
		{	/* read in dataset */
			fread( dataset, sizeof( struct idi_dataset ), 1, fd );
#if defined( MAIN_DEBUG )
printf( "IDI_Initialization_Data_Structure_Load - file_size=" );
# if defined( __MSDOS__ )
   printf( "%u\n", file_size );
# else
   printf( "%lu\n", file_size );
# endif
#endif
		}
		fclose( fd );
	}
	dataset->svn_revision_string 				= svn_revision_string;
	dataset->irq_handler_active 				= false;
	dataset->irq_please_install_handler_request = false;
	dataset->irq_count          				= 0;
	dataset->irq_count_previous 				= 0;
	dataset->spi_is_present						= false;
	//dataset->loop_command       = false;
	dataset->bank_previous      				= IDI_BANK_UNDEFINED;
	dataset->quit_application                   = false;
	dataset->board_id							= ID_IDI48;
}
/**********************************************************************//**
 *
 * @brief
 * 
 */
static void IDO_Initialization_Data_Structure_Load( struct ido_dataset * dataset )
{
	FILE * 	fd;
	size_t	file_size;
	/* restore the data set, if we can otherwise initialize with defaults */
	fd = fopen( IDO_BOARD_INIT_FILE_NAME, "rb" );
	if ( NULL == fd )
	{ /* use the defaults if file not found */
		IDO_Dataset_Defaults( dataset );
#if defined( MAIN_DEBUG )
printf( "IDO_Initialization_Data_Structure_Load - fd null -> using defaults\n" );
#endif
	}
	else
	{   /* determine file size - if data set has changed we must restart */
		fseek( fd, 0L, SEEK_END );
		file_size = (size_t) ftell( fd );
		fseek( fd, 0L, SEEK_SET );
		if ( file_size != sizeof( struct ido_dataset ) )
		{
			IDO_Dataset_Defaults( dataset );
#if defined( MAIN_DEBUG )
printf( "IDO_Initialization_Data_Structure_Load - file_size=" );
# if defined( __MSDOS__ )
   printf( "%u, sizeof(struct ido_dataset)=%u\n", file_size, sizeof( struct ido_dataset ) );
# else
   printf( "%lu, sizeof(struct ido_dataset)=%lu\n", file_size, sizeof( struct ido_dataset ) );
# endif
#endif
		}
		else
		{	/* read in dataset */
			fread( dataset, sizeof( struct ido_dataset ), 1, fd );
#if defined( MAIN_DEBUG )
printf( "IDO_Initialization_Data_Structure_Load - file_size=" );
# if defined( __MSDOS__ )
   printf( "%u\n", file_size );
# else
   printf( "%lu\n", file_size );
# endif
#endif
		}
		fclose( fd );
	}
	dataset->svn_revision_string 				= svn_revision_string;
	dataset->irq_handler_active 				= false;
	dataset->irq_please_install_handler_request = false;
	dataset->irq_count          				= 0;
	dataset->irq_count_previous 				= 0;
	dataset->spi_is_present						= false;
	//dataset->loop_command       = false;
	dataset->bank_previous      				= IDO_BANK_UNDEFINED;
	dataset->quit_application                   = false;
	dataset->board_id							= ID_IDO48;
}
/**********************************************************************//**
 *
 * @brief
 *
 */
static void IDI_Initialization_Register( int board_id, struct idi_dataset * dataset )
{
	int index;

	for ( index = 0; index < REGS_INIT_QTY; index++ )
	{
		if ( dataset->reg_init[index].used )
		{
			if ( IDI_UNDEFINED != dataset->reg_init[index].location )
			{
				IO_Write_U8( board_id, __LINE__, dataset->reg_init[index].location, dataset->reg_init[index].value );
			}
			else
			{
				IO_Write_U8_Port( 	(struct board_dataset *) dataset,
									__LINE__,
									dataset->reg_init[index].address,
									dataset->reg_init[index].value
								);
			}
		}
	}
}
/**********************************************************************//**
 *
 * @brief
 *
 */
static void IDO_Initialization_Register( int board_id, struct ido_dataset * dataset )
{
	int index;

	for ( index = 0; index < REGS_INIT_QTY; index++ )
	{
		if ( dataset->reg_init[index].used )
		{
			if ( IDO_UNDEFINED != dataset->reg_init[index].location )
			{
				IO_Write_U8( board_id, __LINE__, dataset->reg_init[index].location, dataset->reg_init[index].value );
			}
			else
			{
				IO_Write_U8_Port( 	(struct board_dataset *) dataset,
									__LINE__,
									dataset->reg_init[index].address,
									dataset->reg_init[index].value
								);
			}
		}
	}
}

/**********************************************************************//**
 *
 * @brief
 * Runs upon application startup.  It restores the idi_dataset data structure
 * or if the file cannot be found it will simply initialize those parameters
 * to default values.
 *
 * @return SUCCESS (0) if no errors encountered, otherwise errors are reported
 * as a negative value.
 */
int Initialization( int board_id )
{
	struct board_dataset *  dataset;
	(void) board_id;

	dataset = (struct board_dataset *) board_definition[ID_IDI48].dataset;
	dataset->quit_application = false;
	IDI_Initialization_Data_Structure_Load( (struct idi_dataset *) dataset );
	dataset = (struct board_dataset *) board_definition[ID_IDO48].dataset;
	dataset->quit_application = false;
	IDO_Initialization_Data_Structure_Load( (struct ido_dataset *) dataset );

#if defined( __MSDOS__ )
	IOKern_Resource_Initialization();
#endif
	dataset = (struct board_dataset *) board_definition[ID_IDI48].dataset;
	IDI_Initialization_Register( ID_IDI48, (struct idi_dataset *) dataset );
	dataset = (struct board_dataset *) board_definition[ID_IDO48].dataset;
	IDO_Initialization_Register( ID_IDO48, (struct ido_dataset *) dataset );

	/* if ctrl-c we want to gracefully quit */
	signal( SIGINT, Signal_Handler );

	IO_Trace_Initialize( ID_IDI48 );
	IO_Trace_Initialize( ID_IDO48 );

#if(0)
	struct board_dataset *  dataset;
	dataset = ( struct board_dataset * )  board_definition[board_id].dataset;

	/* restore the data set, if we can otherwise initialize with defaults */
	switch( board_id )
	{
		case ID_IDI48:
			IDI_Initialization_Data_Structure_Load( (struct idi_dataset *) dataset );
			break;
		case ID_IDO48:
			IDO_Initialization_Data_Structure_Load( (struct ido_dataset *) dataset );
			break;
		default:
			return -EC_BOARD_TYPE;
	}
#if defined( __MSDOS__ )
	IOKern_Resource_Initialization();
#endif
	/* initialize any registers */
	switch( board_id )
	{
		case ID_IDI48:
			IDI_Initialization_Register( board_id, (struct idi_dataset *) dataset );
			break;
		case ID_IDO48:
			IDO_Initialization_Register( board_id, (struct ido_dataset *) dataset );
			break;
		default:
			return -EC_BOARD_TYPE;
	}

	/* if ctrl-c we want to gracefully quit */
	dataset->quit_application = false;
	signal( SIGINT, Signal_Handler );
#endif
    return SUCCESS;
}

/**********************************************************************//**
 * 
 * @brief
 */
int IDI_Main_Loop_Testing( struct idi_dataset * dataset )
{
	if ( dataset->irq_handler_active )
	{
		if ( dataset->irq_count != dataset->irq_count_previous )
		{
//TODO: spinlock - SPIN_LOCK_IRQ_SAVE method
			dataset->irq_count_previous = dataset->irq_count;
//TODO: release spinlock
#if defined( __MSDOS__ )
			printf( " --> irq %u: count = %d\n", dataset->irq_number, dataset->irq_count );
#else
			printf( " --> irq %u: count = %lu\n", dataset->irq_number, dataset->irq_count );
#endif
			if ( dataset->irq_count_previous >= dataset->irq_quantity )
			{
				return -1; /* quit the loop */
			}
		}
	}
	return 0;
}
/**********************************************************************//**
 *
 * @brief
 */
static void Main_Versalogic( void )
{
//TODO: make this a "set" tuple command??????????????????????
#if defined( __MSDOS__ )
//fixes Versalogic address forwarding to STDbus
	outportb( 0x00E2, 2 );
#endif
}

/**********************************************************************//**
 *
 * @brief
 * Processes and dispatches the top level of the command and passes the remaining
 * string list onto specialized functions to further process arguments.
 * If no command is specified then a help output is produced.
 *
 * @param[in] argc  number of arguments including the executable file name
 * @param[in] argv  list of string arguments lex'd from the command line
 * @return SUCCESS (0) if no errors encountered, otherwise errors are reported
 * as a negative value.
 */
int main( int argc, char* argv[] )
{
	//int index;
    //int count;
	int		board_id;
	int 	error_code;
	int     index;
    int     argc_new;
    char ** argv_new;
	struct board_dataset *  dataset;
	//struct reg_definition * definition;

#if defined( MAIN_DEBUG )
printf( "Main:  Entry\n" );
#endif
#if defined( SPI_PRINT_XFR_DEBUG )
global_internal_tx_byte_count = 0;
global_internal_rx_byte_count = 0;
#endif

/* used only for Win7 debugging sessions with cygwin */
//#if defined( __MSDOS__ )
//#else
# if(1)
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
# endif
//#endif

#if defined( MAIN_DEBUG )
printf( "Main:  1\n" );
#endif

#if(0)
    {
    	char buf[80];
		int count = argc;
		int index = 0;     /* OK, zero value is name of executable file  */
		while ( count > 0 )
		{
			printf( "index = %d, str = <%s>\n", index, argv[index] );
			index++;
			count--;
		}
		Time_Current_String( buf, 80 );
		printf( "Current time is:  %s", buf );
    }
#endif

    /* Versalogic */
    Main_Versalogic();

#if defined( MAIN_DEBUG )
printf( "Main:  2\n" );
#endif

    error_code				= SUCCESS;
    board_id        		= ID_BOARD_UNKNOWN;
    global_board_id 		= board_id;

    global_loop_space		= false;

    global_io_trace_enable 	= false;

   	index = 1;
   	if ( argc > 1 )
    {
//   		if ( 0 == strcmpi( "help", argv[index]) )
//		{
//			Help( false /* pause */, stdout );
//			goto Main_Termination_Skip_Termination;
//		}

#if defined( MAIN_DEBUG )
printf( "Main:  3\n" );
#endif

   		/* prefixed portion of the commands */
		argv_new   = &(argv[index]);
		argc_new   = argc - index;
		error_code = Command_Line_Prefix( argc_new, argv_new );
		if ( error_code < 0 )  goto Main_Termination_Skip_Termination;
		else                   index += error_code;

//		if ( argc > 1 )
//		{
			/* special case condition -- scan commands looking for these special cases */
			switch( Command_Line_Main__Board_Type_Status( argv[index], &board_id ) )
			{
				case CLM_BTS_NORMAL:	/* need board type */
					break;
				case CLM_BTS_IMPLIED:
					global_board_id = board_id;
					goto Main_Initialization_Start;
				case CLM_BTS_NOT_REQUIRED:
					goto Main_Loop_Start;
			}

			/* board specific */
			if       ( 0 == strcmpi( "idi", argv[index]) )
			{
				board_id 		= ID_IDI48;
				global_board_id = board_id;
				index++;
			}
			else if  ( 0 == strcmpi( "ido", argv[index]) )
			{
				board_id        = ID_IDO48;
				global_board_id = board_id;
				index++;
			}
			else
			{
				error_code = -EC_BOARD_TYPE;
				goto Main_Termination_Error_Codes_Skip_Termination;
			}
//		}

Main_Initialization_Start:
#if defined( MAIN_DEBUG )
printf( "Main:  Main_Initialization_Start\n" );
#endif
   	    error_code = Initialization( board_id );
   	    if ( SUCCESS != error_code ) goto Main_Termination;

   	    dataset    = ( struct board_dataset * )  board_definition[board_id].dataset;
   	 	//definition = ( struct reg_definition * ) board_definition[board_id].definition;

Main_Loop_Start:
#if defined( MAIN_DEBUG )
printf( "Main:  Main_Loop_Start\n" );
#endif
		do
		{ /* assumes that all functions utilize arguments in read only fashion */
			argv_new = &(argv[index]);
			argc_new = argc - index;
			error_code = Command_Line_Main( board_id, argc_new, argv_new );
			if ( SUCCESS != error_code ) goto Main_Termination_Error_Codes;

			switch( board_id )
			{
				case ID_IDI48:
					if ( -1 == IDI_Main_Loop_Testing( (struct idi_dataset *) dataset ) )
					{
						goto Main_Termination;
					}
					break;
			}

			if ( global_loop_command )
			{
				if ( global_loop_delay_ms > 0 )
				{
#if defined( __MSDOS__ )
					delay( global_loop_delay_ms );
#endif
				}

				if ( global_loop_count > 0 )
				{
					global_loop_count_counter--;
					if ( 0 == global_loop_count_counter ) global_loop_command = false;
				}

				if ( true == global_loop_space )
				{ /* loop structure to test key strokes */
					int  character;
					BOOL not_done;

					not_done = true;
					do
					{
						if ( Character_Get( &character ) )
						{
							if ( ' ' == ( (char) character) )
							{ /* space, ok do command again */
								not_done = false;
							}
							else
							{ /* not a space, so exit */
								not_done            = false;
								global_loop_command = false;
							}
						}
					} while( true == not_done );
				}
				else
				{
					if ( Character_Get( NULL ) ) global_loop_command = false;
				}

				//if ( Character_Get(NULL) ) global_loop_command = false;
			}

		} while ( global_loop_command );
    }
   	else
   	{ /* produce help */
   		Help( false /* pause */, stdout );
   		goto Main_Termination_Skip_Termination;
   	}

Main_Termination:

#if defined( MAIN_DEBUG )
printf( "Main:  Main_Termination\n" );
#endif
   	Termination( board_id );

Main_Termination_Skip_Termination:

#if defined( MAIN_DEBUG )
printf( "Main:  Main_Termination_Skip_Termination\n" );
#endif

   	return error_code;

Main_Termination_Error_Codes:

	Termination( board_id );

Main_Termination_Error_Codes_Skip_Termination:
	printf( "ERROR: %d, %s\n",  error_code, EC_Code_To_Human_Readable( error_code ) );
	return error_code;
}
