/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v3.0
processor: MKV46F256xxx16
package_id: MKV46F256VLL16
mcu_data: ksdk2_0
processor_version: 0.0.8
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"
#include "fsl_xbara.h"




/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '1', peripheral: UART1, signal: TX, pin_signal: ADCB_CH6f/PTE0/UART1_TX/XBAROUT10/XBARIN11}
  - {pin_num: '2', peripheral: UART1, signal: RX, pin_signal: ADCB_CH7f/PTE1/LLWU_P0/UART1_RX/XBAROUT11/XBARIN7}
  - {pin_num: '93', peripheral: PWMA, signal: 'A, 0', pin_signal: PTD0/LLWU_P12/SPI0_PCS0/FTM3_CH0/FTM0_CH0/FlexPWM_A0}
  - {pin_num: '95', peripheral: PWMA, signal: 'A, 1', pin_signal: PTD2/LLWU_P13/SPI0_SOUT/FTM3_CH2/FTM0_CH2/FlexPWM_A1/I2C0_SCL}
  - {pin_num: '97', peripheral: PWMA, signal: 'A, 2', pin_signal: PTD4/LLWU_P14/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/FlexPWM_A2/EWM_IN/SPI0_PCS0}
  - {pin_num: '94', peripheral: PWMA, signal: 'B, 0', pin_signal: ADCA_CH7f/PTD1/SPI0_SCK/FTM3_CH1/FTM0_CH1/FlexPWM_B0}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void)
{
    /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortD);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);
	/* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PORTD0 (pin 93) is configured as FlexPWM_A0 */
    PORT_SetPinMux(PORTD, 0U, kPORT_MuxAlt6);

    /* PORTD1 (pin 94) is configured as FlexPWM_B0 */
    PORT_SetPinMux(PORTD, 1U, kPORT_MuxAlt6);

    /* PORTD2 (pin 59) is configured as FlexPWM_A1 */
    PORT_SetPinMux(PORTD, 2U, kPORT_MuxAlt6);

	/* PORTD3 (pin 60) is configured as FlexPWM_B1 */
	PORT_SetPinMux(PORTD, 3U, kPORT_MuxAlt6);

    /* PORTD4 (pin 61) is configured as FlexPWM_A2 */
    PORT_SetPinMux(PORTD, 4U, kPORT_MuxAlt5);

	/* PORTD5 (pin 62) is configured as FlexPWM_B2 */
	 PORT_SetPinMux(PORTD, 5U, kPORT_MuxAlt5);

     /*解码芯片脚位配置*/
	 /* PORTC1 (pin 71) is configured as XBARIN11 -ENC */
    PORT_SetPinMux(PORTC, 1U, kPORT_MuxAlt6);  // PTC1 -> XB_IN11 -> XBARA_OUT44 -> ENC_PHA

    /* PORTC2 (pin 72) is configured as XBARIN6 */
    PORT_SetPinMux(PORTC, 2U, kPORT_MuxAlt6);  // PTC2 -> XB_IN6  -> XBARA_OUT45 -> ENC_PHB

    /* PORTC6 (pin 78) is configured as XBARIN3 */
    PORT_SetPinMux(PORTC, 6U, kPORT_MuxAlt4);  // PTC6 -> XB_IN3  -> XBARA_OUT46 -> ENC_INDEX
						                      // XBARA_OUT47 -> ENC_HOME
						                      // XBARA_OUT48 -> ENC_TRIGGER(Capture)
    /*USART of PORT*/
    /* PORTE0 (pin 1) is configured as UART1_TX */
    PORT_SetPinMux(PORTE, 0U, kPORT_MuxAlt3);

    /* PORTE1 (pin 2) is configured as UART1_RX */
    PORT_SetPinMux(PORTE, 1U, kPORT_MuxAlt3);



    #if 0  //WT.EDIT 20191105
    SIM->ADCOPT = ((SIM->ADCOPT &
                    /* Mask bits to zero which are setting */
                    (~(SIM_ADCOPT_ADCACH6SEL_MASK | SIM_ADCOPT_ADCACH7SEL_MASK)))

                   /* ADCB MUX1 selection for ADCB channel 6: ADCB MUX0's channel a. */
                   | SIM_ADCOPT_ADCACH6SEL(ADCOPT_ADCACH6SEL_CH_C)

                   /* ADCB MUX1 selection for ADCB channel 7: ADCB MUX1's channel e. */
                   | SIM_ADCOPT_ADCACH7SEL(ADCOPT_ADCACH7SEL_CH_C));
#endif

	/*配置串口通信UART0_TX,UART0_RX*/
    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART1TXSRC_MASK | SIM_SOPT5_UART1RXSRC_MASK)))

                  /* UART 1 transmit data source select: UART1_TX pin. */
                  | SIM_SOPT5_UART1TXSRC(SOPT5_UART1TXSRC_UART_TX)

                  /* UART 1 receive data source select: UART1_RX pin. */
                  | SIM_SOPT5_UART1RXSRC(SOPT5_UART1RXSRC_UART_RX));

	  /* XBARIN6 input pin output assigned to XBARA_IN6 input is connected
     * to XBARA_OUT44 output assigned to ENC0 quadrature waveform phase A */
    XBARA_SetSignalsConnection(XBARA, kXBARA_InputXbarIn6, kXBARA_OutputEnc0PhA);
    /* XBARIN11 input pin output assigned to XBARA_IN11 input is connected
     * to XBARA_OUT45 output assigned to ENC0 quadrature waveform phase B */
    XBARA_SetSignalsConnection(XBARA, kXBARA_InputXbarIn11, kXBARA_OutputEnc0PhB);
    /* XBARIN3 input pin output assigned to XBARA_IN3 input is connected*/
    /**< XBARA_OUT46 output assigned to ENC0 refresh/reload */
    XBARA_SetSignalsConnection(XBARA, kXBARA_InputXbarIn3, kXBARA_OutputEnc0Index);
	 /**< XBARA_OUT47 output assigned to ENC0 home position */
  //  XBARA_SetSignalsConnection(XBARA, kXBARA_InputXbarIn3, kXBARA_OutputEnc0Home);//edit 2020-02-24

                  
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
