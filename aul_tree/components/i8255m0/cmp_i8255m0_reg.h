#ifndef  CMP_I8255M0_REG_H_
#define  CMP_I8255M0_REG_H_
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
Configuration register patterns for port directions.
*/
#if(0)
#define I8255M0_PDIR_AIN__BIN__CIN	0x9B
#define I8255M0_PDIR_AIN__BIN__COUT	0x92
#define I8255M0_PDIR_AIN__BOUT_CIN	0x99
#define I8255M0_PDIR_AIN__BOUT_COUT	0x90
#define I8255M0_PDIR_AOUT_BIN__CIN	0x8B
#define I8255M0_PDIR_AOUT_BIN__COUT	0x82
#define I8255M0_PDIR_AOUT_BOUT_CIN	0x89
#define I8255M0_PDIR_AOUT_BOUT_COUT	0x80
#endif

enum i8255m0_direction
{
	I8255M0_PDIR_AIN__BIN__CIN	= 0x9B,
	I8255M0_PDIR_AIN__BIN__COUT	= 0x92,
	I8255M0_PDIR_AIN__BOUT_CIN	= 0x99,
	I8255M0_PDIR_AIN__BOUT_COUT	= 0x90,
	I8255M0_PDIR_AOUT_BIN__CIN	= 0x8B,
	I8255M0_PDIR_AOUT_BIN__COUT	= 0x82,
	I8255M0_PDIR_AOUT_BOUT_CIN	= 0x89,
	I8255M0_PDIR_AOUT_BOUT_COUT	= 0x80
};
typedef enum i8255m0_direction I8255M0_DIRECTION_ENUM;
/******************************************************************/
#define I8255M0_PORT_BIT_WIDTH	8
#define I8255M0_A_BITS		I8255M0_PORT_BIT_WIDTH
#define I8255M0_B_BITS		I8255M0_PORT_BIT_WIDTH
#define I8255M0_C_BITS		I8255M0_PORT_BIT_WIDTH
#define I8255M0_BITS_QTY    ( I8255M0_A_BITS + I8255M0_B_BITS + I8255M0_C_BITS ) 

#define I8255M0_PORT_QTY	3

/******************************************************************
Description
Register set definition.
Used to build the register_table as well as enumeration of table indices.
*/
/* enum_name, index or row, address offset, byte_width, reg-aperture-bytes, r/w, name */
#define I8255M0_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol          row    offset  width  aperture        r/w           acronym	 state  */ \
	_( I8255M0_REG_A,		0,		0,		1,		1,		REG_DIR_READ_WRITE,	"a",	0		 ) \
	_( I8255M0_REG_B,		1,		1,		1,		1,		REG_DIR_READ_WRITE,	"b",	0		 ) \
	_( I8255M0_REG_C,		2,		2,		1,		1,		REG_DIR_READ_WRITE,	"c",	0		 ) \
	_( I8255M0_REG_CFG,		3,		3,		1,		1,		REG_DIR_WRITE,		"cfg",	0		 )

/*******************************************************************/
typedef enum
{
	I8255M0_REGISTER_SET_DEFINITION( REG_EXTRACT_ENUM )
} CMP_I8255M0_REG_ENUM;
/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/
#endif /* CMP_I8255M0_REG_H_ */
