#include "bldc.h"

pwm_config_t pwmConfig;

static void PWM_DRV_Init3PhPwm(void);



__IO int32_t uwStep = 0;      //六步引脚状态

__IO uint32_t Lock_Time = 0;  // 堵转电流

__IO int8_t Dir; //电机方向


__IO int16_t  PWM_Duty= 20;	 //占空比


/**********************************************************
 *
 *Function Name:HALL_Init(void)
 *
 *
 *
***********************************************************/
void HALL_Init(void)
{
    gpio_pin_config_t hall_config = {
	  kGPIO_DigitalInput ,
	  0,
    };

   //  port_pin_config_t pd_config;
	 

   /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);
    
   /* PORTB is configured as PTB1, */
     PORT_SetPinMux(PORTB, 1U, kPORT_MuxAsGpio);
     PORT_SetPinMux(PORTB, 2U, kPORT_MuxAsGpio);
	 PORT_SetPinMux(PORTB, 3U, kPORT_MuxAsGpio);

	 GPIO_PinInit(HALL_A_GPIO, HALL_A_GPIO_PIN, &hall_config);
	 GPIO_PinInit(HALL_B_GPIO, HALL_B_GPIO_PIN, &hall_config);
	 GPIO_PinInit(HALL_C_GPIO, HALL_C_GPIO_PIN, &hall_config);
    #if 0
	 pd_config.pullSelect =  kPORT_PullUp;//kPORT_PullDisable ;
	// pd_config.slewRate = kPORT_FastSlewRate ;


	 PORT_SetPinConfig(PORTB,1U,&pd_config);
	 PORT_SetPinConfig(PORTB,2U,&pd_config);
	 PORT_SetPinConfig(PORTB,3U,&pd_config);
	 #endif 
	 #if 1
     /*GPIO input pullup or pulldown or disable*/
	 PORT_PinPullConfig(HW_GPIOB, 1, kPORT_PullUp);
	 PORT_PinPullConfig(HW_GPIOB, 2, kPORT_PullUp);
	 PORT_PinPullConfig(HW_GPIOB, 3, kPORT_PullUp);

	
     #endif 
 
}
/*********************************************************
 *
 *Funtion Name: HallSensor_GetPinState
 *
 *
 *
*********************************************************/
uint32_t HallSensor_GetPinState(void)
{
  __IO  static uint32_t State ;
  State  = 0;
  if(GPIO_PinRead(GPIOB,1)!= GPIO_PIN_RESET)  //U
  {
    State |= 0x01U;
  }
  if(GPIO_PinRead(GPIOB,2) != GPIO_PIN_RESET)  //V
  {
    State |= 0x02U;
  }
  if(GPIO_PinRead(GPIOB,3) != GPIO_PIN_RESET)  //W
  {
    State |= 0x04U;
  }
   return State;

}
/******************************************************************
 *
 *Function Name: void PWM_BLDC_Init()
 *
 *
 *
******************************************************************/
void PWM_BLDC_Init(void)
{
    
    pwm_config_t pwmConfig;
    XBARA_Init(XBARA);
    XBARA_SetSignalsConnection(XBARA, kXBARA_InputVdd, kXBARA_OutputPwmAFault0);
    XBARA_SetSignalsConnection(XBARA, kXBARA_InputVdd, kXBARA_OutputPwmAFault1);
    XBARA_SetSignalsConnection(XBARA, kXBARA_InputVdd, kXBARA_OutputPwmAFault2);
    XBARA_SetSignalsConnection(XBARA, kXBARA_InputVdd, kXBARA_OutputPwmAFault3);

    /*
     * pwmConfig.enableDebugMode = false;
     * pwmConfig.enableWait = false;
     * pwmConfig.reloadSelect = kPWM_LocalReload;
     * pwmConfig.faultFilterCount = 0;
     * pwmConfig.faultFilterPeriod = 0;
     * pwmConfig.clockSource = kPWM_BusClock;
     * pwmConfig.prescale = kPWM_Prescale_Divide_1;
     * pwmConfig.initializationControl = kPWM_Initialize_LocalSync;
     * pwmConfig.forceTrigger = kPWM_Force_Local;
     * pwmConfig.reloadFrequency = kPWM_LoadEveryOportunity;
     * pwmConfig.reloadLogic = kPWM_ReloadImmediate;
     * pwmConfig.pairOperation = kPWM_Independent;
     */
    PWM_GetDefaultConfig(&pwmConfig);
   
	pwmConfig.enableDebugMode = true;  //WT.EDIT 2019-07-13
    /* Use full cycle reload */
    pwmConfig.reloadLogic =kPWM_ReloadPwmHalfCycle;//kPWM_ReloadPwmFullCycle;//kPWM_ReloadPwmHalfCycle; //WT.EDIT 2019-07-13 //kPWM_ReloadPwmFullCycle;
    /* PWM A & PWM B form a complementary PWM pair */
    pwmConfig.pairOperation   = kPWM_Independent; //WT.EDIT kPWM_ComplementaryPwmA;
    pwmConfig.enableDebugMode = true;
    pwmConfig.forceTrigger =  kPWM_Force_Local; //kPWM_Force_LocalReload;//kPWM_Force_Local;  //WT.EDIT
    

    /* Initialize submodule 0 */
    if (PWM_Init(BOARD_PWM_BASEADDR, kPWM_Module_0, &pwmConfig) == kStatus_Fail)
    {
        PRINTF("PWM initialization failed\n");
       // return 1;
    }

   /* Initialize submodule 1 */
    //pwmConfig.clockSource           = kPWM_Submodule0Clock;
    pwmConfig.clockSource           =  kPWM_BusClock; //WT.EDIT 2019-06-29
    //pwmConfig.initializationControl = kPWM_Initialize_MasterSync;
    pwmConfig.initializationControl = kPWM_Initialize_LocalSync;//WT.EDIT 2019-06-29
    if (PWM_Init(BOARD_PWM_BASEADDR, kPWM_Module_1, &pwmConfig) == kStatus_Fail)
    {
        PRINTF("PWM initialization failed\n");
      //  return 1;
    }

    /* Initialize submodule 2 the same way as submodule 1 */
    if (PWM_Init(BOARD_PWM_BASEADDR, kPWM_Module_2, &pwmConfig) == kStatus_Fail)
    {
        PRINTF("PWM initialization failed\n");
      //  return 1;
    }
    
    /* Call the init function with demo configuration */
    PWM_DRV_Init3PhPwm();

    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmA,  kPWM_SignedEdgeAligned, 0); 
   
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
    



	 //close PWMA_A1/B1 channel
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmA,  kPWM_SignedEdgeAligned, 0); 
   
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
    


	 //close PWMA_A0/B0 channel
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmA,  kPWM_SignedEdgeAligned, 0); 
   
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0);
   
    /* Set the load okay bit for all submodules to load registers from their buffer */
    PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_0 | kPWM_Control_Module_1 | kPWM_Control_Module_2, true);

    /* Start the PWM generation from Submodules 0, 1 and 2 */
    PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_0 | kPWM_Control_Module_1 | kPWM_Control_Module_2);
    
    PRINTF("PWMA init end \r\n");
}
/**************************************************************
 *
 *Function Name: PWM_DRV_Init3PhPwm(void)
 *Function Active :be used to kPWM_Control_Module_0 
 *
 *
 *
**************************************************************/


