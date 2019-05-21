#ifndef DEVICE_STX104
#define DEVICE_STX104		18    /* device ID number 	*/
#define DEVICE_STX104_NAME	"stx104"
/* valid alias 		*/
#define DEVICE_STX104_EGS   18
#define DEVICE_STX104_NAME_EGS 	"stx104egs"
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

1. split into two header files in order to maintain compatibility
   and also hide things that we want to make opaque to prevent errors?

2. encapsulate enums into structures to create more unique types?
http://stackoverflow.com/questions/8597426/enum-type-check-in-c-gcc

*********************************************************************************/
/**
 *  @defgroup stx104 STX104 Specific Functions
 *  STX104 specific functions found within the Apex Univeral Library.
 */
/** @defgroup stx104_ai Analog Input Functions 
 *  @ingroup stx104
 *  Functions related specifically for STX104 Analog Inputs (AI). 
 */
/** @defgroup stx104_ao Analog Output Functions 
 *  @ingroup stx104
 *  Functions related specifically for STX104 Analog Outputs (AO). 
 */
/** @defgroup stx104_di Digital Input Functions 
 *  @ingroup stx104
 *  Functions related specifically for STX104 Digital Inputs (DI). 
 */
/** @defgroup stx104_do Digital Output Functions 
 *  @ingroup stx104
 *  Functions related specifically for STX104 Digital Outpus (DO). 
 */
/** @defgroup stx104_events Event (Interrupt) Functions 
 *  @ingroup stx104
 *  Functions related specifically for STX104 Interrupts (Events). 
 */
/** @defgroup stx104_general General Functions 
 *  @ingroup stx104
 *  Functions related specifically for STX104 General or board level functions. 
 */
/** @defgroup stx104_trg Triggering Functions
 *  @ingroup stx104
 *  Functions related specifically for STX104 Analog Input Trigger (Trg) functions.
 */

#include "../../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../../shared/typebool.h"
#include "../../devmgr/devmgr.h"
#include "../../shared/callconv.h"

#include "../../components/i8254/cmp_i8254.h"
/* <EGS_BEGIN> */
#include "../../components/egs/cmp_egs.h"
/* <EGS_END> */



/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
/* #define MODULE_STX104		DEVICE_STX104 */  /* base ID -- module ID */

/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 FIRMWARE VERSION SUB-SECTION              */
/******************************************************/

/* Firmware release April 16, 2007. */
enum { STX104_REVISION_071604              =      0 };
/* Firmware release February 16, 2008. */
enum { STX104_REVISION_080214              = 0x1008 };
/* Firmware release April 7, 2008. */
enum { STX104_REVISION_080407              = 0x1008 };
/* Firmware release January 15, 2009. */
enum { STX104_REVISION_090115              = 0x1009 };
/* Firmware release March 28, 2012.  
      - analog output update rate improvement 
	   - analog input twos complement, inversion, etc.
*/
enum { STX104_REVISION_120328              = 0x100A };
/* Firmware release January 1, 2014.
	  - spi added.
     - NOTE: this version still in progress.
*/
enum { STX104_REVISION_140101              = 0x100B };

/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 ANALOG INPUT (AI) SUB-SECTION                                                             */
/******************************************************/

/*****************************************************/
/* Minimum time between analog input samples in nano-seconds. */
#define STX104_AI_INTRA_SAMPLE_TIME_MIN_NS ((double)  5000.0)
/*****************************************************/
/* Maximum STX104 FIFO sample buffer size            */
#define STX104_BUFFER_SIZE  ( 1048576 - 1 )
/*****************************************************/
/* STX104 Analog Input gain ranges. */
typedef enum
{
	STX104_AI_CH0 = 0,
	STX104_AI_CH1,
	STX104_AI_CH2,
	STX104_AI_CH3,
	STX104_AI_CH4,
	STX104_AI_CH5,
	STX104_AI_CH6,
	STX104_AI_CH7,		/* maximum channel for differential mode */
	STX104_AI_CH8,
	STX104_AI_CH9,
	STX104_AI_CH10,
	STX104_AI_CH11,
	STX104_AI_CH12,
	STX104_AI_CH13,
	STX104_AI_CH14,
	STX104_AI_CH15
} STX104_AI_ENUM;
/*****************************************************/
/* STX104 Analog Input frame/sample strobe source;
   AIGC.AISS[3:0] and/or ACR.ALSS[1:0]. The source for analog
   \input sampling rate typically defined by frame rate.      */
typedef enum
{	  
	AISS_SOFTWARE = 0, 			/* hardware default - Software adc sample strobe AISS[3:0]=0000 and ALSS[1:0]=00 */
	AISS_AI_FRAME_TIMER = 1,	/* Analog Input Sample/Frame Timer	*/
	AISS_INT_THRESHOLD_COUNTER,	/* Interrupt Threshold Counter 		*/ 
	AISS_NONE = 3,				/* none - AISS[3:0]=0000 and ALSS[1:0]=00 */
	AISS_DIN0 = 6,				/* Legacy DIN0_RE, AISS[3:0]=0000 and ALSS[1:0]=10 */
	AISS_CT_OUT2 = 7,			/* Legacy CT_OUT2, AISS[3:0]=0000 and ALSS[1:0]=11 */
	AISS_DIN0_RE = 8,			/* DIN0 rising edge  	*/
	AISS_DIN0_FE,					/* DIN0 falling edge  	*/
	AISS_DIN1_RE,					/* DIN1 rising edge  	*/
	AISS_DIN1_FE,					/* DIN1 falling edge  	*/
	AISS_DIN2_RE,					/* DIN2 rising edge  	*/
	AISS_DIN2_FE,					/* DIN2 falling edge  	*/
	AISS_DIN3_RE,					/* DIN3 rising edge  	*/
	AISS_DIN3_FE					/* DIN3 falling edge   	*/
} STX104_AI_STROBE_SOURCE_ENUM;	 /* STX104 Analog Input frame/sample strobe source;
                               	    AIGC.AISS[3:0] and/or ACR.ALSS[1:0]. The source for analog
                               	    \input sampling rate typically defined by frame rate.      */
/*****************************************************/
/** STX104 Analog Input gain ranges; ACFG.GAIN[1:0]. */
typedef enum
{   					/*  BIPOLAR			UNIPOLAR						*/
	               		/*  ADBU=0			ADBU=1							*/
	ACFG_GAIN_10V = 0,	/** Differential: +/-10V. Single-Ended: 0 to 10V. 	*/
	ACFG_GAIN_5V,    	/** Differential: +/-5V. Single-Ended: 0 to 5V. 	*/
	ACFG_GAIN_2_5V,		/** Differential: +/-2.5V. Single-Ended: 0 to 2.5V. */
	ACFG_GAIN_1_25V		/** Differential: +/-1.25V. Single-Ended: 0 to 1.25V. 	*/
} STX104_AI_GAIN_ENUM; 	/** STX104 Analog Input gain ranges; ACFG.GAIN[1:0]. 	*/
/*****************************************************/
typedef enum 
{
	STX104_AIGC_NONE           	= 0x0000,
   STX104_AIGC_AISS            	= 0x000F,
   STX104_AIGC_nSGATE          	= 0x0010,
   STX104_AIGC_SAIFCTS         	= 0x0020,
   STX104_AIGC_SAIFTTS         	= 0x0040,
   STX104_AIGC_MAVG_IN         	= 0x0080,
   STX104_AIGC_ENCODE_BEHAVIOR 	= 0x0300,
	STX104_AIGC_ENCODE 			= 0x0400
} STX104_AI_GENERAL_CONFIG_BITS_MASK_ENUM;
/*****************************************************/
enum { STX104_AI_AIGC_ENCODE_SHIFT = 8 };
/*****************************************************/
/* STX104 Analog Input binary encoding schemes   */
typedef enum
{
	/** Most negative (0x0000) to most positive (0xFFFF)
      Default.  Straight Binary Encoding (SBE)
   */
	AI_ENCODING_STRAIGHT_BINARY							  = 0,

	/** ai_cfg.polarity[channel] = '1': Most negative (0xFFFF) to most positive (0x0000).
		 ai_cfg.polarity[channel] = '0'. Most negative (0x0000) to most positive (0xFFFF).	
	*/
	AI_ENCODING_STRAIGHT_BINARY_ALLOW_POLARITY			   = 1,
	/**  Most negative (0x8000) to most postive (0x7FFF)  */
	AI_ENCODING_TWOS_COMPLEMENT							   = 4,

	/** ai_cfg.polarity[channel] = '1': Most negative (0x7FFF) to most positive (0x8001).
		 ai_cfg.polarity[channel] = '0'. Most negative (0x8001) to most positive (0x7FFF).	
	*/	 
	AI_ENCODING_TWOS_COMPLEMENT_ALLOW_POLARITY			   = 5,

	/** Converts all channels to absolute values.
	    ai_cfg.polarity[channel] = '1': Range: most negative (0x8001) to most positive (0x0000).
       ai_cfg.polarity[channel] = '0'. Range: most negative (0x0000) to most positive (0x7FFF).
	*/
	AI_ENCODING_TWOS_COMPLEMENT_ABSOLUTE				   = 6
	
} STX104_AI_ENCODING_ENUM;
/*****************************************************/
/* STX104 Analog Input configuration data structure type definition. */
#define STX104_AI_QTY  16
struct stx104_ai_cfg
{	
	SYS_TYPE						type;  /* SYS_TYPE__stx104_ai_cfg_type */

