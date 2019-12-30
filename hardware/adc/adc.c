#include "adc.h"

/* Initialize CADC */

uint8_t Phase;
//__IO uint16_t  PWM_Duty	; //占空比
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
 volatile bool g_Transfer_Done = false; /* DMA transfer completion flag. */
uint16_t g_cadcSampleDataArray[DEMO_CADC_SAMPLE_COUNT];
uint32_t g_avgADCValue = 0U; /* Average ADC value .*/
edma_handle_t g_EDMA_Handle; /* Edma handler. */
edma_transfer_config_t g_transferConfig;
/*!
 * @brief Initialize the EDMA.
 */
static void EDMA_Configuration(void);

/*!
 * @brief Initialize the DMAMUX.
 */
static void DMAMUX_Configuration(void);

/*!
 * @brief Initialize the CADC.
 */
static void CADC_Configuration(void);

/*!
 * @brief Process ADC values.
 */
static void ProcessSampleData(void);

/*!
 * @brief Callback function for EDMA.
 */
static void Edma_Callback(edma_handle_t *handle, void *userData, bool transferDone, uint32_t tcds);

//static void ADC_Channel_Select(void);


/*******************************************************************************
 * 
 *Function Name: ADC_DMA_Init(void)
 *
 *
 *
 ******************************************************************************/

void ADC_DMA_Init(void)
{

    EDMA_Configuration();   /* Initialize EDMA. */
    DMAMUX_Configuration(); /* Initialize DMAMUX. */
    CADC_Configuration();   /* Initialize CADC. */

    /* Trigger the converter.
     * Trigger converter A would execute the loop sequential conversion when in
     * "kCADC_DualConverterWorkAsLoopSequential" work mode. */
    CADC_DoSoftwareTriggerConverter(DEMO_CADC_BASEADDR, kCADC_ConverterA);

   // PRINTF("Press any key to get user channel's ADC value ...\r\n");



}

/*******************************************************************************
 * 
 *Function Name: ADC_Channel_Select() 
 *
 *
 *
 ******************************************************************************/

uint16_t ADC_DMA_ReadValue(void)
{

       uint16_t adc_value;
       g_Transfer_Done = false;
        while (!g_Transfer_Done)
        {
        }
        ProcessSampleData();
       // PRINTF("ADC value: %d\r\n", g_avgADCValue >> ADC_RSLT_RSLT_SHIFT);
        PRINTF("ADC value: %d\r\n", ((g_avgADCValue >> ADC_RSLT_RSLT_SHIFT)*100)/4090);
       adc_value =(uint16_t) ((g_avgADCValue >> ADC_RSLT_RSLT_SHIFT)*100)/4090;
       return adc_value ;
       


}
/*********************************************************************************************************************/
static void DMAMUX_Configuration(void)
{
    /* Configure DMAMUX */
    DMAMUX_Init(DEMO_DMAMUX_BASEADDR);
    DMAMUX_SetSource(DEMO_DMAMUX_BASEADDR, DEMO_DMA_CHANNEL, DEMO_DMA_ADC_SOURCE); /* Map ADC source to channel 0 */
    DMAMUX_EnableChannel(DEMO_DMAMUX_BASEADDR, DEMO_DMA_CHANNEL);
}

static void EDMA_Configuration(void)
{
    edma_config_t userConfig;

    EDMA_GetDefaultConfig(&userConfig);
    EDMA_Init(DEMO_DMA_BASEADDR, &userConfig);
    EDMA_CreateHandle(&g_EDMA_Handle, DEMO_DMA_BASEADDR, DEMO_DMA_CHANNEL);
    EDMA_SetCallback(&g_EDMA_Handle, Edma_Callback, NULL);
    EDMA_PrepareTransfer(&g_transferConfig, (void *)CADC_RESULT_REG_ADDR, sizeof(uint16_t),
                         (void *)g_cadcSampleDataArray, sizeof(uint16_t), sizeof(uint16_t),
                         sizeof(g_cadcSampleDataArray), kEDMA_PeripheralToMemory);
    EDMA_SubmitTransfer(&g_EDMA_Handle, &g_transferConfig);
    /* Enable interrupt when transfer is done. */
    EDMA_EnableChannelInterrupts(DEMO_DMA_BASEADDR, DEMO_DMA_CHANNEL, kEDMA_MajorInterruptEnable);
#if defined(FSL_FEATURE_EDMA_ASYNCHRO_REQUEST_CHANNEL_COUNT) && FSL_FEATURE_EDMA_ASYNCHRO_REQUEST_CHANNEL_COUNT
    /* Enable async DMA request. */
    EDMA_EnableAsyncRequest(DEMO_DMA_BASEADDR, DEMO_DMA_CHANNEL, true);
#endif /* FSL_FEATURE_EDMA_ASYNCHRO_REQUEST_CHANNEL_COUNT */
    /* Enable transfer. */
    EDMA_StartTransfer(&g_EDMA_Handle);
}

