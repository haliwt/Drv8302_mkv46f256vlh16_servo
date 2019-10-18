#include "input.h"

void SD315AI_SO12_Input_Init(void)
{
    gpio_pin_config_t sd315ai_config = {
	  kGPIO_DigitalInput ,
	  0,
    };
    
    // port_pin_config_t config ;
    // LLWU_PE3_WUPE11(0); 
    // LLWU_PE5_WUPE19(0);
    // LLWU_PE6_WUPE20(0);
   /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);
    CLOCK_EnableClock(kCLOCK_PortC);

	//PORT_SetMultiplePinsConfig();

    /* 检测2SD315AI 输入信号 */
        PORT_SetPinMux(PORTE, 19U, kPORT_MuxAsGpio);
		PORT_SetPinMux(PORTE, 18U, kPORT_MuxAsGpio);
		PORT_SetPinMux(PORTE, 17U,kPORT_MuxAsGpio);

        PORT_SetPinMux(PORTC, 11U, kPORT_MuxAsGpio);
		PORT_SetPinMux(PORTC, 10U, kPORT_MuxAsGpio);
		PORT_SetPinMux(PORTC,  9U, kPORT_MuxAsGpio);
		
        GPIO_PinInit(GPIOE,    19, 	&sd315ai_config);
        GPIO_PinInit(GPIOE,    18, 	&sd315ai_config);
        
		GPIO_PinInit(GPIOE,    17, 	&sd315ai_config);
        GPIO_PinInit(GPIOC,    11, 	&sd315ai_config);

        GPIO_PinInit(GPIOC,    10, 	&sd315ai_config);
        GPIO_PinInit(GPIOC,    9, 	&sd315ai_config);

        #if 0

        config.pullSelect= kPORT_PullUp;
        
        PORT_SetPinConfig(PORTE,19,&config);
        PORT_SetPinConfig(PORTE,18,&config);
        PORT_SetPinConfig(PORTE,17,&config);


        PORT_SetPinConfig(PORTC,11,&config);
        PORT_SetPinConfig(PORTC,10,&config);
        PORT_SetPinConfig(PORTC, 9,&config);
        #endif

        #if 0

        PORT_PinPullConfig(HW_GPIOE, 19, kPORT_PullUp);
        PORT_PinPullConfig(HW_GPIOE, 18, kPORT_PullUp);
        PORT_PinPullConfig(HW_GPIOE, 17, kPORT_PullUp);
        PORT_PinPullConfig(HW_GPIOC, 11, kPORT_PullUp);
        PORT_PinPullConfig(HW_GPIOC, 10, kPORT_PullUp);
        PORT_PinPullConfig(HW_GPIOC,  9, kPORT_PullUp);
       #endif 
        
      
        
}








