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
  int32_t Horizon_Position;
  int32_t Vertical_Position;
  int8_t  End_H_flag;
  int8_t  End_V_flag;
  uint16_t eInit_n;   //初始化起始位置和终点位置的次数
  int16_t Horizon_HALL_Pulse;  //水平位置hall 方向判断
  int16_t Vertical_HALL_Pulse; //垂直位置hall 方向判断
  uint8_t HorVer_R_times;  //水平判断hall ，方向次数
  
  uint8_t First_H_dec;    //第一次水平检测的标志位。
  uint8_t First_V_dec;
  uint8_t HorizonStop_flag; /* 水平停止标志位 */
  uint8_t Stop_flag;
  uint8_t Idrun_times;
  uint32_t VH_Total_Dis;      /*绝对误差值*/
  uint8_t VerToHor_Position ; /*垂直位置的水平位置第二次判断*/
  uint8_t HorToVer_Position;  /*水平位置的垂直位置第二判断*/
  volatile int32_t encoder_count ;

 }encoder_t;

extern encoder_t en_t;


#define DEMO_ENC_BASEADDR ENC


/* Get source clock for PIT driver */
#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)

/* The Flextimer instance/channel used for board */
#define DEMO_FTM_BASEADDR FTM1

/* Get source clock for FTM driver */
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)

#define DEMO_QUAD_DECODER_MODULO 65535U//2000U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/



void FTM_Quadrature_Init(void);
uint8_t FTM_Detect_Direction(void);





#endif 