static void CADC_Configuration(void)
{
    cadc_config_t               cadcConfigStruct;
    cadc_converter_config_t     cadcConverterConfigStruct;
    cadc_sample_config_t        cadcSampleConfigStruct;
    uint16_t sampleMask;

    /*
     * cadcConfigStruct.dualConverterScanMode = kCADC_DualConverterWorkAsTriggeredParallel;
     * cadcConfigStruct.enableSimultaneousMode = true;
     * cadcConfigStruct.DMATriggerSoruce = kCADC_DMATriggerSourceAsEndOfScan;
     * cadcConfigStruct.idleWorkMode = kCADC_IdleKeepNormal;
     * cadcConfigStruct.powerUpDelay = 26U;
     */
    CADC_GetDefaultConfig(&cadcConfigStruct);
    cadcConfigStruct.dualConverterScanMode = kCADC_DualConverterWorkAsLoopSequential;
    cadcConfigStruct.DMATriggerSoruce      = kCADC_DMATriggerSourceAsSampleReady;
    CADC_Init(DEMO_CADC_BASEADDR, &cadcConfigStruct);

    /* Configure converterA. */
    CADC_GetDefaultConverterConfig(&cadcConverterConfigStruct);
    cadcConverterConfigStruct.clockDivisor = 10U;
    CADC_SetConverterConfig(DEMO_CADC_BASEADDR, kCADC_ConverterA, &cadcConverterConfigStruct);
    /* Enable the power for converterA. */
    CADC_EnableConverterPower(DEMO_CADC_BASEADDR, kCADC_ConverterA, true);
    while (kCADC_ConverterAPowerDownFlag == (kCADC_ConverterAPowerDownFlag & CADC_GetStatusFlags(ADC)))
    {
    }
    /* Open the gate for converterA. */
    CADC_EnableConverter(DEMO_CADC_BASEADDR, kCADC_ConverterA, true);

    /* Configure the samples. */
    cadcSampleConfigStruct.channelGain      = kCADC_ChannelGainx1;
    cadcSampleConfigStruct.zeroCrossingMode = kCADC_ZeroCorssingForAnySignChanged;//kCADC_ZeroCorssingDisabled;
    cadcSampleConfigStruct.highLimitValue   = 0xFFFFU;
    cadcSampleConfigStruct.lowLimitValue    = 0x0U;
    cadcSampleConfigStruct.offsetValue      = 0x0U;
    cadcSampleConfigStruct.enableWaitSync   = false;

    /* For converter A. */
    cadcSampleConfigStruct.channelNumber          = DEMO_CADC_USER_CHANNEL;
    cadcSampleConfigStruct.enableDifferentialPair = false;
    CADC_SetSampleConfig(DEMO_CADC_BASEADDR, 0U, &cadcSampleConfigStruct);

    /* Enable the sample slot. */
    sampleMask = CADC_SAMPLE_MASK(0U); /* For converter A. */
    CADC_EnableSample(DEMO_CADC_BASEADDR, sampleMask, true);
    CADC_EnableSample(DEMO_CADC_BASEADDR, (uint16_t)(~sampleMask), false); /* Disable other sample slot. */

    /* Enable the DMA for converterA. */
    CADC_EnableConverterDMA(DEMO_CADC_BASEADDR, kCADC_ConverterA, true);
}

