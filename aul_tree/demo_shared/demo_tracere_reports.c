
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

	COPYRIGHT NOTICE Copyright (c) 2014-2015 by Apex Embedded Systems. All rights
	reserved. This document is the confidential property of Apex Embedded
	Systems Any reproduction or dissemination is prohibited unless specifically
	authorized in writing.

_____________________________________________________________________________
License:

_____________________________________________________________________________
TODO:


*********************************************************************************/

#include "demo_tracere_reports.h"
#include "../aul/aul_public.h"
#include <stddef.h> /* NULL */
#include <time.h>   /* time_t, time(), ctime() */

/***************************************************************************/

static const char * bool_str[] = { "false", "true " 	};  /* value ? bl[1] : bl[0].  if value then print bl[1] else print bl[0] */
//static const char * jmp_str[]  = { "open", "installed" 	};  /* value ? bl[1] : bl[0].  if value then print bl[1] else print bl[0] */




/*******************************************************************//**
 *
 */
void Tracere_Report_Prefix( FILE * out, struct report_prefix_list * list )
{
	if ( NULL == list         ) return;
	if ( NULL == list->prefix ) return;
	do
	{
		fprintf( out, "%s", list->prefix );
		list = list->next;
	} while ( NULL != list );
}
/*******************************************************************//**
 *
 */
int I8255M0_Report_Cfg( i8255m0_cfg_type * cfg, FILE * out, struct report_prefix_list * list )
{
	int index;
	if ( SYS_TYPE_MAKE( i8255m0_cfg_type ) != cfg->type ) return -EC_APP_TYPE_INVALID;
	Tracere_Report_Prefix( out, list );
	fprintf( out, "simultaneous_update           = %s\n", ( cfg->simultaneous_update ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "port_directions               = " );
	switch( cfg->port_directions )
	{
		case I8255M0_PDIR_AIN__BIN__CIN:	fprintf( out, "I8255M0_PDIR_AIN__BIN__CIN"  );	break;
		case I8255M0_PDIR_AIN__BIN__COUT:	fprintf( out, "I8255M0_PDIR_AIN__BIN__COUT" );	break;
		case I8255M0_PDIR_AIN__BOUT_CIN:	fprintf( out, "I8255M0_PDIR_AIN__BOUT_CIN"  );	break;
		case I8255M0_PDIR_AIN__BOUT_COUT:	fprintf( out, "I8255M0_PDIR_AIN__BOUT_COUT" );	break;
		case I8255M0_PDIR_AOUT_BIN__CIN:	fprintf( out, "I8255M0_PDIR_AOUT_BIN__CIN"  );	break;
		case I8255M0_PDIR_AOUT_BIN__COUT:	fprintf( out, "I8255M0_PDIR_AOUT_BIN__COUT" );	break;
		case I8255M0_PDIR_AOUT_BOUT_CIN:	fprintf( out, "I8255M0_PDIR_AOUT_BOUT_CIN"  );	break;
		case I8255M0_PDIR_AOUT_BOUT_COUT:	fprintf( out, "I8255M0_PDIR_AOUT_BOUT_COUT" );	break;
	};
	fprintf( out, "\n" );

	for ( index = 0; index < I8255M0_PORT_QTY; index++ )
	{
	Tracere_Report_Prefix( out, list );
	fprintf( out, "port[%d].polarity              = 0x%02X\n", index, cfg->port[index].polarity   );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "port[%d].init_value            = 0x%02X\n", index, cfg->port[index].init_value );
	}

	return SUCCESS;
}



