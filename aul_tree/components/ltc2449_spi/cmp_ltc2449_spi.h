#ifndef COMPONENT_LTC2449
#define COMPONENT_LTC2449	    23 /* component ID number */
#define COMPONENT_LTC2449_NAME	"ltc2449"
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
#include "cmp_ltc2449reg.h"
#include "../../cmpmgr/cmpmgr.h"
#include "../../shared/callconv.h"


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
struct ltc2449ct_xfr
{
	SYS_TYPE	type;
	int			error_code;
	uint8_t		control;
	uint16_t	count_write;
	uint16_t	count_read[LTC2449_CT_QTY];
	uint8_t		status_read[LTC2449_CT_QTY]; /* readback could be up to three values returned */
};
typedef struct ltc2449ct_xfr ltc2449ct_xfr_type;

/*******************************************************************/
struct ltc2449ct_xfr_array
{	
	SYS_TYPE			type;
	size_t				qty;
	ltc2449ct_xfr_type * cxfr;
};
typedef struct ltc2449ct_xfr_array ltc2449ct_xfr_array_type;
/*******************************************************************/
struct ltc2449cfg
{
	SYS_TYPE			type;
	/*  These are commands (write commands only) that are used
		to initialize the counter/timer.

		If any of the cxfr have an SUCCESS != error_code, then the
		data is considered invalid.
	*/
	ltc2449ct_xfr_type	cxfr[LTC2449_CT_QTY];
};
typedef struct ltc2449cfg ltc2449cfg_type;




/*******************************************************************/
/* to become usable globally */
enum channel
{
	CH00    	=  0,
	CH01		=  1,
	CH02    	=  2,
	CH03		=  3,
	CH04    	=  4,
	CH05		=  5,
	CH06    	=  6,
	CH07		=  7,
	CH08    	=  8,
	CH09		=  9,
	CH10    	= 10,
	CH11		= 11,
	CH12    	= 12,
	CH13		= 13,
	CH14    	= 14,
	CH15		= 15,
	COM			= 255
};
typedef enum channel  CHANNEL_ENUM;


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
#define I8255M0_HW_SCAN_AVAILABLE(_) _( I8255M0_BASE_ID, I8255M0_Hardware_Scan )
*/

int Ltc2449_Create( component_type * component, size_t instance );

int Ltc2449_Destroy( component_type * component );

int Ltc2449_Config_Initialize( ltc2449_cfg_type * cfg );




/*******************************************************************/


struct ltc2449_slot
{
	SYS_TYPE	type;
	uint32_t	op_code;
	uint32_t	delay_ns;
};
typedef struct ltc2449_slot ltc2449_slot_type;

/* sends the entire frame */
int Ltc2449_Spi_Transmit_Frame( component_type * component, ltc2449_slot_type * list, size_t list_qty );

/*	1. Transmit one slot out of a frame.  
	2. If it is the first slot, then first_slot != 0.  This tells the internal state machine
	   to start a new receiving buffer and prepare the last one for readout.  Internally, we
	   have two receive buffers, one that has completed and second one that is in progress.
	3. Data is ready to be read out using the Ltc2449_Spi_Frame_Receive() at the end of the
	   Ltc2449_Spi_Transmit_Slot() when first_slot != 0.
	4. Maximum number of slots per frame is retrieved via Ltc2449_Spi_Frame_Slot_Count_Max() which
	   is typically no more than 32.  
	5. 

Note: Receiving FIFO will be maintained in the Library. 
*/
int Ltc2449_Spi_Transmit_Slot( component_type * component, ltc2449_slot_type slot, int first_slot );

/* maximum number of slots supported within a single frame */
int Ltc2449_Spi_Frame_Slot_Count_Max( component_type * component );

/* Data at frame[0] will correspond to the first slot transmitted and so on. 
   It is expected that the slot_count returned ought to be the same as the number
   of slots transmitted.
*/
int Ltc2449_Spi_Frame_Receive( component_type * component, int frame_size, uint32_t * frame, int * slot_count );



/* total number of op-codes currently in the list */
int Ltc2449_Frame_Slot_Count( component_type * component, int * list_count );
int Ltc2449_Spi_Frame_Transmit( component_type * component, BOOL nonblocking );
int Ltc2449_Spi_Frame_Clear( component_type * component );
int Ltc2449_Spi_Frame_Slot_Get( component_type * component, int index, uint32_t * op_code );
int Ltc2449_Spi_Frame_Slot_Insert( component_type * component, int index, uint32_t * op_code );
int Ltc2449_Spi_Frame_Slot_Remove( component_type * component, int index );

/* Returns the previous buffer read in resulting from the LAST or PREVIOUS outbound list commit.  



*/
int Ltc2449_Spi_Inbound_List_Get( component_type * component, size_t buf_size, uint32_t * buffer, size_t * buf_length );


/*******************************************************************/

AUL_DLL_API char * Ltc2449_Slot_String( ltc2449_slot_type slot );
/*
	String Examples (note: string could be later parsed in LUA):  
		0xA0300000  CH00 CH01 DIF OSR 256 TWOX 0 DELAY 1m
		0xB0300000  CH00 COM  SE  OSR 256 TWOX 0 DELAY 1m
		0xB0F80000  CH00 COM  SE  OSR 32768 TWOX 1 DELAY 100m
*/


AUL_DLL_API int Ltc2449_Opcode_Build(	CHANNEL_ENUM	in_plus,   /* IN+  input */
										CHANNEL_ENUM	in_minus,  /* IN-  input */
										int				speed_hz,   /* to the nearest faster rate */
										uint32_t *		op_code /* returns an opcode, if no error */
									);
/*
1. Must compute, given SPI clock and conversion the actual time required.  From that we can determine
   actual delay time.



*/
AUL_DLL_API int Ltc2449_Slot_Build(	CHANNEL_ENUM		in_plus,   /* IN+  input */
									CHANNEL_ENUM		in_minus,  /* IN-  input */
									LTC2449_OSR_ENUM	osr,   /* to the nearest faster rate */
									int					delay_ns,
									ltc2449_slot_type *	slot /* returns an opcode, if no error */
								  );




/******************************************************************
*/
#if defined( TARGET_CHECK_BUILD )
#endif

extern component_definition_type component_ltc2449_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_LTC2449 */