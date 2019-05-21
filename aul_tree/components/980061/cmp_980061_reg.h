#ifndef  CMP_980061_REG_H_
#define  CMP_980061_REG_H_
/********************************************************************************
  _____________________________________________________________________________
  Description:

  _____________________________________________________________________________
  Remarks:
    _____________________________________________________________________________
  Example:
    _____________________________________________________________________________
  Copyright Notice:
    \COPYRIGHT NOTICE Copyright (c) 2010 by Apex Embedded
    Systems. All rights reserved. This document is the
    confidential property of Apex Embedded Systems Any
    reproduction or dissemination is prohibited unless
    specifically authorized in writing.
  _____________________________________________________________________________
  TODO:

  ********************************************************************************/

#include "../../reg/reg.h"


/******************************************************************
Description
Register set definition.
Used to build the register_table as well as enumeration of table indices.
*/

/*   BANK Register - bit weighting
 *   7 6 5 4 3 2 1 0
 *   1 0 2 X X X X X   write
 *
 *   bank     register
 *   X			don't care			 registers independent of banking.
 *   0			0x00   0000 0000
 *   1			0x40   0100 0000
 *   2			0x80   1000 0000
 *   3			0xC0   1100 0000apex
 *
 *   4			0x20   0010 0000
 *   5			0x60   0110 0000
 *   6			0xA0   1010 0000
 *   7			0xE0   1110 0000
 *
 */

enum
{
	980061_BANK_0				= 0x0000,
	980061_BANK_1				= 0x0040,
	980061_BANK_2				= 0x0080,
	980061_BANK_3				= 0x00C0,
	980061_BANK_4				= 0x0020,
	980061_BANK_5				= 0x0060,
	980061_BANK_6				= 0x00A0,
	980061_BANK_7				= 0x00E0,
	980061_BANK_NONE			= REG_BANK_NONE
};

/*  Register addressing hierachy
 *
 *   980061 monolithic component
 *          |
 *    bank component
 *          |
 *      Registers
 *          |
 *         IO
 */


/* enum_name, index or row, address offset, byte_width, reg-aperture-bytes, r/w, name */

/*
 *  The 'row' becomes the logical address.
 *
 *  When using the bank component, then auto-indexing will work correctly across registers.  The
 *  only reason to use the auto-indexing is for purposes of simplifying the output/input of complete
 *  data structures.
 *
 */

#define 980061_GROUP_QTY   6

struct 980061_digital_inputs_registers
{
	uint8_t		dig[980061_GROUP_QTY];
};

struct 980061_pending_clear_registers
{
	uint8_t		isbg;   				/* interrupt by group 	*/
	uint8_t		pc[980061_GROUP_QTY];
};

struct 980061_data_registers
{
	uint8_t		dig[980061_GROUP_QTY];
	uint8_t		ise
	uint8_t		pc[980061_GROUP_QTY];
};

struct 980061_config_registers
{

	uint8_t		ep[980061_GROUP_QTY];	/* edge polarity 		*/
	uint8_t		ibe[980061_GROUP_QTY];	/* interrupt bit enable */
};

struct 980061_config
{
	int		type;
	struct 980061_config_registers		reg;
};


//code snippet
/*
	struct 980061_config   cfg;
	Load_Block( &(cfg.reg), sizeof( cfg.reg ) );
*/

