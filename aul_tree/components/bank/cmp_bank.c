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
    
	COPYRIGHT NOTICE Copyright (c) 2015 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded 
	Systems Any reproduction or dissemination is prohibited unless specifically 
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


*********************************************************************************/

#include "cmp_bank.h"
#include "../../shared/cmd.h"
#include "../../shared/status.h"
#include "../../io/io_private.h"  /* IO_COPY_FROM_USER, IO_COPY_TO_USER */ 
#include "../../shared/error.h"
/*******************************************************************/
#define BANK_MODULE_REV  "$Date: 2015-02-05 13:57:29 -0600 (Thu, 05 Feb 2015) $"
/****************************************************************/
enum
{
	BANK_REG      = 0,
	BANK_REG_LSB  = 1,
	BANK_REG_MSB  = 2,
} CMP_BANK_REG_ENUM;

static const char * bank_reg_name[] =
{
	"bank", "bank_lsb", "bank_msb"
};
/****************************************************************/
#define BANK_REGISTER_COUNT 6
/****************************************************************/
struct bank_dataset
{
	SYS_TYPE			type;   /* must always be first */
	bank_cfg_type		cfg;	/* must always be second */
	REGISTER_TYPE		reg_table[BANK_REGISTER_COUNT + 1];
};
typedef struct bank_dataset bank_dataset_type;
/*******************************************************************/
static int Bank_CMD_Write(	component_type *	upper_component,
								size_t				index,		/* register ID, of upper level component */
								char *				buf, 
								size_t				buf_len 
							 ) 
{
	int					error_code;
	component_type *	bac;			/* bank addressing component (this component)		    */
	REGISTER_TYPE *		uc_reg;			/* upper component register								*/
	bank_dataset_type *	ds;
	size_t				uc_reg_size;	/* upper component register size in bytes				*/
	size_t				uc_reg_offset;	/* upper component register offset in the indexed space */
	size_t				ar_size;		/* address register size								*/
	/* support parameters */
	size_t				address;
	size_t				bank;
	size_t				i;
	size_t				word;
	uint16_t *			buf_u16;
	size_t				u16_total;
	size_t				element_total;

	bac = upper_component->component_link;
	if ( NULL == bac )
	{ error_code = EC_BANK_COMPONENT_LINK;  goto Bank_CMD_Write_Error; }
	ds				= ( bank_dataset_type *) bac->private_data;
//TODO: test for logical id??
	uc_reg			= &(upper_component->definition->register_table[index]);

	//upper_component->definition->register_table[index].virtual_address;

	uc_reg_size		= REG_GET_ELEMENT_SIZE( uc_reg->info );
	ar_size			= REG_GET_ELEMENT_SIZE( ds->reg_table[BANK_INDEX].info );

	/* calculate the indexed address location */
	uc_reg_offset = uc_reg->virtual_address + upper_component->offset;

	if ( REG_TEST_AUTO_INCREMENT( uc_reg->info ) )
	{ /* write to sequential locations */
		if ( REG_BYTES_LESS_ONE(1) == REG_GET_REGISTER_APERTURE( uc_reg->info ) )
		{ /* write out single byte at a time - size does not matter! */

			for ( i = 0; i < buf_len; i++ )
			{
				/* write the bank address out first */
				if ( REG_BANK_NONE != uc_reg->state )
				{
					bank = REG_GET_BANK( uc_reg->state ); /* retrieve the associated registers bank value */
					if ( !( ds->write_based_on_copy ) || ( bank != ds->bank ) )
					{
						CmpMgr_Reg_Write(	bac,
											BANK_REG,			/* the bank register								*/
											(char *)(&bank),	/* the address of the register to be written		*/
											REG_GET_ELEMENT_SIZE( ds->reg_table[BANK_REG].info )
										);
						ds->bank = bank;
					}
				}

				/* write the data */
				CmpMgr_Reg_Write(	bac,
									(index + i),	/* register index or ID or row				*/
									&(buf[i]),		/* the data to be written to the register	*/
									sizeof(uint8_t)
								);
			}

		}
		else
		{ /* write out 16-bit word at a time, address is byte resolution, size does not matter */
			buf_u16   = (uint16_t *) buf;  /* char to UINT16 */
			u16_total = buf_len / sizeof( uint16_t ); /* total UINT16 count */
			/* write the bank address out first */
			if ( REG_BANK_NONE != uc_reg->state )
			{
				bank = REG_GET_BANK( uc_reg->state ); /* retrieve the associated registers bank value */
				if ( !( ds->write_based_on_copy ) || ( bank != ds->bank ) )
				{
					CmpMgr_Reg_Write(	bac,
										BANK_REG,			/* the bank register								*/
										(char *)(&bank),	/* the address of the register to be written		*/
										REG_GET_ELEMENT_SIZE( ds->reg_table[BANK_REG].info )
									);
					ds->bank = bank;
				}
			}
			for ( i = 0; i < u16_total; i++ )
			{
				address = uc_reg_offset + i*2;
				/* write the data */
				CmpMgr_Reg_Write(	bac,
								BANK_DATA,				/* the data register								*/
								(char *)(&(buf_u16[i])),/* the data to be written to the register			*/
								sizeof(uint16_t)
						     ); 
			}
		}
	}
	else
	{ /* write to same location 'element_count' times */
		if ( REG_BYTES_LESS_ONE(1) == REG_GET_REGISTER_APERTURE( uc_reg->info ) )
		{ /* write out single byte at a time */
			if ( REG_SIZE_UINT8 == uc_reg_size )
			{ /* write out 8-bit byte string  */

				/* write the bank address out first */
				if ( REG_BANK_NONE != uc_reg->state )
				{
					bank = REG_GET_BANK( uc_reg->state ); /* retrieve the associated registers bank value */
					if ( !( ds->write_based_on_copy ) || ( bank != ds->bank ) )
					{
						CmpMgr_Reg_Write(	bac,
											BANK_REG,			/* the bank register								*/
											(char *)(&bank),	/* the address of the register to be written		*/
											REG_GET_ELEMENT_SIZE( ds->reg_table[BANK_REG].info )
										);
						ds->bank = bank;
					}
				}




				for ( i = 0; i < buf_len; i++ )
				{ 
					address = uc_reg_offset;
					/* write the address out first */
					CmpMgr_Reg_Write(	bac,
									BANK_INDEX,		/* the address register								*/
									(char *)(&address),	/* the address of the register to be written		*/
									ar_size 
								 );

					/* write the data */
					CmpMgr_Reg_Write(	bac,
									BANK_DATA_LSB,	/* the data register								*/
									&(buf[i]),				/* the data to be written to the register			*/
									sizeof(uint8_t) 
								 );
				}
			}
			else
			{ /* write words a byte at a time */
				element_total   = buf_len / uc_reg_size;

				/* write the bank address out first */
				if ( REG_BANK_NONE != uc_reg->state )
				{
					bank = REG_GET_BANK( uc_reg->state ); /* retrieve the associated registers bank value */
					if ( !( ds->write_based_on_copy ) || ( bank != ds->bank ) )
					{
						CmpMgr_Reg_Write(	bac,
											BANK_REG,			/* the bank register								*/
											(char *)(&bank),	/* the address of the register to be written		*/
											REG_GET_ELEMENT_SIZE( ds->reg_table[BANK_REG].info )
										);
						ds->bank = bank;
					}
				}




				for ( i = 0; i < element_total; i = i + uc_reg_size )
				{
					for( word = 0; word < uc_reg_size; word++ )
					{



						address = uc_reg_offset + word;
						/* write the address out first */
						CmpMgr_Reg_Write(	bac,
										BANK_INDEX,		/* the address register								*/
										(char *)(&address),	/* the address of the register to be written		*/
										ar_size 
									 );

						/* write the data */
						CmpMgr_Reg_Write(	bac,
										BANK_DATA,		/* the data register								*/
										&(buf[i + word]),		/* the data to be written to the register			*/
										sizeof(uint8_t)
									 ); 
					}
				}
			}
		}
		else
		{ /* write out 16-bit word at a time */
			/* write 16-bit word string, address is byte resolution, size matters! */
			buf_u16   = (uint16_t *) buf;
			u16_total = buf_len / sizeof( uint16_t );
			if ( (buf_len & 0x0001) == buf_len )
			{ /* odd number of bytes */
				u16_total++;
			}
			if ( REG_SIZE_UINT16 >= uc_reg_size )
			{ /* size is UINT8 or UINT16. */


				/* write the bank address out first */
				if ( REG_BANK_NONE != uc_reg->state )
				{
					bank = REG_GET_BANK( uc_reg->state ); /* retrieve the associated registers bank value */
					if ( !( ds->write_based_on_copy ) || ( bank != ds->bank ) )
					{
						CmpMgr_Reg_Write(	bac,
											BANK_REG,			/* the bank register								*/
											(char *)(&bank),	/* the address of the register to be written		*/
											REG_GET_ELEMENT_SIZE( ds->reg_table[BANK_REG].info )
										);
						ds->bank = bank;
					}
				}


				for ( i = 0; i < u16_total; i++ )
				{




					address = uc_reg_offset;
					/* write the address out first */
					CmpMgr_Reg_Write(	bac,
									BANK_INDEX,		/* the address register								*/
									(char *)(&address),	/* the address of the register to be written		*/
									ar_size 
									);

					/* write the data */
					CmpMgr_Reg_Write(	bac,
									BANK_DATA,				/* the data register								*/
									(char *)(&(buf_u16[i])),/* the data to be written to the register			*/
									sizeof(uint16_t)
									); 
				}
			}
			else
			{ /* size is UINT32 or larger */
				element_total = buf_len / uc_reg_size;


				/* write the bank address out first */
				if ( REG_BANK_NONE != uc_reg->state )
				{
					bank = REG_GET_BANK( uc_reg->state ); /* retrieve the associated registers bank value */
					if ( !( ds->write_based_on_copy ) || ( bank != ds->bank ) )
					{
						CmpMgr_Reg_Write(	bac,
											BANK_REG,			/* the bank register								*/
											(char *)(&bank),	/* the address of the register to be written		*/
											REG_GET_ELEMENT_SIZE( ds->reg_table[BANK_REG].info )
										);
						ds->bank = bank;
					}
				}


				for ( i = 0; i < element_total; i = i + uc_reg_size )
				{
					for( word = 0; word < u16_total; word++ )
					{



						address = uc_reg_offset + word*2;
						/* write the address out first */
						CmpMgr_Reg_Write(	bac,
										BANK_INDEX,		/* the address register								*/
										(char *)(&address),	/* the address of the register to be written		*/
										ar_size 
										);

						/* write the data */
						CmpMgr_Reg_Write(	bac,
										BANK_DATA,						/* the data register								*/
										(char *)(&(buf_u16[i + word])),	/* the data to be written to the register			*/
										sizeof(uint16_t)
										); 
					}
				}
			} /* size is UINT32 or larger */
		} /* write out 16-bit word at a time */
	} /* write to same location 'element_count' times */

	return SUCCESS;
Bank_CMD_Write_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_BANK_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int Bank_CMD_Read(	component_type *	upper_component,
								size_t				index,		/* register ID, of upper level component */
								char *				buf, 
								size_t				buf_len 
							 ) 
{
	int					error_code;
	component_type *	iac;			/* indexed addressing component (this component)		*/
	REGISTER_TYPE *		uc_reg;			/* upper component register								*/
	bank_dataset_type *	ds;
	size_t				uc_reg_size;	/* upper component register size in bytes				*/
	size_t				uc_reg_offset;	/* upper component register offset in the indexed space */
	size_t				ar_size;		/* address register size								*/
	/* support parameters */
	size_t				address;
	size_t				i;
	size_t				word;
	uint16_t *			buf_u16;
	size_t				u16_total;
	size_t				element_total;

	iac = upper_component->component_link;
	if ( NULL == iac )
	{ error_code = EC_BANK_COMPONENT_LINK;  goto Bank_CMD_Read_Error; }
	ds				= ( bank_dataset_type *) iac->private_data;
//TODO: test for logical id??
	uc_reg			= &(upper_component->definition->register_table[index]);
	uc_reg_size		= REG_GET_ELEMENT_SIZE( uc_reg->info );
	ar_size			= REG_GET_ELEMENT_SIZE( ds->reg_table[BANK_INDEX].info );

	/* calculate the indexed address location */
	uc_reg_offset = uc_reg->virtual_address + upper_component->offset;

	if ( REG_TEST_AUTO_INCREMENT( uc_reg->info ) )
	{ /* read sequential locations */
		if ( REG_BYTES_LESS_ONE(1) == REG_GET_REGISTER_APERTURE( uc_reg->info ) )
		{ /* read single byte at a time - size does not matter! */
			for ( i = 0; i < buf_len; i++ )
			{
				address = uc_reg_offset + i;
				/* write the address out first */
				CmpMgr_Reg_Write(	iac,
								BANK_INDEX,		/* the address register								*/
								(char *)(&address),	/* the address of the register to be written		*/
								ar_size 
								);

				/* read the data */
				CmpMgr_Reg_Read(	iac,
								BANK_DATA_LSB,	/* the data register								*/
								&(buf[i]),		/* the data to be read from the register			*/
								sizeof(uint8_t) 
								);
			}
		}
		else
		{ /* read 16-bit word at a time, address is byte resolution, size does not matter */
			buf_u16   = (uint16_t *) buf;  /* char to UINT16 */
			u16_total = buf_len / sizeof( uint16_t ); /* total UINT16 count */
			for ( i = 0; i < u16_total; i++ )
			{
				address = uc_reg_offset + i*2;
				/* write the address out first */
				CmpMgr_Reg_Write(	iac,
								BANK_INDEX,		/* the address register								*/
								(char *)(&address),	/* the address of the register to be written		*/
								ar_size 
						     );

				/* read the data */
				CmpMgr_Reg_Read(	iac,
								BANK_DATA,				/* the data register								*/
								(char *)(&(buf_u16[i])),/* the data to be read from the register			*/
								sizeof(uint16_t)
						     ); 
			}
		}
	}
	else
	{ /* read same location 'element_count' times */
		if ( REG_BYTES_LESS_ONE(1) == REG_GET_REGISTER_APERTURE( uc_reg->info ) )
		{ /* read single byte at a time */
			if ( REG_SIZE_UINT8 == uc_reg_size )
			{ /* read 8-bit byte string  */
				for ( i = 0; i < buf_len; i++ )
				{ 
					address = uc_reg_offset;
					/* write the address out first */
					CmpMgr_Reg_Write(	iac,
									BANK_INDEX,		/* the address register								*/
									(char *)(&address),	/* the address of the register to be written		*/
									ar_size 
								 );

					/* write the data */
					CmpMgr_Reg_Read(	iac,
									BANK_DATA_LSB,	/* the data register								*/
									&(buf[i]),		/* the data to be read from the register			*/
									sizeof(uint8_t) 
								 );
				}
			}
			else
			{ /* read words a byte at a time */
				element_total   = buf_len / uc_reg_size;
				for ( i = 0; i < element_total; i = i + uc_reg_size )
				{
					for( word = 0; word < uc_reg_size; word++ )
					{
						address = uc_reg_offset + word;
						/* write the address out first */
						CmpMgr_Reg_Write(	iac,
										BANK_INDEX,		/* the address register								*/
										(char *)(&address),	/* the address of the register to be written		*/
										ar_size 
									 );

						/* write the data */
						CmpMgr_Reg_Read(	iac,
										BANK_DATA,		/* the data register								*/
										&(buf[i + word]),/* the data to be read from the register			*/
										sizeof(uint8_t)
									 ); 
					}
				}
			}
		}
		else
		{ /* read 16-bit word at a time */
			/* read 16-bit word string, address is byte resolution, size matters! */
			buf_u16   = (uint16_t *) buf;
			u16_total = buf_len / sizeof( uint16_t );
			if ( (buf_len & 0x0001) == buf_len )
			{ /* odd number of bytes */
				u16_total++;
			}
			if ( REG_SIZE_UINT16 >= uc_reg_size )
			{ /* size is UINT8 or UINT16. */
				for ( i = 0; i < u16_total; i++ )
				{
					address = uc_reg_offset;
					/* write the address out first */
					CmpMgr_Reg_Write(	iac,
									BANK_INDEX,		/* the address register								*/
									(char *)(&address),	/* the address of the register to be written		*/
									ar_size 
									);

					/* write the data */
					CmpMgr_Reg_Read(	iac,
									BANK_DATA,				/* the data register								*/
									(char *)(&(buf_u16[i])),/* the data to be read from the register			*/
									sizeof(uint16_t)
									); 
				}
			}
			else
			{ /* size is UINT32 or larger */
				element_total = buf_len / uc_reg_size;
				for ( i = 0; i < element_total; i = i + uc_reg_size )
				{
					for( word = 0; word < u16_total; word++ )
					{
						address = uc_reg_offset + word*2;
						/* write the address out first */
						CmpMgr_Reg_Write(	iac,
										BANK_INDEX,		/* the address register								*/
										(char *)(&address),	/* the address of the register to be written		*/
										ar_size 
										);

						/* write the data */
						CmpMgr_Reg_Read(	iac,
										BANK_DATA,						/* the data register								*/
										(char *)(&(buf_u16[i + word])),	/* the data to be read from the register			*/
										sizeof(uint16_t)
										); 
					}
				}
			} /* size is UINT32 or larger */
		} /* read in 16-bit word at a time */
	} /* read from same location 'element_count' times */

	return SUCCESS;
Bank_CMD_Read_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_BANK_NAME, __func__, __LINE__, error_code );
	return -error_code;
}

