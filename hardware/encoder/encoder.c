#include "encoder.h"



/***********************************************
	*
	*Function Name:void Encoder_Init(void)
	*

	*
************************************************/
void Capture_Input_Init(void)

{
	ftm_config_t ftmInfo;


	 /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PORTC8 (pin 80) is configured as FTM3_CH4 */
    //PORT_SetPinMux(PORTC, 8U, kPORT_MuxAlt3);
    
     /* PORTC8 (pin 80) is configured as FTM3_CH6 */
    PORT_SetPinMux(PORTC, 10U, kPORT_MuxAlt3);
     /* PORTC8 (pin 80) is configured as FTM3_CH7 */
    PORT_SetPinMux(PORTC, 11U, kPORT_MuxAlt3);

	
	FTM_GetDefaultConfig(&ftmInfo);
    /* Initialize FTM module */
    FTM_Init(BOARD_FTM_BASEADDR, &ftmInfo);

    /* Setup dual-edge capture on a FTM channel pair */
  //  FTM_SetupInputCapture(BOARD_FTM_BASEADDR, BOARD_FTM_INPUT_CAPTURE_CHANNEL, kFTM_FallingEdge, 0);
    FTM_SetupInputCapture(BOARD_FTM_BASEADDR, BOARD_FTM_INPUT_CAPTURE_CHANNEL_1, kFTM_FallingEdge  , 3);
    FTM_SetupInputCapture(BOARD_FTM_BASEADDR, BOARD_FTM_INPUT_CAPTURE_CHANNEL_2, kFTM_RisingEdge  , 3);

    /* Set the timer to be in free-running mode */
    BOARD_FTM_BASEADDR->MOD = 0xFFFF;

   FTM_StartTimer(BOARD_FTM_BASEADDR, kFTM_SystemClock);
	
}
/***********************************************
	*
	*Function Name:void Encoder_Init(void)
	*

	*
************************************************/

void ENC_SetPositionZero(ENC_Type *base, uint32_t value)
{
    base->UPOS = (uint16_t)(value << 16U); /* Set upper 16 bits. */
    base->LPOS = (uint16_t)(value);        /* Set lower 16 bits. */
}

