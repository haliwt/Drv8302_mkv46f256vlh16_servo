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
	 volatile int32_t dError_sum,dvError_sum;
	 volatile int32_t iError,last_iError,ivError,last_ivError ;
    
 
     uint8_t ucKeyCode=0,eIn[1]={0};
     uint8_t RxBuffer[5],i,k0,judge_n=0;
     uint16_t z=0,m=0,mn;

	 volatile int16_t DectBuf[6];
     volatile uint16_t Time_CNT,EnBuf[2]={0,0};
	 volatile int32_t mCurPosValue=0,mHoldPos=0,HDff,VDff,Dff;
	 uint32_t eIn_n= 0,lstartHorValue=0,lhoradd,p;
	 int16_t j=0,ldiff,lkeydir,lhorizonpos;
   
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
       
	
	//	  printf("currePosHold: %ld\r\n", mHoldPos);
	//     printf("position diff = %d\r\n", en_t.Pos_diff);
          printf("en_t.eInit_n: %d\r\n", en_t.eInit_n);
	 
#if 1
	 /***********Position :Home and End*****************/
        if(en_t.eInit_n ==0)
        {
          //  printf("itself %%%%%%%%%%%%%\r\n"); 
            PWM_Duty=50 ;
			LED2=!LED2;
			if(eIn_n >= 3){
                  
				   j++;
                   if(j==1){
                            EnBuf[0]=mHoldPos;
                            
							   }
				   else if(j==3){
				   			EnBuf[1]= mCurPosValue;
                           }

                   if(j==3){  /*judge and setup this Home and End Position */
					
			               if((EnBuf[0]==EnBuf[1])){
                     			/*judge home and end position twice*/
                             printf("################################################################################# \n");
                              printf("################################################################################# \n");
                               printf("################################################################################# \n");
                                PMW_AllClose_ABC_Channel();
                                motor_ref.motor_run = 0;
								/*To judge  Home position and End position*/
								
								 if(Dir ==0){/*motor to honrizon Position*/
								 	    judge_n++; 
							    		PRINTF("Horizon judge_n = %d\r\n",judge_n);
								 		if((judge_n==1||judge_n==3)){
											 
										      
													en_t.First_H_dec = 1;
													en_t.oneKey_H_flag =1;
													
					
													printf("HorizPos_1 = %d\r\n",en_t.X_axis);
													HALL_Pulse =0;
													if(abs(en_t.X_axis)< 200 ){
														Dir =0 ; /* motor move horizon  */
														PWM_Duty =50 ;
														motor_ref.motor_run = 1;
                                                        en_t.X_axis = mHoldPos;
														printf("HorizonPos 1!!!!!! =%d \r\n",en_t.X_axis);
													}
													else if(abs(en_t.X_axis) > 800){
														/* one key study*/
														Dir =1 ; /* motor move vertical  */
														PWM_Duty =50 ;
													    motor_ref.motor_run = 1;
                                                        en_t.Y_axis = 0;
		                                                printf("HorizonPos 1_3 =%d \r\n",en_t.X_axis);
														if(abs(en_t.X_axis) <800)
															en_t.X_axis=1024;
													}
													
												
								 			}
										else{ /*judge_n ==2,from vertical to horizon position*/
												PRINTF("judge_n = %d\r\n",judge_n);
												if((judge_n==2)||(judge_n==4)){

												     if(judge_n ==4)judge_n =2;
													en_t.End_H_flag = 1;
													en_t.X_axis = mHoldPos;
													 HALL_Pulse =0;
													PRINTF("HorizonPos_2 = %d\r\n",en_t.X_axis);
													if(abs(en_t.X_axis) > 800){
	 													Dir =0 ; /* motor move horizon  */
														PWM_Duty =50;
														motor_ref.motor_run = 1;
													    printf("Hor_2_0 = %d\r\n",en_t.X_axis);

													    }
													
													else if(abs(en_t.X_axis) < 200){
													/* one key study*/
													Dir =1 ; /* motor move horizon  */
													PWM_Duty =50;
													motor_ref.motor_run = 1;
													printf("Hor_4_0 = %d\r\n",en_t.X_axis);
													}
												  
												}
										 
					                     }
								 }
								/*******************************************************/
								/*******************VERTICAL****************************/
								else{ /*direction to vertical Dir = 1,from horizon to vetical*/
									   judge_n++; 
								        /*HALL_Pulse < 0*/
										PRINTF("Vertical judge_n = %d\n\r",judge_n);
										PRINTF("End_H_flag = %d \r\n",en_t.End_H_flag);
										PRINTF("First_H_dec = %d \r\n",en_t.First_H_dec);
								         en_t.Y_axis = mCurPosValue;
                                          printf("VerPos_dec= %d\r\n",en_t.Y_axis);
										if(judge_n==1 ||judge_n==3 ){
											 
											    en_t.First_V_dec =1;
												en_t.oneKey_V_flag =1;
											 
												 printf("Vertical_Position = %ld\r\n",en_t.Y_axis);
                                                if(abs(en_t.Y_axis)<1000 ){
                                                   en_t.mini_value =mHoldPos;
                                                   for(p=0;p<1000000;p++){
                                                   Dir =1 ; /* motor move horizon  */
                                                   PWM_Duty =50;
                                                  
                                                   uwStep = HallSensor_GetPinState();
						                           HALLSensor_Detected_BLDC(PWM_Duty);
                                                    mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
                                                    if(abs(mCurPosValue) > 900){
                                                         eIn[0]=1;
                                                         en_t.eInit_n=eIn[0] ;
                                                         en_t.Y_axis = mCurPosValue;
                                                         en_t.X_axis =0;
                                                         break;
                                                    }
                                                   }
                                                   Dir =1;
                                                    motor_ref.motor_run = 1;
                                                   printf("--VerPos_1_1 = %ld\r\n",en_t.Y_axis);
                                                   
												   
                                                  
                                                }
												else if(abs(en_t.Y_axis)> 1000){
												   Dir =0 ; /* motor move horizon  */
                                                   PWM_Duty =50;
                                                   motor_ref.motor_run = 1;
                                                   en_t.X_axis =0;
                                                   printf("3333333333333333333 \n");
                                                   
												   
                                                   printf("--VerPos_1_3 = %ld\r\n",en_t.Y_axis);
												   
                                                  
												}
											}
										    else if((judge_n==2)||(judge_n ==4)){ 
											    
													if(judge_n==4)judge_n=2;
													en_t.Y_axis = mHoldPos;
												   if(abs(en_t.mini_value) <1000 && en_t.First_V_dec ==1&& abs(mHoldPos)<1000){
													 judge_n=0;  
													Dir =1 ; /* motor move vertical  */
                                                   PWM_Duty =50;
                                                   motor_ref.motor_run = 1;
												   en_t.Y_axis = mHoldPos;
                                                   printf(" dir=2_0 VerPos2_0=%d \r\n",en_t.Y_axis);
                                                 

												   }
													else{
												    en_t.End_V_flag = 1;
												
			
												    HALL_Pulse =0;
												 
												 if(abs(en_t.Y_axis) >= 800){

													 Dir =1 ; /* motor move vertical  */
													 PWM_Duty =50;
													 motor_ref.motor_run = 1;
													 PRINTF("--VerPos_2-0 = %d\r\n",en_t.Y_axis);
												 	}
												 else if(abs(en_t.Y_axis) < 100){
														  /*one key study*/
														 Dir =0 ; /* motor move horizon  */
														 PWM_Duty =50;
														 motor_ref.motor_run = 1;
														 printf("--VerPos_2-2 = %d\r\n",en_t.Y_axis);
														 printf("ML auto dir reverse = 2@@@@@@@@@@@ \n");
												 	}
													
												}
									   		 }	
								}
						
						
						}//end if(EndBuf[0]==EndBuf[1])
                  
                     }
                     if(judge_n==3 || judge_n == 1||judge_n==5){
                            if((Dir == 0)&& judge_n ==1 && en_t.First_H_dec !=0){
                               eIn[0]=1;
                                en_t.eInit_n=eIn[0];     
                            }
                            else if((judge_n ==3)&& Dir==1){
                              en_t.eInit_n++; 
                            }
						 	      
                     
								  printf("eInit_n = %d \r\n",en_t.eInit_n);
								  en_t.HorizonStop_flag=0;
								  HALL_Pulse =0;
						
                                  iError =0;
								  en_t.VH_Total_Dis=1;
                                  PID_PWM_Duty=50;
								PMW_AllClose_ABC_Channel();
                                motor_ref.motor_run = 0;
								  /* algorithm */
								  if(en_t.X_axis > en_t.Y_axis)
								  	en_t.mini_value = en_t.Y_axis;
								  else en_t.mini_value = en_t.X_axis ;
								  en_t.VH_Total_Dis = abs(en_t.X_axis -en_t.Y_axis);//Toatal= horizonPos-verticalPos
				     
                               }
				    if(j>=3){
                     j=0;
                    
			         } //end if(eIn_n >=3)
            
        }
        if(eIn_n >= 3)eIn_n =0;
		}//end en_t.eIn_n == 0
        
