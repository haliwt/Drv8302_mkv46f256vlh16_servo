#include "adc.h"

/* Initialize CADC */
//void ADC_CADC_Init(void);



/*******************************************************************************
 * 
 *Function Name: ADC_CADC_Init() 
 *
 *
 *
 ******************************************************************************/
void ADC_CADC_Init(void)
{
    
    cadc_config_t cadcConfigStruct;
    cadc_converter_config_t cadcConverterConfigStruct;
    cadc_sample_config_t cadcSampleConfigStruct;
    uint16_t sampleMask;

     /* Initialize the CADC common digital control.
     * "kCADC_DualConverterWorkAsTriggeredSequential" is used in this case. The a long conversion sequence would be
     * executed by both converter. The sequence would be controlled by converter A's control logic, including the
     * trigger input.
     */
    /*
     * cadcConfigStruct.dualConverterScanMode = kCADC_DualConverterWorkAsTriggeredParallel;
     * cadcConfigStruct.enableSimultaneousMode = true;
     * cadcConfigStruct.DMATriggerSoruce = kCADC_DMATriggerSourceAsEndOfScan;
     * cadcConfigStruct.idleWorkMode = kCADC_IdleKeepNormal;
     * cadcConfigStruct.powerUpDelay = 26U;
     */
    CADC_GetDefaultConfig(&cadcConfigStruct);
    cadcConfigStruct.dualConverterScanMode =  kCADC_DualConverterWorkAsLoopSequential;//WT.EDIT kCADC_DualConverterWorkAsTriggeredSequential;
    CADC_Init(CADC_BASEADDR, &cadcConfigStruct);

    /* Configure each converter. */
    CADC_GetDefaultConverterConfig(&cadcConverterConfigStruct);
    CADC_SetConverterConfig(CADC_BASEADDR, kCADC_ConverterA | kCADC_ConverterB, &cadcConverterConfigStruct);
    /* Enable the power for each converter. */
    CADC_EnableConverterPower(CADC_BASEADDR, kCADC_ConverterA | kCADC_ConverterB, true);
    while ((kCADC_ConverterAPowerDownFlag | kCADC_ConverterBPowerDownFlag) ==
           ((kCADC_ConverterAPowerDownFlag | kCADC_ConverterBPowerDownFlag) & CADC_GetStatusFlags(ADC)))
    {
    }
    /* Open the gate for each converter. */
    CADC_EnableConverter(CADC_BASEADDR, kCADC_ConverterA | kCADC_ConverterB, true);

    /* Configure the samples. */
    cadcSampleConfigStruct.channelGain      = kCADC_ChannelGainx1;
    cadcSampleConfigStruct.zeroCrossingMode = kCADC_ZeroCorssingDisabled;
    cadcSampleConfigStruct.highLimitValue   = 0xFFFFU;
    cadcSampleConfigStruct.lowLimitValue    = 0x0U;
    cadcSampleConfigStruct.offsetValue      = 0x0U;
    cadcSampleConfigStruct.enableWaitSync   = false;

    /* For converter A. */
    cadcSampleConfigStruct.channelNumber          = CADC_CHANNEL1_NUMBER;
    cadcSampleConfigStruct.enableDifferentialPair = CADC_CHANNEL1_ENABLE_DIFF;
    CADC_SetSampleConfig(CADC_BASEADDR, 0U, &cadcSampleConfigStruct);
    cadcSampleConfigStruct.channelNumber          = CADC_CHANNEL2_NUMBER;
    cadcSampleConfigStruct.enableDifferentialPair = CADC_CHANNEL2_ENABLE_DIFF;
    CADC_SetSampleConfig(CADC_BASEADDR, 1U, &cadcSampleConfigStruct);
#if 0
    /* For converter B. */
    cadcSampleConfigStruct.channelNumber          = CADC_CHANNEL3_NUMBER;
    cadcSampleConfigStruct.enableDifferentialPair = CADC_CHANNEL3_ENABLE_DIFF;
    CADC_SetSampleConfig(CADC_BASEADDR, 2U, &cadcSampleConfigStruct);
    cadcSampleConfigStruct.channelNumber          = CADC_CHANNEL4_NUMBER;
    cadcSampleConfigStruct.enableDifferentialPair = CADC_CHANNEL4_ENABLE_DIFF;
    CADC_SetSampleConfig(CADC_BASEADDR, 3U, &cadcSampleConfigStruct);
#endif 
    /* Enable the sample slot. */
    sampleMask = CADC_SAMPLE_MASK(0U)    /* For converter A. */
                 | CADC_SAMPLE_MASK(1U);  /* For converter A. */
                // | CADC_SAMPLE_MASK(2U)  /* For converter B. */
                // | CADC_SAMPLE_MASK(3U); /* For converter B. */
    CADC_EnableSample(CADC_BASEADDR, sampleMask, true);
    CADC_EnableSample(CADC_BASEADDR, (uint16_t)(~sampleMask), false); /* Disable other sample slot. */

    /* The available sample slots are sample 0, 1, 2, 3. */

    PRINTF("Press any key to trigger the conversion ...\r\n");
    PRINTF("\r\nSample 0\tSample 1\tSample 2\tSample 3\r\n");


}