static void PWM_DRV_Init3PhPwm(void)
{
    

    uint16_t deadTimeVal;
    pwm_signal_param_t pwmSignal[2];
    uint32_t pwmSourceClockInHz;
    uint32_t pwmFrequencyInHz = 4000;//1300; //1.3KHZ


    pwmSourceClockInHz = PWM_SRC_CLK_FREQ;

    /* Set deadtime count, we set this to about 650ns */
    deadTimeVal = 650;//((uint64_t)pwmSourceClockInHz * 650) / 1000000000;

    pwmSignal[0].pwmChannel       = kPWM_PwmA;
    pwmSignal[0].level            = kPWM_HighTrue;
    pwmSignal[0].dutyCyclePercent = 50; /* 1 percent dutycycle */
    pwmSignal[0].deadtimeValue    = deadTimeVal;

    pwmSignal[1].pwmChannel = kPWM_PwmB;
    pwmSignal[1].level      = kPWM_HighTrue;
    /* Dutycycle field of PWM B does not matter as we are running in PWM A complementary mode */
    pwmSignal[1].dutyCyclePercent = 50;
    pwmSignal[1].deadtimeValue    = deadTimeVal;

   // PWM_SetupSwCtrlOut(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmA|kPWM_PwmB, false);
   // PWM_SetupSwCtrlOut(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmA|kPWM_PwmB, false);
   // PWM_SetupSwCtrlOut(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmA|kPWM_PwmB, false);

    /*********** PWMA_SM0 - phase A, configuration, setup 2 channel as an example ************/
    PWM_SetupPwm(BOARD_PWM_BASEADDR, kPWM_Module_0, pwmSignal, 2, kPWM_SignedEdgeAligned , pwmFrequencyInHz,
                 pwmSourceClockInHz);
	

    /*********** PWMA_SM1 - phase B configuration, setup PWM A channel only ************/
    PWM_SetupPwm(BOARD_PWM_BASEADDR, kPWM_Module_1, pwmSignal, 2, kPWM_SignedEdgeAligned , pwmFrequencyInHz,
                 pwmSourceClockInHz);


    /*********** PWMA_SM2 - phase C configuration, setup PWM A channel only ************/
    PWM_SetupPwm(BOARD_PWM_BASEADDR, kPWM_Module_2, pwmSignal, 2, kPWM_SignedEdgeAligned , pwmFrequencyInHz,
                 pwmSourceClockInHz);

}



