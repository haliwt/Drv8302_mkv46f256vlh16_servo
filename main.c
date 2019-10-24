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




BLDC_REF bldc_ref ;


/*
**********************************************************************************************************
											变量声明
**********************************************************************************************************
*/
//__IO int32_t uwStep ;      //六步引脚状态
    
//__IO uint32_t Lock_Time ;  // 堵转电流

//__IO int16_t  PWM_Duty;  //占空比

//__IO int8_t Dir;






/*******************************************************************************
 *
 * Code
 *
 * @brief Main function
 *
******************************************************************************/
int main(void)
{

 
   
     uint16_t sampleMask;
     uint8_t ucKeyCode=0,abc_s=0,dirvalue=0;
     uint8_t start_s =0;
   

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    
   
    LED_Init();
    KEY_Init();
    DelayInit();
    HALL_Init();
    
    SD315AI_SO12_Input_Init();
    
    HallSensor_GetPinState();
    OUTPUT_Fucntion_Init();
    ADC_CADC_Init();
    ABC_POWER_OUTPUT_Init();
    /* Set the PWM Fault inputs to a low value */
    PWM_BLDC_Init();
    USART_POLLING_Init();
     
   while(1)
   {

          ucKeyCode = KEY_Scan(0);
          
          if(bldc_ref.motor_run == 1)
          {

            uwStep = HallSensor_GetPinState();
            HALLSensor_Detected_BLDC();
          }
          else 
         {
           PMW_AllClose_ABC_Channel();  
          
         }
      
   
           /*Check U phase*/
          if(GPIO_PinRead(SD315AI_SO1_A_GPIO,SD315AI_SO1_A_PIN)==1 ||GPIO_PinRead(SD315AI_SO2_A_GPIO,SD315AI_SO1_A_PIN)==1)
          {
               GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,1);//SD315_VL_A_OUTPUT=0;
           
          }
          else
          {
              GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,0);//SD315_VL_A_OUTPUT=0;

          }
          /*Check V phase*/
          if(GPIO_PinRead(SD315AI_SO1_B_GPIO,SD315AI_SO1_B_PIN)== 1 ||GPIO_PinRead(SD315AI_SO2_B_GPIO,SD315AI_SO2_B_PIN)== 1)
          {
               
               GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,1);//SD315_VL_B_OUTPUT=0;
            

          }
          else
          {
               GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,0);//SD315_VL_B_OUTPUT=0;
          }
          /*Check W phase*/
          if(GPIO_PinRead(SD315AI_SO1_C_GPIO,SD315AI_SO1_C_PIN)== 1 || GPIO_PinRead(SD315AI_SO2_C_GPIO,SD315AI_SO2_C_PIN)== 1)
          {
                GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,1);
               
          }
          else
          {
                GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,0);
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
                      A_POWER_OUTPUT =0;

                      
                     
                     }
				     else 
				     {
                            A_POWER_OUTPUT =1; //shut down
                            abc_s =0;
                        
				     }
                      
				  	break;

				 case START_PRES:
                   PRINTF("START_PRES key \r\n");
				    start_s ++;
		          if(start_s == 1)
		          {
                     bldc_ref.motor_run = 1;
                     printf("motor is one \n");
                    
    			   }
				  else 
				  {
                    bldc_ref.motor_run = 0;
					 start_s =0;
					 
					 printf("START KEY IS STOP\n");
				  }
                 
				  break;
				  
				 case DIR_PRES: //3

			       dirvalue ++;
	  			 if(dirvalue == 1)
	   			 {
                        Dir =1;
				  }
				 else 
				   {
                      Dir = 0;
                      dirvalue =0;
					  printf(" dir is  =0 \r\n");
				   }
			
           		break;
            default :
               /**********************adjust frequency ****************************/
			{
            
		    CADC_DoSoftwareTriggerConverter(CADC_BASEADDR, kCADC_ConverterA);
	             /* Wait the conversion to be done. */
	         while (kCADC_ConverterAEndOfScanFlag !=
	               (kCADC_ConverterAEndOfScanFlag & CADC_GetStatusFlags(CADC_BASEADDR)))
	        {
	        }

	       
            CADC_ClearStatusFlags(CADC_BASEADDR, kCADC_ConverterAEndOfScanFlag);

     	    }

            PWM_Duty = (uint16_t)((CADC_GetSampleResultValue(CADC_BASEADDR, 1U))/ 330);
           
            PRINTF("PWM_Duty = %d\r\n",PWM_Duty);
      
              break;
			
        }
        
	}
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
    bldc_ref.motor_run = 0;
	PRINTF("interrupte has happed  \r\n");
	                  
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif 
/*******************************************************************************************/


