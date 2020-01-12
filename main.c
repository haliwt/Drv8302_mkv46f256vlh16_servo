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
//#include "port.c"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

output_t  motor_ref;
encoder_t en_t;



volatile uint32_t g_EncIndexCounter = 0U;
uint32_t pulseWidth;

//__IO uint16_t	PWM_Duty; 

int32_t setHome = 0xfff;
int32_t setEnd=0xfff;
int16_t setPositionHome=0xfff;
int16_t setPositionEnd=0xfff;
uint8_t setRun_flag=0 ;
uint8_t setStop_flag=0;

uint8_t  arithmetic_flag ;
uint8_t home_flag;
uint8_t end_flag;
uint8_t judge_he_flag;

int32_t array_data[4]={0xfff,0xfff,0xfff,0xfff};




//__IO uint32_t PWM_ChangeFlag = 0;
//__IO int32_t CaptureNumber = 0;      // 输入捕获数




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
    
	 static uint8_t keyRunTime=0;
     static uint8_t rem_times = 0;
    
    XBARA_Init(XBARA);
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    IncPIDInit() ;
    
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
       
		
	    en_t.capture_width =Capture_ReadPulse_Value(); 
        PRINTF("Cpw = %d\r\n", en_t.capture_width);
		mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
        PRINTF("Current position : %d\r\n", mCurPosValue);
	
		
	#if 1	
		
      /***************************Zerio AND Point***************************************************************/
	   if((rem_times <4)&&(en_t.en_interrupt_flag == 1 ))
		{
			  
		      PRINTF("Cpw = %d\r\n", en_t.capture_width);
			  PRINTF("Current position : %d\r\n", mCurPosValue);

			   PRINTF("--------------------------------------\r\n" ); 
			   en_t.en_interrupt_flag=0;
			   rem_times++;
			   PRINTF("rem_times= %d \r\n",rem_times);
	           if(rem_times < 4)
	           	{
				  
					
				   if(rem_times ==2)
				   	{
					   setHome = en_t.capture_width-20;
					   array_data[2]= setHome;
	                   if(setPositionHome == 0 )
	                   	{
					       setPositionHome = mCurPosValue + 20;
						   array_data[0]=setPositionHome;
	                   	}
	                   else if(Dir ==1) //顺时针---水平方向移动
	                   	{
	                     setPositionHome = mCurPosValue + 20;
						 array_data[0]=setPositionHome;
	                   	}
					   else //逆时针旋转 Dir =0 
					   	{
					   	 setPositionHome = mCurPosValue - 20;
						 array_data[0]=setPositionHome;
					   	}
					    
					
					    home_flag =1;
					   PRINTF("setHome= %d \r\n",array_data[2]);
					   PRINTF("setPositionHome^^^= %d \r\n",array_data[0]);
									  
				   	}
				    if(rem_times ==3)
				   {

					   setEnd = en_t.capture_width-20;
					   array_data[3]= setEnd;
	                   if(setPositionEnd ==0 )
	                   	{
	                      setPositionEnd = mCurPosValue + 20;
						  array_data[1]=setPositionEnd;
	                   	}
	                   else if(Dir ==1) //顺时针
	                   	{
	                     setPositionEnd = mCurPosValue +20;
						  array_data[1]=setPositionEnd;
	                   	}
					   else
					   	{
					   	   setPositionEnd = mCurPosValue - 20;
						    array_data[1]=setPositionEnd;
					   	}
					   
					   end_flag =1;
					    PRINTF("setEnd!!!!= %d \r\n",array_data[3]);
					   PRINTF("setPositionEnd@@@= %d \r\n",array_data[1]);
					   arithmetic_flag  = 1;
				   }
	                /*判断起始的位置*/
	                if((home_flag ==1)&&(end_flag ==1))
	                	{
							if(setHome > setEnd) //起始位置，垂直位置
							{
								judge_he_flag =1; //只能往水平位置移动
							}
							else //setHome < setEnd  //起始位置在，水平位置
								judge_he_flag =2;
							
	                	}
					
						   PRINTF("setHome@@@= %d \r\n",array_data[2]);
						   PRINTF("setEnd!!!!= %d \r\n",array_data[3]);
						   PRINTF("setPositionHome^^= %d \r\n",array_data[0]);
						   PRINTF("setPositionEnd@@@= %d \r\n",array_data[1]);
	           	}
			    else 
					 PRINTF("rem_times over~~~~ \r\n");
								
		}
           
     /*********************************检测信号************************************************************************/ 
		 if(((array_data[0]  <abs( mCurPosValue +30)) && (array_data[0]> abs(mCurPosValue -30)))\
		 	||((array_data[2] < abs(en_t.capture_width +30)) && (array_data[2] >abs(en_t.capture_width-30))))
		 { 
              
           PRINTF("setPositionHome ok \r\n");
		   PRINTF("setPositionHome = %d \r\n",setPositionHome);
		   if(keyRunTime == 1)
		   {
              setStop_flag=0; 
               PRINTF("setPosHomeRun||||| =0 \r\n");
		   }
		   else
           {
            // setStop_flag=1; 
             PRINTF("setPosHomeRun%%%%%%%% = 1\r\n");
			 PRINTF("Current position : %d\r\n", mCurPosValue);
           }	  
          }
          if(((array_data[1] < abs(mCurPosValue +30))&& (array_data[1]  > abs(mCurPosValue-30)))||((array_data[3] < abs(en_t.capture_width +30)) && (array_data[3]>abs(en_t.capture_width-30))))//绝对位置是不变的，setHome 软件指定的
         {
			 
		       PRINTF("setPositionEnd ok \r\n");
			   PRINTF("setPositionEnd = %d \r\n",setPositionEnd);
               if(keyRunTime ==1)
               {
				  setStop_flag=0;
                  PRINTF("setPosEndRun~~~~~~~~= 1 \r\n");
               }
			   else
               {
			    // setStop_flag = 1;
                 PRINTF("setPosEndRun########## =  1\r\n");
				 PRINTF("Current position : %d\r\n", mCurPosValue);
				 
               }

		   }
