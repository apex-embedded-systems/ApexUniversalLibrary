#ifndef DEVICE_TRACERE
#define DEVICE_TRACERE		   17 /* device ID number */
#define DEVICE_TRACERE_NAME	"tracere"
/*******************************************************************

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
#include "../../shared/callconv.h"
#include "../../devmgr/devmgr.h"   /* device_definition_type */
#include "../../components/i8255m0/cmp_i8255m0.h"
#include "../../components/i8254/cmp_i8254.h"

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
/* #define MODULE_TRACERE	DEVICE_TRACERE  */ /* base ID -- module ID */

/*****************************************************/
/*****************************************************/

/******************************************************/
/*   TRACERE FIRMWARE VERSION SUB-SECTION                                                             */
/******************************************************/

/* Firmware release November 2, 2002. -- implied revision */
enum { TRACERE_REVISION_021102            =      0 };



/*******************************************************************//**
\ingroup tracere_led
\enum tracere_led
 */
enum tracere_led
{
	TRACERE_LED_BASE_ADDRESS		 =  0,  /**< valid read-write bus transactions	*/
	TRACERE_LED_CT0_INPUT			 =  1,	/**< I8254 channel 0 clock input. See ::tracere_clk_ct0	*/
	TRACERE_LED_CT1_INPUT			 =  2,  /**< I8254 channel 1 clock input. See ::tracere_clk_ct1	*/
	TRACERE_LED_CT2_INPUT			 =  3,  /**< I8254 channel 2 clock input. See ::tracere_clk_ct2	*/
	TRACERE_LED_CT0_OUTPUT			 =  4,  /**< I8254 channel 0 output				*/
	TRACERE_LED_CT1_OUTPUT			 =  5,  /**< I8254 channel 1 output				*/
	TRACERE_LED_CT2_OUTPUT			 =  6,  /**< I8254 channel 2 output				*/
	TRACERE_LED_INTERRUPT_SRCA		 =  7,	/**< interrupt source A					*/
	TRACERE_LED_IRQ_IN0				 =  8,  /**< IRQ_IN0 at J12.22 (J5.1 to J5.2)	*/
	TRACERE_LED_IRQ_IN1				 =  9,  /**< IRQ_IN1 at J9.22 (J7.1 to J7.2)	*/
	TRACERE_LED_IRQ_IN2				 = 10,  /**< IRQ_IN2 at J13.8					*/
	TRACERE_LED_NONE				 = 11,  /**< LED always off						*/
	TRACERE_LED_INTERRUPT_SRCB		 = 12,	/**< interrupt source B					*/
	TRACERE_LED_INTERRUPT_SRCC		 = 13,	/**< interrupt source C					*/
	TRACERE_LED_OFF					 = 14,  /**< LED always off						*/
	TRACERE_LED_ON					 = 15   /**< LED always on						*/
};
/*******************************************************************//**
\ingroup tracere_led
\typedef TRACERE_LED_ENUM
 */
typedef enum tracere_led TRACERE_LED_ENUM;
/*****************************************************************************/
enum tracere_component
{ 
	TRACERE_COMPONENT_I8255M0_0		= 0,	/**< Select component I8255M0 associated with J12.		*/
	TRACERE_COMPONENT_I8255M0_1		= 1,	/**< Select component I8255M0 associated with J9.		*/
	TRACERE_COMPONENT_I8254			= 2		/**< Select component I8254 couter-timer.				*/
};
typedef enum tracere_component TRACERE_COMPONENT_ENUM;

/*******************************************************************//**
\ingroup tracere_counter_timer
\enum tracere_clk_ct0
 */
enum tracere_clk_ct0  /* CT0 clock source selection */
{
	TRACERE_CLK_CT0_CT_IN0		= 0,	/**< (default)	*/
	TRACERE_CLK_CT0_CT_IN1		= 1,
	TRACERE_CLK_CT0_CT_OUT2		= 2, 
	TRACERE_CLK_CT0_IRQ_IN2 	= 3,
	TRACERE_CLK_CT0_4_MHZ 		= 4,
	TRACERE_CLK_CT0_10_MHZ 		= 5,
	TRACERE_CLK_CT0_20_MHZ 		= 6,
	TRACERE_CLK_CT0_40_MHZ		= 7
};
/*******************************************************************//**
\ingroup tracere_counter_timer
\typedef TRACERE_CLK_CT0_ENUM
 */
