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
__IO int32_t  PID_PWM_Duty;
BLDC_Typedef BLDCMotor;


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
 
     uint8_t ucKeyCode=0,z=0,m=0;
     uint8_t RxBuffer[5],i,ki,kp,kd,k0,judge_n;
	 float KP,KI,KD;
     volatile uint16_t Time_CNT,EnBuf[2]={0,0};
	 volatile int32_t mCurPosValue,mHoldPos,eIn_n=0,H,V,Dff;
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
            
            if(eIn_n > 200 ){
                   
				   j++;
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
                           // DelayMs(10);
                            EnBuf[1]= 0;
							EnBuf[0]=0;
							PRINTF("Z==1 ZZZZZZZZZZZ \n\r");
							 HALL_Pulse =0;
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
							    		PRINTF("HALL > 0\n\r");
										en_t.Horizon_J_n++;
								 		if((en_t.Horizon_J_n==1)&&(en_t.First_V_dec !=1)){
												en_t.Horizon_HALL_Pulse =HALL_Pulse;
										        en_t.Home_flag = 1;
												en_t.Horizon_Position = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
												en_t.First_H_dec = 1;
												PRINTF("HorizP_1 = %d\r\n",en_t.Horizon_Position);
												PRINTF("Hor_HALL_1 = %d\r\n",en_t.Horizon_HALL_Pulse);
												HALL_Pulse =0;
								 		}
										else{

												if((judge_n==2)&&(en_t.End_V_flag !=1)){

												        /*第一检测到水平位置，进行第二次到垂直位置*/
													    
														if(en_t.First_H_dec==1){

														    en_t.Vertical_HALL_Pulse =HALL_Pulse;
													        en_t.End_V_flag = 1;
															en_t.Vertical_Position = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
															
															PRINTF("VerPos_2 = %d\r\n",en_t.Vertical_Position);
															PRINTF("VerHal_2 = %d\r\n",en_t.Vertical_HALL_Pulse);
															HALL_Pulse =0;
															
															
														}
														else{
															/**/
															en_t.Horizon_HALL_Pulse =HALL_Pulse;
													        en_t.End_V_flag = 1;
															en_t.Horizon_Position = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
														
															PRINTF("HoPos_2 = %d\r\n",en_t.Horizon_Position);
															PRINTF("HoHal_2 = %d\r\n",en_t.Horizon_HALL_Pulse);
															HALL_Pulse =0;
														}
											 
										        }
										 
					                     }
								 }
								else{
										PRINTF("HALL < 0 \r\n");
										en_t.Vertical_J_n++;
										if((en_t.Vertical_J_n == 1)&&( en_t.First_H_dec!=1)){
											 PRINTF("-HallN 1=- %d\r\n", HALL_Pulse );
											 en_t.Vertical_HALL_Pulse = HALL_Pulse;
									         en_t.Home_flag = 1;
											 en_t.First_V_dec =1;
											 en_t.Vertical_Position = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
											
											 PRINTF("--Ver_1 = %d\r\n",en_t.Vertical_Position);
											 PRINTF("--Ver_HALL_1 =- %d\r\n",en_t.Vertical_HALL_Pulse);
											 HALL_Pulse =0;

										
										}
										else if((judge_n==2)&& (en_t.End_H_flag !=1)){ 

											
											/*第一次检测到垂直位置，第二还是检测到水平位置,hall <0*/
											if(en_t.First_V_dec == 1){
												
												 en_t.Horizon_HALL_Pulse = HALL_Pulse;
												 en_t.End_V_flag = 1;
												 en_t.Horizon_Position = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
												
												 PRINTF("--Hor_2 = %d\r\n",en_t.Horizon_Position);
												 PRINTF("--Hor_HALL_2 =- %d\r\n",en_t.Horizon_HALL_Pulse);
												 HALL_Pulse =0;
											}
											else{
												
												/*第二次垂直位置*/
												en_t.Vertical_HALL_Pulse = HALL_Pulse;
										        en_t.End_V_flag = 1;
												en_t.Vertical_Position = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
												
												 PRINTF("--VerPos_2= %d\r\n",en_t.Vertical_Position);
												 PRINTF("--Ver_HALL_2 =- %d\r\n",en_t.Vertical_HALL_Pulse);
												 HALL_Pulse =0;
												

											}
												
										}
								 }
						 if(judge_n==2){
						 	      en_t.eInit_n++;
								  HALL_Pulse =0;
						         
						 	}
								
                      }
                  }
                
        }
        if(eIn_n > 200)eIn_n =0;
        
				
				
        }

	  
	   
