#include "encoder.h"


encoder_t encap_t;

/***********************************************
	*
	*Function Name:void Encoder_Init(void)
	*

	*
************************************************/
void Capture_Input_Init(void)

{

	ftm_config_t ftmInfo;
	
		   ftm_dual_edge_capture_param_t edgeParam;


    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTC8 (pin 80) is configured as FTM3_CH4 */
    //PORT_SetPinMux(PORTC, 8U, kPORT_MuxAlt3);
    
    /* PORTC8 (pin 80) is configured as FTM3_CH6 */
    PORT_SetPinMux(PORTC, 10U, kPORT_MuxAlt3);
	// ftm_dual_edge_capture_param_t edgeParam;
	PORT_SetPinMux(PORTC, 11U, kPORT_MuxAlt3);
   
	FTM_GetDefaultConfig(&ftmInfo);
    /* Initialize FTM module */
    FTM_Init(DEMO_FTM_BASEADDR, &ftmInfo);
    
        edgeParam.mode=kFTM_Continuous;
        edgeParam.currChanEdgeMode = kFTM_FallingEdge;//kFTM_RisingEdge;
        edgeParam.nextChanEdgeMode = kFTM_RisingEdge;//kFTM_FallingEdge;
       /* Setup dual-edge capture on a FTM channel pair */
		FTM_SetupDualEdgeCapture(DEMO_FTM_BASEADDR, BOARD_FTM_INPUT_CAPTURE_CHANNEL_PAIR, &edgeParam, 1);

	

  
}
/***********************************************
	*
	*Function Name:void Encoder_Init(void)
	*

	*
************************************************/
uint16_t Capture_ReadPulse_Value(void)
{
		
		 volatile uint32_t g_firstChannelOverflowCount	 = 0u;
		 volatile uint32_t g_secondChannelOverflowCount  = 0u;

		
		 uint32_t capture1Val;
         uint32_t capture2Val;

      
	      int32_t pulseWidth;
       
	
		  /* Set the timer to be in free-running mode */
		  DEMO_FTM_BASEADDR->MOD = 0xFFFF;
		  FTM_StartTimer(DEMO_FTM_BASEADDR, kFTM_SystemClock);
	
	      capture1Val = DEMO_FTM_BASEADDR->CONTROLS[BOARD_FTM_INPUT_CAPTURE_CHANNEL_PAIR * 2].CnV;
		  capture2Val = DEMO_FTM_BASEADDR->CONTROLS[(BOARD_FTM_INPUT_CAPTURE_CHANNEL_PAIR * 2) + 1].CnV;
		 
		//  PRINTF("\r\nCapture value C(n)V=%x\r\n", capture1Val);
		//  PRINTF("\r\nCapture value C(n+1)V=%x\r\n", capture2Val);
	
		  /* FTM clock source is not prescaled and is
		   * divided by 1000000 as the output is printed in microseconds
		   */
		  pulseWidth =
			 (int32_t)(((g_secondChannelOverflowCount - g_firstChannelOverflowCount) * 65536 + capture2Val - capture1Val) + 1) /
			 (FTM_SOURCE_CLOCK / 1000000);

	   return pulseWidth;
	}
	





void ENC_SetPositionZero(ENC_Type *base, uint32_t value)
{
    base->UPOS = (uint16_t)(value << 16U); /* Set upper 16 bits. */
    base->LPOS = (uint16_t)(value);        /* Set lower 16 bits. */
}