#endif 
         
/**************************************************************************************************************************************/
        
     /***********motor run main*********************/
      if(motor_ref.motor_run == 1)
      {
   				
               keyRunTime=2;
			 
			   GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,1);
			  	  
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
              else{
                    if(Dir == 0) //向垂直方向移动
                    {
						uwStep = HallSensor_GetPinState();
	               
	                 	HALLSensor_Detected_BLDC(PWM_Duty);
						PWM_Duty=60;

					}
			        else //Dir == 1 水平方向移动
		        	{
						 SysTick_IRQ_Handler ();  
						 uwStep = HallSensor_GetPinState();
		          		 HALLSensor_Detected_BLDC(PWM_Duty);
	                     
						 
		        	}
                    }
            }/*end if motor_ref.motor_run == 1*/
          
          else
		  {

             if(motor_ref.power_on==2||motor_ref.motor_run==1)
             	{
				  
				  
				  PWM_Duty = 30;
				  uwStep = HallSensor_GetPinState();
	              HALLSensor_Detected_BLDC( PWM_Duty);
				  DelayMs(50);
                  
                   PWM_Duty = 20;
				  uwStep = HallSensor_GetPinState();
	              HALLSensor_Detected_BLDC( PWM_Duty);
				  DelayMs(50);
				 
				  PWM_Duty = 10;
				  uwStep = HallSensor_GetPinState();
	              HALLSensor_Detected_BLDC( PWM_Duty);
				  DelayMs(50);
				  PRINTF("KEY STOP motor_run = %d \r\n",motor_ref.motor_run);
				  PRINTF("KEY STOP power_on = %d \r\n",motor_ref.power_on);
				 
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
                 
                  case DIR_DOWN_PRES ://顺时针---方向--“下”-水平方向
				  	 Dir = 1;
					 keyRunTime = 1;
					  setRun_flag = 0;
					  setStop_flag=0;
				     PRINTF("Right Key: %d\r\n", setRun_flag);
                    if(Dir == 0) //顺时针旋转
	   			    {

                        if((motor_ref.power_on ==2)||(motor_ref.motor_run == 1))//motor is runing
                        {
                            if(motor_ref.Dir_flag == 0)
                            {
                               PWM_Duty= 10;
							  uwStep = HallSensor_GetPinState();
				              HALLSensor_Detected_BLDC( PWM_Duty);
							   PWM_Duty = 5;
							  uwStep = HallSensor_GetPinState();
							  HALLSensor_Detected_BLDC( PWM_Duty);
                              DelayMs(30);
                              motor_ref.power_on = 1;
                              motor_ref.Dir_flag =1;
                              Dir =1;
                            }
                          
                        }
						else
							Dir = 1;
                     }
                      else
                      {
                        Dir=1;
                        motor_ref.Dir_flag =1;
                        motor_ref.power_on = 1;
                       
                      
                      }
                         UART_WriteBlocking(DEMO_UART, printx1, sizeof(printx1) - 1);
				    
                      
				  	break;
        		
                 case START_PRES:
                   
				   motor_ref.motor_run ++ ;
                   motor_ref.power_on ++ ;
                   keyRunTime = 1;
                   setRun_flag = 0;
                   setStop_flag=0;
				  PRINTF("Run = %d \r\n",setRun_flag);
                  PRINTF("keysetStop_flag = %d\r\n",setStop_flag);
                 if(motor_ref.motor_run == 1)
    			  {
                      motor_ref.power_on =1;
                      setRun_flag = 1;
                      UART_WriteBlocking(DEMO_UART, printx5, sizeof(printx5) - 1);
    			  }
				  else 
				  {
					  setRun_flag = 1;
					  motor_ref.motor_run = 0;
                      motor_ref.power_on =0;
                      UART_WriteBlocking(DEMO_UART, printx4, sizeof(printx4) - 1);
				  }
                 
				  break;
		
				 case DIR_UP_PRES: //逆时针旋转 Dir = 0;“上”--垂直方向
				    Dir = 0 ; //逆时针
					keyRunTime = 1;
                   setRun_flag = 0;
				   setStop_flag=0;
				   PRINTF("key setRun_flag: %d\r\n", setRun_flag);
	  			
			     if(Dir==1) // Dir = 0; //逆时针旋转
				   {
	               
	                 if((motor_ref.power_on == 2)||(motor_ref.motor_run == 1)) //motor is runing
	                 {
	                    if(motor_ref.Dir_flag ==1)
	                    {

	                       PWM_Duty = 10;
						  uwStep = HallSensor_GetPinState();
			              HALLSensor_Detected_BLDC( PWM_Duty);
						   PWM_Duty = 5;
						  uwStep = HallSensor_GetPinState();
						  HALLSensor_Detected_BLDC( PWM_Duty);
	                      DelayMs(30);
	                      motor_ref.power_on = 1;
	                      motor_ref.Dir_flag = 0;
	                      Dir =0;
	                    }
						else
							Dir = 0;
	                  
	                  }
                   }
	                  else
	                  {
	                     
	                     Dir = 0;
	                    
	                     motor_ref.Dir_flag = 0;
	                     motor_ref.power_on = 1;
	                     

	                  }
                 UART_WriteBlocking(DEMO_UART, printx2, sizeof(printx2) - 1);
			   
			
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






/*******************************************************************************************/


