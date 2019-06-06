
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

#include "cfgrprt.h"
#include "../aul/aul_public.h"
#include <stddef.h> /* NULL */
#include <time.h>   /* time_t, time(), ctime() */

/*TODO:  review  "= %lu nS\n", prefix, (int) ai_cfg->burst_timer_ns"  not totally correct, need unsigned.*/

/***************************************************************************/

static const char * bool_str[] = { "false", "true " 	};  /* value ? bl[1] : bl[0].  if value then print bl[1] else print bl[0] */
static const char * jmp_str[]  = { "open", "installed" 	};  /* value ? bl[1] : bl[0].  if value then print bl[1] else print bl[0] */


/***************************************************************************/
/*
*/
void STX104_Report_Status( int board, FILE * out, char * prefix )
{
	int					error_code;  /* used for debugging */
	int					driver_compiled_with_debug_flag;
	unsigned long		aul_driver_revision_code;
	unsigned long		process_id;
	long				aul_library_revision_code;
	BOOL				state;
	uint16_t			stx104_firmware_revision;
	STX104_GEN_JMP_TYPE	jumpers;

	error_code = AUL_Driver_Revision( &aul_driver_revision_code );
	fprintf( out, "%s Driver: name = %s, revision = %ld\n", prefix, AUL_Driver_Name(), aul_driver_revision_code );


	error_code = AUL_Driver_Is_Debug_Active( &driver_compiled_with_debug_flag );
	if ( driver_compiled_with_debug_flag )
	{
		fprintf( out, "           *** NOTICE ***\n" );
		fprintf( out, "           DRIVER IN DEBUG MODE: DATA BANDWIDTH LIMITED & KERNEL LOG BLOATING POSSIBLE\n" );
	}

	error_code = AUL_Library_Revision( &aul_library_revision_code );
	fprintf( out, "%sLibrary: name = %s, revision = %ld\n", prefix, AUL_Library_Name(), aul_library_revision_code );

	error_code = AUL_Library_Process_Id_Get( &process_id );
	fprintf( out, "%sLibrary: process ID           = %u\n", prefix, process_id );

	/* hardware revision from FPGA firmware */
	/* obtain the STX104 firmware revision from the hardware */
	error_code = STX104_Gen_Revision_Get( board, &stx104_firmware_revision );

	fprintf( out, "%sstx104_firmware_revision      = ", prefix );
	switch( stx104_firmware_revision )
	{
		case STX104_REVISION_071604:	fprintf( out, "STX104_REVISION_071604" );	break;
		case STX104_REVISION_080214:	fprintf( out, "STX104_REVISION_080214/080407" );	break;
		/*case STX104_REVISION_080407:	fprintf( out, "STX104_REVISION_080407" );	break;*/
		case STX104_REVISION_090115:	fprintf( out, "STX104_REVISION_090115" );	break;
		case STX104_REVISION_120328:	fprintf( out, "STX104_REVISION_120328" );	break;
		case STX104_REVISION_140101:	fprintf( out, "STX104_REVISION_140101" );	break;
	}
	fprintf( out, "\n" );

#if(0)
//TODO:
//#define NAME_STRING_LENGTH  32
//   UINT32 value;
//   char name[NAME_STRING_LENGTH];
//   error_code = AUL_Device_Base_Name_Get( board, name );
//   fprintf( out, "%sBoard name                    = %s\n", prefix, name );
//   error_code = AUL_Device_Base_Type_Get( board, &value ); //need enumeration of types.
//   fprintf( out, "%sBoard type                    = %s\n", prefix, value );
//   error_code = AUL_Device_Base_Address_Get( board, &value );
//   fprintf( out, "%sBase address                  = %ld\n", prefix, value );
//   error_code = AUL_Device_Base_Irq_Get( board, &value );
//   fprintf( out, "%sInterrupt IRQ                 = %ld\n", prefix, value );
//#undef NAME_STRING_LENGTH

//	/******************************************************/
//	/* The next few lines are to see the parsed hardware definition file
//   	information.
//	*/
//	error_code = AUL_Hardware_Definition_Get( hw_list, AUL_HW_DEF_ROW_QUANTITY, AUL_HW_DEF_MNEMONIC_LENGTH, &row_actual_count );
//
//   fprintf( out, "%sHardware definition file:\n" );
//	fprintf( out_file, "%s  ROW  BASE_ADDRESS  IRQ  MNEMONIC\n" );
//	for ( index = 0; index < row_actual_count; index++ )
//	{
//		if ( strlen( hw_list[index].mnemonic ) > 0 )
//		{
//			fprintf( out_file, "%s  %2d ", index );
//			fprintf( out_file, "%s       0x%03X", hw_list[index].base_address      );
//			fprintf( out_file, "%s        %2d", hw_list[index].interrupt_number  );
//			fprintf( out_file, "%s     %s\n", hw_list[index].mnemonic          );
//		}
//		fprintf( out_file, "%s   %s", hw_list[index].type         );
//	}
#endif

   /******************************************************/
	error_code = STX104_Gen_Jumpers_Get( board, &jumpers );
	fprintf( out, "%sJumpers: hardware\n", prefix );
	fprintf( out, "%s  M0_mode                     = %s\n", prefix, ( jumpers.M0_mode ? jmp_str[1] : jmp_str[0] ) );
	fprintf( out, "%s  M1_mode                     = %s\n", prefix, ( jumpers.M1_mode ? jmp_str[1] : jmp_str[0] ) );

	fprintf( out, "%s  M2_dacs_16bits              = %s",   prefix, ( jumpers.M2_dacs_16bits ? jmp_str[1] : jmp_str[0] ) );
	if ( jumpers.M2_dacs_16bits ) fprintf( out, " (16-bit data)\n" );
	else                          fprintf( out, " (12-bit data)\n" );

	fprintf( out, "%s  M3_moving_average_filter    = %s\n", prefix, ( jumpers.M3_moving_average_filter ? jmp_str[1] : jmp_str[0] ) );

	fprintf( out, "%s  M4_address_10bit_mode       = %s",   prefix, ( jumpers.M4_address_10bit_mode ? jmp_str[1] : jmp_str[0] ) );
	if ( jumpers.M4_address_10bit_mode ) fprintf( out, " (10-bit CPU address)\n" );
	else                                 fprintf( out, " (16-bit CPU address)\n" );

	fprintf( out, "%s  DMA_ch3                     = %s",   prefix, ( jumpers.DMA_ch3 ? jmp_str[1] : jmp_str[0] ) );
	if ( jumpers.DMA_ch3 ) fprintf( out, " (DMA3, DMA is deprecated)\n" );
	else                   fprintf( out, " (DMA1, DMA is deprecated)\n" );
	/*
	*/
	fprintf( out, "%s  CLK_1mhz                    = %s",   prefix, ( jumpers.CLK_1mhz ? jmp_str[1] : jmp_str[0] ) );
	if ( jumpers.CLK_1mhz ) fprintf( out, " (8254  1MHz clock, legacy)\n" );
	else                    fprintf( out, " (8254 10MHz clock, legacy)\n" );

	fprintf( out, "%s  J9_ai_unipolar              = %s",   prefix, ( jumpers.J9_ai_unipolar ? jmp_str[1] : jmp_str[0] ) );
	if ( jumpers.J9_ai_unipolar ) fprintf( out, " (Unipolar inputs)\n" );
	else                          fprintf( out, " (Bipolar inputs)\n" );

	fprintf( out, "%s  J8_ai_differential          = %s",   prefix, ( jumpers.J8_ai_differential ? jmp_str[1] : jmp_str[0] ) );
	if ( jumpers.J8_ai_differential ) fprintf( out, " (Differential inputs)\n" );
	else                              fprintf( out, " (Single-Ended inputs)\n" );

	fprintf( out, "%sMode summary:\n", prefix );
	if      (  jumpers.M1_mode &  jumpers.M0_mode )
	{ /* M1=1 M0=1 */
		fprintf( out, "%s  DAS1602 compatibility and FIFO status\n", prefix );
		fprintf( out, "%s   registers visible (single interrupt \n", prefix );
		fprintf( out, "%s   per ADC-burst completion)\n", prefix);
	}
	else if (  jumpers.M1_mode & (!jumpers.M0_mode) )
	{ /* M1=1 M0=0 */
		fprintf( out, "%s  Enable FIFO Superset Functionality \n", prefix );
	}
	else if ( (!jumpers.M1_mode) &  jumpers.M0_mode )
	{ /* M1=0 M0=1 */
		fprintf( out, "%s  DAS1602 Compatibility w/1MegaSample\n", prefix );
		fprintf( out, "%s   FIFO (single interrupt per sample \n", prefix );
		fprintf( out, "%s   during ADC-burst) \n", prefix );
	}
	else
	{ /* M1=0 M0=0 */
		fprintf( out, "%s  DAS16jr/16 + DAC02 compatibility \n", prefix );
	}

	error_code = STX104_Trg_Active_Get( board, &state );
	fprintf( out, "%sTrigger active                = %s\n", prefix, ( state ? bool_str[1] : bool_str[0] ) );
	error_code = STX104_Int_Enable_Get( board, &state );
	fprintf( out, "%sInterrupt enabled             = %s\n", prefix, ( state ? bool_str[1] : bool_str[0] ) );
/*TODO:   error_code = STX104_Int_Active_Get( board, &state );*/
/*TODO:   fprintf( out, "%sInterrupt active              = %s\n", prefix, ( state ? bool_str[1] : bool_str[0] ) );*/
}

