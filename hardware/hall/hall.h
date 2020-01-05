#ifndef __HALL_H__
#define __HALL_H__

#include "fsl_gpio.h"
#include "fsl_port.h"

#define ENCODER_A_GPIO  GPIOC //PTC1
#define ENCODER_B_GPIO  GPIOC //PTC2
#define ENCODER_Z_GPIO  GPIOC //PTC3

#define ENCODER_A_GPIO_PIN	1
#define ENCODER_B_GPIO_PIN	2
#define ENCODER_Z_GPIO_PIN	3

#define EN_GATE_GPIO GPIOE 
#define EN_GATE_GPIO_PIN   18

/* 电机参数结构体 ------------------------------------------------------------------*/
typedef struct {
  __IO int32_t  uvwStep ;  	      // 电机旋转状态
  __IO int32_t  uvwDir ;          // 电机旋转方向
  __IO int32_t  Position;		  // 电机位置
  __IO int32_t  motorSpeed;		  // 电机旋转速度
  __IO uint16_t motorPWM_Duty; 	  // 电机占空比
  __IO int32_t  Hall_Period;      // 霍尔传感器触发周期
  __IO int32_t  Encode_PulNum;      //解码器脉冲数
  __IO int32_t  Lock_Time;        // 电机堵转时间
}BLDC_Typedef;

/* PID参数结构体 --------------------------------------------------------------*/
typedef struct 
{
  __IO int32_t  SetPoint;                                 //设置目标值Desired Value
  __IO int32_t  SetAngle;								  //设置目标值(角度)
  __IO long     SumError;                                 //误差累积
  __IO float    Proportion;                               //比列常数 Proportional Const
  __IO float    Integral;                                 //积分常数 Integral Const
  __IO float    Derivative;                               //微分常数 Derivative Const
  __IO int      LastError;                                //Error[-1]
  __IO int      PrevError;                                //Error[-2]
}PID_TypeDef;


#define  P_DATA      0.85f                                // P参数
#define  I_DATA      0.055f                                // I参数
#define  D_DATA      0.558f                                 // D参赛

#define P_ANGLE_DATA        0.35f   		//比例参数
#define I_ANGLE_DATA		0.002f
#define D_ANGLE_DATA		0.35f

#define abs(x)    ((x)>0?(x):-(x))


/* 扩展声明 ------------------------------------------------------------------*/


extern BLDC_Typedef BLDCMotor ;
extern PID_TypeDef  sPID;



void ENCODER_Init(void);
void IncPIDInit(void) ;    //PID 初始化函数

int32_t LocPIDCalc(int32_t NextPoint);  //位置PID计算
int32_t AanglePIDCalc(int32_t NextAngle); //角度PID计算

void DRV8302_Control_Init(void);




#endif 
