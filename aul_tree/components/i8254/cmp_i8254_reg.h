#ifndef  CMP_I8254_REG_H_
#define  CMP_I8254_REG_H_
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

/*******************************************************************/
#define I8254_CT_QTY		3

/******************************************************************
Description
Counter / Timer modes of operation
*/
enum i8254_mode
{
	I8254_MODE_INTERRUPT_ON_TERMINAL_COUNT		= 0,/**< 000 = Mode 0, Interrupt on terminal count					*/
	I8254_MODE_HARDWARE_RETRIGGERABLE_ONE_SHOT	= 1,/**< 001 = Mode 1, Hardware retriggerable one-shot				*/
	I8254_MODE_RATE_GENERATOR					= 2,/**< X10 = Mode 2, Rate generator								*/
	I8254_MODE_SQUARE_WAVE_GENERATOR			= 3,/**< X11 = Mode 3, Square wave generator						*/
	I8254_MODE_SOFTWARE_TRIGGERED_STROBE		= 4,/**< 100 = Mode 4, Software triggered strobe					*/
	I8254_MODE_HARDWARE_TRIGGER_STROBE			= 5,/**< 101 = Mode 5, Hardware triggered strobe (retriggerable)	*/
	I8254_MODE_READBACK_CHAN_0					= 1,
	I8254_MODE_READBACK_CHAN_1					= 2, 
	I8254_MODE_READBACK_CHAN_2					= 4,
	I8254_MODE_READBACK_CHAN_ALL				= 7
};
typedef enum i8254_mode I8254_MODE_ENUM;
/*******************************************************************/
enum i8254_rw
{
	I8254_RW_LATCH		= 0,
	I8254_RW_LSB		= 1,
	I8254_RW_MSB		= 2,
	I8254_RW_LSB_MSB	= 3,
	I8254_RW_CNT_STA	= 0, /**< CNT = count and STA = status		*/
	I8254_RW_CNT		= 1, /**< CNT = count						*/
	I8254_RW_STA		= 2  /**< STA = status						*/
};
typedef enum i8254_rw I8254_RW_ENUM;

/*******************************************************************/
enum i8254_type
{
	I8254_TYPE_BINARY = 0,
	I8254_TYPE_BCD    = 1
};
typedef enum i8254_type I8254_TYPE_ENUM;
/*******************************************************************/
enum i8254_channel
{
	I8254_CHANNEL_0			= 0x0,
	I8254_CHANNEL_1			= 0x1,
	I8254_CHANNEL_2			= 0x2,
	I8254_CHANNEL_READBACK  = 0x3
};
typedef enum i8254_channel I8254_CHANNEL_ENUM;

/******************************************************************/
#define I8254_CT_QTY   3

/******************************************************************/
#define I8254_READBACK_LATCH_STATUS (~0x10)
#define I8254_READBACK_LATCH_COUNT  (~0x20)
#define I8254_READBACK_LATCH_NONE   ( ~( I8254_READBACK_LATCH_STATUS & I8254_READBACK_LATCH_COUNT ) )
/******************************************************************/

/*---------------------------------------------*/
#define I8254_CTRL_BITS_BCD		1
#define I8254_CTRL_BITS_MODE	3
#define I8254_CTRL_BITS_RW		2
#define I8254_CTRL_BITS_SC		2
/*---------------------------------------------*/
#define I8254_CTRL_MASK_BCD		( (1 << I8254_CTRL_BITS_BCD	) -1 )
#define I8254_CTRL_MASK_MODE	( (1 << I8254_CTRL_BITS_MODE) -1 )
#define I8254_CTRL_MASK_RW		( (1 << I8254_CTRL_BITS_RW	) -1 )
#define I8254_CTRL_MASK_SC		( (1 << I8254_CTRL_BITS_SC	) -1 )
/*---------------------------------------------*/
#define I8254_CTRL_SHIFT_BCD	0
#define I8254_CTRL_SHIFT_MODE	( I8254_CTRL_SHIFT_BCD	+ I8254_CTRL_BITS_BCD	)
#define I8254_CTRL_SHIFT_RW		( I8254_CTRL_SHIFT_MODE + I8254_CTRL_BITS_MODE	)
#define I8254_CTRL_SHIFT_SC		( I8254_CTRL_SHIFT_RW	+ I8254_CTRL_BITS_RW	)
/*---------------------------------------------*/
#define I8254_CTRL_CREATE(sc,rw,mode,bcd)								\
	(	(uint8_t)														\
		( ((sc)   & I8254_CTRL_MASK_SC	) << I8254_CTRL_SHIFT_SC  )  |	\
		( ((rw)   & I8254_CTRL_MASK_RW	) << I8254_CTRL_SHIFT_RW  )  |	\
		( ((mode) & I8254_CTRL_MASK_MODE) << I8254_CTRL_SHIFT_MODE)  |	\
		( ((bcd)  & I8254_CTRL_MASK_BCD	) << I8254_CTRL_SHIFT_BCD )  )	
/*---------------------------------------------*/
#define I8254_CTRL_SET_SC(sc)   ( (uint8_t) ( ((sc) & I8254_CTRL_MASK_SC ) << I8254_CTRL_SHIFT_SC ) )
#define I8254_CTRL_SET_RW(rw)   ( (uint8_t) ( ((rw) & I8254_CTRL_MASK_RW ) << I8254_CTRL_SHIFT_RW ) )
/*---------------------------------------------*/
#define I8254_CTRL_GET_BCD(reg)   ( (I8254_TYPE_ENUM)    ( ( (reg) >> I8254_CTRL_SHIFT_BCD  ) & I8254_CTRL_MASK_BCD  ) )
#define I8254_CTRL_GET_MODE(reg)  ( (I8254_MODE_ENUM)    ( ( (reg) >> I8254_CTRL_SHIFT_MODE ) & I8254_CTRL_MASK_MODE ) )
#define I8254_CTRL_GET_RW(reg)	  ( (I8254_RW_ENUM)      ( ( (reg) >> I8254_CTRL_SHIFT_RW   ) & I8254_CTRL_MASK_RW   ) )
#define I8254_CTRL_GET_SC(reg)	  ( (I8254_CHANNEL_ENUM) ( ( (reg) >> I8254_CTRL_SHIFT_SC   ) & I8254_CTRL_MASK_SC   ) )
/******************************************************************
Description
Register set definition.
Used to build the register_table as well as enumeration of table indices.
*/
/* enum_name, index or row, address offset, byte_width, reg-aperture-bytes, r/w, name */
#define I8254_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol          row    offset  width  aperture        r/w           acronym	 state */ \
	_( I8254_CT0,		0,		0,		1,		1,		REG_DIR_READ_WRITE,		"ct0",		0	) \
	_( I8254_CT1,		1,		1,		1,		1,		REG_DIR_READ_WRITE,		"ct1",		0	) \
	_( I8254_CT2,		2,		2,		1,		1,		REG_DIR_READ_WRITE,		"ct2",		0	) \
	_( I8254_CONTROL,	3,		3,		1,		1,		REG_DIR_WRITE,			"ctrl",		0	)

/*******************************************************************/
typedef enum
{
	I8254_REGISTER_SET_DEFINITION( REG_EXTRACT_ENUM )
} CMP_I8254_REG_ENUM;
/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/
#endif /* CMP_I8254_REG_H_ */
