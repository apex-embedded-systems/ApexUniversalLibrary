#ifndef COMPONENT_I8254
#define COMPONENT_I8254		   19 /* component ID number */
#define COMPONENT_I8254_NAME	"i8254"
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

#include "../../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../../shared/typebool.h"
#include "cmp_i8254_reg.h"
#include "../../cmpmgr/cmpmgr.h"
#include "../../shared/callconv.h"


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
struct i8254_ct_xfr
{
	SYS_TYPE	type;
	int			error_code;
	uint8_t		control;
	uint16_t	count_write;
	uint16_t	count_read[I8254_CT_QTY];
	uint8_t		status_read[I8254_CT_QTY]; /* readback could be up to three values returned */
};
typedef struct i8254_ct_xfr i8254_ct_xfr_type;

/*******************************************************************/
struct i8254_ct_xfr_array
{	
	SYS_TYPE			type;
	size_t				qty;
	i8254_ct_xfr_type * cxfr;
};
typedef struct i8254_ct_xfr_array i8254_ct_xfr_array_type;
/*******************************************************************/
struct i8254_cfg
{
	SYS_TYPE			type;
	/*  These are commands (write commands only) that are used
		to initialize the counter/timer.

		If any of the cxfr have an SUCCESS != error_code, then the
		data is considered invalid.
	*/
	i8254_ct_xfr_type	cxfr[I8254_CT_QTY];
};
typedef struct i8254_cfg i8254_cfg_type;



/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************
Description
Hardware Scanning

This is used to map in a hardware scanner during hardware
initialization that will occur during Initialize() and/or
Open().  This is a special case for components that provide
address space such as the index-data array implementation.
*/
/*<HWSCAN_BEGIN>*/
/*
#define I8255M0_HW_SCAN_AVAILABLE(_) _( I8255M0_BASE_ID, I8255M0_Hardware_Scan )
*/
/*<HWSCAN_END>*/

int I8254_Create( component_type * component, size_t instance );

int I8254_Destroy( component_type * component );

int I8254_Config_Initialize( i8254_cfg_type * cfg );

/*
uint8_t I8254_Control_Create( I8254_CHANNEL_ENUM	channel,				
							  I8254_RW_ENUM			rw,	
							  I8254_MODE_ENUM		mode,
							  I8254_TYPE_ENUM		bcd
							);
*/

/*******************************************************************/
/*******************************************************************/
/* LIBRARY LAYER ONLY */
/*******************************************************************/
#ifndef __KERNEL__
/*******************************************************************/

AUL_DLL_API int AULCALLCONV I8254_Control_Create(	I8254_CHANNEL_ENUM	channel,				
													I8254_RW_ENUM		rw,	
													I8254_MODE_ENUM		mode,
													I8254_TYPE_ENUM		bcd,
													uint8_t *			i8254_control_word
												);

AUL_DLL_API int AULCALLCONV I8254_Control_Extract(	I8254_CHANNEL_ENUM	channel,
													I8254_RW_ENUM *		rw,	
													I8254_MODE_ENUM *	mode,
													I8254_TYPE_ENUM *	bcd,
													uint8_t 			i8254_control_word
												  );

AUL_DLL_API int AULCALLCONV I8254_Count_Freq_Hz(	double		requested_hz,		/* requested freq		*/
													double		clock_source_hz,	/* default, used if not CTn_nMHz			*/
													int			ct_clock_source,	/* TRACERE_CLK_CTn_4MHZ _10MHZ _20MHZ _40MHZ */
													uint8_t		control,			/* i8254 control word	*/
													uint16_t *	initial_count,		/* i8254 count to load	*/
													double *    actual_hz,			/* actual output frequency     */
													double *    error_actual		/* computed error, x1.0e6 to get ppm */
												);

AUL_DLL_API int AULCALLCONV I8254_Count_Time_Sec(	double		requested_sec,		/* requested freq						*/
													double		clock_source_hz,	/* default, used if not CTn_nMHz		*/
													uint8_t		control,			/* i8254 control word					*/
													uint16_t *	initial_count,		/* i8254 count to load					*/
													double *    actual_sec,			/* actual output time interval			*/
													double *    error_actual		/* computed error, x1.0e6 to get ppm	*/
												);

AUL_DLL_API int AULCALLCONV I8254_Do_Freq_Hz( uint8_t control  /* i8254 control word	*/ );

/* #include <stdio.h>  */
/* #include "../../shared/report.h"  */
/* AUL_DLL_API int AULCALLCONV I8254_Report_Cfg( i8254_cfg_type * cfg, FILE * out, struct report_prefix_list * list ); */

/*******************************************************************/
#endif /* __KERNEL__ */
/*******************************************************************/
/*******************************************************************/

/******************************************************************
*/
#if defined( TARGET_CHECK_BUILD )
#endif

extern component_definition_type component_i8254_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_I8254 */
