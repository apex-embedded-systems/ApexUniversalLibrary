
typedef enum
{
	AUL_TEST_READY			= 0, /* AUL_EC_SUCCESS / AUL_EC_TEST_INVALID */
	AUL_TEST_IO				= 1, /* AUL_EC_SUCCESS / AUL_EC_TEST_IO_FAIL */
	AUL_TEST_INTERRUPT		= 2, /* AUL_EC_SUCCESS / AUL_EC_TEST_INT_FAIL_BOARD _INT_FAIL_CALLBACK */
	AUL_TEST_ALL			= 3, /* see above */
} AUL_TEST_ENUM;

AUL_Device_Diagnostic_Test( int board, AUL_TEST_IO );
AUL_Device_Diagnostic_Base_Address_Get( int board, UINT32 * address );
AUL_Device_Diagnostic_IRQ_Get( int board, UINT32 * irq );
AUL_Device_Diagnostic_Name_Get( int board, char * name );


/***************************************************************************/
/*
*/
typedef struct AUL_EC_ENUM_STR_TYPE
{
	STX104_EC_ENUM		ec;
	char *				str;	
} AUL_EC_ENUM;
/************************************/
static AUL_EC_ENUM stx104_ec_enum_info[] =
{
	{	AUL_EC_SUCCESS                                   				, "AUL_EC_SUCCESS"               							},
	{	AUL_EC_OPEN_FAILED                                      , "AUL_EC_OPEN_FAILED" 												},
	{	AUL_EC_OPEN_MAXIMUM_DEVICES_OPEN                        , "AUL_EC_OPEN_MAXIMUM_DEVICES_OPEN" 					},
	{	AUL_EC_OPEN_STX104_MNEMONIC_NOT_FOUND                   , "AUL_EC_OPEN_STX104_MNEMONIC_NOT_FOUND" 		},
	{	AUL_EC_OPEN_DEVICE_INCORRECT_FIRMWARE_VERSION           , "AUL_EC_OPEN_DEVICE_INCORRECT_FIRMWARE_VERSION" },
	{	AUL_EC_HW_DEFINITION_NO_DEVICES_FOUND						  			, "AUL_EC_HW_DEFINITION_NO_DEVICES_FOUND" 		},
	{	AUL_EC_AULDRV_H__INCORRECT_VERSION   						  			, "AUL_EC_AULDRV_H__INCORRECT_VERSION" 				},
	{	AUL_EC_AULDRV_SYS_VERSION_TEST_ERROR						  			, "AUL_EC_AULDRV_SYS_VERSION_TEST_ERROR" 			},

	{	AUL_EC_CLOSE_DEVICE_NOT_OPEN                            , "AUL_EC_CLOSE_DEVICE_NOT_OPEN" 									},
	{	AUL_EC_CLOSE_FAILED_TO_DISABLE_INTERRUPT                , "AUL_EC_CLOSE_FAILED_TO_DISABLE_INTERRUPT" 			},
	{	AUL_EC_CLOSE_FAILED_TO_DEALLOCATE_DEVICE                , "AUL_EC_CLOSE_FAILED_TO_DEALLOCATE_DEVICE" 			},
	{	AUL_EC_CLOSE_HANDLE_OUT_OF_RANGE                        , "AUL_EC_CLOSE_HANDLE_OUT_OF_RANGE" 							},

	{	AUL_EC_INIT_HW_CANNOT_FIND_DEFINITION_FILE              , "AUL_EC_INIT_HW_CANNOT_FIND_DEFINITION_FILE" 		},
	{	AUL_EC_INIT_HW_DEFINITION_FILE_LOAD                     , "AUL_EC_INIT_HW_DEFINITION_FILE_LOAD" 					},
	{	AUL_EC_INIT_HW_DEFINITION_COLUMN_NAME_MISSING           , "AUL_EC_INIT_HW_DEFINITION_COLUMN_NAME_MISSING" },
	{	AUL_EC_INIT_HW_DEFINITION_COLUMN_NAME_UNKNOWN           , "AUL_EC_INIT_HW_DEFINITION_COLUMN_NAME_UNKNOWN" },
	{	AUL_EC_INIT_HW_DEFINITION_IRQ_OUT_OF_RANGE              , "AUL_EC_INIT_HW_DEFINITION_IRQ_OUT_OF_RANGE" },
	{	AUL_EC_INIT_HW_DEFINITION_BASE_ADDRESS_OUT_OF_RANGE     , "AUL_EC_INIT_HW_DEFINITION_BASE_ADDRESS_OUT_OF_RANGE" },
	{	AUL_EC_INIT_HW_DEFINITION_TYPE_UNDEFINED					  		, "AUL_EC_INIT_HW_DEFINITION_TYPE_UNDEFINED" 	},
	{	AUL_EC_INIT_HW_DEFINITION_MNEMONIC_ERROR                , "AUL_EC_INIT_HW_DEFINITION_MNEMONIC_ERROR" 	},

	{	AUL_EC_INIT_LIB_SYSTEM_INTERNAL_ERROR                   , "AUL_EC_INIT_LIB_SYSTEM_INTERNAL_ERROR" 		},
	{	AUL_EC_INIT_LIB_DEBUG_ERROR                             , "AUL_EC_INIT_LIB_DEBUG_ERROR" 							},
	{	AUL_EC_INIT_LIB_ERROR                                   , "AUL_EC_INIT_LIB_ERROR" 										},

	{	AUL_EC_TERMINATION_CLOSE_ERROR                          , "AUL_EC_TERMINATION_CLOSE_ERROR" 						},

	{	AUL_EC_BOARD_NOT_FOUND                                  , "AUL_EC_BOARD_NOT_FOUND" 										},

	{	AUL_EC_TEST_FAIL_IO						  						  					, "AUL_EC_TEST_FAIL_IO" 											},
	{	AUL_EC_TEST_FAIL_INTERRUPT_BOARD		  						  		, "AUL_EC_TEST_FAIL_INTERRUPT_BOARD" 					},
	{	AUL_EC_TEST_FAIL_INTERRUPT_DRIVER   						  			, "AUL_EC_TEST_FAIL_INTERRUPT_DRIVER" 				},
	{	AUL_EC_TEST_FAIL_INTERRUPT_APPLICATION						  		, "AUL_EC_TEST_FAIL_INTERRUPT_APPLICATION" 		}
};

