#include "led.h"

 gpio_pin_config_t led_config = {
	  kGPIO_DigitalOutput,
	  0,
      };


void LED_Init(void)
{
   
    

     GPIO_QuickInit(HW_GPIOD,  7, kGPIO_Mode_OPP);
	 GPIO_QuickInit(HW_GPIOD,  6, kGPIO_Mode_OPP);
#if 0

/* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortD);
    
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PORTD0 (pin 93) is configured as PTD0, */
    PORT_SetPinMux(PORTD, 7U, kPORT_MuxAsGpio);
    
    PORT_SetPinMux(PORTD, 6U, kPORT_MuxAsGpio);

    GPIO_PinInit(BOARD_LED1_GPIO, BOARD_LED1_GPIO_PIN,   &led_config);
    GPIO_PinInit(BOARD_LED2_GPIO, BOARD_LED2_GPIO_PIN,   &led_config);

#endif
}


