#include <stdio.h>
#include "bsp_algorithm.h"

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
void Self_Locking(void)
{
   int16_t self_temp,lvalue,ldiff_value;
   int16_t error,pid_pwm,derror_sum ,last_error;
   float kp = 0.01,ki = 0.01,kd = 0.01;

   
   self_temp = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
   DelayMs(1000);
   lvalue = ENC_GetPositionValue(DEMO_ENC_BASEADDR);
   ldiff_value = self_temp - lvalue ;
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
   	
			error =lvalue - en_t.Horizon_Position ; /*  pid error  */
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