/*******************************************************************/
static int Bank_CMD_Power_Up(	component_type *	component,
									size_t				index,
									char *				buf, 
									size_t				buf_len 
								)
{
	(void)  component;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}
/*******************************************************************/
static int Bank_CMD_Power_Down(	component_type *	component,
									size_t				index,
									char *				buf, 
									size_t				buf_len 
								  )
{
	(void)  component;
	(void)	index;
	(void)	buf;
	(void)	buf_len;
	return SUCCESS;
}
/*******************************************************************/
static int Bank_CMD_Cfg_Set( component_type *	component,
								 size_t				index,		/* indicates which DI channel to obtain */
								 char *				buf, 
								 size_t				buf_len 
							   )
{
	int						error_code;
	bank_cfg_type		candidate;
	bank_dataset_type *	ds;
	size_t					register_bytes;
	size_t					aperture_bytes;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto Bank_Cfg_Set_Error;		}

	/* get transfer command from user space */
	if ( IO_COPY_FROM_USER( &candidate, buf, sizeof( candidate ) ) )
	{ error_code = EC_IO_COPY_FROM_USER;	goto Bank_Cfg_Set_Error;	 }

	ds = ( bank_dataset_type *) component->private_data;

/* perform any additional testing/verification here */
	if ( candidate.type != ds->cfg.type )
	{ error_code = EC_CMP_TYPE_TYPES_DO_NOT_MATCH;		goto Bank_Cfg_Set_Error;	}

	if ( ( candidate.address.size > sizeof( uint16_t ) ) || ( 0 == candidate.address.size )		)
	{ error_code = EC_BANK_ADDRESS_SIZE;		goto Bank_Cfg_Set_Error;	}

	if ( ( candidate.data.aperture > sizeof( uint16_t ) ) || ( 0 == candidate.address.aperture ) )
	{ error_code = EC_BANK_ADDRESS_APERTURE;	goto Bank_Cfg_Set_Error;	}

	if ( ( candidate.data.aperture > sizeof( uint16_t ) ) || ( 0 == candidate.data.aperture )	 )
	{ error_code = EC_BANK_DATA_APERTURE;		goto Bank_Cfg_Set_Error;	}

	Mem_Copy( &(ds->cfg), &candidate, sizeof( candidate ) );

	/* setup data registers */
	if ( sizeof( uint8_t ) == ds->cfg.data.size )
	{
		/* setup data lsb register */
		register_bytes = sizeof( uint8_t );
		aperture_bytes = sizeof( uint8_t );
		ds->reg_table[BANK_DATA_LSB].name = bank_reg_name[BANK_DATA_LSB];
		ds->reg_table[BANK_DATA_LSB].virtual_address = ds->cfg.data.offset_lsb;
		ds->reg_table[BANK_DATA_LSB].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																		   REG_DIR_READ_WRITE,					\
																		   REG_AUTO_INCREMENT(0),				\
																		   REG_SHADOW(0),						\
																		   REG_APERTURE_BYTES(aperture_bytes) 	\
																	     );
		/* setup data msb register */
		register_bytes = 0;
		aperture_bytes = 0;
		ds->reg_table[BANK_DATA_MSB].name = NULL;
		ds->reg_table[BANK_DATA_MSB].virtual_address = 0;
		ds->reg_table[BANK_DATA_MSB].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																		   REG_DIR_READ_WRITE,					\
																		   REG_AUTO_INCREMENT(0),				\
																		   REG_SHADOW(0),						\
																		   REG_APERTURE_BYTES(aperture_bytes) 	\
																	     );
		/* setup data register */
		register_bytes = sizeof( uint8_t );
		aperture_bytes = sizeof( uint8_t );
		ds->reg_table[BANK_DATA].name = bank_reg_name[BANK_DATA];
		ds->reg_table[BANK_DATA].virtual_address = ds->cfg.data.offset;
		ds->reg_table[BANK_DATA].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																	   REG_DIR_READ_WRITE,					\
																	   REG_AUTO_INCREMENT(0),				\
																	   REG_SHADOW(0),						\
																	   REG_APERTURE_BYTES(aperture_bytes) 	\
																	 );
	}
	else
	{
		/* setup data lsb register */
		register_bytes = sizeof( uint8_t );
		aperture_bytes = sizeof( uint8_t );
		ds->reg_table[BANK_DATA_LSB].name = bank_reg_name[BANK_DATA_LSB];
		ds->reg_table[BANK_DATA_LSB].virtual_address = ds->cfg.data.offset_lsb;
		ds->reg_table[BANK_DATA_LSB].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																		   REG_DIR_READ_WRITE,					\
																		   REG_AUTO_INCREMENT(0),				\
																		   REG_SHADOW(0),						\
																		   REG_APERTURE_BYTES(aperture_bytes) 	\
																	     );
		/* setup data msb register */
		register_bytes = sizeof( uint8_t );
		aperture_bytes = sizeof( uint8_t );
		ds->reg_table[BANK_DATA_MSB].name = bank_reg_name[BANK_DATA_MSB];
		ds->reg_table[BANK_DATA_MSB].virtual_address = ds->cfg.data.offset_lsb;
		ds->reg_table[BANK_DATA_MSB].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																		   REG_DIR_READ_WRITE,					\
																		   REG_AUTO_INCREMENT(0),				\
																		   REG_SHADOW(0),						\
																		   REG_APERTURE_BYTES(aperture_bytes) 	\
																	     );
		/* setup data register */
		register_bytes = ds->cfg.data.size;
		aperture_bytes = ds->cfg.data.aperture;
		ds->reg_table[BANK_DATA].name = bank_reg_name[BANK_DATA];
		ds->reg_table[BANK_DATA].virtual_address = ds->cfg.data.offset;
		ds->reg_table[BANK_DATA].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																	   REG_DIR_READ_WRITE,					\
																	   REG_AUTO_INCREMENT(0),				\
																	   REG_SHADOW(0),						\
																	   REG_APERTURE_BYTES(aperture_bytes) 	\
																	 );
	}
	/* setup address registers */
	if ( sizeof( uint8_t ) == ds->cfg.data.size )
	{
		/* setup address lsb register */
		register_bytes = sizeof( uint8_t );
		aperture_bytes = sizeof( uint8_t );
		ds->reg_table[BANK_INDEX_LSB].name = bank_reg_name[BANK_INDEX_LSB];
		ds->reg_table[BANK_INDEX_LSB].virtual_address = ds->cfg.address.offset_lsb;
		ds->reg_table[BANK_INDEX_LSB].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																		    REG_DIR_READ_WRITE,					\
																		    REG_AUTO_INCREMENT(0),				\
																		    REG_SHADOW(0),						\
																		    REG_APERTURE_BYTES(aperture_bytes) 	\
																	      );
		/* setup address msb register */
		register_bytes = 0;
		aperture_bytes = 0;
		ds->reg_table[BANK_INDEX_MSB].virtual_address = 0;
		ds->reg_table[BANK_INDEX_MSB].name = NULL;
		ds->reg_table[BANK_INDEX_MSB].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																		    REG_DIR_READ_WRITE,					\
																		    REG_AUTO_INCREMENT(0),				\
																		    REG_SHADOW(0),						\
																		    REG_APERTURE_BYTES(aperture_bytes) 	\
																	      );
		/* setup data register */
		register_bytes = sizeof( uint8_t );
		aperture_bytes = sizeof( uint8_t );
		ds->reg_table[BANK_INDEX].name = bank_reg_name[BANK_INDEX];
		ds->reg_table[BANK_INDEX].virtual_address = ds->cfg.address.offset;
		ds->reg_table[BANK_INDEX].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																	    REG_DIR_READ_WRITE,					\
																	    REG_AUTO_INCREMENT(0),				\
																	    REG_SHADOW(0),						\
																	    REG_APERTURE_BYTES(aperture_bytes) 	\
																	  );
	}
	else
	{
		/* setup address lsb register */
		register_bytes = sizeof( uint8_t );
		aperture_bytes = sizeof( uint8_t );
		ds->reg_table[BANK_INDEX_LSB].name = bank_reg_name[BANK_INDEX_LSB];
		ds->reg_table[BANK_INDEX_LSB].virtual_address = ds->cfg.address.offset_lsb;
		ds->reg_table[BANK_INDEX_LSB].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																		    REG_DIR_READ_WRITE,					\
																		    REG_AUTO_INCREMENT(0),				\
																		    REG_SHADOW(0),						\
																		    REG_APERTURE_BYTES(aperture_bytes) 	\
																	      );
		/* setup address msb register */
		register_bytes = sizeof( uint8_t );
		aperture_bytes = sizeof( uint8_t );
		ds->reg_table[BANK_INDEX_MSB].name = bank_reg_name[BANK_INDEX_MSB];
		ds->reg_table[BANK_INDEX_MSB].virtual_address = ds->cfg.address.offset_lsb;
		ds->reg_table[BANK_INDEX_MSB].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																		    REG_DIR_READ_WRITE,					\
																		    REG_AUTO_INCREMENT(0),				\
																		    REG_SHADOW(0),						\
																		    REG_APERTURE_BYTES(aperture_bytes) 	\
																	      );
		/* setup address register */
		register_bytes = ds->cfg.address.size;
		aperture_bytes = ds->cfg.address.aperture;
		ds->reg_table[BANK_INDEX].name = bank_reg_name[BANK_INDEX];
		ds->reg_table[BANK_INDEX].virtual_address = ds->cfg.address.offset;
		ds->reg_table[BANK_INDEX].info            = REG_CREATE_INFO( REG_WIDTH_BYTES(register_bytes),	\
																	    REG_DIR_READ_WRITE,					\
																	    REG_AUTO_INCREMENT(0),				\
																	    REG_SHADOW(0),						\
																	    REG_APERTURE_BYTES(aperture_bytes) 	\
																	  );
	}
	/* map the newly created register table to the component definition for use by reg.c */
	component->definition->register_table = ds->reg_table;
	return SUCCESS;