	double							capacitance_pf[STX104_AI_QTY]; /** Total capacitance (pF) as seen looking out of an analog
	      								                   				     input. This is used in conjunction with the resistance_ohms
	      								                   				     to calculate minimum time between samples to achieve 16-bit
	      								                   				     resolution (i.e. settle to 16-bits).                         */
	double							resistance_ohms[STX104_AI_QTY];/** Total Thevenin resistance (ohms) at the input of each analog
	      								                    			        input channel. This is used in conjunction with the
	      								                    			        capacitance_pf to calculate minimum time between samples to
	      								                    			        achieve 16-bit resolution (i.e. settle to 16-bits).          */
	STX104_AI_ENUM					first_channel;							/** ACHAN.FC[3:0]. */
	STX104_AI_ENUM					last_channel;							/** ACHAN.LC[3:0]. */
	STX104_AI_GAIN_ENUM		  		gain;										/** ACFG.GAIN[1:0]. */

/** If burst_timer_sec has a non-zero value, it will take precedence over burst_timer_ns.
    If frame_timer_sec has a non-zero value, it will take precedence over frame_timer_ns.
*/
	uint32_t						burst_timer_ns;					/** nSec (driver converts to steps) AIBT.AIBT[31:0]. */
	uint32_t 						frame_timer_ns;					/** nSec (driver converts to steps) AIFT.AIFT[31:0]. */

	double							burst_timer_sec;					/** seconds - changed from UINT32 to double to support larger dynamic range.  Min value 5000 ns */
	double 							frame_timer_sec;					/** seconds - changed from UINT32 to double to support larger dynamic range */
	uint32_t						frame_maximum;						/** AIFM.AIFM[31:0]. */
	BOOL  							moving_average_filter;			/** AIGC.MAVG_INV 	*/
	BOOL  							sync_aift_to_trigger_start;	/** AIGC.SAIFTTS  	*/
	BOOL  							sync_aifc_to_trigger_start;	/** AIGC.SAIFCTS  	*/
	BOOL  							use_triggering_subsystem; 		/** AIGC.nSGATE   	*/
	BOOL							high_speed_pre_queue_fifo;		/** FCFG.HSFIFOEN   */
	STX104_AI_STROBE_SOURCE_ENUM	strobe_source; 					/** Used to start an analog input frame (i.e. ADC-burst) */

	/** start - Firmware 120328H or higher, otherwise ignored */
	STX104_AI_ENCODING_ENUM		encoding;   /** provides a variety of ADC encodings */
	BOOL 						polarity[STX104_AI_QTY];   /** straight binary encoding (SBE) - invert, Twos complement - multiply by -1 */


/*Better method is to have an array of intra-sample timers.  */
/*	BOOL			allow_selective_enable_and_route;  		*/ /* if true, then selective_enable[] and selective_route_to_fifo[] will function    			*/
/*	BOOL 			selective_enable[STX104_AI_QTY];		*/ /* only channels between first_channel and last_channel inclusive will be observed */
/*	BOOL 			selective_route_to_fifo[STX104_AI_QTY]; */ /* only channels between first_channel and last_channel inclusive will be observed */
	/* end -   Firmware 120328H or higher                    */
};
typedef struct stx104_ai_cfg stx104_ai_cfg_type;


/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 TIMING CONSTANTS SUB-SECTION                                                             */
/******************************************************/

/*****************************************************/
/* Minimum clock period used for STX104 internal timers. 
   NOTE: future versions of STX104 may have clock speed
   changes, so please use this macro.  Future clock
   speed will be faster.
*/
#define STX104_CLOCK_RESOLUTION_NS         ((double)    25.0)

/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 GENERAL (Gen) SUB-SECTION                                                             */
/******************************************************/

/*****************************************************/
/* STX104 read-only jumper configuration data structure type
   definition.                                               */
struct stx104_gen_jmp
{
	BOOL M0_mode;
	BOOL M1_mode;
	BOOL M2_dacs_16bits;
	BOOL M3_moving_average_filter;
	BOOL M4_address_10bit_mode;
	BOOL DMA_ch3;
	BOOL CLK_1mhz;
	BOOL J9_ai_unipolar;
	BOOL J8_ai_differential;
};
typedef struct stx104_gen_jmp STX104_GEN_JMP_TYPE;

/*****************************************************/
enum stx104_clk_ct0 /* i8254 CT0 clock source select */
{
	STX104_CLK_CT0_CT_CLK0	= 0, /* external J7.4 */
	STX104_CLK_CT0_100KHz   = 1
};
typedef enum stx104_clk_ct0 STX104_CLK_CT0_ENUM;
/*****************************************************/
enum stx104_gate_ct1_ct2 /* i8254 CT1 & CT2 gate source select */
{
	STX104_GATE_CT1_CT2_FREE	= 0, /* no gating, free run */
	STX104_GATE_CT1_CT2_DIN0	= 1  /* gate via DIN0 J7.12	*/
};
typedef enum stx104_gate_ct1_ct2 STX104_GATE_CT0_CT1_ENUM;
/*****************************************************/
struct stx104_ct_cfg
{
	SYS_TYPE		type;	/* SYS_TYPE__stx104_ct_cfg_type */

	struct
	{
		STX104_CLK_CT0_ENUM		clock_source;  /* controlled by CT_SRC0 bit */
		/* gate, always DIN2 J7.10 */
	} ct0;

	struct
	{
		/* clock source set by 1MHz/10MHz jumper */
		STX104_GATE_CT0_CT1_ENUM	gate_source; /* controlled by GCTRL bit */
	} ct1_ct2;

	i8254_cfg_type					i8254_cfg;
/* <EGS_BEGIN> */
	egs_cfg_type					egs_cfg;
/* <EGS_END> */
};
typedef struct stx104_ct_cfg stx104_ct_cfg_type;
/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 ANALOG OUTPUTS (AO) SUB-SECTION                                                             */
/******************************************************/

/*****************************************************/
enum { STX104_AO_QTY = 2 };
/* STX104 analog output channels */
typedef enum
{
	STX104_AO_A		= 0,
	STX104_AO_B		= 1,
	STX104_AO_CH0	= 0, /* alias names */
	STX104_AO_CH1	= 1
} STX104_AO_ENUM; /* DACA.DA[15:0] and DACB.DB[15:0]. */

/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 DIGITAL INPUTS (DI) SUB-SECTION                                                             */
/******************************************************/

/*****************************************************/
#define STX104_DI_QTY  4
/* STX104 digital input channels */
typedef enum
{
	STX104_DI0 = 0,
	STX104_DI1 = 1,
	STX104_DI2 = 2,
	STX104_DI3 = 3
} STX104_DI_ENUM;	/* STX104 digital input channels; DI.DIN[3:0]. */

/*****************************************************/
/* STX104 Digital Input configuration data structure type
   definition.                                            */
