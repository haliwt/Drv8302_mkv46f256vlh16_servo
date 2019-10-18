#ifndef __INPUT_H_
#define __INPUT_H_

#include "fsl_gpio.h"
#include "fsl_port.h"
#include "clock_config.h"
#include "fsl_llwu.h"

#define SD315AI_SO1_A_INPUT    PEin(19)
#define SD315AI_SO2_A_INPUT    PEin(18)

#define SD315AI_SO1_B_INPUT    PEin(17)
#define SD315AI_SO2_B_INPUT    PCin(9)

#define SD315AI_SO1_C_INPUT    PCin(10)
#define SD315AI_SO2_C_INPUT    PCin(11)


#define SD315AI_SO1_A_GPIO    GPIOE
#define SD315AI_SO2_A_GPIO    GPIOE

#define SD315AI_SO1_B_GPIO    GPIOE
#define SD315AI_SO2_B_GPIO    GPIOC

#define SD315AI_SO1_C_GPIO    GPIOC
#define SD315AI_SO2_C_GPIO    GPIOC

#define SD315AI_SO1_A_PIN     19
#define SD315AI_SO2_A_PIN     18

#define SD315AI_SO1_B_PIN     17
#define SD315AI_SO2_B_PIN     9

#define SD315AI_SO1_C_PIN     10
#define SD315AI_SO2_C_PIN     11



void SD315AI_SO12_Input_Init(void);




#endif 