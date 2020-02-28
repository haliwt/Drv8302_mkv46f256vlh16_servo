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
 
     uint8_t ucKeyCode=0,kp,ki,kd,z=0,w=0,m=0;
     uint8_t RxBuffer[4],i,judge_n=0;
	 float KP,KI,KD;
     volatile uint16_t Time_CNT,EnBuf[2]={0,0};
	 volatile int32_t mCurPosValue,mHoldPos,eIn_n=0,H,V;
	 int16_t j=0;

	 
    
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
   
	
   while(1)
   {
       ucKeyCode = KEY_Scan(0);
    mHoldPos = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
       
	#ifdef DEBUG_PRINT 
		   PRINTF("CPHod: %d\r\n", mHoldPos);
	#endif
			
		
#if 1
	 /***********Position :Home and End*****************/
        if(en_t.eInit_n ==0)
        {
			PWM_Duty =50;
            
            if(eIn_n > 300 ){
                   
						   j++;

                           abs(HALL_Pulse);
	                       if(j==1){
	                                EnBuf[0]=mHoldPos;
	                                
 								   }
						   else if(j==7){
						   			EnBuf[1]= mCurPosValue;
	                               }

						   if(j==10)j=0;

							/*judge and setup this Home and End Position */
					      if((EnBuf[0]==EnBuf[1])&&(EnBuf[0]>=1 || EnBuf[1]>=1)){
                             
	                           z++;
	                           if(z==1) {
	                                DelayMs(10);
	                                EnBuf[1]= 0;
									EnBuf[0]=0;
									PRINTF("Z==1 ZZZZZZZZZZZ \n\r");
	                             }
							  else if(z ==2){
										judge_n++; /*judge home and end position twice*/
										PRINTF("Z==2 ############# \n\r");
		                                PMW_AllClose_ABC_Channel();
		                                motor_ref.motor_run = 0;
		                                eIn_n=0;
		                                z=0;
										
										/*set Home position and End position*/
										 if(HALL_Pulse >=0){
									    		PRINTF("HallN= %d\r\n", HALL_Pulse );
												en_t.Horizon_J_n++;
										 		if(en_t.Horizon_J_n==1){
														en_t.Horizon_HALL_Pulse =HALL_Pulse;
												        en_t.Home_flag = 1;
														en_t.Horizon_Position = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
											
														PRINTF("HorizP = %d\r\n",en_t.Horizon_Position);
										 		}
												else{

													if((judge_n==2)&&(en_t.End_flag !=1)){
												
						                          	 en_t.Vertical_HALL_Pulse = HALL_Pulse;	
													 en_t.End_flag =1;
													 en_t.Vertical_Position = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
													
													 PRINTF("VertialP = %d\r\n",en_t.Vertical_Position);
											
												 	}
							
												}
												
						                 }
										 else{
										        PRINTF("-HallN=- %d\r\n", HALL_Pulse );
												en_t.Vertical_HALL_Pulse = HALL_Pulse;
										        en_t.End_flag = 1;
												en_t.Vertical_Position = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
												
												PRINTF("--VertialP = %d\r\n",en_t.Vertical_Position);
										 }
										if(judge_n==2)en_t.eInit_n++;
										
	                              }
                          }
                        
                }
                if(eIn_n > 300)eIn_n =0;
                
				
				
        }

	  
	   
#endif 
         
    /***********motor run main*********************/
     if(motor_ref.motor_run == 1)
      {
   		  if(en_t.eInit_n == 1)PWM_Duty = PID_PWM_Duty;
				
   		 
		   #ifdef DRV8302 
            GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,1);
		   #endif 
		  
         
         
	      uwStep = HallSensor_GetPinState();
          HALLSensor_Detected_BLDC(PWM_Duty);
          
                    mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
                    #ifdef DEBUG_PRINT 
                     PRINTF("CurrPos : %d\r\n", mCurPosValue);
                    #endif
               
					
            
           eIn_n ++;    
        
			
          Time_CNT++;
#if 1    
        /* 100ms arithmetic PID */
    	if(Time_CNT % 100== 0){

						mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
						if(Dir == 0)//CCW HB0 = Horizion
						{
								if(en_t.eInit_n==0)iError = 0;
								else
							 		iError = mCurPosValue - en_t.Horizon_Position ; //
							 	if(en_t.eInit_n==1){

									PRINTF("HB0= %d \n\r",en_t.Horizon_Position);
							      	PRINTF("mCurPosValue= %d \n\r",mCurPosValue);
								    mCurPosValue = abs(mCurPosValue);
							       	H = (int32_t)( en_t.Horizon_Position - mCurPosValue);
								   	if(H>=0)
							      	PRINTF("H = %d \n\r",H);
								   	else
								   	PRINTF("-H = - %d \n\r",H);
								    H=abs(H);
								    PRINTF("abs_H == %d \n\r",H);
                                   	if((H == 0)||(H< 5)){
								   			m++;
										    if(m==1)DelayMs(10);
											else{
											 PMW_AllClose_ABC_Channel();
		                                     motor_ref.motor_run =0;
											 PRINTF("hor = %d \n\r",m);
											 m=0;
											}
										
									}
								}
								iError= abs(iError);
								PRINTF("iError = %d \r\n",iError);
								if(iError<37) //´Å±àÂëµÄÎó²î iError <36
		    						iError = 0;
							
								dError_sum += iError; /*Îó²îÀÛ¼ÆºÍ*/
								if(dError_sum > 1000)dError_sum =1000; //»ý·ÖÏÞ·ù
								if(dError_sum < -1000)dError_sum = -1000; 
								PID_PWM_Duty = (int32_t)(iError *KP + dError_sum * KI + (iError - last_iError)*KD);//proportion + itegral + differential
								PRINTF("PID pwm= %d\r \n",PID_PWM_Duty);
								if(PID_PWM_Duty >=50)PID_PWM_Duty=50;
		                        if(w==2){
									 PMW_AllClose_ABC_Channel();
		                             motor_ref.motor_run =0;
									 PRINTF("W = %d \n\r",w);
									 w=0;
								}
								if(m==2){
									 PMW_AllClose_ABC_Channel();
		                             motor_ref.motor_run =0;
									 PRINTF("hor = %d \n\r",m);
									 m=0;
								}
		                     
								  
		                        
								last_iError = iError;
								PWM_Duty = PID_PWM_Duty;
						}
						else{  //Vertical HB1
						      PID_PWM_Duty = 50;
                              if(en_t.eInit_n ==1){
                                  
                                  PRINTF("VEND= %d \n\r",en_t.Vertical_Position);
							      PRINTF("mCurPosValue= %d \n\r",mCurPosValue);
							      en_t.Vertical_Position = abs(en_t.Vertical_Position);
							      mCurPosValue = abs(mCurPosValue);
							       V = (int32_t)( en_t.Vertical_Position - mCurPosValue);
								   if(V>=0)
							      PRINTF("V == %d \n\r",V);
								   else
								   	 PRINTF("-V = - %d \n\r",V);
								    V=abs(V);
								    PRINTF("abs_V == %d \n\r",V);
                                   if((V == 0)||(V==1)){
								   	
										
										 PMW_AllClose_ABC_Channel();
	                                     motor_ref.motor_run =0;
										 PRINTF("W = %d \n\r",w);
										 w=0;
										}
											
                                 }
                             
							  
						}
						
						
                        
                         
		}
	   if(Time_CNT ==100)
			Time_CNT = 0;

              
#endif 
	} 
    else { 
            PMW_AllClose_ABC_Channel();
			#ifdef DRV8302
		 	GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,0);
		    #endif 
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
         PRINTF("Motor Stop !!!!!!!!!!! \r\n");
         PRINTF("motor_run = %d \r\n",motor_ref.motor_run );
                
    }
            
    
   
   /**********8Key process********************/  
   if(ucKeyCode !=KEY_UP){

	  switch(ucKeyCode){ 
                 
                  case DIR_CW_PRES ://Dir =1 ,PTE29-CW,KEY1
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
                   
				   motor_ref.motor_run =1;
                
                   PRINTF("START_KEY @@@@@@@@@@@@@@@@@@@@@@@@@\n\r");
#if 0
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
#endif              
				  break;
		
				 case DIR_CCW_PRES: //Dir = 0;PTE24 = CCW,KEY3
				   // Dir = 0 ; //
				
				   PRINTF("DIR =0\r\n");
	  			
			     if(Dir==1) // Dir = 0; //Ã¯Â¿Â½Ã¯Â¿Â½ÃŠÂ±Ã¯Â¿Â½Ã¯Â¿Â½Ã¯Â¿Â½Ã¯Â¿Â½Ã—Âª
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






/*******************************************************************************************/


