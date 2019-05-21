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


*********************************************************************************/

/**********************************************************************//**
 * @file  lib_report.c
 * Reports all parameters of library/driver
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
 * Demonstration of obtaining reports and enumerations from the AES Library.
 *
 * @page page_contact   Contact
 * Apex Embedded Systems<br>
 * email: customer.service@apexembeddedsystems.com<br>
 * voice: 608-256-0767<br>
 *
 *************************************************************************/

 /**********************************************************************//**
 * Copyright (c) 2013-2015 by Apex Embedded Systems LLC.
 * All rights reserved. This document is the confidential property
 * of Apex Embedded Systems Any reproduction or dissemination is
 * prohibited unless specifically authorized in writing.
 *************************************************************************/

/* printf() */
#include <stdio.h>

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
 * @def LIB_REPORT_REV
 * Subversion time stamp.
 */
#define LIB_REPORT_REV  "$Date: 2014-05-06 11:47:33 -0500 (Tue, 06 May 2014) $"


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
 * This demonstration various reporting and auto-code generation mechanisms
 * supported by the AES Library.  The code generated by this demo can be used
 * with user applications.
 *
 **/
int main(int argc, char *argv[], char *envp[])
{
	/*********************************************************************//**
	 * <HR>
	 * <b>Main Variables</b>
	 *
	 * <i>int error_code</i><br>
	 * Variable used to report error codes from the library function calls.<br>
	 *
	 * <i>unsigned long revision</i><br>
	 * Variable used to hold the library revision.
	 *
	 * All other variables are not used.
	 */
	int error_code;
	unsigned long revision;
	(void) argc;
	(void) argv;
	(void) envp;

	/*********************************************************************//**
	 * <HR>
	 * <b>AUL_Library_Name()</b>
	 *
	 * DESCRIPTION:<br>
	 * Report the AUL library name.  This is a library function call.<br>
	 **/
	printf( "Library name = %s\n", AUL_Library_Name() );
	error_code = AUL_Library_Revision( &revision );
	if ( error_code ) goto Main_Error;
	printf( "Library:  Rev_Hex = 0x%08X,  Rev_Dec = %lu\n", (unsigned int) revision, revision );

	/*********************************************************************//**
	 * <HR>
	 * <b>AUL_Initialization()</b>
	 *
	 * DESCRIPTION:<br>
	 * We need to do this so that all of the underlying modules can populate
	 * their complete names and/or provide the proper serialized date/time
	 * summary stamps.<br>
	 *
	 * POSSIBLE ERRORS:<br>
	 * If an error reported is "No such file or directory", this indicates that the
	 * mod_auldrv kernel driver has not been installed properly.
	 **/
	AUL_Initialization( NULL );

	/*********************************************************************//**
	 * <HR>
	 * <b>ACGR_Report_CSV_Module()</b>
	 *
	 * DESCRIPTION:<br>
	 * Produces a human readable summary list of all modules within the
	 * library space.  Format is a comma delimited file which can be opened
	 * in any text editor, Calc or Excel.<br>
	 *
	 * USAGE:<br>
	 * For informational purposes at this time.
	 **/
	error_code = ACGR_Report_CSV_Module( TRUE, "aul_module_list.csv" );
	if ( error_code ) goto Main_Error;

	/*********************************************************************//**
	 * <HR>
	 * <b>ACGR_Report_CSV_Command()</b>
	 *
	 * DESCRIPTION:<br>
	 * Produces a human readable summary list of all the error codes within the
	 * library and driver space.  Format is a comma delimited file which can be
	 * opened in any text editor, Calc or Excel.<br>
	 *
	 * USAGE:<br>
	 * For informational purposes at this time.  Can be used used by other applications
	 * where needed at run-time should dynamic modules be inserted into the library.
	 **/
	error_code = ACGR_Report_CSV_Command( TRUE, "aul_command_list.csv" );
	if ( error_code ) goto Main_Error;

	/*********************************************************************//**
	 * <HR>
	 * <b>ACGR_Report_CSV_Status()</b>
	 *
	 * DESCRIPTION:<br>
	 * Produces a human readable summary list of all the error codes within the
	 * library and driver space.  Format is a comma delimited file which can be
	 * opened in any text editor, Calc or Excel.<br>
	 *
	 * USAGE:<br>
	 * For informational purposes at this time.  Can be used used by other applications
	 * where needed at run-time should dynamic modules be inserted into the library.
	 **/
	error_code = ACGR_Report_CSV_Status( TRUE, "aul_status_list.csv" );
	if ( error_code ) goto Main_Error;

	/*********************************************************************//**
	 * <HR>
	 * <b>ACGR_Source_File_Command_Enumeration()</b>
	 *
	 * DESCRIPTION:<br>
	 * Produces an enumeration of all IOCTL commands that are used in the library and
	 * driver space.  In this case we are producing a text file that is a
	 * header file which can be used within a user application; the only down
	 * side to using this is that it ought to be produced each time the library
	 * is updated (just to be sure).  This enumeration is really best used as a
	 * reference to get a feel for all of the possible commands.<br>
	 *
	 * USAGE:<br>
	 * For informational purposes and/or embedded into user applications.
	 **/
	error_code = ACGR_Source_File_Command_Enumeration( "aul_command_enum.h" );
	if ( error_code ) goto Main_Error;

	/*********************************************************************//**
	 * <HR>
	 * <b>ACGR_Source_File_Status_Enumeration()</b>
	 *
	 * DESCRIPTION:<br>
	 * Produces an enumeration of all error/status codes that are used in the
	 * library and driver space.  In this case we are producing a text file that
	 * is a header file which can be used within a user application; the only down
	 * side to using this is that it ought to be produced each time the library
	 * is updated (just to be sure).  This enumeration is really best used as a
	 * reference to get a feel for all of the possible commands.<br>
	 *
	 * USAGE:<br>
	 * For informational purposes and/or embedded into user applications.
	 **/
	error_code = ACGR_Source_File_Status_Enumeration( "aul_status_enum.h" );
	if ( error_code ) goto Main_Error;

	/*********************************************************************//**
	 * <HR>
	 * <b>ACGR_Source_File_Status_Enumeration()</b>
	 *
	 * DESCRIPTION:<br>
	 * Produces an enumeration of all the logical device types that are
	 * available.  In this case we are producing a text file that is a
	 * header file which can be used within a user application; the only
	 * down side to using this is that it ought to be produced each time the
	 * library is updated (just to be sure).  This enumeration can be used
	 * to configure application code specific to the target device.<br>
	 *
	 * Using the library function AUL_Device_Logical_Get() and the
	 * device acronym name (usually defined using insmod) one can
	 * obtain the logical_id of the device.  This can then be matched
	 * up against the DEVICE_LOGICAL_ENUMeration to determine exactly
	 * what kind of device it is.<br>
	 *
	 * USAGE:<br>
	 * For informational purposes and/or embedded into user applications.
	 **/
	error_code = ACGR_Source_File_Device_Logical_Enumeration( "aul_device_logical_enum.h" );
	if ( error_code ) goto Main_Error;

	/*********************************************************************//**
	 * <HR>
	 * <b>ACGR_Source_File_Status_Enumeration()</b>
	 *
	 * DESCRIPTION:<br>
	 * Produces a summary data structure of the serialized date codes of
	 * the modules used within the library,  The purpose is for both reporting
	 * and verification of application with library.  In other words, an application
	 * may require certain library release (or release range) of either all or
	 * particular modules.  You can think of this as a finger print of a particular
	 * library release and your application may have been written for that release.
	 * Now, say, a much newer release comes along and there have been major changes
	 * to status codes, well, that will show itself here and your application can
	 * warn users (or perhaps stop the application) indicating that there is a
	 * library revision conflict with the application.  This avoids problems down
	 * the road when everyone forgets all the details about the system.<br>
	 *
	 * Note that this integrity test is in its early stages of development.<br>
	 *
	 * USAGE:<br>
	 * For informational purposes and/or embedded into user applications.  The
	 * current intent is that one would compare a previously created "aul_integrity.c"
	 * with a new one generated.  Why?  This provides a way to verify that all
	 * library and driver revision information matches a well tested and accepted
	 * set of code.  It offers a way to detect mix-ups in software updates and
	 * provides a mechanism to root out which revision is out of sync. with the
	 * qualified set of revisions that have been accepted for production release.
	 **/
	error_code = ACGR_Source_File_Integrity( "aul_integrity.c" );
	if ( error_code ) goto Main_Error;

	/*********************************************************************//**
	 * <HR>
	 * <b>AUL_Termination()</b>
	 *
	 * DESCRIPTION:<br>
	 * Terminates the driver.  This 'closes' the controller portion of the driver.
	 * In other words, we are closing the /dev/auldrv from the kernel perspective.<br>
	 *
	 **/
	AUL_Termination();

	/***************************************************************************/
	printf( "library reports completed successfully\n" );
	return SUCCESS;
Main_Error:
	printf( "library reports error: error_code = %d, %s\n", error_code, AUL_Error_String( error_code ) );
	return error_code;
}

