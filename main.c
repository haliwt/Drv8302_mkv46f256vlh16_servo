/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause  MKV46F256VLH16
 */

/*System includes.*/
#include <stdio.h>
#include "amclib_FP.h"
#include "mlib_FP.h"
#include "gflib_FP.h"
#include "gdflib_FP.h"
#include "gmclib_FP.h"

/* Kernel includes. */
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_debug_console.h"

#include "fsl_pwm.h"
#include "pin_mux.h"
#include "fsl_xbara.h"
#include "led.h"
#include "key.h"
#include "bldc.h"
#include "adc.h"
#include "pollingusart.h"
#include "output.h"
#include "input.h"
//#include "usart_edma_rb.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MAX_LOG_LENGTH 20







/*
**********************************************************************************************************
											变量声明
**********************************************************************************************************
*/




/*******************************************************************************
 *
 * Code
 *
 * @brief Main function
 *
******************************************************************************/
int main(void)
{

 
     
     uint8_t printx1[]="Dir = 1 is OK !!!! CW \r\n";
     uint8_t printx2[]="Dir = 0 is OK #### CCW \r\n";
     uint8_t printx3[]="Start key is run @@@@@@@@@@@@@@@@@ \r\n";
     uint8_t printx4[]="Start key is stop !!!!\n";
     uint8_t ucKeyCode=0,abc_s=0;
     uint8_t start_s =0,motor,dir_s =0;
     volatile uint8_t power_on = 0;
 

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    
   
    LED_Init();
    KEY_Init();
    DelayInit();
    HALL_Init();
    
    SD315AI_SO12_Input_Init();
    
    
    OUTPUT_Fucntion_Init();
    ADC_CADC_Init();
    ABC_POWER_OUTPUT_Init();
    /* Set the PWM Fault inputs to a low value */
    PWM_BLDC_Init();
    USART_POLLING_Init();
     
   while(1)
   {

          ucKeyCode = KEY_Scan(0);
          
          if(motor == 1 )
           {
               if(power_on == 0)
               {
                   printf("************************************************************\r \n");
                   power_on ++ ;
                   SD315AI_Check_Fault();
                   if(Dir==0)  
                   {
                        
                  
                         
                        uwStep = HallSensor_GetPinState();
                        switch(uwStep)
                        {
                        case 5 :
                         BLDC_CCW_SIX_5_Run();
                          break;
                        case 4:
                          
                          BLDC_CCW_SIX_4_Run();
                         break;
                        
                        case 6:
                          BLDC_CCW_SIX_6_Run();
                        break;
                          
                         case 2:
                           BLDC_CCW_SIX_2_Run();
                         break;
                         
                         case 3:
                           BLDC_CCW_SIX_3_Run();
                         break;
                         
                         case 1:
                            BLDC_CCW_SIX_1_Run();
                         break;
                        }
                          
                     printf("Dir = %d\r \n",Dir); 
                     printf("PWM_Duty= %d\r \n",PWM_Duty);
                         
                 }
                 else //CW 
                 {


                    uwStep = HallSensor_GetPinState();
                        switch(uwStep)
                        {
                        case 5 :
                         BLDC_CW_Six_5_Run();
                          break;
                        case 4:
                          
                          BLDC_CW_Six_4_Run();
                         break;
                        
                        case 6:
                          BLDC_CW_Six_6_Run();
                        break;
                          
                         case 2:
                           BLDC_CW_Six_2_Run();
                         break;
                         
                         case 3:
                           BLDC_CW_Six_3_Run();
                         break;
                         
                         case 1:
                            BLDC_CW_Six_1_Run();
                         break;
                        }

                    printf("Dir = %d\r \n",Dir); 
                 
                 }
               }
              else 
              {
                  
             
                  uwStep = HallSensor_GetPinState();
                                 
                  HALLSensor_Detected_BLDC();
                  PWM_Duty = CADC_Read_ADC_Value();
                  printf("PWM_Duty= %d\r \n",PWM_Duty);
                  SD315AI_Check_Fault();
                 
               }
             }
          
          else
          {
              
              GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,1);
              GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,1);
              GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,1);
              PMW_AllClose_ABC_Channel();
              
          
          }
      
   
          
	    
       

      /*处理接收到的按键功能*/  
		if(ucKeyCode !=KEY_UP) 
		{
           switch(ucKeyCode)
            { 
                 
                  case ABC_POWER_PRES :
                     PRINTF("ABC_PRES key  \r\n");
                     abc_s ++;
                    if(abc_s == 1)
				    {
                       A_POWER_OUTPUT =1;
                       power_on = 0;
                 
                     
                     }
				     else 
				     {
                            A_POWER_OUTPUT =0; //shut down
                            power_on = 1;
                            abc_s =0;
                        
				     }
                      
				  	break;
                 			
                   case START_PRES:
                   PRINTF("START_PRES key \r\n");
				    start_s ++;
		          if( start_s== 1)
		          {
                      motor  = 1;
                      power_on = 0;
                    // printf("motor is one \n");
                     UART_WriteBlocking(DEMO_UART, printx3, sizeof(printx3) - 1);
                    
    			   }
				  else 
				  {
                      motor = 0;
                      power_on = 1;
					 start_s =0;
					 
					  //printf("START KEY IS STOP\n");
					  UART_WriteBlocking(DEMO_UART, printx4, sizeof(printx4) - 1);
				  }
                 
				  break;
				  
				 case DIR_PRES: //3

			       dir_s ++ ;
	  			 if(dir_s == 1)
	   			 {
                         Dir=1;
                         power_on = 0;
                       //  printf(" dir is  =1 \r\n");
                         UART_WriteBlocking(DEMO_UART, printx1, sizeof(printx1) - 1);
				  }
				 else 
				   {
                       dir_s = 0;
                      //dirvalue =0; //逆时针旋转
                        Dir = 0;
					 // printf(" dir is  =0 \r\n");
                       UART_WriteBlocking(DEMO_UART, printx2, sizeof(printx2) - 1);
				   }
			
           		break;
            default :
              
      
              break;
			
        }
        
	}
#if 0
        if(Start_Key_StateRead() == KEY_DOWN)
        {

           PRINTF("START_PRES key \r\n");
				    start_s ++;
		          if( start_s== 1)
		          {
                      motor  = 1;
                      power_on = 0;
                    // printf("motor is one \n");
                     UART_WriteBlocking(DEMO_UART, printx3, sizeof(printx3) - 1);
                    
    			   }
				  else 
				  {
                      motor = 0;
                      power_on = 1;
					 start_s =0;
					 
					  //printf("START KEY IS STOP\n");
					  UART_WriteBlocking(DEMO_UART, printx4, sizeof(printx4) - 1);
				  }


        }
#endif 
   }

}
/******************************************************************************
 *
 * Function Name:BARKE_KEY_IRQ_HANDLER(void)
 * Function Active: Interrpt brake input key 
 * @brief Interrupt service fuction of switch.
 *
 * This function toggles the LED
 *
******************************************************************************/
#if 1
void BARKE_KEY_IRQ_HANDLER(void )//void BOARD_BRAKE_IRQ_HANDLER(void)
{
  
    /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(BRAKE_KEY_GPIO, 1U << BRAKE_KEY_GPIO_PIN );
    /* Change state of button. */
    A_POWER_OUTPUT =1;
	
	PRINTF("interrupte has happed  \r\n");
	                  
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif 
/*******************************************************************************************/