#endif 
    /********************************************************************************/     
    /***********motor run main*********************/
     if((motor_ref.motor_run == 1)&&(en_t.HorizonStop_flag !=2))
     {
   		  if(en_t.eInit_n == 1){
            
           if(en_t.DIR_flag ==1)PWM_Duty =50;
			  else
			  PWM_Duty = PID_PWM_Duty;
			   
   		  	}
		  else if(en_t.eInit_n == 0)
		  {
			if(Dir ==0){/*start detected horizon position motor  speed slowly limit*/
                  m++;
				  if(m >1000)
				  {
					  mn ++;
					  if(mn<=50) {
					 
	                     PWM_Duty = 50 - mn;
	                    
	                  }
					  else PWM_Duty = 0 ;
				  }
				  else if(m< 500) PWM_Duty = 30 ;
#if 1
             
		
               if(abs(en_t.X_axis) > 800 && en_t.Pos_diff >0){
						printf("HorizonPos >800 \r\n");
			   		    ldiff =abs(mCurPosValue);
						if(ldiff > 800){

						for(z=0;z<100;z++){
						Dir =1;
						PWM_Duty =30;
						uwStep = HallSensor_GetPinState();
						HALLSensor_Detected_BLDC(PWM_Duty);
						PRINTF("HorPowerPos-2 : %d\r\n", mCurPosValue);
						Dir =0;
						
						}
						 
					}
							
               	 }
                 else if(abs(en_t.X_axis)  < 200  && en_t.oneKey_V_flag !=1){
							    printf("Horizon_Pos <200 @@@@@@\r\n");
								mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
								printf("200mCurrPos = %d \r\n",mCurPosValue);
								printf("200PWM_Duty = %d \r\n",PWM_Duty);
								ldiff = abs(mCurPosValue);
		
								if(ldiff > 800 ){
								for(z=0;z<100;z++){
			                    Dir =1;
			                    PWM_Duty =30;
			                    uwStep = HallSensor_GetPinState();
			                    HALLSensor_Detected_BLDC(PWM_Duty);
								printf("HorStartPos-1 : %ld\r\n", mCurPosValue);
			                    Dir =0;
								en_t.mini_value =2;
							   }
							 		

						       }
								
				     }
                  else if(en_t.oneKey_V_flag ==1){

				  	            ldiff = abs(mCurPosValue);
								if(ldiff < 200 ){
								for(z=0;z<100;z++){
			                    Dir =1;
			                    PWM_Duty =30;
			                    uwStep = HallSensor_GetPinState();
			                    HALLSensor_Detected_BLDC(PWM_Duty);
								printf("HorStartPos-1 : %ld\r\n", mCurPosValue);
			                    Dir =0;
								en_t.mini_value =2;
							   }
									PWM_Duty=0; 	

						}

				  
                  	}
                 #endif 
				} //end if(Dir ==0)
       
              }//end if(en_t.eIn_n == 0)
			
		
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
	           printf("judge_n = %d \r\n",judge_n);
	           printf("run_HALL_dir = %ld\r\n", HALL_Pulse);
	   		   printf("motor start pwm= %d\r\n",PWM_Duty);
			   printf("motor Dir = %d\n",Dir);
			   printf("VerticalPos = %ld\r\n",en_t.Y_axis);
			   printf("HorzionPos = %ld\r\n",en_t.X_axis);
	           printf("POS_diff = %d \r\n", en_t.Pos_diff);
	           printf("en_t.eInit_n: %d\r\n", en_t.eInit_n);
		  }
		   
