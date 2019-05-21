#ifndef  CMP_<TEMPLATE>_REG_H_
#define  CMP_<TEMPLATE>_REG_H_
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
/* enum_name, index or row, address offset, byte_width, reg-aperture-bytes, r/w, name */
#define <TEMPLATE>_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol          row    offset  width  aperture        r/w           acronym	   */ \
	_( <TEMPLATE>_REG_A,		0,		0,		1,		1,		REG_DIR_READ_WRITE,	"a"			) \
	_( <TEMPLATE>_REG_B,		1,		1,		1,		1,		REG_DIR_READ_WRITE,	"b"			) \
	_( <TEMPLATE>_REG_C,		2,		2,		1,		1,		REG_DIR_READ_WRITE,	"c"			) \
	_( <TEMPLATE>_REG_CFG,		3,		3,		1,		1,		REG_DIR_WRITE,		"cfg"		)

/*******************************************************************/
typedef enum
{
	<TEMPLATE>_REGISTER_SET_DEFINITION( REG_EXTRACT_ENUM )
} CMP_<TEMPLATE>_REG_ENUM;
/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/
#endif /* CMP_<TEMPLATE>_REG_H_ */
