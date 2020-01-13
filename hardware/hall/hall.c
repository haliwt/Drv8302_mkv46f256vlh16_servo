
#include "hall.h"

uint32_t mCurPosValue;

BLDC_Typedef BLDCMotor = {0,CW,0,0,100,0,0,0}; //CW = - 
PID_TypeDef  sPID; 
extern int32_t PID_Result ;


uint32_t Time_CNT = 0;
int32_t  flag = 0;

volatile  uint32_t ABZ_CNT;

uint8_t motor_en_stop_flag;




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
	 #endif 
	  if(Dir == 1) //Dir ==1 顺时针旋转 “下”---Down --往---往水平方向移动
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
void SysTick_IRQ_Handler  (void)
{

	 uint32_t total_value;
	 float  one_step ;	
	 static uint8_t i=0;
     static uint8_t j=0;
	 int32_t iError,dError;
	 uint8_t iEr_flag=0;
	 uint16_t before_value[2];
	 uint16_t before_times=0;
	//  mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
	  PRINTF("PID_Result = %d \r\n",PID_Result);
	  
	  Time_CNT++;
	  before_times++;

      if(Dir == 1) //Dir ==1 顺时针旋转 “下”---Down --往---往水平方向移动
	  {
			if (judge_he_flag == 1)  //起始点位置在垂直的位置,setHome > setEnd 
			{
				
			   iError =  array_data[1] - mCurPosValue;//setPositionEnd;
               iEr_flag =1;
				
			}
			if(judge_he_flag ==2) //起点位置，在水平位置
			{
			   iError =  array_data[0] - mCurPosValue;//setPositionHome
			   iEr_flag =2;
			}
	  }

	  
	   PID_Result =abs(iError);
	   if(before_times==1) before_value[0]=PID_Result;
	   else if(before_times==2)
	   	{
	   	   before_value[1]=PID_Result;
		   before_times =0;
	   	}
	   PRINTF("iError= %d \r\n",iError);

  if(( arithmetic_flag  == 1)&&(motor_en_stop_flag ==0))
    {
		/* 100ms 采样周期,控制周期 */
		//if(Time_CNT % 50 == 0)
		{
		  /* 获取速度值:由捕获到的脉冲数除以总的时间 Pul/t */
		  /* BLDCMotor是4对极,旋转一圈有4个脉冲信号,3相UVW信号线,共12个脉冲信号,每个脉冲边沿计数一次,
		   * 所以BLDCM旋转一圈,可以捕获到的mCurPosValue
		   * 每个信号边沿都会捕获到接口定时器的CCR1,每100ms读取捕获到的时间和脉冲数,就可以得到电机的速度值.
		   */
		  
		  // PID_Result = LocPIDCalc(mCurPosValue);
		   //PRINTF("PID_Result = %d \r\n",iError);
	 
		  /* 限定PWM数值范围 */
	   if(Dir ==1)
	   	{
		  if((PID_Result ==0))//判断方向，PID值＜ 0
		  {
              uwStep = HallSensor_GetPinState();
              HALLSensor_Detected_BLDC( PWM_Duty);
			  DelayMs(20);
              
          
			  PRINTF("PID PROCESS STOP 000\r\n");
		  }
		  else if(PID_Result !=0)
		  {
              total_value =abs(setPositionEnd)+abs(setPositionHome);

			  one_step = (float)total_value / 90 ;
			  if(PID_Result >= 100 || PID_Result < 0)
			  {
                  
				  i++;
				  if((before_value[0] > before_value[1])&&(before_value[0] > 100))
                  {
                     goto  BECODER;

				  } 
				 if(i==1)PID_Result =40;
               }
		       else 
BECODER:		  {
						
						if((before_value[0] > before_value[1])&&(before_value[0] < 100))
						{
							 ABZ_CNT = 40 - i ;
							if(ABZ_CNT ==0)
								{
							 	 motor_en_stop_flag=1;
										 PWM_Duty = 30;
								  uwStep = HallSensor_GetPinState();
					              HALLSensor_Detected_BLDC( PWM_Duty);
								  DelayMs(50);
				                  
				                   PWM_Duty = 20;
								  uwStep = HallSensor_GetPinState();
					              HALLSensor_Detected_BLDC( PWM_Duty);
								  DelayMs(50);
								 
								  PWM_Duty = 10;
								  uwStep = HallSensor_GetPinState();
					              HALLSensor_Detected_BLDC( PWM_Duty);
								  DelayMs(50);
								}
							else
								{
							  		PRINTF("ABZ_CNT1= %d \r\n",ABZ_CNT);
							  		 uwStep = HallSensor_GetPinState();
								   
					            	HALLSensor_Detected_BLDC(PWM_Duty);//HAL_TIM_TriggerCallback(&htimx_HALL); //换向函数,6步换向，无刷电机
								}
						}
						else
						{ 
						    ABZ_CNT = 40 - i ;
							if(ABZ_CNT ==0)
								{
							 	 motor_en_stop_flag=1;
								 PWM_Duty = 30;
								  uwStep = HallSensor_GetPinState();
					              HALLSensor_Detected_BLDC( PWM_Duty);
								  DelayMs(50);
				                  
				                   PWM_Duty = 20;
								  uwStep = HallSensor_GetPinState();
					              HALLSensor_Detected_BLDC( PWM_Duty);
								  DelayMs(50);
								 
								  PWM_Duty = 10;
								  uwStep = HallSensor_GetPinState();
					              HALLSensor_Detected_BLDC( PWM_Duty);
								  DelayMs(50);
								}
							else
								{
							  		PRINTF("ABZ_CNT2 = %d \r\n",ABZ_CNT);
							  		 uwStep = HallSensor_GetPinState();
								   
					            	HALLSensor_Detected_BLDC(PWM_Duty);//HAL_TIM_TriggerCallback(&htimx_HALL); //换向函数,6步换向，无刷电机
								}


						}

				      if(((PID_Result) < 30)||(PID_Result == 30))
				      {

						for(j=0;j<5;j++)
							{
						  motor_en_stop_flag=1;
						  PWM_Duty = 30;
						  uwStep = HallSensor_GetPinState();
			              HALLSensor_Detected_BLDC( PWM_Duty);
						  DelayMs(50);
		                  
		                   PWM_Duty = 20;
						  uwStep = HallSensor_GetPinState();
			              HALLSensor_Detected_BLDC( PWM_Duty);
						  DelayMs(50);
						 
						  PWM_Duty = 10;
						  uwStep = HallSensor_GetPinState();
			              HALLSensor_Detected_BLDC( PWM_Duty);
						  DelayMs(50);
						
						   // ABZ_CNT = PID_Result  ;
						   
							ABZ_CNT = 10;
						    PRINTF("ABZ_CNT20 = %d \r\n",ABZ_CNT);
							 uwStep = HallSensor_GetPinState();
							   
				             HALLSensor_Detected_BLDC(PWM_Duty);//HAL_TIM_TriggerCallback(&htimx_HALL); //换向函数,6步换向，无刷电机
								if(j==4)motor_en_stop_flag=1;
						   }
						}

				 }
			  }
        }
		      // uwStep = HallSensor_GetPinState();
					   
		     // HALLSensor_Detected_BLDC(PWM_Duty);//HAL_TIM_TriggerCallback(&htimx_HALL); //换向函数,6步换向，无刷电机
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





