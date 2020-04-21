#include <stdio.h>
#include "bsp_algorithm.h"

tpid_refer pid_r={0.01f,0.001f,0.1f,0.5f,0.01f,0.5f};
/****************************************************
	*
	*Function Name:void Search_Start_HorizonPos(void)
	*Function: start power automatic check horizon 
	*          position
	*
	*
*****************************************************/
void Search_Start_HorizonPos(void)
{
   int32_t mhorHold,mhorPosValue;
   int16_t horBuf[2]={0,0},ldiff=0;
   uint8_t i=0,j;
	/*judge home and end position twice*/
	printf("################################################################################# \n");
	
	motor_ref.motor_run = 0;
	en_t.First_H_dec = 1;
	en_t.oneKey_H_flag =1;
	
    mhorHold = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
	en_t.Pos_diff = (int16_t)ENC_GetHoldPositionDifferenceValue(DEMO_ENC_BASEADDR);

		if(en_t.eInit_n==1)PWM_Duty =0;
		else
		  PWM_Duty =30;
        uwStep = HallSensor_GetPinState();
        HALLSensor_Detected_BLDC(PWM_Duty);
		
		mhorPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
		i++;
		if(i==1)
			horBuf[0]=mhorHold;
		if(i==3)horBuf[1]=mhorPosValue;
		printf("hor_Buf_0 = %d\r\n",horBuf[0]);
        printf("hor_Buf_1 = %d\r\n",horBuf[1]);
		
		ldiff = abs(mhorPosValue);
	    /*horizon decelerate region */
		if(abs(en_t.X_axis)  < 1300 && ldiff >= 1000 ){
			    printf("Horizon_Pos <200 @@@@@@\r\n");
				
				for(j=0;j<100;j++){
				Dir =1;
				PWM_Duty =30;
				uwStep = HallSensor_GetPinState();
				HALLSensor_Detected_BLDC(PWM_Duty);
				printf("HorStartPos-1 : %ld\r\n", mhorPosValue);
				Dir =0;
				horBuf[0]=mhorPosValue;
				}
				horBuf[1]=mhorPosValue;
				if((en_t.Pos_diff==0||(abs(horBuf[0])==abs(horBuf[1])))){

				         PWM_Duty =0;
						 en_t.End_V_flag=2;
		                 en_t.eInit_n=1 ;
		                 en_t.Y_axis =0;
		                 en_t.X_axis = horBuf[0];
						 PMW_AllClose_ABC_Channel();
		                 //en_t.HorizonStop_flag=2;
						 motor_ref.motor_run = 0;
		                 printf("X_Pos_10 = %d\r\n",en_t.X_axis);
		                 printf("Y_Pos_10 = %d\r\n",en_t.Y_axis);
						

			}
				en_t.End_H_flag = 1;
		        	
		}
		if(i>=3)i=0;
	


}
/***************************************************
	*
	*Function Name:void Search_Start_VerticalPos(void)
	*
	*
	*
***************************************************/
void Search_Start_VerticalPos(void)
{
	
	int32_t mvHold, vBuf[2]={0,0};

	
	    mvHold = ENC_GetHoldPositionValue(DEMO_ENC_BASEADDR);
		en_t.Pos_diff = (int16_t)ENC_GetHoldPositionDifferenceValue(DEMO_ENC_BASEADDR);

		PWM_Duty =50;
        uwStep = HallSensor_GetPinState();
        HALLSensor_Detected_BLDC(PWM_Duty);
		
		 ENC_GetPositionValue(DEMO_ENC_BASEADDR);
        
		 vBuf[0]= mvHold;
		
          printf("vBuf_0 = %d\r\n",vBuf[0]);
          printf("vBuf_1 = %d\r\n",vBuf[1]);
									
			if(en_t.Pos_diff==0 &&( abs(vBuf[1])>900 || abs(vBuf[0])> 900)){

	                 PWM_Duty =30;
					 en_t.End_V_flag=2;
	                 en_t.eInit_n=1 ;
	                 en_t.X_axis =0;
	                 en_t.Y_axis = vBuf[0];
					 PMW_AllClose_ABC_Channel();
	                 motor_ref.motor_run = 0;
	                 printf("X_Pos_10 = %d\r\n",en_t.X_axis);
	                 printf("Y_Pos_10 = %d\r\n",en_t.Y_axis);
				
			}
		
		
}        
            