#endif 
         
    /***********motor run main*********************/
     if(motor_ref.motor_run == 1)
      {
   		  if(en_t.eInit_n == 1){
			   if(Dir == 1){
			   	   PWM_Duty =50;
			   	}
			   else
			     PWM_Duty = PID_PWM_Duty;
   		  	}	
   		 
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
		 	if(eIn_n == 0xffff)eIn_n =1;
           Time_CNT++;
#if 1    
        /* 100ms arithmetic PID */
    	if(Time_CNT % 100== 0){

			//	mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
			if(Dir == 0)//CCW HB0 = Horizion
			{
					if(en_t.eInit_n==0)iError = 1;
					else
                    {
				 		iError = mCurPosValue - en_t.Horizon_Position ; //
						#ifdef DEBUG_PRINT
                       		PRINTF("HB0= %d \n\r",en_t.Horizon_Position);
					      	PRINTF("mCurPosValue= %d \n\r",mCurPosValue);
						    PRINTF("currHALL= %d \n\r",HALL_Pulse);
						#endif
				       	H = iError;
					   	if(H>=0)
				      	PRINTF("H = %d \n\r",H);
					   	else
					   	PRINTF("-H = - %d \n\r",H);
                       	if(((H == 0)||(H == -1||H ==1)||(H==2||H==-2)||(H==3||H==-3)||(H==4||H==-4)||(H==5||H==-5))&&(HALL_Pulse>1)){
							

					        m++;
							if(m==1)
							{
								
								// uwStep = HallSensor_GetPinState();
                                // HALLSensor_Detected_BLDC(PWM_Duty);

							}
						    else if(m==4){
								 
								 PMW_AllClose_ABC_Channel();
                                 motor_ref.motor_run =0;
								 PRINTF("H= %d\r\n",H);
								 PRINTF("PID_PWM_Duty = %d\r\n",PID_PWM_Duty);
								 HALL_Pulse =0;
								  m=0;
								 PRINTF("HHHHHHHHHH\r\n");
								  BLDCMotor.Lock_Time=0;
								  iError =0;
						    }
								
						}
					}
					
					dError_sum += iError; /*误差累计和*/
					
				    if(dError_sum > 1000)dError_sum =1000; //积分限幅
					if(dError_sum < -1000)dError_sum = -1000; 
                    PID_PWM_Duty = (int32_t)(iError *KP + dError_sum * KI + (iError - last_iError)*KD);//proportion + itegral + differential
			        #ifdef DEBUG_PRINT
						if(PID_PWM_Duty > 0)
							PRINTF("PID pwm= %d\r \n",PID_PWM_Duty);
						else if(PID_PWM_Duty == 0)PRINTF("PID pwm= %d\r \n",PID_PWM_Duty);
						else PRINTF("-PID pwm= -%d\r \n",PID_PWM_Duty);
					#endif
					PID_PWM_Duty = abs(PID_PWM_Duty);
					if(PID_PWM_Duty >=50)PID_PWM_Duty=50;

                 	last_iError = iError;
					PWM_Duty = PID_PWM_Duty;
					HALL_Pulse =0;
					
			}
			else{  //Vertical Position judge is boundary
			     
                   if(en_t.eInit_n ==1){
				  
                    #ifdef DEBUG_PRINT 
                      PRINTF("VENDPOS= %d \n\r",en_t.Vertical_Position);
					  PRINTF("VcurrHALL= %d \n\r",HALL_Pulse);
				      PRINTF("mCurPosValue= %d \n\r",mCurPosValue);
				    #endif 
				       en_t.Vertical_Position = abs(en_t.Vertical_Position);
				       mCurPosValue = abs(mCurPosValue);
				       V = (int32_t)( en_t.Vertical_Position  - mCurPosValue);
					 #ifdef DEBUG_PRINT
						 if(V>=0)
					      	PRINTF("V == %d \n\r",V);
						 else
						   	PRINTF("-V = - %d \n\r",V);
						    PRINTF("firstlocktime = %d\r\n",BLDCMotor.Lock_Time);
					 #endif
					   V = abs(V);
					   HALL_Pulse = abs(HALL_Pulse);
					   PRINTF("V= %d  \r\n",V);
					   PRINTF("VcurrHALL= %d \n\r",HALL_Pulse);
                      if(( V<=5 )&&(HALL_Pulse>10)){
					  	      BLDCMotor.Lock_Time ++;
							  BLDCMotor.Position = V;
					          
							 if(BLDCMotor.Lock_Time >=3){
							 	if(BLDCMotor.Position <=5){
									
									 PRINTF("V= %d \r\n",V);
									 PMW_AllClose_ABC_Channel();
                                     motor_ref.motor_run =0;
									 PRINTF("locktime = %d\r\n",BLDCMotor.Lock_Time);
									 BLDCMotor.Lock_Time=0;
									 PRINTF("VVVVVVVV\r\n");
							 	}
				   	     
			   	   	         }
                      }
					   
                 }
				 HALL_Pulse =0;
			}
						
        }
	   if(Time_CNT ==100){
			Time_CNT = 0;
			HALL_Pulse =0;
	   	}
	   #endif 
	} 
    else { 
            PMW_AllClose_ABC_Channel();
			HALL_Pulse =0;
			#ifdef DRV8302
		 	GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,0);
		    #endif 
	      DelayMs(50);
	      GPIO_PortToggle(GPIOD,1<<BOARD_LED1_GPIO_PIN);
	      DelayMs(50);
	     
          if( motor_ref.motor_run == 3){
		  	
				  UART_ReadBlocking(DEMO_UART, RxBuffer, 5);
                  for(i=0;i<5;i++){
				  	     
						  if(RxBuffer[0]==0xff){
							  kp=RxBuffer[1];
		                      ki=RxBuffer[2];
							  kd=RxBuffer[3];
		                      motor_ref.motor_run =RxBuffer[4];
		                      PRINTF("KP KI KD = %d %d %d \n\r",kp,ki,kd);
		                      KP = (float)kp/10;
		                      KI = (float)ki/100;
		                      KD = (float)kd/10;
					  	  }
						  else{
								k0=RxBuffer[0];
								 PRINTF("USART Error !!!!!!!!\n\r");
								 PRINTF("k0 = %d \n\r",k0);
								 
						  }
                     
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
			          BLDCMotor.Lock_Time=0;
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
				    HALL_Pulse =0;
                      
				  	break;
        		
                 case START_PRES:
                   
				   motor_ref.motor_run =1;
				   HALL_Pulse =0;
                   BLDCMotor.Lock_Time=0;
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
	  			   
			     if(Dir==1) // Dir = 0; //茂驴陆茂驴陆脢卤茂驴陆茂驴陆茂驴陆茂驴陆脳陋
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
				   HALL_Pulse =0;
				   BLDCMotor.Lock_Time=0;
                 UART_WriteBlocking(DEMO_UART, printx2, sizeof(printx2) - 1);
			   
			
           		break;
				case MOTOR_STOP_PRES:
                   
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






/*******************************************************************************************/