/*********************************************************
 *
 *Funtion Name: HallSensor_Detected_BLDC
 *Function Active: HALL sensor detected signal 
 *Input Reference :No
 *
 *
*********************************************************/
void HALLSensor_Detected_BLDC()
{
      

  //uwStep = HallSensor_GetPinState();
 // PRINTF("uwStep = %d\n",uwStep);
  // __IO uint32_t tmp = 0; 
 if(Dir == 0)
  {
    uwStep = (uint32_t)7 - uwStep;        // 逆时针 CW = 7 - CCW;
  }
 
  
  /*---- six step changed phase */
  /*---- 1(001,U),IC2(010,V),IC3(100,W) ----*/
  PRINTF("uwStep = %d\n",uwStep);
 switch(uwStep)//switch(BLDCMotor.uwStep)
 {
    case 5://stm32 B+ A-  // B+ C-
       /*  PWM_A0 B 0 stop  */ 
       PWMA_Close_ABC_Channel(2);  //close C channel 
      
      /*  PWM_A1 and PWM_B1 output */
       PWMA_Select_BC_Channel(0);
     
     // PRINTF("uwStep = %d\n",uvw);
      break;
      
    case 4:// stm32 B+ C- // B+ A-
      /*  Channel configuration */ 
         PWMA_Close_ABC_Channel(0); //close A channel 
     
      /*  Channe3 configuration B+  */
         PWMA_Select_BC_Channel(1);
    
      
      //  PRINTF("uwStep = %d\n",uvw);
      break;
    
    case 6: //STM32 A+ C- //  C+ A-
      /*  Channe3 configuration */ 
        PWMA_Close_ABC_Channel(1); //close B channel 
   
    
      /*  Channel configuration A+ */
         PWMA_Select_AB_Channel(1);
      
    
     // PRINTF("uwStep = %d\n",uvw);
      break;
    case 2: //STM32 A+ B- // //C+ B-

         /*  Channe2 configuration */ 
         PWMA_Close_ABC_Channel(2); //close A channel 
     
         PWMA_Select_AB_Channel(0);
         
         break;
    
    
    case 3:// STM32 C+ B- //A + B -
      /*  Channel configuration */ 
          PWMA_Close_ABC_Channel(0); //close A channel 
      
        /*  Channe2 configuration */
         PWMA_Select_CA_Channel(1);
      
      break;
    
    case 1: //STM32 C+ A- //A+ C-

      /*  Channe3 configuration */       
        PWMA_Close_ABC_Channel(1); //close B channel 
 
        PWMA_Select_CA_Channel(0);
    
      break;
    
    
 default :
        break;
  }
 
  
  /* 立刻触发换向*/
 // HAL_TIM_GenerateEvent(&htimx_BLDC, TIM_EVENTSOURCE_COM);
 // __HAL_TIM_CLEAR_IT(htim, TIM_FLAG_COM);
  

 }





