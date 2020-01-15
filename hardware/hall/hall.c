
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
void SysTick_IRQ_Handler  (void)
{

	 uint32_t total_value;
	 uint32_t  one_step ;	
	 static uint8_t i=0;
     static uint8_t j=0;
	 int32_t iError,dError;
	 uint8_t iEr_flag=0;
	 uint8_t ne_symbol = 0;
	 uint16_t comp_array[2];
	 uint8_t com_result;
	 uint8_t pid_stop=0;
	// mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
	 
	 PRINTF("setHome = %d \r\n",setHome);
	 PRINTF("setEnd= %d \r\n",setEnd); 
	 #if 0
	if(Dir == 1) //Dir ==1 顺时针旋转 “下”---Down --往---往水平方向移动
	  {
			if (judge_home_flag == 1)   //ֻ起始点在水平位置
			{
				
			   iError =  array_data[0] - mCurPosValue;//setPositionHome;
               iEr_flag =1;
				
			}
			if(judge_home_flag ==2) //起点位置，垂直位置
			{
			   iError =  array_data[1] - mCurPosValue;//setPositionEnd
			   iEr_flag =2;
			}
	  }
	   PID_Result =iError;
	   #endif 
	 //  PRINTF("iError= %d \r\n",iError);
	  
		/**/
	   if(j==1) comp_array[0]=PID_Result;
	   else if(j==2)
	   {
			comp_array[1]=PID_Result;
			j=0;
	   }
	   com_result = abs(comp_array[1]- comp_array[0]);
	   
	  PWM_Duty =60;

if( arithmetic_flag  == 1)
  {
		/* 100ms 采样周期,控制周期 */
		//if(Time_CNT % 50 == 0)
		
		  /* 获取速度值:由捕获到的脉冲数除以总的时间 Pul/t */
		  /* BLDCMotor是4对极,旋转一圈有4个脉冲信号,3相UVW信号线,共12个脉冲信号,每个脉冲边沿计数一次,
		   * 所以BLDCM旋转一圈,可以捕获到的mCurPosValue
		   * 每个信号边沿都会捕获到接口定时器的CCR1,每100ms读取捕获到的时间和脉冲数,就可以得到电机的速度值.
		   */
		  
		  // PID_Result = LocPIDCalc(mCurPosValue);
		   //PRINTF("PID_Result = %d \r\n",iError);
		    /*-PID_Result 在此语句执行 递减*/
		total_value =abs(array_data[0])+abs(array_data[1]); //1107 /90=12.3

		one_step = total_value / 90 ;
	 
		  
		   if((com_result!=0)&&(pid_stop ==0))
		   {
			  Time_CNT++;
              ABZ_CNT = 1;
			//  motor_ref.power_on =4;
			  motor_ref.motor_run=1; 
			//  PRINTF("motor_ref.power_on= %d \r\n", motor_ref.power_on);
			//  PRINTF("ABZ_CNT = %d \r\n",com_result);
			//  PRINTF("total_value = %d \r\n",total_value);
			
			 // if(Time_CNT == one_step)
			  {
				//PRINTF("one_step = %d \r\n",one_step);
				uwStep = HallSensor_GetPinState();
		        HALLSensor_Detected_BLDC(PWM_Duty);
				//DelayMs(10);
			  }
		      if(setStop_flag ==1)pid_stop =1;
           }
		  
		  /* 限定PWM数值范围 */
	       if ((judge_home_flag == 1) &&(pid_stop ==1) ) //ֻ起始点在水平位置
		   {
			
				PWM_Duty = 60;
				if((com_result ==0) ||(array_data[2]==capture_width)\
					|| ((array_data[2] -10 < capture_width )&&(array_data[2]+10 > capture_width)))
				{
						
		STOP:		
                    i++;
                    PRINTF("i == %d \r\n",i);
					if(g_destination_end == capture_width || g_destination_end + 5 > capture_width)
					{
						PWM_Duty =0;
						pid_stop =1;
					    motor_ref.motor_run=0; 
						PRINTF("Destination STOP ^^^^^^^^^^^^^^&&\r\n");
					}
					uwStep = HallSensor_GetPinState();
					HALLSensor_Detected_BLDC(PWM_Duty);
					DelayMs(500);
					//PRINTF("PWM_Duty == %d \r\n",PWM_Duty);
					
					PRINTF("PID PROCESS STOP &&&&&&&&&&&&&&&&&&&&&&&&\r\n");
							
				}
		   }
		 if((judge_home_flag ==2) &&(pid_stop ==1))//起点位置，垂直位置
		   {
			   	if((com_result ==0) ||(array_data[3]==capture_width)\
					|| (array_data[3] -10 < capture_width )&&(array_data[3]+10 < capture_width))
					{
							PRINTF("flag =2 start vertial \r\n");
							goto STOP;

					}

            }
		
		}
    
	  //50ms反馈一次数据
	 // if(Time_CNT % 10 == 0)
	 // {
		//PRINTF("Tick position : %d\r\n", mCurPosValue);//Transmit_FB(ptr_FB);
	 // }
	  if(Time_CNT == one_step)
		Time_CNT = 0;

}



