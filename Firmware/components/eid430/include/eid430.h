#ifndef _EID430_H
#define _EID430_H

#define EID430

// I2c def
#define I2C_SCL_IO  22
#define I2C_SDA_IO  21

#define I2C_PCA_ADR  0x49


#define I2C_MASTER_NUM             I2C_NUM_1        /*!< I2C port number for master dev */
#define I2C_MASTER_RX_BUF_DISABLE  0
#define I2C_MASTER_TX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_RX_BUF_DISABLE  0                /*!< I2C master do not need buffer */

#define I2C_MASTER_FREQ_HZ         100000           /*!< I2C master clock frequency */

#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */

// output 

#define LED0_IO   2
#define LED1_IO   4

// input
#define BP_IO     32

//LORA SX1272

#define SPI_LORA_SCK_IO  14
#define SPI_LORA_MOSI_IO 13
#define SPI_LORA_MISO_IO 12
#define SPI_LORA_CSN_IO  15
#define IRQ_LORA_IO      27

#define LORA_DIO0		 27
#define LORA_DIO1		 16			 

// SPI Externe

#define SPI_E_SCK_IO		18
#define SPI_E_MISO_IO		19
#define SPI_E_MOSI_IO		23
#define SPI_E_CSN_IO		5

//ADC

#define POT_ADC_IO       34

// fonctions

void InitI2c();
void InitSpiExterne();
uint8_t Bme280Read(uint8_t reg);
int8_t Bme280WriteBuf(int8_t d,uint8_t reg,uint8_t *buf,uint16_t len);
int8_t Bme280ReadBuf(int8_t d,uint8_t reg,uint8_t *buf,uint16_t len);

#endif