static void ProcessSampleData(void)
{
    uint32_t i = 0U;

    g_avgADCValue = 0;
    /* Get average adc value. */
    for (i = 0; i < DEMO_CADC_SAMPLE_COUNT; i++)
    {
        g_avgADCValue += g_cadcSampleDataArray[i];
    }
    g_avgADCValue = g_avgADCValue / DEMO_CADC_SAMPLE_COUNT;

    /* Reset old value. */
    for (i = 0; i < DEMO_CADC_SAMPLE_COUNT; i++)
    {
        g_cadcSampleDataArray[i] = 0U;
    }
}

static void Edma_Callback(edma_handle_t *handle, void *userData, bool transferDone, uint32_t tcds)
{
    /* Clear Edma interrupt flag. */
    EDMA_ClearChannelStatusFlags(DEMO_DMA_BASEADDR, DEMO_DMA_CHANNEL, kEDMA_InterruptFlag);
    /* Setup transfer */
    EDMA_PrepareTransfer(&g_transferConfig, (void *)CADC_RESULT_REG_ADDR, sizeof(uint16_t),
                         (void *)g_cadcSampleDataArray, sizeof(uint16_t), sizeof(uint16_t),
                         sizeof(g_cadcSampleDataArray), kEDMA_PeripheralToMemory);
    EDMA_SetTransferConfig(DEMO_DMA_BASEADDR, DEMO_DMA_CHANNEL, &g_transferConfig, NULL);
    /* Enable transfer. */
    EDMA_StartTransfer(&g_EDMA_Handle);
    g_Transfer_Done = true;
}

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
    cadcConfigStruct.dualConverterScanMode = kCADC_DualConverterWorkAsLoopParallel;//WT.EDIT kCADC_DualConverterWorkAsTriggeredSequential;
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
    cadcSampleConfigStruct.zeroCrossingMode = kCADC_ZeroCorssingForAnySignChanged;//WT.EDIT 20191116//kCADC_ZeroCorssingDisabled;
    cadcConverterConfigStruct.speedMode=  kCADC_SpeedMode3;

    cadcSampleConfigStruct.highLimitValue   = 0xBB8;//3000//0xFFFFU;
    cadcSampleConfigStruct.lowLimitValue    = 0x0U;
    cadcSampleConfigStruct.offsetValue      = 0x0U;
    cadcSampleConfigStruct.enableWaitSync   = false;

    /* For converter A ADCA_CHC7 12PIN,NO HALL U. */
    cadcSampleConfigStruct.channelNumber          = CADC_CHANNEL1_NUMBER;
    cadcSampleConfigStruct.enableDifferentialPair = CADC_CHANNEL1_ENABLE_DIFF;
    CADC_SetSampleConfig(CADC_BASEADDR, 0U, &cadcSampleConfigStruct);

    /* ADCA_CHC6 11PIN NO HALL V*/
    cadcSampleConfigStruct.channelNumber          = CADC_CHANNEL2_NUMBER;
    cadcSampleConfigStruct.enableDifferentialPair = CADC_CHANNEL2_ENABLE_DIFF;
    CADC_SetSampleConfig(CADC_BASEADDR, 1U, &cadcSampleConfigStruct);
    
    /*ADCA_CH3 10PIN NO HALL W*/
    cadcSampleConfigStruct.channelNumber          = CADC_CHANNEL3_NUMBER;
    cadcSampleConfigStruct.enableDifferentialPair = CADC_CHANNEL3_ENABLE_DIFF;
    CADC_SetSampleConfig(DEMO_CADC_BASEADDR, 2U, &cadcSampleConfigStruct);
    
    /*ADCA_CH2 9PIN PWM_Duty */
    cadcSampleConfigStruct.channelNumber          = CADC_CHANNEL4_NUMBER;
    cadcSampleConfigStruct.enableDifferentialPair = CADC_CHANNEL4_ENABLE_DIFF;
    CADC_SetSampleConfig(DEMO_CADC_BASEADDR, 3U, &cadcSampleConfigStruct);

    /* Enable the sample slot. */
    sampleMask = CADC_SAMPLE_MASK(0U)    /* For converter A. */
                 | CADC_SAMPLE_MASK(1U)  /* For converter A. */
                 | CADC_SAMPLE_MASK(2U) /* For converter A. */
                | CADC_SAMPLE_MASK(3U);  /* For converter A. */
    CADC_EnableSample(CADC_BASEADDR, sampleMask, true);
    CADC_EnableSample(CADC_BASEADDR, (uint16_t)(~sampleMask), false); /* Disable other sample slot. */

    /* The available sample slots are sample 0, 1, 2, 3. */

   // PRINTF("Press any key to trigger the conversion ...\r\n");
   // PRINTF("\r\nSample 0\tSample 1\tSample 2\tSample 3\r\n");


}

