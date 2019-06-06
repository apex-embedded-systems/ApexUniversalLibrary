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

1. Everywhere there is a AUL_Error_Log() replace with Debug_Log()

*********************************************************************************/

#include "dev_stx104.h"
#include "../../shared/status.h"
#include "../../shared/mem.h"
#include "../../shared/error.h"
#include "../../shared/compiler.h"  /* inline */
#include "../../shared/callconv.h"
#include "../../aul/aul_shared.h"
#include "../../io/io.h"
#include "dev_stx104_index.h"

#include "../../str_safe/str_safe.h"

/*TODO: cannot have in kernel!*/
#include <stdlib.h>  /* _fcvt() */
/*#include <math.h>  */  /* pow(), log() */


/*********************************************************************************/
/* if DEBUG then allow IO_DEBUG */
#define STX104_AI_DEBUG			_DEBUG
/*********************************************************************************/
#define STX104_AI_DEBUG_LOG	DEBUG_OUT

/**********************************************************************************************************/
/**********************************************************************************************************/
#if (0)

typedef void ( * ANALOG_INPUT_BUFFER_HANDLER )( struct ANALOG_INPUT_BUFFER_TYPE * buffer, ANALOG_INPUT_ERROR_ENUM error );
typedef ANALOG_INPUT_BUFFER_HANDLER AI_BUFFER_FUNCTION;
			
typedef struct ANALOG_INPUT_BUFFER_TYPE
{
/*TEMPORARY: 	MUTEX  mutex;  */     	/* negotiation between driver and user application */
	WORD*  buffer;	  	/* pointer to data buffer */
	WORD   buffer_size; /* total buffer size */
	WORD   threshold; 	/* number of samples in the buffer for the callback to be called */
	WORD   samples;		/* actual quantity of samples in the buffer */
	AI_BUFFER_FUNCTION  callback_function;
} ANALOG_INPUT_BUFFER_TYPE;

/**********************************************************************************************************
*/
static double STX104_AI_Time_Settling( double capacitance_pf, double resistance_ohms )
{
	double settle_time_ns;

/*	settle_time = ai_resistance_ohms * ai_capacitance_pf * pow10(-12) * 16 * ln(2) * pow10(9)   */
	settle_time_ns = ai_resistance_ohms * ai_capacitance_pf * pow10(-3) * 16 * log(2);		
/*printf( "  R=%f, C=%f\n", ai_resistance_ohms, ai_capacitance_pf ); */
	return( settle_time_ns );
}
#endif

/********************************************************************************************************/
/********************************************************************************************************/
/* the following functions and macro used by STX104_AI_CFG_Set(). */
#if defined( STX104_AI_DEBUG )

/********************************************************************************************************/
#define STX104_AI_DEBUG_STRING_SIZE  32
/********************************************************************************************************/
static char * Float_To_String_Convert( double value,      /* input value                          */
                                       int    right,      /* number of digits to right of decimal */
                                       char * out_string, /* resulting output */
                                       int    out_size    /* max length of string */
                                     )
{
	int    osi, bi, i;  /* osi = output_string index,  bi = buf index */
	int    dec, sign;
	char * buf;

	buf = fcvt( value, right, &dec, &sign );

	if ( (right + dec + 1) > out_size ) return out_string;

	bi = 0;
	for ( osi = 0; osi < dec; osi++ )
	{
		out_string[osi] = buf[bi];
		bi++;
	}
	out_string[osi] = '.';

	if ( right > 0 )
	{
		osi++;
		for ( i = 0; i < right; i++ )
		{
			out_string[osi] = buf[bi];
			if ( 0 == out_string[osi] )
			{
				break;
			}
			else
			{
				osi++; bi++;
			}
		}
	}
	out_string[osi] = 0;

	return out_string;
}
/**********************************************************************************************************
   1.0e-2 ms   --> 10 uS
   1,000 ms    -->  1 sec
*/
static char * Float_To_String_Multiplier_Suffix(  double time_seconds,
												  char * output_string,
												  size_t output_string_size,
												  int *  error_code
                                               )
{
      double time;

      if ( error_code ) *error_code = 0;
      if ( output_string_size < 11 )
      {
         if ( error_code ) *error_code = -1;
         return output_string;
      }
      else if ( time_seconds < 0.0 )
      {
         if ( error_code ) *error_code = -2;
         return output_string;
      }
      if      ( time_seconds >=   1.0    )
      {
		time  = time_seconds;
		Float_To_String_Convert( time, 9, output_string, output_string_size );
		String_Concatenate( output_string, " sec", output_string_size );
      }
      else if ( time_seconds >=   1.0e-3 )
      {
		time  = time_seconds * 1.0e3; /* convert to mS */
		Float_To_String_Convert( time, 6, output_string, output_string_size );
		String_Concatenate( output_string, " ms", output_string_size );
      }
      else if ( time_seconds >=   1.0e-6 )
      {
		time  = time_seconds * 1.0e6; /* convert to uS */
		Float_To_String_Convert( time, 3, output_string, output_string_size );
		String_Concatenate( output_string, " us", output_string_size );
      }
      else if ( time_seconds >=   1.0e-9 )
      {
		time  = time_seconds * 1.0e9; /* convert to nS */
		Float_To_String_Convert( time, 0, output_string, output_string_size );
		String_Concatenate( output_string, " ns", output_string_size );
      }
      else
      {
		  String_Copy( output_string, "0 ns", output_string_size );
      }
      return output_string;
}
#endif
/********************************************************************************************************/
/********************************************************************************************************/

