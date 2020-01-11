
#include "hall.h"

uint32_t mCurPosValue;

BLDC_Typedef BLDCMotor = {0,CW,0,0,100,0,0,0}; //CW = - 
PID_TypeDef  sPID; 
int32_t PID_Result = 0;

uint32_t Time_CNT = 0;
int32_t  flag = 0;




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
  if(Dir == 0) //逆时针旋转
  	{
	  if(setHome < setEnd)
	  {
	     sPID.SetPoint = setEnd;
		 iError = sPID.SetPoint - NextPoint; //偏差= 目标位置 - 当前位置 (distance)
	  }
	  else
	  {
		  sPID.SetPoint = setHome;
		  iError = sPID.SetPoint - NextPoint; //偏差= 目标位置 - 当前位置 (distance)
	  }
  	}
  else //顺时针旋转
  	{
       if(setHome < setEnd)
	  {
	     sPID.SetPoint = setEnd;
		
		 iError = sPID.SetPoint - NextPoint; //偏差= 目标位置 - 当前位置 (distance)
	  }
	  else
	  {
		  sPID.SetPoint = setHome;
		 
		  iError = sPID.SetPoint - NextPoint; //偏差= 目标位置 - 当前位置 (distance)
	  }

    }
  iError = sPID.SetPoint - NextPoint; //偏差= 目标位置 - 当前位置 (distance)
  if((iError<1 )&& (iError>-1)) //霍尔传感器误差，精度低（闭环）
    iError = 0;
  /* 积分分离 */
  if((iError<5 )&& (iError>-5))
  { 
    sPID.SumError += iError; //积分-累积误差
    /* 积分上限 */
    if(sPID.SetPoint >0)
    {
      if(sPID.SumError >= 2 * sPID.SetPoint)
        sPID.SumError = 2 * sPID.SetPoint;
    }
    else if(sPID.SumError <= 2 * sPID.SetPoint)
         sPID.SumError = 2 * sPID.SetPoint;
  }
  dError = iError - sPID.LastError; //微分项
  sPID.LastError = iError;
  
  return (int32_t)(sPID.Proportion * iError //比例项
  + sPID.Integral * sPID.SumError //积分项
  + sPID.Derivative * dError); //微分项
}

/*******************************************************
  *
  * Function Name:PID 
  * Function:闭环PID,计算
  * 输入参数：当前角度值
  * 返回参数：目标控制量
  *
  */
void SysTick_IRQ_Handler  (void)
{
		

	//  static uint32_t Last_Dir = 0;
	  Time_CNT++;
	//	PRINTF("vPortSetupTimerInterrupt = OK \r\n");
	  if( arithmetic_flag  == 1)
	  {
		/* 100ms 采样周期,控制周期 */
		if(Time_CNT % 100 == 0)
		{
		  /* 获取速度值:由捕获到的脉冲数除以总的时间 Pul/t */
		  /* BLDCMotor是4对极,旋转一圈有4个脉冲信号,3相UVW信号线,共12个脉冲信号,每个脉冲边沿计数一次,
		   * 所以BLDCM旋转一圈,可以捕获到的mCurPosValue
		   * 每个信号边沿都会捕获到接口定时器的CCR1,每100ms读取捕获到的时间和脉冲数,就可以得到电机的速度值.
		   */
		   PID_Result = LocPIDCalc(mCurPosValue);
	
		  /* 限定PWM数值范围 */
		  if(PID_Result<0)//判断方向，PID值＜ 0
		  {
			PID_Result = abs(PID_Result);
			BLDCMotor.Dir = CCW; //判断方向，逆时针
			Dir = 1;
		  }
		  else 
			{
			  BLDCMotor.Dir = CW;  //顺时针方向
			  Dir =0 ;
			}
	
		#if 0	
		  /*判断电机运行方向*/
		   BLDCMotor.Dir= Dir ; //逆时针
		  if(Last_Dir != BLDCMotor.Dir ) //
		  {
			//Disable_BLDC(); //停止电机运转
		   // Enable_BLDC();  //开启电机运转
		   uwStep = HallSensor_GetPinState();
		   HALLSensor_Detected_BLDC(PWM_Duty);
		  }
		  Last_Dir = BLDCMotor.Dir;// CW
		 #endif 
		  if(PID_Result>= 20000) //PMW =20000
		     PID_Result = 20000;
		  
		    PWM_Duty = PID_Result;
		
		  //uwStep = HallSensor_GetPinState();
					   
		  //HALLSensor_Detected_BLDC(BLDCMotor.PWM_Duty);//HAL_TIM_TriggerCallback(&htimx_HALL); //换向函数,6步换向，无刷电机
		 
	
		}
	  }  
	  //50ms反馈一次数据
	  if(Time_CNT % 50 == 0)
	  {
		PRINTF("Tick position : %d\r\n", mCurPosValue);//Transmit_FB(ptr_FB);
	  }
	  if(Time_CNT == 100)
		Time_CNT = 0;
}




