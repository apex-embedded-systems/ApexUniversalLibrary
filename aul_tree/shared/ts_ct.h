#ifndef COMPILE_TIME_TIME_STAMP_H_
#define COMPILE_TIME_TIME_STAMP_H_
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

/********************************************************************************
  _____________________________________________________________________________
  Description:
	 Compile Time Time Stamp (CTTS).  The purpose is primarily to automate The
	 time stamping of any source that has been compiled, and the time stamp 
	 provides a unique serialized sequence of software revisions that are 
	 released out to the wild.  The format provides a unique, not repeating
	 serial number (to the resolution you might need) that serves both as a 
	 time stamp and/or sequential serial number.
	 
	 Converts __DATE__ and __TIME__ to particular time stamp formats.
	 
	             01234567890
	 __DATE__    MMM DD YYYY        M=month, D=Day, Y=Year
	 __TIME__    HH:NN:SS           H=hours, N=minutes, S=seconds
	

    _____________________________________________________________________________
  Copyright Notice:
    COPYRIGHT NOTICE Copyright (c) 2010 by Apex Embedded
    Systems. All rights reserved. This document is the
    confidential property of Apex Embedded Systems Any
    reproduction or dissemination is prohibited unless
    specifically authorized in writing.

  _____________________________________________________________________________
  Remarks:
    _____________________________________________________________________________
  Example:
    _____________________________________________________________________________
  License:
    
  _____________________________________________________________________________
  TODO:

  ********************************************************************************/
#include "../shared/typebase.h"



/******************************************************************************/
/******************************************************************************/
/* Time stamp in decimal format

	 This format can be observed by inspection and can be printed
	 using the following example: printf( "%d", CTTS_YYMMDDHH_DEC ).
	 
	 CTTS_YYMMDDHH_DEC consumes six nibbles or three bytes which is
	 slightly more compact that the hex version and will easily fit
	 reliably within a signed 32-bit integer.
*/
/******************************************************************************/


/*******************************************************************************
*/
#define DIGIT(str,index,mult)  ( (int)( mult * ( (str)[index] == ' ' ? 0 : ( (str)[index] - '0' ) ) ) )
#define DIGIT2(str,start) ( DIGIT(str,start+0,10) + DIGIT(str,start+1,1) )
#define DIGIT4(str,start) ( DIGIT(str,start+0,1000) + DIGIT(str,start+1,100) + DIGIT(str,start+2,10) + DIGIT(str,start+3,1) )

/*******************************************************************************
*/
#define CTTS_MONTH_DEC  \
	( __DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? 1 : 6) 	\
	: __DATE__[2] == 'b' ? 2 															\
	: __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 3 : 4) 	\
	: __DATE__[2] == 'y' ? 5 															\
	: __DATE__[2] == 'l' ? 7 															\
	: __DATE__[2] == 'g' ? 8 															\
	: __DATE__[2] == 'p' ? 9 															\
	: __DATE__[2] == 't' ? 10															\
	: __DATE__[2] == 'v' ? 11 : 12)
		
#define CTTS_YEAR_DEC  		( DIGIT4(__DATE__,7)  )
#define CTTS_DAY_DEC   		( DIGIT2(__DATE__,4)  )

#define CTTS_HOUR_DEC   	( DIGIT2(__TIME__,0) )
#define CTTS_MINUTE_DEC 	( DIGIT2(__TIME__,3) )
#define CTTS_SECOND_DEC  	( DIGIT2(__TIME__,6) )

#define CTTS_YY_DEC				( CTTS_YEAR_DEC - 2000 )
#define CTTS_YYMM_DEC			( ((uint32_t)100) * CTTS_YY_DEC       + CTTS_MONTH_DEC  )
#define CTTS_YYMMDD_DEC			( ((uint32_t)100) * CTTS_YYMM_DEC     + CTTS_DAY_DEC    )
#define CTTS_YYMMDDHH_DEC		( ((uint32_t)100) * CTTS_YYMMDD_DEC   + CTTS_HOUR_DEC   ) 
#define CTTS_YYMMDDHHNN_DEC		( ((uint32_t)100) * CTTS_YYMMDDHH_DEC + CTTS_MINUTE_DEC )

#define CTTS_HH_DEC				( CTTS_HOUR_DEC )
#define CTTS_HHNN_DEC			( ((uint32_t)100) * CTTS_HH_DEC     + CTTS_MINUTE_DEC )
#define CTTS_HHNNSS_DEC			( ((uint32_t)100) * CTTS_HHNN_DEC   + CTTS_SECOND_DEC ) 

