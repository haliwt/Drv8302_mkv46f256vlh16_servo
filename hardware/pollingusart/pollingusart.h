#ifndef __POLLINGUSART_H__
#define __POLLINGUSART_H__ 

#include "board.h"
#include "fsl_uart_edma.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "fsl_dmamux.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_UART UART1
#define DEMO_UART_CLKSRC       kCLOCK_FastPeriphClk
#define DEMO_UART_CLK_FREQ     CLOCK_GetFreq(kCLOCK_FastPeriphClk)
	
/*******************************************************************************
 * Variables
 ******************************************************************************/

void USART_POLLING_Init(void);
unsigned char ASCIIToHex(unsigned char bHex);


#endif 
