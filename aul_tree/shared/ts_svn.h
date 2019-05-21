#ifndef TIME_STAMP_SUBVERSION_H_
#define TIME_STAMP_SUBVERSION_H_
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
    Subversion $Date: 2015-02-01 15:12:58 -0600 (Sun, 01 Feb 2015) $ time stamp to unique time based serial number.
	  0         1         2         3         4         5 
	  01234567890123456789012345678901234567890123456789012
	 "$Date: 2015-02-01 15:12:58 -0600 (Sun, 01 Feb 2015) $"

	 Item   index range
	 Year   8 to  11
	 Month  13 to 14
	 Day    16 to 17
	 Hour   19 to 20
	 Minute 22 to 23
	 Second 25 to 26

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
#define TSSVN_DIGIT(str,index,mult)  ( (int)( mult * ( (str)[index] == ' ' ? 0 : ( (str)[index] - '0' ) ) ) )
#define TSSVN_DIGIT2(str,start) ( TSSVN_DIGIT(str,start+0,10) + TSSVN_DIGIT(str,start+1,1) )
#define TSSVN_DIGIT4(str,start) ( TSSVN_DIGIT(str,start+0,1000) + TSSVN_DIGIT(str,start+1,100) + TSSVN_DIGIT(str,start+2,10) + TSSVN_DIGIT(str,start+3,1) )

/*******************************************************************************
*/
		
#define TSSVN_YEAR_DEC(x)  			( TSSVN_DIGIT4(x, 7)	)
#define TSSVN_MONTH_DEC(x)   		( TSSVN_DIGIT2(x,12) )
#define TSSVN_DAY_DEC(x)   			( TSSVN_DIGIT2(x,15) )

#define TSSVN_HOUR_DEC(x)   		( TSSVN_DIGIT2(x,18) )
#define TSSVN_MINUTE_DEC(x) 		( TSSVN_DIGIT2(x,21) )
#define TSSVN_SECOND_DEC(x)  		( TSSVN_DIGIT2(x,24) )

#define TSSVN_YY_DEC(x)				( TSSVN_YEAR_DEC(x) - 2000 )
#define TSSVN_YYMM_DEC(x)			( ((uint32_t)100) * TSSVN_YY_DEC(x)       + TSSVN_MONTH_DEC(x)  )
#define TSSVN_YYMMDD_DEC(x)			( ((uint32_t)100) * TSSVN_YYMM_DEC(x)     + TSSVN_DAY_DEC(x)    )
#define TSSVN_YYMMDDHH_DEC(x)		( ((uint32_t)100) * TSSVN_YYMMDD_DEC(x)   + TSSVN_HOUR_DEC(x)   ) 
#define TSSVN_YYMMDDHHNN_DEC(x)		( ((uint32_t)100) * TSSVN_YYMMDDHH_DEC(x) + TSSVN_MINUTE_DEC(x) )

#define TSSVN_HH_DEC(x)				( TSSVN_HOUR_DEC(x) )
#define TSSVN_HHNN_DEC(x)			( ((uint32_t)100) * TSSVN_HH_DEC(x)     + TSSVN_MINUTE_DEC(x) )
#define TSSVN_HHNNSS_DEC(x)			( ((uint32_t)100) * TSSVN_HHNN_DEC(x)   + TSSVN_SECOND_DEC(x) )

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/* Time stamp in hexadecimal format

	 This format is easy to spot and read in binary data and can be
	 printed in hex format: printf( "0x%X", CTTS_YYMMDDHH_HEX ).
	 
	 CTTS_YYMMDDHH_HEX consumes eight nibbles or four bytes and can only
	 fit reliably into an unsigned 32-bit integer.
*/
/******************************************************************************/


/*******************************************************************************
*/
#define TSSVN_DIGIT2H(str,start) ( TSSVN_DIGIT(str,start+0,0x10) + TSSVN_DIGIT(str,start+1,0x01) )
#define TSSVN_DIGIT4H(str,start) ( TSSVN_DIGIT(str,start+0,0x1000) + TSSVN_DIGIT(str,start+1,0x100) + TSSVN_DIGIT(str,start+2,0x10) + TSSVN_DIGIT(str,start+3,0x01) )

/*******************************************************************************
*/

#define TSSVN_YEAR_HEX(x)    	( TSSVN_DIGIT4H(x, 7) )
#define TSSVN_MONTH_HEX(x)   	( TSSVN_DIGIT2H(x,12) )
#define TSSVN_DAY_HEX(x)   		( TSSVN_DIGIT2H(x,15) )
#define TSSVN_HOUR_HEX(x)   	( TSSVN_DIGIT2H(x,18) )
#define TSSVN_MINUTE_HEX(x) 	( TSSVN_DIGIT2H(x,21) )
#define TSSVN_SECOND_HEX(x)  	( TSSVN_DIGIT2H(x,24) )

#define TSSVN_BITS_PER_NIBBLE	4
#define TSSVN_YY_HEX(x)			( TSSVN_YEAR_HEX(x) & 0x00FF )
#define TSSVN_YYMM_HEX(x)		( (((uint32_t)TSSVN_YY_HEX(x)    ) << (2 * TSSVN_BITS_PER_NIBBLE)) | TSSVN_MONTH_HEX(x) )
#define TSSVN_YYMMDD_HEX(x)	( (((uint32_t)TSSVN_YYMM_HEX(x)  ) << (2 * TSSVN_BITS_PER_NIBBLE)) | TSSVN_DAY_HEX(x)   )
#define TSSVN_YYMMDDHH_HEX(x)	( (((uint32_t)TSSVN_YYMMDD_HEX(x)) << (2 * TSSVN_BITS_PER_NIBBLE)) | TSSVN_HOUR_HEX(x)  )

#define TSSVN_HH_HEX(x)			( TSSVN_HOUR_HEX(x) )
#define TSSVN_HHNN_HEX(x)		( (((uint32_t)TSSVN_HH_HEX(x)    ) << (2 * TSSVN_BITS_PER_NIBBLE)) | TSSVN_MINUTE_HEX(x) )
#define TSSVN_HHNNSS_HEX(x)	( (((uint32_t)TSSVN_HHNN_HEX(x)  ) << (2 * TSSVN_BITS_PER_NIBBLE)) | TSSVN_SECOND_HEX(x) ) 


#endif /* TIME_STAMP_SUBVERSION_H_ */
