#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_xbara.h"
#include "fsl_enc.h"
#include "pin_mux.h"
#include "fsl_ftm.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

typedef struct _encoder_t_
{
  volatile int16_t  en_add_value;
  volatile int16_t  en_reduce_value;
  volatile int16_t  capture_width;

  volatile int16_t captureVal_1;
  volatile int16_t captureVal_2;

   uint16_t drv8302_adc_value;

   uint32_t mCurPosValue;
   uint16_t mCurVelValue;
   uint16_t mCurRevValue;
  

  

  uint8_t arr[2];

  uint8_t firstPowerOn;

  uint32_t PulseWidth;

  uint8_t L_flag;
  uint8_t R_flag;
  uint8_t en_interrupt_flag;
  uint8_t setStop_flag;

 }encoder_t;

#define abs(x)    ((x)>0?(x):-(x))

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_ENC_BASEADDR ENC


/* The Flextimer instance/channel used for board */
#define DEMO_FTM_BASEADDR FTM3

/* FTM channel pair used for the dual-edge capture, channel pair 2 uses channels 4 and 5 */
#define BOARD_FTM_INPUT_CAPTURE_CHANNEL_PAIR kFTM_Chnl_3    //kFTM_Chnl_2

/* Interrupt number and interrupt handler for the FTM instance used */
#define FTM_INTERRUPT_NUMBER FTM3_IRQn
#define FTM_INPUT_CAPTURE_HANDLER FTM3_IRQHandler

/* Interrupt to enable and flag to read; depends on the FTM channel used for dual-edge capture */
#define FTM_FIRST_CHANNEL_INTERRUPT_ENABLE      kFTM_Chnl6InterruptEnable       //kFTM_Chnl4InterruptEnable
#define FTM_FIRST_CHANNEL_FLAG              kFTM_Chnl6Flag   //kFTM_Chnl4Flag
#define FTM_SECOND_CHANNEL_INTERRUPT_ENABLE      kFTM_Chnl7InterruptEnable                 //kFTM_Chnl5InterruptEnable
#define FTM_SECOND_CHANNEL_FLAG      kFTM_Chnl7Flag           //kFTM_Chnl5Flag

/* Get source clock for FTM driver */
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_FastPeriphClk)

extern int16_t setHome;
extern int16_t setEnd;


  

void Capture_Input_Init(void);
uint16_t Capture_ReadPulse_Value(void);
void ENC_SetPositionZero(ENC_Type *base, uint32_t value);




#endif 