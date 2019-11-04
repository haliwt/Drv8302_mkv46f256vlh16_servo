#include "output.h"


/*******************************************
 *
 *Function Name:OUTPUT_Function_Init()
 *Function : initial
 *
 *
*******************************************/
void OUTPUT_Fucntion_Init(void)
{

     gpio_pin_config_t led_config = {
	 kGPIO_DigitalOutput ,
	  0,                    //set a defult output logic,which has no use input
    };

  //  port_pin_config_t config ;

    // LLWU_PE5_WUPE18(0) ;
    // LLWU_PE3_WUPE10(0);
   /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);
    CLOCK_EnableClock(kCLOCK_PortA);
    CLOCK_EnableClock(kCLOCK_PortB);
    
	//PORT_SetMultiplePinsConfig();
    
 
        PORT_SetPinMux(PORTC, 7U, kPORT_MuxAsGpio );
		PORT_SetPinMux(PORTC, 6U, kPORT_MuxAsGpio );
		PORT_SetPinMux(PORTC, 4U, kPORT_MuxAsGpio );

        PORT_SetPinMux(PORTB, 19U, kPORT_MuxAsGpio);
		PORT_SetPinMux(PORTB, 17U, kPORT_MuxAsGpio);
		PORT_SetPinMux(PORTB,  0U, kPORT_MuxAsGpio);
      
        PORT_SetPinMux(PORTA, 19U, kPORT_MuxAsGpio);
		
        GPIO_PinInit(GPIOC,  7, &led_config );
        GPIO_PinInit(GPIOC,  6, &led_config );
		GPIO_PinInit(GPIOC,  4, &led_config );

        GPIO_PinInit(GPIOB,    19, 	&led_config );
        GPIO_PinInit(GPIOB,    17, &led_config );
		GPIO_PinInit(GPIOB,    0,  &led_config );
        
        GPIO_PinInit(GPIOA,    19, &led_config );

   #if 1
      //  config.pullSelect= kPORT_PullDown;
        PORT_PinPullConfig(HW_GPIOC, 7, kPORT_PullDown);
        PORT_PinPullConfig(HW_GPIOC, 6, kPORT_PullDown);
        PORT_PinPullConfig(HW_GPIOC, 4, kPORT_PullDown);
        
        
   #endif    
     #if 0
        config.pullSelect= kPORT_PullDown;
        PORT_PinPullConfig(HW_GPIOC, 7, kPORT_PullDown);
        PORT_PinPullConfig(HW_GPIOC, 6, kPORT_PullDown);
        PORT_PinPullConfig(HW_GPIOC, 4, kPORT_PullDown);

        PORT_PinPullConfig(HW_GPIOB, 19, kPORT_PullDown);
        PORT_PinPullConfig(HW_GPIOB, 17, kPORT_PullDown);
        PORT_PinPullConfig(HW_GPIOB, 0, kPORT_PullDown);

        PORT_PinPullConfig(HW_GPIOA, 19, kPORT_PullDown);
    #endif 
       
      
}

/*******************************************
 *
 *Function Name:AIR_OUTPUT_F(void)
 *Function : air conditioner output
 *
 *
*******************************************/
void ABC_POWER_OUTPUT_Init(void)
{
   GPIO_QuickInit(HW_GPIOC,  1, kGPIO_Mode_OPP);
   
   A_POWER_OUTPUT =0;
   
}

/********************************************************************************************
 *
 *Function Name:2SD315AI_Check_Fault()
 *Function : air conditioner output
 *
 *
*********************************************************************************************/


void SD315AI_Check_Fault(void)
{
   /*Check U phase*/
          if(GPIO_PinRead(SD315AI_SO1_A_GPIO,SD315AI_SO1_A_PIN)==1 ||GPIO_PinRead(SD315AI_SO2_A_GPIO,SD315AI_SO1_A_PIN)==1)
          {
               GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,1);//SD315_VL_A_OUTPUT=0;
               LED3 = 0;
           
          }
          else
          {
              GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,0);//SD315_VL_A_OUTPUT=0;
              LED3 = 1;

          }
          /*Check V phase*/
          if(GPIO_PinRead(SD315AI_SO1_B_GPIO,SD315AI_SO1_B_PIN)== 1 ||GPIO_PinRead(SD315AI_SO2_B_GPIO,SD315AI_SO2_B_PIN)== 1)
          {
               
               GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,1);//SD315_VL_B_OUTPUT=0;
                LED3 = 0;

          }
          else
          {
               GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,0);//SD315_VL_B_OUTPUT=0;
               LED3 = 1;
          }
          /*Check W phase*/
          if(GPIO_PinRead(SD315AI_SO1_C_GPIO,SD315AI_SO1_C_PIN)== 1 || GPIO_PinRead(SD315AI_SO2_C_GPIO,SD315AI_SO2_C_PIN)== 1)
          {
                GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,1);
                LED3 = 0;
          }
          else
          {
                GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,0);
                LED3 = 1;
          }
		




}


/********************************************************************************************
 *
 *Function Name:SD315AI_Disable_Output()
 *Function : air conditioner output
 *
 *
*********************************************************************************************/
void SD315AI_Disable_Output(void)
{

     GPIO_PinWrite(SD315AI_VL_A_GPIO,SD315AI_VL_A_PIN,1);//SD315_VL_A_OUTPUT=0;
     GPIO_PinWrite(SD315AI_VL_B_GPIO,SD315AI_VL_B_PIN,1);//SD315_VL_B_OUTPUT=0;
     GPIO_PinWrite(SD315AI_VL_C_GPIO,SD315AI_VL_C_PIN,1);
     LED3 = 0;

}






