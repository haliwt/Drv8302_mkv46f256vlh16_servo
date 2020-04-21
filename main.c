/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause  MKV46F256VLH16
 */

/*System includes.*/


/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include <stdio.h>
#include "fsl_debug_console.h"

#include "fsl_pwm.h"
#include "pin_mux.h"
#include "fsl_xbara.h"
#include "led.h"
#include "key.h"
#include "bsp_bldc.h"
#include "adc.h"
#include "pollingusart.h"
#include "output.h"
#include "input.h"
#include "hall.h"
#include "bsp_encoder.h"
#include "fsl_xbara.h"
#include "fsl_enc.h"
#include "bsp_algorithm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
 #define AUTOMATICA  0 //1 
__IO int32_t HALL_Pulse;  

output_t  motor_ref;
encoder_t en_t;
int32_t PID_Result;
PID_TypeDef  sPID;
__IO int32_t  PID_PWM_Duty;
BLDC_Typedef BLDCMotor;
//tpid_refer pid_r;

 
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
	 volatile int32_t iError,last_iError,ivError,last_ivError ;
    
     uint8_t ucKeyCode=0;
     uint8_t RxBuffer[5],i,k0;
     
     volatile int16_t DectBuf[6];
     volatile uint16_t Time_CNT,EnBuf[2]={0,0};
	 volatile int32_t mCurPosValue=0,mHoldPos=0,HDff,VDff,Dff;
	 int16_t lkeydir;
   
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
	Dir=3;
    en_t.First_V_dec =0;
    en_t.First_H_dec =0;
	en_t.HorizonStop_flag=0;
	
   while(1)
   {
    ucKeyCode = KEY_Scan(0);

    mHoldPos = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
	en_t.Pos_diff = (int16_t)ENC_GetHoldPositionDifferenceValue(DEMO_ENC_BASEADDR);
       
	
		printf("currePosHold: %ld\r\n", mHoldPos);
	    printf("Xpos = %d\r\n", en_t.X_axis);
        printf("Ypos = %d\r\n", en_t.Y_axis);
        printf("en_t.eInit_n: %d\r\n", en_t.eInit_n);
	 

	 /***********Position :Home and End*****************/
        if(en_t.eInit_n !=1) {
          //  printf("itself %%%%%%%%%%%%%\r\n"); 
            PWM_Duty=50 ;
			LED2=!LED2;
			if(Dir == 1)
			{
		       en_t.End_V_flag = 1;  
			   HALL_Pulse =0;
			   Search_Start_VerticalPos();
			}
			else if(Dir == 0){
				     en_t.End_V_flag = 1; 
					 HALL_Pulse =0;
					Search_Start_HorizonPos();
                   
			     }
       
		 }//end en_t.eIn_n == 0
		 
     
    /********************************************************************************/     
    /***********motor run main*********************/
     if((motor_ref.motor_run == 1)&&(en_t.HorizonStop_flag !=2)&&(en_t.End_V_flag!=1))
     {
   		  if(en_t.eInit_n == 1){
            
           if(en_t.DIR_flag ==1)PWM_Duty =50;
		   else PWM_Duty = PID_PWM_Duty;
			   
   		  }

		  if(en_t.HorizonStop_flag ==1 && en_t.eInit_n ==1){

				Balance_HorizonRegion();
					
		 }
		if(Dir==0)
	  	{
			  uwStep = HallSensor_GetPinState();
	          HALLSensor_Detected_BLDC(PWM_Duty);
	  	}
	   else{
	  	       
            uwStep = HallSensor_GetPinState();
	         HALLSensor_Detected_BLDC(PWM_Duty);
	  	}
		mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
		en_t.Pos_diff = (int16_t)ENC_GetHoldPositionDifferenceValue(DEMO_ENC_BASEADDR);

#if 1
         if(Dir == 0){
	          
	           iPrintf();
		 }
#endif
		   

		  
           Time_CNT++;
#if 1    
        /* 100ms arithmetic PID */
    	if((Time_CNT % 25 == 0)&&(en_t.eInit_n == 1)&&(en_t.HorizonStop_flag !=2)&&(en_t.HorizonStop_flag !=1)){
   
            HALL_Pulse =0;
			//mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
            //printf("mXPos: %d\r\n", mCurPosValue);
	
			if(Dir == 0)//CCW  Horizion Direction
			{
						en_t.DIR_flag =0;
						Horizon_Decelerate();
						printf("PID_PWM_H 25 = %d\r\n",PID_PWM_Duty);
				
			}
			else{  //Vertical Position judge is boundary
					  
					Vertical_Decelerate();
			    
			}
			
		}
	   if(Time_CNT >=25){
			Time_CNT = 0;
			
	   	}
	   #endif 
	 } 
     else if(en_t.HorizonStop_flag==2){
          en_t.DIR_flag = 1;
          Dir =1;
          PWM_Duty =30; /*real be test*/
          uwStep = HallSensor_GetPinState();
          HALLSensor_Detected_BLDC(PWM_Duty);
 		  
          Dir =0;
		  PRINTF("flag=2 stop CurrPos !!!!!!!\n");
		  #if 0//AUTOMATICA 
		   Dir = 1;
		   en_t.HorizonStop_flag= 0;
           motor_ref.motor_run =1;
		   en_t.DIR_flag = 1;

           #endif
		  // Self_Locking();
		  
		  
     
     }
    else if(en_t.eInit_n !=0){ 
            
		    en_t.HorizonStop_flag=0;
			PMW_AllClose_ABC_Channel();
			HALL_Pulse =0;
			iError =0;
			#ifdef DRV8302
			GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,0);
			#endif 
		  en_t.DIR_flag =1;
	      DelayMs(50);
	      GPIO_PortToggle(GPIOD,1<<BOARD_LED1_GPIO_PIN);
	      DelayMs(50);
	      HALL_Pulse =0;
         if(motor_ref.motor_run == 3){
				  UART_ReadBlocking(DEMO_UART, RxBuffer, 5);
				  PRINTF("PID input referece \n");
                  for(i=0;i<8;i++){
				  	    
						  if(RxBuffer[0]==0xff){
							  pid_r.KD_H=RxBuffer[1];
		                      pid_r.KI_H=RxBuffer[2];
							  pid_r.KD_H=RxBuffer[3];
							 
		                      motor_ref.motor_run =RxBuffer[4];
		                     PRINTF("KP KI KD = %d %d %d \n\r",pid_r.KP_H,pid_r.KI_H,pid_r.KD_H);
							 
							 
					  	  }
						  else{
								 k0=RxBuffer[0];
								 PRINTF("USART Error !!!!!!!!\n\r");
								 PRINTF("k0 = %d \n\r",k0);
								 PRINTF("KP KI KD = %d %d %d \n\r",pid_r.KP_H,pid_r.KI_H,pid_r.KD_H);
								
								  motor_ref.motor_run =RxBuffer[4];
						      }
                        }
		          
					}
         #if AUTOMATICA 
		   Dir = 0;
		   en_t.HorizonStop_flag= 0;
           motor_ref.motor_run =1;
		   en_t.DIR_flag = 1;

           #endif
		
    }
            
    
   
   /**********8Key process********************/  
   if(ucKeyCode !=KEY_UP){

	  switch(ucKeyCode){ 
                 
                  case DIR_CW_PRES ://Dir =1 ,PTE29-CW,KEY1,motor to Vertical 
				  	  HALL_Pulse =0;
                      en_t.DIR_flag = 1;
	  				  if(en_t.HorizonStop_flag==2){
							
						  for(lkeydir=0;lkeydir< 500;lkeydir++){
							  Dir =1;
							  PWM_Duty =50; /*real be test*/
					          uwStep = HallSensor_GetPinState();
					          HALLSensor_Detected_BLDC(PWM_Duty);
						  }
						
						  en_t.HorizonStop_flag= 0;
                          motor_ref.motor_run =1;
						  en_t.DIR_flag = 1;
                          
	  				  }
                      else {
					  	      Dir =1;
							 for(lkeydir=0;lkeydir< 500;lkeydir++){
							  Dir =1;
							  PWM_Duty =50; /*real be test*/
					          uwStep = HallSensor_GetPinState();
					          HALLSensor_Detected_BLDC(PWM_Duty);
						  }
							  motor_ref.motor_run =1;
                             en_t.DIR_flag =1;

                      	}
                      
					  	
                     
                    
                     PRINTF("Dir 11111111111111111\n");
                      
				  	break;
        		
                 case START_PRES:
				   motor_ref.motor_run =1;
				   HALL_Pulse =0;
                   PID_PWM_Duty=50;
                    en_t.DIR_flag = 1;
                   en_t.HorizonStop_flag =0;
				   en_t.HorizonStop_flag=0;
                   PRINTF("START_KEY @@@@@@@@@@@@@@@@@@@@@@@@@\n\r");
              
				  break;
		
				 case DIR_CCW_PRES: //Dir = 0;PTE24 = CCW,KEY3,水平方向
				    Dir = 0 ; //
					en_t.DIR_flag = 1;
				   HALL_Pulse =0;
				   PRINTF("DIR =000000000\r\n");
	  			   
			    
				 
			       
			   
			
           		break;
				case MOTOR_STOP_PRES:
                     en_t.HorizonStop_flag=0;
					 motor_ref.motor_run = 3;
					 HALL_Pulse =0;
				     BLDCMotor.Lock_Time=0;
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
   
	BLDCMotor.Lock_Time=0;
    motor_ref.motor_run = 0;
    en_t.HorizonStop_flag=0;
	PMW_AllClose_ABC_Channel();
	HALL_Pulse =0;
	#ifdef DRV8302
    GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,0);
    #endif 
	PRINTF("interrupte has happed  \r\n");
	                  
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/*****************************************END**********************************************/


