#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_xbara.h"
#include "fsl_enc.h"
#include "pin_mux.h"
#include "fsl_ftm.h"
#include "clock_config.h"


typedef struct _encoder_t_
{
  volatile int16_t  en_add_value;
  volatile int16_t  en_reduce_value;
  volatile int16_t  capture_width;

  volatile int16_t captureVal_1;
  volatile int16_t captureVal_2;

  uint32_t mCurPosValue;
  uint16_t mCurVelValue;
  uint16_t mCurRevValue;

}encoder_t;

#define abs(x)    ((x)>0?(x):-(x))

#define DEMO_ENC_BASEADDR ENC
#define DEMO_ENC_INDEX_IRQ_ID ENC_INDEX_IRQn


#define BOARD_FTM_BASEADDR FTM3

/* FTM channel used for input capture */
#define BOARD_FTM_INPUT_CAPTURE_CHANNEL_1  kFTM_Chnl_7//kFTM_Chnl_4

#define BOARD_FTM_INPUT_CAPTURE_CHANNEL_2  kFTM_Chnl_6//kFTM_Chnl_4

/* Interrupt number and interrupt handler for the FTM base address used */
#define FTM_INTERRUPT_NUMBER FTM3_IRQn
#define FTM_INPUT_CAPTURE_HANDLER FTM3_IRQHandler

/* Interrupt to enable and flag to read */
#define FTM_CHANNEL_INTERRUPT_ENABLE_1      kFTM_Chnl7InterruptEnable//kFTM_Chnl4InterruptEnable
#define FTM_CHANNEL_FLAG_1   kFTM_Chnl7Flag  //kFTM_Chnl4Flag

#define FTM_CHANNEL_INTERRUPT_ENABLE_2      kFTM_Chnl6InterruptEnable//kFTM_Chnl4InterruptEnable
#define FTM_CHANNEL_FLAG_2   kFTM_Chnl6Flag  //kFTM_Chnl4Flag




void Capture_Input_Init(void);
void ENC_SetPositionZero(ENC_Type *base, uint32_t value);




#endif 