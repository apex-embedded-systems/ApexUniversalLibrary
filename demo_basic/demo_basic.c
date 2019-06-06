/**********************************************************************//**
 * @file  demo_basic.c
 * A simple application example
 *
 * @mainpage Demo Basic
 * See main() for details.
 *
 * @subpage page_contact
 *
 * @subpage page_summary
 *
 * @subpage page_aul_ref
 *
 * @page page_summary  Summary
 * The most basic application demonstration of communicating with the Apex Universal Library.
 * Illustrates initialization, device open, device close and termination.
 *
 * @page page_contact   Contact
 * Apex Embedded Systems<br>
 * email: customer.service@apexembeddedsystems.com<br>
 * voice: 608-256-0767<br>
 *
 *************************************************************************/

 /**********************************************************************//**
 * Copyright (c) 2008-2015 by Apex Embedded Systems LLC.
 * All rights reserved. This document is the confidential property
 * of Apex Embedded Systems Any reproduction or dissemination is
 * prohibited unless specifically authorized in writing.
 *************************************************************************/

#include <stdio.h>   /* required to use printf() */

/**********************************************************************//**
 * @page page_aul_ref Apex Universal Library Include Reference
 * <b>#include "../aul_tree/aul/aul_public.h"</b>
 *
 * DESCRIPTION:<br>
 * A single include for all applications which access the Apex Universal Library.<br>
 *
 * Includes all of the top level Apex Universal Library Interfaces:<br>
 * 1. Function prototypes (or APIs) of the form AUL_<function name>(),
 *    STX104_<function_name>(), Tracere_<function_name>() and so on.
 * 2. References to all library data structure definitions.
 * 3. References to all status or error code definitions for both driver,
 *    and library levels.
 * 4. References to all Ioctl() commands for both the library and driver.
 *
 * USAGE:<br>
 * Required for any application using the Apex Universal library and driver.<br>
 */
#include "../aul_tree/aul/aul_public.h"

/**********************************************************************//**
 * @def DEMO_BASIC_REV
 * Subversion time stamp.
 */
#define DEMO_BASIC_REV  "$Date: 2015-01-15 06:04:48 -0600 (Thu, 15 Jan 2015) $"


/*************************************************************************//**
 * <b>main()</b><br>
 *
 * Demonstrates simple interactions with the library and driver.
 *
 * ASSUMPTIONS:<br>
 * 1. The library (aul_tree) has been previously compiled to a static
 *    library.  Run make in "/aul_eclipse/aul_tree/_Out_Library_Shared_Application_Debug").
 * 2. AUL driver has been successfully compiled and installed. Please
 *    refer to the readme.txt file in either "/aul_eclipse/drv_aul_x86" or
 *    "/aul_eclipse/drv_aul_sim".
 *
 * DESCRIPTION:<br>
 * This demonstration illustrates library initialization, device open,
 * device close and library termination.  It also illustrates how to
 * print out the name of the library and driver.
 *
 **/
