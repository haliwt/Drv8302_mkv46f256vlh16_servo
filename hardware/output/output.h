#ifndef __OUTPUT_H__
#define __OUTPUT_H__


#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_llwu.h"
#include "input.h"
#include "led.h"

#define A_POWER_OUTPUT           PCout(1)//PCout(10) //2SD315AI ABC电源切换输出，打开15V供电电源
//#define B_POWER_OUTPUT         PCout(11)
//#define C_POWER_OUTPUT         PCout(9)

#define AIR_OUTPUT               PBout(17)   //空调输出 
#define WIPER_OUTPUT_1           PBout(19)   //雨刮器输出一档
#define DOOR_OUTPUT              PAout(19)   //车门输出
#define WHEEL_OUTPUT             PBout(0)    //车轮输出

#define SD315_VL_A_OUTPUT        PCout(7)//   //2SD315AI -4脚，保护模块，低电平保护
#define SD315_VL_B_OUTPUT        PCout(6)//
#define SD315_VL_C_OUTPUT        PCout(4)//



#define OUTPUT_AIR_GPIO          GPIOB
#define OUTPUT_WIPER_1_GPIO      GPIOB 
#define OUTPUT_DOOR_GPIO         GPIOA 
#define OUTPUT_WHEEL_GPIO        GPIOB



#define OUTPUT_AIR_GPIO_PIN           17
#define OUTPUT_WIPER_1_GPIO_PIN       19
#define OUTPUT_DOOR_GPIO_PIN          19
#define OUTPUT_WHEEL_GPIO_PIN         0

#define SD315AI_VL_A_GPIO      GPIOC//GPIOE
#define SD315AI_VL_B_GPIO      GPIOC//GPIOE
#define SD315AI_VL_C_GPIO      GPIOC//GPIOE



#define SD315AI_VL_A_PIN      7//19U
#define SD315AI_VL_B_PIN      6//18U
#define SD315AI_VL_C_PIN      4//17U


#define DRV8302_EN_GATE_GPIO 		GPIOE 
#define DRV8302_EN_GATE_GPIO_PIN   	18

typedef struct _output_t_
{

   uint8_t motor_run;
   uint8_t power_on ;
   uint8_t abc_numbers;
   uint8_t Dir_flag;
  

}output_t;




void OUTPUT_Fucntion_Init(void);
void AIR_OUTPUT_F(void);
void WIPER_OUTPUT_F(void);
void DOOR_OUTPUT_F(void);
void WHEEL_OUTPUT_F(void);
void ABC_POWER_OUTPUT_Init(void);
void SD315AI_Check_Fault(void);
void SD315AI_Disable_Output(void);


#endif 