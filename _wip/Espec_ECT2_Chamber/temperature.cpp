/*

2014AUG04:

Original master found here:
Sourcesafe:  Engineering database
Location:  $/STX104/Software/sth

*/




/*#############################################################################
	 < < <  TEMPERATURE CONTROL AND INTERFACE  > > >
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

#include "temperature.hpp"

struct fifo_struct
{
   byte empty;
   byte full;
   byte begin_pointer;
   byte end_pointer;
	byte data[FIFO_SIZE];
};

struct uart_struct
{
   unsigned int port;
   unsigned int base;
   unsigned int baud_rate;
   byte word_size;
   byte stop_bits;
   byte parity;
   byte irq;
	byte line_status;
   byte modem_status;
};
/*************************************************************************
/
/
*/
typedef struct fifo_struct fifo_type;
typedef struct uart_struct uart_type;
/*************************************************************************
/
/
*/
fifo_type tx1;
fifo_type rx1;
fifo_type tx2;
fifo_type rx2;
uart_type uart1;
uart_type uart2;

/*************************************************************************
/
/
*/
void interrupt ( *old_irq2)(__CPPARGS);
void interrupt ( *old_irq3)(__CPPARGS);
void interrupt ( *old_irq4)(__CPPARGS);
void interrupt ( *old_irq5)(__CPPARGS);
void interrupt ( *old_irq6)(__CPPARGS);
void interrupt ( *old_irq7)(__CPPARGS);

#include "uart16450.c"


float temperature_setpoint;
int   temperature_status;
int   temperature_profile_pointer;
int   temperature_task_complete;
int   temperature_phase;
int   temperature_prev_phase;
int   temperature_timer;

int temperature_profile[128];
int temperature_max_profile_pointer;
char temperature_work_file_name[32];

/*************************************************************************
/                                                            SEND SETPOINT
*/
unsigned int Temperature_Calc_Crc(unsigned char *start_of_packet, unsigned char *end_of_packet)
{
	unsigned int crc;
	unsigned char bit_count;
	unsigned char *char_ptr;

	/* Start at the beginning of the packet */
	char_ptr = start_of_packet;
	/* Intitialize CRC */
	crc = 0xffff;
	/* Loop through the entire packet */
	do {
		/* Exlusive-OR the byte with the CRC */
		crc ^= (unsigned int)*char_ptr;
		/* Loop through all 8 data bits */
		bit_count = 0;
		do {
			/* If the LSB is 1, shift the CRC and XOR the poynomial mask with the CRC */
         if(crc & 0x0001)
         {
         	crc >>= 1;
            crc ^= POLYNOMIAL;
         }
			/* If the LSB is 0, shift the CRC only */
			else
         {
				crc >>= 1;
      	}
		} while(bit_count++ < 7);
	} while(char_ptr++ < end_of_packet);
	return(crc);
}
/*************************************************************************
/                                                           RECEIVE ACTUAL
*/
byte Temperature_Get_Character()
{
	byte data;
   data = Serial_Receive( &uart1, &rx1 );
//   if (rx1.empty == TRUE) printf("E:");
//   else printf("%02X:",data);
   return data;
}
/*************************************************************************
/                                                            SEND SETPOINT
*/
void Temperature_Send_Setpoint_Packet( float new_setpoint )
{
   int sp;
   int i;

	unsigned char mssp[8]={0x01,0x06,0x01,0x2C,0x00,0x00,0x00,0x00};

   sp = (int) (new_setpoint*10.0);
   /* insert set point */
   mssp[4] = (unsigned char) ((sp >> 8) & 0x00FF);
   mssp[5] = (unsigned char) (sp & 0x00FF);
   /* insert CRC */
   sp = Temperature_Calc_Crc( &mssp[0], &mssp[5] );  /* not sure on ending value */
   mssp[6] = (unsigned char) (sp & 0x00FF);
   mssp[7] = (unsigned char) ((sp >> 8) & 0x00FF);
   /* send packet */
   for ( i=0; i<8; i++ )
   {
   	//delay(3);
   	//Serial_Transmit( mssp[i], &uart1, &tx1 );
      Serial_Fill_TX_Buffer( mssp[i], &uart1, &tx1 );
   }
   //Serial_Start_TX( &uart1, &tx1 );
   Serial_Send_Buffer( &uart1, &tx1 );

//   printf("SP::");
//   for ( i=0; i<8; i++ )
//   {
//   	printf("%02X:", mssp[i]);
//   }
//   printf("\n");

      /* flush to be sure */
   for ( i=0; i<16; i++ ) sp = Temperature_Get_Character();
}

