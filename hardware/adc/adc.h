#ifndef __ADC_H__
#define __ADC_H__


#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_cadc.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "bsp_bldc.h"


#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_port.h"
#include "fsl_common.h"



/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CADC_BASEADDR ADC

#define CADC_CHANNEL1_NUMBER 7U     //2U
#define CADC_CHANNEL1_ENABLE_DIFF    false//true  ADCA_CH2 -9PIN

#define CADC_CHANNEL2_NUMBER 6U     //3U
#define CADC_CHANNEL2_ENABLE_DIFF   false //true  ADCA_CH3 -10PIN

#define CADC_CHANNEL3_NUMBER 3U
#define CADC_CHANNEL3_ENABLE_DIFF   false //true

#define CADC_CHANNEL4_NUMBER 2U
#define CADC_CHANNEL4_ENABLE_DIFF   false //true




#define DEMO_CADC_BASEADDR              ADC
#define DEMO_CADC_USER_CHANNEL          7U //wt.edit 4U
#define DEMO_DMAMUX_BASEADDR            DMAMUX0
#define DEMO_DMA_CHANNEL                0U
#define DEMO_DMA_ADC_SOURCE             40U
#define DEMO_DMA_BASEADDR               DMA0
#define CADC_RESULT_REG_ADDR            0x4005c01cU

#define DEMO_CADC_SAMPLE_COUNT 16U /* The cadc sample count. */


void ADC_CADC_Init(void);


uint16_t CADC_Read_ADC_Value(void);


void ADC_DMA_Init(void);

uint16_t ADC_DMA_ReadValue(void);





#endif 

