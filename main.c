/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause  MKV46F256VLH16
 */

/*System includes.*/
#include <stdio.h>
#include "amclib_FP.h"
#include "mlib_FP.h"
#include "gflib_FP.h"
#include "gdflib_FP.h"
#include "gmclib_FP.h"

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
//#include "usart_edma_rb.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MAX_LOG_LENGTH 20




BLDC_REF bldc_ref ;


/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
//__IO int32_t uwStep ;      //��������״̬
    
//__IO uint32_t Lock_Time ;  // ��ת����

//__IO int16_t  PWM_Duty;  //ռ�ձ�

//__IO int8_t Dir;






/*******************************************************************************
 *
 * Code
 *
 * @brief Main function
 *
******************************************************************************/
int main(void)
{

 
    uint8_t printx1[]="Dir = Dir is OK !!!! CW \r\n";
    uint8_t printx2[]="Dir = - Dir is OK #### CCW \r\n";
     uint16_t sampleMask;
	//uint16_t sampleMask;
     uint8_t ucKeyCode=0,abc_s=0,dirvalue=0;
     uint8_t start_s =0,motor;
   

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    
   
    LED_Init();
    KEY_Init();
    DelayInit();
    HALL_Init();
    
    SD315AI_SO12_Input_Init();
    
    HallSensor_GetPinState();
    OUTPUT_Fucntion_Init();
    ADC_CADC_Init();
    ABC_POWER_OUTPUT_Init();
    /* Set the PWM Fault inputs to a low value */
    PWM_BLDC_Init();
    USART_POLLING_Init();
     
   while(1)
   {

          ucKeyCode = KEY_Scan(0);
          
          if(bldc_ref.motor_run == 1)
          {

            uwStep = HallSensor_GetPinState();
            HALLSensor_Detected_BLDC();
          }
      
   
           /*Check U phase*/
          if(GPIO_PinRead(SD315AI_SO1_A_GPIO,SD315AI_SO1_A_PIN)==1 ||GPIO_PinRead(SD315AI_SO2_A_GPIO,SD315AI_SO1_A_PIN)==1)
          {
               GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,1);//SD315_VL_A_OUTPUT=0;
           
          }
          else
          {
              GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,0);//SD315_VL_A_OUTPUT=0;

          }
          /*Check V phase*/
          if(GPIO_PinRead(SD315AI_SO1_B_GPIO,SD315AI_SO1_B_PIN)== 1 ||GPIO_PinRead(SD315AI_SO2_B_GPIO,SD315AI_SO2_B_PIN)== 1)
          {
               
               GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,1);//SD315_VL_B_OUTPUT=0;
            

          }
          else
          {
               GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,0);//SD315_VL_B_OUTPUT=0;
          }
          /*Check W phase*/
          if(GPIO_PinRead(SD315AI_SO1_C_GPIO,SD315AI_SO1_C_PIN)== 1 || GPIO_PinRead(SD315AI_SO2_C_GPIO,SD315AI_SO2_C_PIN)== 1)
          {
                GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,1);
               
          }
          else
          {
                GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,0);
          }
		
	    
       

      /*������յ��İ�������*/  
		if(ucKeyCode !=KEY_UP) 
		{
           switch(ucKeyCode)
            { 
                 
                  case ABC_POWER_PRES :
                     PRINTF("ABC_PRES key  \r\n");
                     abc_s ++;
                    if(abc_s == 1)
				    {
                      A_POWER_OUTPUT =0;

                      
                     
                     }
				     else 
				     {
                            A_POWER_OUTPUT =1; //shut down
                            abc_s =0;
                        
				     }
                      
				  	break;

				 case START_PRES:
                   PRINTF("START_PRES key \r\n");
				    start_s ++;
		          if(start_s == 1)
		          {
                     bldc_ref.motor_run = 1;
                     printf("motor is one \n");
                    
    			   }
				  else 
				  {
                    bldc_ref.motor_run = 0;
					 start_s =0;
					 
					 printf("START KEY IS STOP\n");
				  }
                 
				  break;
				  
				 case DIR_PRES: //3

			       dirvalue ++;
	  			 if(dirvalue == 1)
	   			 {
                        Dir =1;
				  }
				 else 
				   {
                      Dir = 0;
                      dirvalue =0;
					  printf(" dir is  =0 \r\n");
				   }
			
           		break;
            default :
               /**********************adjust frequency ****************************/
			{
            
		    CADC_DoSoftwareTriggerConverter(CADC_BASEADDR, kCADC_ConverterA);
	             /* Wait the conversion to be done. */
	         while (kCADC_ConverterAEndOfScanFlag !=
	               (kCADC_ConverterAEndOfScanFlag & CADC_GetStatusFlags(CADC_BASEADDR)))
	        {
	        }

	        /* Read the result value. */
	        if (sampleMask == (sampleMask & CADC_GetSampleReadyStatusFlags(CADC_BASEADDR)))
	        {
              //  pwm_f =(int16_t)CADC_GetSampleResultValue(CADC_BASEADDR, 1U);
		      //   PRINTF("%d\t\t",pwm_f );
	           
			 // DelayMs(100U);
             // pwm_f = (uint16_t)((CADC_GetSampleResultValue(CADC_BASEADDR, 1U))/ 330);
	         // PRINTF("PWM_Duty = %d\r\n",pwm_f);
			 // DelayMs(200U);
	           
            }
            CADC_ClearStatusFlags(CADC_BASEADDR, kCADC_ConverterAEndOfScanFlag);

     	    }

            PWM_Duty = (uint16_t)((CADC_GetSampleResultValue(CADC_BASEADDR, 1U))/ 330);
           
            PRINTF("PWM_Duty = %d\r\n",PWM_Duty);
      
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
#if 1
void BARKE_KEY_IRQ_HANDLER(void )//void BOARD_BRAKE_IRQ_HANDLER(void)
{
  
    /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(BRAKE_KEY_GPIO, 1U << BRAKE_KEY_GPIO_PIN );
    /* Change state of button. */
    A_POWER_OUTPUT =1;
    bldc_ref.motor_run = 0;
	PRINTF("interrupte has happed  \r\n");
	                  
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif 
/*******************************************************************************************/

#if 0
/******************************************************************************************************
*	�� �� ��: vTaskTaskUSART
*	����˵��: �ӿ���Ϣ����
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 1  (��ֵԽС���ȼ�Խ�ͣ�������ȼ�)
********************************************************************************************************/
static void vTaskUSART(void *pvParameters)
{

  uint8_t vTasktx[]="vTaskUSART-1 \r\n";
  // TickType_t xLastWakeTime;
 // const TickType_t xFrequency = 300;
  uint8_t i,ch;
  MSG_T *ptMsg;
 
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(300); /* �������ȴ�ʱ��Ϊ5ms */
  /* ��ʼ���ṹ��ָ�� */
	ptMsg = &g_tMsg;
	
	/* ��ʼ������ */
	ptMsg->ucMessageID = 0;
    for(i=0;i<4;i++)
        ch =ptMsg->usData[i];

  while(1)
    {
    
      //printf("vTaskUSART-1 \r\n");
       UART_WriteBlocking(DEMO_UART,vTasktx,sizeof(vTasktx)-1);
       UART_ReadBlocking(DEMO_UART, ptMsg->usData, 4);
      
        for(i=0;i<4;i++)
		{
          ch=ptMsg->usData[i];
		  printf("ptMsg->usData[i]= %#x \r\n",ch);

		}
      
        if(ptMsg->usData[0] == 0x01) //'1' = 0x31
         {
           
		      xTaskNotify(xHandleTaskCOTL,      /* Ŀ������ */
								ptMsg->usData[2],              /* �������� */
								eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
               printf("Send to xHanderCONT is OK \n");
                for(i=0;i<10;i++)
                 ch =ptMsg->usData[i];
         }
        if(ptMsg->usData[0]== 0x02)
        {
           
	      xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
							ptMsg->usData[2],              /* �������� */
							eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
        }
		//taskYIELD();//vTaskDelay(xMaxBlockTime);
	    vTaskDelay(xMaxBlockTime);// vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
/*********************************************************************************************************
*
*	�� �� ��: vTaskSUBJ
*	����˵��: ���������������ְ���������
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 2  
*
*********************************************************************************************************/
static void vTaskSUBJ(void *pvParameters)
{
     
     uint8_t vTasktx1[]="ABC_= 1 @@@@~~~@@@\r\n";
     uint8_t vTasktx2[]="ABC_= 0 ~~~~~~~~~\r\n";
     uint16_t sampleMask;
      TickType_t xLastWakeTime;
	
	  const TickType_t xFrequency = 200;
      xLastWakeTime = xTaskGetTickCount();
     
     uint8_t abc_s=0,door_s=0,wipers_s=0;
     uint32_t vlSubj;
     uint32_t ucConKeyValue;
	 BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* �������ȴ�ʱ��Ϊ5ms */

	while(1)
    {
          printf("vTaskSUBJ-2 \r\n");
		
		  xResult = xTaskNotifyWait(0x00000000,      
						           0xFFFFFFFF,      
						           &vlSubj,        /* �洢ulNotifiedValue��ulvalue�� */
						           xMaxBlockTime);  /* ����ӳ�ʱ�� */
          
		  
		if( xResult == pdPASS )
		{
			
             ucConKeyValue = vlSubj;
             printf("vTaskSUBJ vlSubj = %#x\r\n", ucConKeyValue);
             /*******************ABC_POWER_F*************************/
             #if 1
             if(ucConKeyValue==0x01)
			 {
                 
                  //abc_s ++ ;
                 if(abc_s == 0)
                 {
                   A_POWER_OUTPUT =0;
                   abc_s ++ ;
                  //printf("ABC_= 1 @@@@~~~@@@\r\n");
                  UART_WriteBlocking(DEMO_UART,vTasktx1,sizeof(vTasktx1)-1);
		     
			     }
			     else 
                 {
                   abc_s = 0;
                   A_POWER_OUTPUT =1;
                  
                  //printf("ABC_ = 0 @@@@@@@~~~\r\n"); 
                   UART_WriteBlocking(DEMO_UART,vTasktx2,sizeof(vTasktx2)-1);
                 }
		     }
             #endif 
			 /*******************Door_F*************************/
			 if(ucConKeyValue==0x08)
			 {
               
                door_s ++ ;
                if(door_s ==1)
                {
                  DOOR_OUTPUT =1;//door open
                  printf("DOOR_OUTPUT = 1 @@@@@@@\r\n");  
		        }
                else
                {
                  door_s =0;
                  DOOR_OUTPUT =0;//door open
                  printf("DOOR_OUTPUT = 0 ~~~~~~~\r\n");  
                }
		     }
			 /*******************WIPERS_F***************************/
             if(ucConKeyValue == 0x09)
	         {
	  			
	  			 wipers_s ++ ;
                 if(wipers_s == 1)
                 {
                   WIPER_OUTPUT_1 = 1;
                   printf("WIPERS  = 1 @@@@~~~~\r\n");   
                 }
			     else if(wipers_s == 2)
                 {
                   wipers_s = 0;
                   WIPER_OUTPUT_1 = 2;   
                   printf("WIPERS  = 2 ~~~~@@@@\r\n");
                 }
			 
			    else if(wipers_s == 3)
                 {
                     WIPER_OUTPUT_1 = 3;   
                
                    printf("WIPERS  = 3 @@@~~~~@@@\r\n");
                  }
                else
                 {
                     wipers_s =0;
                      WIPER_OUTPUT_1 = 0;   
                    
                      printf("WIPERS  = 0 @@@~~~~@@@~~~~\r\n");
                 }
             }
		     /*******************AIR_F***************************/   
              if(ucConKeyValue == 0x12)
			 {
                 AIR_OUTPUT = 1;
                
				printf("AIR  = 1 ~~@@@~~\r\n");
			 }
              if(ucConKeyValue == 0x13)
			 {
                 AIR_OUTPUT = 0;
                
				printf("AIR  = 0 ~~@@@~~@@@~~~~@@@\r\n");
			 }
        }
		else
		{
			/* 3?���� */
			LED1=!LED1;
			
		}

    vTaskDelayUntil(&xLastWakeTime, xFrequency); // vTaskDelay(xMaxBlockTime); 
	}
}
/*********************************************************************************************************
*	�� �� ��: vTaskBLDC
*	����˵��: ʹ�ú���xQueueReceive��������vTaskTaskUserIF���͵���Ϣ��������(xQueue2)	
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 3  
*********************************************************************************************************/
static void vTaskBLDC(void *pvParameters)
{
    uint8_t txbuff[]="vTaskBLDC 3\r\n";
    uint32_t ucValue;
   
//	TickType_t xLastWakeTime;
	
//	const TickType_t xFrequency = 100;
 //  xLastWakeTime = xTaskGetTickCount();
    volatile uint16_t pwm_f=0;
	uint16_t sampleMask;
	BaseType_t xResult;
   
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(5); /* �������ȴ�ʱ��Ϊ300ms */
	uint32_t ucConValue;
    uint16_t dirvalue;
	uint8_t vTasktx1[]="BLDC receive success!!";
   
    uint8_t vBLDCtx2[] = "Motor Run is OK !!!!";
    uint8_t printx1[]="Dir = Dir is OK !!!! CW \r\n";
    uint8_t printx2[]="Dir = - Dir is OK #### CCW \r\n";
	while(1)
    {       
        
        //printf("vTaskBLDC-2 \r\n");  
        UART_WriteBlocking(DEMO_UART,txbuff,sizeof(txbuff)-1);
        xResult = xTaskNotifyWait(0x00000000,      
						          0xFFFFFFFF,      
						          &ucConValue,        /* �洢ulNotifiedValue��ulvalue�� */
						          xMaxBlockTime);  /* ����ӳ�ʱ�� */
	  if(xResult == pdPASS)
		{
			/* �������ݳɹ� */
          //printf("vTaskBLDC ConmessageID = %#x\r\n",ucConValue );
           UART_WriteBlocking(DEMO_UART,vTasktx1,sizeof(vTasktx1)-1);
		  ucValue = ucConValue ;
		}
		else
		{
			/* ��ʱ */
          LED2= !LED2;
		}
     #if 0 
      if(ucValue == 0x01)
      {
         A_POWER_OUTPUT =1;
		
		 
        // printf("ABC_= 1 @@@@~~~@@@\r\n");
         UART_WriteBlocking(DEMO_UART,vTasktx2,sizeof(vTasktx2)-1);
		 
	  }
	  else if(ucValue == 0x02)
	  {

         A_POWER_OUTPUT =0;
		
		
		//printf("ABC_= 0 ~~~~~~~~~\r\n"); 
         UART_WriteBlocking(DEMO_UART,vTasktx3,sizeof(vTasktx3)-1);
	  }
	  #endif 

		
	  if((ucValue==0xa1)|| (recoder_number.break_f ==1))//ɲ��
	  {
       
        
           
         taskENTER_CRITICAL(); //�����ٽ�״̬
		 PMW_AllClose_ABC_Channel();
         PMW_AllClose_ABC_Channel();
		
		 if(recoder_number.break_f ==1)
		 {
		     A_POWER_OUTPUT =0;
			  
		  }
		 taskEXIT_CRITICAL(); //�˳��ٽ�״̬
		 
		 //printf("Break is OK $$$$$$$$$$$$$\r\n");
        
	  }
	  else if(ucValue==0xa0)  
	  { 
            
            /* �������ݳɹ� */
             printf("Motor run = %#x\r\n",ucConValue);
			// printf("Motor Run is OK !!!!\r\n");
             UART_WriteBlocking(DEMO_UART,vBLDCtx2,sizeof(vBLDCtx2)-1);

	       /**********************adjust frequency ****************************/
			{
            
		    CADC_DoSoftwareTriggerConverter(CADC_BASEADDR, kCADC_ConverterA);
	             /* Wait the conversion to be done. */
	         while (kCADC_ConverterAEndOfScanFlag !=
	               (kCADC_ConverterAEndOfScanFlag & CADC_GetStatusFlags(CADC_BASEADDR)))
	        {
	        }

	        /* Read the result value. */
	        if (sampleMask == (sampleMask & CADC_GetSampleReadyStatusFlags(CADC_BASEADDR)))
	        {
              //  pwm_f =(int16_t)CADC_GetSampleResultValue(CADC_BASEADDR, 1U);
		      //   PRINTF("%d\t\t",pwm_f );
	           
			 // DelayMs(100U);
             // pwm_f = (uint16_t)((CADC_GetSampleResultValue(CADC_BASEADDR, 1U))/ 330);
	         // PRINTF("PWM_Duty = %d\r\n",pwm_f);
			 // DelayMs(200U);
	           
            }
            CADC_ClearStatusFlags(CADC_BASEADDR, kCADC_ConverterAEndOfScanFlag);

     	    }

            //pwm_f = (uint16_t)((CADC_GetSampleResultValue(CADC_BASEADDR, 1U))/ 330);
            pwm_f = 50;
           // PRINTF("PWM_Duty = %d\r\n",pwm_f);
            
			if(recoder_number.dir_change == 1) 
            {
                  dirvalue = 1 ;
				 // printf("Dir = Dir is OK !!!! CW \r\n");
				  UART_WriteBlocking(DEMO_UART,printx1,sizeof(printx1)-1);
			}
			else 
			{
			    dirvalue = 0;
				//printf("Dir = - Dir is OK #######\r\n");
				UART_WriteBlocking(DEMO_UART,printx2,sizeof(printx2)-1);
			}
			/***********Motor Run**************/
          
			 uwStep = HallSensor_GetPinState();
        	//PRINTF("ouread = %d \r\n",uwStep);
        	
        	
        	HALLSensor_Detected_BLDC(uwStep,pwm_f,dirvalue); 
           

        }
	
    // vTaskDelayUntil(&xLastWakeTime, xFrequency); // vTaskDelay(xMaxBlockTime);  
     vTaskDelay(xMaxBlockTime); //taskYIELD();////vTaskDelay(xMaxBlockTime); 
      }
 } 

/*********************************************************************************************************
*
*	�� �� ��: vTaskCOTL
*	����˵��: ���������������ְ���������
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 4  
*
*********************************************************************************************************/
static void vTaskCOTL(void *pvParameters)
{
   
     uint8_t vTaskCotltx[]= "vTaskCOTL-4";
     uint8_t ucKeyCode=0,abc_s=0,digital_s=0;
     uint8_t start_s =0,door_s = 0,wiper_s=0,air_s=0;
     uint8_t hall_s = 0,wheel_s=0,abc_power_s=0;
     
     TickType_t xLastWakeTime;

    const TickType_t xFrequency = 200;
    xLastWakeTime = xTaskGetTickCount();
    BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100); /* �������ȴ�ʱ��Ϊ5ms */
	uint8_t ucControl=0;
	uint32_t rlValue,ulValue;
	

	while(1)
    {
	       //printf("vTaskCOTL-4 \r\n");
          
            UART_WriteBlocking(DEMO_UART,vTaskCotltx,sizeof(vTaskCotltx)-1);
          
          
		   ucKeyCode = KEY_Scan(0);
           /*Check U phase*/
           if(GPIO_PinRead(SD315AI_SO1_A_GPIO,SD315AI_SO1_A_PIN)==1 ||GPIO_PinRead(SD315AI_SO2_A_GPIO,SD315AI_SO1_A_PIN)==1)
          {
               GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,1);//SD315_VL_A_OUTPUT=0;
           
          }
          else
          {
              GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,0);//SD315_VL_A_OUTPUT=0;

          }
          /*Check V phase*/
          if(GPIO_PinRead(SD315AI_SO1_B_GPIO,SD315AI_SO1_B_PIN)== 1 ||GPIO_PinRead(SD315AI_SO2_B_GPIO,SD315AI_SO2_B_PIN)== 1)
          {
               
               GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,1);//SD315_VL_B_OUTPUT=0;
            

          }
          else
          {
               GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,0);//SD315_VL_B_OUTPUT=0;
          }
          /*Check W phase*/
          if(GPIO_PinRead(SD315AI_SO1_C_GPIO,SD315AI_SO1_C_PIN)== 1 || GPIO_PinRead(SD315AI_SO2_C_GPIO,SD315AI_SO2_C_PIN)== 1)
          {
                GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,1);
               
          }
          else
          {
                GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,0);
          }
		  xResult = xTaskNotifyWait(0x00000000,      
						            0xFFFFFFFF,      
						            &rlValue,        /* �洢ulNotifiedValue��ulvalue�� */
						            xMaxBlockTime);  /* ����ӳ�ʱ�� */
		
		if( xResult == pdPASS )
		{
            ulValue=rlValue;
			printf("vTaskCOTL = %#x\r\n", ulValue);
			/****************digitalkey coder******************/
			 if(ulValue == 0x01)
			 {
                ucKeyCode =ABC_POWER_PRES  ; 
			 }
			 if(ulValue == 0x2)
			 {
                ucKeyCode =START_PRES  ; 
			 }
			 if(ulValue == 0x4)
			 {
                ucKeyCode =DIR_PRES  ; 
			 }
			 if(ulValue == 0x07)
			 {
                ucKeyCode =DIGITAL_ADD_PRES  ; 
			 }
			 if(ulValue == 0x06)
			 {
                ucKeyCode =DIGITAL_REDUCE_PRES  ; 
			 }
			 if(ulValue == 0x09)
			 {
                ucKeyCode =DOOR_PRES ; 
			 }
			 if(ulValue == 0x0a)
			 {
                ucKeyCode =HALL_PRES ; 
			 }
			 if(ulValue == 0x0c)
			 {
                ucKeyCode =WHEEL_PRES ; 
			 }
			  if(ulValue == 0x0e)
			 {
                ucKeyCode =WIPERS_PRES ; 
			 }
			  if(ulValue == 0x12)
			 {
                ucKeyCode =AIR_PRES ; 
			 }
			 
		}
		else
		{
			/* 3?���� */
			LED1=0;
			LED2 =0 ;
		}

      /*������յ��İ�������*/  
		if(ucKeyCode !=KEY_UP) 
				
			{
               switch(ucKeyCode)//if(ptMsg->ucMessageID == 0x32)
                { 
                 
                  case ABC_POWER_PRES :

				  PRINTF("ABC_PRES key \r\n");
				     abc_power_s++;
				  if((abc_power_s ==1)||(recoder_number.break_f==1))
				  	{
                       ucControl = 0x01;
                       abc_s ++;
                       if(abc_s == 255) abc_s =3;
                       if(recoder_number.break_f ==1)
                       {
                            recoder_number.break_f=0;
                            abc_s =2;
                       }
                       xTaskNotify(xHandleTaskSUBJ ,          //xHandleTaskBLDC,        /* Ŀ������ */
                                   ucControl,              /* �������� */
                                   eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
                       printf("ABC POWER IS SEND WORKS \n");
                     }
					else 
					{
                     ucControl = 0x01;
                     abc_power_s =0 ;
                     xTaskNotify( xHandleTaskSUBJ ,//xHandleTaskBLDC,        /* Ŀ������ */
                                 ucControl,              /* �������� */
                                 eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
                     printf("ABC POWER IS STOP \n");

					}
				  	break;

				 case START_PRES:
                   PRINTF("START_PRES key \r\n");
				    start_s ++;
		          if(((start_s == 1)||(abc_s ==2))&&(abc_s!=0))
		          {
                    
                     ucControl =0xa0;
                     abc_s =5; ;
    				 recoder_number.break_f=0;
    				 xTaskNotify(xHandleTaskBLDC,      /* Ŀ������ */
    								ucControl,              /* �������� */
    								eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
                    printf("START KEY IS SEND WORKS \n");
				  }
				  else 
				  {
                     ucControl = 0xa1;
					 start_s =0;
					  xTaskNotify(xHandleTaskBLDC,      /* Ŀ������ */
									ucControl,              /* �������� */
									eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
					 printf("START KEY IS STOP\n");
				  }
                 
				  break;
				  
				 case DIR_PRES: //3

			      recoder_number.dir_change++;
	  			 PRINTF(" DIR_change = %d  \r\n", recoder_number.dir_change);
	  			 if(recoder_number.dir_change == 1)
	   				{
                        LED1 =0;
						LED2 =0;
				    }
				 else 
				   {
                      recoder_number.dir_change =0;
					  
				   }
			
           		break;
				
			 case DIGITAL_ADD_PRES ://4
				PRINTF("DIGITAL_ADD_PRES key \r\n");
                  digital_s ++;
				 if(digital_s ==1)
				  	{
                       ucControl = 0x01;
                       xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
                                   ucControl,              /* �������� */
                                   eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
                     }
					else 
					{
                     ucControl = 0x00;
                     digital_s =0 ;
                     xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
                           ucControl,              /* �������� */
                           eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */

					}
				break;
				
			 case DIGITAL_REDUCE_PRES ://5 PAT13
			 	PRINTF("DIGITAL_REDUCE_PRES key \r\n");
                digital_s ++;
			     if((digital_s ==1)||(recoder_number.break_f ==1))
				   {
                      recoder_number.break_f =0;
					//  abc_power_s=1;
					  ucControl = 0x05;
					  xTaskNotify(xHandleTaskBLDC,		/* Ŀ������ */
								  ucControl,			  /* �������� */
								  eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
					}
				   else 
				   {
                     //abc_power_s=0;
					ucControl = 0x04;
					digital_s =0 ;
					xTaskNotify(xHandleTaskBLDC,	  /* Ŀ������ */
						  ucControl,			  /* �������� */
						  eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
				   	}
				
				break;
				
			 case DOOR_PRES ://6
			 	   PRINTF("DOOR_PRES key \r\n");
			 	   door_s ++; ;
				   if(door_s ==1)
				   {
                      
                       ucControl = 0x09;
					
					  xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
								   ucControl,              /* �������� */
								   eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
				   }
				   else
				   {
                      door_s =0; 
                     ucControl = 0x08;
					  xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
								   ucControl,              /* �������� */
								   eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
                    }
			 	   
				break;
				   
			 case HALL_PRES://7
			 	PRINTF("HALL_PRES key \r\n");
				  hall_s ++ ;
				   if(hall_s ==1)
				   {
                      
                       ucControl = 0x0b;
					
					  xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
								   ucControl,              /* �������� */
								   eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
				   }
				   else
				   {
            		  hall_s = 0;
					  ucControl = 0x0a;
					  xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
								   ucControl,              /* �������� */
								   eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
                   }
				break;
			 
			 case WHEEL_PRES : //8 PTA5
			    PRINTF("WHEEL_PRES key \r\n");
				  wheel_s ++ ;
				 
			    break;
				
			 case WIPERS_PRES: //9�����
			  	PRINTF("WIPERS_PRES key \r\n");
				wiper_s ++;
				
				if(wiper_s ==1)
				{

                   ucControl = 0x0e;
				   xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
							   ucControl,              /* �������� */
							   eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
				}
				else if(wiper_s ==2)
				{
                    ucControl = 0x0f;
					xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
								ucControl,              /* �������� */
								eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
				 }
				else if(wiper_s == 3)
			    {
					ucControl = 0x10;
					xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
								ucControl,              /* �������� */
								eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
				}
				else
				{
                    wiper_s = 0;
					ucControl = 0x11;
					xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
								ucControl,              /* �������� */
								eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
				}
             break;
				
	         case AIR_PRES : //10 PE29
	         	PRINTF("AIR_PRES key \r\n");
	            air_s ++;
				        
				if(air_s ==1)
				{
			        ucControl = 0x12;
					xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
								ucControl,              /* �������� */
								eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
				}	  
			    else 
			    {
		            air_s =0;
				    ucControl = 0x13;
					xTaskNotify(xHandleTaskSUBJ,      /* Ŀ������ */
								ucControl,              /* �������� */
								eSetValueWithOverwrite);/* �ϴ�Ŀ������û��ִ�У��ᱻ���� */
				  
				   
			    }
                break;
        }
        
	}
     vTaskDelayUntil(&xLastWakeTime, xFrequency);//taskYIELD();//
   }//end whilt(1)
}
/********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************/
static void AppTaskCreate (void)
{
    xTaskCreate( vTaskUSART,   									/* ������  */
                 "vTaskUserIF",     							/* ������    */
                 configMINIMAL_STACK_SIZE + 166,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,              							/* �������  */
                 tskIDLE_PRIORITY+1,                 			/* �������ȼ� ���*/
                 &xHandleTaskUSART );  							/* ������  */

	xTaskCreate( vTaskSUBJ,    									/* ������  */
                 "vTaskBLDC",  									/* ������    */
                 configMINIMAL_STACK_SIZE + 166,         		/* stack��С����λword��Ҳ����4�ֽ� */
                 NULL,        									/* �������  */
                 tskIDLE_PRIORITY+2,           					/* �������ȼ�*/
                 &xHandleTaskSUBJ); 							/* ������  */
	
	xTaskCreate( vTaskBLDC,    									/* ������  */
                 "vTaskBLDC",  									/* ������    */
                 configMINIMAL_STACK_SIZE + 934,         		/* stack��С����λword��Ҳ����4�ֽ� */
                 NULL,        									/* �������  */
                 tskIDLE_PRIORITY+3,           					/* �������ȼ�*/
                 &xHandleTaskBLDC); 							/* ������  */

	xTaskCreate( vTaskCOTL,    									/* ������  */
                 "vTaskCOTL",  									/* ������    */
                 configMINIMAL_STACK_SIZE + 166,         		/* stack��С����λword��Ҳ����4�ֽ� */
                 NULL,        									/* �������  */
                 tskIDLE_PRIORITY+4,           					/* �������ȼ�*/
                 &xHandleTaskCOTL); 							/* ������  */

}
/********************************************************************
 *
 *	�� �� ��: AppObjCreate
 *	����˵��: ��������ͨ�Ż���
 *	��    ��: ��
 *	�� �� ֵ: ��
 *
********************************************************************/
#if 0
static void AppObjCreate (void)
{
  
    /* ����10��uint8_t����Ϣ���� */
	xQueue1 = xQueueCreate(10, sizeof(uint8_t));
    if( xQueue1 == 0 )
    {
       printf("xQueuel set up fail!!!!"); 
       /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
    }
    #if 0
     /* ����10���洢ָ���������Ϣ���У�����CM3/CM4�ں���32λ����һ��ָ�����ռ��4���ֽ� */
	xQueue2 = xQueueCreate(10, sizeof(struct Msg *));
    if( xQueue2 == 0 )
    {
         printf("xQueue2 set up fail!!!!"); /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
    }
    #endif 

}
#endif 
#endif 



