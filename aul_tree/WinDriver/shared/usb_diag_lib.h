/* Jungo Confidential. Copyright (c) 2011 Jungo Ltd.  http://www.jungo.com */

/**********************************************************************
 * File - USB_DIAG_LIB.H
 *
 * Library for USB diagnostics and samples, using WinDriver functions.
 **********************************************************************/

#ifndef _USB_DIAG_LIB_H_
#define _USB_DIAG_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "windrvr.h"
#include "diag_lib.h"

enum {MAX_BUFFER_SIZE = 4096};

/* Function: pipeType2Str()
     Returns a string identifying the pipe type
*/
char *pipeType2Str(ULONG pipeType);

/* Function: PrintPipesInfo()
     Prints the pipes information for the specified alternate setting
*/
void PrintPipesInfo(WDU_ALTERNATE_SETTING *pAltSet);

/* Function: PrintPipe0Info()
     Prints control pipe (pipe 0) information
*/
void PrintPipe0Info(WDU_DEVICE *pDevice);

/* Function: PrintDevicePipesInfoByHandle()
     Prints the pipes information for all the active device pipes
*/
void PrintDevicePipesInfoByHandle(HANDLE hDevice);

/* Function: PrintDeviceConfigurations()
     Prints the device's configurations information
*/
void PrintDeviceConfigurations(HANDLE hDevice);

/* Function: ReadWritePipesMenu()
     Displays a menu to read/write from/to the device's pipes
*/
void ReadWritePipesMenu(HANDLE hDevice);

/* Function: SelectiveSuspendMenu()
   Displays a menu to enter/exit Selective Suspend mode
*/
void SelectiveSuspendMenu(WDU_DEVICE_HANDLE hDevice);

/* The following are defined for backwards compatibility with v6.2x and below */
#define PrintHexBuffer(pBuffer, dwBytes) \
    DIAG_PrintHexBuffer(pBuffer, dwBytes, TRUE)
#define GetHexBuffer(pBuffer, dwBytes) \
    DIAG_GetHexBuffer(pBuffer, dwBytes)
    
#ifdef __cplusplus
}
#endif

#endif