typedef enum tracere_clk_ct0 TRACERE_CLK_CT0_ENUM;
/*******************************************************************//**
\ingroup tracere_counter_timer
\enum tracere_gate_ct0
 */
enum tracere_gate_ct0  /* CT0 gate source selection */
{
	TRACERE_GATE_CT0_CT_GATE0 = 0,	/**< J13.3 (default) 		*/
	TRACERE_GATE_CT0_CT_GATE1 = 1	/**< J13.3				 	*/
};
/*******************************************************************//**
\ingroup tracere_counter_timer
\typedef TRACERE_GATE_CT0_ENUM
 */
typedef enum tracere_gate_ct0 TRACERE_GATE_CT0_ENUM;
/*******************************************************************//**
\ingroup tracere_counter_timer
\enum tracere_clk_ct1
 */
enum tracere_clk_ct1  /* CT1 clock source selection */
{
	TRACERE_CLK_CT1_CT_IN1		= 0,	/**< (default)	*/
	TRACERE_CLK_CT1_CT_OUT0		= 1,
	TRACERE_CLK_CT1_CT_OUT2		= 2, 
	TRACERE_CLK_CT1_IRQ_IN2 	= 3,
	TRACERE_CLK_CT1_4_MHZ 		= 4,
	TRACERE_CLK_CT1_10_MHZ 		= 5,
	TRACERE_CLK_CT1_20_MHZ 		= 6,
	TRACERE_CLK_CT1_40_MHZ		= 7
};
/*******************************************************************//**
\ingroup tracere_counter_timer
\typedef TRACERE_CLK_CT1_ENUM
 */
typedef enum tracere_clk_ct1 TRACERE_CLK_CT1_ENUM;
/*******************************************************************//**
\ingroup tracere_counter_timer
\enum tracere_clk_ct2
 */
enum tracere_clk_ct2  /* CT2 clock source selection */
{
	TRACERE_CLK_CT2_CT_IN0		= 0,	/**< (default)	*/
	TRACERE_CLK_CT2_CT_IN1		= 1,
	TRACERE_CLK_CT2_CT_OUT2		= 2, 
	TRACERE_CLK_CT2_IRQ_IN2 	= 3,
	TRACERE_CLK_CT2_4_MHZ 		= 4,
	TRACERE_CLK_CT2_10_MHZ 		= 5,
	TRACERE_CLK_CT2_20_MHZ 		= 6,
	TRACERE_CLK_CT2_40_MHZ		= 7
};
/*******************************************************************//**
\ingroup tracere_counter_timer
\typedef TRACERE_CLK_CT2_ENUM
 */
typedef enum tracere_clk_ct2 TRACERE_CLK_CT2_ENUM;
/*******************************************************************//**
\ingroup tracere_intr
\enum tracere_intra_source
 */
enum tracere_intra_source
{
	TRACERE_ISA_CT_OUT0		= 0,	/**< (J13.5) (default)			*/
	TRACERE_ISA_CT_OUT1		= 1,	/**< (J13.6)					*/
	TRACERE_ISA_CT_OUT2		= 2,	/**< (J13.11)					*/
	TRACERE_ISA_IRQ_IN0		= 3,	/**< (J12.22)					*/
	TRACERE_ISA_IRQ_IN1		= 4,	/**< (J9.22)					*/
	TRACERE_ISA_IRQ_IN2		= 5,	/**< (J13.8)					*/
	TRACERE_ISA_8255_1_C0	= 6,	/**< (J12.31) 8255 #1 Bit C0	*/
	TRACERE_ISA_8255_2_C0	= 7 	/**< (J9.31)  8255 #2 Bit C0	*/
};
/*******************************************************************//**
\ingroup tracere_intr
\typedef TRACERE_INTRA_SOURCE_ENUM
 */
