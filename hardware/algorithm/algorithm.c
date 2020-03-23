#include "algorithm.h"

pid_t algpid_t ;
/*************************************************
	*
	*函数名：
	*函数功能：电机水平运动减速算法函�?
	*
	*
**************************************************/
void Horizontal_Decelerate_Function(void)
{
		int32_t HDff;
		int8_t m,n;
		algpid_t.iError =abs( (algpid_t.mCurPosValue - en_t.Horizon_Position)/en_t.VH_Total_Dis);//iError =[0 1]
		if(algpid_t.iError <= 20 && algpid_t.iError >= -20)algpid_t.iError =0; //error range 
		#ifdef DEBUG_PRINT
	   		PRINTF("HB0= %d \n\r",en_t.Horizon_Position);
	      	PRINTF("mCurPosValue= %d \n\r",mCurPosValue);
		    PRINTF("currHALL= %d \n\r",HALL_Pulse);
		#endif
	   	HDff = algpid_t.iError; 
	   	if(HDff>=0)
	  		PRINTF("HDff = %d \n\r",HDff);
	   	else
	   		PRINTF("-HDff = - %d \n\r",HDff);
	     //¼ÆËãË®Æ½Î»ÖÃµÄ¼õËÙÇø
	   	if(((HDff <= 400 && HDff >= -400)&&(HALL_Pulse>10))&&(en_t.eInit_n==1)){

			m++;
		
		    if(m>=2){
	              HDff = abs(HDff);
				  en_t.HorizonStop_flag =2;
				   
					//en_t.HorizonStop_flag =1;
					PID_PWM_Duty=0; //WT.EDIT 2020-03-12
					PRINTF("HDff= %d\r\n",HDff);
					PRINTF("PID_PWM_Duty = %d\r\n",PID_PWM_Duty);
					HALL_Pulse =0;
					PRINTF("HHHHHHHHHH\r\n");
					BLDCMotor.Lock_Time=0;
					algpid_t.iError =0;
				
					for(n =0;n<250;n++)//300 times motor run to Vertical is error
					{
	                    Dir =1;
					    PWM_Duty=50;
						uwStep = HallSensor_GetPinState();
						HALLSensor_Detected_BLDC(PWM_Duty);
					
				    	Dir =0;
		        	}
					
					      
				     PRINTF("nnnnnnnn---- \r\n");
				    if(m>=2)m=0;
								   
			}
				
		}

	if(en_t.HorizonStop_flag ==2){
		
		    PWM_Duty = PID_PWM_Duty;
	}
	else {//Ë®Æ½Î»ÖÃ
			algpid_t.dError_sum += algpid_t.iError; 
			
		    if(algpid_t.dError_sum > 50)algpid_t.dError_sum =50; /*error accumulate */
			if(algpid_t.dError_sum < -50)algpid_t.dError_sum = -50; 
	        PID_PWM_Duty = (int32_t)(algpid_t.iError *P_DATA + algpid_t.dError_sum * I_DATA + (algpid_t.iError - algpid_t.last_iError)*D_DATA);//proportion + itegral + differential
	        #ifdef DEBUG_PRINT
				if(PID_PWM_Duty > 0)
					PRINTF("PID pwm= %d\r \n",PID_PWM_Duty);
				else if(PID_PWM_Duty == 0)PRINTF("PID pwm= %d\r \n",PID_PWM_Duty);
				else PRINTF("-PID pwm= -%d\r \n",PID_PWM_Duty);
			#endif
			PID_PWM_Duty = abs(PID_PWM_Duty);
			if(PID_PWM_Duty >=50)PID_PWM_Duty=50;
	      
	     	algpid_t.last_iError = algpid_t.iError;
			PWM_Duty = PID_PWM_Duty;
		
	}
	HALL_Pulse =0;	

}
/*************************************************
	*
	*函数名：
	*函数功能：电机垂直运动减速算法函�?
	*
	*
**************************************************/
void Vertical_Decelerate_Function(void)
{
  		int32_t VDff;
		algpid_t.iVError = abs((algpid_t.mCurPosValue -en_t.Vertical_Position) /en_t.VH_Total_Dis);
			if(algpid_t.iVError <= 20 && algpid_t.iVError >= -20)algpid_t.iVError =0;
		#ifdef DEBUG_PRINT
			PRINTF("HB0= %d \n\r",en_t.Vertical_Position);
	      	PRINTF("mCurPosValue= %d \n\r",mCurPosValue);
		    PRINTF("currHALL= %d \n\r",HALL_Pulse);
		#endif
	   	VDff = algpid_t.iVError;
		#ifdef DEBUG_PRINT
		   	if(VDff>=0)
		      	PRINTF("VDff = %d \n\r",VDff);
			 else
			   	PRINTF("-VDff = - %d \n\r",VDff);
	    #endif 
		
			VDff = abs(VDff);
	   HALL_Pulse = abs(HALL_Pulse);
	   PRINTF("VDff= %d  \r\n",VDff);
	   PRINTF("VcurrHALL= %d \n\r",HALL_Pulse);
	  if((( VDff<=50 )&&(HALL_Pulse>2))&&en_t.eInit_n==1){
	  	      BLDCMotor.Lock_Time ++;
			  BLDCMotor.Position = VDff;
	          HALL_Pulse = 0;
			 if(BLDCMotor.Lock_Time >=2){//Â¼Ã†Ã‹Ã£ÂµÂ¼Â¸Ã‹ÂµÂ½Â´Â¹Ã–Â±ÃŽÂ»Ã–ÃƒÂµÃ„Â¼ÃµÃ‹Ã™Ã‡Ã�?
		
					    Dir =0;
				    PWM_Duty=30;
					uwStep = HallSensor_GetPinState();
					HALLSensor_Detected_BLDC(PWM_Duty);
				
			    	Dir =1;                       

					 PRINTF("VDff= %d \r\n",VDff);
	                 PMW_AllClose_ABC_Channel();
	                 motor_ref.motor_run =0;
					 DelayMs(1000);
	                 PRINTF("locktime = %d\r\n",BLDCMotor.Lock_Time);
	                 BLDCMotor.Lock_Time=0;
	                 PRINTF("VVVVVVVV\r\n");
					 HALL_Pulse =0;
					 algpid_t.iVError =0;
					 algpid_t.dError_sum=0;
					 algpid_t.last_iError=0;
					 PRINTF("PID_PWM= %d \r\n",PID_PWM_Duty);
	            }
	}
				
	algpid_t.dError_sum += algpid_t.iVError; 

	if(algpid_t.dError_sum > 50)algpid_t.dError_sum =50; 
	if(algpid_t.dError_sum < -50)algpid_t.dError_sum = -50; 
	PID_PWM_Duty = (int32_t)(algpid_t.iVError *VP_DATA + algpid_t.dError_sum * VI_DATA + (algpid_t.iVError - algpid_t.last_iError)*VD_DATA);//proportion + itegral + differential
#ifdef DEBUG_PRINT
		if(PID_PWM_Duty > 0)
			PRINTF("PID pwm= %d\r \n",PID_PWM_Duty);
		else if(PID_PWM_Duty == 0)PRINTF("PID pwm= %d\r \n",PID_PWM_Duty);
		else PRINTF("-PID pwm= -%d\r \n",PID_PWM_Duty);
#endif
	PID_PWM_Duty = abs(PID_PWM_Duty);

		algpid_t.last_iError = algpid_t.iVError;
	PWM_Duty = PID_PWM_Duty;
	HALL_Pulse =0;
}