/**********************************************/
float Temperature_Read_Actual_Packet( )
{
	unsigned char value;
   unsigned int scratch;
   int result;
   int i;
   float fresult;

   unsigned char mrat[8]={0x01,0x03,0x00,0x64,0x00,0x01,0xC5,0xD5};

   for ( i=0; i<8; i++ )
   {
   	//delay(3);
   	//Serial_Transmit( mrat[i], &uart1, &tx1 );
      Serial_Fill_TX_Buffer( mrat[i], &uart1, &tx1 );
   }
   //Serial_Start_TX( &uart1, &tx1 );
   Serial_Send_Buffer( &uart1, &tx1 );
   delay(1000); /* wait for data to arrive */
//   printf("RAS::");
//   for ( i=0; i<8; i++ )
//   {
//   	printf("%02X:", mrat[i]);
//   }
//   printf("\n");

   result = Temperature_Calc_Crc( &mrat[0], &(mrat[5]) );  /* not sure on ending value */
//   printf("CRC=%04X::", result );

//   printf("RAR::");


   value = Temperature_Get_Character(); /* address */
   value = Temperature_Get_Character(); /* function read */
   value = Temperature_Get_Character(); /* number of bytes returned */
   value = Temperature_Get_Character(); /* high byte of register 100 */
   scratch = ((unsigned int) value)<<8;
   value = Temperature_Get_Character(); /* low byte of register 100 */
   scratch = scratch | ((unsigned int) value);
   result = (int) scratch;
   value = Temperature_Get_Character(); /* low byte CRC */
   value = Temperature_Get_Character(); /* high byte CRC */

   /* flush to be sure */
   for ( i=0; i<8; i++ ) value = Temperature_Get_Character();

   fresult = ((float) result)/10.0;
//   printf("\nReceived Temperature = %f\n", fresult);
   return ( fresult );
}

/*************************************************************************
/                                                                   STATUS
*/
int Temperature_Status()
{
	return temperature_status;
}
/*************************************************************************
/                                                                 COMPLETE
*/
void Temperature_Tasks_Completed()
{
	temperature_task_complete = 1;
}
/*************************************************************************
/                                                                SET POINT
*/
void Temperature_Set_SetPoint( float new_set_point )
{
	temperature_setpoint = new_set_point;
   Temperature_Send_Setpoint_Packet( temperature_setpoint );
}
/*************************************************************************
/                                                                SET POINT
*/
float Temperature_Get_SetPoint()
{
	return temperature_setpoint;
}
/*************************************************************************
/                                                                    STEP
*/
int Temperature_Get_Step()
{
	return temperature_profile_pointer;
}
/*************************************************************************
/													              TEMP WORK FILE UPDATE
*/
void Temperature_Work_File_Update()
{
	FILE *work_file;
   char valbuf[16], strbuf[16];

   if ( (work_file = fopen(temperature_work_file_name, "wt"))!= NULL )
   {
   	itoa( Temperature_Get_Step(), valbuf, 10 );    /* index */
      strcpy( strbuf, valbuf );
      fputs( strbuf, work_file );
      fclose( work_file );
   }
}
/*************************************************************************
/                                                                     NEXT
*/
void Temperature_Next()
{
   float scratch;

   if ( temperature_profile_pointer < temperature_max_profile_pointer )
   {
      Temperature_Work_File_Update();
      temperature_profile_pointer++;
   	temperature_status = TEMPERATURE_OK;
      scratch = temperature_profile[temperature_profile_pointer];
      Temperature_Set_SetPoint( scratch );
   }
   else
   {
   	temperature_status = TEMPERATURE_END_PROFILE;
   }
}
/*************************************************************************
/                                                                 PREVIOUS
*/
void Temperature_Previous()
{
   float scratch;

   if ( temperature_profile_pointer >= 0 )
   {
   	Temperature_Work_File_Update();
      temperature_profile_pointer--;
   	temperature_status = TEMPERATURE_OK;
      scratch = temperature_profile[temperature_profile_pointer];
      Temperature_Set_SetPoint( scratch );
   }
   else
   {
   	temperature_status = TEMPERATURE_BEGIN_PROFILE;
   }
}
/*************************************************************************
/                                                                   ACTUAL
*/
float Temperature_Actual()
{
   return Temperature_Read_Actual_Packet();
}

