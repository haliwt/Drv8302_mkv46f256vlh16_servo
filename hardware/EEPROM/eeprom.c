#include "eeprom.h"

void I2C_Init(void)
{
  /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    const port_pin_config_t portc14_pin50_config = {/* Internal pull-up 
      resistor is enabled */
                                                    kPORT_PullUp,
                                                    /* Fast slew rate is 
		configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is 
		disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is enabled */
                                                    kPORT_OpenDrainEnable,
                                                    /* Low drive strength is 
		configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as 
		I2C0_SCL */
                                                    kPORT_MuxAsGpio ,
                                                    /* Pin Control Register 
		fields [15:0] are not locked */
                                                    kPORT_UnlockRegister};
    /* PORTC14 (pin 86) is configured as I2C0_SCL */
    PORT_SetPinConfig(PORTC, 5U, &portc14_pin50_config);

    const port_pin_config_t portc15_pin51_config = {/* Internal pull-up 
						resistor is enabled */
                                                    kPORT_PullUp,
                                                    /* Fast slew rate is 
						configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is 
						disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is enabled */
                                                    kPORT_OpenDrainEnable,
                                                    /* Low drive strength is 
						configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as 
						I2C0_SDA */
                                                    kPORT_MuxAsGpio ,
                                                    /* Pin Control Register 
				fields [15:0] are not locked */
                                                    kPORT_UnlockRegister};
    /* PORTC15 (pin 87) is configured as I2C0_SDA */
    PORT_SetPinConfig(PORTC, 4U, &portc15_pin51_config);



}

/*
*********************************************************************************************************
*	Function: i2c_Delay
*	¹¦ÄÜËµÃ÷: I2C×ÜÏßÎ»ÑÓ³Ù£¬×î¿ì400KHz
*	ÐÎ    ²Î:  ÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*¡¡
		CPUÖ÷Æµ168MHzÊ±£¬ÔÚÄÚ²¿FlashÔËÐÐ, MDK¹¤³Ì²»ÓÅ»¯¡£ÓÃÌ¨Ê½Ê¾²¨Æ÷¹Û²â²¨ÐÎ¡£
		Ñ­»·´ÎÊýÎª5Ê±£¬SCLÆµÂÊ = 1.78MHz (¶ÁºÄÊ±: 92ms, ¶ÁÐ´Õý³££¬µ«ÊÇÓÃÊ¾²¨Æ÷Ì½Í·ÅöÉÏ¾Í¶ÁÐ´Ê§°Ü¡£Ê±Ðò½Ó½üÁÙ½ç)
		Ñ­»·´ÎÊýÎª10Ê±£¬SCLÆµÂÊ = 1.1MHz (¶ÁºÄÊ±: 138ms, ¶ÁËÙ¶È: 118724B/s)
		Ñ­»·´ÎÊýÎª30Ê±£¬SCLÆµÂÊ = 440KHz£¬ SCL¸ßµçÆ½Ê±¼ä1.0us£¬SCLµÍµçÆ½Ê±¼ä1.2us

		ÉÏÀ­µç×èÑ¡Ôñ2.2KÅ·Ê±£¬SCLÉÏÉýÑØÊ±¼äÔ¼0.5us£¬Èç¹ûÑ¡4.7KÅ·£¬ÔòÉÏÉýÑØÔ¼1us

		Êµ¼ÊÓ¦ÓÃÑ¡Ôñ400KHz×óÓÒµÄËÙÂÊ¼´¿É
	*/
	for (i = 0; i < 30; i++);
}
/*
*********************************************************************************************************
*	Function Name: i2c_Start
*	¹¦ÄÜËµÃ÷: CPU·¢ÆðI2C×ÜÏßÆô¶¯ÐÅºÅ
*	ÐÎ    ²Î:  ÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* µ±SCL¸ßµçÆ½Ê±£¬SDA³öÏÖÒ»¸öÏÂÌøÑØ±íÊ¾I2C×ÜÏßÆô¶¯ÐÅºÅ */
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();
	
	I2C_SCL_0();
	i2c_Delay();
}

/*
*********************************************************************************************************
*	Function Name: i2c_Stop
*	¹¦ÄÜËµÃ÷: CPU·¢ÆðI2C×ÜÏßÍ£Ö¹ÐÅºÅ
*	ÐÎ    ²Î:  ÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* µ±SCL¸ßµçÆ½Ê±£¬SDA³öÏÖÒ»¸öÉÏÌøÑØ±íÊ¾I2C×ÜÏßÍ£Ö¹ÐÅºÅ */
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
	i2c_Delay();
}