/*******************************************************************/
#define STX104_AI_MODULE_REV "$Date: 2015-02-02 17:41:32 -0600 (Mon, 02 Feb 2015) $"
/********************************************************************************************************/
const char * STX104_AI_SVN_Time_Stamp( void ) { return STX104_AI_MODULE_REV; }
/**********************************************************************************************************
*/
static double STX104_AI_Intra_Sample_Timing_Minimum( stx104_ai_cfg_type * cfg )
{
	int		channel;
	double	settle_time_ns;
	double	time_ns;
	time_ns = STX104_AI_INTRA_SAMPLE_TIME_MIN_NS;
	if ( cfg->first_channel != cfg->last_channel )
	{
		for( channel = cfg->first_channel; channel <= cfg->last_channel; channel++ )
		{	/*settle_time_ns = cfg->resistance_ohms[channel] * cfg->capacitance_pf[channel] * pow(10.0,-3.0) * 16.0 * log(2.0);*/
			settle_time_ns = cfg->resistance_ohms[channel] * cfg->capacitance_pf[channel] * ( 1.0 / 207.6205 );
			if ( settle_time_ns > time_ns ) time_ns = settle_time_ns;
		}
	}
	return time_ns;
}
/**********************************************************************************************************/
/**********************************************************************************************************/
static double stx104_adc_gain[8] = 
{ 
	20.0/65536.0, 10.0/65536.0, 5.0/65536.0, 2.5/65536.0,
	10.0/65536.0,  5.0/65536.0, 2.5/65536.0, 1.25/65536.0 
};
/********************************/
static double stx104_adc_offset[8] = 
{ 
	-10.0, -5.0, -2.5, -1.25, 0.0, 0.0, 0.0, 0.0 
};