struct stx104_di_cfg
{								
	BOOL deglitch_200ns;		/* DIC.SDGF     			*/ 
	BOOL swap_din1_and_din3;	/* swap digital inputs DIN1 and DIN3, DIC.SDI31. */
	/* If set true, then input is inverted, otherwise the input is
	   non-inverted.                                               */
	struct
	{
		BOOL din3;
		BOOL din2;
		BOOL din1;
		BOOL din0;
	} polarity;					/* DIC.DIP[3:0]. */
};
typedef struct stx104_di_cfg STX104_DI_CFG_TYPE;

/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 DIGITAL OUTPUTS (DO) SUB-SECTION                                                             */
/******************************************************/

/*****************************************************/

#define STX104_DO_QTY  6
/* STX104 digital output channels */
typedef enum
{
	STX104_DO0 = 0,
	STX104_DO1 = 1,
	STX104_DO2 = 2,
	STX104_DO3 = 3,
	STX104_DO4 = 4,
	STX104_DO5 = 5
} STX104_DO_ENUM;	/* STX104 digital output channels; DO.DOUT[5:0]. */
/*****************************************************/
enum stx104_do5  /* CTOUT2SEL[2:0], DOUT5 (CT_OUT2) J7.2   */
{
	STX104_DO5_I8254_CT_OUT2				= 0, /* default - i8254 counter/timer 2 output  */
	STX104_DO5_DOUT5						= 1, /* register STX104_DIGITAL_OUTPUTS, bit 5	*/
	STX104_DO5_ADC_SAMPLE					= 2, /* AI sample pulse							*/
	STX104_DO5_ADC_SAMPLING_ACTIVE			= 3, /* monitor AI sampling active				*/
	STX104_DO5_AI_FRAME_TIMER				= 4, /* monitor AI frame timer output			*/
	STX104_DO5_INTERRUPT_COUNTER_INCREMENT	= 5, /* monitor interrupt counter increments	*/
	STX104_DO5_ZERO							= 6, /* output always zero						*/
	STX104_DO5_IOCHRDY						= 7  /* used to monitor IOCHRDY bus signal		*/
};
typedef enum stx104_do5 STX104_DO5_MAP_ENUM;
/*****************************************************/
enum stx104_do4  /* CTOUT0SEL[2:0], DOUT5 (CT_OUT0) J7.3   */
{
	STX104_DO4_I8254_CT_OUT0				= 0, /* default - i8254 counter/timer 0 output  */
	STX104_DO4_DOUT4						= 1, /* register STX104_DIGITAL_OUTPUTS, bit 4	*/
	STX104_DO4_TRIGGER_DELAY_PULSE_END		= 2, /* Trigger delay end pulse					*/
	STX104_DO4_TRIGGER_DELAY_PULSE_START	= 3, /* Trigger delay start pulse				*/
	STX104_DO4_INTERRUPT_THRESHOLD_COUNTER_MAX_REACHED	= 4, /* interrupt counter max threshold reached	*/
	STX104_DO4_ADC_SAMPLING_ACTIVE			= 5, /* monitor AI sampling active				*/
	STX104_DO4_ADC_DATA_MSB_ADDRESS_DECODE	= 6, /* reading ADC MSB data from FIFO			*/
	STX104_DO4_INTERRUPT					= 7  /* used to monitor interrupt sent to bus	*/
};
typedef enum stx104_do4 STX104_DO4_MAP_ENUM;
/*****************************************************/
typedef enum
{
	STX104_DO3_MAP_DOUT3   = 0,	/* normal, default */
	STX104_DO3_MAP_UNUSED1 = 1,
	STX104_DO3_MAP_UNUSED2 = 2,
	STX104_DO3_MAP_UNUSED3 = 3
} STX104_DO3_MAP_ENUM;	/* STX104 digital output mappings. */
/*****************************************************/
typedef enum
{
	STX104_DO2_MAP_DOUT2   = 0,	/* normal, default */
	STX104_DO2_MAP_SPI_CS  = 1,
	STX104_DO2_MAP_UNUSED2 = 2,
	STX104_DO2_MAP_UNUSED3 = 3
} STX104_DO2_MAP_ENUM;	/* STX104 digital output mappings. */
/*****************************************************/
typedef enum
{
	STX104_DO1_MAP_DOUT1   = 0,	/* normal, default */
	STX104_DO1_MAP_SPI_CLK = 1,
	STX104_DO1_MAP_UNUSED2 = 2,
	STX104_DO1_MAP_UNUSED3 = 3
} STX104_DO1_MAP_ENUM;	/* STX104 digital output mappings. */
/*****************************************************/
typedef enum
{
	STX104_DO0_MAP_DOUT0   = 0,	/* normal, default */
	STX104_DO0_MAP_SPI_DO  = 1,
	STX104_DO0_MAP_UNUSED2 = 2,
	STX104_DO0_MAP_UNUSED3 = 3
} STX104_DO0_MAP_ENUM;	/* STX104 digital output mappings. */

/*****************************************************/
/* STX104 Digital Output configuration data structure type
   definition.                                             */
struct stx104_do_config
{	
	/* start - Firmware 120328H or higher, otherwise ignored */
	STX104_DO3_MAP_ENUM		do3_map;			/*  DOC.DO3_MAP[1:0]	*/
	STX104_DO2_MAP_ENUM		do2_map;			/*  DOC.DO2_MAP[1:0]	*/
	STX104_DO1_MAP_ENUM		do1_map;			/*  DOC.DO1_MAP[1:0]	*/
	STX104_DO0_MAP_ENUM		do0_map;			/*  DOC.DO0_MAP[1:0]	*/
	/* end -   Firmware 120328H or higher								*/
	STX104_DO4_MAP_ENUM		do4_map; /* CTOUT0SEL, DOUT4 J7.3 */
	STX104_DO5_MAP_ENUM		do5_map; /* CTOUT2SEL, DOUT5 J7.2 */

	/* Depending on do4_map or do5_map, these could output 25nS pulses,
	   so in order to capture (especially with an analog scope or low-end
	   frequency counters, we offer pulse extension capability.
	*/
	BOOL					do4_pulse_extend;	/* if true, extend by 100nS */
	BOOL					do5_pulse_extend;	/* if true, extend by 100nS */

	/* If value is true, then the output is inverted, otherwise
	   non-inverted.                                            */
	struct 
	{
		BOOL	sshp;			    /*  DOC.SSHP  			*/
		BOOL	ct_out0;			/*  DOC.CT0OP 			*/
		BOOL	ct_out2;			/*  DOC.CT2OP 			*/
		BOOL	dout3;				/*  DOC.DOP3  			*/
		BOOL	dout2;				/*  DOC.DOP2  			*/
		BOOL	dout1;				/*  DOC.DOP1  			*/
		BOOL	dout0;				/*  DOC.DOP0  			*/
	} polarity;
};
typedef struct stx104_do_config stx104_do_cfg_type; 

/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 TRIGGERING (TRG) SUB-SECTION       		  */
/******************************************************/
typedef enum
{
	STM_IMMEDIATE = 0,		/**< valid start                                    */
	STM_DELAY, 				/**< valid start followed by delay                  */
	STM_SYNC_DELAY			/**< valid start followed by sync followed by delay */
} STX104_TRG_MODE_ENUM; 	/**< TCFG.STM[1:0]. 								*/

/*****************************************************/

typedef enum
{
	TSS_NONE = 0,			/**< none (default) */
	TSS_SOFTWARE_STROBE,	/**< writing 0x5A to the software strobe register  	*/
	TSS_CT_OUT0_RE = 4,		/**< 8254 Counter 0 Output (CT_OUT0) rising-edge  	*/
	TSS_CT_OUT0_FE,			/**< 8254 Counter 0 Output (CT_OUT0) falling-edge  	*/
	TSS_CT_OUT2_RE,			/**< 8254 Counter 2 Output (CT_OUT2) rising-edge  	*/
	TSS_CT_OUT2_FE,			/**< 8254 Counter 2 Output (CT_OUT2) falling-edge  	*/
	TSS_DIN0_RE,			/**< DIN0 rising edge  								*/
	TSS_DIN0_FE,			/**< DIN0 falling edge  							*/
	TSS_DIN1_RE,			/**< DIN1 rising edge  								*/
	TSS_DIN1_FE,			/**< DIN1 falling edge  							*/
	TSS_DIN2_RE,			/**< DIN2 rising edge  								*/
	TSS_DIN2_FE,			/**< DIN2 falling edge  							*/
	TSS_DIN3_RE,			/**< DIN3 rising edge  								*/
	TSS_DIN3_FE				/**< DIN3 falling edge  							*/
} STX104_TRG_START_SYNC_ENUM;	/**< TCFG.TSS[3:0]. 							*/

