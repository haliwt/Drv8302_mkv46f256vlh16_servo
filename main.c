/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause  MKV46F256VLH16
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
#include "encoder.h"
#include "fsl_xbara.h"
#include "fsl_enc.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
__IO int32_t HALL_Pulse;  

output_t  motor_ref;
encoder_t en_t;
int32_t PID_Result;
PID_TypeDef  sPID;
__IO uint16_t  PID_PWM_Duty;

/*******************************************************************************
 *
 * Code
 *
 * @brief Main function
 *
******************************************************************************/
int main(void)
{

     enc_config_t mEncConfigStruct;
	 int32_t iError,dError_sum,last_iError;
     uint8_t printx1[]="Key Dir = 1 is CW !!!! CW \r\n";
     uint8_t printx2[]="Key Dir = 0 is CCW \r\n";
     uint8_t printx4[]="key motor run = 0 ^^^^ \r\n";
     uint8_t printx5[]="key motor run  = 1 $$$$ \r\n";
     uint8_t ucKeyCode=0,kp,ki,kd;
     uint8_t RxBuffer[4],i;
	 float KP,KI,KD;
     uint32_t Time_CNT;
	 uint32_t mDiffPosValue,mCurPosValue;
    
    XBARA_Init(XBARA);
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
 
    
     LED_Init();
     KEY_Init();
     DelayInit();
     HALL_Init();
     Capture_Input_Init();

   
    OUTPUT_Fucntion_Init();
    ADC_CADC_Init();
   
	
    /* Set the PWM Fault inputs to a low value */
    PWM_BLDC_Init();
   // USART_POLLING_Init();
     #if 1  //no interrupt
    /* Initialize the ENC module. */
    ENC_GetDefaultConfig(&mEncConfigStruct);
    ENC_Init(DEMO_ENC_BASEADDR, &mEncConfigStruct);
    ENC_DoSoftwareLoadInitialPositionValue(DEMO_ENC_BASEADDR); /* Update the position counter with initial value. */
    #endif
	Dir =3;
    PWM_Duty =70;

   while(1)
   {
       ucKeyCode = KEY_Scan(0);
       
       
        #ifdef DEBUG_PRINT 
       // PRINTF("Cpw = %d\r\n", capture_width);
        #endif
		mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
        #ifdef DEBUG_PRINT 
        PRINTF("Cpos : %d\r\n", mCurPosValue);
        #endif
		mDiffPosValue=(int16_t)ENC_GetPositionDifferenceValue(DEMO_ENC_BASEADDR);/*differential value DK*/
		#ifdef DEBUG_PRINT 
        PRINTF("Di = %d\r\n", mDiffPosValue);
        PRINTF("Re %d\r\n", ENC_GetRevolutionValue(DEMO_ENC_BASEADDR));
        PRINTF("CPHod: %d\r\n", ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR));
        #endif
		if(HALL_Pulse >=0)
	    PRINTF("HallN= %d\r\n", HALL_Pulse );
		else 
		PRINTF("-HallN=- %d\r\n", HALL_Pulse );
			
		
#if 0
		
      /***********Position :Home and End*****************/
	  
	   
#endif 
         
/**************************************************************************************************************************************/
        
     /***********motor run main*********************/
     if(motor_ref.motor_run == 1)
      {
   		   PWM_Duty =95;	
       //  GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,1);
         
	      uwStep = HallSensor_GetPinState();
          HALLSensor_Detected_BLDC(PWM_Duty);
        
         // PWM_Duty = PID_PWM_Duty;
          Time_CNT++;
#if 1    
        /* 100ms arithmetic PID */
    	if(Time_CNT % 100 == 0){

						mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
						//iError = mCurPosValue - sPID.SetPoint ; //
						iError = 60;
						if((iError<5 )&& (iError>-5)) //�ű�������
    						iError = 0;
						if(iError >=0)//CW
						{
							
						}
						else{
							  
							}
						dError_sum += iError; /*����ۼƺ�*/
						if(dError_sum > 1000)dError_sum =1000; //�����޷�
						if(dError_sum < -1000)dError_sum = -1000; 
						PID_PWM_Duty = (int32_t)(iError *KP + dError_sum * KI + (iError - last_iError)*KD);//proportion + itegral + differential
						PRINTF("PID pwm= %d\r \n",PID_PWM_Duty);
						if(PID_PWM_Duty >=100)PID_PWM_Duty=100;
						if(PID_PWM_Duty <= 0)PID_PWM_Duty =0;
						 
						last_iError = iError;
						PWM_Duty = PID_PWM_Duty;
		}
	   if(Time_CNT == 100)
			Time_CNT = 0;
#endif 
	} 
    else { 
            PMW_AllClose_ABC_Channel();
		 // GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,0);
	      DelayMs(50);
	      GPIO_PortToggle(GPIOD,1<<BOARD_LED1_GPIO_PIN);
	      DelayMs(50);
	     
          if( motor_ref.motor_run == 3){
		  	
                  UART_ReadBlocking(DEMO_UART, RxBuffer, 4);
                  for(i=0;i<5;i++)
                  {
                      kp=RxBuffer[0];
                      ki=RxBuffer[1];
					  kd=RxBuffer[2];
                      motor_ref.motor_run =RxBuffer[3];
                      PRINTF("KP KI KD = %d %d %d \n\r",kp,ki,kd);
                      KP = (float)kp/10;
                      KI = (float)ki/100;
                      KD = (float)kd/10;
                     
                  }

         }
         PRINTF("Motor Stop ! \r\n");
         PRINTF("motor_run = %d \r\n",motor_ref.motor_run );
                
    }
            
    
   
   /**********8Key process********************/  
   if(ucKeyCode !=KEY_UP){

	  switch(ucKeyCode){ 
                 
                  case DIR_DOWN_PRES ://Dir =1
				  	// Dir = 1;
			
                    if(Dir == 0) //
	   			    {

                        if((motor_ref.motor_run == 1)&&(motor_ref.Dir_flag == 0))//motor is runing
                        {
                            
                              PWM_Duty= 10;
							  uwStep = HallSensor_GetPinState();
				              HALLSensor_Detected_BLDC( PWM_Duty);
							   PWM_Duty = 5;
							  uwStep = HallSensor_GetPinState();
							  HALLSensor_Detected_BLDC( PWM_Duty);
                              DelayMs(30);
                         
                              motor_ref.Dir_flag =1;
                              Dir =1;
                            
                          
                        }
						else 
						{
							Dir=1;
                       		 motor_ref.Dir_flag =1;
						}
						
                     }
                      else
                      {
                        Dir=1;
                        motor_ref.Dir_flag =1;
                       
                       
                      
                      }
                         UART_WriteBlocking(DEMO_UART, printx1, sizeof(printx1) - 1);
				    
                      
				  	break;
        		
                 case START_PRES:
                   
				   motor_ref.motor_run ++ ;
                   motor_ref.power_on ++ ;
                
                   PRINTF("START_KEY \n\r");
                
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
		
				 case DIR_UP_PRES: //ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½×ª Dir = 0;ï¿½ï¿½ï¿½Ï¡ï¿½--ï¿½ï¿½Ö±ï¿½ï¿½ï¿½ï¿½
				   // Dir = 0 ; //ï¿½ï¿½Ê±ï¿½ï¿½
				
				   PRINTF("DIR =0\r\n");
	  			
			     if(Dir==1) // Dir = 0; //ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½×ª
				   {
	               
	                 if((motor_ref.motor_run == 1) &&(motor_ref.Dir_flag ==1))//motor is runing
	                 {
	                   
	                      PWM_Duty = 10;
						  uwStep = HallSensor_GetPinState();
			              HALLSensor_Detected_BLDC( PWM_Duty);
						   PWM_Duty = 5;
						  uwStep = HallSensor_GetPinState();
						  HALLSensor_Detected_BLDC( PWM_Duty);
	                      DelayMs(30);
	                   
	                      motor_ref.Dir_flag = 0;
	                      Dir =0;
	                    
	                 }
						else
						{
						  Dir =0;
						  motor_ref.Dir_flag = 0;
	                     
						}
						
	               }
	               else
	                  {
	                     
	                     Dir = 0;
	                    
	                     motor_ref.Dir_flag = 0;
	                  
	                  }
                 UART_WriteBlocking(DEMO_UART, printx2, sizeof(printx2) - 1);
			   
			
           		break;
				case MOTOR_STOP_PRES:
                   
					 motor_ref.motor_run = 3;
                     PRINTF("motor stop = %d \n\r",motor_ref.motor_run);
                 break;
				case USART_RT_PRES:
					 
				break;
            default :
              
      
              break;
			
        }
        
	}

  }//end while(1)
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
   
	
    motor_ref.motor_run = 0;
	PMW_AllClose_ABC_Channel();
    //GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,0);
	PRINTF("interrupte has happed  \r\n");
	                  
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}






/*******************************************************************************************/