/**********************************************************************************************************//**
@ingroup stx104_ai
\brief
Apex Universal Library STX104 specific function that retrieves the STX104 analog input (AI) configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out] cfg A pointer to the ::stx104_ai_cfg_type structure that will receive the analog input configuration information.  
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AI_CFG_Get( int board, stx104_ai_cfg_type* cfg )
{
	int						error_code;
	devmgr_device_type *	device;
	stx104_dataset_type *	ds;
	size_t					address;
	uint8_t					reg_value;
	uint16_t				scratch_uint16;
	uint16_t				channels_per_frame;
	uint32_t				time_reg;
	double					time_ns;
	STX104_GEN_JMP_TYPE		jumpers;
	size_t					index;
	uint16_t				firmware_revision;

	STX104_Gen_Jumpers_Get( board, &jumpers );

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AI_CFG_Get_Error;
	ds = (stx104_dataset_type *) device->private_data;

	Mem_Set( cfg, 0, sizeof( stx104_ai_cfg_type ) );

	/* channels */
	address = device->io_region_offset + STX104_CHANNEL;
	IO_Read_U8( device->io_device, address, &reg_value );
	ds->ai_channel_reg = reg_value;
	cfg->first_channel = (STX104_AI_ENUM)(   reg_value & 0x0F        );
	cfg->last_channel  = (STX104_AI_ENUM)( ( reg_value & 0xF0 ) >> 4 );

	channels_per_frame = (uint16_t) ( cfg->last_channel - cfg->first_channel + 1 );
	ds->ai_channels_per_frame = channels_per_frame;

	STX104_Index_Read_U32( board, STX104_ANALOG_INPUT_BURST_TIMER, &time_reg );
	time_ns = ((double) time_reg) * STX104_CLOCK_RESOLUTION_NS + STX104_AI_INTRA_SAMPLE_TIME_MIN_NS;
	cfg->burst_timer_sec = time_ns * 1.0e-9; /* convert to seconds */
	cfg->burst_timer_ns = (uint32_t) time_ns;

	STX104_Index_Read_U32( board, STX104_ANALOG_INPUT_FRAME_TIMER, &time_reg );
	time_ns = ((double) (time_reg + 1)) * STX104_CLOCK_RESOLUTION_NS;
	cfg->frame_timer_sec = time_ns * 1.0e-9; /* convert to seconds */
	cfg->frame_timer_ns = (uint32_t) time_ns;

	STX104_Index_Read_U32( board, STX104_ANALOG_INPUT_FRAME_MAX,   &cfg->frame_maximum );
	
	/* gain */
	address = device->io_region_offset + STX104_ADC_CONFIGURATION;
	IO_Read_U8( device->io_device, address, &reg_value );
	cfg->gain = (STX104_AI_GAIN_ENUM)( reg_value & 0x03 );

	/* include the unipolar bit as well which we need for data conversions */
	ds->ai_gain = reg_value & 0x07; 
	/* save unipolar value */
	if ( jumpers.J9_ai_unipolar ) ds->ai_unipolar = 1;
	else                          ds->ai_unipolar = 0;

	/* MAVG, SAIFTTS, SAIFTS, nSGATE, strobe source, encoding behavior if relevant */
	STX104_Index_Read_U16( board, STX104_ANALOG_INPUT_GENERAL_CONFIG, &scratch_uint16 );
	ds->ai_general_config_reg = scratch_uint16;
	reg_value = (uint8_t)(scratch_uint16 & 0x00FF );  /* provide lower byte for decoding below */

	STX104_Gen_Revision_Get( board, &firmware_revision );
	ds->firmware_revision = firmware_revision; 
	if ( firmware_revision >= STX104_REVISION_120328 )
	{ /* get aditional parameters */
		scratch_uint16 &= STX104_AIGC_ENCODE | STX104_AIGC_ENCODE_BEHAVIOR;
      scratch_uint16  = ( scratch_uint16 >> STX104_AI_AIGC_ENCODE_SHIFT );  /* AI_ENCODE,AI_ENCODE_BEHAVIOR[1:0] */

		switch( scratch_uint16 )
		{
			case 0: /* AI_ENCODE='0' AI_ENCODE_BEHAVIOR[1:0] = "00" */
         case 2:
				cfg->encoding = AI_ENCODING_STRAIGHT_BINARY;
				break;
			case 1: /* AI_ENCODE='0' AI_ENCODE_BEHAVIOR[1:0] = "01" */
         case 3:
				cfg->encoding = AI_ENCODING_STRAIGHT_BINARY_ALLOW_POLARITY;
				break;
			case 4: /* AI_ENCODE='1' AI_ENCODE_BEHAVIOR[1:0] = "00" */
				cfg->encoding = AI_ENCODING_TWOS_COMPLEMENT;
				break;
			case 5: /* AI_ENCODE='1' AI_ENCODE_BEHAVIOR[1:0] = "01" */
				cfg->encoding = AI_ENCODING_TWOS_COMPLEMENT_ALLOW_POLARITY;
				break;
			case 6: /* AI_ENCODE='1' AI_ENCODE_BEHAVIOR[1:0] = "10" */
         case 7:
				cfg->encoding = AI_ENCODING_TWOS_COMPLEMENT_ABSOLUTE;
				break;
		default:
				break;
		}

		STX104_Index_Read_U16( board, STX104_ANALOG_INPUT_POLARITY_CONFIG, &scratch_uint16 );
		ds->ai_polarity_config = scratch_uint16;
		for ( index = 0; index < STX104_AI_QTY; index++ )
		{
			if ( scratch_uint16 & 0x0001 ) cfg->polarity[index] = true;
			else                           cfg->polarity[index] = false;

			scratch_uint16 = scratch_uint16 >> 1;
		}
	}
	cfg->strobe_source = ( STX104_AI_STROBE_SOURCE_ENUM)( reg_value & 0x0F );
	if ( ( reg_value & 0x40 ) == 0x40 ) cfg->sync_aift_to_trigger_start = true;
	if ( ( reg_value & 0x20 ) == 0x20 ) cfg->sync_aifc_to_trigger_start = true;
	if ( ( reg_value & 0x10 ) == 0x10 ) cfg->use_triggering_subsystem = true;
   /* moving average filter - MAVG_INV
      moving average jumper position is M3

         Desire jumper to be a '1' or set or enable moving average filter
         jumper  mavg_inv   actual value
            0      0          0
            0      1          0
            1      0          1
            1      1          1
   */
	if ( jumpers.M3_moving_average_filter  )
	{
		cfg->moving_average_filter = true;
	}
	/* HSFIFOEN */
	STX104_Index_Read_U8( board, STX104_FIFO_CONFIGURATION, &reg_value );
	if ( ( reg_value & 0x80 ) == 0x80 ) cfg->high_speed_pre_queue_fifo = true; 
	return error_code;