/**************************************************************
 *
 *Function Name: PWMA_A_Selection 
 *Function Active :first cycle of up -bridge for A channel
 *
 *
 *
**************************************************************/
void PWMA_Select_AB_Channel(uint8_t s_pwma)
{
     if(s_pwma == 0)  //A + B -
     	{
	    
	     /*PWMA A1 channel PWM output*/
         PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
	     PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmA,   kPWM_SignedEdgeAligned, PWM_Duty); 
	
	     PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmA,   kPWM_SignedEdgeAligned, 0);
		 PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmB,   kPWM_SignedEdgeAligned, PWM_Duty); 
	     PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_0| kPWM_Control_Module_1, true);
	 
	   //  PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_0 |kPWM_Control_Module_1 );
     	}
	 else if(s_pwma == 1)  //A + C -
     	{
	      
	     /*PWMA A1 channel PWM output*/
         PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmB, kPWM_SignedEdgeAligned, 0); 
	     PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmA, kPWM_SignedEdgeAligned, PWM_Duty); 
	  
	     PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmA, kPWM_SignedEdgeAligned, 0); //
		 PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmB, kPWM_SignedEdgeAligned, PWM_Duty); //
	     PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_0| kPWM_Control_Module_2, true);
	   
	  //   PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_0 |kPWM_Control_Module_2 );
     	}
    
}
/**************************************************************
 *
 *Function Name: PWMA_B_Selection 
 *Function Active :second  cycle of up -bridge for B channel
 *Input Reference : s_pwmb is Chanel B = PWMA_A1/B1
 *
 *
**************************************************************/

void PWMA_Select_BC_Channel(uint8_t s_pwmb)
{
  
     if(s_pwmb==0) //B+ A -
     {
	     
	     /*PWMA B channel PWM output*/
         PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
	     PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmA,   kPWM_SignedEdgeAligned, PWM_Duty); 

	    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmA,   kPWM_SignedEdgeAligned, 0);
		PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmB,   kPWM_SignedEdgeAligned, PWM_Duty); 
	    PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_1| kPWM_Control_Module_0, true);
	
	   //  PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_1 |kPWM_Control_Module_0 );
     }


	 else if(s_pwmb==1) //B+ C -
     {
	     /*PWMA B channel PWM output*/
        PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
	    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmA,   kPWM_SignedEdgeAligned, PWM_Duty); 
	
		 PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmA,   kPWM_SignedEdgeAligned, 0); //
		 PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmB,   kPWM_SignedEdgeAligned, PWM_Duty); //
	     PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_1| kPWM_Control_Module_2, true);
	
	   //  PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_1 |kPWM_Control_Module_2 );
     }
}
/**************************************************************
 *
 *Function Name: PWMA_C_Selection 
 *Function Active :second  cycle of up -bridge for C channel
 *Input Reference :channel C =PWMA_A2 B2 ,
 *Return Reference: No
 *
**************************************************************/

void PWMA_Select_CA_Channel(uint8_t s_pwmc)
{
 
     if(s_pwmc == 0) //C + A -
     {
	   
	     /*PWMA C channel PWM output*/
         PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
	     PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmA,   kPWM_SignedEdgeAligned, PWM_Duty); 
	  
	     PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmA,   kPWM_SignedEdgeAligned, 0); //
		 PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmB,   kPWM_SignedEdgeAligned, PWM_Duty); //
	     PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_0| kPWM_Control_Module_2, true);

	   //  PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_0 |kPWM_Control_Module_2 );
     }
	 else if(s_pwmc == 1) //C + B -
     {
	    //PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_1);
	     /*PWMA A1 channel PWM output*/
         PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
	     PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmA,   kPWM_SignedEdgeAligned, PWM_Duty); 
	
	     PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmA,   kPWM_SignedEdgeAligned, 0);
		 PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmB,   kPWM_SignedEdgeAligned, PWM_Duty); //
	     PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_1| kPWM_Control_Module_2, true);
	   
	  //  PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_1 |kPWM_Control_Module_2 );
     }
 
}