/*******************************************************************************
 * 
 *Function Name: void CADC_Read_ADC_Value(void)
 *
 *
 *
 ******************************************************************************/
uint16_t CADC_Read_ADC_Value(void)
{

         
    uint16_t pwm_duty;

        /* Enable the sample slot. */
    pwm_duty = CADC_SAMPLE_MASK(3U) ;   /* For converter A. */
                // | CADC_SAMPLE_MASK(1U)  /* For converter A. */
                // | CADC_SAMPLE_MASK(2U)  /* For converter A. */
                // | CADC_SAMPLE_MASK(3U);  /* For converter A. */
    CADC_EnableSample(CADC_BASEADDR, pwm_duty, true);
    CADC_EnableSample(CADC_BASEADDR, (uint16_t)(~pwm_duty), false); /* Disable other sample slot. */
       CADC_DoSoftwareTriggerConverter(CADC_BASEADDR, kCADC_ConverterA);

	  /* Wait the conversion to be done. */
       while (kCADC_ConverterAEndOfScanFlag !=
               (kCADC_ConverterAEndOfScanFlag & CADC_GetStatusFlags(CADC_BASEADDR)))
        {
        }
       /* Read the result value. */
       // if (pwm_duty == (pwm_duty & CADC_GetSampleReadyStatusFlags(CADC_BASEADDR)))
       // {

         //   PRINTF("PWM_Duty2 = %d\r\n",(uint16_t)((CADC_GetSampleResultValue(CADC_BASEADDR, 0U))/ 330));
          //  PRINTF("PWM_Duty3 = %d\r\n",(uint16_t)((CADC_GetSampleResultValue(CADC_BASEADDR, 1U))/ 330));
           
            
      //  }
        pwm_duty = (uint16_t)((CADC_GetSampleResultValue(CADC_BASEADDR, 3U))/ 330);//ADCA_CH2-9PIN
	    if(pwm_duty > 98)
			pwm_duty = 100;
        
        CADC_ClearStatusFlags(CADC_BASEADDR, kCADC_ConverterAEndOfScanFlag);//kCADC_ConverterAInProgressFlag
      //  CADC_ClearStatusFlags(CADC_BASEADDR, kCADC_ConverterAInProgressFlag);
       
   		return pwm_duty;
     
 }
/*******************************************************************************
 * 
 *Function Name: void ADC_UVW_Sample_HALL_Value(void)
 *Function : no hall element ,sample of Motor of phase_U,V,W.
 *
 *
 ******************************************************************************/
