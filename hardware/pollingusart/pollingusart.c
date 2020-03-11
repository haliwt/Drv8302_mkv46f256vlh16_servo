#include "pollingusart.h"


uint8_t txbuff[]   = "Vers:2019-10-28\r\n";
uint8_t rxbuff[20] ;


void USART_POLLING_Init(void)
{


    uart_config_t config;
   
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx     = true;
    config.enableRx     = true;

    UART_Init(DEMO_UART, &config, DEMO_UART_CLK_FREQ);

    UART_WriteBlocking(DEMO_UART, txbuff, sizeof(txbuff) - 1);
    
   

 }
/************************************************
  *
  *Function Name:
  *Function: 函数功能是把‘bHex’ ASCII 转换成16进制数，
  *          0~f数。
  * 
  *
  *
************************************************/
 unsigned char ASCIIToHex(unsigned char bHex)
 {
   if((bHex >= 0x30)&&(bHex<= 0x39))
   {
      bHex -= 0x30;
   }
   else if((bHex >= 0x41)&&(bHex<= 0x46))
   {
       bHex -=0x37;
   }
   else if((bHex >= 0x61)&&(bHex<=0x66))
   {
       bHex -= 0x57;
   }
   else
    {
       bHex = 0xff;
    }
   return bHex;
 }
 /************************************************
   *
   *Function Name:
   *Function: 16进制数转换成 字母：a~f(10~15
   *		  
   * 
   *
   *
 ************************************************/
 #if 0
unsigned char charToHex(unsigned char bHex)
 {
	 if((bHex >= 0)&&(bHex<=9))
	 {
	   bHex += 0x30;
	 }
	 else if((bHex >= 10)&&(bHex <= 15))
	 {
	   bHex += 0x37;
	 }
	 else
	 {
		bHex = 0xff;
	 }
	 return bHex;
 }
#endif 
void USART_txtSend(void)

{
   

}
