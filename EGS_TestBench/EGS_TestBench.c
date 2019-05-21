/**********************************************************************//**
 * @file  egs_testbench.c
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
#define EGS_TESTBENCH_REV  "$Date: 2015-04-17 19:13:42 -0500 (Fri, 17 Apr 2015) $"


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
  
  printf( "Library name = %s\n", AUL_Library_Name() );
  
  error_code = AUL_Initialization( NULL );
  if ( error_code ) 
  { /* Library failed to initialize, report the error and exit application. */
    printf( "AUL_Initialization error: %s\n", AUL_Error_String( error_code ) );
    return -error_code;
  }
  printf( "AUL_Initialization successful\n" );
  printf( "Driver name = %s\n", AUL_Driver_Name() );


  error_code = AUL_Device_Open( "stx104egs", &board );
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
  
  AUL_Device_Close( board );
  printf( "AUL_Device_Close successful\n" );
  
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

