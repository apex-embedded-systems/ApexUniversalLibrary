/*#############################################################################
	 < < <  TEMPERATURE   > > >
###############################################################################

-------------------------------------------------------------------------------
Copyright (c) 1997-2003 Apex Embeddded Systems.  All rights reserved.
This document is the confidential property of Apex Embedded Systems.
Any reproduction or dissemination is prohibited unless specifically
authorized in writing.
-------------------------------------------------------------------------------

Written by Mike Ihm

Date: May 7, 2003

Description:

############################################################################ */

#ifndef  __TEMPERATURE_HPP__
#define  __TEMPERATURE_HPP__

#include "global.hpp"
#include <conio.h>
#include <dos.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*************************************************************************
/													                             MACROS
*/
#define ASCIILINESZ                64
#define TEMPERATURE_OK             0
#define TEMPERATURE_READY      	  1
#define TEMPERATURE_END_PROFILE    2
#define TEMPERATURE_BEGIN_PROFILE  3
#define FIFO_SIZE  64
#define POLYNOMIAL 0xA001

typedef unsigned char byte;
typedef unsigned int  word;

/*************************************************************************
/													                         PROTOTYPES
*/
unsigned int Temperature_Calc_Crc(unsigned char *start_of_packet, unsigned char *end_of_packet);
byte Temperature_Get_Character();
void Temperature_Send_Setpoint_Packet( float new_setpoint );
float Temperature_Read_Actual_Packet( );
int Temperature_Status();
void Temperature_Tasks_Completed();
void Temperature_Set_SetPoint( float new_set_point );
float Temperature_Get_SetPoint();
int Temperature_Get_Step();
void Temperature_Next();
void Temperature_Previous();
float Temperature_Actual();
int Temperature_Process();
int Temperature_Initialization( char * file_name  );
void Temperature_Terminate();

void  Serial_Terminate( void );

#endif