/*****************************************************/ 

typedef enum
{
	ETS_NONE = 0,			/**< none (default)				   					*/
	ETS_SOFTWARE_STROBE,	/**< writing 0x55 to the software strobe register	*/
	ETS_AI_FRAME_MAX = 2,	/**< Analog Input Frame Maximum                     */
	ETS_AI_FRAME_TIMER,		/**< Analog Input Sample/Frame Timer                */
	ETS_CT_OUT0_RE,			/**< 8254 Counter 0 Output (CT_OUT0) rising-edge    */
	ETS_CT_OUT0_FE,			/**< 8254 Counter 0 Output (CT_OUT0) falling-edge   */
	ETS_CT_OUT2_RE,			/**< 8254 Counter 2 Output (CT_OUT2) rising-edge    */
	ETS_CT_OUT2_FE,			/**< 8254 Counter 2 Output (CT_OUT2) falling-edge	*/
	ETS_DIN0_RE,			/**< DIN0 rising edge	   							*/
	ETS_DIN0_FE,		 	/**< DIN0 falling edge   							*/
	ETS_DIN1_RE,			/**< DIN1 rising edge	   							*/
	ETS_DIN1_FE,			/**< DIN1 falling edge   							*/
	ETS_DIN2_RE,			/**< DIN2 rising edge	   							*/
	ETS_DIN2_FE,			/**< DIN2 falling edge    							*/
	ETS_DIN3_RE,			/**< DIN3 rising edge	    						*/
	ETS_DIN3_FE				/**< DIN3 falling edge    							*/
} STX104_TRG_STOP_ENUM;	/**< TCFG.ETS[3:0]. */

/*****************************************************/

typedef enum
{
	STS_NONE = 0,			/**< none (default)				                  	*/
	STS_SOFTWARE_STROBE,	/**< writing 0x55 to the software strobe register   */
	STS_AI_FRAME_TIMER = 3,	/**< Analog Input Sample/Frame Timer                */
	STS_CT_OUT0_RE,			/**< 8254 Counter 0 Output (CT_OUT0) rising-edge    */
	STS_CT_OUT0_FE,			/**< 8254 Counter 0 Output (CT_OUT0) falling-edge   */
	STS_CT_OUT2_RE,			/**< 8254 Counter 2 Output (CT_OUT2) rising-edge    */
	STS_CT_OUT2_FE,			/**< 8254 Counter 2 Output (CT_OUT2) falling-edge   */
	STS_DIN0_RE,			/**< DIN0 rising edge	   							*/
	STS_DIN0_FE,		 	/**< DIN0 falling edge   							*/
	STS_DIN1_RE,			/**< DIN1 rising edge	   							*/
	STS_DIN1_FE,			/**< DIN1 falling edge   							*/
	STS_DIN2_RE,			/**< DIN2 rising edge	   							*/
	STS_DIN2_FE,			/**< DIN2 falling edge    							*/
	STS_DIN3_RE,			/**< DIN3 rising edge	    						*/
	STS_DIN3_FE				/**< DIN3 falling edge    							*/
} STX104_TRG_START_ENUM;   /**< TCFG.STS[3:0]. 									*/

/*****************************************************/
/* STX104 analog input trigger configuration data structure type
   definition.                                                   */
typedef struct
{	
	BOOL  						enabled;     		/**< TCFG.TEN    	*/
	STX104_TRG_MODE_ENUM   		start_mode;	 		/**< TCFG.STM[1:0]. */
	STX104_TRG_START_SYNC_ENUM 	start_sync_source;	/**< TCFG.TSS[3:0]. */
	STX104_TRG_STOP_ENUM  		stop_source;		/**< TCFG.ETS[3:0]. */
	STX104_TRG_START_ENUM  		start_source;		/**< TCFG.STS[3:0]. */
/**
 * @brief
 * Note regarding delay_<units>
 * The library now supports both delay_ns and delay_sec.  The reason is that
 * delay_ns has finer resolution at shorter intervals without round-off errors
 * while delay_sec has larger dynamic range for larger time intervals.  We will
 * not be deprecating delay_ns.
 */
	uint32_t					delay_ns;			/**< nSec (driver converts to steps) TSD.TSD[31:0]. 					*/
	double 						delay_sec;			/**< seconds - changed from UINT32 to double to improve dynamic range  	*/
} STX104_TRG_CFG_TYPE; 


/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 INTERRUPT SUB-SECTION                                                             */
/******************************************************/

/*****************************************************/
typedef enum
{
	ISB_NONE = 0,					/**< none (default)										   */
	ISB_THRESHOLD_COUNTER = 2,		/**< Interrupt Threshold Counter							   */
	ISB_AI_FRAME_MAX,				/**< Analog Input Frame Maximum							   */
	ISB_TRIGGER_ACTIVATED,			/**< Trigger Start and transition to active trigger state	   */
	ISB_TRIGGER_DEACTIVATED,		/**< Trigger Stop and transition to inactive trigger state   */
	ISB_DIN0_RE = 8,				/**< DIN0 rising edge	   	*/
	ISB_DIN0_FE,		 			/**< DIN0 falling edge   	*/
	ISB_DIN1_RE,					/**< DIN1 rising edge	   	*/
	ISB_DIN1_FE,					/**< DIN1 falling edge   	*/
	ISB_DIN2_RE,					/**< DIN2 rising edge	   	*/
	ISB_DIN2_FE,					/**< DIN2 falling edge    */
	ISB_DIN3_RE,					/**< DIN3 rising edge	    */
	ISB_DIN3_FE						/**< DIN3 falling edge    */
} STX104_INT_SRCB_ENUM;  /**< Event or Interrupt Source-B select; ISS.ISB[3:0]. */

/*****************************************************/	 

typedef enum
{
	ISA_NONE = 0,					/**< none (default)										   */
	ISA_THRESHOLD_COUNTER = 2,	/**< Interrupt Threshold Counter							   */
	ISA_AI_FRAME_MAX,				/**< Analog Input Frame Maximum							   */
	ISA_TRIGGER_ACTIVATED,		/**< Trigger Start and transition to active trigger state	   */
	ISA_TRIGGER_DEACTIVATED,	/**< Trigger Stop and transition to inactive trigger state   */
	ISA_DIN0_RE = 8,				/**< DIN0 rising edge	   	*/
	ISA_DIN0_FE,		 			/**< DIN0 falling edge   	*/
	ISA_DIN1_RE,					/**< DIN1 rising edge	   	*/
	ISA_DIN1_FE,					/**< DIN1 falling edge   	*/
	ISA_DIN2_RE,					/**< DIN2 rising edge	   	*/
	ISA_DIN2_FE,					/**< DIN2 falling edge    */
	ISA_DIN3_RE,					/**< DIN3 rising edge	    */
	ISA_DIN3_FE						/**< DIN3 falling edge    */
} STX104_INT_SRCA_ENUM;  		/**< Event or Interrupt Source-A select; ISS.ISA[3:0]. */

/*****************************************************/	   

typedef enum
{
	ITS_NONE = 0,					/**< none (default)										   	*/
	ITS_AI_FRAME,					/**< Sample or Frame (i.e. ADC burst)						 	*/
	ITS_AI_BLOCK,					/**< Block (i.e. 256 samples)								 	*/
	ITS_AI_FRAME_MAX,				/**< Analog Input Frame Maximum							   	*/
	ITS_TRIGGER_ACTIVATED,		/**< Trigger Start and transition to active trigger state	   	*/
	ITS_TRIGGER_DEACTIVATED,	/**< Trigger Stop and transition to inactive trigger state   	*/
	ITS_DIN0_RE = 8,				/**< DIN0 rising edge	   	*/
	ITS_DIN0_FE,		 			/**< DIN0 falling edge   	*/
	ITS_DIN1_RE,					/**< DIN1 rising edge	   	*/
	ITS_DIN1_FE,					/**< DIN1 falling edge   	*/
	ITS_DIN2_RE,					/**< DIN2 rising edge	   	*/
	ITS_DIN2_FE,					/**< DIN2 falling edge    */
	ITS_DIN3_RE,					/**< DIN3 rising edge	    */
	ITS_DIN3_FE						/**< DIN3 falling edge    */
} STX104_INT_THRESHOLD_SRC_ENUM;	/**< Event (Interrupt) threshold counter source; ICFG.ITS[3:0]. */