/*************************************************************************
/                                                                  PROCESS
*/
int Temperature_Process()
{
	float scratch;
   float actual_temp;

	switch ( temperature_phase )
   {
   	case 0: /* initialization phase */
      {
      	//temperature_profile_pointer = 0;
         scratch = temperature_profile[temperature_profile_pointer];
         temperature_status = TEMPERATURE_BEGIN_PROFILE;
         Temperature_Set_SetPoint( scratch );
      	temperature_phase++;
         break;
      }
      case 1: /* wait to reach temperature */
      {
         actual_temp = Temperature_Actual();
         temperature_task_complete = 0;
         temperature_timer = 60*5/2; /* wait 5 minutes for pre-soak time */
      	scratch = fabs( actual_temp - temperature_setpoint );
//         printf("Temp=%f\n", actual_temp);
         /* begin time out phase if temperature is 0.1C within set point */
      	if ( scratch < 0.3 ) temperature_phase++;
         break;
      }
      case 2: /* pre-soak time */
      {
         temperature_task_complete = 0;
      	if ( temperature_timer > 0 ) temperature_timer--;
         else temperature_phase++;
         break;
      }
      case 3: /* wait to reach temperature */
      {
      	temperature_task_complete = 0;
         temperature_timer = 60*10/2; /* wait fifteen minutes before taking data */
      	scratch = fabs( Temperature_Actual() - temperature_setpoint );
         /* begin time out phase if temperature is 0.1C within set point */
      	if ( scratch < 0.3 ) temperature_phase++;
         break;
      }
      case 4: /* soak time */
      {
         temperature_task_complete = 0;
      	if ( temperature_timer > 0 ) temperature_timer--;
         else temperature_phase++;
         break;
      }
      case 5:  /* temperature ready */
      {
         temperature_task_complete = 0;
         temperature_status = TEMPERATURE_READY;
         temperature_phase++;
         break;
      }
      case 6:  /* wait to proceed */
      {
      	if ( temperature_task_complete != 0 )
         {
         	temperature_phase++;
            temperature_status = TEMPERATURE_OK;
         }
         break;
      }
      case 7: /* next temperature value */
      {
      	Temperature_Next();
         temperature_task_complete = 0;
         if ( temperature_status != TEMPERATURE_END_PROFILE ) temperature_phase = 1;
         else temperature_phase++;
         break;
      }
      case 8: /* end phase */
      {
      	temperature_task_complete = 0;
         scratch = 22.0;   /* room temperature */
         delay(1000);
      	Temperature_Set_SetPoint( scratch );
         scratch = 22.0;
         delay(1000);
      	Temperature_Set_SetPoint( scratch );
         scratch = 22.0;
         delay(1000);
      	Temperature_Set_SetPoint( scratch );
         scratch = 22.0;
         delay(1000);
      	Temperature_Set_SetPoint( scratch );
         break;
      }
   }
   
   if ( temperature_phase != temperature_prev_phase ) printf("%d  ", temperature_phase );
	temperature_prev_phase = temperature_phase;

   return temperature_status;
}