STX104_AI_CFG_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_ai
\brief
Apex Universal Library STX104 specific function that sets the STX104 analog input (AI) configuration.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] cfg A pointer to the ::stx104_ai_cfg_type structure that will set the analog input configuration information.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AI_CFG_Set( int board, stx104_ai_cfg_type * cfg )
{
	int 					error_code;
	devmgr_device_type * 			device;
	stx104_dataset_type * 	ds;
	size_t address;
	STX104_GEN_JMP_TYPE 	jumpers;
	uint16_t 				firmware_revision;
	size_t 					index;
	uint16_t 				scratch_uint16;
	uint8_t 				reg_value;

	uint16_t 				channels_per_frame;
	uint16_t 				ai_general_cfg_reg;
	uint32_t 				time_reg;
	double time_ns;
	double time_sec;
	double time_intrasample_ns;
	double intrasample_time_sec;
	double frame_time_sec;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AI_CFG_Set_Error;
	ds = (stx104_dataset_type *) device->private_data;

	STX104_Gen_Jumpers_Get( board, &jumpers );
	ai_general_cfg_reg = ds->ai_gain;

	/* general parameter testing */
	if ( (jumpers.M0_mode == false)        || (jumpers.M1_mode == false)       ||
		 (jumpers.M2_dacs_16bits == false)  )
/*TODO		 (jumpers.M2_dacs_16bits == false) || (jumpers.M4_address_10bit_mode == false) )*/
	{
#if defined( STX104_AI_DEBUG )
		Debug_Log( STX104_AI_DEBUG_LOG "%s: please install jumpers M0 M1 M2\n", __func__ );
#endif
		error_code = EC_STX104_AI_JUMPERS_M0_M1_M2_M4;	goto STX104_AI_CFG_Set_Error;
	}
	
	if ( jumpers.J8_ai_differential == true ) 
	{ /* differential */
		if ( cfg->first_channel > STX104_AI_CH7 ) 
		{
#if defined( STX104_AI_DEBUG )
	Debug_Log( STX104_AI_DEBUG_LOG "%s: first channel out of range, differential mode\n", __func__ );
#endif
			error_code = EC_STX104_AI_FIRST_CHANNEL_OUT_OF_RANGE;	goto STX104_AI_CFG_Set_Error;
		}
		if ( cfg->last_channel  > STX104_AI_CH7 ) 
		{
#if defined( STX104_AI_DEBUG )
			Debug_Log( STX104_AI_DEBUG_LOG "%s: ast channel out of range, differential mode\n", __func__ );
#endif
			error_code = EC_STX104_AI_LAST_CHANNEL_OUT_OF_RANGE;	goto STX104_AI_CFG_Set_Error;
		}
	}
	else
	{ /* single ended */
		if ( cfg->first_channel > STX104_AI_CH15 ) 
		{  
#if defined( STX104_AI_DEBUG )
			Debug_Log( STX104_AI_DEBUG_LOG "%s: first channel out of range, single-ended\n", __func__ );
#endif
			error_code = EC_STX104_AI_FIRST_CHANNEL_OUT_OF_RANGE;	goto STX104_AI_CFG_Set_Error;
		}
		if ( cfg->last_channel  > STX104_AI_CH15 ) 
		{ 
#if defined( STX104_AI_DEBUG )
			Debug_Log( STX104_AI_DEBUG_LOG "%s: last channel out of range, single-ended\n", __func__ );
#endif
			error_code = EC_STX104_AI_LAST_CHANNEL_OUT_OF_RANGE;	goto STX104_AI_CFG_Set_Error;
		}
	}

	if ( cfg->last_channel < cfg->first_channel )
	{ 
#if defined( STX104_AI_DEBUG )
		Debug_Log( STX104_AI_DEBUG_LOG "%s: first channel greater than last channel\n", __func__ );
#endif
		error_code = EC_STX104_AI_FIRST_CHAN_GREATER_THAN_LAST_CHAN;	goto STX104_AI_CFG_Set_Error;
	}

   /**** Intra-sample time calculation ****/
/* burst_timer_sec has priority over burst_timer_ns, since burst_timer_ns is deprecated */
   intrasample_time_sec = 0.0;
   if ( cfg->burst_timer_ns  > 0 )
   {
   	intrasample_time_sec = ((double) cfg->burst_timer_ns ) * 1.0e-9; /* convert to seconds */
   }
   if ( cfg->burst_timer_sec > 0 )
   {
   	intrasample_time_sec = cfg->burst_timer_sec;
   }

   time_ns = ( ((double) (UINT32_MAX - 1)) * STX104_CLOCK_RESOLUTION_NS );
   time_sec = time_ns * 1.0e-9; /* convert to sectonds */
   if ( intrasample_time_sec > time_sec )
   {
#if defined( STX104_AI_DEBUG )
   		char time_text[STX104_AI_DEBUG_STRING_SIZE];
   		Float_To_String_Multiplier_Suffix( time_sec, time_text, STX104_AI_DEBUG_STRING_SIZE, NULL );
		Debug_Log( STX104_AI_DEBUG_LOG "%s:intra-sample timing must be less than %s seconds\n", __func__, time_text );
#endif
		error_code = EC_STX104_AI_INTRA_SAMPLE_TIMING_TOO_LARGE_AIBT;	goto STX104_AI_CFG_Set_Error;
   }


	time_intrasample_ns = STX104_AI_Intra_Sample_Timing_Minimum( cfg );
	if ( ( intrasample_time_sec * 1.0e9 /* convert to nSec */ ) < time_intrasample_ns )
	{
#if defined( STX104_AI_DEBUG )
   		char time_text[STX104_AI_DEBUG_STRING_SIZE];
		Float_To_String_Multiplier_Suffix( time_intrasample_ns * 1.0e-9 /* convert to seconds */, time_text, STX104_AI_DEBUG_STRING_SIZE, NULL );
		Debug_Log( STX104_AI_DEBUG_LOG "%s:  intra-sample timing (AIBT) needs to be %s or greater\n", __func__, time_text );
#endif
		error_code = EC_STX104_AI_INTRA_SAMPLE_TIMING_TOO_SMALL_AIBT;	goto STX104_AI_CFG_Set_Error;
	}


   /**** frame time calculation ****/
/* frame_timer_sec has priority over frame_timer_ns, since frame_timer_ns is deprecated */
   frame_time_sec = 0.0;
   if ( cfg->frame_timer_ns  > 0 )
   {
   	frame_time_sec = ((double) cfg->frame_timer_ns ) * 1.0e-9; /* convert to seconds */
   }
   if ( cfg->frame_timer_sec > 0 )
   {
   	frame_time_sec = cfg->frame_timer_sec;
   }

   time_ns = ( ((double) (UINT32_MAX - 1)) * STX104_CLOCK_RESOLUTION_NS );
   time_sec = time_ns * 1.0e-9; /* convert to sectonds */
   if ( frame_time_sec > time_sec )
   {
#if defined( STX104_AI_DEBUG )
   		char time_text[STX104_AI_DEBUG_STRING_SIZE];
   		Float_To_String_Multiplier_Suffix( time_sec, time_text, STX104_AI_DEBUG_STRING_SIZE, NULL );
		Debug_Log( STX104_AI_DEBUG_LOG "%s: frame timing must be less than %s seconds\n", __func__, time_text );
#endif
		error_code = EC_STX104_AI_INTRA_SAMPLE_TIMING_TOO_LARGE_AIBT;	goto STX104_AI_CFG_Set_Error;
   }

	time_ns = ( (double) (cfg->last_channel - cfg->first_channel + 1) ) * time_intrasample_ns; /* previous calculated intra-sample time */
	if ( ( frame_time_sec * 1.0e9 /* convert to nSec */ ) < time_ns )
	{
#if defined( STX104_AI_DEBUG )
		char time_text[STX104_AI_DEBUG_STRING_SIZE];
		Float_To_String_Multiplier_Suffix( time_ns * 1.0e-9 /* convert to seconds */, time_text, STX104_AI_DEBUG_STRING_SIZE, NULL );
		Debug_Log( STX104_AI_DEBUG_LOG "%s: frame timing (AIFT) needs to be %s or greater\n", __func__, time_text );
#endif
		error_code = EC_STX104_AI_FRAME_TIMING_TOO_SMALL_AIFT;	goto STX104_AI_CFG_Set_Error;
	}
/*
	if ( device_allocation[board].interrupt_active )
	{
		AUL_Error_Log("STX104_AI_CFG_SET: Error - interrupt active, cannot commit configuration.\n" );
		error_code = EC_STX104_DEVICE_CFG_UNAVAILABLE_INTERRUPT_ACTIVE;
	}
	else
	{
		*/

/*ai_time_intra_sample_ns = AIBT * ( 25 nSec ) + ( 5000 nSec ). ==> AIBT = ( ai_time_intra_sample_ns - ( 5000 nSec ) ) / ( 25 nSec )*/
	time_reg =  (uint32_t) ( ( ( intrasample_time_sec * 1.0e9 /* convert to nSec */ ) - STX104_AI_INTRA_SAMPLE_TIME_MIN_NS ) / STX104_CLOCK_RESOLUTION_NS );
	STX104_Index_Write_U32( board, STX104_ANALOG_INPUT_BURST_TIMER, time_reg );

/*ai_time_frame_ns = ( AIFT + 1 ) * ( 25 nSec ) ==>   AIFT = (ai_time_frame_ns / ( 25 nS)) - 1*/
	time_reg = (uint32_t) ( ( ( frame_time_sec * 1.0e9 /* convert to nSec */ ) - STX104_CLOCK_RESOLUTION_NS ) / STX104_CLOCK_RESOLUTION_NS );
	STX104_Index_Write_U32( board, STX104_ANALOG_INPUT_FRAME_TIMER, time_reg );
	STX104_Index_Write_U32( board, STX104_ANALOG_INPUT_FRAME_MAX, cfg->frame_maximum );

	/* gain */
	address = device->io_region_offset + STX104_ADC_CONFIGURATION;
	IO_Read_U8( device->io_device, address, &reg_value );
	reg_value = ( reg_value & 0xFC ) | ( cfg->gain & 0x03 );
	IO_Write_U8( device->io_device, address, reg_value );
	/* save unipolar value */
	if ( jumpers.J9_ai_unipolar ) 
	{
		ds->ai_unipolar = 1;
		ds->ai_gain     = ( cfg->gain & 0x03 ) + 0x04;
	}
	else                          
	{
		ds->ai_unipolar = 0;
		ds->ai_gain     = ( cfg->gain & 0x03 );
	}
	/* HSFIFOEN */
	if ( cfg->high_speed_pre_queue_fifo != false )   reg_value = 0x80;
   else                                             reg_value = 0x00; 
	STX104_Index_Write_U8( board, STX104_FIFO_CONFIGURATION, reg_value );

	/* moving average filter - MAVG_INV
      moving average jumper position is M3

      Desire jumper to be a '1' or set or enable moving average filter
      jumper  mavg_inv   mavg_in (new)
         0      0          1   <-- invert to set
         0      1          0   <-- invert to set
         1      0          0   <-- no change
         1      1          1   <-- no change

      Desire jumper to be a '0' or clear or disable moving average filter
      jumper  mavg_inv   mavg_in (new)
         0      0          0   <-- no change
         0      1          1   <-- no change
         1      0          1   <-- invert to set
         1      1          0   <-- invert to set

      In summary, set to the same existing value, then only change if
      required.
   */
   scratch_uint16 = STX104_AIGC_MAVG_IN & ai_general_cfg_reg; /* set to existing value */
	if ( false == cfg->moving_average_filter )
	{
		if ( false != jumpers.M3_moving_average_filter )
      {
         if ( STX104_AIGC_MAVG_IN & ai_general_cfg_reg )
         { /* is set, so clear it */
            scratch_uint16 = ((uint16_t) ~STX104_AIGC_MAVG_IN)& ai_general_cfg_reg;
         }
         else
         { /* is clear, so set it */
            scratch_uint16 = ((uint16_t) STX104_AIGC_MAVG_IN) | ai_general_cfg_reg;
         }
      }
	}
	else
	{
		if ( false == jumpers.M3_moving_average_filter )
      {
         if ( STX104_AIGC_MAVG_IN & ai_general_cfg_reg )
         { /* is set, so clear it */
            scratch_uint16 = ((uint16_t) ~STX104_AIGC_MAVG_IN) & ai_general_cfg_reg;
         }
         else
         { /* is clear, so set it */
            scratch_uint16 =  ((uint16_t) STX104_AIGC_MAVG_IN) | ai_general_cfg_reg;
         }
      }
	}

	/* sync_aift_to_trigger_start - SAIFTTS  */
	if ( false != cfg->sync_aift_to_trigger_start ) scratch_uint16 |= STX104_AIGC_SAIFTTS;

	/* sync_aifc_to_trigger_start - SAIFCTS */
	if ( cfg->sync_aifc_to_trigger_start != false ) scratch_uint16 |= STX104_AIGC_SAIFCTS;

	/* use_triggering_subsystem - nSGATE  */
	if ( cfg->use_triggering_subsystem != false )   scratch_uint16 |= STX104_AIGC_nSGATE;

	/* strobe_source */
	scratch_uint16 |= (uint16_t)( cfg->strobe_source & STX104_AIGC_AISS );

	STX104_Gen_Revision_Get( board, &firmware_revision );
/*   device_allocation[board].reg_store[STX104_FIRMWARE_REVISION].word[0] = firmware_revision;*/
	ds->firmware_revision = firmware_revision;
	if ( firmware_revision >= STX104_REVISION_120328 )
	{ /* assign these parameters as well */
      uint16_t scratch;

		switch( cfg->encoding )
		{
			case AI_ENCODING_STRAIGHT_BINARY:                 /* AI_ENCODE='0' AI_ENCODE_BEHAVIOR[1:0] = "00" */
			case AI_ENCODING_STRAIGHT_BINARY_ALLOW_POLARITY:  /* AI_ENCODE='0' AI_ENCODE_BEHAVIOR[1:0] = "01" */
			case AI_ENCODING_TWOS_COMPLEMENT:                 /* AI_ENCODE='1' AI_ENCODE_BEHAVIOR[1:0] = "00" */
			case AI_ENCODING_TWOS_COMPLEMENT_ALLOW_POLARITY:  /* AI_ENCODE='1' AI_ENCODE_BEHAVIOR[1:0] = "01" */
        	case AI_ENCODING_TWOS_COMPLEMENT_ABSOLUTE:        /* AI_ENCODE='1' AI_ENCODE_BEHAVIOR[1:0] = "10" */
				scratch = (uint16_t) cfg->encoding;
				break;
			default:
/*				AUL_Error_Log("STX104_AI_CFG_SET: Error - unknown encoding value out of range.\n" );*/
				error_code = EC_STX104_AI_ENCODING_OUT_OF_RANGE;	goto STX104_AI_CFG_Set_Error;
		}
		scratch_uint16 |= (scratch << STX104_AI_AIGC_ENCODE_SHIFT);

		STX104_Index_Write_U16( board, STX104_ANALOG_INPUT_GENERAL_CONFIG, scratch_uint16 );
      /* save the updated value */
		ds->ai_general_config_reg = scratch_uint16;
		/* polarity register */
		scratch_uint16 = 0;
      index = STX104_AI_QTY;
      do
      {
         index--;
         scratch_uint16 = scratch_uint16 << 1;
         if ( cfg->polarity[index] ) scratch_uint16 |= 0x0001;

      } while ( index != 0 );
      /* commit the updated value to hardware */
		STX104_Index_Write_U16( board, STX104_ANALOG_INPUT_POLARITY_CONFIG, scratch_uint16 );
      /* save the updated value */
		ds->ai_polarity_config = scratch_uint16; 
	}
	else
	{
		STX104_Index_Write_U8( board, STX104_ANALOG_INPUT_GENERAL_CONFIG, ((uint8_t)(scratch_uint16 & 0x00FF)) );
      /* save the updated value */
		ds->ai_general_config_reg = scratch_uint16;
	}
/*TODO:
 1. when AI is put in run mode, the channel register needs to be written.*/
	/* turn on burst mode */
	STX104_Index_Write_U8( board, STX104_BURST_FUNCTION_ENABLE_INDEXED, 0x40 ); /* enable DAS1602 functionality */
	STX104_Index_Write_U8( board, STX104_BURST_MODE_ENABLE_INDEXED,     0x40 ); /* enable ADC-burst sampling    */
	STX104_Index_Write_U8( board, STX104_CONVERSION_DISABLE_INDEXED,    0x00 ); /* ADC conversions enabled      */
	reg_value = ( (cfg->last_channel & 0x0F) - (cfg->first_channel & 0x0F) ) << 4;
	address = device->io_region_offset + STX104_PACER_CLOCK_CONTROL;
	ds->ai_pacer_clock_control_shadow_copy;
	IO_Write_U8( device->io_device, address, reg_value );

	/* store the channel register for sample start up sequencing */
	reg_value = ( (cfg->last_channel & 0x0F) << 4 ) | ( cfg->first_channel & 0x0F );
	ds->ai_channel_reg = reg_value;

	channels_per_frame = (uint16_t) ( cfg->last_channel - cfg->first_channel + 1 );
	ds->ai_channels_per_frame = channels_per_frame; 
	address = device->io_region_offset + STX104_CHANNEL;
	IO_Write_U8( device->io_device, address, reg_value );
	return error_code;
STX104_AI_CFG_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_ai
\brief
Apex Universal Library STX104 specific function that retrieves the number of available analog input samples remaining for readout within the FIFO.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out]  fifo_count Pointer to an unsigned 32-bit variable which upon return will contain the quantity of analog input samples available in the FIFO. The value can range from 0 to 1,048,576 samples; up to 1,050,624 samples when high_speed_pre_queue_fifo is set true). 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AI_FIFO_Data_Count_Get( int board, uint32_t* fifo_count )
{
	int							error_code;
	devmgr_device_type *				device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AI_FIFO_Data_Count_Get_Error;
	STX104_Index_Read_U32( board, STX104_FIFO_DATA_AVAILABLE,   fifo_count );	
	return error_code;
STX104_AI_FIFO_Data_Count_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_ai
\brief
Apex Universal Library STX104 specific function that retrieves the number of analog input frames that have been collected irrespective of any analog data that has been read out.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out]  frame_count Pointer to an unsigned 32-bit variable which upon return will contain the quantity of analog input frames that have been collected. The range is 0 to 2,147,483,647. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AI_Frame_Count_Get( int board, uint32_t* frame_count )
{
	int							error_code;
	devmgr_device_type *				device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AI_Frame_Count_Get_Error;
	STX104_Index_Read_U32( board, STX104_ANALOG_INPUT_FRAME_COUNT, frame_count );	
	return error_code;
STX104_AI_Frame_Count_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************
TODO:
1. pass another parameter that is a "wait until"  or no wait sort of thing.  Block/non-block
2. If data is collected via interrupt, then this copies from the temporary interrupt buffer.
DONE: 3. THERE NEEDS TO BE A FIFO_FULL TEST AND THAT ERROR CODE NEEDS TO BE REPORTED.
4. mmap()??
http://people.ee.ethz.ch/~arkeller/linux/multi/kernel_user_space_howto-8.html

*/
/**********************************************************************************************************//**
@ingroup stx104_ai
\brief
Apex Universal Library STX104 specific function that retrieves up to the number of requested data frames from the STX104 FIFO.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[out]  buffer Pointer to the destination buffer. Must be sized to: frame_count_request * sizeof( UINT16 ) 
\param[out] frame_count_actual Returns the actual data frame quantity that have been copied to the buffer. 
\param[in] frame_count_request Sent to the function to request up to a maximum number of data frames.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AI_Frames_Get( int board, uint16_t* buffer, uint16_t* frame_count_actual, uint16_t frame_count_request )
{
	int							error_code;
	devmgr_device_type *				device;
	stx104_dataset_type *	ds;
	size_t						address;
	uint32_t		fifo_count_register;
   size_t		fifo_count;
	size_t		channels_per_frame;
	size_t		samples_requested;
	size_t		frames_requested;
	uint8_t		fifo_status;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AI_Frames_Get_Error;
	ds = (stx104_dataset_type *) device->private_data;
	address = device->io_region_offset + STX104_FIFO_FLAGS;
	IO_Read_U8( device->io_device, address, &fifo_status );
	if ( fifo_status & 0x20 )
	{
#if defined( STX104_AI_DEBUG )
		Debug_Log( STX104_AI_DEBUG_LOG "%s: Error - FIFO full\n", __func__ );
#endif
      *frame_count_actual = 0;
		error_code = EC_STX104_AI_FIFO_FULL;	goto STX104_AI_Frames_Get_Error;
	}
	else
	{
		STX104_Index_Read_U32( board, STX104_FIFO_DATA_AVAILABLE, &fifo_count_register );
      fifo_count = (size_t) fifo_count_register;
		if ( fifo_count == 0 )
		{
			*frame_count_actual = 0;
		}
		else
		{
			channels_per_frame = ds->ai_channels_per_frame;
			samples_requested  = channels_per_frame * frame_count_request;
			frames_requested   = frame_count_request;

			if ( samples_requested > fifo_count )
			{
				frames_requested  = fifo_count / channels_per_frame;
				samples_requested = frames_requested * channels_per_frame;
			}

			*frame_count_actual = (uint16_t) frames_requested;
			address = device->io_region_offset + STX104_ADC_DATA;
			IO_Read_Repeat_U16( device->io_device, address, buffer, samples_requested ); 
		}
	}
	return error_code;
STX104_AI_Frames_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************//**
@ingroup stx104_ai
\brief
Apex Universal Library STX104 specific function that converts raw ADC data values to voltage returned in a floating point value.
The function looks up the current analog input behavior (i.e. bipolar/unipolar and the gain) and converts the raw ADC binary data to a formatted floating point voltage value. 

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 
\param[in] raw_value ADC raw value sent to the function to be converted. 
\param[out]  voltage Returns the voltage value. 
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AI_Raw_To_Volts( int board, uint16_t raw_value, double * voltage )
{
	int						error_code;
	devmgr_device_type *	device;
	stx104_dataset_type *	ds;
	int						gain_index;
	uint16_t				firmware_revision;
	uint16_t				ai_gfr;
	size_t					encoding;
	int						twos_complement;

	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AI_Raw_To_Volts_Error;
	ds = (stx104_dataset_type *) device->private_data;
	firmware_revision = ds->firmware_revision;

	if ( firmware_revision >= STX104_REVISION_120328 )
	{
		ai_gfr     = ds->ai_general_config_reg;
      	  encoding   = (size_t) ( ( ai_gfr & ( STX104_AIGC_ENCODE | STX104_AIGC_ENCODE_BEHAVIOR ) ) >> STX104_AI_AIGC_ENCODE_SHIFT );
		gain_index = (int) ds->ai_gain;
		switch( encoding )
		{
		case AI_ENCODING_STRAIGHT_BINARY:                 /* AI_ENCODE='0' AI_ENCODE_BEHAVIOR[1:0] = "00" */
		case AI_ENCODING_STRAIGHT_BINARY_ALLOW_POLARITY:  /* AI_ENCODE='0' AI_ENCODE_BEHAVIOR[1:0] = "01" */
			*voltage = ((double) raw_value) * stx104_adc_gain[gain_index] + stx104_adc_offset[gain_index];
			break;
		 case AI_ENCODING_TWOS_COMPLEMENT:                 /* AI_ENCODE='1' AI_ENCODE_BEHAVIOR[1:0] = "00" */
		 case AI_ENCODING_TWOS_COMPLEMENT_ALLOW_POLARITY:  /* AI_ENCODE='1' AI_ENCODE_BEHAVIOR[1:0] = "01" */
		 case AI_ENCODING_TWOS_COMPLEMENT_ABSOLUTE:        /* AI_ENCODE='1' AI_ENCODE_BEHAVIOR[1:0] = "10" */
			twos_complement = (int)( (short) raw_value );
			*voltage        = ((double) twos_complement) * stx104_adc_gain[gain_index];
				break;
			default:
				/*AUL_Error_Log("STX104_AI_CFG_SET: Error - unknown encoding value out of range.\n" );*/
				error_code = EC_STX104_AI_ENCODING_OUT_OF_RANGE;
				goto STX104_AI_Raw_To_Volts_Error;
		}
	/*      firmware_revision = device_allocation[board].reg_store[STX104_REG_STORE_AI_POLARITY_CONFIG].word[0];*/
		firmware_revision = ds->ai_polarity_config;  /*TODO:  makes not sense!!!!!*/
	}
	else
	{
		gain_index = (int) ds->ai_gain; /*device_allocation[board].reg_store[STX104_REG_STORE_AI_GAIN].byte[0];*/
		*voltage = ((double) raw_value) * stx104_adc_gain[gain_index] + stx104_adc_offset[gain_index];
	}