/******************************************************************************/
/******************************************************************************/
/* Time stamp in hexadecimal format

	 This format is easy to spot and read in binary data and can be
	 printed in hex format: printf( "0x%X", CTTS_YYMMDDHH_HEX ).
	 
	 CTTS_YYMMDDHH_HEX consumes eight nibbles or four bytes and can only
	 fit reliably into an unsigned 32-bit integer.
*/
/******************************************************************************/
/******************************************************************************/

/*******************************************************************************
*/
#define DIGIT2H(str,start) ( DIGIT(str,start+0,0x10) + DIGIT(str,start+1,0x01) )
#define DIGIT4H(str,start) ( DIGIT(str,start+0,0x1000) + DIGIT(str,start+1,0x100) + DIGIT(str,start+2,0x10) + DIGIT(str,start+3,0x01) )

/*******************************************************************************
*/
#define CTTS_MONTH_HEX  \
	( __DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? 0x1 : 0x6) 	\
	: __DATE__[2] == 'b' ? 0x2 																\
	: __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 0x3 : 0x4) 	\
	: __DATE__[2] == 'y' ? 0x5 																\
	: __DATE__[2] == 'l' ? 0x7 																\
	: __DATE__[2] == 'g' ? 0x8 																\
	: __DATE__[2] == 'p' ? 0x9 																\
	: __DATE__[2] == 't' ? 0x10																\
	: __DATE__[2] == 'v' ? 0x11 : 0x12 )

#define CTTS_YEAR_HEX    	( DIGIT4H(__DATE__,7) )
#define CTTS_DAY_HEX   		( DIGIT2H(__DATE__,4) )
#define CTTS_HOUR_HEX   	( DIGIT2H(__TIME__,0) )
#define CTTS_MINUTE_HEX 	( DIGIT2H(__TIME__,3) )
#define CTTS_SECOND_HEX  	( DIGIT2H(__TIME__,6) )

#define BITS_PER_NIBBLE		4
#define CTTS_YY_HEX			( CTTS_YEAR_HEX & 0x00FF )
#define CTTS_YYMM_HEX		( (((uint32_t)CTTS_YY_HEX    ) << (2 * BITS_PER_NIBBLE)) | CTTS_MONTH_HEX )
#define CTTS_YYMMDD_HEX		( (((uint32_t)CTTS_YYMM_HEX  ) << (2 * BITS_PER_NIBBLE)) | CTTS_DAY_HEX   )
#define CTTS_YYMMDDHH_HEX	( (((uint32_t)CTTS_YYMMDD_HEX) << (2 * BITS_PER_NIBBLE)) | CTTS_HOUR_HEX  )

#define CTTS_HH_HEX			( CTTS_HOUR_HEX )
#define CTTS_HHNN_HEX		( (((uint32_t)CTTS_HH_HEX    ) << (2 * BITS_PER_NIBBLE)) | CTTS_MINUTE_HEX )
#define CTTS_HHNNSS_HEX		( (((uint32_t)CTTS_HHNN_HEX  ) << (2 * BITS_PER_NIBBLE)) | CTTS_SECOND_HEX ) 

#define CTTS_TEST   CTTS_YYMMDDHH_HEX

/******************************************************************************/
/******************************************************************************/
/* Time stamp in string format
*/
/******************************************************************************/
/******************************************************************************/
/*#define STR(s) # s
#define XSTR(s) STR(s)
#define CTTS_YEAR_STR  char[] { __DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10] }

const char ctts_year_str[] = { __DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10], '\0' };*/




#define COMPILE_TIME_STRING  __DATE__ " " __TIME__

#if(0)
/*TODO: does not work.*/
#define CTT_VERSION_STAMP_(x)  # x
#define CTTS_VERSION_STAMP(x)  CTT_VERSION_STAMP_(x)

typedef struct ctts_t
{
	uint32_t  date;
	uint32_t	time;
} CTTS_TYPE;

/*const struct ctts_t ctts_date_time_dec = { CTTS_YYMMDD_DEC, CTTS_HHNNSS_DEC };
const struct ctts_t ctts_date_time_hex = { CTTS_YYMMDD_HEX, CTTS_HHNNSS_HEX }; */
#endif


#endif /* COMPILE_TIME_TIME_STAMP_H_ */
