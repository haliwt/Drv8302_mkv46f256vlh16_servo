/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*System includes.*/
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MAX_LOG_LENGTH 20

#define BOARD_LED_GPIO        GPIOD
#define BOARD_LED_GPIO_PIN    6U
#define BOARD_LED2_GPIO       GPIOD
#define BOARD_LED2_GPIO_PIN    7U

/*******************************************************************************
 * Globals
 ******************************************************************************/
/* Logger queue handle */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Application API */
 
static void vTaskTaskUSART(void *pvParameters);
static void vTaskCOTL(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);

static void AppTaskCreate (void);
static void AppObjCreate (void);
/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskUserIF = NULL;
static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;
static QueueHandle_t xQueue1 = NULL;
static QueueHandle_t xQueue2 = NULL;


typedef struct Msg
{
	uint8_t  ucMessageID;
	uint16_t usData[2];
	uint32_t ulData[2];
}MSG_T;

MSG_T   g_tMsg; /* ����һ���ṹ��������Ϣ���� */

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    /* Initialize logger for 10 logs with maximum lenght of one log 20 B */
    /* �������� */
	AppTaskCreate();
    
    /* ��������ͨ�Ż��� */
	AppObjCreate();// WT.EDIT log_init(10, MAX_LOG_LENGTH);
    
    vTaskStartScheduler();
    for (;;)
        ;
}

/*******************************************************************************
 * Application functions
 ******************************************************************************/
/*!
 * @brief write_task_1 function
 ******************************************************************************/
static void vTaskTaskUserIF(void *pvParameters)
{
    char log[MAX_LOG_LENGTH + 1];
    uint32_t i = 0;
    for (i = 0; i < 5; i++)
    {
        sprintf(log, "Task1 Message %d", (int)i);
        
        taskYIELD();
    }
    vTaskDelay(10);//vTaskSuspend(NULL);
}

/*************************************************************************
 *
 * @brief write_task_2 function
 *
 *
 ************************************************************************/
