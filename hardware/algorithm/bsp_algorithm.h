#ifndef __BSP_ALGORITHM_H__
#define __BSP_ALGORITHM_H__
#include "bsp_encoder.h"
#include "bsp_bldc.h"

#define  float_t     float;    
/* ���ų����Ķ���            */
#define GENMAX  	1000 /*ѧϰ���ظ�����*/
#define NODENO  	15  /*Qֵ�Ľڵ���*/
#define ALPHA  		1   /*ѧϰϵ��*/
#define GAMMA 		0.9/*�ۿ���*/
#define EPSILON 	2 /*ˮƽ����ѧϰϵͳ*/
#define SEED 		32767 /*�������seed*/

typedef struct _pid_t_
{
	 volatile float   iError;
	 volatile float   dError_sum;
	 volatile float   last_iError;
	 volatile float   iVError;
	 volatile float   last_ivError;
	 volatile float   dvError_sum;
	 volatile int32_t mCurPosValue;
	 volatile int32_t mHoldPos;
	 volatile int32_t mStopHoldPos; /*indication stop Position value*/
	 volatile int32_t mStopVerPos;
	 int32_t  Buff[2];
	 uint8_t  hv_n;
	 uint8_t  total_n;
	 uint8_t  hor_n;
	 uint8_t  oneKeyDetector_flag;  /*one key detector flag*/
	 uint8_t  times_vertical_stop;   /*recoder stop times*/
	 uint8_t  times_horizon_stop; 
	 uint8_t  pwm_api; 
	 uint8_t  balance_stop_flag; 
	 uint8_t  horizon_balance_stop_flag;   
	 uint8_t  g_memory_n;
}pid_t;

struct _pid_reference
{
  float  KP_H;
  float  KI_H;
  float  KD_H;

 float  KP_V;
 float  KI_V;
 float  KD_V;

};



#if 0
void Horizontal_Decelerate_Function(void);
void Vertical_Decelerate_Function(void);
void Detect_HorVer_Position(void);

void HorizonStop_Region(void);
void VerticalStop_Regin(void);

void Balance_Stop_Function(void);


extern pid_t algpid_t ;

#endif 


#endif 