typedef enum tracere_intra_source  TRACERE_INTRA_SOURCE_ENUM;
/*******************************************************************//**
\ingroup tracere_intr
\enum tracere_intrb_source
 */
enum tracere_intrb_source
{
	TRACERE_ISB_CT_OUT0		= 0,	/**< (J13.5) (default)			*/
	TRACERE_ISB_CT_OUT1		= 1,	/**< (J13.6)					*/
	TRACERE_ISB_CT_OUT2		= 2,	/**< (J13.11)					*/
	TRACERE_ISB_IRQ_IN0		= 3,	/**< (J12.22)					*/
	TRACERE_ISB_IRQ_IN1		= 4,	/**< (J9.22)					*/
	TRACERE_ISB_IRQ_IN2		= 5,	/**< (J13.8)					*/
	TRACERE_ISB_8255_1_C0	= 6,	/**< (J12.31) 8255 #1 Bit C0	*/
	TRACERE_ISB_8255_2_C0	= 7 	/**< (J9.31)  8255 #2 Bit C0	*/
};
/*******************************************************************//**
\ingroup tracere_intr
\typedef TRACERE_INTRB_SOURCE_ENUM
 */
typedef enum tracere_intrb_source  TRACERE_INTRB_SOURCE_ENUM;
/*******************************************************************//**
\ingroup tracere_intr
\enum tracere_intrc_source
 */
enum tracere_intrc_source
{
	TRACERE_ISC_CT_OUT0		= 0,	/**< (J13.5) (default)			*/
	TRACERE_ISC_CT_OUT1		= 1,	/**< (J13.6)					*/
	TRACERE_ISC_CT_OUT2		= 2,	/**< (J13.11)					*/
	TRACERE_ISC_IRQ_IN0		= 3,	/**< (J12.22)					*/
	TRACERE_ISC_IRQ_IN1		= 4,	/**< (J9.22)					*/
	TRACERE_ISC_IRQ_IN2		= 5,	/**< (J13.8)					*/
	TRACERE_ISC_8255_1_C0	= 6,	/**< (J12.31) 8255 #1 Bit C0	*/
	TRACERE_ISC_8255_2_C0	= 7 	/**< (J9.31)  8255 #2 Bit C0	*/
};
/*******************************************************************//**
\ingroup tracere_intr
\typedef TRACERE_INTRC_SOURCE_ENUM
 */
typedef enum tracere_intrc_source  TRACERE_INTRC_SOURCE_ENUM;
/*******************************************************************/
/* Counter/timer config register - considered on large uint16_t register */
#define TRACERE_BITS_GT2_INV   1
#define TRACERE_BITS_CLK2_INV  1
#define TRACERE_BITS_CLK2      3

#define TRACERE_BITS_GT1_INV   1
#define TRACERE_BITS_CLK1_INV  1
#define TRACERE_BITS_CLK1      3

#define TRACERE_BITS_GT0_INV   1
#define TRACERE_BITS_GT0       1
#define TRACERE_BITS_CLK0_INV  1
#define TRACERE_BITS_CLK0      3
/*---------------------------------------------*/
#define TRACERE_MASK_GT2_INV   ( (1 << TRACERE_BITS_GT2_INV		) -1 )
#define TRACERE_MASK_CLK2_INV  ( (1 << TRACERE_BITS_CLK2_INV	) -1 )
#define TRACERE_MASK_CLK2      ( (1 << TRACERE_BITS_CLK2		) -1 )

#define TRACERE_MASK_GT1_INV   ( (1 << TRACERE_BITS_GT1_INV		) -1 )
#define TRACERE_MASK_CLK1_INV  ( (1 << TRACERE_BITS_CLK1_INV	) -1 )
#define TRACERE_MASK_CLK1      ( (1 << TRACERE_BITS_CLK1		) -1 )