/**************************************************************
 *
 *Function Name: PWMABC_Selection 
 *Function Active :be used to kPWM_Control_Module_0 
 *
 *
 *
**************************************************************/
void PWMA_Close_ABC_Channel(uint8_t f_pwmabc)
{
 
 
    if(f_pwmabc == 0) //Close A channel
    	{
		    //close PWMA_A0/B0 channel
		    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmA,  kPWM_SignedEdgeAligned, 0); 
		   
		    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
		    
			PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_0, true);
		//	PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_0 );
		        
		   
    	}
	else if(f_pwmabc==1)  //Close B channel 
	{
         //close PWMA_A1/B1 channel
		    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmA,  kPWM_SignedEdgeAligned, 0); 
		   
		    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
		    
			PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_1, true);
         //   PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_1 );
	}
	else if(f_pwmabc ==2) //close C channel
	{
         //close PWMA_A0/B0 channel
		    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmA,  kPWM_SignedEdgeAligned, 0); 
		   
		    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
		    
           PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_2, true);
          //  PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_2 );

	}
    
 
    
}
/**************************************************************
 *
 *Function Name: PWM_ALLClose_ABC_Channel
 *Function Active :be used to kPWM_Control_Module_0 
 *
 *
 *
**************************************************************/

void PMW_AllClose_ABC_Channel(void)
{

   
   //close PWMA_A0/B0 channel
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmA,  kPWM_SignedEdgeAligned, 0); 
   
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
    
	PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_0, true);


	 //close PWMA_A1/B1 channel
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmA,  kPWM_SignedEdgeAligned, 0); 
   
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
    
	PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_1, true);

	 //close PWMA_A0/B0 channel
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmA,  kPWM_SignedEdgeAligned, 0); 
   
    PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmB,   kPWM_SignedEdgeAligned, 0); 
    
	PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_2, true);
 
  
}
/********************************************************************************************************
    *
    *Function Name  : void BLDC_CCW_SIX_1_Run(void)
    *
    *
    *
    *

*********************************************************************************************************/
#if 1 
void BLDC_CCW_SIX_1_Run(void)
{
    uint8_t i ;
    for(i = 0;i<3 ;i++)
  {
      if(i == 0)
        PWM_Duty = 60;
      else if(i==1)PWM_Duty = 70;
      else if(i==2)PWM_Duty = 80;
    
      uwStep = 1;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 5;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 4 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 6 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 2 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 3 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
  }

}

#endif
/********************************************************************************************************
    *
    *Function Name  : void BLDC_CCW_SIX_2_Run(void)
    *
    *
    *
    *

*********************************************************************************************************/
void BLDC_CCW_SIX_2_Run(void)
{
    uint8_t i;
    for(i = 0;i<3 ;i++)
    {
        if(i == 0)
            PWM_Duty = 60;
          else if(i==1)PWM_Duty = 70;
          else if(i==2)PWM_Duty = 80;
        
          uwStep = 2;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 3;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 1 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 5 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 4 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 6 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
    }

}

