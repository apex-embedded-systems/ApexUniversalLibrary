
#include "../aul/aul.h"
#include <stdio.h>

#include "../shared/cmd.h"

//http://stackoverflow.com/questions/57776/how-to-add-existing-item-an-entire-directory-structure-in-visual-studio?rq=1

/*****************************************************************************/
int main(int argc, char *argv[], char *envp[])
{
	int error_code;
	int board[2];
	int value;
//	unsigned long revision;
//	unsigned int command;
//	unsigned long argument;
	stx104_do_cfg_type do_cfg;


	error_code = AUL_Initialization( "aul_hwdef.txt" );
	if ( error_code ) goto Main_Error;

//	error_code = IO_Version( &revision );
	error_code = AUL_Device_Open( "mySTX104", &(board[0]) );
	if ( error_code ) goto Main_Error;

	error_code = STX104_AO_Set( board[0], STX104_AO_A, 0x1234 );
	if ( error_code ) goto Main_Close;

	error_code = STX104_AO_Set( board[0], STX104_AO_B, 0x5678 );
	if ( error_code ) goto Main_Close;

	Mem_Set( &do_cfg, 0, sizeof( do_cfg ) );
	do_cfg.polarity.dout0 = 1;
	do_cfg.polarity.dout1 = 1;
	do_cfg.polarity.dout2 = 1;
	do_cfg.polarity.dout3 = 1;
	error_code = STX104_DO_CFG_Set( board[0], &do_cfg );
	if ( error_code ) goto Main_Close;

	error_code = STX104_DO_Set( board[0], STX104_DO1, 1 );
	if ( error_code ) goto Main_Close;

	error_code = STX104_DI_Get( board[0], STX104_DI1, &value );
	if ( error_code ) goto Main_Close;

	error_code = STX104_Trg_Software_Start( board[0] );
	if ( error_code ) goto Main_Close;

#if(0)
	error_code = AUL_Device_Open( "myTracer", &(board[0]) );
	if ( error_code ) goto Main_Error;

	/* example of calling a device command */
	argument = 0;
	command = CMD_TRACERE_NOP;
	error_code = AUL_Ioctl( board[0], command, argument );

	/* example of calling a component within device */
	argument = 0;
	command = CMD_I8255M0_POWER_UP; /* assumes instance = 0 */ 
	command = IOC_SET_INSTANCE( command, 0 );
	error_code = AUL_Ioctl( board[0], command, argument );

	argument = 0;
	command = CMD_I8255M0_POWER_UP; /* assumes instance = 0 */ 
	command = IOC_SET_INSTANCE( command, 1 );
	error_code = AUL_Ioctl( board[0], command, argument );

	/* force a command down to driver level 
		example of calling a device command 
	*/
	argument = 0;
	command = CMD_TRACERE_NOP;
	command = IOC_SET_DRV( command );
	error_code = AUL_Ioctl( board[0], command, argument );

#endif
//	error_code = AUL_Device_Open( "myTracer", &(board[1]) );
//	if ( error_code ) 
//	{
//		AUL_Device_Close( board[0] );
//		goto Main_Error;
//	}

//	error_code = AUL_Device_Close( board[1] );
//	if ( error_code ) goto Main_Error;

Main_Close:
	error_code = AUL_Device_Close( board[0] );
	if ( error_code ) goto Main_Error;

	//IO_Close( device );
	//Module_Terminate();

	error_code = AUL_Termination();
	if ( error_code ) goto Main_Error;


	return error_code;

Main_Error:
	return error_code;
}