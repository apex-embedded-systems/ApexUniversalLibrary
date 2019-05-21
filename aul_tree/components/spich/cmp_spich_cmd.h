#ifndef COMPONENT_SPI_COMMAND
#define COMPONENT_SPI_COMMAND 22 /* component ID number */
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

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
#define SPI_COMMAND_REV  "$Date: 2014-06-27 09:56:24 -0500 (Fri, 27 Jun 2014) $"

/*******************************************************************/
/* This merges both the base ID above along with a specific instance.
   Actually, the instance information will be updated later
*/
#define SPI_LOGICAL_ID	     IOC_CREATE_LOGICAL_ID_COMPONENT(COMPONENT_SPI_COMMAND)

/*
Spi_Read()  - read software buffer
Spi_Write() - write software buffer, if "auto", then send to hardware and block.

Spi_Buffer_Size() - reports actual hardware buffer size.
Spi_Buffer_Clear() - Clear read and/or write buffers
Spi_Buffer_Transfer(	component_type * component, 
						struct spi_data * tx_data,
						struct spi_data * rx_data,

						) - transfer buffer data, blocking until complete.

Configuration information:

bit_rate_hz
clock_polarity  - CPOL
clock_phase     - CPHA  
bit_order		- big/little endian
byte_delay_ns	- delay between each byte transmitted.
other timing items - if not supported and have been sent either ignore or return warning.

Commands - part of buffer stream
- chip select (each bit indicates or bit pattern)
- byte values
- chip deselect

- the buffer really ought to be held by upper component, since it could then manage how many
  and what exactly goes on.

BUFFER_CLEAR		- clears the transmit buffer.

BUFFER_COMMIT		- executes the transmit buffer and returns with the returned data.
					  it may or may not write the data to the hardware buffer first, all
					  depending on implementation.

BUFFER_LOOP_END		- end auto cycling.

BUFFER_WRITE		- writes a buffer of bytes/commands to the transmit buffer
					  returns with error code if something not right.

					  The software must analyze the data sent to be sure it conforms to any
					  restrictions of the hardware/implementation. 

BUFFER_READ			- returns any read data sitting in a buffer, this all depends on
					  configuration.  

BUFFER_SIZE			- returns the size of the hardware transmit buffer

BUFFER_AVAILABLE	- returns the number of bytes available in the hardware transmit buffer 


LTC2449:  
1. Method #1: CS0_ENABLE, 4-bytes, CS0_DISABLE
2. Method #2: CS0 always enabled.  Delay + transmission ==> sample interval.
			  ( DELAY, 4-bytes ) x 8  
3. Method #3: CS0 always enabled.  Requires new hardware implementation.
			  REPEAT( DELAY, 4-bytes )
			  
			  Total hardware u16 buffer size: 32-bytes + 8-delays = 40 u16 words.

			  REPEAT UNTIL <request> { DELAY, 4-bytes, DELAY, 4-bytes, ...., DELAY, 4-bytes }  8-sets
	
Commands could be compressed in a more meaningful way:

This gives us the testability we are looking for!
0000 0001 CCCC CCCC  - chip select - enable
0000 0010 CCCC CCCC  - chip select - disable
0000 0000 DDDD DDDD  - data
1TTT TTTT TTTT TTTT  - delay in half clock intervals 0 to 32767 ticks.
						basically, we load the counter with this value, decrement and wait until zero.
0100 0000 AAAA AAAA	 - jump to address until signal is reached.

Example:  
	SPI_CHIP_SELECT_ENABLE  | 1
	SPI_DELAY_HCLK			| 3000
	SPI_DATA				| 0x12
	SPI_DELAY_HCLK			| 3000
	SPI_DATA				| 0x34
	SPI_CHIP_SELECT_DISABLE | 1

Configuration:
1. half-clock interval
2. buffer loop start:   software, timer loop.
3. buffer loop stop request source:  software, other.


Modify STX104 SPI to accept a 16-bit buffer value where the top 8-bits are the control and
the bottom 8-bits are the data.  The top 8-bits control the chip selects and possibly pre-
byte transmit?
*/


#define COMPONENT_SPI_CMD_DEFINITIONS(_) \
	_( SPI_LOGICAL_ID, 	Spi_CMD_Restart,	CMD_SPI_RESTART,	CMD_CMP_RESTART			)\
	_( SPI_LOGICAL_ID, 	Spi_CMD_Update,		CMD_SPI_UPDATE,		CMD_CMP_UPDATE			)\
	_( SPI_LOGICAL_ID, 	Spi_CMD_Power_Up,	CMD_SPI_POWER_UP,	CMD_CMP_POWER_UP		)\
	_( SPI_LOGICAL_ID, 	Spi_CMD_Power_Down,	CMD_SPI_POWER_DOWN,	CMD_CMP_POWER_DOWN		)\
	_( SPI_LOGICAL_ID, 	Spi_CMD_Cfg_Set,	CMD_SPI_CONFIG_SET,	CMD_CMP_CONFIG_SET		)\
	_( SPI_LOGICAL_ID, 	Spi_CMD_Cfg_Get,	CMD_SPI_CONFIG_GET,	CMD_CMP_CONFIG_GET		)\
	_( SPI_LOGICAL_ID, 	Spi_CMD_Read,		CMD_SPI_READ,		CMD_CMP_READ			)\
	_( SPI_LOGICAL_ID, 	Spi_CMD_Write,		CMD_SPI_WRITE,		CMD_CMP_WRITE			)\
	_( SPI_LOGICAL_ID, 	NULL,				CMD_SPI_PRESENT,	CMD_CMP_PRESENT			)\

	_( SPI_LOGICAL_ID, 	Spi_Write,			CMD_SPI_BUFFER_CLEAR,	CMD_CMP_LAST + 1		)\
	_( SPI_LOGICAL_ID, 	Spi_Write,			CMD_SPI_BUFFER_RUN,		CMD_CMP_LAST + 2		)\
	_( SPI_LOGICAL_ID, 	Spi_Write,			CMD_SPI_BUFFER_RUN,		CMD_CMP_LAST + 2		)\

enum {  SPI_JUMP_TABLE_QTY = (CMD_CMP_LAST + 0) - CMD_CMP_RESTART + 1 };
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


#endif /* COMPONENT_SPI_COMMAND */
