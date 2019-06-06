#ifndef _DEMO_TRACERE_REPORTS_H_
#define _DEMO_TRACERE_REPORTS_H_
/*
	Tracer-E Configuration Reports


	DESCRIPTION

	ASSUMPTIONS

	TODO


	COPYRIGHT
	Copyright(c) 2013-2015 by Apex Embedded Systems. All rights reserved.
	This document is the confidential property of Apex Embedded Systems
	Any reproduction or dissemination is prohibited unless specifically
	authorized in writing.
	Customers are free to use this software to develop applications for
	any Apex Embedded Systems product.
*/

#include "../aul/aul_public.h"
#include <stdio.h>

/***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*
 * TODO: much of this will eventually migrate to report.h and report.c so that a more
 * robust reporting scheme can be created for both library and kernel levels.
 *
 */
/*******************************************************************************/
struct report_prefix_list
{
	struct report_prefix_list * next;		/* append additional prefix information here */
	char *                      prefix;
};


/***************************************************************************/


int I8255M0_Report_Cfg( i8255m0_cfg_type * cfg, FILE * out, struct report_prefix_list * list );

int I8254_Report_Cxfr( i8254_ct_xfr_type * cxfr, FILE * out, struct report_prefix_list * list );

int I8254_Report_Cfg( i8254_cfg_type * cfg, FILE * out, struct report_prefix_list * list );


int Tracere_Report_LED( 	int 	board,
							enum 	tracere_led led,
							FILE * 	out,
							struct report_prefix_list * list
				   	   );

int Tracere_Report_DIO_Cfg( 	int 		board,
								void *		info,
								FILE *		out,
								struct report_prefix_list * list
				   	   	   );

int Tracere_Report_CT_Cfg( 		int 						board,
								tracere_ct_cfg_type *		cfg,
								FILE *						out,
								struct report_prefix_list * list
				   	   	   );

int Tracere_Report_Intr_Cfg( 	int 						board,
								tracere_intr_cfg_type *		cfg,
								FILE *						out,
								struct report_prefix_list * list
				   	   	   );

int Tracere_Report_Cfg( 	int 						board,
							tracere_cfg_type *			cfg,
							FILE *						out,
							struct report_prefix_list * list
				   	   );

int Tracere_Report( 	int 						board,
						void *						info,
						FILE *						out,
						struct report_prefix_list * list
				   );

int Tracere_Report_Status( int board, FILE * out, struct report_prefix_list * list );

int Tracere_Report_Full( int board, FILE * out_file );


/***************************************************************************/



#ifdef __cplusplus
}
#endif

#endif  /* _DEMO_TRACERE_REPORTS_H_ */
