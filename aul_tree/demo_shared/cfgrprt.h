#ifndef _CFGRPRT_H_
#define _CFGRPRT_H_
/*
	STX104 Configuration Reports


	DESCRIPTION

	ASSUMPTIONS

	TODO


	COPYRIGHT
	Copyright(c) 2013 by Apex Embedded Systems. All rights reserved.
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


/***************************************************************************/

/*char * STX104_Report_Error_Code( STX104_EC_ENUM error_code );*/

void STX104_Report_Status( int board, FILE * out, char * prefix );


void STX104_Report_AI_Configration( stx104_ai_cfg_type * ai_cfg,
									FILE *   out,
                                    char *   prefix,
                                    uint16_t hw_rev
                                  );

void STX104_Report_DI_Configration( STX104_DI_CFG_TYPE * di_cfg,
									FILE * out,
                                    char * prefix,
                                    uint16_t hw_rev
                                  );

void STX104_Report_DO_Configration( stx104_do_cfg_type * do_cfg,
									FILE *   out,
                                    char *   prefix,
                                    uint16_t hw_rev
									);

void STX104_Report_Triggering_Configration( STX104_TRG_CFG_TYPE * trg_cfg,
                                            FILE *   out,
                                            char *   prefix,
                                            uint16_t hw_rev
                                          );

void STX104_Report_Int_Configuration( STX104_INT_CFG_TYPE * int_cfg,
                                      FILE *   out,
                                      char *   prefix,
                                      uint16_t hw_rev
                                    );

void STX104_Report_SPI_Configuration( STX104_SPI_CFG_TYPE * spi_cfg,
                                      FILE *   out,
                                      char *   prefix,
                                      uint16_t hw_rev
                                    );

void STX104_Report_Full( int board, FILE * out_file );


/***************************************************************************/



#ifdef __cplusplus
}
#endif

#endif  /* _CFGRPRT_H_ */