/************************************************
	*
	*Function Name:void Horizon_Decelerate(void)
	*
	*
	*
************************************************/
void Horizon_Decelerate(void)
{
    int16_t iError,last_iError,dError_sum;
	int16_t lhorizonpos,n;
    int32_t mCurPosValue;
	en_t.DIR_flag=0;
	mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
	
	iError =abs(mCurPosValue) - abs(en_t.X_axis) ; /*  pid error  */
	printf("mCurPosValue= %ld \n\r",mCurPosValue);
	printf("iError = %ld \r\n",iError);
	
	if(abs(en_t.X_axis) < 600){/*refer vertical*/
		lhorizonpos = abs(mCurPosValue);
		if(lhorizonpos <=400){
		for(n=0;n<600;n++){
		Dir =1;
		PWM_Duty =30;
		uwStep = HallSensor_GetPinState();
		HALLSensor_Detected_BLDC(PWM_Duty);
		//Dir =0;
        }
		 PWM_Duty = 0;
		 en_t.HorizonStop_flag =1;
		 dError_sum = 0;
			iError=0;
			last_iError =0;
         printf("Stop300 CurrPos ##############################: %ld\r\n", mCurPosValue);
		}
		HALL_Pulse =0;	
	}
	else if(abs(en_t.X_axis) >800){

		lhorizonpos =abs(mCurPosValue);
		if(lhorizonpos > 700){
			for(n=0;n<600;n++){
				Dir =1;
				PWM_Duty =30;
				uwStep = HallSensor_GetPinState();
				HALLSensor_Detected_BLDC(PWM_Duty);
			//	Dir =0;
			//	printf("Stop800 CurrPos : %ld\r\n", mCurPosValue);

			}
			PWM_Duty = 0;
			en_t.HorizonStop_flag =1;
			dError_sum = 0;
			iError=0;
			last_iError =0;
			printf("Stop800 CurrPos : %ld\r\n", mCurPosValue);
			}
			HALL_Pulse =0;					

	}


	
		dError_sum += iError; 

		if(dError_sum > 1000)dError_sum =1000; /*error accumulate */
		if(dError_sum < -1000)dError_sum = -1000; 
		PID_PWM_Duty = (int32_t)(iError *pid_r.KP_H + dError_sum * pid_r.KI_H + (iError - last_iError)*pid_r.KD_H);//proportion + itegral + differential

		//printf("hor_pwm= %ld\r \n",PID_PWM_Duty);

		PID_PWM_Duty = abs(PID_PWM_Duty)/2;
		if(PID_PWM_Duty >=50)PID_PWM_Duty=50;

		last_iError = iError;
		PWM_Duty = PID_PWM_Duty;
		HALL_Pulse =0;
		
	if(en_t.HorizonStop_flag ==2){
		Dir = 1;
		PWM_Duty = 30;
		iError =0;
		last_iError =0;
		HALL_Pulse =0;
   }
	if(en_t.HorizonStop_flag ==1){
		Dir =1;
		PWM_Duty =30;
		uwStep = HallSensor_GetPinState();
		HALLSensor_Detected_BLDC(PWM_Duty);

	
	}



}

