/*
 *  Inc.
 * Copyright 2019 KST
 * All rights reserved.
 *
 * No Hall soft  BSD-3-Clause  MKV46F256VLH16
 */

/*System includes.*/
#include <stdio.h>
//#include "amclib_FP.h"
//#include "mlib_FP.h"
//#include "gflib_FP.h"
//#include "gdflib_FP.h"
//#include "gmclib_FP.h"

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
#include "hall.h"

//#include "usart_edma_rb.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/




output_t motor_ref;


/*******************************************************************************
 *
 * Code
 *
 * @brief Main function
 *
******************************************************************************/
int main(void)
{

 
     
     uint8_t printx1[]="Key Dir = 1 is CW !!!! CW \r\n";
     uint8_t printx2[]="Key Dir = 0 is CCW \r\n";
     uint8_t printx4[]="key motor run = 0 ^^^^ \r\n";
     uint8_t printx5[]="key motor run  = 1 $$$$ \r\n";
     uint8_t ucKeyCode=0,abc_s=0;
     uint8_t dir_s =0,step,power=0;
     uint16_t pwm_duty;
	
	
     

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    
   
    LED_Init();
    KEY_Init();
    DelayInit();
    HALL_Init();
    NO_HALL_GPIO_Init(); 
    
    SD315AI_SO12_Input_Init();
    
    
    OUTPUT_Fucntion_Init();
    ADC_CADC_Init();
    ADC_DMA_Init();
    ABC_POWER_OUTPUT_Init();
    /* Set the PWM Fault inputs to a low value */
    PWM_BLDC_Init();
    USART_POLLING_Init();
     
   while(1)
   {

          ucKeyCode = KEY_Scan(0);
          
          if(motor_ref.motor_run == 1 )
           {
            // pwm_duty = ADC_DMA_ReadValue();
             pwm_duty = CADC_Read_ADC_Value();
             
#ifdef DEBUG_PRINT 
             printf("pw = %d\r \n",pwm_duty); 
         
#endif 
              //  step = ADC_NO_HALL_Value();
              //  CADC_Read_ADC_Value();
               if(motor_ref.power_on == 1)

               {
                   
                   motor_ref.power_on ++;
                   SD315AI_Check_Fault();
#ifdef IRFP4768PbF
                   printf("************************************************************\r \n");
                   if(Dir==0)  
                   {
                      
                      // uwStep = HallSensor_GetPinState();
                       //uwStep= NO_HallSensor_GetPinState();
                       uwStep =2;
                        switch(uwStep)
                        {
                        case 5 :
                         BLDC_CCW_SIX_5_Run(uwStep);
                         DelayMs(5);
                          break;
                        case 4:
                          
                          BLDC_CCW_SIX_4_Run(uwStep);
                          DelayMs(5);
                         break;
                        
                        case 6:
                          BLDC_CCW_SIX_6_Run(uwStep);
                          DelayMs(5);
                        break;
                          
                         case 2:
                           BLDC_CCW_SIX_2_Run(uwStep);
                           DelayMs(5);
                         break;
                         
                         case 3:
                           BLDC_CCW_SIX_3_Run(uwStep);
                           DelayMs(5);
                         break;
                         
                         case 1:
                            BLDC_CCW_SIX_1_Run(uwStep);
                            DelayMs(5);
                         break;
                        }
                          
                     printf("Dir = %d\r \n",Dir); 
                     printf("PWM_Duty= %d\r \n",PWM_Duty);
                     
                        
                 }
                 else //CW 
                 {      
                        uwStep=5; 
                        switch(uwStep)
                        {
                        case 5 :
                         BLDC_CW_Six_5_Run(uwStep);
                         DelayMs(5);
                          break;
                        case 4:
                          
                          BLDC_CW_Six_4_Run(uwStep);
                          DelayMs(5);
                         break;
                        
                        case 6:
                          BLDC_CW_Six_6_Run(uwStep);
                          DelayMs(5);
                        break;
                          
                         case 2:
                           BLDC_CW_Six_2_Run(uwStep);
                           DelayMs(5);
                         break;
                         
                         case 3:
                           BLDC_CW_Six_3_Run(uwStep);
                           DelayMs(5);
                         break;
                         
                         case 1:
                            BLDC_CW_Six_1_Run(uwStep);
                            DelayMs(5);
                         break;
                        }

                    printf("Dir = %d\r \n",Dir); 
                                   
                 }
                #endif 
               }
              else
              {
             
                uwStep = NO_HallSensor_Hex();
                 
              }
                 HALLSensor_Detected_BLDC(pwm_duty, uwStep);
                 ADC_NO_HALL_Value(pwm_duty);
				  SD315AI_Check_Fault();
             
             
                 #ifdef DEBUG_PRINT
                  
             
                //printf("Phase = %d\r \n", Phase);
                 
                    printf("------------\r \n");
                 #endif 
                 
                
            
          }
          else
          {

             if(motor_ref.power_on==2||motor_ref.motor_run==1)
             	{
				  pwm_duty = 40;
				  //uwStep = HallSensor_GetPinState();
                 // uwStep= NO_HallSensor_Hex();
				  HALLSensor_Detected_BLDC(pwm_duty,uwStep);
				  
				  pwm_duty = 35;
				 // uwStep = HallSensor_GetPinState();
                //  uwStep= NO_HallSensor_Hex();
	              HALLSensor_Detected_BLDC(pwm_duty,uwStep);
				  
				  pwm_duty = 30;
				  //uwStep = HallSensor_GetPinState();
                 // uwStep= NO_HallSensor_Hex();
	              HALLSensor_Detected_BLDC(pwm_duty,uwStep);
				  
				  pwm_duty = 25;
				  //uwStep = HallSensor_GetPinState();
                //  uwStep= NO_HallSensor_Hex();
	              HALLSensor_Detected_BLDC(pwm_duty,uwStep);
				  
				  pwm_duty = 20;
				 // uwStep = HallSensor_GetPinState();
                 //  uwStep= NO_HallSensor_Hex();
	              HALLSensor_Detected_BLDC(pwm_duty,uwStep);
				  
				  pwm_duty = 10;
				 // uwStep = HallSensor_GetPinState();
                 // uwStep= NO_HallSensor_Hex();
	              HALLSensor_Detected_BLDC(pwm_duty,uwStep);
				  
				  pwm_duty = 5;
				  //uwStep = HallSensor_GetPinState();
                 // uwStep= NO_HallSensor_Hex();
				  HALLSensor_Detected_BLDC(pwm_duty,uwStep);
             	}
              
			  PMW_AllClose_ABC_Channel();
              SD315AI_Disable_Output();
              GPIO_PortToggle(GPIOD,1<<BOARD_LED1_GPIO_PIN);
              DelayMs(100);
              
            
           }
      
       /*�������յ��İ�������*/  
		if(ucKeyCode !=KEY_UP) 
		{
           switch(ucKeyCode)
            { 
                 
                  case ABC_POWER_PRES :
                     PRINTF("ABC_PRES key  \r\n");
                     abc_s ++;
                     LED2 = 1;
                    if((abc_s == 1)||(motor_ref.abc_numbers ==1))
				    {
                       A_POWER_OUTPUT =1;
                       motor_ref.abc_numbers =0;
                       LED2 = 0;
                 
                    }
				     else 
				     {
                         A_POWER_OUTPUT =0; //shut down
                         motor_ref.motor_run = 0;
                         motor_ref.power_on =0;
                         abc_s =0;
                         LED2 = 0;
                        
				     }
                      
				  	break;
        		
                  case START_PRES:
                   
				   motor_ref.motor_run ++ ;
                   motor_ref.power_on ++ ;
                   LED3 =1;
                 if(motor_ref.motor_run == 1)
    			  {
                      motor_ref.power_on =1;
                      UART_WriteBlocking(DEMO_UART, printx5, sizeof(printx5) - 1);
    			  }
				  else 
				  {
                      motor_ref.motor_run = 0;
                      motor_ref.power_on =0;
                      UART_WriteBlocking(DEMO_UART, printx4, sizeof(printx4) - 1);
				  }
                 
				  break;
		
				 case DIR_PRES: //3

			       dir_s ++ ;
	  			 if(dir_s == 1) //Dir =1 ;  //˳ʱ����ת
	   			 {

                        if(motor_ref.power_on ==2)//motor is runing
                        {
                            if(motor_ref.Dir_flag == 0)
                            {
                              pwm_duty = 10;
							  //uwStep = HallSensor_GetPinState();
                            //  uwStep= NO_HallSensor_Hex();
				              HALLSensor_Detected_BLDC(pwm_duty,uwStep);
							  pwm_duty = 5;
							  //uwStep = HallSensor_GetPinState();
                           //   uwStep= NO_HallSensor_Hex();
							  HALLSensor_Detected_BLDC(pwm_duty,uwStep);
                              motor_ref.power_on = 1;
                              motor_ref.Dir_flag =1;
                              Dir =1;
                            }
                          
                        }
                        else
                        {
                          Dir=1;
                          motor_ref.Dir_flag =1;
                          motor_ref.power_on = 1;
                        
                        }
                         UART_WriteBlocking(DEMO_UART, printx1, sizeof(printx1) - 1);
				  }
			   else // Dir = 0; //��ʱ����ת
			   {
                  dir_s=0;
                  if(motor_ref.power_on == 2) //motor is runing
                 {
                    if(motor_ref.Dir_flag ==1)
                    {

                      pwm_duty = 10;
					  //uwStep = HallSensor_GetPinState();
                     // uwStep= NO_HallSensor_Hex();
		              HALLSensor_Detected_BLDC(pwm_duty,uwStep);
					  pwm_duty = 5;
					  //uwStep = HallSensor_GetPinState();
                     // uwStep= NO_HallSensor_Hex();
					  HALLSensor_Detected_BLDC(pwm_duty,uwStep);
                      motor_ref.power_on = 1;
                      motor_ref.Dir_flag = 0;
                      Dir =0;
                    }
                  
                  }
                  else
                  {
                     
                     Dir = 0;
                     motor_ref.Dir_flag = 0;
                     motor_ref.power_on = 1;
                     

                  }
                 UART_WriteBlocking(DEMO_UART, printx2, sizeof(printx2) - 1);
			   }
			
           		break;
            default :
              
      
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
void BARKE_KEY_IRQ_HANDLER(void )//void BOARD_BRAKE_IRQ_HANDLER(void)
{
  
    /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(BRAKE_KEY_GPIO, 1U << BRAKE_KEY_GPIO_PIN );
    /* Change state of button. */
    A_POWER_OUTPUT =0;
	motor_ref.abc_numbers = 1;
    motor_ref.motor_run = 0;
	PRINTF("interrupte has happed  \r\n");
	                  
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}






/*******************************************************************************************/


