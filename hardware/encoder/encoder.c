#include "encoder.h"

volatile uint32_t encoder_count         = 0U;
volatile uint8_t dir_when_overflow      = 0U;
volatile uint8_t counter_overflow_flag  = 0U;
volatile uint8_t counter_overflow_count = 0U;
volatile uint32_t loop_counter          = 0U;
volatile bool encoder_direction         = false;
volatile bool gQdFreshReady             = false;

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