#define TRACERE_MASK_GT0_INV   ( (1 << TRACERE_BITS_GT0_INV		) -1 )
#define TRACERE_MASK_GT0       ( (1 << TRACERE_BITS_GT0			) -1 )
#define TRACERE_MASK_CLK0_INV  ( (1 << TRACERE_BITS_CLK0_INV	) -1 )
#define TRACERE_MASK_CLK0      ( (1 << TRACERE_BITS_CLK0		) -1 )
/*---------------------------------------------*/
#define TRACERE_SHIFT_GT2_INV  14 
#define TRACERE_SHIFT_CLK2_INV 11
#define TRACERE_SHIFT_CLK2      8

#define TRACERE_SHIFT_GT1_INV  13
#define TRACERE_SHIFT_CLK1_INV  7
#define TRACERE_SHIFT_CLK1      4

#define TRACERE_SHIFT_GT0_INV  12 
#define TRACERE_SHIFT_GT0      15 
#define TRACERE_SHIFT_CLK0_INV  3
#define TRACERE_SHIFT_CLK0      0
/*---------------------------------------------*/
#define TRACERE_GET_GT2_INV(ctsu)	( (BOOL) ( ((ctsu) >> TRACERE_SHIFT_GT2_INV  ) & TRACERE_MASK_GT2_INV  ) )
#define TRACERE_GET_CLK2_INV(ctsu)	( (BOOL) ( ((ctsu) >> TRACERE_SHIFT_CLK2_INV ) & TRACERE_MASK_CLK2_INV ) )
#define TRACERE_GET_CLK2(ctsu)	( (TRACERE_CLK_CT2_ENUM) ( ((ctsu) >> TRACERE_SHIFT_CLK2 ) & TRACERE_MASK_CLK2 ) )

#define TRACERE_GET_GT1_INV(ctsu)	( (BOOL) ( ((ctsu) >> TRACERE_SHIFT_GT1_INV  ) & TRACERE_MASK_GT1_INV  ) )
#define TRACERE_GET_CLK1_INV(ctsu)	( (BOOL) ( ((ctsu) >> TRACERE_SHIFT_CLK1_INV ) & TRACERE_MASK_CLK1_INV ) )
#define TRACERE_GET_CLK1(ctsu)	( (TRACERE_CLK_CT1_ENUM) ( ((ctsu) >> TRACERE_SHIFT_CLK1 ) & TRACERE_MASK_CLK1 ) )

#define TRACERE_GET_GT0_INV(ctsu)	( (BOOL) ( ((ctsu) >> TRACERE_SHIFT_GT0_INV  ) & TRACERE_MASK_GT0_INV  ) ) 
#define TRACERE_GET_GT0(ctsu)	( (TRACERE_GATE_CT0_ENUM) ( ((ctsu) >> TRACERE_SHIFT_GT0 ) & TRACERE_MASK_GT0 ) ) 
#define TRACERE_GET_CLK0_INV(ctsu)	( (BOOL) ( ((ctsu) >> TRACERE_SHIFT_CLK0_INV ) & TRACERE_MASK_CLK0_INV ) )
#define TRACERE_GET_CLK0(ctsu)	( (TRACERE_CLK_CT0_ENUM) ( ((ctsu) >> TRACERE_SHIFT_CLK0 ) & TRACERE_MASK_CLK0 ) )
/*---------------------------------------------*/
#define TRACERE_SET_GT2_INV(gt2inv)		( (((uint16_t) (gt2inv))  & TRACERE_MASK_GT2_INV )  << TRACERE_SHIFT_GT2_INV  )	
#define TRACERE_SET_CLK2_INV(clk2inv)	( (((uint16_t) (clk2inv)) & TRACERE_MASK_CLK2_INV ) << TRACERE_SHIFT_CLK2_INV )
#define TRACERE_SET_CLK2(clk2)			( (((uint16_t) (clk2))	  & TRACERE_MASK_CLK2 )     << TRACERE_SHIFT_CLK2     )