/*printf( "STX104_AI_Raw_To_Volts index=%d\n", index );
		*voltage = 0.0;
TODO:
 1. Provide a mechanism for software calibration y=mx+b
	*/
	error_code = EC_STX104_AI_UNCALIBRATED_CONVERSION_WARNING;
	return error_code;
STX104_AI_Raw_To_Volts_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_ai
\brief
Apex Universal Library STX104 specific function that performs an analog input strobe or frame request operation.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 

\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AI_Strobe( int board )
{
	int							error_code;
	devmgr_device_type *		device;
	size_t						address;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AI_Strobe_Error;
	address = device->io_region_offset + STX104_SOFTWARE_STROBE;
	IO_Write_U8( device->io_device, address, 0x00 );
	return error_code;
STX104_AI_Strobe_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup stx104_ai
\brief
Apex Universal Library STX104 specific function that read/writes to channel register to restart the acquisition and clear the FIFO.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open. 

\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AI_Aquisition_Restart( int board )
{
	int							error_code;
	devmgr_device_type *		device;
	size_t						address;
	uint8_t						channel_register;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto STX104_AI_Aquisition_Restart_Error;
	address = device->io_region_offset + STX104_CHANNEL;
	IO_Read_U8( device->io_device, address, &channel_register );
	IO_Write_U8( device->io_device, address, channel_register );
	return error_code;
STX104_AI_Aquisition_Restart_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/**********************************************************************************************************//**
@ingroup  stx104_ai
\brief
 Apex Universal Library STX104 analog input specific function

\param[out] config A pointer to the ::tracere_cfg_type structure that will be used to initialize the structure.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
AUL_DLL_API int AULCALLCONV STX104_AI_Config_Initialize( stx104_ai_cfg_type * config )
{
	int error_code;

	Mem_Set( config, 0, sizeof( stx104_ai_cfg_type ) );
	config->type = SYS_TYPE_MAKE( stx104_ai_cfg_type );

	config->burst_timer_ns = 5000;
	config->frame_timer_ns = 5000;

	return SUCCESS;
STX104_AI_Config_Initialize_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_STX104_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/**********************************************************************************************************
*/
void STX104_AI_Termination( void )
{
}
/**********************************************************************************************************
*/
void STX104_AI_Initialization( void )
{
}
