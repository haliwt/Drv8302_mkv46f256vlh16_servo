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
  __IO float    Derivative;                               //΢�� Derivative Const
  __IO int      LastError;                                //Error[-1]
  __IO int      PrevError;                                //Error[-2]
}PID_TypeDef;


   



      

#define abs(x)    ((x)>0?(x):-(x))




/* ��չ���� ------------------------------------------------------------------*/

extern int32_t PID_Result ;

extern uint32_t Time_CNT ;

void IncPIDInit(void);














#endif 
