#include "hall.h"


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
    sPID.LastError=0;            // Error[-1]
    sPID.PrevError=0;            // Error[-2]
    sPID.Proportion=P_DATA;      // 比列常数 Proportional Const
    sPID.Integral=I_DATA;        // 积分常数  Integral Const
    sPID.Derivative=D_DATA;      // 微分常数 Derivative Const
    sPID.SetPoint=TARGET_PULSE;            // 设定目标Desired Value
}
/*******************************************************
  *
  * Function Name:LocPIDCalc(int32_t nextPoint)
  * Function:位置闭环PID,计算
  * 输入参数：下一个点位置
  * 返回参数：目标控制量
  *
  */
int32_t LocPIDCalc(int32_t NextPoint)
{
  int32_t iError,dError;
  iError = sPID.SetPoint - NextPoint; //偏差= 目标位置 - 当前位置 (distance)
  if((iError<10 )&& (iError>-10)) //霍尔传感器误差，精度低（闭环）
    iError = 0;
  /* 积分分离 */
  if((iError<200 )&& (iError>-200))
  { 
    sPID.SumError += iError; //积分-累积误差
    /* 积分上限 */
    if(sPID.SetPoint>0)
    {
      if(sPID.SumError >= 7 * sPID.SetPoint)
        sPID.SumError = 7 * sPID.SetPoint;
    }
    else if(sPID.SumError <= 7 * sPID.SetPoint)
         sPID.SumError = 7 * sPID.SetPoint;
  }
  dError = iError - sPID.LastError; //微分项
  sPID.LastError = iError;
  
  return (int32_t)(sPID.Proportion * iError //比例项
  + sPID.Integral * sPID.SumError //积分项
  + sPID.Derivative * dError); //微分项
}

/*******************************************************
  *
  * Function Name:AnglePIDCalc(int32_t nextPoint)
  * Function:角度闭环PID,计算
  * 输入参数：当前角度值
  * 返回参数：目标控制量
  *
  */




