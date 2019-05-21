#ifndef  CMP_EGS_REG_H_
#define  CMP_EGS_REG_H_
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
#define EGS_REGISTER_SET_DEFINITION(_) \
/*    enum_symbol         				row    offset  width  aperture        r/w           acronym	  state  */ \
	_( EGS_POSITION_COUNTER_LSB,		 0,		0,		1,		1,		REG_DIR_READ_WRITE,	"poclsb",	0	) \
	_( EGS_POSITION_COUNTER_MSB,		 1,		1,		1,		1,		REG_DIR_READ_WRITE,	"pocmsb",	0	) \
	_( EGS_POSITION_COUNTER,			 2,		0,		2,		2,		REG_DIR_READ_WRITE,	"pocmsb",	0	) \
	_( EGS_PULSE_PRESCALE_LOAD,			 3,		2,		1,		1,		REG_DIR_WRITE,		"ppl",		0	) \
	_( EGS_PULSE_COUNTER,				 4,		2,		1,		1,		REG_DIR_READ,		"pc",		0	) \
	_( EGS_GENERAL_CONTROL,				 5,		3,		1,		1,		REG_DIR_READ_WRITE,	"gcr",		0	) \
	_( EGS_INTERRUPT_CLEAR,				 6,		4,		1,		1,		REG_DIR_WRITE,		"iclr",		0	) \
	_( EGS_BUFFER_COPY_REQUEST,			 7,		5,		1,		1,		REG_DIR_WRITE,		"bcr",		0	) \
	_( EGS_BUFFER_COPY_STATUS,			 8,		5,		1,		1,		REG_DIR_READ,		"bcs",		0	) \
	_( EGS_SAMPLE_COUNT_LSB,			 9,		6,		1,		1,		REG_DIR_READ,		"sclsb",	0	) \
	_( EGS_SAMPLE_COUNT_MSB,			10,		7,		1,		1,		REG_DIR_READ,		"scmsb",	0	) \
	_( EGS_SAMPLE_COUNT,				11,		6,		2,		2,		REG_DIR_READ,		"sc",		0	) \
	_( EGS_AI_INTRASAMPLE_TIMER_LSB,	12,		8,		1,		1,		REG_DIR_READ_WRITE,	"istlsb",	0	) \
	_( EGS_AI_INTRASAMPLE_TIMER_MSB,	13,		9,		1,		1,		REG_DIR_READ_WRITE,	"istmsb",	0	) \
	_( EGS_AI_INTRASAMPLE_TIMER,		14,		8,		2,		2,		REG_DIR_READ_WRITE,	"ist",		0	)

/*******************************************************************/
typedef enum
{
	EGS_REGISTER_SET_DEFINITION( REG_EXTRACT_ENUM )
} CMP_EGS_REG_ENUM;
/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
#ifdef __cplusplus
}
#endif
/*******************************************************************/
#endif /* CMP_EGS_REG_H_ */