#define TRACERE_SET_GT1_INV(gt1inv)		( (((uint16_t) (gt1inv))  & TRACERE_MASK_GT1_INV )  << TRACERE_SHIFT_GT1_INV  )
#define TRACERE_SET_CLK1_INV(clk1inv)	( (((uint16_t) (clk1inv)) & TRACERE_MASK_CLK1_INV ) << TRACERE_SHIFT_CLK1_INV )
#define TRACERE_SET_CLK1(clk1)			( (((uint16_t) (clk1))    & TRACERE_MASK_CLK1 )     << TRACERE_SHIFT_CLK1     )

#define TRACERE_SET_GT0_INV(gt0inv)		( (((uint16_t) (gt0inv))  & TRACERE_MASK_GT0_INV )  << TRACERE_SHIFT_GT0_INV  )
#define TRACERE_SET_GT0(gt0)			( (((uint16_t) (gt0))     & TRACERE_MASK_GT0 )      << TRACERE_SHIFT_GT0      )
#define TRACERE_SET_CLK0_INV(clk0inv)	( (((uint16_t) (clk0inv)) & TRACERE_MASK_CLK0_INV ) << TRACERE_SHIFT_CLK0_INV )
#define TRACERE_SET_CLK0(clk0)			( (((uint16_t) (clk0))    & TRACERE_MASK_CLK0 )     << TRACERE_SHIFT_CLK0     )

/*******************************************************************/
/*---------------------------------------------*/
#define TRACERE_BITS_LED	4
#define TRACERE_MASK_LED	( (1 << TRACERE_BITS_LED		) -1 )

/*---------------------------------------------*/

/*******************************************************************/
/*******************************************************************//**
\ingroup tracere_dio
 */
#define TRACERE_I8255M0_QTY 2
/*******************************************************************//**
\ingroup tracere_dio
 */
#define TRACERE_DIO_PORT_QTY  ( TRACERE_I8255M0_QTY * I8255M0_PORT_QTY )
/*******************************************************************//**
\ingroup tracere_dio
 */
#define TRACERE_DIO_BIT_QTY   ( TRACERE_I8255M0_QTY * I8255M0_BITS_QTY )

/*******************************************************************//**
\ingroup tracere_dio
\struct tracere_dio_cfg
 */
struct tracere_dio_cfg
{
	SYS_TYPE			type;   /* SYS_TYPE__tracere_dio_cfg_type */
	i8255m0_cfg_type	i8255m0_cfg[TRACERE_I8255M0_QTY];
#if(0)
	struct
	{
		/* Sets the port directions.  
			0 = input
			1 = output
		*/
		BOOL	direction[TRACERE_DIO_PORT_QTY];
		BOOL	direction_use; /* over-rides i8255m0_cfg settings */

		/* Maps the logical port number to the physical port number.
		   
		   If the array is all zeros, then we revert to i8255m0 
		   default settings.  
		   
		   If non-zero, then this portion of the configuration
		   will over-ride the i8255m0_cfg values.
		*/
		uint8_t		port_physical[TRACERE_DIO_PORT_QTY]; 
		BOOL		port_physical_use;
		/* Maps the logical dio number to the physical port number.
		   
		   If the array is all zeros, then we revert to i8255m0 
		   default settings.  
		   
		   If non-zero, then this portion of the configuration
		   will over-ride the i8255m0_cfg values.
		*/
		uint8_t		bit_physical[TRACERE_DIO_BIT_QTY]; 
		BOOL		bit_physical_use;
		/*
			The software must reconcile all of the above parameters
			to make sure that all the mappings and directionality
			can all co-exist properly.
		*/
	} abstraction;
#endif
};
/*******************************************************************//**
\ingroup tracere_dio
\typedef tracere_dio_cfg_type
 */
typedef struct tracere_dio_cfg tracere_dio_cfg_type;
/*******************************************************************/
enum { TRACERE_CT_QTY = I8254_CT_QTY };
/*******************************************************************//**
\ingroup tracere_counter_timer
\struct tracere_ct_cfg
 */
struct tracere_ct_cfg  
{
	SYS_TYPE					type;   /* SYS_TYPE__tracere_ct_cfg_type */

