#ifndef  CMP_SYSCON_REG_H_
#define  CMP_SYSCON_REG_H_
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


/******************************************************************
Description
Register set definition.
Used to build the register_table as well as enumeration of table indices.
*/
/* enum_name, index or row, address offset, byte_width, reg-aperture-bytes, r/w, name */
#define SYSCON_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol          row    offset  width  aperture        r/w           acronym	   */	  \
	_( SYSCON_ID,			0,	 0x00,		1,		1,		REG_DIR_READ,		"id"			) \
	_( SYSCON_STATUS,		1,	 0x01,		1,		1,		REG_DIR_READ,		"status"		) \
	_( SYSCON_RUN,			2,	 0x02,		1,		1,		REG_DIR_READ_WRITE,	"run"			) \
	_( SYSCON_SW_RESET,		3,	 0x04,		1,		1,		REG_DIR_WRITE,		"reset"			) \
	_( SYSCON_PERMISSION,	4,	 0x10,		1,		1,		REG_DIR_READ_WRITE,	"permission"	)

/*******************************************************************/
typedef enum
{
	SYSCON_REGISTER_SET_DEFINITION( REG_EXTRACT_ENUM )
} CMP_SYSCON_REG_ENUM;
/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/
#endif /* CMP_SYSCON_REG_H_ */
