/*
 * spi.c
 *
 *  Created on: Aug 24, 2015
 *      Author: Mike
 */

#define SPI_FIFO_SIZE 15


int SPI_Data_Transfer_Block( int 		  board_id,
							 size_t		  object_size,  	/**< TX & RX object size: 1 <= object_size <= FIFO_SIZE */
							 size_t		  tx_object_count, 	/**< object count */
							 const void * tx_buffer,		/**< transmit buffer */
							 size_t		  rx_object_count,  /**< object count */
							 const void * rx_buffer			/**< receive buffer */
					 	   )
{
	int 	error_code;
	int		reg_symbol;
	size_t	byte_buffer_count;
	size_t	object_count;
	size_t  bp;
	size_t  byte_index;
	BOOL	active_tx, active_rx;


	if ( ( NULL == tx_buffer ) && ( NULL == rx_buffer ) )
	{
		error_code = -EC_SPI_BUFFERS_NULL;
		goto SPI_Data_Transfer_Block_Error_Exit;
	}

	active_tx = false;
	active_rx = false;
	if ( NULL == tx_buffer )
	{
		active_tx    = false;
		object_count = rx_object_count;
	}

	if ( NULL == rx_buffer )
	{
		active_rx = false;
		object_count = tx_object_count;
	}

	block_index       = 0;
	byte_buffer_count = object_size * object_count;

	/* TX_FIFO must be empty */
	/* RX_FIFO must be empty */


	switch ( board_id )
	{
		case ID_IDI48:	reg_symbol = (int) IDI_SPI_DATA;	break;
		case ID_IDO48:	reg_symbol = (int) IDO_SPI_DATA;	break;
	}

	while ( byte_buffer_count >= SPI_FIFO_SIZE )
	{
	  /* transmit */
		if ( active_tx )
		{ /* transmit real data */
			byte_index = block_index * block_size;
			for ( bp = 0; bp < SPI_FIFO_SIZE; bp++ )
			{
				IO_Write_U8( board_id, __LINE__, reg_symbol, ((uint8_t *) tx_buffer)[byte_index + bp] );
			}
		}
		else
		{
			for ( bp = 0; bp < SPI_FIFO_SIZE; bp++ )
			{
				IO_Write_U8( board_id, __LINE__, reg_symbol, 0x00 );
			}
		}

	/* read out data and wait for fifo to empty */
		byte_index = 0;
		while ( byte_index < SPI_FIFO_SIZE )
		{
			/* get status */

		}

	/* wait for transmit FIFO to empty */




		error_code = SUCCESS;
		if ( dataset->quit_application ) error_code = -EC_APP_TERMINATE_CTRLC;
		if ( Character_Get( NULL )     ) error_code = -EC_APP_TERMINATE;
		if ( SUCCESS != error_code     )
		{
			goto SPI_Data_Transfer_Block_Error_Exit;
		}




	}






SPI_Data_Transfer_Block_Error_Exit:

	return error_code;

}



int SPI_Data_Write_Read( 	int 		 board_id,
							size_t		 object_size,  		/**< TX & RX object size: 1 <= object_size <= FIFO_SIZE */
							size_t		 tx_object_count, 	/**< object count */
							const void * tx_buffer,
							size_t		 rx_object_count,  	/**< object count */
							const void * rx_buffer
	)
{
	int			error_code;
	uint8_t *	byte_ptr;
	size_t 		byte_buffer_count;
	size_t		byte_buffer_index;

	byte_buffer_index = 0;
	byte_buffer_count = object_size * tx_object_count;


	while ( byte_buffer_count >= SPI_FIFO_SIZE )
	{

		error_code = SPI_Data_Transfer_Block( 	board_id, 			/* board 							*/
												sizeof( uint8_t ), 	/* object size 						*/
												SPI_FIFO_SIZE,		/* number of objects to transfer 	*/
												0,					/* transmit buffer size				*/
												NULL,				/* transmit buffer 					*/
												SPI_FIFO_SIZE,		/* receive buffer size 				*/
												rx_buffer			/* receive buffer 					*/
						 	 	 	  	  	  );
		byte_buffer_count -= SPI_FIFO_SIZE;
	}



}

