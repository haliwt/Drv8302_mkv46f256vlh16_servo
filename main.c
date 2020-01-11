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




volatile uint32_t g_EncIndexCounter = 0U;
uint32_t pulseWidth;


output_t  motor_ref;
encoder_t en_t;
 BLDC_Typedef BLDCMotor = {0,CW,0,0,100,0,0,0}; //CW = - 
PID_TypeDef  sPID; 

 uint16_t pwm_duty;
int16_t setHome=0xfff;
int16_t setEnd=0xfff;
int16_t setPositionHome=0xfff;
int16_t setPositionEnd=0xfff;
uint8_t setRun_flag=0 ;
uint8_t setStop_flag=0;


/* 私有变量 ------------------------------------------------------------------*/
uint32_t IS_EnableMotor = 0;  // 使能电机标志
uint32_t Time_CNT = 0;
int32_t  flag = 0;
int32_t  start_flag = 0;



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
       
		
	      
		en_t.mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
		BLDCMotor.Hall_PulNum = en_t.mCurPosValue;//
		pwm_duty=60;
	#if 1	
		
      /***************************Zerio AND Point***************************************************************/
	   if(((en_t.firstPowerOn ==0)||(en_t.firstPowerOn <4))&&(en_t.en_interrupt_flag == 1 ))
		{
			  en_t.capture_width =Capture_ReadPulse_Value();
		      PRINTF("Cpw = %d\r\n", en_t.capture_width);
			  PRINTF("Current position : %d\r\n", en_t.mCurPosValue);

			   PRINTF("--------------------------------------\r\n" ); 
			   en_t.en_interrupt_flag=0;
			   en_t.firstPowerOn++;
	          
			   PRINTF("firstOn= %d \r\n",en_t.firstPowerOn);
				
			   if(en_t.firstPowerOn ==2)
			   	{
				   setHome = en_t.capture_width-10;
                   if(setPositionHome == 0 )
				   setPositionHome = en_t.mCurPosValue + 10;
                   else if(Dir ==1) //顺时针
                     setPositionHome = en_t.mCurPosValue + 10;
				   else
				   	 setPositionHome = en_t.mCurPosValue - 10;

				    sPID.SetPoint=setPositionHome;
				   PRINTF("setHome= %d \r\n",setHome);
				   PRINTF("setPositionHome^^^= %d \r\n",setPositionHome);
								  
			   	}
			    if(en_t.firstPowerOn ==3)
			   {

				   setEnd = en_t.capture_width-10;
                   if(setPositionEnd ==0 )
                    setPositionEnd = en_t.mCurPosValue + 10;
                   else if(Dir ==1) //顺时针
                     setPositionEnd = en_t.mCurPosValue +10;
				   else
				   	 setPositionEnd = en_t.mCurPosValue - 10;
				   sPID.SetPoint=setPositionEnd;
				   PRINTF("setPositionEnd@@@= %d \r\n",setPositionEnd);
			   }
			  
			   PRINTF("setHome= %d \r\n",setHome);
			   PRINTF("setEnd= %d \r\n",setEnd);
			   PRINTF("setPositionHome^^= %d \r\n",setPositionHome);
			   PRINTF("setPositionEnd@@@= %d \r\n",setPositionEnd);
								
		}
           
     /*********************************检测信号************************************************************************/ 
		 if(setPositionHome  == en_t.mCurPosValue||setPositionHome == (en_t.mCurPosValue +1 )||setPositionHome == en_t.mCurPosValue-1)
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
             setStop_flag=1; 
             PRINTF("setPosHomeRun%%%%%%%% = 1\r\n");
			 PRINTF("Current position : %d\r\n", en_t.mCurPosValue);
           }	  
          }
          if((setPositionEnd == en_t.mCurPosValue-1)||setPositionEnd  == en_t.mCurPosValue+1||setPositionEnd  == en_t.mCurPosValue)//绝对位置是不变的，setHome 软件指定的
         {
			 
		       PRINTF("setPositionEnd ok \r\n");
			   PRINTF("setPositionEnd = %d \r\n",setPositionEnd);
               if(keyRunTime ==1)
               {
				  setStop_flag=0;
                  PRINTF("setPosEndRun~~~~~~~~ ~~~~~= 1 \r\n");
               }
			   else
               {
			     setStop_flag = 1;
                 PRINTF("setPosEndRun########## =  1\r\n");
				 PRINTF("Current position : %d\r\n", en_t.mCurPosValue);
               }

		   }
