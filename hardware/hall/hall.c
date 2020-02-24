
#include "hall.h"

uint32_t mCurPosValue;


BLDC_Typedef BLDCMotor = {0,CW,0,0,100,0,0,0}; //CW = - 




uint32_t Time_CNT = 0;
int32_t  flag = 0;

uint32_t ABZ_CNT;

 volatile uint16_t g_destination_home; 
volatile uint16_t g_destination_end; 



/***************************************************
	*function Name:void ENCODER_Init(void)
	*
	*
	*
***************************************************/

/*************************************************
  * Function Name: void IncPIDInit()
  * Function: PID参数初始化
  * 输入参数: 无
  * 返回值: 无
*************************************************/
void IncPIDInit(void) 
{
    PID_TypeDef  sPID; 
    sPID.LastError=0;            // Error[-1]
    sPID.PrevError=0;            // Error[-2]
    sPID.Proportion=P_DATA;      // 比列常数 Proportional Const
    sPID.Integral=I_DATA;        // 积分常数  Integral Const
    sPID.Derivative=D_DATA;      // 微分常数 Derivative Const
    sPID.SetPoint=TARGET_PULSE;            // 设定目标Desired Value
}



