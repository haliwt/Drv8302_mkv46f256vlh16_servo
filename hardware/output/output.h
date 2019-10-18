#ifndef __OUTPUT_H__
#define __OUTPUT_H__


#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_llwu.h"

#define A_POWER_OUTPUT           PCout(1)//PCout(10) //2SD315AI ABC��Դ�л��������15V�����Դ
//#define B_POWER_OUTPUT         PCout(11)
//#define C_POWER_OUTPUT         PCout(9)

#define AIR_OUTPUT               PBout(17)   //�յ���� 
#define WIPER_OUTPUT_1           PBout(19)   //��������һ��
#define DOOR_OUTPUT              PAout(19)   //�������
#define WHEEL_OUTPUT             PBout(0)    //�������

#define SD315_VL_A_OUTPUT        PCout(7)//   //2SD315AI -4�ţ�����ģ�飬�͵�ƽ����
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

typedef struct _output_t_
{

  volatile uint8_t dir_change;
  volatile uint8_t break_f;

}output_t;




void OUTPUT_Fucntion_Init(void);
void AIR_OUTPUT_F(void);
void WIPER_OUTPUT_F(void);
void DOOR_OUTPUT_F(void);
void WHEEL_OUTPUT_F(void);
void ABC_POWER_OUTPUT_Init(void);


#endif 