static void vTaskMsgPro(void *pvParameters)
{
    MSG_T   *ptMsg;
	uint8_t ucCount = 0;
	uint8_t ucKeyCode;
	uint8_t pcWriteBuffer[500];
	
	/* ��ʼ���ṹ��ָ�� */
	ptMsg = &g_tMsg;
	
	/* ��ʼ������ */
	ptMsg->ucMessageID = 0;
	ptMsg->ulData[0] = 0;
	ptMsg->usData[0] = 0;
#if 0
    while(1)
    {
		//ucKeyCode = bsp_GetKey();
		
		//if (ucKeyCode != KEY_NONE)
		{
		//	switch (ucKeyCode)
			{
				/* K1������ ��ӡ����ִ����� */
				case KEY_DOWN_K1:			 
					printf("=================================================\r\n");
					printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
					vTaskList((char *)&pcWriteBuffer);
					printf("%s\r\n", pcWriteBuffer);
				
					printf("\r\n������       ���м���         ʹ����\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					printf("%s\r\n", pcWriteBuffer);
					break;
				
				/* K2�����£���xQueue1�������� */
				case KEY_DOWN_K2:
					ucCount++;
				
					/* ����Ϣ���з����ݣ������Ϣ�������ˣ��ȴ�10��ʱ�ӽ��� */
					if( xQueueSend(xQueue1,
								   (void *) &ucCount,
								   (TickType_t)10) != pdPASS )
					{
						/* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
						printf("K2�����£���xQueue1��������ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ���\r\n");
					}
					else
					{
						/* ���ͳɹ� */
						printf("K2�����£���xQueue1�������ݳɹ�\r\n");						
					}
					break;
				
				/* K3�����£���xQueue2�������� */
				case KEY_DOWN_K3:
					ptMsg->ucMessageID++;
					ptMsg->ulData[0]++;;
					ptMsg->usData[0]++;
					
					/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
					if(xQueueSend(xQueue2,                  /* ��Ϣ���о�� */
								 (void *) &ptMsg,           /* ���ͽṹ��ָ�����ptMsg�ĵ�ַ */
								 (TickType_t)10) != pdPASS )
					{
						/* ����ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ��� */
						printf("K3�����£���xQueue2��������ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ���\r\n");
					}
					else
					{
						/* ���ͳɹ� */
						printf("K3�����£���xQueue2�������ݳɹ�\r\n");						
					}
				
				/* �����ļ�ֵ������ */
				default:                     
					break;
			}
		}
		
		vTaskDelay(20);
	}
#endif 
}
/********************************************************************
 *
 * WT.EDIT ADD new function
 * @brief write_task_/ function
 *
 *********************************************************************/
static void vTaskLED(void *pvParameters)
{
     /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };
    
    MSG_T *ptMsg;
	BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* �������ȴ�ʱ��Ϊ200ms */
	
    while(1)
    {
		xResult = xQueueReceive(xQueue2,                   /* ��Ϣ���о�� */
		                        (void *)&ptMsg,  		   /* �����ȡ���ǽṹ��ĵ�ַ */
		                        (TickType_t)xMaxBlockTime);/* ��������ʱ�� */
		
		
		if(xResult == pdPASS)
		{
			/* �ɹ����գ���ͨ�����ڽ����ݴ�ӡ���� */
			printf("���յ���Ϣ��������ptMsg->ucMessageID = %d\r\n", ptMsg->ucMessageID);
			printf("���յ���Ϣ��������ptMsg->ulData[0] = %d\r\n", ptMsg->ulData[0]);
			printf("���յ���Ϣ��������ptMsg->usData[0] = %d\r\n", ptMsg->usData[0]);
		}
		else
		{
			/* ��ʱ */
          /* Init output LED GPIO. */
         GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_GPIO_PIN, &led_config); 
         GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED2_GPIO_PIN, &led_config);
         GPIO_PinWrite(BOARD_LED_GPIO, 6, 0);
         GPIO_PinWrite(BOARD_LED_GPIO, 7, 0);
			
		}
    }

    // TickType_t xLastWakeTime;
    // const TickType_t xFrequency = 10; // 10 x 0.5Khz��Ticks��= 10 * 5ms = 50ms
    // vTaskDelayUntil(&xLastWakeTime,xFrequency);//vTaskDelay(10);  //vTaskSuspend(NULL);

}
/*
*********************************************************************************************************
*	�� �� ��: vTaskStart
*	����˵��: ��������Ҳ����������ȼ�����������������ɨ�衣
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 4  
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
    while(1)
    {
		/* ����ɨ�� */
		//bsp_KeyScan();
        vTaskDelay(10);
    }
}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    xTaskCreate( vTaskTaskUSART,   	/* ������  */
                 "vTaskUserIF",     	/* ������    */
                 512,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,              	/* �������  */
                 1,                 	/* �������ȼ�*/
                 &xHandleTaskUserIF );  /* ������  */
	
	
	xTaskCreate( vTaskCOTL,    		/* ������  */
                 "vTaskCOTL",  		/* ������    */
                 512,         		/* stack��С����λword��Ҳ����4�ֽ� */
                 NULL,        		/* �������  */
                 2,           		/* �������ȼ�*/
                 &xHandleTaskLED ); /* ������  */
	
	xTaskCreate( vTaskMsgPro,     		/* ������  */
                 "vTaskMsgPro",   		/* ������    */
                 512,             		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		/* �������  */
                 3,               		/* �������ȼ�*/
                 &xHandleTaskMsgPro );  /* ������  */
	
	
	xTaskCreate( vTaskStart,     		/* ������  */
                 "vTaskStart",   		/* ������    */
                 512,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		/* �������  */
                 4,              		/* �������ȼ�*/
                 &xHandleTaskStart );   /* ������  */
}

/*******************************************************************************
 * Logger functions
 ******************************************************************************/

/********************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
********************************************************************/
//void log_init(uint32_t queue_length, uint32_t max_log_lenght)
static void AppObjCreate (void)
{
  
    /* ����10��uint8_t����Ϣ���� */
	xQueue1 = xQueueCreate(10, sizeof(uint8_t));
    if( xQueue1 == 0 )
    {
       printf("xQueuel set up fail!!!!"); 
       /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
    }
     /* ����10��uint8_t����Ϣ���� */
	xQueue2 = xQueueCreate(10, sizeof(uint8_t));
    if( xQueue2 == 0 )
    {
       printf("xQueue2 set up fail!!!!"); 
      /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
    }
	
}