Bank_Cfg_Set_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_BANK_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*******************************************************************/
static int Bank_CMD_Cfg_Get( component_type *	component,
								 size_t				index,		/* indicates which DI channel to obtain */
								 char *				buf, 
								 size_t				buf_len 
							   )
{
	int  					error_code;
	bank_dataset_type *	ds;
	(void)					index;
	(void)					buf_len;

	if ( NULL == buf )
	{	error_code = EC_CMP_BUF_NULL;		goto Bank_Cfg_Get_Error;	 }

	ds = ( bank_dataset_type *) component->private_data;

	if ( IO_COPY_TO_USER( buf, &(ds->cfg), sizeof( bank_dataset_type ) ) )
	{ error_code = EC_IO_COPY_TO_USER;		goto Bank_Cfg_Get_Error;	 }

	return SUCCESS;
Bank_Cfg_Get_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_BANK_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/******************************************************************
Description.  Updates I/O all at one time.
*/
static int Bank_CMD_Update(	component_type *	component,
					size_t				index,		/* not used */
					char *				buf, 
					size_t				buf_len 
				  )
{
	size_t					byte_position;
	bank_dataset_type *	ds;
	(void)					buf;
	(void)					buf_len;

	ds = ( bank_dataset_type * ) component->private_data;



	return SUCCESS;
}
/*******************************************************************/
static int Bank_CMD_Restart( component_type *	component,
								 size_t				index,
								 char *				buf, 
								 size_t				buf_len 
							   )
{
	size_t						byte_position;
	size_t						operation;

	bank_dataset_type *	ds;
	(void)						buf_len;

	if      ( NULL == buf  ) operation = RESTART_COLD;
	else if ( 0    == *buf ) operation = RESTART_COLD;
	else  	                 operation = RESTART_WARM;

	ds = ( bank_dataset_type * ) component->private_data;
	switch( operation )
	{	/*--------------------------------*/
		case RESTART_COLD:
			Bank_CMD_Update(	component,
									index,
									buf,
									buf_len
								);
			break;
		/*--------------------------------*/
		case RESTART_WARM:
			break;
		/*--------------------------------*/
		default:  /* cold or any other */
/*			 copy intialize values from config into hardware registers.  more like a reset.*/
			break;
	}
	return SUCCESS;
}
/*****************************************************************/
#include "../../shared/mem.h"
#include "../../shared/error.h"
/*****************************************************************/
int Bank_Config_Initialize( bank_cfg_type * cfg )
{
	Mem_Set( cfg, 0, sizeof( bank_cfg_type ) );
	cfg->type = SYS_TYPE_MAKE( bank_cfg_type );
	return SUCCESS;
}
/*****************************************************************/
int Bank_Create( component_type * component, size_t instance )
{
	int						error_code;
	bank_dataset_type *	ds;
	size_t					mem_size;
	void *					mem;
	/* create dataset */
	mem_size = sizeof( bank_dataset_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_BANK_DATASET_ALLOCATE;  goto Bank_Create_Error; }
	Mem_Set( mem, 0, mem_size );
	ds = (bank_dataset_type *) mem;
	/* initialize the configuration portion */
	error_code = Bank_Config_Initialize( &(ds->cfg) );
	if ( error_code ) goto Bank_Create_Error;
	
	component->physical_id  = IOC_CREATE_PHYSICAL_ID_COMPONENT( COMPONENT_BANK, instance );
	component->offset       = 0	   /* fill in at device level */;
	component->private_data = ds;  /* assign private data */
	component->status       = component->status | CMP_STS_BUILD(0,1,0,0,0); /* private data assigned */
	
	/* create definition */
	mem_size = sizeof( component_definition_type );
	mem = Mem_Allocate( NULL, mem_size );
	if ( NULL == mem )
	{	error_code = EC_BANK_DATASET_ALLOCATE;  goto Bank_Create_Error; }
	Mem_Set( mem, 0, mem_size );
	component->definition   = (component_definition_type *) mem; 
	/* copy in most of the fixed parameters */
	Mem_Copy( component->definition, &component_bank_definition, sizeof( component_definition_type ) );

	return SUCCESS;
Bank_Create_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_BANK_NAME, __func__, __LINE__, error_code );
	return -error_code;
} 
/*****************************************************************/
int Bank_Destroy( component_type * component )
{
	int error_code;

	if ( NULL == component ) return SUCCESS;
	
	if ( NULL != component->definition ) 
	{ /* if it is null, we will just have to assume we are working with the 
	     correct data type.
	  */
		if ( SYS_TYPE_MAKE(component_definition_type) != component->definition->type )
		{  error_code = EC_BANK_COMPONENT_INVALID;	goto Bank_Destroy_Error;  }

		Mem_Free( NULL, (void **) &(component->definition) );
	}
	if ( NULL != component->private_data )
	{
		Mem_Free( NULL, &(component->private_data) );
		/*component->private_data = NULL;*/
	}
	return SUCCESS;
Bank_Destroy_Error:
	if ( error_code < 0 ) error_code = -error_code;
	Error_Log( COMPONENT_BANK_NAME, __func__, __LINE__, error_code );
	return -error_code;
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
static cmp_fnc bank_jump_table[] =
{
	COMPONENT_BANK_CMD_DEFINITIONS( CMD_CMP_EXTRACT_JUMP_TABLE )
};
/*****************************************************************/
component_definition_type component_bank_definition =
{
	SYS_TYPE_MAKE(component_definition_type),
	NULL,  /* bank_register_table, - now built at run-time */
	bank_jump_table,
	BANK_JUMP_TABLE_QTY,
#if defined( TARGET_CHECK_BUILD )
	COMPONENT_BANK_NAME,
#endif
	BANK_MODULE_REV,
	BANK_COMMAND_REV,
	BANK_STATUS_REV
};