/*****************************************************/
/* Apex Universal Library general error codes. These error codes
   apply to all functions named with a "AUL_" prefix.
   
   AUL = Apex Universal Library.
   
   EC = Error Code.                                              */
typedef enum 
{
	AUL_EC_SUCCESS                                          = 0x0000,
	
	AUL_EC_OPEN_FAILED                                      = 0x0010,
	AUL_EC_OPEN_MAXIMUM_DEVICES_OPEN                        = 0x0011,
	AUL_EC_OPEN_STX104_MNEMONIC_NOT_FOUND                   = 0x0012,
	AUL_EC_OPEN_DEVICE_INCORRECT_FIRMWARE_VERSION           = 0x0013,
	AUL_EC_HW_DEFINITION_NO_DEVICES_FOUND						  = 0x0014,
   AUL_EC_AULDRV_H__INCORRECT_VERSION   						  = 0x0015,
   AUL_EC_AULDRV_SYS_VERSION_TEST_ERROR						  = 0x0016,
	
	AUL_EC_CLOSE_DEVICE_NOT_OPEN                            = 0x0020,
	AUL_EC_CLOSE_FAILED_TO_DISABLE_INTERRUPT                = 0x0021,
	AUL_EC_CLOSE_FAILED_TO_DEALLOCATE_DEVICE                = 0x0022,
	AUL_EC_CLOSE_HANDLE_OUT_OF_RANGE                        = 0x0023,
	
	AUL_EC_INIT_HW_CANNOT_FIND_DEFINITION_FILE              = 0x0030,
	AUL_EC_INIT_HW_DEFINITION_FILE_LOAD                     = 0x0031,
	AUL_EC_INIT_HW_DEFINITION_COLUMN_NAME_MISSING           = 0x0032,
	AUL_EC_INIT_HW_DEFINITION_COLUMN_NAME_UNKNOWN           = 0x0033,
	AUL_EC_INIT_HW_DEFINITION_IRQ_OUT_OF_RANGE              = 0x0034,
	AUL_EC_INIT_HW_DEFINITION_BASE_ADDRESS_OUT_OF_RANGE     = 0x0035,
	AUL_EC_INIT_HW_DEFINITION_TYPE_UNDEFINED					  = 0x0036,
	AUL_EC_INIT_HW_DEFINITION_MNEMONIC_ERROR                = 0x0037,

	AUL_EC_INIT_LIB_SYSTEM_INTERNAL_ERROR                   = 0x0040,
	AUL_EC_INIT_LIB_DEBUG_ERROR                             = 0x0041,
	AUL_EC_INIT_LIB_ERROR                                   = 0x0042,

	AUL_EC_TERMINATION_CLOSE_ERROR                          = 0x0050,
	
	AUL_EC_BOARD_NOT_FOUND                                  = 0x0060,

   AUL_EC_TEST_FAIL_IO						  						  = 0x0070,
   AUL_EC_TEST_FAIL_INTERRUPT_BOARD		  						  = 0x0070, /* failed to generate at board level */
   AUL_EC_TEST_FAIL_INTERRUPT_DRIVER   						  = 0x0070, /* failed at driver level */
   AUL_EC_TEST_FAIL_INTERRUPT_APPLICATION						  = 0x0070, /* failed at application level */
   AUL_EC_TEST_														  = 0x0070,

} AUL_EC_ENUM;