/********************************************************************************************************
    *
    *Function Name  : void BLDC_CCW_SIX_3_Run(void)
    *Funciton Active :  电机逆时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CCW_SIX_3_Run(void)
{
    uint8_t i;
  for(i = 0;i<3 ;i++)
    {
      if(i == 0)
        PWM_Duty = 60;
      else if(i==1)PWM_Duty = 70;
      else if(i==2)PWM_Duty = 80;
      uwStep = 3;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 1;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 5 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 4 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 6 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 2 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
    }

}
/********************************************************************************************************
    *
    *Function Name  : void BLDC_CCW_SIX_4_Run(void)
    *Funciton Active :  电机逆时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CCW_SIX_4_Run(void)
{
   uint8_t i;
    for(i = 0;i<3 ;i++)
    {
       if(i == 0)
           PWM_Duty = 60;
          else if(i==1)PWM_Duty = 70;
          else if(i==2)PWM_Duty = 80;
        uwStep = 4;
        HALLSensor_Detected_BLDC();
          DelayMs(8);
        uwStep = 6;
        HALLSensor_Detected_BLDC();
          DelayMs(8);
        uwStep = 2 ;
        HALLSensor_Detected_BLDC();
          DelayMs(8);
        uwStep = 3 ;
        HALLSensor_Detected_BLDC();
        DelayMs(8);
        uwStep = 1 ;
        HALLSensor_Detected_BLDC();
          DelayMs(8);
        uwStep = 5 ;
        HALLSensor_Detected_BLDC();
    }

}
/********************************************************************************************************
    *
    *Function Name  : void BLDC_CCW_SIX_5_Run(void)
    *Funciton Active :  电机逆时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CCW_SIX_5_Run(void)
{
     uint8_t i;
     for(i = 0; i<3;i++)
      {
          
          if(i == 0)
            PWM_Duty = 60;
          else if(i==1)PWM_Duty = 70;
          else if(i==2)PWM_Duty = 80;
          uwStep = 5;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 4;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 6 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 2 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 3 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 1 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
      }

}

/********************************************************************************************************
    *
    *Function Name  : void BLDC_CCW_SIX_6_Run(void)
    *Funciton Active :  电机逆时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CCW_SIX_6_Run(void)
{
    uint8_t i;
    for(i = 0;i<3 ;i++)
      {
          
          if(i == 0)
            PWM_Duty = 60;
          else if(i==1)PWM_Duty = 70;
          else if(i==2)PWM_Duty = 80;
          uwStep = 6;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 2;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 3 ;
          HALLSensor_Detected_BLDC();
           DelayMs(8);
          uwStep = 1 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 5 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 4 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
      }

}

/********************************************************************************************************
    *
    *Function Name  : void BLDC_CW_Six_1_Run(void)
    *Funciton Active : 电机顺时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CW_Six_1_Run(void)
{
   uint8_t i ;
    for(i = 0;i<3 ;i++)
  {
      if(i == 0)
        PWM_Duty = 60;
      else if(i==1)PWM_Duty = 70;
      else if(i==2)PWM_Duty = 80;
    
      uwStep = 1;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 3;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 2 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 6 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 4 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
      uwStep = 5 ;
      HALLSensor_Detected_BLDC();
      DelayMs(8);
  }



}

/********************************************************************************************************
    *
    *Function Name  : void BLDC_CW_Six_2_Run(void)
    *Funciton Active :  电机逆时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CW_Six_2_Run(void)
{
    uint8_t i;
    for(i = 0;i<3 ;i++)
    {
        if(i == 0)
            PWM_Duty = 60;
          else if(i==1)PWM_Duty = 70;
          else if(i==2)PWM_Duty = 80;
        
          uwStep = 2;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 6;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 4 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 5 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 1 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 3 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
    }



}

/********************************************************************************************************
    *
    *Function Name  : void BLDC_CW_Six_3_Run(void)
    *Funciton Active :  电机逆时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CW_Six_3_Run(void)
{
    
uint8_t i;
    for(i = 0;i<3 ;i++)
    {
        if(i == 0)
            PWM_Duty = 60;
          else if(i==1)PWM_Duty = 70;
          else if(i==2)PWM_Duty = 80;
        
          uwStep = 3;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 2;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 6 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 4 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 5 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 1 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
    }

}
/********************************************************************************************************
    *
    *Function Name  : void BLDC_CW_Six_4_Run(void)
    *Funciton Active :  电机逆时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CW_Six_4_Run(void)
{
    
uint8_t i;
    for(i = 0;i<3 ;i++)
    {
        if(i == 0)
            PWM_Duty = 60;
          else if(i==1)PWM_Duty = 70;
          else if(i==2)PWM_Duty = 80;
        
          uwStep = 4;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 5;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 1 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 3 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 2 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 6 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
    }

}
/********************************************************************************************************
    *
    *Function Name  : void BLDC_CW_Six_5_Run(void)
    *Funciton Active :  电机逆时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CW_Six_5_Run(void)
{
    
uint8_t i;
    for(i = 0;i<3 ;i++)
    {
        if(i == 0)
            PWM_Duty = 60;
          else if(i==1)PWM_Duty = 70;
          else if(i==2)PWM_Duty = 80;
        
          uwStep = 5;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 1;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 3 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 2 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 6 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 4 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
    }

}
/********************************************************************************************************
    *
    *Function Name  : void BLDC_CW_Six_6_Run(void)
    *Funciton Active :  电机逆时针旋转
    *
    *
    *

*********************************************************************************************************/
void BLDC_CW_Six_6_Run(void)
{

    
uint8_t i;
    for(i = 0;i<3 ;i++)
    {
        if(i == 0)
            PWM_Duty = 60;
          else if(i==1)PWM_Duty = 70;
          else if(i==2)PWM_Duty = 80;
        
          uwStep = 6;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 4;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 5 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 1 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 3 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
          uwStep = 2 ;
          HALLSensor_Detected_BLDC();
          DelayMs(8);
    }

}



