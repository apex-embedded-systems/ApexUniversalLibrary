/*
 * general_test.c
 *
 *  Created on: Jun 11, 2014
 *      Author: mike
 */

/*  Make this part of the generic I/O functions.  If the device supports the command.

	if the device supports the command then it will run the command otherwise it will
	report an error.


	OK, we can add commands, but part of the issue will be that the jump tables for
	the devices will begin to grow to be quite large, potentially.  The upside is
	that it will all run fast, the down side is a lot more memory.

	if we made a list of pairs ( command, function ), then a search would be
	required.

	We could make infrequent commands at the top / end of the table so that
	most all other tables could be kept small.  The device manager does check
	to see that the command is within the range of the jump table.



*/
AUL_DLL_API int AULCALLCONV AUL_DIO_Port_U8_Get( int board, int channel, uint8_t * value )
{
	int						error_code;
	devmgr_device_type *	device;
	if ( SUCCESS != ( error_code = AUL_Board( board, &device ) ) ) goto AUL_DIO_Port_Get_Error;

	error_code = AUL_Ioctl( board, CMD_DEV_DIO_PORT_U8_GET | channel, (unsigned long) value );
	if ( !error_code ) goto AUL_DIO_Port_Get_Error;

AUL_DIO_Port_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( DEVICE_TRACERE_NAME, __func__, __LINE__, error_code );
	return -error_code;
}


/*

0. interrupts

DONE: 0a. demo_stx104_simple

1. Need a SPI bus interface.  First one would be STX104.  I think I have already done
 this in an older version somewhere.

2. Probably ought to implement index register support as well so that we can
 implement the i8254 for STX104.

3. Need Summit

4. Need component model or template.
 Need to finalize (as best as possible) the structures of the components
 and devices so that the next ones can be implemented quickly.


5. Devices.  Separation of LIB functions from all other stuff.

 dev_<name>_lib.h
 dev_<name>_lib.c

 dev_<name>_shared.h   <-- for all data structures and things that are used
                           at all levels.

  shared then gets referenced by both dev_<name>.h and dev_<name>_lib.h

  The dev_<name>_lib.h is then used in dev_avail_lib.h which is then
  referenced in aul_public.h

  Since dev_<name>_lib.h or .c will not be referenced in the driver builds
  this will work automatically.

6. Need indexing or indirect addressing component.

7. thinking that a large portion of functions within the Tracer device can migrate
to the I8255 and I8254 which then would see reuse in other devices.  In some cases
the component could be called directly, given physical_id,


8. Need UDP bus component.  If connected to components via cmp->link_rw
   then it is automatically forwarded.

   Alternatively, simply create a new io_<os>_lib_net.c.  This might be
   a better approach as it would intercept ALL things.

 */
