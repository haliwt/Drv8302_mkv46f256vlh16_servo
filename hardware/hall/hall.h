#ifndef __HALL_H__
#define __HALL_H__

#include "fsl_gpio.h"
#include "fsl_port.h"
#include "bldc.h"
#include "encoder.h"
#include "output.h"



/* PID struct*/
typedef struct 
{
  __IO int32_t  SetPoint;                                 //�趨Ŀ�� Desired Value
  __IO long     SumError;                                 //����ۼ�?
  __IO float    Proportion;                               //�������� Proportional Const
  __IO float    Integral;                                 //���ֳ��� Integral Const
  __IO float    Derivative;                               //΢�ֳ��� Derivative Const
  __IO int      LastError;                                //Error[-1]
  __IO int      PrevError;                                //Error[-2]
}PID_TypeDef;


   


#define  P_DATA      0.1f//0.85f                                // P����
#define  I_DATA      0.01//0.055f                                // I����
#define  D_DATA      0.2//0.558f                                 // D����

#define P_ANGLE_DATA        0.35f   		//��������
#define I_ANGLE_DATA		0.002f
#define D_ANGLE_DATA		0.35f

#define abs(x)    ((x)>0?(x):-(x))
#define  TARGET_PULSE  1000

extern int32_t array_data[4];

/* ��չ���� ------------------------------------------------------------------*/


extern uint8_t  arithmetic_flag ;


extern int32_t PID_Result ;

extern uint32_t Time_CNT ;
extern uint32_t ABZ_CNT;

extern int32_t PID_Result ;













#endif 