/**********************************************************************************************************//**
@ingroup i8254
\brief
Apex Universal Library TracerE specific function that retrieves the two I8255M0 or digital I/O configuration information.
\param[in] 	cxfr		i8254 counter/timer transfer command.
\param[out] out  		The destination of the report.  Can be 'stdout' or any file.
						If NULL, then routes to debug output.
\param[in] list  		Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int I8254_Report_Cxfr( i8254_ct_xfr_type * cxfr, FILE * out, struct report_prefix_list * list )
{
	int index;

	if ( SYS_TYPE_MAKE( i8254_ct_xfr_type ) != cxfr->type ) return -EC_APP_TYPE_INVALID;

	//Report_Write( out, list, "error_code                    = %s\n",     AUL_Error_String( cxfr->error_code ) );

	Tracere_Report_Prefix( out, list );
	fprintf( out, "error_code                    = %s\n",     AUL_Error_String( cxfr->error_code ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "control                       = 0x%02X\n", cxfr->control                        );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "count_write                   = %d\n",     cxfr->count_write                    );

	for ( index = 0; index < I8254_CT_QTY; index++ )
	{
		Tracere_Report_Prefix( out, list );
		fprintf( out, "count_read[%d]                = %d\n",     index, cxfr->count_read[index]          );
		Tracere_Report_Prefix( out, list );
		fprintf( out, "status_read{%d]               = 0x%02X\n", index, cxfr->status_read[index]         );
	}

	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup i8254
\brief
Apex Universal Library TracerE specific function that retrieves the two I8255M0 or digital I/O configuration information.

\param[in] board 		The descriptor obtained from the function ::AUL_Device_Open.
\param[out] out  		The destination of the report.  Can be 'stdout' or any file.
						If NULL, then routes to debug output.
\param[in] tracere_led 	This is the data from which to produce the report.
\param[in] list  		Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int I8254_Report_Cfg( i8254_cfg_type * cfg, FILE * out, struct report_prefix_list * list )
{
	int error_code;
	int index;
	if ( SYS_TYPE_MAKE( i8254_cfg_type ) != cfg->type ) return -EC_APP_TYPE_INVALID;

	for ( index = 0; index < I8254_CT_QTY; index++ )
	{
		error_code = I8254_Report_Cxfr( &(cfg->cxfr[index]), out, list );
		if ( SUCCESS != error_code ) return error_code;
	}
	return SUCCESS;
}



/**********************************************************************************************************//**
@ingroup tracere_report
\brief
Apex Universal Library TracerE specific function that retrieves the two I8255M0 or digital I/O configuration information.

\param[in] board 		The descriptor obtained from the function ::AUL_Device_Open.
\param[out] out  		The destination of the report.  Can be 'stdout' or any file.
\param[in] tracere_led 	This is the data from which to produce the report.
\param[in] list  		Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int Tracere_Report_LED( 	int 	board,
							enum 	tracere_led led,
							FILE * 	out,
							struct report_prefix_list * list
				   	   )
{
	Tracere_Report_Prefix( out, list );
	fprintf( out, "LED                           = " );
	switch( led )
	{
		case TRACERE_LED_BASE_ADDRESS:			fprintf( out, "TRACERE_LED_BASE_ADDRESS" );		break;
		case TRACERE_LED_CT0_INPUT:			 	fprintf( out, "TRACERE_LED_CT0_INPUT" );		break;
		case TRACERE_LED_CT1_INPUT:			 	fprintf( out, "TRACERE_LED_CT1_INPUT" );		break;
		case TRACERE_LED_CT2_INPUT:			 	fprintf( out, "TRACERE_LED_CT2_INPUT" );		break;
		case TRACERE_LED_CT0_OUTPUT:			fprintf( out, "TRACERE_LED_CT0_OUTPUT" );		break;
		case TRACERE_LED_CT1_OUTPUT:			fprintf( out, "TRACERE_LED_CT1_OUTPUT" );		break;
		case TRACERE_LED_CT2_OUTPUT:			fprintf( out, "TRACERE_LED_CT2_OUTPUT" );		break;
		case TRACERE_LED_INTERRUPT_SRCA:	 	fprintf( out, "TRACERE_LED_INTERRUPT_SRCA" );	break;
		case TRACERE_LED_IRQ_IN0:				fprintf( out, "TRACERE_LED_IRQ_IN0" );			break;
		case TRACERE_LED_IRQ_IN1:				fprintf( out, "TRACERE_LED_IRQ_IN1" );			break;
		case TRACERE_LED_IRQ_IN2:				fprintf( out, "TRACERE_LED_IRQ_IN2" );			break;
		case TRACERE_LED_NONE:				 	fprintf( out, "TRACERE_LED_NONE" );				break;
		case TRACERE_LED_INTERRUPT_SRCB:		fprintf( out, "TRACERE_LED_INTERRUPT_SRCB" );	break;
		case TRACERE_LED_INTERRUPT_SRCC:		fprintf( out, "TRACERE_LED_INTERRUPT_SRCC" );	break;
		case TRACERE_LED_OFF:					fprintf( out, "TRACERE_LED_OFF" );				break;
		case TRACERE_LED_ON:					fprintf( out, "TRACERE_LED_ON" );				break;
	}
	fprintf( out, "\n" );

	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup tracere_report
\brief
Apex Universal Library TracerE specific function that retrieves the two I8255M0 or digital I/O configuration information.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] out  The destination of the report.  Can be 'stdout' or any file.
\param[in] cfg   This is the data structure from which we wish to produce a report.
\param[in] list  Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int Tracere_Report_DIO_Cfg( 	int 		board,
								void *		info,
								FILE *		out,
								struct report_prefix_list * list
				   	   	   )
{
	int 						error_code;
	int 						index;
	tracere_dio_cfg_type * 		cfg;
	char 						prefix[] = "  ";
	struct report_prefix_list  	list1;
	(void)						board;

	error_code   = SUCCESS;

	cfg = ( tracere_dio_cfg_type * ) info;
	if ( SYS_TYPE_MAKE( tracere_dio_cfg_type ) != cfg->type ) return -EC_APP_TYPE_INVALID;

	list1.next   = NULL;
	list1.prefix = prefix;
	for ( index = 0; index < TRACERE_I8255M0_QTY; index++ )
	{
		Tracere_Report_Prefix( out, list );
		fprintf( out, "I8255M0[%d] config:\n", index );
		list->next = &list1; /* pass additional indent */
		error_code = I8255M0_Report_Cfg( &(cfg->i8255m0_cfg[index]), out, list );
		list->next = NULL; /* remove additional indent */
		if ( SUCCESS != error_code ) break;
	}
	list->next   = NULL; /* remove additional indent */
	return error_code;
}

