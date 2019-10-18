#ifndef __ADC_H__
#define __ADC_H__


#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_cadc.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CADC_BASEADDR ADC

#define CADC_CHANNEL1_NUMBER 2U
#define CADC_CHANNEL1_ENABLE_DIFF    false//true ADCA_CH2 -9PIN

#define CADC_CHANNEL2_NUMBER 3U
#define CADC_CHANNEL2_ENABLE_DIFF   false //true  ADCA_CH3 -10PIN

#define CADC_CHANNEL3_NUMBER 12U
#define CADC_CHANNEL3_ENABLE_DIFF   false //true

#define CADC_CHANNEL4_NUMBER 14U
#define CADC_CHANNEL4_ENABLE_DIFF   false //true

void ADC_CADC_Init(void);








#endif 

