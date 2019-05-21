/********************************************************************************
_____________________________________________________________________________  
Description:

_____________________________________________________________________________
Purpose:

The intent of this source file is as a test bed for evaluation of the library
and driver.  It is more for the purposes of Apex internal testing.  However, 
we keep this with the files as sometimes it may illustrate helpful examples.
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


*********************************************************************************/
/*http://stackoverflow.com/questions/2119539/visual-studio-how-to-set-path-to-dll?rq=1    */
/*http://stackoverflow.com/questions/57776/how-to-add-existing-item-an-entire-directory-structure-in-visual-studio?rq=1  */


/*TODO: not sure how to reference DLL in its compiled directory? */
#include "../aul/aul_public.h"
#include <stdio.h>
#include <memory.h>
#include "aul_device_logical_enum.h" /* auto-generated running lib_report */
/*#include "../shared/cmd.h"*/


/*****************************************************************************/
/*#include "aul_integrity.c"    */
extern aul_integrity_type aul_integrity_list[];

const char * main_acronym[3] = { "stx104", "tracere", "buger" };

/*****************************************************************************/

/*****************************************************************************/
int Main_Tracer_DIO_Test( int board )
{ /* brackets are intentional in order to create more variables */
	int						error_code;
	int						channel;
	uint8_t					value_port;
	TRACERE_LED_ENUM		led;
	BOOL					bit;
	unsigned long			argument;
	tracere_dio_cfg_type	dio_cfg;
	const char *		    pin_name;

	error_code = Tracere_DIO_Config_Initialize( &dio_cfg );
	dio_cfg.i8255m0_cfg[TRACERE_COMPONENT_I8255M0_0].port_directions = I8255M0_PDIR_AOUT_BOUT_COUT;
	dio_cfg.i8255m0_cfg[TRACERE_COMPONENT_I8255M0_1].port_directions = I8255M0_PDIR_AOUT_BOUT_COUT;

	error_code = Tracere_DIO_Config_Set( board, &dio_cfg );
	if ( error_code ) 	printf( "Tracere_DIO_Config_Set error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );

	/* Commit the configuration out to hardware, and initialize the outputs. */
	error_code = Tracere_Restart( board, TRACERE_RESTART_COLD_DIO ); 
	if ( error_code ) 	printf( "Tracere_Restart error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );

	/* write 0x55 to port #1 (tracere register at offset location 0x1 or port B) */
	value_port = 0x55;
	error_code = Tracere_DIO_Port_Set( board, 1 /* port number */, value_port );
	if ( error_code ) 	printf( "Tracere_DIO_Port_Set error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );

	/* Write 0xAA to port #2 (tracere register at offset location 0x2 or port C) 
	   This illustrates another way to write to the Tracer DIO registers.
	*/
	value_port = 0xAA;
	argument = (unsigned long) &value_port;
	error_code = AUL_Ioctl( board, CMD_TRACERE_DIO_PORT_SET | 2, argument );
	if ( error_code ) 	printf( "AUL_Ioctl error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );

	/* read port #1 (tracere register at offset 0x1 ) */
	error_code = Tracere_DIO_Port_Get( board, 1 /* port number */, &value_port );
	if ( error_code ) 	printf( "Tracere_DIO_Port_Get error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );
	else				printf( "DIO_Port[1] => 0x%02X\n", (int) value_port ); 

	/* write bit #25 (tracere register at offset 0x4 at bit A1 ) */
	bit = true;
	channel = 25;
	Tracere_DIO_Bit_Connector_Pin_Name_Get( channel, &pin_name );
	error_code = Tracere_DIO_Bit_Set( board, channel /* bit */, bit );
	if ( error_code ) 	printf( "Tracere_DIO_Bit_Set error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );
	else				printf( "DIO[25] at %s <= %d\n", pin_name, (int) bit );

	error_code = Tracere_DIO_Bit_Get( board, 24 /* bit */, &bit );
	if ( error_code ) 	printf( "Tracere_DIO_Bit_Get error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );
	else				printf( "DIO[24] => %d\n", (int) bit );

	/* set bit #47 (tracere register at offset 0x7) */
	bit = true;
	argument = (unsigned long) &bit;
	error_code = AUL_Ioctl( board, CMD_TRACERE_DIO_BIT_SET | 47 /* bit */, argument );
	if ( error_code ) 	printf( "AUL_Ioctl error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );
	else				printf( "DIO[47] <= %d\n", (int) bit );

	argument = (unsigned long) &bit;
	error_code = AUL_Ioctl( board, CMD_TRACERE_DIO_BIT_GET | 46, argument );
	if ( error_code ) 	printf( "AUL_Ioctl error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );
	else				printf( "DIO[46] => %d\n", (int) bit );

	/* set LED */
	error_code = Tracere_LED_Set( board, TRACERE_LED_ON );
	if ( error_code ) 	printf( "Tracere_LED_Set error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );

	led = TRACERE_LED_BASE_ADDRESS;
	error_code = AUL_Ioctl( board, CMD_TRACERE_LED_SET, (unsigned long) &led );
	if ( error_code ) 	printf( "AUL_Ioctl error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );

	return error_code;
}
/*****************************************************************************/
int Main_I8254_Test( int board )
{
#define MAIN_I8254_XFR_ARRAY_SIZE  4
	int						error_code;
	DEVICE_LOGICAL_ENUM		logical_id;
	size_t					index;
	i8254_ct_xfr_array_type cxa;
	i8254_ct_xfr_type		cxfr[MAIN_I8254_XFR_ARRAY_SIZE];

	error_code = SUCCESS;

	memset( cxfr, 0, sizeof( i8254_ct_xfr_type ) * MAIN_I8254_XFR_ARRAY_SIZE );

	error_code = AUL_Device_Logical_Get( main_acronym[0], &logical_id );
	if ( DEVICE_LOGICAL_STX104 == logical_id )
	{

	}


	printf( " I8254 Single transaction entry:\n" );
	/* perform a single counter/timer transaction */
	cxfr[0].type = SYS_TYPE_MAKE( i8254_ct_xfr_type );

	cxfr[0].count_write = 0x1234;
	I8254_Control_Create( I8254_CHANNEL_0,
						  I8254_RW_LSB_MSB,
						  I8254_MODE_SQUARE_WAVE_GENERATOR,
						  I8254_TYPE_BINARY,
						  &(cxfr[0].control)
					   );
	/* error_code = AUL_Ioctl( board, CMD_I8254_CT_XFR, (unsigned long)( &(cxfr[0]) ) ); */
	error_code = AUL_I8254_Transfer( board, &(cxfr[0]) ); 
	if ( error_code ) return error_code;



	printf( " I8254 Single transaction exit:\n" );


	printf( " I8254 Multiple transaction entry:\n" );
	cxa.type = SYS_TYPE_MAKE( i8254_ct_xfr_array_type );
	cxa.qty  = MAIN_I8254_XFR_ARRAY_SIZE;
	cxa.cxfr = cxfr;  /* tie the array list */

	/* read counter value at CT 0 and CT1 -- Note: these two
		transactions will be merged at the i8254 into one 
		control word which will become:  b11010110.
	*/
	cxfr[0].type = SYS_TYPE_MAKE( i8254_ct_xfr_type );
	I8254_Control_Create(	I8254_CHANNEL_READBACK,
							I8254_RW_STA,
							I8254_MODE_READBACK_CHAN_0,
							I8254_TYPE_BINARY, /* readback ignores this */
							&(cxfr[0].control)
						);
	cxfr[1].type = cxfr[0].type;
	I8254_Control_Create(	I8254_CHANNEL_READBACK,
							I8254_RW_STA,
							I8254_MODE_READBACK_CHAN_1,
							I8254_TYPE_BINARY, /* readback ignores this */
							&(cxfr[1].control)
						);
	/* write value to CT 0 */
	cxfr[2].type = cxfr[0].type;
	cxfr[2].count_write = 0x0132;
	I8254_Control_Create(	I8254_CHANNEL_0,
							I8254_RW_LSB_MSB,
							I8254_MODE_SQUARE_WAVE_GENERATOR,
							I8254_TYPE_BINARY,
							&(cxfr[2].control)
						);
	/* readback the status and counts of all counter/timers in i8254 */
	cxfr[3].type = cxfr[0].type;
	I8254_Control_Create(	I8254_CHANNEL_READBACK,
							I8254_RW_CNT_STA,
							I8254_MODE_READBACK_CHAN_ALL,
							I8254_TYPE_BINARY, /* readback ignores this */
							&(cxfr[3].control)
						);
	/* The first transaction encountering an error will cause an error to be returned
		and all subsequent transactions to not run.  In other words, transactions will
		run in order up to the first error encountered.
	*/
/*	error_code = AUL_Ioctl( board, CMD_I8254_CT_XFR_ARRAY, (unsigned long)( &cxa ) );  */
	error_code = AUL_I8254_Transfer_Array( board, &cxa );

	for ( index = 0; index < MAIN_I8254_XFR_ARRAY_SIZE; index++ )
	{
		if ( cxfr[index].error_code )
		{ 
			error_code = cxfr[index].error_code;
			printf( "  I8254 transaction (index=%d) failed: error_code = %d, %s.\n", index, cxfr[index].error_code, AUL_Error_String( error_code ) );
		}
	}

	if ( error_code )
	{
		printf( "  Readback all results: error has occurred, not available\n" );
	}
	else
	{
		printf( "  Readback all results:\n" );
		for ( index = 0; index < I8254_CT_QTY; index++ )
		{
		printf( "    ct[%d]: status = 0x%02X, count = 0x%04X\n", index, (int)(cxfr[3].status_read[index]), (int)(cxfr[3].count_read[index]) );
		}
	}
	printf( " I8254 Multiple transaction exit:\n" );
		
	return error_code;
#undef MAIN_I8254_XFR_ARRAY_SIZE
}
/*****************************************************************************/
int Main_Tracer_CT_Test( int board )
{
	int					error_code;
	int					chan;
	tracere_ct_cfg_type ct_cfg;
	i8254_ct_xfr_type	cxfr;
	int					clk_src;
	double				ct_hz[I8254_CT_QTY];
	double				actual[I8254_CT_QTY];
	double				error_actual[I8254_CT_QTY];

	I8254_MODE_ENUM		mode;

	error_code = Tracere_CT_Config_Initialize( &ct_cfg );

	/* set source information to i8254 */
	ct_cfg.ct0.clock_invert = false;
	ct_cfg.ct0.gate_invert  = false;
	ct_cfg.ct0.gate_source  = TRACERE_GATE_CT0_CT_GATE0;
	ct_cfg.ct0.clock_source = TRACERE_CLK_CT0_4_MHZ;
	ct_hz[0] = 4321.0; /* Hz */

	ct_cfg.ct1.clock_invert = false;
	ct_cfg.ct1.gate_invert  = false;
	ct_cfg.ct1.clock_source = TRACERE_CLK_CT1_4_MHZ;
	ct_hz[1] = 3333.0; /* Hz */

	ct_cfg.ct2.clock_invert = false;
	ct_cfg.ct2.gate_invert  = false;
	ct_cfg.ct2.clock_source = TRACERE_CLK_CT2_4_MHZ;
	ct_hz[2] = 10000.0; /* Hz */

	{
		const char * clock_name;
		Tracere_CT_CLock_Name_Get( &ct_cfg, 0, &clock_name );
		printf( "ct_cfg.ct0.clock_source = %s\n", clock_name );
		Tracere_CT_CLock_Name_Get( &ct_cfg, 1, &clock_name );
		printf( "ct_cfg.ct1.clock_source = %s\n", clock_name );
		Tracere_CT_CLock_Name_Get( &ct_cfg, 2, &clock_name );
		printf( "ct_cfg.ct2.clock_source = %s\n", clock_name );
	}

	/* setup i8254 */
	for ( chan = 0; chan < I8254_CT_QTY; chan++ )
	{
		switch( chan )
		{
			case 0: clk_src = ct_cfg.ct0.clock_source;	break;
			case 1:	clk_src = ct_cfg.ct1.clock_source;	break;
			case 2:	clk_src = ct_cfg.ct2.clock_source;	break;
		}
		I8254_Control_Create(	(I8254_CHANNEL_ENUM) chan,
								I8254_RW_LSB_MSB,
								I8254_MODE_SQUARE_WAVE_GENERATOR,
								I8254_TYPE_BINARY,
								&(ct_cfg.i8254_cfg.cxfr[chan].control )
							);
		if ( SUCCESS != Tracere_I8254_Do_Freq_Hz( ct_cfg.i8254_cfg.cxfr[chan].control ) )
			printf( "Oops!, the mode selected doesn't make sense to compute frequency!\n" );
		error_code = Tracere_I8254_Count_Freq_Hz(  ct_hz[chan],
												0.0 /* Hz */,      /* not used */
												clk_src,
												chan,
												ct_cfg.i8254_cfg.cxfr[chan].control,
												&(ct_cfg.i8254_cfg.cxfr[chan].count_write),
												&(actual[chan]),
												&(error_actual[chan])
											 );
		if ( error_code ) goto Main_Tracer_CT_Test_Done;
		/* multiply the error by 1.0e6 to obtain parts per million (ppm) */
		printf( "CT[%d]: request = %8.2f Hz, actual = %8.2f Hz, error = %8.2f ppm\n", chan, ct_hz[chan], actual[chan], error_actual[chan] * 1.0e6  );
	}
	/* send config to lib/driver */
	/*error_code = Tracere_CT_Config_Get( board, &ct_cfg );*/
	error_code = Tracere_CT_Config_Set( board, &ct_cfg );
	if ( error_code ) goto Main_Tracer_CT_Test_Done;  /* config error */
	/* commit configuration to hardware */
	error_code = Tracere_Restart( board, TRACERE_RESTART_COLD_CT );
	if ( error_code ) goto Main_Tracer_CT_Test_Done;

/* lets change a counter #1 to a different frequency */
	memset( &cxfr, 0, sizeof( i8254_ct_xfr_type ) );	/* clear the transfer */
	cxfr.type = SYS_TYPE_MAKE( i8254_ct_xfr_type );
	chan = 1;  /* channel #1 */
	/* lets get the status of this counter to auto-populate the
	   cxfr structure and then we can just modify it.
	   1. Need to determine mode read-back channel.  In other words,
	      the control/command has a different bit pattern and the
	      channel information falls into the normal mode region.
	   2. Create a control command to the i8254 to read the status.
	   3. Get the status, which ends up as the current control word,
	       for the counter, well we replace the upper two bits SC[1:0]
	       back to channel bits rather than output and null count bits.
	   4. Using the same control word (which was status) compute our
	       new count value to be written into the counter.
	   5. Submit to counter.
	*/
	switch( chan )
	{ /* yes this part is weird, but just the way the hardware works */
		case 0: mode = I8254_MODE_READBACK_CHAN_0;	break;
		case 1: mode = I8254_MODE_READBACK_CHAN_1;	break;
		case 2: mode = I8254_MODE_READBACK_CHAN_2;	break;
	}
	I8254_Control_Create(	I8254_CHANNEL_READBACK, /* readback command */
							I8254_RW_STA,  /* STA = status */
							mode,		   /* represents readback channel */
							I8254_TYPE_BINARY,	/* ignored */
							&(cxfr.control )
						);
	error_code = Tracere_I8254_Transfer( board, &cxfr );  /* get status */
	{ /* set a breakpoint and observe cxfr.status_read[chan] and
	     the Extractor will bust apart the status (well, most of it).  */
		I8254_MODE_ENUM test_mode;
		I8254_RW_ENUM   test_rw;
		I8254_TYPE_ENUM test_bcd;
		Tracere_I8254_Control_Extract( (I8254_CHANNEL_ENUM) chan, &test_rw, &test_mode, &test_bcd, cxfr.status_read[chan] );
		printf( "I8254 Chan %d status = 0x%02X\n", chan, cxfr.status_read[chan] );
	}
	ct_hz[chan] = 7500.0; /* Hz */
	cxfr.control = cxfr.status_read[chan];  /* move control to out-bound control word */
	if ( SUCCESS != Tracere_I8254_Do_Freq_Hz( cxfr.control ) )
		printf( "Oops!, the mode selected doesn't make sense to compute frequency!\n" );

	error_code = Tracere_I8254_Count_Freq_Hz( ct_hz[chan],
											  0.0 /* Hz */,      /* not used */
											  clk_src,
											  chan,
											  cxfr.control,
											  &(cxfr.count_write),
											  &(actual[chan]),
											  &(error_actual[chan])
										    );
	/* multiply the error by 1.0e6 to obtain parts per million (ppm) */
	printf( "CT[%d]: request = %8.2f Hz, actual = %8.2f Hz, error = %8.2f ppm\n", chan, ct_hz[chan], actual[chan], error_actual[chan] * 1.0e6 );
	error_code = Tracere_I8254_Transfer( board, &cxfr );  /* change the frequency */
	/* Another way to do the same thing in the previous line, this time
	 * we will change the frequency to 3200Hz so you can observe the change.
	 * Many library functions can also be called in this way, laying the ground
	 * work for migration to kernel driver as required.
	 */
	ct_hz[chan] = 3200.0; /* Hz */
	if ( SUCCESS != Tracere_I8254_Do_Freq_Hz( cxfr.control ) )
		printf( "Oops!, the mode selected doesn't make sense to compute frequency!\n" );
	error_code = Tracere_I8254_Count_Freq_Hz( ct_hz[chan],
											  0.0 /* Hz */,      /* not used */
											  clk_src,
											  chan,
											  cxfr.control,
											  &(cxfr.count_write),
											  &(actual[chan]),
											  &(error_actual[chan])
										    );
	/* multiply the error by 1.0e6 to obtain parts per million (ppm) */
	printf( "CT[%d]: request = %8.2f Hz, actual = %8.2f Hz, error = %8.2f ppm\n", chan, ct_hz[chan], actual[chan], error_actual[chan] * 1.0e6 ); 
	error_code = AUL_Ioctl( board, CMD_I8254_CT_XFR, (unsigned long) &cxfr );
Main_Tracer_CT_Test_Done:
	return error_code;
}
/*****************************************************************************/
int Main_STX104_Test_Register_Direct( int board )
{
	int			error_code;
	io_xfr_type xfr;

/* TODO:
 * This is way too complex and it does not provide good error prevention.
 * And, I do not like the REG_ID_ prefix as it just adds confusion.
 *  error_code = AUL_Register_Write( board, <register>, buf, buf_len );
 *  error_code = AUL_Register_Read( board, <register>, buf, buf_len );
 *
 */

	memset( &xfr, 0, sizeof( xfr ) );
	xfr.command = IO_XFR_NONE;
	xfr.data.u8 = 0x5A;
	xfr.reg_id  = REG_ID_STX104_CHANNEL;
	error_code  = AUL_Ioctl( board, CMD_STX104_REG_WRITE, (unsigned long) &xfr );
	if ( error_code ) goto Main_STX104_Test_Register_Direct_Error;

	{
		uint16_t buf[4] = { 0x1234, 0x3456, 0x5678, 0x789A };
		memset( &xfr, 0, sizeof( xfr ) );
		xfr.command = IO_XFR_NONE;
		xfr.data.buf = (char *) buf;
		xfr.buf_len = 4 * sizeof( uint16_t);
		xfr.reg_id  = REG_ID_STX104_SCRATCH_PAD;
		error_code  = AUL_Ioctl( board, CMD_STX104_REG_WRITE, (unsigned long) &xfr );
	}
	if ( error_code ) goto Main_STX104_Test_Register_Direct_Error;

	{
		uint32_t buf[4] = { 0x12345678, 0x89ABCDEF, 0x3456789A, 0x789ABCDE };
		memset( &xfr, 0, sizeof( xfr ) );
		xfr.command = IO_XFR_NONE;
		xfr.data.buf = (char *) buf;
		xfr.buf_len = 4 * sizeof( uint32_t);
		xfr.reg_id  = REG_ID_STX104_ANALOG_INPUT_FRAME_TIMER;
		error_code  = AUL_Ioctl( board, CMD_STX104_REG_WRITE, (unsigned long) &xfr );
	}
	if ( error_code ) goto Main_STX104_Test_Register_Direct_Error;

Main_STX104_Test_Register_Direct_Error:
	return error_code; 
}
/*****************************************************************************/
int Main_STX104_Test_Config( int board )
{
	int error_code;
	stx104_ai_cfg_type config;
	IO_ID_ENUM io_id;

	error_code = AUL_Ioctl( board, CMD_IO_ID, (unsigned long) &io_id ); 
	if ( ( IO_ID_WIN32_LIB_SIMULATOR == io_id ) || ( IO_ID_LINUX_LIB_SIMULATOR == io_id ) )
	{
		error_code = STX104_AI_Config_Initialize( &config );
		return error_code;
	}
	else
	{
		error_code = STX104_AI_CFG_Get( board, &config );
	}
/* set breakpoint here and inspect returned configuration */
	return error_code;
}
/*****************************************************************************/
int Main_STX104_Test_Other( int board )
{
	int					error_code;
	stx104_do_cfg_type	do_cfg;
	io_xfr_type			xfr;
	IOC_T				ioctl_command;
	unsigned long revision;
	BOOL				bool_val;

	{
		uint16_t actual;
		uint16_t buf[16];
		error_code = STX104_AI_Frames_Get( board, buf, &actual, 16 );
		if ( error_code ) goto Main_STX104_Test_Other_Error;
	}

	error_code = STX104_AO_Set( board, STX104_AO_A, 0x1234 );
	if ( error_code ) goto Main_STX104_Test_Other_Error;


	ioctl_command = CMD_IO_VERSION;
	error_code = AUL_Ioctl( board,
							ioctl_command,
							(unsigned long)( (void *) &revision )
						  );
	if ( error_code ) goto Main_STX104_Test_Other_Error;
	printf( "IO Revision:  Rev_Hex = 0x%08X,  Rev_Dec = %lu\n", (unsigned int) revision, revision );

	memset( &xfr, 0, sizeof( xfr ) );
	xfr.command	   = IO_XFR_WRITE_U8;
	xfr.offset     = 0x303;
	xfr.data.u8    = 0x5A;
	ioctl_command		  = CMD_IO_XFR;
	error_code = AUL_Ioctl( board,
							ioctl_command,
							(unsigned long)( (void *) &xfr )
						  );
	if ( error_code ) goto Main_STX104_Test_Other_Error;

	memset( &xfr, 0, sizeof( xfr ) );
	xfr.command	    = IO_XFR_WRITE_U8;
	xfr.offset      = 0x31F;  /* 0x302 makes an error. Good! */
	xfr.data.u8     = 0x5A;
	ioctl_command		  = CMD_IO_XFR;
	error_code = AUL_Ioctl( board,
							ioctl_command,
							(unsigned long)( (void *) &xfr )
						  );
/*	if ( error_code ) goto Main_STX104_Test_Other_Error;*/


	error_code = STX104_AO_Set( board, STX104_AO_B, 0x5678 );
	if ( error_code ) goto Main_STX104_Test_Other_Error;

	memset( &do_cfg, 0, sizeof( do_cfg ) );
	do_cfg.polarity.dout0 = 1;
	do_cfg.polarity.dout1 = 1;
	do_cfg.polarity.dout2 = 1;
	do_cfg.polarity.dout3 = 1;
	error_code = STX104_DO_CFG_Set( board, &do_cfg );
	if ( error_code ) goto Main_STX104_Test_Other_Error;

	error_code = STX104_DO_Set( board, STX104_DO1, 1 );
	if ( error_code ) goto Main_STX104_Test_Other_Error;

	error_code = STX104_DI_Get( board, STX104_DI1, &bool_val );
	if ( error_code ) goto Main_STX104_Test_Other_Error;

	error_code = STX104_Trg_Software_Start( board );
	if ( error_code ) goto Main_STX104_Test_Other_Error;

Main_STX104_Test_Other_Error:
	return error_code;
}


/*****************************************************************************/

/*****************************************************************************/


/*****************************************************************************/
/**
 *
 * @param argc
 * @param argv
 * @param envp
 * @return
 */
int main(int argc, char *argv[], char *envp[])
{
	int error_code;
	int board[3] = { 0 };
/*	int value;*/
/*	unsigned long revision;*/
/*	unsigned int command;*/
/*	unsigned long argument;*/
/*	stx104_do_cfg_type do_cfg; */
/*	io_xfr_type xfr;  */
/*	IOC_T		  ioctl_command; */
	unsigned long revision;
	unsigned long process_id;
	device_list_type  device_list;
	DEVICE_LOGICAL_ENUM logical_id[3];
/*	BOOL   bool_val;  */
	(void) argc;
	(void) argv;
	(void) envp;


	printf( "Library name = %s\n", AUL_Library_Name() );
	error_code = AUL_Library_Process_Id_Get( &process_id );
	printf( "Library process ID = %u\n", process_id );


	/*-------- BEGIN:  INTEGRITY TEST  -------- */
		error_code = ACGR_Integrity_Test( aul_integrity_list );
		if ( error_code ) goto Main_Error;
	/*-------- END:   AUTO-CODE GENERATION  -------- */


/*	error_code = AUL_Initialization( "aul_hwdef.txt" );*/
#ifdef _WIN32
	error_code = AUL_Initialization( "aul_hwdef.txt" );
#else
	error_code = AUL_Initialization( NULL );
#endif
	if ( error_code ) goto Main_Error;

	printf( "Driver name = %s\n", AUL_Driver_Name() );

	error_code = AUL_Library_Revision( &revision );
	if ( error_code ) goto Main_Error;
	printf( "Library:  Rev_Hex = 0x%08X,  Rev_Dec = %lu\n", (unsigned int) revision, revision );

	error_code = AUL_Driver_Revision( &revision );
	if ( error_code ) goto Main_Error;
	printf( "Driver:  Rev_Hex = 0x%08X,  Rev_Dec = %lu\n", (unsigned int) revision, revision );

	printf( "Library name = %s\n", AUL_Library_Name() );
	printf( "Driver name  = %s\n", AUL_Driver_Name() );

	error_code = AUL_Device_List_Get( &device_list );
	if ( error_code ) goto Main_Error;

	error_code = AUL_Device_Logical_Get( main_acronym[0],  &(logical_id[0]) );
	if ( error_code ) printf( "AUL_Device_Logical_Get() error: %s for device %s.\n", AUL_Error_String( error_code ), main_acronym[0] );

	error_code = AUL_Device_Logical_Get( main_acronym[1],  &(logical_id[1]) );
	if ( error_code ) printf( "AUL_Device_Logical_Get() error: %s for device %s.\n", AUL_Error_String( error_code ), main_acronym[1] );

	error_code = AUL_Device_Logical_Get( main_acronym[2],  &(logical_id[2]) );
	if ( error_code ) printf( "AUL_Device_Logical_Get() error: %s %s\n", AUL_Error_String( error_code ),  main_acronym[2] );

	error_code = AUL_Device_Open( "tracere", &(board[1]) );
	if ( error_code ) goto Main_Error;

	error_code = Main_Tracer_DIO_Test( board[1] );
	/* if ( error_code ) goto Main_Error; */

	error_code = Main_I8254_Test( board[1] );
	/* if ( error_code ) goto Main_Error; */

	error_code = Main_Tracer_CT_Test( board[1] );
	/* if ( error_code ) goto Main_Error; */

	error_code = AUL_Device_Close( board[1] );


/* goto Main_Close; */





	error_code = AUL_Device_Open( "stx104", &(board[0]) );
	if ( error_code ) goto Main_Error;

#if(0)
	{ /* cannot work, need to write through a component...not what I want! */
		uint32_t x;
		error_code = AUL_Ioctl( board[0], CMD_INDIRECT_READ, (unsigned long)( &x ) );
		if (error_code ) goto Main_Error;
		printf( "x=0x%08X\n", x );
	}
#endif



	error_code = Main_STX104_Test_Register_Direct( board[0] );
	if ( error_code ) goto Main_Error;

	error_code = Main_STX104_Test_Config( board[0] );
	if ( error_code ) goto Main_Error;

	error_code = Main_I8254_Test( board[0] );

	error_code = Main_STX104_Test_Other( board[0] );
	if ( error_code ) goto Main_Error;




/*TODO:	error_code = STX104_Connectivity_Report( board[0], char * buf, size_t buf_size );
TODO:	if ( error_code ) goto Main_Error;*/


/* Main_Close: */
/* Not required, AUL_Termination() will clean up the mess:	error_code = AUL_Device_Close( board[1] );  */
/* Not required, AUL_Termination() will clean up the mess:	error_code = AUL_Device_Close( board[0] );  */
	if ( error_code ) goto Main_Error;
	error_code = AUL_Termination();
	printf( "Application Test: exit success\n" );
	return SUCCESS;
Main_Error:
	if ( error_code < 0 ) error_code = -error_code;

/* TODO: shared library and DLL: Status_EC_String is not an exported function! */
	printf( "Application Test: exit fail, error_code = %d, %s.\n", error_code, AUL_Error_String( error_code ) );

	error_code = AUL_Termination();

	return error_code;
}

