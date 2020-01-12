
#include "hall.h"

uint32_t mCurPosValue;

BLDC_Typedef BLDCMotor = {0,CW,0,0,100,0,0,0}; //CW = - 
PID_TypeDef  sPID; 
int32_t PID_Result ;


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

	  if(Dir == 0) //逆时针旋转 DIR_UP_PRES，//往垂直方向移动
	  {
		     if(judge_he_flag ==1) //setHome垂直是起始位置// 向垂直位置移动setEnd =12241 反向
		     	{
			       sPID.SetPoint = setPositionHome; //往垂直方向移动
			           
				   iError = sPID.SetPoint - NextPoint ; //偏差= 目标位置 - 当前位置 (distance)
		     	}
			
			 if(judge_he_flag == 2) //起始位置在水平位置,UP-往垂直方向移动
			 {
               sPID.SetPoint =setPositionEnd;                         
               iError = sPID.SetPoint - NextPoint ; 
			 }

	  }
	  else //顺时针旋转 “下”---Down --往---往水平方向移动
	  {
			if (judge_he_flag == 1)  //起始点位置在垂直的位置
			{

				sPID.SetPoint = setPositionEnd;

				iError = sPID.SetPoint + NextPoint; 
			}
			if(judge_he_flag ==2) //起点位置，在水平位置
			{
			     sPID.SetPoint = setPositionHome;
				 iError = sPID.SetPoint + NextPoint; 
			}
	  }
  
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
		
     int32_t speed_up =0,speed_cut =0,constant_speed=0;
	  PID_Result = LocPIDCalc(mCurPosValue);
	  PRINTF("PID_Result = %d \r\n",PID_Result);
	  Time_CNT++;
	  speed_up = abs(setPositionEnd - setPositionHome) / 3 ;
	  constant_speed = abs(setPositionEnd - setPositionHome) / 3 ;
	  speed_cut =abs(setPositionEnd - setPositionHome) / 3 ;
	//	PRINTF("vPortSetupTimerInterrupt = OK \r\n");
	 if( arithmetic_flag  == 1)
	 {
		/* 100ms 采样周期,控制周期 */
		if(Time_CNT % 50 == 0)
		{
		  /* 获取速度值:由捕获到的脉冲数除以总的时间 Pul/t */
		  /* BLDCMotor是4对极,旋转一圈有4个脉冲信号,3相UVW信号线,共12个脉冲信号,每个脉冲边沿计数一次,
		   * 所以BLDCM旋转一圈,可以捕获到的mCurPosValue
		   * 每个信号边沿都会捕获到接口定时器的CCR1,每100ms读取捕获到的时间和脉冲数,就可以得到电机的速度值.
		   */
		   PID_Result = LocPIDCalc(mCurPosValue);
		   PRINTF("PID_Result = %d \r\n",PID_Result);
	   #if 1
		  /* 限定PWM数值范围 */
		  if(PID_Result==0)//判断方向，PID值＜ 0
		  {
              PMW_AllClose_ABC_Channel();
              DelayMs(50);
			  PRINTF("IRQ Dir = 000000000\r\n");
		  }
		  else if(PID_Result > 0)
		  {
		  	
			 if(Dir ==0 ) //"UP"--往垂直位置方向移动	
			   {
					if(PID_Result >=100)
				  	PID_Result = 90;
				   PRINTF("Dir= 0 to vertical run\r\n");
			   }
			   else //Dir ==1 ,"Down"---往水平方向移动
			   {
				  if(PID_Result >=100)
				  	PID_Result = 40;
				  PRINTF("Dir= 1 to horizontal run\r\n");
			   }
		   }
		   else
		   {
				GPIO_PinWrite(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,0);
				DelayMs(50);
				PRINTF("STOP run\r\n");
		   }
		 
	    #endif 
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
		 
		  
		   PWM_Duty = PID_Result;
		
		  //uwStep = HallSensor_GetPinState();
					   
		  //HALLSensor_Detected_BLDC(BLDCMotor.PWM_Duty);//HAL_TIM_TriggerCallback(&htimx_HALL); //换向函数,6步换向，无刷电机
		 
	
		}
	  }  
	  //50ms反馈一次数据
	  if(Time_CNT % 25 == 0)
	  {
		PRINTF("Tick position : %d\r\n", mCurPosValue);//Transmit_FB(ptr_FB);
	  }
	  if(Time_CNT == 50)
		Time_CNT = 0;
}




