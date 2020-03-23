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
	
}pid_t;

#define  P_DATA      0.85f                                // P²ÎÊý
#define  I_DATA      0.055f                                // I²ÎÊý
#define  D_DATA      0.558f                                 // D²ÎÊý

#define  VP_DATA      0.85f                                // P²ÎÊý
#define  VI_DATA      0.055f                                // I²ÎÊý
#define  VD_DATA      0.558f                                 // D²ÎÊý


void Horizontal_Decelerate_Function(void);
void Vertical_Decelerate_Function(void);


extern pid_t algpid_t ;




#endif 