/*
*********************************************************************************************************
*	Function Name: i2c_SendByte
*	¹¦ÄÜËµÃ÷: CPUÏòI2C×ÜÏßÉè±¸·¢ËÍ8bitÊý¾Ý
*	ÐÎ    ²Î:  _ucByte £º µÈ´ý·¢ËÍµÄ×Ö½Ú
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* ÏÈ·¢ËÍ×Ö½ÚµÄ¸ßÎ»bit7 */
	for (i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();
		I2C_SCL_0();
		if (i == 7)
		{
			 I2C_SDA_1(); // ÊÍ·Å×ÜÏß
		}
		_ucByte <<= 1;	/* ×óÒÆÒ»¸öbit */
		i2c_Delay();
	}
}

/*
*********************************************************************************************************
*	Function Name: i2c_ReadByte
*	Function: CPU´ÓI2C×ÜÏßÉè±¸¶ÁÈ¡8bitÊý¾Ý
*	Input Reference:  NO
*	Output Reference: read I2C data
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* ¶Áµ½µÚ1¸öbitÎªÊý¾ÝµÄbit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/*
*********************************************************************************************************
*	Function Name: i2c_WaitAck
*	¹¦ÄÜËµÃ÷: CPU²úÉúÒ»¸öÊ±ÖÓ£¬²¢¶ÁÈ¡Æ÷¼þµÄACKÓ¦´ðÐÅºÅ
*	ÐÎ    ²Î:  ÎÞ
*	·µ »Ø Öµ: ·µ»Ø0±íÊ¾ÕýÈ·Ó¦´ð£¬1±íÊ¾ÎÞÆ÷¼þÏìÓ¦
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	I2C_SDA_1();	/* CPUÊÍ·ÅSDA×ÜÏß */
	i2c_Delay();
	I2C_SCL_1();	/* CPUÇý¶¯SCL = 1, ´ËÊ±Æ÷¼þ»á·µ»ØACKÓ¦´ð */
	i2c_Delay();
	if (I2C_SDA_READ())	/* CPU¶ÁÈ¡SDA¿ÚÏß×´Ì¬ */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_0();
	i2c_Delay();
	return re;
}

/*
*********************************************************************************************************
*	Function Name: i2c_Ack
*	¹¦ÄÜËµÃ÷: CPU²úÉúÒ»¸öACKÐÅºÅ
*	ÐÎ    ²Î:  ÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	I2C_SDA_0();	/* CPUÇý¶¯SDA = 0 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU²úÉú1¸öÊ±ÖÓ */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();	/* CPUÊÍ·ÅSDA×ÜÏß */
}

/*
*********************************************************************************************************
*	Function Name: i2c_NAck
*	¹¦ÄÜËµÃ÷: CPU²úÉú1¸öNACKÐÅºÅ
*	ÐÎ    ²Î:  ÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	I2C_SDA_1();	/* CPUÇý¶¯SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU²úÉú1¸öÊ±ÖÓ */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/*
*********************************************************************************************************
*	Function Name: i2c_CheckDevice
*	¹¦ÄÜËµÃ÷: ¼ì²âI2C×ÜÏßÉè±¸£¬CPUÏò·¢ËÍÉè±¸µØÖ·£¬È»ºó¶ÁÈ¡Éè±¸Ó¦´ðÀ´ÅÐ¶Ï¸ÃÉè±¸ÊÇ·ñ´æÔÚ
*	ÐÎ    ²Î:  _Address£ºÉè±¸µÄI2C×ÜÏßµØÖ·
*	·µ »Ø Öµ: ·µ»ØÖµ 0 ±íÊ¾ÕýÈ·£¬ ·µ»Ø1±íÊ¾Î´Ì½²âµ½
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	if (I2C_SDA_READ() && I2C_SCL_READ())
	{
		i2c_Start();		/* ·¢ËÍÆô¶¯ÐÅºÅ */

		/* ·¢ËÍÉè±¸µØÖ·+¶ÁÐ´¿ØÖÆbit£¨0 = w£¬ 1 = r) bit7 ÏÈ´« */
		i2c_SendByte(_Address | I2C_WR);
		ucAck = i2c_WaitAck();	/* ¼ì²âÉè±¸µÄACKÓ¦´ð */

		i2c_Stop();			/* ·¢ËÍÍ£Ö¹ÐÅºÅ */

		return ucAck;
	}
	return 1;	/* I2C is error  */
}