	struct
	{
		BOOL					clock_invert;
		BOOL					gate_invert;
		TRACERE_CLK_CT0_ENUM	clock_source;
		TRACERE_GATE_CT0_ENUM	gate_source;
	} ct0;

	struct
	{
		BOOL					clock_invert;
		BOOL					gate_invert;
		TRACERE_CLK_CT1_ENUM	clock_source;
	} ct1;

	struct
	{
		BOOL					clock_invert;
		BOOL					gate_invert;
		TRACERE_CLK_CT2_ENUM	clock_source;
	} ct2;

	i8254_cfg_type				i8254_cfg;
};
/*******************************************************************//**
\ingroup tracere_counter_timer
\typedef tracere_dio_cfg_type
 */
typedef struct tracere_ct_cfg tracere_ct_cfg_type;
/*******************************************************************/
#define TRACERE_INTR_QTY  3
/*******************************************************************//**
\ingroup tracere_intr
\struct tracere_intr_cfg
 */
struct tracere_intr_cfg  
{
	SYS_TYPE					type;   /* SYS_TYPE__tracere__intr_cfg_type */
	BOOL						enable_a;
	BOOL						enable_b;
	BOOL						enable_c;
	TRACERE_INTRA_SOURCE_ENUM	source_a;  /* IRQ driven at J10 */
	TRACERE_INTRB_SOURCE_ENUM	source_b;  /* IRQ driven at J8  */
	TRACERE_INTRC_SOURCE_ENUM	source_c;  /* IRQ driven at J6  */
};
/*******************************************************************//**
\ingroup tracere_intr
\typedef tracere_intr_cfg_type
 */
typedef struct tracere_intr_cfg tracere_intr_cfg_type;
/*******************************************************************//**
\ingroup tracere_params
\struct tracere_cfg
 */
struct tracere_cfg
{
	SYS_TYPE				type;   /* SYS_TYPE__tracere_cfg_type */
	tracere_dio_cfg_type	dio;
	tracere_ct_cfg_type		ct;
	tracere_intr_cfg_type	intr;
	TRACERE_LED_ENUM		led;
};
typedef struct tracere_cfg tracere_cfg_type;
/*******************************************************************/
enum tracere_register_offset
{
/* It is encouraged to use the device->components for communicating
   with the i8255s.  However, you can bypass them totally.  Just 
   remember that any data that is persistent (i.e. copies of write
   only registers within the software component module) will become corrupt.   */
	TRACERE_I8255M0_J12_PORTA			=  0,
	TRACERE_I8255M0_J12_PORTB			=  1,
	TRACERE_I8255M0_J12_PORTC			=  2,
	TRACERE_I8255M0_J12_CONFIG			=  3,	/* write only */
	TRACERE_I8255M0_J9_PORTA			=  4,
	TRACERE_I8255M0_J9_PORTB			=  5,
	TRACERE_I8255M0_J9_PORTC			=  6,
	TRACERE_I8255M0_J9_CONFIG			=  7,	/* write only */
/* It is encouraged to use the device->components for communicating
   with the i8254.  However, you can bypass them totally.  Just 
   remember that any data that is persistent (i.e. copies of registers 
   within a software component module) will become corrupt.   */
	TRACERE_I8254_DATA0					=  8,
	TRACERE_I8254_DATA1					=  9,
	TRACERE_I8254_DATA2					= 10,
	TRACERE_I8254_MODE					= 11,
	TRACERE_I8254_CT_CFG_LSB			= 12,
	TRACERE_I8254_CT_CFG_MSB			= 13,

	TRACERE_GENERAL_LSB					= 14,
	TRACERE_GENERAL_MSB					= 15
};
typedef enum tracere_register_offset TRACERE_REGISTER_OFFSET_ENUM;

/*******************************************************************//**
\ingroup tracere_dio
\enum tracere_dio_port
 */