/*************************************************************************
/													                            PROFILE
*/
void Temperature_Profile( char * file_name )
{
	FILE *in_file, *work_file;
   int index, scratch;
   char str_value[64];

   temperature_profile_pointer = 0;
   temperature_status = TEMPERATURE_BEGIN_PROFILE;

   if ( (in_file = fopen("temper.pfl", "rt")) == NULL )
   {
      printf(" Temperature: Cannot open temperature profile....using default profile\n");
      temperature_profile[0]  = -40;
      temperature_profile[1]  = -30;
      temperature_profile[2]  = -20;
      temperature_profile[3]  = -10;
      temperature_profile[4]  = 0;
      temperature_profile[5]  = 10;
      temperature_profile[6]  = 20;
      temperature_profile[7]  = 30;
      temperature_profile[8]  = 40;
      temperature_profile[9]  = 50;
      temperature_profile[10]  = 60;
      temperature_profile[11] = 70;
      temperature_profile[12] = 85;
      temperature_max_profile_pointer = 13;
   }
   else
   {
   	temperature_max_profile_pointer = 0;
   	while ( fgets(str_value, ASCIILINESZ, in_file) != NULL )
   	{
//printf("%s\n", str_value );
			if ( strchr(str_value,';')==NULL )
      	{
         	sscanf( str_value, "%d", &temperature_profile[temperature_max_profile_pointer] );
//printf("index=%d, temp=%d\n",temperature_max_profile_pointer,temperature_profile[temperature_max_profile_pointer] );
            temperature_max_profile_pointer++;
         }
		}
		fclose(in_file);
   }

   for ( index=0; index<temperature_max_profile_pointer; index++ )
   {
   	printf("       Step=%d, Temp=%d\n", index, temperature_profile[index] );
   }
   printf("\n");

  	strcpy( temperature_work_file_name, file_name);
  	strcat( temperature_work_file_name, ".tmp");
  	if ( (work_file = fopen(temperature_work_file_name, "r"))!= NULL )
  	{
  		fgets(str_value, 8, work_file);
   	temperature_profile_pointer = atoi( str_value );
  		fclose( work_file );
   	temperature_profile_pointer++;
   }
   else
   {
      temperature_status = TEMPERATURE_END_PROFILE;
   }

   if ( temperature_profile_pointer < temperature_max_profile_pointer )
   {
   	scratch = temperature_profile[temperature_profile_pointer];
   	Temperature_Set_SetPoint( scratch );
   	temperature_phase = 0;
   	temperature_status = TEMPERATURE_BEGIN_PROFILE;
   }
   else
   {
   	temperature_status = TEMPERATURE_END_PROFILE;
   }
}
/*************************************************************************
/                                                               INITIALIZE
*/
int Temperature_Initialization( char * file_name )
{
   byte breg;
   int i;

   Temperature_Profile(file_name);

   if ( temperature_status != TEMPERATURE_END_PROFILE )
   {
   	/* configure line control register */
   	uart1.base			= 0x3f8; /* base address */
   	uart1.parity 		= 0;     /* no parity     */
   	uart1.stop_bits 	= 0;  	/* one stop bit  */
   	uart1.word_size	= 3;  	/* 8-bit word    */
   	uart1.baud_rate	= 6;  	/* BAUD19200     */
   	breg = uart1.parity | uart1.stop_bits | uart1.word_size;
   	outp(uart1.base + IER, 0x00); // temporary for now
   	outp(uart1.base + LCR, breg);
   	Set_Baud ( &uart1 );
   	/* initialize fifo */
   	Init_Fifo( &rx1 );
   	Init_Fifo( &tx1 );
   	outp(uart1.base + IIR, 0x00); // temporary for now
   	//outp(uart1.base + IER, 0x03); // temporary for now
   	outp(uart1.base + IER, 0x01); // only receive interrupts for now
   	outp(uart1.base + MCR, 0x0F); // temporary for now
   	Irq_Install(4,0);
   	outp(IMR, inp(IMR) & IRQ4);   // temporary for now

   	/* flush UART */
   	for (i=0;i<32;i++)
   	{
   		breg= inp(uart1.base + RXR);
      	breg= inp(uart1.base + LSR);
      	breg= inp(uart1.base + MSR);
      	breg= inp(uart1.base + IIR);
   	}
   	enable();

   	temperature_task_complete = 0;
   	temperature_status = TEMPERATURE_BEGIN_PROFILE;
   }
   return temperature_status;
}

/*************************************************************************
/                                                                TERMINATE
*/
void Temperature_Terminate()
{
	Serial_Terminate();
}