/***************************************************************************/
void STX104_Report_AI_Configration( stx104_ai_cfg_type * ai_cfg, FILE * out, char * prefix, uint16_t hw_rev )
{
	size_t  channel;

	if ( NULL == ai_cfg ) return;
	if ( NULL == out    ) return;

	fprintf( out, "%sfirst channel                 = %d\n", prefix, (int) ai_cfg->first_channel );
	fprintf( out, "%slast channel                  = %d\n", prefix, (int) ai_cfg->last_channel  );

	fprintf( out, "%sgain                          = ", prefix );
	switch( ai_cfg->gain )
	{
		case ACFG_GAIN_10V:   fprintf( out, "  10V " ); break;
		case ACFG_GAIN_5V:    fprintf( out, "   5V " ); break;
		case ACFG_GAIN_2_5V:  fprintf( out, "2.5 V " ); break;
		case ACFG_GAIN_1_25V: fprintf( out, "1.25V " ); break;
	}
	fprintf( out, "\n" );


	fprintf( out, "%sburst_timer_ns (intra-sample) = %lu nS", prefix, (unsigned int) ai_cfg->burst_timer_ns );
	if ( ai_cfg->burst_timer_sec > 0 ) fprintf( out, " (priority: burst_timer_sec)\n" );
	else                               fprintf( out, "\n" );

	fprintf( out, "%sframe_timer_ns                = %lu nS", prefix, (unsigned int) ai_cfg->frame_timer_ns );
	if ( ai_cfg->frame_timer_sec > 0 ) fprintf( out, " (priority: frame_timer_sec)\n" );
	else                               fprintf( out, "\n" );

	fprintf( out, "%sburst_timer_sec (intra-sample)= %e Sec", prefix, ai_cfg->burst_timer_sec );
	if ( 0 == ai_cfg->burst_timer_sec ) fprintf( out, " (used: burst_timer_ns)\n" );
	else                                fprintf( out, "\n" );

	fprintf( out, "%sframe_timer_sec               = %e Sec", prefix, ai_cfg->frame_timer_sec );
	if ( 0 == ai_cfg->frame_timer_sec ) fprintf( out, " (used: frame_timer_ns)\n" );
	else                                fprintf( out, "\n" );

	fprintf( out, "%sframe_maximum                 = %lu\n", prefix, (unsigned int) ai_cfg->frame_maximum  );

	fprintf( out, "%smoving_average_filter         = %s\n", prefix, ( ai_cfg->moving_average_filter      ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%ssync_aift_to_trigger_start    = %s\n", prefix, ( ai_cfg->sync_aift_to_trigger_start ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%ssync_aifc_to_trigger_start    = %s\n", prefix, ( ai_cfg->sync_aifc_to_trigger_start ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%suse_triggering_subsystem      = %s\n", prefix, ( ai_cfg->use_triggering_subsystem   ? bool_str[1] : bool_str[0] ) ); 
	fprintf( out, "%shigh_speed_pre_queue_fifo     = %s\n", prefix, ( ai_cfg->high_speed_pre_queue_fifo  ? bool_str[1] : bool_str[0] ) );

	fprintf( out, "%sstrobe_source                 = ", prefix );
	switch( ai_cfg->strobe_source )
	{
		case AISS_SOFTWARE:				fprintf( out, "AISS_SOFTWARE" );				break;
		case AISS_AI_FRAME_TIMER:		fprintf( out, "AISS_AI_FRAME_TIMER" );			break;
		case AISS_INT_THRESHOLD_COUNTER:fprintf( out, "AISS_INT_THRESHOLD_COUNTER" );	break;
		case AISS_NONE:					fprintf( out, "AISS_NONE" );					break;
		case AISS_DIN0:					fprintf( out, "AISS_DIN0" );					break;
		case AISS_CT_OUT2:				fprintf( out, "AISS_CT_OUT2" );					break;
		case AISS_DIN0_RE:				fprintf( out, "AISS_DIN0_RE" );					break;
		case AISS_DIN0_FE:				fprintf( out, "AISS_DIN0_FE" );					break;
		case AISS_DIN1_RE:				fprintf( out, "AISS_DIN1_RE" );					break;
		case AISS_DIN1_FE:				fprintf( out, "AISS_DIN1_FE" );					break;
		case AISS_DIN2_RE:				fprintf( out, "AISS_DIN2_RE" );					break;
		case AISS_DIN2_FE:				fprintf( out, "AISS_DIN2_FE" );					break;
		case AISS_DIN3_RE:				fprintf( out, "AISS_DIN3_RE" );					break;
		case AISS_DIN3_FE:				fprintf( out, "AISS_DIN3_FE" );					break;
	}
	fprintf( out, "\n" );

	if ( hw_rev < STX104_REVISION_120328 )
	{ /* legacy version - same as A/D data */
		fprintf( out, "%sencoding                      = AI_ENCODING_STRAIGHT_BINARY\n", prefix );
		return;
	}
	/* newer stuff below this line */

	fprintf( out, "%sencoding                      = ", prefix );
	switch( ai_cfg->encoding )
	{
		case AI_ENCODING_STRAIGHT_BINARY:					fprintf( out, "AI_ENCODING_STRAIGHT_BINARY" );					break;
		case AI_ENCODING_STRAIGHT_BINARY_ALLOW_POLARITY:	fprintf( out, "AI_ENCODING_STRAIGHT_BINARY_ALLOW_POLARITY" );	break;
		case AI_ENCODING_TWOS_COMPLEMENT:					fprintf( out, "AI_ENCODING_TWOS_COMPLEMENT" );					break;
		case AI_ENCODING_TWOS_COMPLEMENT_ALLOW_POLARITY: 	fprintf( out, "AI_ENCODING_TWOS_COMPLEMENT_ALLOW_POLARITY" );	break;
		case AI_ENCODING_TWOS_COMPLEMENT_ABSOLUTE:			fprintf( out, "AI_ENCODING_TWOS_COMPLEMENT_ABSOLUTE" );			break;
	}
	fprintf( out, "\n" );

	for ( channel = 0; channel < STX104_AI_QTY; channel++ )
	{
		fprintf( out, "%spolarity[%2d]                  = %s\n", prefix, channel, ( ai_cfg->polarity[channel]  ? bool_str[1] : bool_str[0] ) );
	}
}
/***************************************************************************/
void STX104_Report_DI_Configration( STX104_DI_CFG_TYPE * di_cfg, FILE * out, char * prefix, uint16_t hw_rev )
{
	(void) hw_rev;

	if ( NULL == di_cfg ) return;
	if ( NULL == out    ) return;

	fprintf( out, "%sdeglitch_200ns                = %s\n", prefix, ( di_cfg->deglitch_200ns     ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%sswap_din1_and_din3            = %s\n", prefix, ( di_cfg->swap_din1_and_din3 ? bool_str[1] : bool_str[0] ) );

	fprintf( out, "%spolarity.din0                 = %s\n", prefix, ( di_cfg->polarity.din0      ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%spolarity.din1                 = %s\n", prefix, ( di_cfg->polarity.din1      ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%spolarity.din2                 = %s\n", prefix, ( di_cfg->polarity.din2      ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%spolarity.din3                 = %s\n", prefix, ( di_cfg->polarity.din3      ? bool_str[1] : bool_str[0] ) );
}
/***************************************************************************/
static void STX104_DO_Map_Enum_Helper( size_t channel, size_t map_value, FILE * out )
{
	STX104_DO_ENUM do_chan;

	do_chan = (STX104_DO_ENUM) channel;
	switch( do_chan )
	{
		case  STX104_DO0:
		{
			STX104_DO0_MAP_ENUM do_map = (STX104_DO0_MAP_ENUM) map_value;
			switch( do_map )
			{
			case STX104_DO0_MAP_DOUT0:		fprintf( out, "STX104_DO0_MAP_DOUT0" );	  break;
			case STX104_DO0_MAP_SPI_DO:		fprintf( out, "STX104_DO0_MAP_SPI_DO" );  break;
			case STX104_DO0_MAP_UNUSED2:	fprintf( out, "STX104_DO0_MAP_UNUSED2" ); break;
			case STX104_DO0_MAP_UNUSED3:	fprintf( out, "STX104_DO0_MAP_UNUSED3" ); break;
			}
		}
		break;
		case  STX104_DO1:
		{
			STX104_DO1_MAP_ENUM do_map = (STX104_DO1_MAP_ENUM) map_value;
			switch( do_map )
			{
			case STX104_DO1_MAP_DOUT1:		fprintf( out, "STX104_DO1_MAP_DOUT1" );	  break;
			case STX104_DO1_MAP_SPI_CLK:	fprintf( out, "STX104_DO1_MAP_SPI_CLK" ); break;
			case STX104_DO1_MAP_UNUSED2:	fprintf( out, "STX104_DO1_MAP_UNUSED2" ); break;
			case STX104_DO1_MAP_UNUSED3:	fprintf( out, "STX104_DO1_MAP_UNUSED3" ); break;
			}
		}
		break;
		case  STX104_DO2:
		{
			STX104_DO2_MAP_ENUM do_map = (STX104_DO2_MAP_ENUM) map_value;
			switch( do_map )
			{
			case STX104_DO2_MAP_DOUT2:		fprintf( out, "STX104_DO2_MAP_DOUT2" );	  break;
			case STX104_DO2_MAP_SPI_CS:		fprintf( out, "STX104_DO2_MAP_SPI_CS" );  break;
			case STX104_DO2_MAP_UNUSED2:	fprintf( out, "STX104_DO2_MAP_UNUSED2" ); break;
			case STX104_DO2_MAP_UNUSED3:	fprintf( out, "STX104_DO2_MAP_UNUSED3" ); break;
			}
		}
		break;
		case  STX104_DO3:
		{
			STX104_DO3_MAP_ENUM do_map = (STX104_DO3_MAP_ENUM) map_value;
			switch( do_map )
			{
			case STX104_DO3_MAP_DOUT3:		fprintf( out, "STX104_DO3_MAP_DOUT3" );	  break;
			case STX104_DO3_MAP_UNUSED1:	fprintf( out, "STX104_DO3_MAP_UNUSED1" ); break;
			case STX104_DO3_MAP_UNUSED2:	fprintf( out, "STX104_DO3_MAP_UNUSED2" ); break;
			case STX104_DO3_MAP_UNUSED3:	fprintf( out, "STX104_DO3_MAP_UNUSED3" ); break;
			}
		}
		break;
	}
}

/***************************************************************************/
void STX104_Report_DO_Configration( stx104_do_cfg_type * do_cfg, FILE * out, char * prefix, uint16_t hw_rev )
{
	if ( NULL == do_cfg ) return;
	if ( NULL == out    ) return;

	if ( hw_rev >= STX104_REVISION_120328 )
	{
	fprintf( out, "%sdo0_map                       = ", prefix );
	STX104_DO_Map_Enum_Helper( (size_t) STX104_DO0, (size_t) do_cfg->do0_map, out );
	fprintf( out, "\n" );

	fprintf( out, "%sdo1_map                       = ", prefix );
	STX104_DO_Map_Enum_Helper( (size_t) STX104_DO1, (size_t) do_cfg->do1_map, out );
	fprintf( out, "\n" );

	fprintf( out, "%sdo2_map                       = ", prefix );
	STX104_DO_Map_Enum_Helper( (size_t) STX104_DO2, (size_t) do_cfg->do2_map, out );
	fprintf( out, "\n" );

	fprintf( out, "%sdo3_map                       = ", prefix );
	STX104_DO_Map_Enum_Helper( (size_t) STX104_DO3, (size_t) do_cfg->do3_map, out );
	fprintf( out, "\n" );
	}

	fprintf( out, "%spolarity.dout0                = %s\n", prefix, ( do_cfg->polarity.dout0    ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%spolarity.dout1                = %s\n", prefix, ( do_cfg->polarity.dout1    ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%spolarity.dout2                = %s\n", prefix, ( do_cfg->polarity.dout2    ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%spolarity.dout3                = %s\n", prefix, ( do_cfg->polarity.dout3    ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%spolarity.ct_out0              = %s\n", prefix, ( do_cfg->polarity.ct_out0  ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%spolarity.ct_out2              = %s\n", prefix, ( do_cfg->polarity.ct_out2  ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%spolarity.sshp                 = %s\n", prefix, ( do_cfg->polarity.sshp     ? bool_str[1] : bool_str[0] ) );
}
/***************************************************************************/
void STX104_Report_Triggering_Configration( STX104_TRG_CFG_TYPE * trg_cfg, FILE * out, char * prefix, uint16_t hw_rev )
{
	(void) hw_rev;

	if ( NULL == trg_cfg ) return;
	if ( NULL == out     ) return;	

	fprintf( out, "%senabled                       = %s\n", prefix, ( trg_cfg->enabled      ? bool_str[1] : bool_str[0] ) );

	fprintf( out, "%sstart_mode                    = ", prefix );
	switch( trg_cfg->start_mode )
	{
		case STM_IMMEDIATE:   fprintf( out, "STM_IMMEDIATE" );	break;
		case STM_DELAY:		  fprintf( out, "STM_DELAY" );		break;
		case STM_SYNC_DELAY:  fprintf( out, "STM_SYNC_DELAY" ); break;
	}
	fprintf( out, "\n" );

	fprintf( out, "%sstart_sync_source             = ", prefix );
	switch( trg_cfg->start_sync_source )
	{
		case TSS_NONE:				fprintf( out, "TSS_NONE" );				break;
		case TSS_SOFTWARE_STROBE:	fprintf( out, "TSS_SOFTWARE_STROBE" );	break;
		case TSS_CT_OUT0_RE:		fprintf( out, "TSS_CT_OUT0_RE" );		break;
		case TSS_CT_OUT0_FE:		fprintf( out, "TSS_CT_OUT0_FE" );		break;
		case TSS_CT_OUT2_RE:		fprintf( out, "TSS_CT_OUT2_RE" );		break;
		case TSS_CT_OUT2_FE:		fprintf( out, "TSS_CT_OUT2_FE" );		break;
		case TSS_DIN0_RE:			fprintf( out, "TSS_DIN0_RE" );			break;
		case TSS_DIN0_FE:			fprintf( out, "TSS_DIN0_FE" );			break;
		case TSS_DIN1_RE:			fprintf( out, "TSS_DIN1_RE" );			break;
		case TSS_DIN1_FE:			fprintf( out, "TSS_DIN1_FE" );			break;
		case TSS_DIN2_RE:			fprintf( out, "TSS_DIN2_RE" );			break;
		case TSS_DIN2_FE:			fprintf( out, "TSS_DIN2_FE" );			break;
		case TSS_DIN3_RE:			fprintf( out, "TSS_DIN3_RE" );			break;
		case TSS_DIN3_FE:			fprintf( out, "TSS_DIN3_FE" );			break;
	}
	fprintf( out, "\n" );

	fprintf( out, "%sstop_source                   = ", prefix );
	switch( trg_cfg->stop_source )
	{
		case ETS_NONE:				fprintf( out, "ETS_NONE" );				break;
		case ETS_SOFTWARE_STROBE:	fprintf( out, "ETS_SOFTWARE_STROBE" );	break;
		case ETS_AI_FRAME_MAX:		fprintf( out, "ETS_AI_FRAME_MAX" );		break; 
		case ETS_AI_FRAME_TIMER:	fprintf( out, "ETS_AI_FRAME_TIMER" );	break;
		case ETS_CT_OUT0_RE:		fprintf( out, "ETS_CT_OUT0_RE" );		break;
		case ETS_CT_OUT0_FE:		fprintf( out, "ETS_CT_OUT0_FE" );		break;
		case ETS_CT_OUT2_RE:		fprintf( out, "ETS_CT_OUT2_RE" );		break;
		case ETS_CT_OUT2_FE:		fprintf( out, "ETS_CT_OUT2_FE" );		break;	
		case ETS_DIN0_RE:			fprintf( out, "ETS_DIN0_RE" );			break;	 
		case ETS_DIN0_FE:			fprintf( out, "ETS_DIN0_FE" );			break; 
		case ETS_DIN1_RE:			fprintf( out, "ETS_DIN1_RE" );			break;	 
		case ETS_DIN1_FE:			fprintf( out, "ETS_DIN1_FE" );			break;
		case ETS_DIN2_RE:			fprintf( out, "ETS_DIN2_RE" );			break;
		case ETS_DIN2_FE:			fprintf( out, "ETS_DIN2_FE" );			break;
		case ETS_DIN3_RE:			fprintf( out, "ETS_DIN3_RE" );			break;
		case ETS_DIN3_FE:			fprintf( out, "ETS_DIN3_FE" );			break;
	}		
	fprintf( out, "\n" );

	fprintf( out, "%sstart_source                  = ", prefix );
	switch( trg_cfg->start_source )
	{
		case STS_NONE:				fprintf( out, "STS_NONE" );				break;
		case STS_SOFTWARE_STROBE:	fprintf( out, "STS_SOFTWARE_STROBE" );	break;
		case STS_AI_FRAME_TIMER:	fprintf( out, "STS_AI_FRAME_TIMER" );	break; 
		case STS_CT_OUT0_RE:		fprintf( out, "STS_CT_OUT0_RE" );		break;
		case STS_CT_OUT0_FE:	/*
		 */	fprintf( out, "STS_CT_OUT0_FE" );		break;
		case STS_CT_OUT2_RE:		fprintf( out, "STS_CT_OUT2_RE" );		break;
		case STS_CT_OUT2_FE:		fprintf( out, "STS_CT_OUT2_FE" );		break;
		case STS_DIN0_RE:			fprintf( out, "STS_DIN0_RE" );			break;	
		case STS_DIN0_FE:			fprintf( out, "STS_DIN0_FE" );			break;	 
		case STS_DIN1_RE:			fprintf( out, "STS_DIN1_RE" );			break; 
		case STS_DIN1_FE:			fprintf( out, "STS_DIN1_FE" );			break;	 
		case STS_DIN2_RE:			fprintf( out, "STS_DIN2_RE" );			break;
		case STS_DIN2_FE:			fprintf( out, "STS_DIN2_FE" );			break;
		case STS_DIN3_RE:			fprintf( out, "STS_DIN3_RE" );			break;
		case STS_DIN3_FE:			fprintf( out, "STS_DIN3_FE" );			break;
	}		
	fprintf( out, "\n" );

	fprintf( out, "%sdelay_ns                      = %lu", prefix, (int) trg_cfg->delay_ns  );
	if ( trg_cfg->delay_ns  > 0 ) fprintf( out, " (priority: delay_sec)\n" );
	else                          fprintf( out, "\n" );

	fprintf( out, "%sdelay_sec                     = %e Sec", prefix, trg_cfg->delay_sec );
	if ( 0 == trg_cfg->delay_sec) fprintf( out, " (used: delay_ns)\n" );
	else                          fprintf( out, "\n" );
}

/***************************************************************************/
void STX104_Report_Int_Configuration( STX104_INT_CFG_TYPE * int_cfg, FILE * out, char * prefix, uint16_t hw_rev )
{
	(void) hw_rev;

	if ( NULL == int_cfg ) return;
	if ( NULL == out     ) return;

	fprintf( out, "%sthreshold                     = %lu\n", prefix, (unsigned int) int_cfg->threshold  );

	fprintf( out, "%sthreshold_source              = ", prefix );
	switch( int_cfg->threshold_source )
	{
		case ITS_NONE:				 fprintf( out, "ITS_NONE" );				break;
		case ITS_AI_FRAME:			 fprintf( out, "ITS_AI_FRAME" );			break;
		case ITS_AI_BLOCK:			 fprintf( out, "ITS_AI_BLOCK" );			break;
		case ITS_AI_FRAME_MAX:		 fprintf( out, "ITS_AI_FRAME_MAX" );		break;
		case ITS_TRIGGER_ACTIVATED:	 fprintf( out, "ITS_TRIGGER_ACTIVATED" );	break;
		case ITS_TRIGGER_DEACTIVATED:fprintf( out, "ITS_TRIGGER_DEACTIVATED" );	break;
		case ITS_DIN0_RE:			 fprintf( out, "ITS_DIN0_RE" );				break;
		case ITS_DIN0_FE:			 fprintf( out, "ITS_DIN0_FE" );				break;
		case ITS_DIN1_RE:			 fprintf( out, "ITS_DIN1_RE" );				break;
		case ITS_DIN1_FE:			 fprintf( out, "ITS_DIN1_FE" );				break;
		case ITS_DIN2_RE:			 fprintf( out, "ITS_DIN2_RE" );				break;
		case ITS_DIN2_FE:			 fprintf( out, "ITS_DIN2_FE" );				break;
		case ITS_DIN3_RE:			 fprintf( out, "ITS_DIN3_RE" );				break;
		case ITS_DIN3_FE:			 fprintf( out, "ITS_DIN3_FE" );				break;
	}
	fprintf( out, "\n" );

	fprintf( out, "%sblocks_per_interrupt          = ", prefix );
	switch( int_cfg->blocks_per_interrupt )
	{
		case BLOCK_2:		fprintf( out, "BLOCK_2" );		break;
		case BLOCK_4:		fprintf( out, "BLOCK_4" );		break;
		case BLOCK_8:		fprintf( out, "BLOCK_8" );		break;
		case BLOCK_16:		fprintf( out, "BLOCK_16" );		break;
		case BLOCK_32:		fprintf( out, "BLOCK_32" );		break;
		case BLOCK_64:		fprintf( out, "BLOCK_64" );		break;
		case BLOCK_128:		fprintf( out, "BLOCK_128" );	break;
		case BLOCK_256:		fprintf( out, "BLOCK_256" );	break;
		case BLOCK_512:		fprintf( out, "BLOCK_512" );	break;
		case BLOCK_1024:	fprintf( out, "BLOCK_1024" );	break;
		case BLOCK_2048:	fprintf( out, "BLOCK_2048" );	break;
		case BLOCK_0_0625:	fprintf( out, "BLOCK_0_0625" );	break;
		case BLOCK_0_125:	fprintf( out, "BLOCK_0_125" );	break;
		case BLOCK_0_25:	fprintf( out, "BLOCK_0_25" );	break;
		case BLOCK_0_5:		fprintf( out, "BLOCK_0_5" );	break;
		case BLOCK_1:		fprintf( out, "BLOCK_1" );		break;
	}
	fprintf( out, "\n" );

	fprintf( out, "%ssource_a                      = ", prefix );
	switch( int_cfg->source_a )
	{
		case ISA_NONE:				 fprintf( out, "ISA_NONE" );				break;
		case ISA_THRESHOLD_COUNTER:	 fprintf( out, "ISA_THRESHOLD_COUNTER" );	break;
		case ISA_AI_FRAME_MAX:		 fprintf( out, "ISA_AI_FRAME_MAX" );		break;
		case ISA_TRIGGER_ACTIVATED:	 fprintf( out, "ISA_TRIGGER_ACTIVATED" );	break;
		case ISA_TRIGGER_DEACTIVATED:fprintf( out, "ISA_TRIGGER_DEACTIVATED" );	break;
		case ISA_DIN0_RE:			 fprintf( out, "ISA_DIN0_RE" );				break;
		case ISA_DIN0_FE:			 fprintf( out, "ISA_DIN0_FE" );				break;
		case ISA_DIN1_RE:			 fprintf( out, "ISA_DIN1_RE" );				break;
		case ISA_DIN1_FE:			 fprintf( out, "ISA_DIN1_FE" );				break;
		case ISA_DIN2_RE:			 fprintf( out, "ISA_DIN2_RE" );				break;
		case ISA_DIN2_FE:			 fprintf( out, "ISA_DIN2_FE" );				break;
		case ISA_DIN3_RE:			 fprintf( out, "ISA_DIN3_RE" );				break;
		case ISA_DIN3_FE:			 fprintf( out, "ISA_DIN3_FE" );				break;
	}
	fprintf( out, "\n" );

	fprintf( out, "%ssource_b                      = ", prefix );
	switch( int_cfg->source_b )
	{
		case ISB_NONE:				 fprintf( out, "ISB_NONE" );				break;
		case ISB_THRESHOLD_COUNTER:	 fprintf( out, "ISB_THRESHOLD_COUNTER" );	break;
		case ISB_AI_FRAME_MAX:		 fprintf( out, "ISB_AI_FRAME_MAX" );		break;
		case ISB_TRIGGER_ACTIVATED:	 fprintf( out, "ISB_TRIGGER_ACTIVATED" );	break;
		case ISB_TRIGGER_DEACTIVATED:fprintf( out, "ISB_TRIGGER_DEACTIVATED" );	break;
		case ISB_DIN0_RE:			 fprintf( out, "ISB_DIN0_RE" );				break;
		case ISB_DIN0_FE:			 fprintf( out, "ISB_DIN0_FE" );				break;
		case ISB_DIN1_RE:			 fprintf( out, "ISB_DIN1_RE" );				break;
		case ISB_DIN1_FE:			 fprintf( out, "ISB_DIN1_FE" );				break;
		case ISB_DIN2_RE:			 fprintf( out, "ISB_DIN2_RE" );				break;
		case ISB_DIN2_FE:			 fprintf( out, "ISB_DIN2_FE" );				break;
		case ISB_DIN3_RE:			 fprintf( out, "ISB_DIN3_RE" );				break;
		case ISB_DIN3_FE:			 fprintf( out, "ISB_DIN3_FE" );				break;
	}
	fprintf( out, "\n" );

	fprintf( out, "%scallback_source_a              = 0x%p", prefix, int_cfg->callback_source_a );
	if ( NULL == int_cfg->callback_source_a ) fprintf( out, " (null)\n" );
	else                     				  fprintf( out, "\n" );

	fprintf( out, "%scallback_source_b              = 0x%p", prefix, int_cfg->callback_source_b );
	if ( NULL == int_cfg->callback_source_b ) fprintf( out, " (null)\n" );
	else                                      fprintf( out, "\n" );

	fprintf( out, "%scallback_fifo_blk              = 0x%p", prefix, int_cfg->callback_fifo_blk );
	if ( NULL == int_cfg->callback_fifo_blk ) fprintf( out, " (null)\n" );
	else                                      fprintf( out, "\n" );

	fprintf( out, "%scallback_source_a_user_dataset	= 0x%p", prefix, int_cfg->callback_source_a_user_dataset );
	if ( NULL == int_cfg->callback_source_a_user_dataset ) fprintf( out, " (null)\n" );
	else                                                   fprintf( out, "\n" );

	fprintf( out, "%scallback_source_b_user_dataset	= 0x%p", prefix, int_cfg->callback_source_b_user_dataset );
	if ( NULL == int_cfg->callback_source_b_user_dataset ) fprintf( out, " (null)\n" );
	else                     							   fprintf( out, "\n" );

	fprintf( out, "%scallback_fifo_blk_user_dataset	= 0x%p", prefix, int_cfg->callback_fifo_blk_user_dataset );
	if ( NULL == int_cfg->callback_fifo_blk_user_dataset ) fprintf( out, " (null)\n" );
	else                                                   fprintf( out, "\n" );

	fprintf( out, "%sthreshold_sync_to_triggering   = %s\n", prefix, ( int_cfg->threshold_sync_to_triggering    ? bool_str[1] : bool_str[0] ) );
}

/***************************************************************************/
void STX104_Report_SPI_Configuration( STX104_SPI_CFG_TYPE * spi_cfg, FILE * out, char * prefix, uint16_t hw_rev )
{
	if ( NULL == spi_cfg ) return;
	if ( NULL == out     ) return;

	if ( hw_rev < STX104_REVISION_120328 ) return;

	fprintf( out, "%shalf_clock_ns                 = %lu nS\n", prefix, (int) spi_cfg->half_clock_ns );
	
	fprintf( out, "%ssdi_map                       = ", prefix );
	switch( spi_cfg->sdi_map )
	{
		case SPI_SDI_DIN0:	fprintf( out, "SPI_SDI_DIN0" );		break;
		case SPI_SDI_DIN1:	fprintf( out, "SPI_SDI_DIN1" );		break;
		case SPI_SDI_DIN2:	fprintf( out, "SPI_SDI_DIN2" );		break; 
		case SPI_SDI_DIN3:	fprintf( out, "SPI_SDI_DIN3" );		break;
	}		
	fprintf( out, "\n" );
	
	fprintf( out, "%send_cycle_delay_ns            = %lu nS\n", prefix, (int) spi_cfg->end_cycle_delay_ns );
	fprintf( out, "%ssdio_wrap                     = %s\n", prefix, ( spi_cfg->sdio_wrap       ? bool_str[1] : bool_str[0] ) );

	fprintf( out, "%schip_select_behavior          = ", prefix );
	switch( spi_cfg->chip_select_behavior )
	{
		case SPI_CSB_SOFTWARE:	fprintf( out, "SPI_CSB_SOFTWARE" );		break;
		case SPI_CSB_FIFO:		fprintf( out, "SPI_CSB_FIFO" );			break;
		case SPI_CSB_TOGGLE_8:	fprintf( out, "SPI_CSB_TOGGLE_8" );		break; 
		case SPI_CSB_TOGGLE_16:	fprintf( out, "SPI_CSB_TOGGLE_16" );	break;
	}		
	fprintf( out, "\n" );

	fprintf( out, "%ssdo_polarity                  = %s\n", prefix, ( spi_cfg->sdo_polarity    ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%ssdi_polarity                  = %s\n", prefix, ( spi_cfg->sdi_polarity    ? bool_str[1] : bool_str[0] ) );
	fprintf( out, "%ssclk_phase                    = %s\n", prefix, ( spi_cfg->sclk_phase      ? bool_str[1] : bool_str[0] ) );	
	fprintf( out, "%ssclk_polarity                 = %s\n", prefix, ( spi_cfg->sclk_polarity   ? bool_str[1] : bool_str[0] ) );
}

/***************************************************************************/
void STX104_Report_Full( int board, FILE * out_file )
{
	stx104_ai_cfg_type  ai_cfg;
	STX104_DI_CFG_TYPE  di_cfg;
	stx104_do_cfg_type 	do_cfg;
	STX104_TRG_CFG_TYPE trg_cfg;
	STX104_INT_CFG_TYPE int_cfg;
	STX104_SPI_CFG_TYPE spi_cfg;
	int 				error_code;
	uint16_t 			hw_rev;
	char 				prefix[] = "  ";
	time_t				time_current;

	fprintf( out_file, "\n\n" );

	error_code = STX104_Gen_Revision_Get( board, &hw_rev );

	/* date and time */
	fprintf( out_file, "======================================= REPORT FILE\n" );
	time_current = time( NULL );
	fprintf( out_file, "Date:   %s", ctime( &time_current ) );
	/* system status  */
	fprintf( out_file, "Status: stx104 board level:\n" );
	STX104_Report_Status( board, out_file, prefix );

	/* analog inputs */
	fprintf( out_file, "Configuration: analog inputs:\n" );
	error_code = STX104_AI_CFG_Get( board, &ai_cfg ); /* get configuration from board */
	if ( error_code ) return;

	STX104_Report_AI_Configration( &ai_cfg, out_file, prefix, hw_rev );

	/* digital inputs */
	fprintf( out_file, "Configuration: digital inputs:\n" );
	error_code = STX104_DI_CFG_Get( board, &di_cfg );
	if ( error_code ) return;
	STX104_Report_DI_Configration( &di_cfg, out_file, prefix, hw_rev );

	/* digital outputs */
	fprintf( out_file, "Configuration: digital outputs:\n" );
	error_code = STX104_DO_CFG_Get( board, &do_cfg );
	if ( error_code ) return;
	STX104_Report_DO_Configration( &do_cfg, out_file, prefix, hw_rev );

	/* triggering - uncomm		case STS_NONE:				fprintf( out, "STS_NONE" );				break;
		case STS_SOFTWARE_STROBE:	fprintf( out, "STS_SOFTWARE_STROBE" );	break;
		case STS_AI_FRAME_TIMER:	fprintf( out, "STS_AI_FRAME_TIMER" );	break;
		case STS_CT_OUT0_RE:		fprintf( out, "STS_CT_OUT0_RE" );		break;
		case STS_CT_OUT0_FE:		fprintf( out, "STS_CT_OUT0_FE" );		break;
		case STS_CT_OUT2_RE:		fprintf( out, "STS_CT_OUT2_RE" );		break;
		case STS_CT_OUT2_FE:		fprintf( out, "STS_CT_OUT2_FE" );		break;
		case STS_DIN0_RE:			fprintf( out, "STS_DIN0_RE" );			break;
		case STS_DIN0_FE:			fprintf( out, "STS_DIN0_FE" );			break;
		case STS_DIN1_RE:			fprintf( out, "STS_DIN1_RE" );			break;
		case STS_DIN1_FE:			fprintf( out, "STS_DIN1_FE" );			break;
		case STS_DIN2_RE:			fprintf( out, "STS_DIN2_RE" );			break;
		case STS_DIN2_FE:			fprintf( out, "STS_DIN2_FE" );			break;
		case STS_DIN3_RE:			fprintf( out, "STS_DIN3_RE" );			break;
		case STS_DIN3_FE:			fprintf( out, "STS_DIN3_FE" );			break;ent and use as you need  */
	fprintf( out_file, "Configuration: triggering subsystem:\n" );
	error_code = STX104_Trg_CFG_Get( board, &trg_cfg );
	if ( error_code ) return;
	STX104_Report_Triggering_Configration( &trg_cfg, out_file, prefix, hw_rev );

	/* events or interrupts */
	fprintf( out_file, "Configuration: interrupts:\n" );
	error_code = STX104_Int_CFG_Get( board, &int_cfg );
	if ( error_code ) return;
	STX104_Report_Int_Configuration( &int_cfg, out_file, prefix, hw_rev );


	/* SPI - depends on firmware version */
	if ( hw_rev >= STX104_REVISION_120328 )
	{
		fprintf( out_file, "Configuration: SPI I/O:\n" );
		error_code = STX104_SPI_CFG_Get( board, &spi_cfg );
		if ( error_code ) return;
		STX104_Report_SPI_Configuration( &spi_cfg, out_file, prefix, hw_rev );
	}

	fprintf( out_file, "\n\n" );
}

/***************************************************************************/