/*****************************************************/
/* Defines FIFO event per number of blocks of analog input data
   collected ICFG.FIBLK[3:0].                                   */
typedef enum
{	/**< 1 BLOCK = 256 SAMPLES */
	BLOCK_2 = 0,	/**< 2 Blocks or 512 Samples (default)	*/
	BLOCK_4,			/**< 4 Blocks or 1,024 Samples			*/
	BLOCK_8,			/**< 8 Blocks or 2,048 Samples			*/
	BLOCK_16,		/**< 16 Blocks or 4,096 Samples			*/
	BLOCK_32,		/**< 32 Blocks or 8,192 Samples			*/
	BLOCK_64,		/**< 64 Blocks or 16,384 Samples			*/
	BLOCK_128,		/**< 128 Blocks or 32,768 Samples			*/
	BLOCK_256,		/**< 256 Blocks or 65,536 Samples			*/
	BLOCK_512,		/**< 512 Blocks or 131,072 Samples		*/
	BLOCK_1024,		/**< 1024 Blocks or 262,144 Samples		*/
	BLOCK_2048,		/**< 2048 Blocks or 524,288 Samples		*/
	BLOCK_0_0625,	/**< 0.0625 Block or 16 Samples			*/
	BLOCK_0_125,	/**< 0.125 Block or 32 Samples			*/
	BLOCK_0_25,		/**< 0.25 Block or 64 Samples				*/
	BLOCK_0_5,		/**< 0.5 Block or 128 Samples				*/
	BLOCK_1				/**< 1 Block or 256 Samples 				*/
} STX104_INT_BLOCKS_PER_ENUM;  /**< Defines FIFO event per number of blocks of analog input data
                                  collected ICFG.FIBLK[3:0].                                   */
/*****************************************************/
/* interrupt callback function definition */
typedef void ( * AUL_INT_CALLBACK )( void * private_data );

/*****************************************************/
/* STX104 Interrupt (Event) configuration data structure type
   definition; ICFG and ISS registers                          */
struct stx104_intr_cfg
{																	
	uint32_t 						threshold;							 /**< ICFG.ITH[31:0]. */
	STX104_INT_THRESHOLD_SRC_ENUM  	threshold_source;					 /**< ICFG.ITS[3:0]. */
	STX104_INT_BLOCKS_PER_ENUM  	blocks_per_interrupt;			 /**< ICFG.FIBLK[3:0]. */
	STX104_INT_SRCA_ENUM  			source_a;							 /**< ISS.ISA[3:0]. */
	STX104_INT_SRCB_ENUM  			source_b;							 /**< ISS.ISB[3:0]. */
	AUL_INT_CALLBACK				callback_source_a;				 /**< user callback for interrupt source-a */
	AUL_INT_CALLBACK				callback_source_b;				 /**< user callback for interrupt source-b */
	AUL_INT_CALLBACK				callback_fifo_blk;				 /**< user callback for interrupt fifo blocks */
	void *							callback_source_a_user_dataset;/**< user dataset pointer for interrupt source-a */
	void *							callback_source_b_user_dataset;/**< user dataset pointer for interrupt source-b */
	void *							callback_fifo_blk_user_dataset;/**< user dataset pointer for interrupt fifo blocks */
	BOOL  							threshold_sync_to_triggering;	
};	
typedef struct stx104_intr_cfg STX104_INT_CFG_TYPE;

/*****************************************************/
/* STX104 interrupt callback data structure type definition.
   This information is sent to all STX104 interrupt (event)
   related callbacks.                                        */
struct stx104_intr_dataset
{
	/* * Message value sent back to callback function: *
	   
	     * ISSA Callback &#45; message value is the source&#45;a
	       interrupt source (i.e. STX104_INT_SRCA_ENUM )
	   
	     * ISSB Callback &#45; message value is the source&#45;b
	       interrupt source (i.e. STX104_INT_SRCB_ENUM )
	   
	     * FIFO Callback &#45; message value is the fifo status
	       register. The word returned is the following:
	   
	   <table>
	   D15   D14   D13   D12   D11     D10     D9     D8
	   ====  ====  ====  ====  ======  ======  =====  =====
	   X     DFR   FF    FE    FBR11   FBR10   FBR9   FBR8
	   </table>
	   
	   <table>
	   D7     D6     D5     D4     D3     D2     D1     D0
	   =====  =====  =====  =====  =====  =====  =====  =====
	   FBR7   FBR6   FBR5   FBR4   FBR3   FBR2   FBR1   FBR0
	   </table>                                                  */
	uint16_t message;
	/** Descriptor for the board that has generated the interrupt. Use
	   the "board" descriptor for subsequent function calls to
	   retrieve/set inputs/outputs .                                  */
	int		board;
	/** Pointer to user defined dataset.  This is useful for passing
	   user defined datasets to the event.  An example is for passing
	   time related information since the event may be related to 
	   frame timing.
	*/
	void *  user_defined_dataset;
};
typedef struct stx104_intr_dataset STX104_INT_RESULT_TYPE;


/*****************************************************/
/*****************************************************/

/******************************************************/
/*   SERIAL PERIPHERAL INTERFACE (SPI) SUB-SECTION                                                       */
/******************************************************/
/* the SPI port buffer length applies to both receive and transmit buffers */
#define STX104_SPI_BUFFER_LENGTH_MAX   16 

typedef enum
{
	SPI_SDI_DIN0		= 0,		/**< SDI for High Voltage Buffer Board */
	SPI_SDI_DIN1		= 1,
	SPI_SDI_DIN2		= 2,
	SPI_SDI_DIN3		= 3
} STX104_SPI_SDI_ENUM;

/*****************************************************/

typedef enum
{
	SPI_CSB_SOFTWARE		= 0,
	SPI_CSB_FIFO			= 1,
	SPI_CSB_TOGGLE_8		= 2,
	SPI_CSB_TOGGLE_16		= 3
} STX104_SPI_CSB_ENUM;

/*****************************************************/

struct stx104_spi_cfg
{
	/* start - Firmware 120415H or higher, otherwise ignored */
	uint32_t					half_clock_ns;					/* 0 to 102,475 nanoseconds.  Driver converts to/from steps. SPI_SHCI[11:0]  */
	STX104_SPI_SDI_ENUM			sdi_map;								/* */
	uint32_t					end_cycle_delay_ns;			/* Delay between spi words in nanoseconds.  Driver converts to/from steps ECD[3:0] */
	BOOL						sdio_wrap;   						/* wraps data out to data in for testing */
	STX104_SPI_CSB_ENUM			chip_select_behavior;
	BOOL						sdo_polarity;
	BOOL						sdi_polarity;
	BOOL						sclk_phase;
	BOOL						sclk_polarity;
	/* end -   Firmware 120415H or higher                    */
};
typedef struct stx104_spi_cfg STX104_SPI_CFG_TYPE;


/*****************************************************/
/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 REGISTER SET OFFSET DEFINITIONS                                                               */
/******************************************************/

