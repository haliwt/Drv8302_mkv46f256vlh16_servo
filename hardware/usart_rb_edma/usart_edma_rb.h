#ifndef __USART_EDMA_RB_H__
#define __USART_EDMA_RB_H__

#include "board.h"
#include "fsl_uart_edma.h"
#include "fsl_dmamux.h"
#include "timer.h"

#include "pin_mux.h"
#include "clock_config.h"


/*******************************************************************************
 * Variables
 ******************************************************************************/
#define DEMO_UART UART1
#define DEMO_UART_CLKSRC kCLOCK_FastPeriphClk
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(kCLOCK_FastPeriphClk)
#define UART_TX_DMA_CHANNEL 0U
#define UART_RX_DMA_CHANNEL 1U
#define EXAMPLE_UART_DMAMUX_BASEADDR DMAMUX0
#define EXAMPLE_UART_DMA_BASEADDR DMA0
#define UART_TX_DMA_REQUEST kDmaRequestMux0UART1Tx
#define UART_RX_DMA_REQUEST kDmaRequestMux0UART1Rx
#define ECHO_BUFFER_LENGTH 8




uart_edma_handle_t g_uartEdmaHandle;
edma_handle_t g_uartTxEdmaHandle;
edma_handle_t g_uartRxEdmaHandle;
uint8_t g_tipString[]                  = "UART EDMA example\r\nSend back received data\r\nEcho every 8 characters\r\n";
uint8_t g_txBuffer[ECHO_BUFFER_LENGTH] = {0};
uint8_t g_rxBuffer[ECHO_BUFFER_LENGTH] = {0};
volatile bool rxBufferEmpty            = true;
volatile bool txBufferFull             = false;
volatile bool txOnGoing                = false;
volatile bool rxOnGoing                = false;


/* UART user callback */
void UART_UserCallback(UART_Type *base, uart_edma_handle_t *handle, status_t status, void *userData);






void UART_EDMA_RB_Init(void);


void UART_EDMA_RW_Function(void);



#endif 
