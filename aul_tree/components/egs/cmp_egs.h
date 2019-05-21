#ifndef COMPONENT_EGS
#define COMPONENT_EGS		   30 /* component ID number */
#define COMPONENT_EGS_NAME	"egs"
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
#include "cmp_egs_reg.h"
#include "../../cmpmgr/cmpmgr.h"


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************//**
\ingroup egs
\enum egs_lcm
 */
enum egc_lcm
{
	EGS_LCM_X1					= 0, /**< quadrature encoding input x1 counting or normal (default) */
	EGS_LCM_X2					= 1, /**< quadrature encoding input x2 counting						*/
	EGS_LCM_X4					= 2, /**< quadrature encoding input x4 counting						*/
	EGS_LCM_OFF					= 3  /**< quadrature encoding input counting off					*/
};

/*******************************************************************/
#define EGS_AIBT_MIN_SEC   5.0e-9
#define EGS_AIBT_MAX_SEC   16384.75e-6

/******************************************************************
*/
struct egs_cfg
{
	/* set to SYS_TYPE__egs_cfg_type so that the I/O control
	   can confirm that we have the correct data 
	   structure.  Also an indication of allocation.
	*/
	SYS_TYPE	type;   /* SYS_TYPE__egs_cfg_type */
	/* Set to true when we want all inputs/outputs to be 
	   updated one command at one time.  This reduces the
	   number of I/O cycles and speeds up processing 
	   considerably.
	*/
	BOOL		simultaneous_update;

	uint8_t		pulse_prescale_load;		/* PPL[7:0]		*/
	int16_t		position_counter;			/* POC[15:0] 	*/

	/* AI Burst timer (or intra-sample time to be more precise)  	 	*/
	double		ai_intrasample_time_sec;	/* AIBT in seconds 			*/
	int			ai_intrasample_time_ns;		/* AIBT in nanoseconds		*/

	/* General Control register */
	enum egc_lcm	logic_count_multipler;									/* LCM[1:0] */
	BOOL			allow_fifo_fill;										/* AFF 		*/
	BOOL			pulse_counter_will_generate_buf1_to_buf2_transfers;		/* PCXFR	*/
	BOOL			pulse_counter_interrupt_enable;							/* PCIE		*/
	BOOL 			encoder_direction;										/* EDIR		*/
	BOOL			enable_pulse_counter;									/* EPC		*/
};
typedef struct egs_cfg egs_cfg_type;

/******************************************************************
*/
struct egs_cfg_reg
{
	uint8_t				general_control;
	uint8_t				pulse_prescale_load;
	uint16_t			position_counter;
	uint16_t			aibt;
};
typedef struct egs_cfg_reg egs_cfg_reg_type;

/*******************************************************************/
#define EGS_INITIALIZE_DATASET  \
	{	/* type			*/			SYS_TYPE_MAKE( egs_dataset_type ), \
		/* cfg			*/			{ SYS_TYPE_MAKE( egs_cfg_type ) }  \
	}

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
/*
#define EGS_HW_SCAN_AVAILABLE(_) _( EGS_BASE_ID, EGS_Hardware_Scan )
*/

int EGS_Create( component_type * component, size_t instance );

int EGS_Destroy( component_type * component );

int EGS_Config_Initialize( egs_cfg_type * cfg );



/******************************************************************
*/
#if defined( TARGET_CHECK_BUILD )
#endif

extern component_definition_type component_egs_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_EGS */
