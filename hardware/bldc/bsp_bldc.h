#ifndef __BSP_BLDC_H__
#define __BSP_BLDC_H__

#include "fsl_pwm.h"
#include "fsl_xbara.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "hall.h"



#define PWM_SRC_CLK_FREQ 	CLOCK_GetFreq(kCLOCK_FastPeriphClk)//84MHz
/* The PWM base address */
#define BOARD_PWM_BASEADDR PWMA

#define HALL_A_GPIO          GPIOB
#define HALL_B_GPIO          GPIOB
#define HALL_C_GPIO          GPIOB

#define HALL_A_GPIO_PIN       1
#define HALL_B_GPIO_PIN       2
#define HALL_C_GPIO_PIN       3

#define HALL_A          PBin(1)
#define HALL_B          PBin(2)
#define HALL_C          PBin(3)



#define CCW               (int8_t)(1)
#define CW                (int8_t)(-1)


//#define DEBUG_PRINT         1

//#define DRV8302            1
/* BLDC of struct */
typedef struct _BLDC_Typedef_{
  __IO int32_t  uwStep ;  	  // 电机旋转状态
  __IO int32_t  Dir ;        	// 电机旋转方向
  __IO int32_t  Position;		  // 电机位置
  __IO int32_t  Speed;		    // 电机速度
  __IO uint16_t PWM_Duty; 	  // 电机占空比
  __IO int32_t  Hall_Period;  // 霍尔传感器触发周期
  __IO int32_t  Hall_PulNum;  // 霍尔传感器脉冲数
  __IO int32_t  Lock_Time;    // 电机堵转时间
}BLDC_Typedef;


typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
}GPIO_PinState;



extern __IO int32_t  uwStep ;

extern __IO uint32_t Lock_Time; // 堵转时间
extern __IO uint16_t   PWM_Duty;   //占空比
extern __IO int32_t  PID_PWM_Duty ;

extern __IO int8_t Dir;


extern __IO int32_t HALL_Pulse;  



void HALL_Init(void);

uint32_t HallSensor_GetPinState(void);

void HALLSensor_Detected_BLDC(uint16_t duty);


void PWM_BLDC_Init(void);   // initial pwm function

void PWMA_Select_AB_Channel(uint8_t s_pwmA,uint16_t duty);  //select one PWM A channel,H-bridge first 
void PWMA_Select_BC_Channel(uint8_t s_pwmB,uint16_t duty);  //select one PWM B  channel,H-bridge second
void PWMA_Select_CA_Channel(uint8_t s_pwmC,uint16_t duty); 
void PMW_AllClose_ABC_Channel(void);
void PMW_AllClose_ABC_Duty_10_Channel(void);
void PMW_AllClose_ABC_Duty_20_Channel(void);




void PWMA_Close_ABC_Channel(uint8_t f_pwma);

void BLDC_CCW_SIX_1_Run(void);
void BLDC_CCW_SIX_2_Run(void);
void BLDC_CCW_SIX_3_Run(void);
void BLDC_CCW_SIX_4_Run(void);
void BLDC_CCW_SIX_5_Run(void);
void BLDC_CCW_SIX_6_Run(void);


void BLDC_CW_Six_1_Run(void);
void BLDC_CW_Six_2_Run(void);
void BLDC_CW_Six_3_Run(void);
void BLDC_CW_Six_4_Run(void);
void BLDC_CW_Six_5_Run(void);
void BLDC_CW_Six_6_Run(void);
void Motor_Down_Start(void); //电机往水平方向移动，导引
void Motor_Up_Start(void);

















#endif 