/*****************************************************/
#if(0)
typedef enum
{
STX104_SOFTWARE_STROBE					= 0,
STX104_ADC_DATA_LSB   					= 0,
STX104_ADC_DATA_MSB   					= 1,
STX104_ADC_DATA       					= 0,
STX104_CHANNEL					 		= 2,
STX104_DIGITAL_OUTPUTS 					= 3, 
STX104_DIGITAL_INPUTS 					= 3, 
STX104_DAC_CHANA_LSB 					= 4, 
STX104_DAC_CHANA_MSB 					= 5, 
STX104_DAC_CHANA 						= 4,
STX104_DAC_CHANB_LSB 					= 6, 
STX104_DAC_CHANB_MSB 					= 7, 
STX104_DAC_CHANB    					= 6,
STX104_CLEAR_INTERRUPTS					= 8, 
STX104_ADC_STATUS					 	= 8,
STX104_ADC_CONTROL					 	= 9,
STX104_PACER_CLOCK_CONTROL		 		= 10,
STX104_FIFO_FLAGS						= 10,
STX104_ADC_CONFIGURATION		 		= 11,
STX104_CT0_DATA 						= 12,
STX104_CT1_DATA  						= 13,
STX104_CT2_DATA  						= 14,
STX104_CT_CONFIGURATION		  			= 15,
STX104_FIFO_DATA_STATUS		  			= 15,
STX104_INDEXED_DATA_LSB  				= 12,
STX104_INDEXED_DATA_MSB  				= 13,
STX104_INDEX_DATA  						= 12,
STX104_INDEX_POINTER  					= 14,
STX104_CONVERSION_DISABLE				= 1028,  /* deprecated */
STX104_BURST_MODE_ENABLE				= 1029,  /* deprecated */
STX104_BURST_FUNCTION					= 1030,  /* deprecated */
STX104_EXTENDED_STATUS					= 1031,  /* deprecated */
/* Extended 080214 and beyond */
STX104_GENERAL_CONFIGURATION 			=    0,
STX104_INTERRUPT_SOURCE_SELECT			=    2,
STX104_INTERRUPT_CONFIGURATION			=    4,
STX104_INTERRUPT_THRESHOLD				=    8,
STX104_DIGITAL_OUTPUT_CONFIGURATION		=   12,
STX104_DIGITAL_INPUT_CONFIGURATION		=   14,
STX104_TRIGGER_CONFIGURATION			=   16,
STX104_TRIGGER_START_DELAY				=   20,
STX104_ANALOG_INPUT_GENERAL_CONFIG		=   32,
STX104_ANALOG_INPUT_POLARITY_CONFIG		=   34,
STX104_ANALOG_INPUT_FRAME_TIMER			=   36,
STX104_ANALOG_INPUT_BURST_TIMER			=   40,
STX104_ANALOG_INPUT_FRAME_MAX			=   44,
STX104_ANALOG_INPUT_FRAME_COUNT			=   48,

STX104_CONVERSION_DISABLE_INDEXED		=   64,
STX104_BURST_MODE_ENABLE_INDEXED		=   65,
STX104_BURST_FUNCTION_ENABLE_INDEXED	=   66,
STX104_EXTENDED_STATUS_INDEXED			=   67,
/* SPI Registers */
STX104_SPI_CONFIGURATION				=   80,
STX104_SPI_HALF_CLOCK					=   82,
STX104_SPI_RECEIVE_FIFO_DATA			=   84,
STX104_SPI_TRANSMIT_FIFO_DATA			=   84,
STX104_SPI_STATUS						=   85,
STX104_SPI_SOFTWARE_CHIP_SELECT			=   85,

STX104_MISCELLANEOUS_OUTPUT_CONFIG		=  208,
STX104_FIFO_DATA_AVAILABLE				=  224,
STX104_FIFO_CONFIGURATION				=  228,
STX104_SCRATCH_PAD						=  248,
STX104_BOARD_ID							=  250
} STX104_REGISTER_ADDRESS_ENUM;
#endif

/*****************************************************/
#define STX104_REG_BITS_OFFSET		8
#define STX104_REG_BITS_TYPE		3
#define STX104_REG_BITS_INDEXED		1

#define STX104_REG_MASK_OFFSET		( (1 << STX104_REG_BITS_OFFSET	) -1 )
#define STX104_REG_MASK_TYPE		( (1 << STX104_REG_BITS_TYPE	) -1 )
#define STX104_REG_MASK_INDEXED		( (1 << STX104_REG_BITS_INDEXED	) -1 )

#define STX104_REG_SHIFT_OFFSET		0
#define STX104_REG_SHIFT_TYPE		( STX104_REG_SHIFT_OFFSET	 + STX104_REG_BITS_OFFSET  )
#define STX104_REG_SHIFT_INDEXED	( STX104_REG_SHIFT_TYPE + STX104_REG_BITS_TYPE )

#define STX104_REG_INPLACE_INDEXED	( STX104_REG_MASK_INDEXED << STX104_REG_SHIFT_INDEXED )
#define STX104_REG_IS_INDEXED(reg)  !!( (reg) & STX104_REG_INPLACE_INDEXED )

#define STX104_REG_GET_OFFSET(reg)	(  ( (reg) >> STX104_REG_SHIFT_OFFSET) & STX104_REG_MASK_OFFSET )
#define STX104_REG_GET_TYPE(reg)	(  ( (reg) >> STX104_REG_SHIFT_TYPE  ) & STX104_REG_MASK_TYPE   )

#define STX104_REG_CREATE(index,bytes,id)											\
	(	( ((index) & STX104_REG_MASK_INDEXED ) << STX104_REG_SHIFT_INDEXED  )	|	\
		( ((bytes) & STX104_REG_MASK_TYPE    ) << STX104_REG_SHIFT_TYPE		)	|	\
		( ((id)    & STX104_REG_MASK_OFFSET  ) << STX104_REG_SHIFT_OFFSET   )	    ) 