enum tracere_dio_port
{
	TRACERE_DIO_PORT_J12_A			=  0,	
	TRACERE_DIO_PORT_J12_B			=  1,	
	TRACERE_DIO_PORT_J12_C			=  2,
	TRACERE_DIO_PORT_J9_A			=  3,	
	TRACERE_DIO_PORT_J9_B			=  4,	
	TRACERE_DIO_PORT_J9_C			=  5
};
/*******************************************************************//**
\ingroup tracere_dio
\typedef TRACERE_DIO_PORT_ENUM
 */
typedef enum tracere_dio_port TRACERE_DIO_PORT_ENUM;
/*******************************************************************/
#define TRACERE_RESTART_WARM  0x10   /* commits config and initializes hardware */
#define TRACERE_RESTART_COLD  0x00   /* updated data states */

#define TRACERE_RESTART_DIO   0x01
#define TRACERE_RESTART_CT    0x02
#define TRACERE_RESTART_INTR  0x04
#define TRACERE_RESTART_LED   0x08
#define TRACERE_RESTART_ALL   0x0F

/*******************************************************************//**
\ingroup tracere_general
\enum tracere_restart
 */
enum tracere_restart
{
	TRACERE_RESTART_COLD_DIO			= TRACERE_RESTART_COLD | TRACERE_RESTART_DIO,
	TRACERE_RESTART_COLD_CT				= TRACERE_RESTART_COLD | TRACERE_RESTART_CT,
	TRACERE_RESTART_COLD_INTR			= TRACERE_RESTART_COLD | TRACERE_RESTART_INTR,
	TRACERE_RESTART_COLD_LED			= TRACERE_RESTART_COLD | TRACERE_RESTART_LED,
	TRACERE_RESTART_COLD_ALL			= TRACERE_RESTART_COLD | TRACERE_RESTART_ALL,
	TRACERE_RESTART_WARM_DIO			= TRACERE_RESTART_WARM | TRACERE_RESTART_DIO,
	TRACERE_RESTART_WARM_CT				= TRACERE_RESTART_WARM | TRACERE_RESTART_CT,
	TRACERE_RESTART_WARM_INTR			= TRACERE_RESTART_WARM | TRACERE_RESTART_INTR,
	TRACERE_RESTART_WARM_LED			= TRACERE_RESTART_WARM | TRACERE_RESTART_LED,
	TRACERE_RESTART_WARM_ALL			= TRACERE_RESTART_WARM | TRACERE_RESTART_ALL
};
/*******************************************************************//**
\ingroup tracere_general
\typedef TRACERE_RESTART_ENUM
 */
typedef enum tracere_restart TRACERE_RESTART_ENUM;
/*******************************************************************/

/*******************************************************************/

/* 

1. struct tracere_dataset will be defined in dev_tracere.c 

2. remapping of dio will occur at a level above this.  

3.


*/

/*******************************************************************/

#ifndef __KERNEL__

/**********************************************************************************************************/
/**********************************************************************************************************/
/* Digital I/O (DIO) */
/**********************************************************************************************************/

AUL_DLL_API int AULCALLCONV Tracere_DIO_Port_Set( int board, int channel, uint8_t   value );

AUL_DLL_API int AULCALLCONV Tracere_DIO_Port_Get( int board, int channel, uint8_t * value );

AUL_DLL_API int AULCALLCONV Tracere_DIO_Bit_Set( int board, int channel, BOOL   value );

AUL_DLL_API int AULCALLCONV Tracere_DIO_Bit_Get( int board, int channel, BOOL * value );

AUL_DLL_API int AULCALLCONV Tracere_DIO_Bit_Connector_Pin_Name_Get( int channel, const char ** pin_name );

