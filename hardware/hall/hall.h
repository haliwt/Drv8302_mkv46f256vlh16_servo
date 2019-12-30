#ifndef __HALL_H__
#define __HALL_H__

#include "fsl_gpio.h"
#include "fsl_port.h"
#include "bldc.h"
#include "adc.h"

#define NO_HALL_A_GPIO      GPIOB
#define NO_HALL_B_GPIO      GPIOC
#define NO_HALL_C_GPIO      GPIOE

#define NO_HALL_A_GPIO_PIN      18U
#define NO_HALL_B_GPIO_PIN       8U
#define NO_HALL_C_GPIO_PIN      16U

    





extern __IO uint8_t step;



void NO_HALL_GPIO_Init(void);



uint8_t Dector_Phase(uint8_t duty);

uint8_t NO_HallSensor_Hex(void);


#endif 
