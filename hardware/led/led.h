#ifndef __LED_H__
#define __LED_H__



#include "fsl_gpio.h"
#include "fsl_port.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LED1  PDout(7)  //定义PTE端口的6引脚输出控制
#define LED2  PDout(6)
#define LED3  PCout(2)  //定义 PC2

#define BOARD_LED1_GPIO        GPIOD
#define BOARD_LED2_GPIO        GPIOD
#define BOARD_LED3_GPIO        GPIOC


#define BOARD_LED1_GPIO_PIN    7U 
#define BOARD_LED2_GPIO_PIN    6U 
#define BOARD_LED3_GPIO_PIN    2U


void LED_Init(void);

#endif 
