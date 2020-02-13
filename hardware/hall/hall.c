
#include "hall.h"

uint32_t mCurPosValue;

BLDC_Typedef BLDCMotor = {0,CW,0,0,100,0,0,0}; //CW = - 
PID_TypeDef  sPID; 



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

     #if 0  
	 // if(Dir == 0) //逆时针旋转 DIR_UP_PRES，//往垂直方向移动
	  {
		     if(judge_home_flag ==1) //setHome垂直是起始位置// 向垂直位置移动setEnd =12241 反向
		     	{
			       sPID.SetPoint = setPositionHome; //往垂直方向移动
			           
				   iError = sPID.SetPoint - NextPoint ; //偏差= 目标位置 - 当前位置 (distance)
		     	}
			
			 if(judge_home_flag == 2) //起始位置在水平位置,UP-往垂直方向移动
			 {
               sPID.SetPoint =setPositionEnd;                         
               iError = sPID.SetPoint - NextPoint ; 
			 }

	  }
	 #endif 
	  if(Dir == 1) //Dir ==1 顺时针旋转 “下”---Down --往---往水平方向移动
	  {
			if (judge_home_flag == 1)  //起始点位置在垂直的位置
			{

				sPID.SetPoint = setPositionEnd;

				iError = sPID.SetPoint + NextPoint; 
			}
			if(judge_home_flag ==2) //起点位置，在水平位置
			{
			     sPID.SetPoint = setPositionHome;
				 iError = sPID.SetPoint + NextPoint; 
			}
	  }
	

     PRINTF("iError= %d \r\n",iError);
#if 0
//  iError = sPID.SetPoint - NextPoint; //偏差= 目标位置 - 当前位置 (distance)
  if((iError<2 )&& (iError>-2)) //霍尔传感器误差，精度低（闭环）
    iError = 0;
  /* 积分分离 */
  if((iError<30 )&& (iError>-30))
  { 
    sPID.SumError += iError; //积分-累积误差
    /* 积分上限 */
    if(sPID.SetPoint >0)
    {
      if(sPID.SumError >= 2 * sPID.SetPoint)
        sPID.SumError = 2 * sPID.SetPoint;
    }
    else if(sPID.SumError <= 2 * sPID.SetPoint)
         sPID.SumError = 2 * sPID.SetPoint; //积累误差值
  }
  dError = iError - sPID.LastError; //微分项
  sPID.LastError = iError;
  
  return (int32_t)(sPID.Proportion * iError //比例项 
  + sPID.Integral * sPID.SumError //积分项
  + sPID.Derivative * dError); //微分项
 #endif 
  return iError;
}

/*******************************************************
  *
  * Function Name:PID 
  * Function:闭环PID,计算
  * 输入参数：当前角度值
  * 返回参数：目标控制量
  *
  */
void FTM_timer_Init(void)
{
   
 
    ftm_config_t ftmInfo;

    FTM_GetDefaultConfig(&ftmInfo);

    /* Divide FTM clock by 4 */
    ftmInfo.prescale = kFTM_Prescale_Divide_4;

    /* Initialize FTM module */
    FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);

    /*
     * Set timer period.
     */
    FTM_SetTimerPeriod(BOARD_FTM_BASEADDR, USEC_TO_COUNT(1000U, FTM_SOURCE_CLOCK));

    FTM_EnableInterrupts(BOARD_FTM_BASEADDR, kFTM_TimeOverflowInterruptEnable);
}