/**********************************************************************************************************//**
@ingroup tracere_report
\brief
Apex Universal Library TracerE specific function that retrieves the I8254 configuration information.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] out  The destination of the report.  Can be 'stdout' or any file.
\param[in] cfg   This is the data structure from which we wish to produce a report.
\param[in] list  Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int Tracere_Report_CT_Cfg( 		int 						board,
								tracere_ct_cfg_type *		cfg,
								FILE *						out,
								struct report_prefix_list * list
				   	   	   )
{
	char 						prefix[] = "  ";
	struct report_prefix_list  	list1;
	(void)						board;

	if ( SYS_TYPE_MAKE( tracere_ct_cfg_type ) != cfg->type ) return -EC_APP_TYPE_INVALID;

	list1.next   = NULL;
	list1.prefix = prefix;

	/* CT0 */
	Tracere_Report_Prefix( out, list );
	fprintf( out, "I8254 C/T[0] config:\n" );
	list->next = &list1; /* pass additional indent */
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct0.clock_invert              = %s\n", ( cfg->ct0.clock_invert ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct0.gate_invert               = %s\n", ( cfg->ct0.gate_invert ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct0.clock_source              = " );
	switch( cfg->ct0.clock_source )
	{
		case TRACERE_CLK_CT0_CT_IN0:	fprintf( out, "TRACERE_CLK_CT0_CT_IN0" );	break;
		case TRACERE_CLK_CT0_CT_IN1:	fprintf( out, "TRACERE_CLK_CT0_CT_IN1" );	break;
		case TRACERE_CLK_CT0_CT_OUT2:	fprintf( out, "TRACERE_CLK_CT0_CT_OUT2");	break;
		case TRACERE_CLK_CT0_IRQ_IN2:	fprintf( out, "TRACERE_CLK_CT0_IRQ_IN2");	break;
		case TRACERE_CLK_CT0_4_MHZ:		fprintf( out, "TRACERE_CLK_CT0_4_MHZ"  );	break;
		case TRACERE_CLK_CT0_10_MHZ:	fprintf( out, "TRACERE_CLK_CT0_10_MHZ" );	break;
		case TRACERE_CLK_CT0_20_MHZ:	fprintf( out, "TRACERE_CLK_CT0_20_MHZ" );	break;
		case TRACERE_CLK_CT0_40_MHZ:	fprintf( out, "TRACERE_CLK_CT0_40_MHZ" );	break;
	};
	fprintf( out, "\n" );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct0.gate_source               = " );
	switch( cfg->ct0.gate_source )
	{
		case TRACERE_GATE_CT0_CT_GATE0:	fprintf( out, "TRACERE_GATE_CT0_CT_GATE0" );	break;
		case TRACERE_GATE_CT0_CT_GATE1:	fprintf( out, "TRACERE_GATE_CT0_CT_GATE1" );	break;
	};
	fprintf( out, "\n" );

	/* CT1 */
	list->next = NULL; /* remove additional indent */
	Tracere_Report_Prefix( out, list );
	fprintf( out, "I8254 C/T[1] config:\n" );
	list->next = &list1; /* pass additional indent */
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct1.clock_invert              = %s\n", ( cfg->ct1.clock_invert ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct1.gate_invert               = %s\n", ( cfg->ct1.gate_invert ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct1.clock_source              = " );
	switch( cfg->ct1.clock_source )
	{
		case TRACERE_CLK_CT1_CT_IN1:	fprintf( out, "TRACERE_CLK_CT1_CT_IN1" );	break;
		case TRACERE_CLK_CT1_CT_OUT0:	fprintf( out, "TRACERE_CLK_CT1_CT_OUT0");	break;
		case TRACERE_CLK_CT0_CT_OUT2:	fprintf( out, "TRACERE_CLK_CT0_CT_OUT2");	break;
		case TRACERE_CLK_CT0_IRQ_IN2:	fprintf( out, "TRACERE_CLK_CT0_IRQ_IN2");	break;
		case TRACERE_CLK_CT0_4_MHZ:		fprintf( out, "TRACERE_CLK_CT0_4_MHZ"  );	break;
		case TRACERE_CLK_CT0_10_MHZ:	fprintf( out, "TRACERE_CLK_CT0_10_MHZ" );	break;
		case TRACERE_CLK_CT0_20_MHZ:	fprintf( out, "TRACERE_CLK_CT0_20_MHZ" );	break;
		case TRACERE_CLK_CT0_40_MHZ:	fprintf( out, "TRACERE_CLK_CT0_40_MHZ" );	break;
	};
	fprintf( out, "\n" );

	/* CT2 */
	list->next = NULL; /* remove additional indent */
	Tracere_Report_Prefix( out, list );
	fprintf( out, "I8254 C/T[2] config:\n" );
	list->next = &list1; /* pass additional indent */
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct2.clock_invert              = %s\n", ( cfg->ct2.clock_invert ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct2.gate_invert               = %s\n", ( cfg->ct2.gate_invert ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "ct2.clock_source              = " );
	switch( cfg->ct2.clock_source )
	{
		case TRACERE_CLK_CT2_CT_IN0:	fprintf( out, "TRACERE_CLK_CT2_CT_IN0" );	break;
		case TRACERE_CLK_CT2_CT_IN1:	fprintf( out, "TRACERE_CLK_CT2_CT_IN1" );	break;
		case TRACERE_CLK_CT0_CT_OUT2:	fprintf( out, "TRACERE_CLK_CT0_CT_OUT2");	break;
		case TRACERE_CLK_CT0_IRQ_IN2:	fprintf( out, "TRACERE_CLK_CT0_IRQ_IN2");	break;
		case TRACERE_CLK_CT0_4_MHZ:		fprintf( out, "TRACERE_CLK_CT0_4_MHZ"  );	break;
		case TRACERE_CLK_CT0_10_MHZ:	fprintf( out, "TRACERE_CLK_CT0_10_MHZ" );	break;
		case TRACERE_CLK_CT0_20_MHZ:	fprintf( out, "TRACERE_CLK_CT0_20_MHZ" );	break;
		case TRACERE_CLK_CT0_40_MHZ:	fprintf( out, "TRACERE_CLK_CT0_40_MHZ" );	break;
	};
	fprintf( out, "\n" );
	list->next = NULL; /* remove additional indent */
	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup tracere_report
\brief
Apex Universal Library TracerE specific function that retrieves the interrupt configuration information.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] out  The destination of the report.  Can be 'stdout' or any file.
\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] out  The destination of the report.  Can be 'stdout' or any file.
\param[in] cfg   This is the data structure from which we wish to produce a report.
\param[in] list  Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int Tracere_Report_Intr_Cfg( 	int 						board,
								tracere_intr_cfg_type *		cfg,
								FILE *						out,
								struct report_prefix_list * list
				   	   	   )
{
	char 						prefix[] = "  ";
	struct report_prefix_list  	list1;
	(void)						board;

	if ( SYS_TYPE_MAKE( tracere_intr_cfg_type ) != cfg->type ) return -EC_APP_TYPE_INVALID;

	list1.next   = NULL;
	list1.prefix = prefix;

	list->next = NULL; /* remove additional indent */
	Tracere_Report_Prefix( out, list );
	fprintf( out, "Interrupt A config:\n" );
	list->next = &list1; /* pass additional indent */

	Tracere_Report_Prefix( out, list );
	fprintf( out, "enable_a                      = %s\n", ( cfg->enable_a ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "source_a                      = " );
	switch( cfg->source_a )
	{
		case TRACERE_ISA_CT_OUT0:	fprintf( out, "TRACERE_ISA_CT_OUT0"  );	break;
		case TRACERE_ISA_CT_OUT1:	fprintf( out, "TRACERE_ISA_CT_OUT1"  );	break;
		case TRACERE_ISA_CT_OUT2:	fprintf( out, "TRACERE_ISA_CT_OUT2"  );	break;
		case TRACERE_ISA_IRQ_IN0:	fprintf( out, "TRACERE_ISA_IRQ_IN0"  );	break;
		case TRACERE_ISA_IRQ_IN1:	fprintf( out, "TRACERE_ISA_IRQ_IN1"  );	break;
		case TRACERE_ISA_IRQ_IN2:	fprintf( out, "TRACERE_ISA_IRQ_IN2"  );	break;
		case TRACERE_ISA_8255_1_C0:	fprintf( out, "TRACERE_ISA_8255_1_C0"); break;
		case TRACERE_ISA_8255_2_C0:	fprintf( out, "TRACERE_ISA_8255_2_C0");	break;
	};
	fprintf( out, "\n" );


	list->next = NULL; /* remove additional indent */
	Tracere_Report_Prefix( out, list );
	fprintf( out, "Interrupt B config:\n" );
	list->next = &list1; /* pass additional indent */

	Tracere_Report_Prefix( out, list );
	fprintf( out, "enable_b                      = %s\n", ( cfg->enable_b ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "source_b                      = " );
	switch( cfg->source_b )
	{
	case TRACERE_ISB_CT_OUT0:	fprintf( out, "TRACERE_ISB_CT_OUT0"  );	break;
	case TRACERE_ISB_CT_OUT1:	fprintf( out, "TRACERE_ISB_CT_OUT1"  );	break;
	case TRACERE_ISB_CT_OUT2:	fprintf( out, "TRACERE_ISB_CT_OUT2"  );	break;
	case TRACERE_ISB_IRQ_IN0:	fprintf( out, "TRACERE_ISB_IRQ_IN0"  );	break;
	case TRACERE_ISB_IRQ_IN1:	fprintf( out, "TRACERE_ISB_IRQ_IN1"  );	break;
	case TRACERE_ISB_IRQ_IN2:	fprintf( out, "TRACERE_ISB_IRQ_IN2"  );	break;
	case TRACERE_ISB_8255_1_C0:	fprintf( out, "TRACERE_ISB_8255_1_C0"); break;
	case TRACERE_ISB_8255_2_C0:	fprintf( out, "TRACERE_ISB_8255_2_C0");	break;
	};
	fprintf( out, "\n" );

	list->next = NULL; /* remove additional indent */
	Tracere_Report_Prefix( out, list );
	fprintf( out, "Interrupt C config:\n" );
	list->next = &list1; /* pass additional indent */

	Tracere_Report_Prefix( out, list );
	fprintf( out, "enable_c                      = %s\n", ( cfg->enable_c ? bool_str[1] : bool_str[0] ) );
	Tracere_Report_Prefix( out, list );
	fprintf( out, "source_c                      = " );
	switch( cfg->source_c )
	{
	case TRACERE_ISC_CT_OUT0:	fprintf( out, "TRACERE_ISC_CT_OUT0"  );	break;
	case TRACERE_ISC_CT_OUT1:	fprintf( out, "TRACERE_ISC_CT_OUT1"  );	break;
	case TRACERE_ISC_CT_OUT2:	fprintf( out, "TRACERE_ISC_CT_OUT2"  );	break;
	case TRACERE_ISC_IRQ_IN0:	fprintf( out, "TRACERE_ISC_IRQ_IN0"  );	break;
	case TRACERE_ISC_IRQ_IN1:	fprintf( out, "TRACERE_ISC_IRQ_IN1"  );	break;
	case TRACERE_ISC_IRQ_IN2:	fprintf( out, "TRACERE_ISC_IRQ_IN2"  );	break;
	case TRACERE_ISC_8255_1_C0:	fprintf( out, "TRACERE_ISC_8255_1_C0"); break;
	case TRACERE_ISC_8255_2_C0:	fprintf( out, "TRACERE_ISC_8255_2_C0");	break;
	};
	fprintf( out, "\n" );

	list->next = NULL; /* remove additional indent */

	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup tracere_report
\brief
Apex Universal Library TracerE specific function that retrieves the board level configuration information.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] out  The destination of the report.  Can be 'stdout' or any file.
\param[in] cfg   This is the data structure from which we wish to produce a report.
\param[in] list  Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int Tracere_Report_Cfg( 	int 						board,
							tracere_cfg_type *			cfg,
							FILE *						out,
							struct report_prefix_list * list
				   	   )
{
	int error_code;

	if ( SYS_TYPE_MAKE( tracere_cfg_type ) != cfg->type ) return -EC_APP_TYPE_INVALID;

	error_code = Tracere_Report_LED( board, cfg->led, out, list );
	if ( SUCCESS != error_code ) return error_code;

	error_code = Tracere_Report_DIO_Cfg( board, &(cfg->dio), out, list );
	if ( SUCCESS != error_code ) return error_code;

	error_code = Tracere_Report_CT_Cfg( board, &(cfg->ct), out, list );
	if ( SUCCESS != error_code ) return error_code;

	error_code = Tracere_Report_Intr_Cfg( board, &(cfg->intr), out, list );
	if ( SUCCESS != error_code ) return error_code;

	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup tracere_report
\brief
Apex Universal Library TracerE specific function that produces a report based on the type of structure passed.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[in] info  This is the data structure from which we wish to produce a report.  Note that all data structures
                  have a 'type' parameter which indicates its intended use.  This function searches the types and
                  launches the matching report mechanism.
\param[out] out  The destination of the report.  Can be 'stdout' or any file.
\param[in] list  Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int Tracere_Report( 	int 						board,
						void *						info,
						FILE *						out,
						struct report_prefix_list * list
				   )
{
	int error_code;
	error_code = SUCCESS;
	switch ( *( ( SYS_TYPE *) info ) )
	{
		case SYS_TYPE_MAKE( tracere_cfg_type      ):	error_code = Tracere_Report_Cfg( 	 board, (tracere_cfg_type *) info, 		out, list ); break;
		case SYS_TYPE_MAKE( tracere_dio_cfg_type  ):	error_code = Tracere_Report_DIO_Cfg( board, (tracere_dio_cfg_type *) info, 	out, list ); break;
		case SYS_TYPE_MAKE( tracere_ct_cfg_type   ):	error_code = Tracere_Report_CT_Cfg(  board, (tracere_ct_cfg_type *) info, 	out, list ); break;
		case SYS_TYPE_MAKE( tracere_intr_cfg_type ):	error_code = Tracere_Report_Intr_Cfg(board, (tracere_intr_cfg_type *) info, out, list ); break;
		default:
			error_code = -EC_APP_TYPE_INVALID;	break;
	}
	return error_code;
}

/**********************************************************************************************************//**
@ingroup tracere_report
\brief
Apex Universal Library TracerE specific function that retrieves general board, driver and library status information.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] out  The destination of the report.  Can be 'stdout' or any file.
\param[in] list  Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int Tracere_Report_Status( int board, FILE * out, struct report_prefix_list * list )
{
	int					error_code;  /* can be used for debugging purposes when single stepping through the code */
	int					driver_compiled_with_debug_flag;
	unsigned long		aul_driver_revision_code;
	unsigned long		process_id;
	long				aul_library_revision_code;
	uint16_t			id_value;

	/*** Report the driver revision information */
	error_code = AUL_Driver_Revision( &aul_driver_revision_code );
	if ( SUCCESS != error_code ) return error_code;
	Tracere_Report_Prefix( out, list );
	fprintf( out, " Driver: name = %s, revision = %ld\n", AUL_Driver_Name(), aul_driver_revision_code );
	/*** Report driver debug mode */
	error_code = AUL_Driver_Is_Debug_Active( &driver_compiled_with_debug_flag );
	if ( SUCCESS != error_code ) return error_code;
	if ( driver_compiled_with_debug_flag )
	{
		fprintf( out, "           *** NOTICE ***\n" );
		fprintf( out, "           DRIVER IN DEBUG MODE: DATA BANDWIDTH LIMITED & KERNEL LOG BLOATING POSSIBLE\n" );
	}
	/*** Report the library revision information */
	error_code = AUL_Library_Revision( &aul_library_revision_code );
	if ( SUCCESS != error_code ) return error_code;
	Tracere_Report_Prefix( out, list );
	fprintf( out, "Library: name = %s, revision = %ld\n", AUL_Library_Name(), aul_library_revision_code );
	/*** Report the process ID current in use here */
	error_code = AUL_Library_Process_Id_Get( &process_id );
	if ( SUCCESS != error_code ) return error_code;
	Tracere_Report_Prefix( out, list );
	fprintf( out, "Library: process ID           = %lu\n", process_id );
	/*** Report Tracer-E firmware revision information.  At this time the
	 *   firmware does not provide a mechanism for determining this value,
	 *   therefore it is an implied value.
	 */
	error_code = Tracere_Revision_Get( board, &id_value );
	if ( SUCCESS != error_code ) return error_code;
	Tracere_Report_Prefix( out, list );
	fprintf( out, "Tracer-E firmware revision    = " );
	switch( id_value )
	{
		case TRACERE_REVISION_021102:	fprintf( out, "TRACERE_REVISION_021102  (NOV 02, 2002 IMPLIED)" );	break;
		default:						fprintf( out, "UNKNOWN" );					break;
	}
	fprintf( out, "\n" );

	return SUCCESS;
}

/**********************************************************************************************************//**
@ingroup tracere_report
\brief
Apex Universal Library TracerE specific function that produces a complete TracerE report.

\param[in] board The descriptor obtained from the function ::AUL_Device_Open.
\param[out] out  The destination of the report.  Can be 'stdout' or any file.
\param[in] list  Indentation and any prefix verbage required for this report.
\return An error code of type int is returned. If successful, returns 0 (SUCCESS), otherwise it returns a non-zero error code. Please refer to ::AUL_STATUS_ENUM for all the possible error codes.
*/
int Tracere_Report_Full( int board, FILE * out_file )
{
	tracere_cfg_type  	cfg;
	int 				error_code;
	char 				prefix[] = "  ";
	time_t				time_current;
	struct report_prefix_list  list;

	list.next   = NULL;
	list.prefix = prefix;

	fprintf( out_file, "\n\n" );

	/* date and time */
	fprintf( out_file, "======================================= REPORT FILE\n" );
	time_current = time( NULL );
	fprintf( out_file, "Date:   %s", ctime( &time_current ) );

	/* system status  */
	fprintf( out_file, "Status: tracere board level:\n" );
	error_code = Tracere_Report_Status( board, out_file, &list );
	if ( SUCCESS != error_code ) return error_code;

	/* */
	fprintf( out_file, "Configuration:\n" );

	error_code = Tracere_Config_Get( board, &cfg ); /* get configuration from board */
	if ( SUCCESS != error_code ) return error_code;

	error_code = Tracere_Report( board, &cfg, out_file, &list );
	if ( SUCCESS != error_code ) return error_code;

	fprintf( out_file, "\n\n" );
	return SUCCESS;
}



