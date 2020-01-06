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
    CLOCK_EnableClock(kCLOCK_PortE);
    
	//PORT_SetMultiplePinsConfig();
    
        PORT_SetPinMux(PORTE, DRV8302_EN_GATE_GPIO_PIN, kPORT_MuxAsGpio );

		GPIO_PinInit(DRV8302_EN_GATE_GPIO,DRV8302_EN_GATE_GPIO_PIN,&led_config );

		
       
	

        PORT_SetPinMux(PORTB, 19U, kPORT_MuxAsGpio);
		PORT_SetPinMux(PORTB, 17U, kPORT_MuxAsGpio);
		PORT_SetPinMux(PORTB,  0U, kPORT_MuxAsGpio);
      
        PORT_SetPinMux(PORTA, 19U, kPORT_MuxAsGpio);
		
      

        GPIO_PinInit(GPIOB,    19, 	&led_config );
        GPIO_PinInit(GPIOB,    17, &led_config );
		GPIO_PinInit(GPIOB,    0,  &led_config );
        
        GPIO_PinInit(GPIOA,    19, &led_config );

   #if 0
      //  config.pullSelect= kPORT_PullDown;
        PORT_PinPullConfig(HW_GPIOC, 7, kPORT_PullDown);
       
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



/********************************************************************************************
 *
 *Function Name:2SD315AI_Check_Fault()
 *Function : air conditioner output
 *
 *
*********************************************************************************************/







