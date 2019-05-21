#ifndef COMPONENT_SPI
#define COMPONENT_SPI		   22 /* component ID number */
#define COMPONENT_SPI_NAME	"spich"
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

#include "../../shared/typebase.h"	 /* uint8_t, uint16_t, etc */
#include "../../shared/typebool.h"
#include "cmp_spi_reg.h"
#include "../../cmpmgr/cmpmgr.h"


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/* Op-codes used to build sequences.

Typical LTC2449 transaction:
	Configuration:
		clock_interval_ns		= ;  // call a function to retrieve half-clocks for a particular interval.
		clock_polarity			= ;   //
		clock_phase				= ;   // INVERT NONINVERT
		bit_order				= ;   // SPI_ENDIAN_BIG  SPI_ENDIAN_LITTLE
		chip_select_polarity	= 1;  // inverted.
		data_out_polarity		= 0;  //0 or 1
		data_in_polarity		= 0;  //0 or 1
		buffer_commit_source	= SPI_COMMIT_SOURCE__SOFTWARE;  // 16 possible sources
		commit_timmer_half_clocks = 

	Buffer Entries:
		SPI_CMD_SELECT_ENABLE		|  1            <--- CS0 enabled.
		SPI_CMD_DELAY_HALF_CLOCKS	|  1 			<--- delay for 1 half clock
		SPI_CMD_DATA				|  data[0]      <--- send first byte
		SPI_CMD_DATA				|  data[1]		<--- send second byte
		SPI_CMD_DATA				|  data[2]		<--- send third byte
		SPI_CMD_DATA				|  data[3]		<--- send fourth byte
		SPI_CMD_DELAY_HALF_CLOCKS	|  1     		<--- delay for 1 half clock
		SPI_CMD_SELECT_DISABLE		|  1            <--- CS0 disable.


Typical LTC2449 single channel repeat transactions:
	Configuration:
		clock_interval_ns		= ;  // call a function to retrieve half-clocks for a particular interval.
		clock_polarity			= ;
		clock_phase				= ;
		bit_order				= ;
		chip_select_polarity	= 1;  // inverted.
		data_out_polarity		= 0;
		data_in_polarity		= 0;
		buffer_commit_source	= SPI_COMMIT_SOURCE__TIMER;
		commit_timmer_half_clocks = SPI_Half_Clocks_From_Nanoseconds( 1,000,000 );  // once per millisecond

	Buffer Entries:
		SPI_CMD_SELECT_ENABLE		|  1            <--- CS0 enabled.
		SPI_CMD_DELAY_HALF_CLOCKS	|  1 			<--- delay for 1 half clock
		SPI_CMD_DATA				|  data[0]      <--- send first byte
		SPI_CMD_DATA				|  data[1]		<--- send second byte
		SPI_CMD_DATA				|  data[2]		<--- send third byte
		SPI_CMD_DATA				|  data[3]		<--- send fourth byte
		SPI_CMD_DELAY_HALF_CLOCKS	|  1     		<--- delay for 1 half clock
		SPI_CMD_SELECT_DISABLE		|  1            <--- CS0 disable.
*/
enum spi_opcode
{
	SPI_CMD_NOP					= 0x0000, 
	SPI_CMD_SELECT				= 0x1000, /* 0011 SSSS SSSS SSSS - up to twelve select lines	*/
	SPI_CMD_BATCH				= 0x2000, /* + 1:  run  buffer in re-usable buffer mode - blocking */
										  /* + 2:  clear buffer   */

	SPI_CMD_FIFO				= 0x3000, /* + 1:  run buffer in FIFO mode   */

	SPI_CMD_DATA				= 0x4000, /* 0100 XXXX DDDD DDDD - data byte					*/
	SPI_CMD_DELAY_HALF_CLOCKS	= 0x8000  /* 1TTT TTTT TTTT TTTT - half-clock delay count		*/

};

/*******************************************************************/
struct spi_data
{
	SYS_TYPE	type;
	int			size;  /* buffer maximum size */
	int			actual; /* actual data in buffer */
	uint16_t *	buffer;


};

/*******************************************************************/
struct spi_cfg
{
	/* set to SYS_TYPE__I8255_CFG_TYPE so that the I/O control
	   can confirm that we have the correct data 
	   structure.  Also an indication of allocation.
	*/
	SYS_TYPE	type;   /* SYS_TYPE__spi_cfg_type */

	SPI_MODE_ENUM			mode;
	uint32_t				bit_rate_hz;
	uint32_t				bit_interval_ns;
	uint8_t					chip_select_polarity;	/* bit pattern */
	uint8_t					data_out_polarity;		/* bit pattern */
	uint8_t					data_in_polarity;		/* bit pattern */


	BOOL					loop_back;		/* tie SDO directly back into SDI */
	/* ??
	 00 - controlled by software 
	 01 - 
	 10 - 
	 11 - 
	*/
	SPI_CHIP_SELECT_BEHAVIOR_ENUM	chip_select_behavior;
};
typedef struct spi_cfg spi_cfg_type;

/*******************************************************************/
#define SPI_INITIALIZE_DATASET  \
	{	/* type			*/			SYS_TYPE_MAKE( spi_dataset_type ), \
		/* cfg			*/			{ SYS_TYPE_MAKE( spi_cfg_type ) }  \
	}

/*******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************
Description
Hardware Scanning

This is used to map in a hardware scanner during hardware
initialization that will occur during Initialize() and/or
Open().  This is a special case for components that provide
address space such as the index-data array implementation.
*/
/*
#define SPI_HW_SCAN_AVAILABLE(_) _( SPI_BASE_ID, SPI_Hardware_Scan )
*/

int Spi_Create( component_type * component, size_t instance );

int Spi_Destroy( component_type * component );

int Spi_Config_Initialize( spi_cfg_type * cfg );



/******************************************************************
*/
#if defined( TARGET_CHECK_BUILD )
#endif

extern component_definition_type component_spi_definition;

/*******************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_SPI */