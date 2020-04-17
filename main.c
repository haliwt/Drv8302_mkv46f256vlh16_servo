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
__IO int32_t HALL_Pulse;  

output_t  motor_ref;
encoder_t en_t;
int32_t PID_Result;
PID_TypeDef  sPID;
__IO int32_t  PID_PWM_Duty;
BLDC_Typedef BLDCMotor;

struct _pid_reference pid_r={0.1f,0.01f,0.1f,5.0f,0.01f,2.0f};
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
    
 
     uint8_t ucKeyCode=0;
     uint8_t RxBuffer[8],i,k0,judge_n;
     uint16_t z=0,ldectnum=0, tempadd,m=0;

	 volatile int16_t DectBuf[6];
     volatile uint16_t Time_CNT,EnBuf[2]={0,0};
	 volatile int32_t mCurPosValue,mHoldPos,HDff,VDff,Dff;
	 uint32_t eIn_n= 0,mn=0,lstartHorValue=0,lhoradd;
	 int16_t j=0,ldiff,lkeydir, lverticalpos,lhorizonpos,lstarthor;
   
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
	
   while(1)
   {
    ucKeyCode = KEY_Scan(0);

    mHoldPos = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
	en_t.Pos_diff = (int16_t)ENC_GetHoldPositionDifferenceValue(DEMO_ENC_BASEADDR);
       
	
		  printf("currePosHold: %ld\r\n", mHoldPos);
	//     printf("position diff = %d\r\n", en_t.Pos_diff);
          printf("en_t.eInit_n: %d\r\n", en_t.eInit_n);
	 
#if 1
	 /***********Position :Home and End*****************/
        if(en_t.eInit_n ==0)
        {
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
													
													en_t.Horizon_Position = mHoldPos;
													printf("HorizPos_1 = %d\r\n",en_t.Horizon_Position);
													HALL_Pulse =0;
													if(abs(en_t.Horizon_Position)< 200 ||HALL_Pulse ==0){
														Dir =0 ; /* motor move horizon  */
														PWM_Duty =50 ;
														motor_ref.motor_run = 1;
														printf("HorizonPos 1!!!!!! =%d \r\n",en_t.Horizon_Position);
													}
													else if(abs(en_t.Horizon_Position) > 800){
														/* one key study*/
														Dir =1 ; /* motor move vertical  */
														PWM_Duty =50 ;
													    motor_ref.motor_run = 1;
		                                                printf("HorizonPos 1_3 =%d \r\n",en_t.Horizon_Position);
													}
													
												
								 			}
										else{ /*judge_n ==2,from vertical to horizon position*/
												PRINTF("judge_n = %d\r\n",judge_n);
												if((judge_n==2)||(judge_n==4)){

												     if(judge_n ==4)judge_n =2;
													en_t.End_H_flag = 1;
													en_t.Horizon_Position = mHoldPos;
													 HALL_Pulse =0;
													PRINTF("HorizonPos_2 = %d\r\n",en_t.Horizon_Position);
													if(abs(en_t.Horizon_Position) > 800){
	 													Dir =0 ; /* motor move horizon  */
														PWM_Duty =50;
														motor_ref.motor_run = 1;
													    printf("Hor_2_0 = %d\r\n",en_t.Horizon_Position);

													    }
													
													else if(abs(en_t.Horizon_Position) < 200){
													/* one key study*/
													Dir =1 ; /* motor move horizon  */
													PWM_Duty =50;
													motor_ref.motor_run = 1;
													printf("Hor_4_0 = %d\r\n",en_t.Horizon_Position);
													}
												  
												}
										 
					                     }
								 }
								/*******************VERTICAL****************************/
								else{ /*direction to vertical Dir = 1*/
									   judge_n++; 
								        /*HALL_Pulse < 0*/
										PRINTF("Vertical judge_n = %d\n\r",judge_n);
										PRINTF("End_H_flag = %d \r\n",en_t.End_H_flag);
										PRINTF("First_H_dec = %d \r\n",en_t.First_H_dec);
								       
										if(judge_n==1 ||judge_n==3 ){
											 
											    en_t.First_V_dec =1;
												en_t.oneKey_V_flag =1;
											 en_t.Vertical_Position = mHoldPos;
												 printf("Vertical_Position = %ld\r\n",en_t.Vertical_Position);
                                                if(abs(en_t.Vertical_Position)<800 || HALL_Pulse == 0){
                                                   en_t.mini_value = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
                                                   Dir =1 ; /* motor move horizon  */
                                                   PWM_Duty =50;
                                                   motor_ref.motor_run = 1;
                                                   printf("ML auto dir reverse = 1010101010 !!!!!!!!!! \n");
                                                   printf("--VerPos_1_1 = %ld\r\n",en_t.Vertical_Position);
												   
                                                  
                                                }
												else if(abs(en_t.Vertical_Position)> 800){
												   Dir =0 ; /* motor move horizon  */
                                                   PWM_Duty =50;
                                                   motor_ref.motor_run = 1;
                                                   printf("3333333333333333333 \n");
                                                   printf("--VerPos_1_3 = %ld\r\n",en_t.Vertical_Position);
												   en_t.Vertical_Position = mHoldPos;
                                                  
													}
											
                                             
                                             }
										    else if((judge_n==2)||(judge_n ==4)){ 
											    
													if(judge_n==4)judge_n=2;
													en_t.Vertical_Position = mHoldPos;
												   if(abs(en_t.mini_value) <300 && en_t.First_V_dec ==1&& abs(mHoldPos)<300){
													 judge_n=0;  
													Dir =1 ; /* motor move horizon  */
                                                   PWM_Duty =50;
                                                   motor_ref.motor_run = 1;
												   en_t.Vertical_Position = mHoldPos;
                                                   printf(" dir=2_0 VerPos2_0=%d \r\n",en_t.Vertical_Position);
                                                   printf("--VerPos_1_1 = %ld\r\n",en_t.Vertical_Position);

													}
													else{
												    en_t.End_V_flag = 1;
												
			
												    HALL_Pulse =0;
												 
												 if(abs(en_t.Vertical_Position) >= 800){

													 Dir =1 ; /* motor move horizon  */
													 PWM_Duty =50;
													 motor_ref.motor_run = 1;
													 PRINTF("--VerPos_2-0 = %d\r\n",en_t.Vertical_Position);
												 	}
												 else if(abs(en_t.Vertical_Position) < 100){
														  /*one key study*/
														 Dir =0 ; /* motor move horizon  */
														 PWM_Duty =50;
														 motor_ref.motor_run = 1;
														 printf("--VerPos_2-2 = %d\r\n",en_t.Vertical_Position);
														 printf("ML auto dir reverse = 2@@@@@@@@@@@ \n");
												 	}
													
												}
									   		 }	
								}
						if(judge_n==3 ){
						 	      en_t.eInit_n++;
								  HALL_Pulse =0;
						           lhoradd=0;
							        tempadd=0;
                                  iError =0;
                                  PID_PWM_Duty=50;
								  /* algorithm */
								  if(en_t.Horizon_Position > en_t.Vertical_Position)
								  	en_t.mini_value = en_t.Vertical_Position;
								  else en_t.mini_value = en_t.Horizon_Position ;
								  en_t.VH_Total_Dis = abs(en_t.Horizon_Position -en_t.Vertical_Position);//Toatal= horizonPos-verticalPos
						    }
						
						}//end if(EndBuf[0]==EndBuf[1])
                  
                  
                   }//end if(j==3)
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
				PWM_Duty = 50 - mn;
			    m++;
               printf("start_Hor_Horizon_Position  = %ld \r\n", en_t.Horizon_Position ); 
			   printf("mn =%d \r\n",mn);
               if(abs(en_t.Horizon_Position) > 800 && en_t.Pos_diff >0){

			   		    ldiff =abs(mCurPosValue);
						if(ldiff < 200){

						for(z=0;z<100;z++){
						Dir =1;
						PWM_Duty =30;
						uwStep = HallSensor_GetPinState();
						HALLSensor_Detected_BLDC(PWM_Duty);
						PRINTF("HorPowerPos-2 : %d\r\n", mCurPosValue);
						Dir =0;
						
						}
						PWM_Duty=0; 
					}
							
               	 }
                 else if(abs(en_t.Horizon_Position)  < 200  && en_t.Pos_diff >0 && en_t.oneKey_V_flag !=1){
							
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
									PWM_Duty=0; 	

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
                  if(m > 200 ) mn++;
                  if(mn>=50) PWM_Duty=0;
				} //end if(Dir ==0)
                else PWM_Duty = 50;
                  
              }//end if(en_t.eIn_n == 0)

		   if(en_t.HorizonStop_flag ==1){

						mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
						lhorizonpos =abs(abs(en_t.Horizon_Position) - abs(mCurPosValue));
						
						printf("stop lhorizonpos = %d \n",lhorizonpos);
					    if(lhorizonpos <=300){

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
						DelayMs(3);
						mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
						if(abs(mCurPosValue)==(abs(en_t.Horizon_Position -100))||abs(mCurPosValue)>(abs(en_t.Horizon_Position)-100))
				        en_t.HorizonStop_flag =2;
						}
						PWM_Duty=0; 
					}
					
			     }
		
		  //if(Dir == 0)
		  	{
             printf("judge_n = %d \r\n",judge_n);
           printf("run_HALL_dir = %ld\r\n", HALL_Pulse);
   		   printf("motor start pwm= %d\r\n",PWM_Duty);
		   printf("the first Dir = %d \n",Dir);
		   printf("VerticalPos = %ld\r\n",en_t.Vertical_Position);
		   printf("HorzionPos = %ld\r\n",en_t.Horizon_Position);
           printf("POS_diff = %d \r\n", en_t.Pos_diff);
		  }
		  uwStep = HallSensor_GetPinState();
          HALLSensor_Detected_BLDC(PWM_Duty);
          
	      mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
	      en_t.Pos_diff = (int16_t)ENC_GetHoldPositionDifferenceValue(DEMO_ENC_BASEADDR);

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
			if(Dir == 0)//CCW HB0 = Horizion
			{
						en_t.DIR_flag =0;

						#if 1
                        lhoradd ++;
						tempadd  = lhoradd / 10  * 50 ;
						for(i=0;i<tempadd;i++){
							Dir =1;
	                        PWM_Duty =30;
	                        uwStep = HallSensor_GetPinState();
	                        HALLSensor_Detected_BLDC(PWM_Duty);
							Dir =0;
                            printf(" tempadd \\\\\\ =  %d \n", tempadd);
						}
                        if(tempadd >=50)tempadd =0;
						printf("lhoradd!!!!!!!!!!!!!!!!!! = %d \n",lhoradd);

						#endif 
						
						iError =abs(mCurPosValue) - abs(en_t.Horizon_Position) ; /*  pid error  */
					    printf("mCurPosValue= %ld \n\r",mCurPosValue);
						printf("iError = %ld \r\n",iError);
					    if(abs(en_t.Horizon_Position) < 200 ){
						    lhorizonpos = abs(mCurPosValue);
                              if(lhorizonpos < 200){
                            for(z=0;z<400;z++){
                            Dir =1;
                            PWM_Duty =30;
                            uwStep = HallSensor_GetPinState();
                            HALLSensor_Detected_BLDC(PWM_Duty);
							 printf("Stop300 CurrPos : %ld\r\n", mCurPosValue);
                             Dir =0;
							}
							 lhoradd=0;
							 tempadd=0;
						   }
							 HALL_Pulse =0;	
							en_t.HorizonStop_flag =1;
							 
						}
						else if(abs(en_t.Horizon_Position) >800 && (en_t.Pos_diff > 0)){

								lhorizonpos =abs(mCurPosValue);
							    if(lhorizonpos > 800){
							  //  en_t.HorizonStop_flag =2;
	                            for(z=0;z<150;z++){
		                            Dir =1;
		                            PWM_Duty =30;
		                            uwStep = HallSensor_GetPinState();
		                            HALLSensor_Detected_BLDC(PWM_Duty);
		                            Dir =0;
									printf("Stop800 CurrPos : %ld\r\n", mCurPosValue);
									lhoradd=0;
									 tempadd=0;
                                }
								PWM_Duty = 0;
								en_t.HorizonStop_flag =1;
						   }
							 HALL_Pulse =0;					

						}
					    if(en_t.HorizonStop_flag ==1)
					    {
								 PID_PWM_Duty=0;
						}
						else
					    {
							dError_sum += iError; 
							
						    if(dError_sum > 1000)dError_sum =1000; /*error accumulate */
							if(dError_sum < -1000)dError_sum = -1000; 
		                    PID_PWM_Duty = (int32_t)(iError *pid_r.KP_H + dError_sum * pid_r.KI_H + (iError - last_iError)*pid_r.KD_H);//proportion + itegral + differential
					       
						    printf("hor_pwm= %ld\r \n",PID_PWM_Duty);
							
							PID_PWM_Duty = abs(PID_PWM_Duty)/2;
							if(PID_PWM_Duty >=50)PID_PWM_Duty=50;

		                 	last_iError = iError;
							PWM_Duty = PID_PWM_Duty;
							HALL_Pulse =0;
                             if(en_t.HorizonStop_flag ==2){
                             Dir = 1;
						     PWM_Duty = 30;
							 iError =0;
							 last_iError =0;
							 HALL_Pulse =0;
					         }
							
					   }
				
				
			}
            /***************************************************/
			else{  //Vertical Position judge is boundary
					   PWM_Duty=50;

					//   printf("ivError PWM = %d \n",PWM_Duty);
					   ivError = abs(mCurPosValue) - abs(en_t.Vertical_Position) ; //
						
				      
					//	printf("VerstandPos= %ld \n\r",en_t.Vertical_Position);
					//    printf("vert_ivError = %ld \n\r",ivError);
					//	printf("mCurPosValue= %ld \n\r",mCurPosValue);
					//	printf("VcurrHALL= %ld \n\r",HALL_Pulse);
						VDff = ivError;
             		    VDff = abs(VDff);
						if(abs(en_t.Horizon_Position) > 800 &&(en_t.Pos_diff < 0)){
							
							// lverticalpos = abs(en_t.Horizon_Position) - 100;
							 //lverticalpos= abs(mCurPosValue) ;
							 if(abs(mCurPosValue)<200){
					  	      
							 printf("Ver80 pwm  = %d \r\n",PID_PWM_Duty);
	                         PMW_AllClose_ABC_Channel();
	                         motor_ref.motor_run =0;
                             printf("VDffcurrPOS= %d \n\r",mCurPosValue);
							 printf("V>800\r\n");
							 printf("V~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
							 ivError =0;
							 last_iError=0;
							 en_t.DIR_flag=0;
							 }
							 
						}
						else if(abs(en_t.Horizon_Position) < 300 &&(en_t.Pos_diff < 0)){
							
							 
							 if(abs(mCurPosValue) >  800){
					  	      
							 printf("Ver80 pwm  = %d \r\n",PID_PWM_Duty);
	                         PMW_AllClose_ABC_Channel();
	                         motor_ref.motor_run =0;
                             PRINTF("VDffcurrPOS= %d \n\r",mCurPosValue);
							 PRINTF("V < 100 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
							 ivError =0;
							 last_iError=0;
							 en_t.DIR_flag=0;
                             lhoradd=0;
							  tempadd=0;
							 
							 }	
						}

			    
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
				  UART_ReadBlocking(DEMO_UART, RxBuffer, 8);
				  PRINTF("PID input referece \n");
                  for(i=0;i<8;i++){
				  	    
						  if(RxBuffer[0]==0xff){
							  pid_r.KD_H=RxBuffer[1];
		                      pid_r.KI_H=RxBuffer[2];
							  pid_r.KD_H=RxBuffer[3];
							  pid_r.KP_V = RxBuffer[4];
							  pid_r.KI_V = RxBuffer[5];
							  pid_r.KD_V = RxBuffer[6];
		                         motor_ref.motor_run =RxBuffer[7];
		                     PRINTF("KP KI KD = %d %d %d \n\r",pid_r.KP_H,pid_r.KI_H,pid_r.KD_H);
							 PRINTF("vkp vki vkd = %d %d %d \n\r",pid_r.KP_V,pid_r.KI_V,pid_r.KD_V);
		                    
							 
					  	  }
						  else{
								 k0=RxBuffer[0];
								 PRINTF("USART Error !!!!!!!!\n\r");
								 PRINTF("k0 = %d \n\r",k0);
								 PRINTF("KP KI KD = %d %d %d \n\r",pid_r.KP_H,pid_r.KI_H,pid_r.KD_H);
								 PRINTF("VKP VKI VKD = %d %d %d \n\r",pid_r.KP_V,pid_r.KI_V,pid_r.KD_V);
								  motor_ref.motor_run =RxBuffer[7];
						      }
                        }
		          
					}
        // PRINTF("Motor Stop !!!!!!!!!!! \r\n");
        // PRINTF("motor_run = %d \r\n",motor_ref.motor_run );

		lhoradd=0;
		tempadd=0;
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
                   PWM_Duty =50;
				   en_t.HorizonStop_flag=0;
                   PRINTF("START_KEY @@@@@@@@@@@@@@@@@@@@@@@@@\n\r");
              
				  break;
		
				 case DIR_CCW_PRES: //Dir = 0;PTE24 = CCW,KEY3,水平方向
				    Dir = 0 ; //
					en_t.DIR_flag = 1;
				   PRINTF("DIR =000000000\r\n");
	  			   
			    
				   HALL_Pulse =0;
			       
			   
			
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