/************************************************
	*
	*Function Name: Vertical_Decelerate(void)
	*Function: vertical slowdown region 
	*
	*
************************************************/
void Vertical_Decelerate(void)
{
	uint16_t ldectnum;
    int32_t mCurPosValue;
	en_t.DIR_flag =1;
	mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
    PWM_Duty=50;
    en_t.Pos_diff = (int16_t)ENC_GetHoldPositionDifferenceValue(DEMO_ENC_BASEADDR);
   if(abs(en_t.X_axis) > 800 ){
		
		
		 if(abs(mCurPosValue)<250){
		  
			for(ldectnum =0;ldectnum<30;ldectnum++){
			 ldectnum++;
			 if(ldectnum <=30){
				 PWM_Duty = 30 - ldectnum;
			 }
			 else  PWM_Duty =0;
			 uwStep = HallSensor_GetPinState();
			 HALLSensor_Detected_BLDC(PWM_Duty);
			 printf("V>800 break !!!!!!!\r\n");
		 }
		 PMW_AllClose_ABC_Channel();
		 motor_ref.motor_run =0;
		 printf("V>800\r\n");
		 printf("V~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
		
	
		 en_t.DIR_flag=0;
		 }
		 
	}
	else if(abs(en_t.X_axis) < 200 ){
		
		 if(abs(mCurPosValue) >  900 ){

	       for(ldectnum =0;ldectnum<30;ldectnum++){
			 ldectnum++;
			if(ldectnum <=30){
				 PWM_Duty = 30 - ldectnum;
			 }
			 else  PWM_Duty =0;
			 uwStep = HallSensor_GetPinState();
			 HALLSensor_Detected_BLDC(PWM_Duty);
			 printf("V < 100 break #####\r\n");
			
		    }
		 
			 PMW_AllClose_ABC_Channel();
			 motor_ref.motor_run =0;
			 printf("v100Pos= %d \n\r",mCurPosValue);
			 printf("V < 100 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
		 
		}	
	}
	
}
/*****************************************
	*
	*
	*
	*
****************************************/

uint8_t SWAP(uint8_t *p,uint8_t *q)
{
     uint8_t temp = *p;
	 if(*p < *q){
		  *p = *q;
		  *q = temp;
	      return *q; 
	 
	 }
	 else return *q;

}
/***********************************************
	*
	*Function Name:void Self_Locking(void)
	*Function: itself locking somewhere position 
	*
	*
	*
	*
************************************************/
void Self_Locking(int16_t svalue)
{
   int16_t self_temp,lvalue,ldiff_value;
   int16_t error,pid_pwm,derror_sum ,last_error;
   float kp = 0.01,ki = 0.01,kd = 0.01;

   
   self_temp = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
   
   
   ldiff_value = self_temp - svalue ;
   if(ldiff_value == 0 || ldiff_value - 1==0 || ldiff_value + 1 ==0){

         if(Dir ==0){

	          Dir =1;
	          PWM_Duty =30; /*real be test*/
	          uwStep = HallSensor_GetPinState();
	          HALLSensor_Detected_BLDC(PWM_Duty);
	 		  
	          Dir =0;
         }
		 else{
              PWM_Duty =0; /*real be test*/
	          uwStep = HallSensor_GetPinState();
	          HALLSensor_Detected_BLDC(PWM_Duty);
		     
		 	}
   
   	}
   	while(ldiff_value > 2||ldiff_value < -2){
   	
			error =lvalue - en_t.X_axis ; /*  pid error  */
		    derror_sum += error; 
							
		    if(derror_sum > 1000)derror_sum =1000; /*error accumulate */
			if(derror_sum < -1000)derror_sum = -1000; 
            pid_pwm = (int32_t)(error *kp + derror_sum * ki + (error - last_error)*kd);//proportion + itegral + differential

			if(pid_pwm > 0 )/* motor to horizon  */
			{
               Dir = 1 ; /* reverser to direction vertical */
			   PWM_Duty = abs(pid_pwm);
	           uwStep = HallSensor_GetPinState();
	           HALLSensor_Detected_BLDC(PWM_Duty);
			
			}
			else{ /* motor to vertical*/
			    Dir = 0 ; /* reverser to direction horizon */
			   PWM_Duty = abs(pid_pwm);
	           uwStep = HallSensor_GetPinState();
	           HALLSensor_Detected_BLDC(PWM_Duty);
			
			}
		   lvalue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
			
   		}
   


}
/************************************************
	*
	*Function Name:void Balance_HorizonRegion(void)
	*
	*
	*
*************************************************/
void Balance_HorizonRegion(void)
{
	int32_t mCurPosValue,lhorizonpos;
	uint16_t z=0;
	
	mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR); /*read current position of value*/
	lhorizonpos =abs(abs(en_t.X_axis) - abs(mCurPosValue));

	printf("stop lhorizonpos = %d \n",lhorizonpos);
	if(lhorizonpos <300 ){

		for(z=0;z<200;z++){
		Dir =1;
		PWM_Duty =30;
		uwStep = HallSensor_GetPinState();
		HALLSensor_Detected_BLDC(PWM_Duty);
		printf("HorStopPos flag=1 300 : %d\r\n", mCurPosValue);
		Dir =0;
		PWM_Duty=0; 
		uwStep = HallSensor_GetPinState();
		HALLSensor_Detected_BLDC(PWM_Duty);
		DelayMs(1);
		mCurPosValue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
		if(abs(mCurPosValue)==(abs(en_t.X_axis -150))||abs(mCurPosValue)>(abs(en_t.X_axis)-150))
		en_t.HorizonStop_flag =2;
	}
	//PWM_Duty=0; 
	}
}
/***********************************************
	*
	*Function Name: iPrintf(void)
	*
	*
	*
************************************************/
void iPrintf(void)
{
	printf("run_HALL_dir = %ld\r\n", HALL_Pulse);
	printf("motor start pwm= %d\r\n",PWM_Duty);
	printf("motor Dir = %d\n",Dir);
	printf("VerticalPos = %ld\r\n",en_t.Y_axis);
	printf("HorzionPos = %ld\r\n",en_t.X_axis);
	printf("POS_diff = %d \r\n", en_t.Pos_diff);
	printf("iprintf = \r\n");

}
