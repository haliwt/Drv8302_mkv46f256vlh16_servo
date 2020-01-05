#ifndef __HALL_H__
#define __HALL_H__

#include "fsl_gpio.h"
#include "fsl_port.h"

#define ENCODER_A_GPIO  GPIOC //PTC1
#define ENCODER_B_GPIO  GPIOC //PTC2
#define ENCODER_Z_GPIO  GPIOC //PTC3

#define ENCODER_A_GPIO_PIN	1
#define ENCODER_B_GPIO_PIN	2
#define ENCODER_Z_GPIO_PIN	3

#define EN_GATE_GPIO GPIOE 
#define EN_GATE_GPIO_PIN   18

/* ��������ṹ�� ------------------------------------------------------------------*/
typedef struct {
  __IO int32_t  uvwStep ;  	      // �����ת״̬
  __IO int32_t  uvwDir ;          // �����ת����
  __IO int32_t  Position;		  // ���λ��
  __IO int32_t  motorSpeed;		  // �����ת�ٶ�
  __IO uint16_t motorPWM_Duty; 	  // ���ռ�ձ�
  __IO int32_t  Hall_Period;      // ������������������
  __IO int32_t  Encode_PulNum;      //������������
  __IO int32_t  Lock_Time;        // �����תʱ��
}BLDC_Typedef;

/* PID�����ṹ�� --------------------------------------------------------------*/
typedef struct 
{
  __IO int32_t  SetPoint;                                 //����Ŀ��ֵDesired Value
  __IO int32_t  SetAngle;								  //����Ŀ��ֵ(�Ƕ�)
  __IO long     SumError;                                 //����ۻ�
  __IO float    Proportion;                               //���г��� Proportional Const
  __IO float    Integral;                                 //���ֳ��� Integral Const
  __IO float    Derivative;                               //΢�ֳ��� Derivative Const
  __IO int      LastError;                                //Error[-1]
  __IO int      PrevError;                                //Error[-2]
}PID_TypeDef;


#define  P_DATA      0.85f                                // P����
#define  I_DATA      0.055f                                // I����
#define  D_DATA      0.558f                                 // D����

#define P_ANGLE_DATA        0.35f   		//��������
#define I_ANGLE_DATA		0.002f
#define D_ANGLE_DATA		0.35f

#define abs(x)    ((x)>0?(x):-(x))


/* ��չ���� ------------------------------------------------------------------*/


extern BLDC_Typedef BLDCMotor ;
extern PID_TypeDef  sPID;



void ENCODER_Init(void);
void IncPIDInit(void) ;    //PID ��ʼ������

int32_t LocPIDCalc(int32_t NextPoint);  //λ��PID����
int32_t AanglePIDCalc(int32_t NextAngle); //�Ƕ�PID����

void DRV8302_Control_Init(void);




#endif 
