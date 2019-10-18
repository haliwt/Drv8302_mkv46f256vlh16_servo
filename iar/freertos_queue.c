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
											变量声明
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

MSG_T   g_tMsg; /* 定义一个结构体用于消息队列 */

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
    /* 创建任务 */
	AppTaskCreate();
    
    /* 创建任务通信机制 */
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
	
	/* 初始化结构体指针 */
	ptMsg = &g_tMsg;
	
	/* 初始化数组 */
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
				/* K1键按下 打印任务执行情况 */
				case KEY_DOWN_K1:			 
					printf("=================================================\r\n");
					printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
					vTaskList((char *)&pcWriteBuffer);
					printf("%s\r\n", pcWriteBuffer);
				
					printf("\r\n任务名       运行计数         使用率\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					printf("%s\r\n", pcWriteBuffer);
					break;
				
				/* K2键按下，向xQueue1发送数据 */
				case KEY_DOWN_K2:
					ucCount++;
				
					/* 向消息队列发数据，如果消息队列满了，等待10个时钟节拍 */
					if( xQueueSend(xQueue1,
								   (void *) &ucCount,
								   (TickType_t)10) != pdPASS )
					{
						/* 发送失败，即使等待了10个时钟节拍 */
						printf("K2键按下，向xQueue1发送数据失败，即使等待了10个时钟节拍\r\n");
					}
					else
					{
						/* 发送成功 */
						printf("K2键按下，向xQueue1发送数据成功\r\n");						
					}
					break;
				
				/* K3键按下，向xQueue2发送数据 */
				case KEY_DOWN_K3:
					ptMsg->ucMessageID++;
					ptMsg->ulData[0]++;;
					ptMsg->usData[0]++;
					
					/* 使用消息队列实现指针变量的传递 */
					if(xQueueSend(xQueue2,                  /* 消息队列句柄 */
								 (void *) &ptMsg,           /* 发送结构体指针变量ptMsg的地址 */
								 (TickType_t)10) != pdPASS )
					{
						/* 发送失败，即使等待了10个时钟节拍 */
						printf("K3键按下，向xQueue2发送数据失败，即使等待了10个时钟节拍\r\n");
					}
					else
					{
						/* 发送成功 */
						printf("K3键按下，向xQueue2发送数据成功\r\n");						
					}
				
				/* 其他的键值不处理 */
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
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* 设置最大等待时间为200ms */
	
    while(1)
    {
		xResult = xQueueReceive(xQueue2,                   /* 消息队列句柄 */
		                        (void *)&ptMsg,  		   /* 这里获取的是结构体的地址 */
		                        (TickType_t)xMaxBlockTime);/* 设置阻塞时间 */
		
		
		if(xResult == pdPASS)
		{
			/* 成功接收，并通过串口将数据打印出来 */
			printf("接收到消息队列数据ptMsg->ucMessageID = %d\r\n", ptMsg->ucMessageID);
			printf("接收到消息队列数据ptMsg->ulData[0] = %d\r\n", ptMsg->ulData[0]);
			printf("接收到消息队列数据ptMsg->usData[0] = %d\r\n", ptMsg->usData[0]);
		}
		else
		{
			/* 超时 */
          /* Init output LED GPIO. */
         GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_GPIO_PIN, &led_config); 
         GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED2_GPIO_PIN, &led_config);
         GPIO_PinWrite(BOARD_LED_GPIO, 6, 0);
         GPIO_PinWrite(BOARD_LED_GPIO, 7, 0);
			
		}
    }

    // TickType_t xLastWakeTime;
    // const TickType_t xFrequency = 10; // 10 x 0.5Khz（Ticks）= 10 * 5ms = 50ms
    // vTaskDelayUntil(&xLastWakeTime,xFrequency);//vTaskDelay(10);  //vTaskSuspend(NULL);

}
/*
*********************************************************************************************************
*	函 数 名: vTaskStart
*	功能说明: 启动任务，也就是最高优先级任务，这里用作按键扫描。
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 4  
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
    while(1)
    {
		/* 按键扫描 */
		//bsp_KeyScan();
        vTaskDelay(10);
    }
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    xTaskCreate( vTaskTaskUSART,   	/* 任务函数  */
                 "vTaskUserIF",     	/* 任务名    */
                 512,               	/* 任务栈大小，单位word，也就是4字节 */
                 NULL,              	/* 任务参数  */
                 1,                 	/* 任务优先级*/
                 &xHandleTaskUserIF );  /* 任务句柄  */
	
	
	xTaskCreate( vTaskCOTL,    		/* 任务函数  */
                 "vTaskCOTL",  		/* 任务名    */
                 512,         		/* stack大小，单位word，也就是4字节 */
                 NULL,        		/* 任务参数  */
                 2,           		/* 任务优先级*/
                 &xHandleTaskLED ); /* 任务句柄  */
	
	xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
                 "vTaskMsgPro",   		/* 任务名    */
                 512,             		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 3,               		/* 任务优先级*/
                 &xHandleTaskMsgPro );  /* 任务句柄  */
	
	
	xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务名    */
                 512,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 4,              		/* 任务优先级*/
                 &xHandleTaskStart );   /* 任务句柄  */
}

/*******************************************************************************
 * Logger functions
 ******************************************************************************/

/********************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
********************************************************************/
//void log_init(uint32_t queue_length, uint32_t max_log_lenght)
static void AppObjCreate (void)
{
  
    /* 创建10个uint8_t型消息队列 */
	xQueue1 = xQueueCreate(10, sizeof(uint8_t));
    if( xQueue1 == 0 )
    {
       printf("xQueuel set up fail!!!!"); 
       /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
     /* 创建10个uint8_t型消息队列 */
	xQueue2 = xQueueCreate(10, sizeof(uint8_t));
    if( xQueue2 == 0 )
    {
       printf("xQueue2 set up fail!!!!"); 
      /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
	
}


