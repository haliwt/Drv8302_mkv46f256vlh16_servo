#include "encoder.h"



volatile bool encoder_direction         = false;



/*********************************************************
	*
	*Function Name:void FTM_Qudrature_Init()
	*Function :initinal FTM GPIO qudrature
	*
	*
	*
**********************************************************/
void FTM_Quadrature_Init(void)
{

	ftm_config_t ftmInfo;
    ftm_phase_params_t phaseParamsConfigStruct;
    /* Port A Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortA);
 
    /* PORTA12 (pin 42) is configured as FTM1_QD_PHA */
    PORT_SetPinMux(PORTA, 12U, kPORT_MuxAlt7);

    /* PORTA13 (pin 43) is configured as FTM1_QD_PHB */
    PORT_SetPinMux(PORTA, 13U, kPORT_MuxAlt7);

    /* Print a note to terminal */
    PRINTF("\r\nFTM quad decoder example\r\n");

    /* Initialize FTM module */
    FTM_GetDefaultConfig(&ftmInfo);
    ftmInfo.prescale = kFTM_Prescale_Divide_32;
    FTM_Init(DEMO_FTM_BASEADDR, &ftmInfo);

    /* Set the modulo values for Quad Decoder. */
    FTM_SetQuadDecoderModuloValue(DEMO_FTM_BASEADDR, 0U, DEMO_QUAD_DECODER_MODULO);

    /* Enable the Quad Decoder mode. */
    phaseParamsConfigStruct.enablePhaseFilter = true;
    phaseParamsConfigStruct.phaseFilterVal    = 0U;
    phaseParamsConfigStruct.phasePolarity     = kFTM_QuadPhaseNormal;
    FTM_SetupQuadDecode(DEMO_FTM_BASEADDR, &phaseParamsConfigStruct, /* Phase A. */
                        &phaseParamsConfigStruct,                    /* Phase B. */
                        kFTM_QuadPhaseEncode);

}

/*********************************************************
	*
	*Function Name:FTM _Detect_Direction(void)
	*Function :
	*
	*
	*
**********************************************************/
uint8_t FTM_Detect_Direction(void)
{
      /* Read counter value */
	   en_t.encoder_count = FTM_GetQuadDecoderCounterValue(DEMO_FTM_BASEADDR);
	   /* Clear counter */
	   FTM_ClearQuadDecoderCounterValue(DEMO_FTM_BASEADDR);
       PRINTF("Get current counter: %d\r\n", en_t.encoder_count);
	   /* Read direction */
	   if (FTM_GetQuadDecoderFlags(DEMO_FTM_BASEADDR) & kFTM_QuadDecoderCountingIncreaseFlag) //
	   {
		   encoder_direction = true;
		   return 1;
	   }
	   else
	   {
		   encoder_direction = false;
		   return 0;
	   }
}




