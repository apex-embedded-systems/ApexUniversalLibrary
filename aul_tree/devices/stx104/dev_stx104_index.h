#ifndef DEVICE_STX104_INDEX_H_
#define DEVICE_STX104_INDEX_H_
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


#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************/
void STX104_Bank_Device_Set( devmgr_device_type * device, uint8_t bank /* 0 or 1 */ );

void STX104_Bank_Set_Device( devmgr_device_type * device, uint8_t bank /* 0 or 1 */ );
void STX104_Bank_Set( int board, uint8_t bank /* 0 or 1 */ );

void STX104_Index_Write_U8(  int board, uint8_t index, uint8_t   value );
void STX104_Index_Write_U16( int board, uint8_t index, uint16_t  value );
void STX104_Index_Write_U32( int board, uint8_t index, uint32_t  value );

void STX104_Index_Read_U8(   int board, uint8_t index, uint8_t*  value );
void STX104_Index_Read_U16(  int board, uint8_t index, uint16_t* value );
void STX104_Index_Read_U32(  int board, uint8_t index, uint32_t* value );

void STX104_Index_Device_Write_U8(  devmgr_device_type * device, uint8_t index, uint8_t   value );
void STX104_Index_Device_Write_U16( devmgr_device_type * device, uint8_t index, uint16_t  value );
void STX104_Index_Device_Write_U32( devmgr_device_type * device, uint8_t index, uint32_t  value );

void STX104_Index_Device_Read_U8(   devmgr_device_type * device, uint8_t index, uint8_t*  value );
void STX104_Index_Device_Read_U16(  devmgr_device_type * device, uint8_t index, uint16_t* value );
void STX104_Index_Device_Read_U32(  devmgr_device_type * device, uint8_t index, uint32_t* value );

int STX104_Index_Termination( void );
int STX104_Index_Initialization( void );

/************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_STX104_INDEX_H_ */