/*****************************************************/
#define STX104_REG_DIRECT    0
#define STX104_REG_INDEXED   1
/*****************************************************/
#define STX104_REGISTER_DEFINITIONS(_) \
/*		symbol								offset        behavior      width */ \
	_( STX104_SOFTWARE_STROBE,					 0,		STX104_REG_DIRECT,	1	)\
	_( STX104_ADC_DATA_LSB,						 0,		STX104_REG_DIRECT,	1	)\
	_( STX104_ADC_DATA_MSB,						 1,		STX104_REG_DIRECT,	1	)\
	_( STX104_ADC_DATA,							 0,		STX104_REG_DIRECT,	2	)\
	_( STX104_CHANNEL,							 2,		STX104_REG_DIRECT,	1	)\
	_( STX104_DIGITAL_OUTPUTS,					 3, 	STX104_REG_DIRECT,	1	)\
	_( STX104_DIGITAL_INPUTS,					 3,		STX104_REG_DIRECT,	1	)\
	_( STX104_DAC_CHANA_LSB,					 4,		STX104_REG_DIRECT,	1	)\
	_( STX104_DAC_CHANA_MSB,					 5,		STX104_REG_DIRECT,	1	)\
	_( STX104_DAC_CHANA,						 4,		STX104_REG_DIRECT,	2	)\
	_( STX104_DAC_CHANB_LSB,					 6, 	STX104_REG_DIRECT,	1	)\
	_( STX104_DAC_CHANB_MSB,					 7, 	STX104_REG_DIRECT,	1	)\
	_( STX104_DAC_CHANB,						 6,		STX104_REG_DIRECT,	2	)\
	_( STX104_CLEAR_INTERRUPTS,					 8, 	STX104_REG_DIRECT,	1	)\
	_( STX104_ADC_STATUS,						 8,		STX104_REG_DIRECT,	1	)\
	_( STX104_ADC_CONTROL,						 9,		STX104_REG_DIRECT,	1	)\
	_( STX104_PACER_CLOCK_CONTROL,				10,		STX104_REG_DIRECT,	1	)\
	_( STX104_FIFO_FLAGS,						10,		STX104_REG_DIRECT,	1	)\
	_( STX104_ADC_CONFIGURATION,				11,		STX104_REG_DIRECT,	1	)\
	_( STX104_CT0_DATA,							12,		STX104_REG_DIRECT,	1	)\
	_( STX104_CT1_DATA,							13,		STX104_REG_DIRECT,	1	)\
	_( STX104_CT2_DATA,							14,		STX104_REG_DIRECT,	1	)\
	_( STX104_CT_CONFIGURATION,					15,		STX104_REG_DIRECT,	1	)\
	_( STX104_FIFO_DATA_STATUS,					15,		STX104_REG_DIRECT,	1	)\
	_( STX104_INDEXED_DATA_LSB,					12,		STX104_REG_DIRECT,	1	)\
	_( STX104_INDEXED_DATA_MSB,					13,		STX104_REG_DIRECT,	1	)\
	_( STX104_INDEX_DATA,						12,		STX104_REG_DIRECT,	2	)\
	_( STX104_INDEX_POINTER,					14,		STX104_REG_DIRECT,	1	)\
	_( STX104_CONVERSION_DISABLE,	1028,  /* deprecated */		STX104_REG_DIRECT,	1	)\
	_( STX104_BURST_MODE_ENABLE,	1029,  /* deprecated */		STX104_REG_DIRECT,	1	)\
	_( STX104_BURST_FUNCTION,		1030,  /* deprecated */		STX104_REG_DIRECT,	1	)\
	_( STX104_EXTENDED_STATUS,		1031,  /* deprecated */		STX104_REG_DIRECT,	1	)\
	_( STX104_GENERAL_CONFIGURATION,			 0,		STX104_REG_INDEXED,	1	)\
	_( STX104_INTERRUPT_SOURCE_SELECT,			 2,		STX104_REG_INDEXED,	1	)\
	_( STX104_INTERRUPT_CONFIGURATION,			 4,		STX104_REG_INDEXED,	2	)\
	_( STX104_INTERRUPT_THRESHOLD,				 8,		STX104_REG_INDEXED,	4	)\
	_( STX104_DIGITAL_OUTPUT_CONFIGURATION,		12,		STX104_REG_INDEXED,	1	)\
	_( STX104_DIGITAL_INPUT_CONFIGURATION,		14,		STX104_REG_INDEXED,	1	)\
	_( STX104_TRIGGER_CONFIGURATION,			16,		STX104_REG_INDEXED,	2	)\
	_( STX104_TRIGGER_START_DELAY,				20,		STX104_REG_INDEXED,	4	)\
	_( STX104_ANALOG_INPUT_GENERAL_CONFIG,		32,		STX104_REG_INDEXED,	2	)\
	_( STX104_ANALOG_INPUT_POLARITY_CONFIG,		34,		STX104_REG_INDEXED,	2	)\
	_( STX104_ANALOG_INPUT_FRAME_TIMER,			36,		STX104_REG_INDEXED,	4	)\
	_( STX104_ANALOG_INPUT_BURST_TIMER,			40,		STX104_REG_INDEXED,	4	)\
	_( STX104_ANALOG_INPUT_FRAME_MAX,			44,		STX104_REG_INDEXED,	4	)\
	_( STX104_ANALOG_INPUT_FRAME_COUNT,			48,		STX104_REG_INDEXED,	4	)\
	_( STX104_CONVERSION_DISABLE_INDEXED,		64,		STX104_REG_INDEXED,	1	)\
	_( STX104_BURST_MODE_ENABLE_INDEXED,		65,		STX104_REG_INDEXED,	1	)\
	_( STX104_BURST_FUNCTION_ENABLE_INDEXED,	66,		STX104_REG_INDEXED,	1	)\
	_( STX104_EXTENDED_STATUS_INDEXED,			67,		STX104_REG_INDEXED,	1	)\
	_( STX104_I8254_CT0_DATA,					68,		STX104_REG_INDEXED, 1	)\
	_( STX104_I8254_CT1_DATA,					69,		STX104_REG_INDEXED, 1	)\
	_( STX104_I8254_CT2_DATA,					70,		STX104_REG_INDEXED, 1	)\
	_( STX104_I8254_CFG,						71,		STX104_REG_INDEXED, 1	)\
	_( STX104_SPI_CONFIGURATION,				80,		STX104_REG_INDEXED,	2	)\
	_( STX104_SPI_HALF_CLOCK,					82,		STX104_REG_INDEXED,	2	)\
	_( STX104_SPI_RECEIVE_FIFO_DATA,			84,		STX104_REG_INDEXED,	1	)\
	_( STX104_SPI_TRANSMIT_FIFO_DATA,			84,		STX104_REG_INDEXED,	1	)\
	_( STX104_SPI_STATUS,						85,		STX104_REG_INDEXED,	1	)\
	_( STX104_SPI_SOFTWARE_CHIP_SELECT,			85,		STX104_REG_INDEXED,	1	)\
	_( STX104_MISCELLANEOUS_OUTPUT_CONFIG,		208,	STX104_REG_INDEXED,	1	)\
	_( STX104_FIFO_DATA_AVAILABLE,				224,	STX104_REG_INDEXED,	4	)\
	_( STX104_FIFO_CONFIGURATION,				228,	STX104_REG_INDEXED,	1	)\
	_( STX104_SCRATCH_PAD,						248,	STX104_REG_INDEXED,	2	)\
	_( STX104_BOARD_ID,							250,	STX104_REG_INDEXED,	2	)

/*****************************************************/
#define STX104_REGISTER_EXTRACTOR_ENUM(symbol,number,behavior,bytes)			\
	REG_ID_ ## symbol = STX104_REG_CREATE(behavior,bytes,number),

#define STX104_REGISTER_ADDRESS_EXTRACTOR_ENUM(symbol,number,behavior,bytes)	\
	symbol = (number),

/*****************************************************/
enum stx104_reg_id
{
	STX104_REGISTER_DEFINITIONS( STX104_REGISTER_EXTRACTOR_ENUM )
};
typedef enum stx104_reg_id STX104_REG_ID_ENUM;
/*****************************************************/
typedef enum
{
	STX104_REGISTER_DEFINITIONS( STX104_REGISTER_ADDRESS_EXTRACTOR_ENUM )
} STX104_REGISTER_ADDRESS_ENUM;

/*****************************************************/
/*****************************************************/
/*****************************************************/

/******************************************************/
/*   STX104 PRIVATE DATA DEFINITIONS                                                               */
/******************************************************/


/******************************************************************
*/
/* STX104 device information structure for interrupts for any board */
struct stx104_intr_params
{
	int						board;
	devmgr_device_type *	device;
	/* Bit pattern which is a summary of all x_active bools below.
	   Format is: [3:0] = dcba active bits 
	*/
	size_t					interrupt_list_active;   
	io_xfr_type	*			list_status;
	AUL_INT_CALLBACK		interrupt_handler_a;
	AUL_INT_CALLBACK		interrupt_handler_b;
	AUL_INT_CALLBACK		interrupt_handler_c;
	AUL_INT_CALLBACK		interrupt_handler_d;
/*	BOOL					interrupt_callback_a_active;
	BOOL					interrupt_callback_b_active;
	BOOL					interrupt_callback_c_active;
	BOOL					interrupt_callback_d_active;*/
	void *					interrupt_dataset_a;
	void *					interrupt_dataset_b;
	void *					interrupt_dataset_c;
	void *					interrupt_dataset_d;
	uint16_t				message_a;
	uint16_t				message_b;
	uint16_t				message_c;
	uint16_t				message_d;
};

typedef struct stx104_intr_params stx104_intr_params_type;

/*****************************************************************************/
enum stx104_component
{ 
	STX104_COMPONENT_NONE			= 0,
	STX104_COMPONENT_I854,			/* i8254 component will always be present */
	STX104_COMPONENT_INDIRECT,
	/* <EGS_BEGIN>*/
	STX104_COMPONENT_EGS,
	/* <EGS_END>*/
	STX104_COMPONENT_QTY			/* always one more than actual */
};
typedef enum stx104_component STX104_COMPONENT_ENUM;

/******************************************************************
*/
struct stx104_dataset
{
	SYS_TYPE	type;
	uint16_t	firmware_revision;  /* TODO: someone has to set it?? */
	uint16_t	ai_general_config_reg;
	uint16_t	ai_polarity_config;
	uint16_t	ai_channels_per_frame;
	uint8_t		ai_channel_reg;
	uint8_t		ai_unipolar;
	uint8_t		ai_gain;
	uint8_t		ai_pacer_clock_control_shadow_copy;
	uint8_t		do_reg;
	uint8_t		spi_interface_active;

	stx104_intr_params_type		intr_params;

	int			component_list[STX104_COMPONENT_QTY];
};
typedef struct stx104_dataset stx104_dataset_type;

/*******************************************************************/


/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/

extern device_definition_type device_stx104_definition;


/******************************************************************
Description
The following functions are to be exposed to outside world.
*/

/*******************************************************************/

AUL_DLL_API int AULCALLCONV STX104_AI_Config_Initialize( stx104_ai_cfg_type * config );