#define 980061_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol          			row    offset  width  aperture        r/w          acronym	    state (or bank)	   */ \
	/** DATA REGISTERS ***/ \
	_( 980061_DI_GROUP0,			 0,		0,		1,		1,		REG_DIR_READ,		"dig0",		980061_BANK_NONE	) \
	_( 980061_DI_GROUP1,			 1,		1,		1,		1,		REG_DIR_READ,		"dig1",		980061_BANK_NONE	) \
	_( 980061_DI_GROUP2,			 2,		2,		1,		1,		REG_DIR_READ,		"dig2",		980061_BANK_NONE	) \
	_( 980061_DI_GROUP3,			 3,		3,		1,		1,		REG_DIR_READ,		"dig3",		980061_BANK_NONE	) \
	_( 980061_DI_GROUP4,			 4,		4,		1,		1,		REG_DIR_READ,		"dig4",		980061_BANK_NONE	) \
	_( 980061_DI_GROUP5,			 5,		5,		1,		1,		REG_DIR_READ,		"dig5",		980061_BANK_NONE	) \
	/** STATUS REGISTER ***/ \
	_( 980061_INTR_BY_GROUP,		 6,		6,		1,		1,		REG_DIR_READ_WRITE,	"isbg",		980061_BANK_NONE	) \
	/** DATA / CONTROL REGISTERS ***/ \
	_( 980061_PEND_CLEAR_GROUP0,	 7,		8,		1,		1,		REG_DIR_READ_WRITE,	"pc0",		980061_BANK_3		) \
	_( 980061_PEND_CLEAR_GROUP1,	 8,		9,		1,		1,		REG_DIR_READ_WRITE,	"pc1",		980061_BANK_3		) \
	_( 980061_PEND_CLEAR_GROUP2,	 9,	   10,		1,		1,		REG_DIR_READ_WRITE,	"pc2",		980061_BANK_3		) \
	_( 980061_PEND_CLEAR_GROUP3,	10,	   11,		1,		1,		REG_DIR_READ_WRITE,	"pc3",		980061_BANK_3		) \
	_( 980061_PEND_CLEAR_GROUP4,	11,	   12,		1,		1,		REG_DIR_READ_WRITE,	"pc4",		980061_BANK_3		) \
	_( 980061_PEND_CLEAR_GROUP5,	12,	   13,		1,		1,		REG_DIR_READ_WRITE,	"pc5",		980061_BANK_3		) \
	/** CONTROL REGISTERS ***/ \
	_( 980061_BANK,					13,		7,		1,		1,		REG_DIR_READ_WRITE,	"bank",		980061_BANK_NONE	) \
	_( 980061_ID_LSB,				14,    14,		1,		1,		REG_DIR_READ,		"idlsb",	980061_BANK_NONE	) \
	_( 980061_ID_MSB,				15,    15,		1,		1,		REG_DIR_READ,		"idmsb",	980061_BANK_NONE	) \
	_( 980061_ZERO0,				16,		8,		1,		1,		REG_DIR_READ,		"zb0",		980061_BANK_0		) \
	_( 980061_ZERO1,				17,		9,		1,		1,		REG_DIR_READ,		"zb1",		980061_BANK_0		) \
	_( 980061_ZERO2,				18,	   10,		1,		1,		REG_DIR_READ,		"zb2",		980061_BANK_0		) \
	_( 980061_ZERO3,				19,	   11,		1,		1,		REG_DIR_READ,		"zb3",		980061_BANK_0		) \
	_( 980061_ZERO4,				20,	   12,		1,		1,		REG_DIR_READ,		"zb4",		980061_BANK_0		) \
	_( 980061_ZERO5,				21,	   13,		1,		1,		REG_DIR_READ,		"zb5",		980061_BANK_0		) \
	/** CONFIG REGISTERS ***/ \
	_( 980061_EDGE_GROUP0,			22,	    8,		1,		1,		REG_DIR_READ_WRITE,	"ep0",		980061_BANK_1		) \
	_( 980061_EDGE_GROUP1,			23,	    9,		1,		1,		REG_DIR_READ_WRITE,	"ep1",		980061_BANK_1		) \
	_( 980061_EDGE_GROUP2,			24,	   10,		1,		1,		REG_DIR_READ_WRITE,	"ep2",		980061_BANK_1		) \
	_( 980061_EDGE_GROUP3,			25,	   11,		1,		1,		REG_DIR_READ_WRITE,	"ep3",		980061_BANK_1		) \
	_( 980061_EDGE_GROUP4,			26,	   12,		1,		1,		REG_DIR_READ_WRITE,	"ep4",		980061_BANK_1		) \
	_( 980061_EDGE_GROUP5,			27,	   13,		1,		1,		REG_DIR_READ_WRITE,	"ep5",		980061_BANK_1		) \
	_( 980061_INTR_BIT_GROUP0,		28,		8,		1,		1,		REG_DIR_READ_WRITE,	"ibe0",		980061_BANK_2		) \
	_( 980061_INTR_BIT_GROUP1,		29,		9,		1,		1,		REG_DIR_READ_WRITE,	"ibe1",		980061_BANK_2		) \
	_( 980061_INTR_BIT_GROUP2,		30,	   10,		1,		1,		REG_DIR_READ_WRITE,	"ibe2",		980061_BANK_2		) \
	_( 980061_INTR_BIT_GROUP3,		31,	   11,		1,		1,		REG_DIR_READ_WRITE,	"ibe3",		980061_BANK_2		) \
	_( 980061_INTR_BIT_GROUP4,		32,	   12,		1,		1,		REG_DIR_READ_WRITE,	"ibe4",		980061_BANK_2		) \
	_( 980061_INTR_BIT_GROUP5,		33,	   13,		1,		1,		REG_DIR_READ_WRITE,	"ibe5",		980061_BANK_2		)


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
	_( IDI_INTR_BY_GROUP,		 6,		6,		1,		1,		REG_DIR_READ_WRITE,	"isbg",		IDI_BANK_NONE	) \
	/** DATA / CONTROL REGISTERS ***/ \
	_( IDI_PEND_CLEAR_GROUP0,	 7,		8,		1,		1,		REG_DIR_READ_WRITE,	"pc0",		IDI_BANK_3		) \
	_( IDI_PEND_CLEAR_GROUP1,	 8,		9,		1,		1,		REG_DIR_READ_WRITE,	"pc1",		IDI_BANK_3		) \
	_( IDI_PEND_CLEAR_GROUP2,	 9,	   10,		1,		1,		REG_DIR_READ_WRITE,	"pc2",		IDI_BANK_3		) \
	_( IDI_PEND_CLEAR_GROUP3,	10,	   11,		1,		1,		REG_DIR_READ_WRITE,	"pc3",		IDI_BANK_3		) \
	_( IDI_PEND_CLEAR_GROUP4,	11,	   12,		1,		1,		REG_DIR_READ_WRITE,	"pc4",		IDI_BANK_3		) \
	_( IDI_PEND_CLEAR_GROUP5,	12,	   13,		1,		1,		REG_DIR_READ_WRITE,	"pc5",		IDI_BANK_3		) \
	/** CONTROL REGISTERS ***/ \
	_( IDI_BANK,				13,		7,		1,		1,		REG_DIR_READ_WRITE,	"bank",		IDI_BANK_NONE	) \
	_( IDI_ID_LSB,				14,    14,		1,		1,		REG_DIR_READ,		"idlsb",	IDI_BANK_NONE	) \
	_( IDI_ID_MSB,				15,    15,		1,		1,		REG_DIR_READ,		"idmsb",	IDI_BANK_NONE	) \
	_( IDI_ZERO0,				16,		8,		1,		1,		REG_DIR_READ,		"zb0",		IDI_BANK_0		) \
	_( IDI_ZERO1,				17,		9,		1,		1,		REG_DIR_READ,		"zb1",		IDI_BANK_0		) \
	_( IDI_ZERO2,				18,	   10,		1,		1,		REG_DIR_READ,		"zb2",		IDI_BANK_0		) \
	_( IDI_ZERO3,				19,	   11,		1,		1,		REG_DIR_READ,		"zb3",		IDI_BANK_0		) \
	_( IDI_ZERO4,				20,	   12,		1,		1,		REG_DIR_READ,		"zb4",		IDI_BANK_0		) \
	_( IDI_ZERO5,				21,	   13,		1,		1,		REG_DIR_READ,		"zb5",		IDI_BANK_0		) \
	/** CONFIG REGISTERS ***/ \
	_( IDI_EDGE_GROUP0,			22,	    8,		1,		1,		REG_DIR_READ_WRITE,	"ep0",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP1,			23,	    9,		1,		1,		REG_DIR_READ_WRITE,	"ep1",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP2,			24,	   10,		1,		1,		REG_DIR_READ_WRITE,	"ep2",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP3,			25,	   11,		1,		1,		REG_DIR_READ_WRITE,	"ep3",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP4,			26,	   12,		1,		1,		REG_DIR_READ_WRITE,	"ep4",		IDI_BANK_1		) \
	_( IDI_EDGE_GROUP5,			27,	   13,		1,		1,		REG_DIR_READ_WRITE,	"ep5",		IDI_BANK_1		) \
	_( IDI_INTR_BIT_GROUP0,		28,		8,		1,		1,		REG_DIR_READ_WRITE,	"ibe0",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP1,		29,		9,		1,		1,		REG_DIR_READ_WRITE,	"ibe1",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP2,		30,	   10,		1,		1,		REG_DIR_READ_WRITE,	"ibe2",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP3,		31,	   11,		1,		1,		REG_DIR_READ_WRITE,	"ibe3",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP4,		32,	   12,		1,		1,		REG_DIR_READ_WRITE,	"ibe4",		IDI_BANK_2		) \
	_( IDI_INTR_BIT_GROUP5,		33,	   13,		1,		1,		REG_DIR_READ_WRITE,	"ibe5",		IDI_BANK_2		)

/*******************************************************************/
typedef enum
{
	980061_REGISTER_SET_DEFINITION( REG_EXTRACT_ENUM )
} CMP_980061_REG_ENUM;
/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/
#endif /* CMP_980061_REG_H_ */
