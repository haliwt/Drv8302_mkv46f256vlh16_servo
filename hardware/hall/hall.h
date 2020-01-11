#ifndef __HALL_H__
#define __HALL_H__

#include "fsl_gpio.h"
#include "fsl_port.h"
#include "bldc.h"
#include "encoder.h"

/* 类型定义 ------------------------------------------------------------------*/
typedef struct {
  __IO int32_t  uwStep ;  	  // 电机旋转状态
  __IO int32_t  Dir ;        	// 电机旋转方向
  __IO int32_t  Position;		  // 电机位置
  __IO int32_t  Speed;		    // 电机速度
  __IO uint16_t PWM_Duty; 	  // 电机占空比
  __IO int32_t  Hall_Period;  // 霍尔传感器触发周期
  __IO int32_t  Hall_PulNum;  // 霍尔传感器脉冲数
  __IO int32_t  Lock_Time;    // 电机堵转时间
}BLDC_Typedef;





/* 私有类型定义 --------------------------------------------------------------*/
typedef struct 
{
  __IO int32_t  SetPoint;                                 //设定目标 Desired Value
  __IO long     SumError;                                 //误差累计
  __IO float    Proportion;                               //比例常数 Proportional Const
  __IO float    Integral;                                 //积分常数 Integral Const
  __IO float    Derivative;                               //微分常数 Derivative Const
  __IO int      LastError;                                //Error[-1]
  __IO int      PrevError;                                //Error[-2]
}PID_TypeDef;


#define  SysTick_IRQ_Handler            SysTick_Handler     


#define  P_DATA      0.85f                                // P参数
#define  I_DATA      0.055f                                // I参数
#define  D_DATA      0.558f                                 // D参赛

#define P_ANGLE_DATA        0.35f   		//比例参数
#define I_ANGLE_DATA		0.002f
#define D_ANGLE_DATA		0.35f

#define abs(x)    ((x)>0?(x):-(x))
#define  TARGET_PULSE  1000


/* 扩展声明 ------------------------------------------------------------------*/


extern uint8_t  arithmetic_flag ;





void ENCODER_Init(void);
void IncPIDInit(void) ;    //PID 初始化函数



int32_t LocPIDCalc(int32_t NextPoint);  //位置PID计算

void SysTick_IRQ_Handler  (void);






#endif 