#if 0
void ADC_UVW_Sample_HALL_Value(uint8_t duty)
{

      uint8_t sense = 0,hex,SENSE_H,Phase=0;
      uint8_t sense_u[20], sense_v[20],sense_w[20],i=0,j=0; 
      sense_u[0]= 1;
      sense_v[0]= 0;
      sense_w[0] = 1;
        uint16_t u[5],v[5],w[5];
        uint16_t r=0,y=0,b=0;
        uint16_t sampleMask ;
             /* Enable the sample slot. */
    sampleMask = CADC_SAMPLE_MASK(0U)    /* For converter A. */
                 | CADC_SAMPLE_MASK(1U)  /* For converter A. */
                 | CADC_SAMPLE_MASK(2U) ; /* For converter A. */
              //   | CADC_SAMPLE_MASK(3U);  /* For converter A. */
    CADC_EnableSample(CADC_BASEADDR, sampleMask, true);
    CADC_EnableSample(CADC_BASEADDR, (uint16_t)(~sampleMask), false); /* Disable other sample slot. */
       CADC_DoSoftwareTriggerConverter(CADC_BASEADDR, kCADC_ConverterA);
       

        /* Wait the conversion to be done. */
        while (kCADC_ConverterAEndOfScanFlag !=
               (kCADC_ConverterAEndOfScanFlag & CADC_GetStatusFlags(DEMO_CADC_BASEADDR)))
        {
        }

        /* Read the result value. */
       if (sampleMask == (sampleMask & CADC_GetSampleReadyStatusFlags(DEMO_CADC_BASEADDR)))
        {
           for(j=0;j<5;j++)
           {
             u[j] = (int16_t)CADC_GetSampleResultValue(DEMO_CADC_BASEADDR, 0U); //U --A ADC_CH7C
           
              r=r+ u[j];
          
           
             v[i] = (int16_t)CADC_GetSampleResultValue(DEMO_CADC_BASEADDR, 1U); //V  ---B ADC_CH6C
             y=y+v[j];
        
             
             w[j] = (int16_t)CADC_GetSampleResultValue(DEMO_CADC_BASEADDR, 2U); //W  ---C ADC_CH3
             b=b+w[j];
            
           }
         }
       
                i++;           
            sense_u[i] = r/5 ;
            sense_v[i] = y/5  ;
            sense_w[i] = b/5  ;
			
 
   


       #ifdef DEBUG_PRINT  
            PRINTF("u= %d\r\n", sense_u[i]);
            PRINTF("v= %d\r\n", sense_v[i]);
            PRINTF("w= %d\r\n", sense_w[i]);
         #endif 
     
      
   
      

     // do  //消磁检测事件
      {
         if(SENSE_H) 
            sense = 1; 
         else 
            sense = 0;
         
         switch(Phase)  //{AB,AC,BC,BA,CA,CB} Phase =0 =AB通电
         {
            case 0:  //A+ B- = "2" 换相到 A+ C- = '6'
                     if(sense_w[i-1]> 200)
                     {
                        if(sense_w[i] == 0) //A+ B- 到A+ C- 换向
                        {
                           #if NOHALL
                                 /*  A+ C- = "6" */ 
                                 PWMA_Close_ABC_Channel(1); //close B channel 
                                 /*  Channel configuration A+ */
                                 PWMA_Select_AB_Channel(1,duty);

                           #endif 
                               hex= 0x06;
                              
                              PRINTF("Hex= %d \r\n",hex);
                              Phase++;
                              PRINTF("Phase= %d \r\n",Phase);
                              return hex;     
                        } 
                     }
                  else
                  {
                        #if NOHALL
                        /*A+ B- = '2'*/
                        PWMA_Close_ABC_Channel(2); //close C channel 
                        PWMA_Select_AB_Channel(0,duty);
                      #endif 
                      hex= 0x2;
                    
                     PRINTF("Hex= %d \r\n",hex);
                     return hex;
                  }
                        
                  
            break;
            case 1:   /*  A+ C- = "6"  换成 B+ C-= '4'*/   
                    if(sense_v[i-1]==0)
                    {
                        if(sense_v[i] > 200 ) //A+ C- 换成 B+ C-
                        { 
                           #if NOHALL
                              /*  B+ C- = "4" */ 
                              PWMA_Close_ABC_Channel(0); //close A channel 
                              /*  Channe3 configuration B+  */
                              PWMA_Select_BC_Channel(1,duty);
                           #endif 
                             hex = 0x4;
                       
                            PRINTF("Hex= %d \r\n",hex);
                             Phase++;
                        
                           PRINTF("Phase= %d \r\n",Phase);
                                return hex;
                        
                       
                        }
                    }
                  else
                  {
                        #if NOHALL 
                       /*  A+ C- = "6" */ 
                        PWMA_Close_ABC_Channel(1); //close B channel 
                     /*  Channel configuration A+ */
                        PWMA_Select_AB_Channel(1,duty);
                       #endif 
                        hex = 0x6;
                         PRINTF("Hex= %d \r\n",hex);
                        return hex;   
                  }

                  break;
            case 2:  // B+ C- ='4'换成 B + A- '5'
                      if(sense_u[i-1]> 200)
                      {
                         if(sense_u[i]==0 ) // vSaHall = "A"
                         {
                           
                               #if NOHALL
                                //从B+ C-='4' 换成 B+ A- = '5'
                              /*  B+ A-  = '5' */ 
                              PWMA_Close_ABC_Channel(2);  //close C channel 
                              /*  PWM_A1 and PWM_B1 output */
                              PWMA_Select_BC_Channel(0,duty);
                              #endif 
                             hex = 0x5;
                              PRINTF("Hex= %d \r\n",hex);
                         //    return hex;
                              Phase++;
                             
                              PRINTF("Phase= %d \r\n",Phase);
                              return hex;
                           
                         
                        
                        }
                      }
                     else
                     {
                            #if NOHALL
                            /*B+ C- = '4'*/
                           PWMA_Close_ABC_Channel(0); //close A channel 
                           /*  Channe3 configuration B+  */
                           PWMA_Select_BC_Channel(1,duty);
                           #endif 
                           hex = 0x4;
                            PRINTF("Hex= %d \r\n",hex);
                        
                           return hex;
                     }
                     
                  break;
            case 3:  //B+ A- ='5' 换成 C+ A- = '1'
                  
                  if(sense_w[i-1]==0)
                        if(sense_w[i] >200)
                        {
                           #if NOHALL
                           /*  C+ A- = '1' */       
                           PWMA_Close_ABC_Channel(1); //close B channel 
                           PWMA_Select_CA_Channel(0,duty);
                           #endif 
                            hex = 0x1;
                            PRINTF("Hex= %d \r\n",hex);
                          //   return hex;
                           Phase++;
                    
                        PRINTF("Phase= %d \r\n",Phase);
                          return hex;
                        }
                     else
                     {
                           #if NOHALL 
                          /*  B+ A-  = '5' */ 
                           PWMA_Close_ABC_Channel(2);  //close C channel 
                           /*  PWM_A1 and PWM_B1 output */
                           PWMA_Select_BC_Channel(0,duty);
                           #endif 
                             hex = 0x5;
                              PRINTF("Hex= %d \r\n",hex);
                           return hex;
                     }
                     

                  break;
            case 4:  //C+ A- ='1' 换成 C+ B- = '3'
                    if(sense_v[i-1]>200) 
                    {
                        if(sense_v[i]==0)
                        {
                            #if NOHALL
                          /*  C+ B- = '3' */ 
                           PWMA_Close_ABC_Channel(0); //close A channel 
                           /*  Channe2 configuration */
                           PWMA_Select_CA_Channel(1,duty);
                           #endif 
                           hex = 0x3;
                            PRINTF("Hex= %d \r\n",hex);
                             Phase++;
                          PRINTF("Phase= %d \r\n",Phase);
                           return hex;
                        }
                        
                    } 
                
                    else
                     {
                            #if NOHALL
                          /*  C+ A- = '1' */       
                           PWMA_Close_ABC_Channel(1); //close B channel 
                           PWMA_Select_CA_Channel(0,duty);
                           #endif 
                           hex = 0x1;
                           PRINTF("Hex= %d \r\n",hex);
                           return hex;
                     }
                     
                  break;
            case 5:  //C+ B- ='3' 换成 A+ B-  ='2'
                     
                      if(sense_u[i-1]==0)
                      {
                        if(sense_u[i] > 200)
                        {
                          
                             /* A+ B- ='2' */ 
                            // PWMA_Close_ABC_Channel(2); //close C channel 
                            // PWMA_Select_AB_Channel(0,duty);
                             hex = 0x2;
                              PRINTF("Hex= %d \r\n",hex);
                       
                          Phase = 0;
                         
                          PRINTF("Phase= %d \r\n",Phase);
                          
                     
                          return hex;
                        
                        }
                      }
                     else
                     {
                           #if NOHALL
                         /*C+ B- = '3'*/
                         PWMA_Close_ABC_Channel(0); //close A channel 
                           /*  Channe2 configuration */
                         PWMA_Select_CA_Channel(1,duty);
                         #endif
                          hex = 0x3;
                           PRINTF("Hex= %d \r\n",hex);
                        return hex;
                         
                     }
                      
                  break;
         }
    //   printf("Phase = %d\r \n", Phase);
      }
    //  while(SENSE_H && !sense);  //消磁事件检测 
      

    
           CADC_ClearStatusFlags(DEMO_CADC_BASEADDR, kCADC_ConverterAEndOfScanFlag);
     return hex;
          
}
#endif 
/*************************************************************
    *
    *function : singla dector phase U 
    *
    *
*************************************************************/
void ADC_NO_HALL_Value(uint8_t duty)
 {
    
          static uint8_t Phase ;
          uint8_t sense_u[20], sense_v[20],sense_w[20];
          uint8_t i;
    
         uint16_t sampleMask ;
                 /* Enable the sample slot. */
        sampleMask = CADC_SAMPLE_MASK(0U)    /* For converter A. */
                     | CADC_SAMPLE_MASK(1U)  /* For converter A. */
                     | CADC_SAMPLE_MASK(2U)  /* For converter A. */
                     | CADC_SAMPLE_MASK(3U);  /* For converter A. */
        CADC_EnableSample(CADC_BASEADDR, sampleMask, true);
        CADC_EnableSample(CADC_BASEADDR, (uint16_t)(~sampleMask), false); /* Disable other sample slot. */
           CADC_DoSoftwareTriggerConverter(CADC_BASEADDR, kCADC_ConverterA);
           
    
            /* Wait the conversion to be done. */
            while (kCADC_ConverterAEndOfScanFlag !=
                   (kCADC_ConverterAEndOfScanFlag & CADC_GetStatusFlags(DEMO_CADC_BASEADDR)))
            {
            }
    
            /* Read the result value. */
           if (sampleMask == (sampleMask & CADC_GetSampleReadyStatusFlags(DEMO_CADC_BASEADDR)))
            {
            
                sense_u[i] = (int16_t)CADC_GetSampleResultValue(DEMO_CADC_BASEADDR, 0U); //U --A ADC_CH7C
               
               
              
               
                sense_v[i] = (int16_t)CADC_GetSampleResultValue(DEMO_CADC_BASEADDR, 1U); //V  ---B ADC_CH6C
                
            
                 
                 sense_w[i] = (int16_t)CADC_GetSampleResultValue(DEMO_CADC_BASEADDR, 2U); //W  ---C ADC_CH3
                
                
               
             }

		      CADC_ClearStatusFlags(DEMO_CADC_BASEADDR, kCADC_ConverterAEndOfScanFlag);
                i++; 
                if(i==20)i=0;
     
       
    
    
       #ifdef DEBUG_PRINT  
                PRINTF("u= %d\r\n", sense_u[i]);
                PRINTF("v= %d\r\n", sense_v[i]);
                PRINTF("w= %d\r\n", sense_w[i]);
         #endif 
         
          
       switch(Phase)
       {

           case 0: //AB =2
                       if(sense_w[i]==0) //
                         {
                        //  if(sense_w[i] <50 ) //A+ B- 到A+ C- 换向
                            {
                           #if NOHALL
                                     /*  A+ C- = "6" */ 
                                     PWMA_Close_ABC_Channel(1); //close B channel 
                                     /*  Channel configuration A+ */
                                     PWMA_Select_AB_Channel(1,duty);
    
                           #endif 
                          
                                  Phase++;
                                  PRINTF("Phase= %d \r\n",Phase);
                                    
                            } 
                         }
                      else
                      {
                        #if NOHALL
                            /*A+ B- = '2'*/
                            PWMA_Close_ABC_Channel(2); //close C channel 
                            PWMA_Select_AB_Channel(0,duty);
                      #endif 
                         Phase= 0;
                        
                         PRINTF("Hex= %d \r\n",Phase);
                       
                      }
                      break;
          case 1: //AC=6
        
              if(sense_v[i] >200)
                        {
                           // if(sense_v[i] > 200 ) //A+ C- 换成 B+ C-
                            { 
                           #if NOHALL
                                  /*  B+ C- = "4" */ 
                                  PWMA_Close_ABC_Channel(0); //close A channel 
                                  /*  Channe3 configuration B+  */
                                  PWMA_Select_BC_Channel(1,duty);
                           #endif 
                           
                               Phase++;
                            
                               PRINTF("Phase= %d \r\n",Phase);
                              
                            
                           
                            }
                        }
                      else
                      {
                        #if NOHALL 
                           /*  A+ C- = "6" */ 
                            PWMA_Close_ABC_Channel(1); //close B channel 
                         /*  Channel configuration A+ */
                            PWMA_Select_AB_Channel(1,duty);
                       #endif 
                            Phase=1;
                             PRINTF("Hex= %d \r\n",Phase);
                      
                      }
    
                   break;
          case 2://BC=4
            
                          if(sense_u[i]==0)
                          {
                            // if(sense_u[i]<50 ) // vSaHall = "A"
                             {
                               
                               #if NOHALL
                                    //从B+ C-='4' 换成 B+ A- = '5'
                                  /*  B+ A-  = '5' */ 
                                  PWMA_Close_ABC_Channel(2);  //close C channel 
                                  /*  PWM_A1 and PWM_B1 output */
                                  PWMA_Select_BC_Channel(0,duty);
                              #endif 
                           
                                  Phase++;
                                  
                                  PRINTF("Phase= %d \r\n",Phase);
                               }
                          }
                         else
                         {
                            #if NOHALL
                                /*B+ C- = '4'*/
                               PWMA_Close_ABC_Channel(0); //close A channel 
                               /*  Channe3 configuration B+  */
                               PWMA_Select_BC_Channel(1,duty);
                           #endif 
                               Phase = 2;
                                PRINTF("Hex= %d \r\n",Phase);
                            
                              
                         }
                     break;

              case 3: //BA =5
                        if(sense_w[i]==0)
                          //  if(sense_w[i] >200)
                            {
                           #if NOHALL
                               /*  C+ A- = '1' */       
                               PWMA_Close_ABC_Channel(1); //close B channel 
                               PWMA_Select_CA_Channel(0,duty);
                           #endif 
                             
                               Phase++;
                        
                            PRINTF("Phase= %d \r\n",Phase);
                             
                            }
                         else
                         {
                           #if NOHALL 
                              /*  B+ A-  = '5' */ 
                               PWMA_Close_ABC_Channel(2);  //close C channel 
                               /*  PWM_A1 and PWM_B1 output */
                               PWMA_Select_BC_Channel(0,duty);
                           #endif 
                                 Phase = 3;
                                  PRINTF("Hex= %d \r\n",Phase);
                              
                         }
                         
                break;
                 
                  case 4: //CA=1
                        if(sense_v[i]==0) 
                        {
                          //  if(sense_v[i]<50)
                            {
                            #if NOHALL
                              /*  C+ B- = '3' */ 
                               PWMA_Close_ABC_Channel(0); //close A channel 
                               /*  Channe2 configuration */
                               PWMA_Select_CA_Channel(1,duty);
                           #endif 
                              
                                 Phase++;
                              PRINTF("Phase= %d \r\n",Phase);
                              
                            }
                            
                        } 
                    
                        else
                         {
                            #if NOHALL
                              /*  C+ A- = '1' */       
                               PWMA_Close_ABC_Channel(1); //close B channel 
                               PWMA_Select_CA_Channel(0,duty);
                           #endif 
                               Phase=4;
                               PRINTF("Hex= %d \r\n",Phase);
                               
                         }
                        break; 
                  
               
               case 5 ://CB=3       
                          if(sense_u[i]==0)
                          {
                           // if(sense_u[i] > 200)
                            {
                               #if NOHALL
                               
                                 /* A+ B- ='2' */ 
                                PWMA_Close_ABC_Channel(2); //close C channel 
                                 PWMA_Select_AB_Channel(0,duty);
                               #endif 
                                
                              Phase = 0;
                             
                              PRINTF("Phase= %d \r\n",Phase);
                              
                         
                         
                            
                            }
                          }
                         else
                         {
                           #if NOHALL
                             /*C+ B- = '3'*/
                             PWMA_Close_ABC_Channel(0); //close A channel 
                               /*  Channe2 configuration */
                             PWMA_Select_CA_Channel(1,duty);
                         #endif
                              Phase = 5;
                               PRINTF("Hex= %d \r\n",Phase);
                           
                             
                         }
                          
                 
    
    
       
        // return hex;
       }     
    }

