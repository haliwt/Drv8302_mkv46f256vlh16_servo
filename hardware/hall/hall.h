#ifndef __HALL_H__
#define __HALL_H__

#include "fsl_gpio.h"
#include "fsl_port.h"






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




#define  P_DATA      0.85f                                // P参数
#define  I_DATA      0.055f                                // I参数
#define  D_DATA      0.558f                                 // D参赛

#define P_ANGLE_DATA        0.35f   		//比例参数
#define I_ANGLE_DATA		0.002f
#define D_ANGLE_DATA		0.35f

#define abs(x)    ((x)>0?(x):-(x))
#define  TARGET_PULSE  1000


/* 扩展声明 ------------------------------------------------------------------*/


extern BLDC_Typedef BLDCMotor ;
extern PID_TypeDef  sPID;



void ENCODER_Init(void);
void IncPIDInit(void) ;    //PID 初始化函数

int32_t LocPIDCalc(int32_t NextPoint);  //位置PID计算
int32_t AanglePIDCalc(int32_t NextAngle); //角度PID计算






#endif 
