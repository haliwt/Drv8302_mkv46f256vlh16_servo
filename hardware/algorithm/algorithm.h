#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__
#include "encoder.h"
#include "bldc.h"

typedef struct _pid_t_
{
	 volatile int32_t iError;
	 volatile int32_t dError_sum;
	 volatile int32_t last_iError;
	 volatile int32_t iVError;
	 volatile int32_t mCurPosValue;
	 volatile int32_t mHoldPos;
	 uint8_t Buff[2];
	 uint8_t  hv_n;
	 uint8_t hv_g_n;
	 uint8_t total_n;
	 uint8_t hor_n;
	
}pid_t;

extern float  P_DATA  ;//=    0.85f ;                               // P²ÎÊý
extern float  I_DATA  ;// =   0.055f  ;                              // I²ÎÊý
extern float  D_DATA  ;// =   0.558f  ;                               // D²ÎÊý

extern float  VP_DATA ;//=     0.85f  ;                              // P²ÎÊý
extern float  VI_DATA ;//=     0.055f ;                               // I²ÎÊý
extern float VD_DATA  ; //=   0.558f  ;                               // D²ÎÊý


void Horizontal_Decelerate_Function(void);
void Vertical_Decelerate_Function(void);
void Detect_HorVer_Position(void);
void Decelerate_Speed_Region(void);


extern pid_t algpid_t ;




#endif 
