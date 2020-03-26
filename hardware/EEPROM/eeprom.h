#ifndef __EEPROM_H__
#define __EEPROM_H__
#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"
#include "MKV46F16.h"

#define I2C_WR	0		/* write  bit */
#define I2C_RD	1		/* read bit */


#define PORT_I2C_SCL	GPIOC			/* GPIO¶Ë¿Ú */
#define PIN_I2C_SCL		5U		/* GPIOÒý½Å */

#define PORT_I2C_SDA	GPIOC			/* GPIO¶Ë¿Ú */
#define PIN_I2C_SDA		4U	/* GPIOÒý½Å */

#define I2C_SCL_PIN		5U			/* Á¬½Óµ½SCLÊ±ÖÓÏßµÄGPIO */
#define I2C_SDA_PIN		4U
/* ¶¨Òå¶ÁÐ´SCLºÍSDAµÄºê */
#define I2C_SCL_1()  PORT_I2C_SCL->PCOR = 1 << I2C_SCL_PIN			/* SCL = 1 */
#define I2C_SCL_0()  PORT_I2C_SCL->PSOR = 1 << I2C_SCL_PIN			/* SCL = 0 */

#define I2C_SDA_1()  PORT_I2C_SDA->PSOR = I2C_SDA_PIN				/* SDA = 1 */
#define I2C_SDA_0()  PORT_I2C_SDA->PCOR = I2C_SDA_PIN				/* SDA = 0 */

#define I2C_SDA_READ()  ((((PORT_I2C_SDA->PDIR) >> I2C_SDA_PIN) & 0x01U)!=0)	/* Read SDA */
#define I2C_SCL_READ()  ((((PORT_I2C_SCL->PDIR) >> I2C_SCL_PIN) & 0x01U)!=0)	/* Read SCL */

#define AT24C02


#ifdef AT24C02
	#define EE_MODEL_NAME		"AT24C02"
	#define EE_DEV_ADDR			0xA0		/* Éè±¸µØÖ· */
	#define EE_PAGE_SIZE		8			/* Ò³Ãæ´óÐ¡(×Ö½Ú) */
	#define EE_SIZE				256			/* ×ÜÈÝÁ¿(×Ö½Ú) */
	#define EE_ADDR_BYTES		1			/* µØÖ·×Ö½Ú¸öÊý */
#endif





                          
void I2C_Init(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);

uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);



#endif 