#endif 
         
/**************************************************************************************************************************************/
        
     /***********motor run main*********************/
      if((motor_ref.motor_run == 1)&&(setStop_flag ==0))
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
                  
                     uwStep = HallSensor_GetPinState();
	               
	                 HALLSensor_Detected_BLDC(pwm_duty);
                    // en_t.mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
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
                 
                  case ABC_POWER_PRES ://顺时针---方向
				  	 Dir = 1;
					 keyRunTime = 1;
					  setRun_flag = 0;
					  setStop_flag=0;
				     PRINTF("Right Key: %d\r\n", setRun_flag);
                    if(Dir == 0) //Dir =1 ;  //顺时针旋转
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
		
				 case DIR_PRES: //逆时针旋转
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

#if 0
/*!
 * @brief ISR for systick intrrupt
 */
 int32_t PID_Result = 0;
//void SysTick_IRQ_Handler(void)
 void vPortSetupTimerInterrupt( void )
{
 	PRINTF("vPortSetupTimerInterrupt = OK \r\n");
  #if 0
  static uint32_t Last_Dir = 0;
  Time_CNT++;
  PRINTF("vPortSetupTimerInterrupt = OK \r\n");
  if(start_flag  == 1)
  {
    /* 100ms 采样周期,控制周期 */
    if(Time_CNT % 100 == 0)
    {
      /* 获取速度值:由捕获到的脉冲数除以总的时间 Pul/t */
      /* BLDCMotor是4对极,旋转一圈有4个脉冲信号,3相UVW信号线,共12个脉冲信号,每个脉冲边沿计数一次,
       * 所以BLDCM旋转一圈,可以捕获到的霍尔信号是24,计数值可以计数到24.
       * 每个信号边沿都会捕获到接口定时器的CCR1,每100ms读取捕获到的时间和脉冲数,就可以得到电机的速度值.
       */
       PID_Result = LocPIDCalc(BLDCMotor.Hall_PulNum);
      /* 限定PWM数值范围 */
      if(PID_Result<0)//判断方向，PID值＜ 0
      {
        PID_Result = abs(PID_Result);
        BLDCMotor.Dir = CCW; //判断方向，逆时针
        Dir = 1;
      }
      else 
      	{
          BLDCMotor.Dir = CW;  //顺时针方向
          Dir =0 ;
      	}
        
      /*判断电机运行方向*/
	   BLDCMotor.Dir= Dir ; //逆时针
      if(Last_Dir != BLDCMotor.Dir ) //
      {
        Disable_BLDC(); //停止电机运转
       // Enable_BLDC();  //开启电机运转
       uwStep = HallSensor_GetPinState();
	   HALLSensor_Detected_BLDC(pwm_duty);
      }
      Last_Dir = BLDCMotor.Dir;// CW
      
     // if(PID_Result>=(int32_t)(BLDCMOTOR_TIM_PERIOD/3)) //1.9KHZ/3=0.63khz
      //  PID_Result = (int32_t)(BLDCMOTOR_TIM_PERIOD/3);
      
      BLDCMotor.PWM_Duty = PID_Result;
	 /// pwm_duty = PID_Result;
     // PWM_ChangeFlag = 1; //中断，标志位
      uwStep = HallSensor_GetPinState();
	               
	  HALLSensor_Detected_BLDC(BLDCMotor.PWM_Duty);//HAL_TIM_TriggerCallback(&htimx_HALL); //换向函数,6步换向，无刷电机
      //PWM_ChangeFlag = 0;

    }
  }  
  //50ms反馈一次数据
  if(Time_CNT % 50 == 0)
  {
    PRINTF("Current position : %d\r\n", en_t.mCurPosValue);//Transmit_FB(ptr_FB);
  }
  if(Time_CNT == 100)
    Time_CNT = 0;
   // PRINTF("g_en = %d \r\n",g_EncIndexCounter);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
#endif 
}
#endif 

/*******************************************************************************************/


