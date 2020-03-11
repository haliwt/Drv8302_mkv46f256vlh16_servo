#ifndef __OUTPUT_H__
#define __OUTPUT_H__


#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_llwu.h"
#include "input.h"
#include "led.h"


//#define B_POWER_OUTPUT         PCout(11)
//#define C_POWER_OUTPUT         PCout(9)

#define AIR_OUTPUT               PBout(17)   //空调输出 
#define WIPER_OUTPUT_1           PBout(19)   //雨刮器输出一档
#define DOOR_OUTPUT              PAout(19)   //车门输出
#define WHEEL_OUTPUT             PBout(0)    //车轮输出





#define OUTPUT_AIR_GPIO          GPIOB
#define OUTPUT_WIPER_1_GPIO      GPIOB 
#define OUTPUT_DOOR_GPIO         GPIOA 
#define OUTPUT_WHEEL_GPIO        GPIOB



#define OUTPUT_AIR_GPIO_PIN           17
#define OUTPUT_WIPER_1_GPIO_PIN       19
#define OUTPUT_DOOR_GPIO_PIN          19
#define OUTPUT_WHEEL_GPIO_PIN         0







#define DRV8302_EN_GATE_GPIO 		GPIOE 
#define DRV8302_EN_GATE_GPIO_PIN   	18

typedef struct _output_t_
{

   uint8_t motor_run;
   uint8_t power_on ;
   uint8_t abc_numbers;
   uint8_t Dir_flag;
   uint8_t en_on;

  

}output_t;



extern output_t  motor_ref;

void OUTPUT_Fucntion_Init(void);

void WIPER_OUTPUT_F(void);
void DOOR_OUTPUT_F(void);
void WHEEL_OUTPUT_F(void);
void ABC_POWER_OUTPUT_Init(void);
void SD315AI_Check_Fault(void);
void SD315AI_Disable_Output(void);


#endif 