AUL_DLL_API int AULCALLCONV Tracere_DIO_Config_Set( int board, tracere_dio_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_DIO_Config_Get( int board, tracere_dio_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_DIO_Config_Initialize( tracere_dio_cfg_type * config );


/**********************************************************************************************************/
/**********************************************************************************************************/
/* Interrupt (Intr) */
/**********************************************************************************************************/

AUL_DLL_API int AULCALLCONV Tracere_Intr_Config_Set( int board, tracere_intr_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_Intr_Config_Get( int board, tracere_intr_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_Intr_Config_Initialize( tracere_intr_cfg_type * config );


/**********************************************************************************************************/
/**********************************************************************************************************/
/* Counter/Timer (CT) I8254 */
/**********************************************************************************************************/

AUL_DLL_API int AULCALLCONV Tracere_CT_Config_Set( int board, tracere_ct_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_CT_Config_Get( int board, tracere_ct_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_CT_Config_Initialize( tracere_ct_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_I8254_Control_Create( I8254_CHANNEL_ENUM	channel,				
														  I8254_RW_ENUM			rw,	
														  I8254_MODE_ENUM		mode,
														  I8254_TYPE_ENUM		bcd,
														  uint8_t *				i8254_control_word
														);

AUL_DLL_API int AULCALLCONV Tracere_I8254_Do_Freq_Hz( uint8_t control  /* i8254 control word	*/ );

AUL_DLL_API int AULCALLCONV Tracere_I8254_Count_Freq_Hz(	double		requested_hz,	/**< requested frequency in hertz				*/
														double		clock_external_hz,	/**< default, used if not CTn_nMHz				*/
														int			ct_clock_source,	/**< TRACERE_CLK_CTn_4MHZ _10MHZ _20MHZ _40MHZ 	*/
														int			channel,			/**< CT channel									*/
														uint8_t		control,			/**< i8254 control word							*/
														uint16_t *	initial_count,		/**< i8254 count to load						*/
														double *    actual_hz,			/**< actual output frequency     				*/
														double *    error_actual		/**< computed error, x100 to get percentage 	*/
													);

AUL_DLL_API int AULCALLCONV Tracere_I8254_Count_Time_Sec(	double		requested_sec,		/* requested freq		*/
															double		clock_external_hz,	/* default, used if not CTn_nMHz		*/
															int			ct_clock_source,	/* TRACERE_CLK_CTn_4MHZ _10MHZ _20MHZ _40MHZ */
															int			channel,			/* CT channel			*/
															uint8_t		control,			/* i8254 control word	*/
															uint16_t *	initial_count,		/* i8254 count to load	*/
															double *    actual_sec,			/* actual output time interval     */
															double *    error_actual		/* computed error, x100 to get percentage */
														);

AUL_DLL_API int AULCALLCONV Tracere_I8254_Control_Extract( I8254_CHANNEL_ENUM	channel,
														   I8254_RW_ENUM *		rw,	
														   I8254_MODE_ENUM *	mode,
														   I8254_TYPE_ENUM *	bcd,
														   uint8_t 				i8254_control_word
														);

AUL_DLL_API int AULCALLCONV Tracere_CT_CLock_Name_Get( tracere_ct_cfg_type * ct_cfg, int ct_channel, const char ** clock_name );

AUL_DLL_API int AULCALLCONV Tracere_I8254_Transfer( int board, i8254_ct_xfr_type * cxfr );

AUL_DLL_API int AULCALLCONV Tracere_CT_Count_Get( int board, int channel, uint16_t * value );


/**********************************************************************************************************/
/**********************************************************************************************************/
/* LED */
/**********************************************************************************************************/

AUL_DLL_API int AULCALLCONV Tracere_LED_Set( int board, TRACERE_LED_ENUM   value );

AUL_DLL_API int AULCALLCONV Tracere_LED_Get( int board, TRACERE_LED_ENUM * value );


/**********************************************************************************************************/
/**********************************************************************************************************/
/* General */
/**********************************************************************************************************/

AUL_DLL_API int AULCALLCONV Tracere_Revision_Get( int board, uint16_t * id_value );

AUL_DLL_API int AULCALLCONV Tracere_Config_Set( int board, tracere_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_Config_Get( int board, tracere_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_Config_Initialize( tracere_cfg_type * config );

AUL_DLL_API int AULCALLCONV Tracere_Restart( int board, TRACERE_RESTART_ENUM value );

/**********************************************************************************************************/
/**********************************************************************************************************/


#endif


/*******************************************************************/


/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/

extern device_definition_type device_tracere_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* DEVICE_TRACERE */