AUL_DLL_API int AULCALLCONV STX104_AI_CFG_Set( int board, stx104_ai_cfg_type * config );
AUL_DLL_API int AULCALLCONV STX104_AI_CFG_Get( int board, stx104_ai_cfg_type * config );
AUL_DLL_API int AULCALLCONV STX104_AI_Strobe( int board );
AUL_DLL_API int AULCALLCONV STX104_AI_Aquisition_Restart( int board );

AUL_DLL_API int AULCALLCONV STX104_AI_Frame_Count_Get( int board, uint32_t * frame_count );
AUL_DLL_API int AULCALLCONV STX104_AI_FIFO_Data_Count_Get( int board, uint32_t * fifo_count );


/* TODO: buffer overflow issue and how to verify buffer size for STX104_AI_Frames_Get().

By design the customer must guarantee that the receiving buffer will not overflow
by ensuring that the frame_count_request parameter never exceeds the
buffer size passed into the STX104_AI_Frames_Get().  Specifically

Must guarantee the following:

	frame_count_request <= ( BUFFER_SIZE_BYTES / 2 )  / ( last_channel - first_channel + 1 )


NOTE:
1. The function STX104_AI_Frames_Get() will return STX104_EC_AI_FIFO_FULL
	error code if the FIFO is full.

2. The absolute maximum number of samples that can reside in
   the FIFO is: 1,050,624

*/
AUL_DLL_API int AULCALLCONV STX104_AI_Frames_Get( int board, uint16_t * buffer, uint16_t * frame_count_actual, uint16_t frame_count_request );
AUL_DLL_API int AULCALLCONV STX104_AI_Raw_To_Volts( int board, uint16_t raw_value, double* voltage );


/*******************************************************************/
AUL_DLL_API int AULCALLCONV STX104_Gen_Revision_Get( int board, uint16_t * id_value );
AUL_DLL_API int AULCALLCONV STX104_Gen_Jumpers_Get( int board, STX104_GEN_JMP_TYPE * jumpers );
AUL_DLL_API int AULCALLCONV STX104_Gen_Revision_Get( int board, uint16_t * id_value );

/*******************************************************************/
AUL_DLL_API int AULCALLCONV STX104_AO_Set( int board, STX104_AO_ENUM channel,  uint16_t value );
AUL_DLL_API int AULCALLCONV STX104_AO_All_Set( int board, uint16_t value_ao_a, uint16_t value_ao_b );

/*******************************************************************/
AUL_DLL_API int AULCALLCONV STX104_DI_CFG_Set( int board, STX104_DI_CFG_TYPE * cfg );
AUL_DLL_API int AULCALLCONV STX104_DI_CFG_Get( int board, STX104_DI_CFG_TYPE * cfg );
AUL_DLL_API int AULCALLCONV STX104_DI_All_Get( int board, uint8_t * value );
AUL_DLL_API int AULCALLCONV STX104_DI_Get( int board, STX104_DI_ENUM channel, BOOL * value );

/*******************************************************************/
AUL_DLL_API int AULCALLCONV STX104_DO_CFG_Set( int board, stx104_do_cfg_type * cfg );
AUL_DLL_API int AULCALLCONV STX104_DO_CFG_Get( int board, stx104_do_cfg_type * cfg );
AUL_DLL_API int AULCALLCONV STX104_DO_All_Set( int board, uint8_t  value );
AUL_DLL_API int AULCALLCONV STX104_DO_All_Get( int board, uint8_t * value );
AUL_DLL_API int AULCALLCONV STX104_DO_Set( int board, STX104_DO_ENUM channel, BOOL  value );
AUL_DLL_API int AULCALLCONV STX104_DO_Get( int board, STX104_DO_ENUM channel, BOOL * value );

/*****************************************************/

AUL_DLL_API int AULCALLCONV STX104_Trg_CFG_Set( int board, STX104_TRG_CFG_TYPE * cfg );
AUL_DLL_API int AULCALLCONV STX104_Trg_CFG_Get( int board, STX104_TRG_CFG_TYPE * cfg );
AUL_DLL_API int AULCALLCONV STX104_Trg_Active_Get( int board, BOOL * state ); 
AUL_DLL_API int AULCALLCONV STX104_Trg_Software_Start( int board ); 
AUL_DLL_API int AULCALLCONV STX104_Trg_Software_Stop( int board ); 
AUL_DLL_API int AULCALLCONV STX104_Trg_Software_Sync( int board ); 

/*******************************************************************/

AUL_DLL_API int AULCALLCONV STX104_Int_CFG_Set( int board, STX104_INT_CFG_TYPE * cfg );
AUL_DLL_API int AULCALLCONV STX104_Int_CFG_Get( int board, STX104_INT_CFG_TYPE * cfg );

AUL_DLL_API int AULCALLCONV STX104_Int_Enable_Set( int board, BOOL  enable );
AUL_DLL_API int AULCALLCONV STX104_Int_Enable_Get( int board, BOOL * enable );


/*******************************************************************/

AUL_DLL_API int AULCALLCONV STX104_SPI_CFG_Set( int board, STX104_SPI_CFG_TYPE * config );
AUL_DLL_API int AULCALLCONV STX104_SPI_CFG_Get( int board, STX104_SPI_CFG_TYPE * config );

/* Since this interface inherits digital inputs and outputs, we must indicate whether
   or not the SPI interface will be operational or active.  When the SPI module is active, 
   any subsequent configuration sent to the digital inputs and outputs must be 'approved' 
   by the SPI interface module.  The purpose of the 'approval' is to guarantee that 
   digital I/O settings do not conflict with SPI settings.
   
   If the SPI module is active and a digital input polarity is changed and it is the 
   same digital input used by the SPI module, then an error is generated.  To work 
   around this, first set the SPI module inactive, make the digital input change, then
   re-activate the SPI module.
   
   When the SPI module is set to active, it will make several tests to verify that 
   SPI operations may proceed. 
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Active_Set( int board, BOOL   active_state );
AUL_DLL_API int AULCALLCONV STX104_SPI_Active_Get( int board, BOOL * active_state );

/*
   Use these functions to set the chip select manually.  Will only work if 
	 spi configuration chip_select_behavior = SPI_SCSB_SOFTWARE.
 */
AUL_DLL_API int AULCALLCONV STX104_SPI_Chip_Select_Commit_Set( int board, BOOL   value );
AUL_DLL_API int AULCALLCONV STX104_SPI_Chip_Select_Commit_Get( int board, BOOL * value );

/* Read SPI status register -- for debug purposes primarily */
AUL_DLL_API int AULCALLCONV STX104_SPI_Status_Get( int board, uint8_t * value );

/*  NOTE: TX and RX FIFOs are up to STX104_SPI_BUFFER_LENGTH_MAX bytes each.
    Make sure application buffers are at least STX104_SPI_BUFFER_LENGTH_MAX
	in length.
*/ 

/*
REMARK
Returns an array of buffers.
NOTE: the array must be at least size STX104_SPI_BUFFER_LENGTH_MAX
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Receive(	int			board,
												uint8_t *	rx_buffer,
												uint8_t		rx_count_max,
												uint8_t *	rx_count_actual
											 );
/*
REMARK
Sequence of events that this function performs:
1. Fills the transmit buffer (transmit array must be at least size STX104_SPI_BUFFER_LENGTH_MAX)
2. starts SPI transceiver.  Transmits the buffered data.
3. returns
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Transmit(	int			board,
													uint8_t *	tx_buffer,
													uint8_t		tx_count,
													uint8_t *	tx_count_actual
												);
/* 
REMARK
Sequence of events that this function performs.
1. Clears the receive buffer
2. Fills the transmit buffer
3. starts SPI transceiver
4. Read out receive buffer until at least the same bytes transmitted is received (or more)
   Transmit and receive arrays must be at least STX104_SPI_BUFFER_LENGTH_MAX in length.
5. returns
*/
AUL_DLL_API int AULCALLCONV STX104_SPI_Transceiver_Blocking(  int		board,
															  uint8_t * tx_buffer,
															  uint8_t   tx_count,
															  uint8_t *	tx_count_actual,
															  uint8_t * rx_buffer,
															  uint8_t   rx_count_max,
															  uint8_t * rx_count_actual,
															  uint8_t * tx_retry_count
															);


/*******************************************************************/


/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* DEVICE_STX104 (dev_stx104.h) */