int main( int argc, char* argv[] )
{
  /*********************************************************************//**
   * <HR>
   * <b>Main Variables</b>
   *
   * <i>int error_code</i><br>
   * Variable used to report error codes from the library function calls.<br>
   *
   * <i>int board</i><br>
   * Variable used to hold the device descriptor.  This descriptor is used
   * for all subsequent library calls that will communicate with the device.<br>
   */
  int error_code;
  int board;
  
  /*********************************************************************//**
   * <HR>
   * <b>AUL_Library_Name()</b>
   *
   * DESCRIPTION:<br>
   * Report the AUL library name.  This is a library function call.<br>
   *
   * How does this application know which library to use?   It is defined
   * in Eclipse under "Project-->Properties-->C/C++ Build-->Settings-->GCC Linker-->Libraries".
   * Left click on the project in the Eclipse Project Explorer and then right-click and
   * go down to "Properties...".  Note that the library will be different between
   * Debug and Release builds.<br>
   *
   * This application uses the statically built library.  So, either "auldebug.a"
   * or "aul.a" will be reported by the AUL_Library_Name() function.  The
   * "auldebug.a" is the library that contains additional debug features.  The
   * library (in this case the application) will append debug information to a
   * "dmesg.txt" file located in the same folder as the application.<br>
   *
   * The "dmesg.txt" file contains very detailed information regarding library
   * behavior.  If at any time you run into an issue, please be prepared to send
   * the dmesg.txt file to Apex for further analysis.  The dmesg.txt file can be
   * deleted at any time; debug features within the library will automatically
   * create the file and continue to append to the file.<br>
   *
   * In terms of the kernel driver, the kernel driver appends its debug information
   * to the kernel debug ring buffer.  Retrieve the information by using the Linux
   * command "dmesg".<br>
   *
   * USAGE:<br>
   * Optional.  Not required to be used in any application.  Its purpose
   * is more to support debug.
   **/
  printf( "Library name = %s\n", AUL_Library_Name() );
  
  /*********************************************************************//**
   * <HR>
   * <b>AUL_Initialization()</b>
   *
   * DESCRIPTION:<br>
   * Initialize the driver.  This 'opens' the controller portion of the driver.
   * In other words, we are opening the /dev/auldrv from the kernel perspective.
   * In addition, this is an opportunity for the driver and library to exchange
   * house keeping information.<br>
   *
   * This function is a library function call that will open the /dev/auldrv
   * kernel device.  The "/dev/auldrv" device is made available upon completion
   * of the driver installation; in other words, successful insmod command.<br>
   *
   * PARAMETERS<br>
   * Hardware is completely defined during installation of the Linux
   * driver and therefore we simply pass a NULL.<br>
   *
   * USAGE:<br>
   * Required.  This is required by all applications which use the AUL library.<br>
   *
   * POSSIBLE ERRORS:<br>
   * If an error reported is "No such file or directory", this indicates that the
   * mod_auldrv kernel driver has not been installed properly.
   **/
  error_code = AUL_Initialization( NULL );
  if ( error_code ) 
  { /* Library failed to initialize, report the error and exit application. */
    printf( "AUL_Initialization error: %s\n", AUL_Error_String( error_code ) );
    return -error_code;
  }
  printf( "AUL_Initialization successful\n" );
  
  /*********************************************************************//**
   * <HR>
   * <b>AUL_Driver_Name()</b>
   *
   * DESCRIPTION:<br>
   * Report the AUL driver name.  This is a library function call.<br>
   *
   * The actual string name is received from the driver itself.  Within the
   * driver this string is hard-coded.  The suffix portion of the driver
   * name originates from the Makefile of the driver (NAME := auldrv).  In other
   * words, the name of the driver will always be "mod_"<suffix name>.  The
   * name of the driver can also be found using the lsmod Linux command.<br>
   *
   * USAGE:<br>
   * Optional.  Not required to be used in any application.  Its purpose
   * is more to support debug.
   **/
  printf( "Driver name = %s\n", AUL_Driver_Name() );

  /*********************************************************************//**
   * <HR>
   * <b>AUL_Device_Open()</b>
   * 
   * DESCRIPTION:<br>
   * Open the device, in this case "stx104".  The string name must match the
   * Linux driver (kernel module) insmod acronym="stx104".   If the insmod
   * acronym was for example:  acronym="mystx104", then we would open using
   * a string called "mystx104" rather than "stx104".<br>
   *
   * This function is a library call which eventually opens the device /dev/stx104.
   * In addition there is a whole bunch of housekeeping operations that are
   * performed within this library call.  This function helps to keep application
   * code short and concise.  The 'board' parameter is a descriptor that must
   * be passed to other library functions so that it knows which board you
   * are referring to.<br>
   *
   * USAGE:<br>
   * Required only if you want to communicate with a hardware device (board).
   * It is possible to make library function calls to non-related device
   * function calls without opening a device.<br>
   **/
//Uncomment the next line to induce a "definition not found" device open error.
//#define TEST_DEVICE_OPEN_ERROR 1

#if TEST_DEVICE_OPEN_ERROR
  error_code = AUL_Device_Open( "device_unknown",  /* some string that is not found in driver device names (acronyms) */
		  	  	  	  	  	  	&board
		  	  	  	  	  	  );
#else

  if ( argc > 1 )
  { /* use a name passed in from command line. */
	  printf( "Attempting to open device \"%s\"\n", argv[1] );
	  error_code = AUL_Device_Open( argv[1], &board );
  }
  else
  { /* use a default board name */
	  error_code = AUL_Device_Open( "stx104", &board );
  }
#endif
  if ( error_code ) 
  { /* Device failed to open, report the error, library terminate and exit application. */
    printf( "AUL_Device_Open error: %s\n", AUL_Error_String( error_code ) );
    /* Either one of the next two lines will work; which one you use depends
     * on your application and how you wisht to handle this situation.  One
     * could instead use atexit()/exit() strategies too.
     */
    goto   Main_Terminate;
    //Choosing to not use this line:  AUL_Termination();
    //Choosing to not use this line:  return -error_code;
  } 
  printf( "AUL_Device_Open successful\n" );
  
  /*************************************************************************
   * Code that interacts with the device would normally reside here.
   **/


  /*********************************************************************//**
   * <HR>
   * <b>AUL_Device_Close()</b>
   *
   * DESCRIPTION:<br>
   * Close the device.  In this specific case the previously opened "stx104" device
   * is closed.<br>
   * 
   * This function is a library call that is made which eventually closes
   * the /dev/stx104.  In addition, this function does a variety of house keeping
   * operations.<br>
   *
   * USAGE<br>
   * Strongly suggested that it be paired with a previously successful AUL_Device_Open().
   * Note:  AUL_Termination() will also force the closure of any opened devices.<br>
   **/
  AUL_Device_Close( board );
  printf( "AUL_Device_Close successful\n" );
  
  /*********************************************************************//**
   * <HR>
   * <b>AUL_Termination()</b>
   *
   * DESCRIPTION:<br>
   * Terminates the driver.  This 'closes' the controller portion of the driver.
   * In other words, we are closing the /dev/auldrv from the kernel perspective.<br>
   * 
   * There are other house keeping activities that occur during this call as it 
   * works its way through the library and driver.<br>
   * 
   * This function is a library call.<br>
   *
   * USAGE:<br>
   * Required in that it must be paired with a previously successful AUL_Initialization().
   * If the previous AUL_Initialization() failed, then this function need not be called.
   **/
Main_Terminate:
  error_code = AUL_Termination();
  if ( error_code ) 
  {
    printf( "AUL_Termination error %s\n", AUL_Error_String( error_code ) );
    return -error_code;
  }
  printf( "AUL_Termination successful\n" );
  
  
  return error_code;
}
/* FILE END */