/***************************************************************************/
int STX104_Diagnostic( int board, char * prefix, FILE * out )
{
	int            board;
	AUL_EC_ENUM    ec_driver;  /* "ec_" = error code.  Used for debugging */

   /**** Determine if we are connected to the driver ****/


   /**** Determine if the board is actually opened  ****/
//TODO: new DLL command
   ec_driver = AUL_Device_Is_Open( board );
   if ( AUL_EC_SUCCESS != ec_driver ) goto STX104_Diagnostic_Error_Handler;

	/**** Produce a report ****/
	STX104_Report_Status( board, out, prefix );

	/**** Perform diagnostics  ****/
//TODO: new DLL command
   ec_driver = AUL_Device_Test( board, AUL_TEST_ALL );
   if ( AUL_EC_SUCCESS != ec_driver ) goto STX104_Diagnostic_Error_Handler;

	return AUL_EC_SUCCESS;

STX104_Diagnostic_Error_Handler:
	/* report errors here - maybe jumpers or BIOS has been set incorrectly? */
   fprintf( out_file, "%s  -> Error: %s\n", prefix, AUL_Report_Error_Code( ec_driver ) );
   switch( ec_driver )
   {
   	case AUL_EC_TEST_FAIL_IO:
       fprintf( out_file, "%s  -> Check base address jumpers and BIOS config.\n" );
       fprintf( out_file, "%s  -> Does STX104 LED blink?\n" );
       break;
      case AUL_EC_TEST_FAIL_INTERRUPT_BOARD:
       fprintf( out_file, "%s  -> Board level interrupt failed, verify jumpers.\n" );
       break;
    	case AUL_EC_TEST_FAIL_INTERRUPT_DRIVER:
       fprintf( out_file, "%s  -> Driver level interrupt failed, verify BIOS config.\n" );
       break;
    	case AUL_EC_TEST_FAIL_INTERRUPT_APPLICATION:
       fprintf( out_file, "%s  -> Application level interrupt failed, verify BIOS config.\n" );
       break;
   }
	return (int) ec_driver;
}

/***************************************************************************/
int AUL_Diagnostic( char * board_name, char * prefix, FILE * out )
{

//inside driver this would be a whole lot easier!
	const char hwdef[] =
   {
   	"MNEMONIC, TYPE, BASE_ADDRESS, IRQ\n",
		"mySTX104, stx104, 0x300, 7\n"
   };

   /**** connect to driver ****/
	ec_driver = AUL_Initialization( hwdef );
	if ( ec_driver != AUL_EC_SUCCESS )
	{
		/* NOTE:
			Possible error conditions:
			1. Cannot find the hardware definition file
			2. Error within the hardware definition file
			3. Kernel portion of the driver not present in the system.
		*/
		fprintf( out_file, "%s  -> %s  Return code 0x%04X\n", prefix, AUL_Last_Error_Get(), (int) ec_driver );
		return( (int) ec_driver );
	}


   /**** open board  *****/

   

   if ( AUL_TYPE_STX104 != type )
   {
   	/**** close board            ****/
   	/**** disconnect from driver ****/
      /* message */
      return WRONG_BOARD_TYPE;
   }


   error_code = STX104_Diagnostic( board, prefix, out );
   if ( AUL_EC_SUCCESS != error_code )
   {
   	/**** close board            ****/
   	/**** disconnect from driver ****/
      /* message */
      return error_code;
   }

   return AUL_EC_SUCCESS;
}