#endif 
		    if(en_t.HorizonStop_flag ==1 && en_t.eInit_n ==1){

						mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
						lhorizonpos =abs(abs(en_t.X_axis) - abs(mCurPosValue));
						
						printf("stop lhorizonpos = %d \n",lhorizonpos);
					    if(lhorizonpos <=300 ||lhorizonpos > 700){

						for(z=0;z<150;z++){
						Dir =1;
						PWM_Duty =30;
						uwStep = HallSensor_GetPinState();
						HALLSensor_Detected_BLDC(PWM_Duty);
						printf("HorStopPos flag=1 300 : %d\r\n", mCurPosValue);
						Dir =0;
						PWM_Duty=0; 
						uwStep = HallSensor_GetPinState();
						HALLSensor_Detected_BLDC(PWM_Duty);
						DelayMs(2);
						mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
						if(abs(mCurPosValue)==(abs(en_t.X_axis -150))||abs(mCurPosValue)>(abs(en_t.X_axis)-150))
				        en_t.HorizonStop_flag =2;
						}
						//PWM_Duty=0; 
					}
					
			     }
		   eIn_n ++; 
           if(eIn_n > 0xffffe){
            
		       if(en_t.eInit_n ==1) eIn_n = 1;
               else eIn_n =0 ;
           }
           Time_CNT++;
#if 1    
        /* 100ms arithmetic PID */
    	if((Time_CNT % 25== 0)&&(en_t.eInit_n == 1)&&(en_t.HorizonStop_flag !=2)&&(en_t.HorizonStop_flag !=1)){
   
          //  PRINTF("HorizonStandPos: %d\r\n", en_t.Horizon_Position);
			mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
			//printf("mCurPosValue TIme_cnt = %d \r\n",mCurPosValue);
			if(Dir == 0)//CCW HB0 = Horizion
			{
						Horizon_Accelerate();
				
			}
            /***************************************************/
            /****************************************************/
            /**************************************************/
			else{  //Vertical Position judge is boundary
					  
					Vertical_SlowDown();
			    
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
		  #if AUTOMATICA 
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


