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




volatile uint32_t g_EncIndexCounter = 0U;
uint32_t pulseWidth;


output_t  motor_ref;
encoder_t en_t;
int16_t setHome=0xfff;
int16_t setEnd=0xfff;
int16_t setPositionHome=0xfff;
int16_t setPositionEnd=0xfff;
uint8_t setRun_flag=0 ;
uint8_t setStop_flag=0;


/*******************************************************************************
 * Code
 ******************************************************************************/

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
	 
     uint8_t printx1[]="Key Dir = 1 is CW !!!! CW \r\n";
     uint8_t printx2[]="Key Dir = 0 is CCW \r\n";
     uint8_t printx4[]="key motor run = 0 ^^^^ \r\n";
     uint8_t printx5[]="key motor run  = 1 $$$$ \r\n";
     uint8_t ucKeyCode=0;
     uint8_t dir_s =0;
     uint16_t pwm_duty;
	// uint16_t adcr;

    
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
	

   while(1)
   {
         ucKeyCode = KEY_Scan(0);
		//adcr = CADC_Read_ADC_Value();
	   // PRINTF("ADC: %d\r\n", adcr);
        //  PRINTF("setHome= %d \r\n",setHome);
		//  PRINTF("setEnd= %d \r\n",setEnd);
		  PRINTF("setPositionHome^^^= %d \r\n",setPositionHome);
		   PRINTF("setPositionEnd@@@@@@= %d \r\n",setPositionEnd);
	      en_t.capture_width =Capture_ReadPulse_Value();
		  PRINTF("Cpw = %d\r\n", en_t.capture_width);
		
		
		en_t.mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
		/* Read the position values. */
        PRINTF("Current position : %d\r\n", en_t.mCurPosValue);
      /******************************************************************************************/
	   if(((en_t.firstPowerOn ==0)||(en_t.firstPowerOn <4))&&(en_t.en_interrupt_flag == 1 ))
		{
			   PRINTF("--------------------------------------\r\n" ); 
			   en_t.en_interrupt_flag=0;
			   en_t.firstPowerOn++;
	          
			   PRINTF("firstOn= %d \r\n",en_t.firstPowerOn);
				
			   if(en_t.firstPowerOn ==2)
			   	{
				   setHome = en_t.capture_width;
				   setPositionHome = en_t.mCurPosValue;
				   PRINTF("setHome= %d \r\n",setHome);
				   PRINTF("setPositionHome^^^= %d \r\n",setPositionHome);
								  
			   	}
			    if(en_t.firstPowerOn ==3)
			   {

				   setEnd = en_t.capture_width;
				   setPositionEnd = en_t.mCurPosValue;
				   PRINTF("setPositionEnd@@@= %d \r\n",setPositionEnd);
			   }
			  
			   PRINTF("setHome= %d \r\n",setHome);
			   PRINTF("setEnd= %d \r\n",setEnd);
			   PRINTF("setPositionHome^^^= %d \r\n",setPositionHome);
			   PRINTF("setPositionEnd@@@@@@= %d \r\n",setPositionEnd);
								
		}
           
     /*********************************检测信号************************************************************************/ 
		 if(setPositionHome  == en_t.mCurPosValue||setPositionHome == (en_t.mCurPosValue +1 )||setPositionHome == en_t.mCurPosValue-1)
		 {
              
           PRINTF("setPositionHome ok \r\n");
		   PRINTF("setPositionHome = %d \r\n",setPositionHome);
		   if(setRun_flag == 1)
		   {
               setStop_flag=0; 
               PRINTF("setPosHomeRun||||| = %d \r\n",setRun_flag);
		   }
		   else
           {
             setStop_flag=1; 
             PRINTF("setPosHomeRun = %d \r\n",setRun_flag);
           }	  
          }
          if((setPositionEnd == en_t.mCurPosValue-1)||setPositionEnd  == en_t.mCurPosValue+1||setPositionEnd  == en_t.mCurPosValue)//绝对位置是不变的，setHome 软件指定的
         {
			 
		       PRINTF("setPositionEnd ok \r\n");
			   PRINTF("setPositionEnd = %d \r\n",setPositionEnd);
               if(setRun_flag ==1)
               {
				  setStop_flag=0;
                  PRINTF("setPosEndRun ~~~~~= %d \r\n",setRun_flag);
               }
			   else
               {
			     setStop_flag = 1;
			   PRINTF("setPosEndRun = %d \r\n",setRun_flag);
               }

		}

         
/**************************************************************************************************************************************/
        
     /***********motor run main*********************/
      if((motor_ref.motor_run == 1)&&(setStop_flag !=1))
      {
   				
			        GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,1);
					  
				
			  
             pwm_duty=60;
	#ifdef DEBUG_PRINT 
             printf("pwm_duty = %d\r \n",pwm_duty); 
	#endif 
              if(motor_ref.power_on == 1)
              {
                   motor_ref.en_on=0;
                   motor_ref.power_on ++;
				   setRun_flag=0;
                  
	#ifdef IRFP4768PbF
                   printf("************************************************************\r \n");
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
                #endif 
               }
              else
			  {
                  
                     
					 uwStep = HallSensor_GetPinState();
	               
	                 HALLSensor_Detected_BLDC(pwm_duty);
					
                     
					  #ifdef DEBUG_PRINT
					     // PRINTF("uwStep = %d\r \n",uwStep); 
						  PRINTF("Current position value: %d\r\n", mCurPosValue);
		                  PRINTF("Position differential value: %d\r\n",mCurVelValue );
		                  PRINTF("Position revolution value: %d\r\n",mCurRevValue );
						  PRINTF("......................\r \n"); 
				   	  #endif 
				  
              }
           }/*end if motor_ref.motor_run == 1*/
          
          else
		  {

             if(motor_ref.power_on==2||motor_ref.motor_run==1)
             	{
				  
				  
				  pwm_duty = 20;
				  uwStep = HallSensor_GetPinState();
	              HALLSensor_Detected_BLDC(pwm_duty);
				  DelayMs(50);
                  
                  pwm_duty = 10;
				  uwStep = HallSensor_GetPinState();
	              HALLSensor_Detected_BLDC(pwm_duty);
				  DelayMs(50);
				 
				  pwm_duty = 5;
				  uwStep = HallSensor_GetPinState();
	              HALLSensor_Detected_BLDC(pwm_duty);
				  DelayMs(50);
				 
             	}
              
                 
             
			  PMW_AllClose_ABC_Channel();
              DelayMs(50);
              GPIO_PortToggle(GPIOD,1<<BOARD_LED1_GPIO_PIN);
              DelayMs(50);
              
		
            }
        
   
       /*处理接收到的按键功能*/  
		if(ucKeyCode !=KEY_UP) 
		{
           switch(ucKeyCode)
            { 
                 
                  case ABC_POWER_PRES :
                    
                      
				  	break;
        		
                 case START_PRES:
                   
				   motor_ref.motor_run ++ ;
                   motor_ref.power_on ++ ;
                   setRun_flag = 1;
				   PRINTF("Run = %d \r\n",setRun_flag);
                 if(motor_ref.motor_run == 1)
    			  {
                      motor_ref.power_on =1;
                      setRun_flag = 1;
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
                   setRun_flag = 1;
	  			 if(dir_s == 1) //Dir =1 ;  //顺时针旋转
	   			 {

                        if((motor_ref.power_on ==2)||(motor_ref.motor_run == 1))//motor is runing
                        {
                            if(motor_ref.Dir_flag == 0)
                            {
                              pwm_duty = 10;
							  uwStep = HallSensor_GetPinState();
				              HALLSensor_Detected_BLDC(pwm_duty);
							  pwm_duty = 5;
							  uwStep = HallSensor_GetPinState();
							  HALLSensor_Detected_BLDC(pwm_duty);
                            
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
						  setRun_flag = 1;
                        
                        }
                         UART_WriteBlocking(DEMO_UART, printx1, sizeof(printx1) - 1);
				  }
			   else // Dir = 0; //逆时针旋转
			   {
                 dir_s=0;
                 if((motor_ref.power_on == 2)||(motor_ref.motor_run == 1)) //motor is runing
                 {
                    if(motor_ref.Dir_flag ==1)
                    {

                      pwm_duty = 10;
					  uwStep = HallSensor_GetPinState();
		              HALLSensor_Detected_BLDC(pwm_duty);
					  pwm_duty = 5;
					  uwStep = HallSensor_GetPinState();
					  HALLSensor_Detected_BLDC(pwm_duty);
                     
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
   
	en_t.en_interrupt_flag=1;
    motor_ref.motor_run = 0;
    GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,0);
	PRINTF("interrupte has happed  \r\n");
	                  
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

#if 0
/*!
 * @brief ISR for INDEX event
 */
void ENC_INDEX_IRQHandler(void)
{
    g_EncIndexCounter++;
    ENC_ClearStatusFlags(DEMO_ENC_BASEADDR, kENC_INDEXPulseFlag);
   // PRINTF("g_en = %d \r\n",g_EncIndexCounter);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif 
#if 0
void FTM_INPUT_CAPTURE_HANDLER(void)
{
    if ((FTM_GetStatusFlags(DEMO_FTM_BASEADDR) & kFTM_TimeOverflowFlag) == kFTM_TimeOverflowFlag)
    {
        /* Clear overflow interrupt flag.*/
        FTM_ClearStatusFlags(DEMO_FTM_BASEADDR, kFTM_TimeOverflowFlag);
        g_timerOverflowInterruptCount++;
    }
    else if (((FTM_GetStatusFlags(DEMO_FTM_BASEADDR) & FTM_FIRST_CHANNEL_FLAG) == FTM_FIRST_CHANNEL_FLAG) &&
             (ftmFirstChannelInterruptFlag == false))
    {
        /* Disable first channel interrupt.*/
        FTM_DisableInterrupts(DEMO_FTM_BASEADDR, FTM_FIRST_CHANNEL_INTERRUPT_ENABLE);
        g_firstChannelOverflowCount  = g_timerOverflowInterruptCount;
        ftmFirstChannelInterruptFlag = true;
    }
    else if ((FTM_GetStatusFlags(DEMO_FTM_BASEADDR) & FTM_SECOND_CHANNEL_FLAG) == FTM_SECOND_CHANNEL_FLAG)
    {
        /* Clear second channel interrupt flag.*/
        FTM_ClearStatusFlags(DEMO_FTM_BASEADDR, FTM_SECOND_CHANNEL_FLAG);
        /* Disable second channel interrupt.*/
        FTM_DisableInterrupts(DEMO_FTM_BASEADDR, FTM_SECOND_CHANNEL_INTERRUPT_ENABLE);
        g_secondChannelOverflowCount  = g_timerOverflowInterruptCount;
        ftmSecondChannelInterruptFlag = true;
    }
    else
    {
    }
    __DSB();
}
#endif 

/*******************************************************************************************/


