/* ========================================================================== */
/*                                                                            */
/*   EID430.c                                                                 */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include <string.h>
#include "eid430.h"

static spi_device_handle_t __spi_e;

#define SPI_BUFFER	32

uint8_t buf_spi_rx[SPI_BUFFER];
uint8_t buf_spi_tx[SPI_BUFFER];

void InitI2c()
{
printf("Init I2c...\r\n");
int i2c_master_port = I2C_MASTER_NUM;
i2c_config_t conf={
   .mode=I2C_MODE_MASTER,
   .sda_io_num=I2C_SDA_IO,
   .scl_io_num=I2C_SCL_IO,
   .sda_pullup_en=GPIO_PULLUP_ENABLE,
   .scl_pullup_en=GPIO_PULLUP_ENABLE,
   .master.clk_speed=I2C_MASTER_FREQ_HZ,
};
//conf.mode = I2C_MODE_MASTER;
//conf.sda_io_num = I2C_SDA_IO;
//conf.sda_pullup_en = GPIO_PULLUP_ENABLE; //GPIO_PULLUP_DISABLE;
//conf.scl_io_num = I2C_SCL_IO;
//conf.scl_pullup_en = GPIO_PULLUP_ENABLE; //GPIO_PULLUP_DISABLE;
//conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

printf("I2C: %d %d %d\r\n",conf.sda_io_num,conf.scl_io_num,conf.master.clk_speed);
i2c_param_config(i2c_master_port, &conf);
i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_DISABLE,
                       I2C_MASTER_TX_BUF_DISABLE, 0);
}

void InitSpiExterne()
{
esp_err_t ret;
printf("Init SPI Externe...\r\n");
gpio_pad_select_gpio(SPI_E_CSN_IO);
gpio_set_direction(SPI_E_CSN_IO, GPIO_MODE_OUTPUT);
spi_bus_config_t bus = {
      .miso_io_num = SPI_E_MISO_IO,
      .mosi_io_num = SPI_E_MOSI_IO,
      .sclk_io_num = SPI_E_SCK_IO,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 0
   };
           
   ret = spi_bus_initialize(HSPI_HOST, &bus, 0);
   assert(ret == ESP_OK);
   printf("spi_e cs=%d clk=%d mosi=%d miso=%d\r\n",SPI_E_CSN_IO,bus.sclk_io_num,bus.mosi_io_num,bus.miso_io_num);
   spi_device_interface_config_t dev = {
      .clock_speed_hz = 900000,
      .mode = 0,
      .spics_io_num = -1,
      .queue_size = 1,
      .flags = 0,
      .pre_cb = NULL
   };
   ret = spi_bus_add_device(HSPI_HOST, &dev, &__spi_e);
   assert(ret == ESP_OK);
   memset(buf_spi_tx,0,SPI_BUFFER);
   memset(buf_spi_rx,0,SPI_BUFFER);
}

void Bme280Write(uint8_t reg,uint8_t val)
{
 uint8_t out[2] = { reg&0x7F, val };
   uint8_t in[2];

   spi_transaction_t t = {
      .flags = 0,
      .length = 8 * sizeof(out),
      .tx_buffer = out,
      .rx_buffer = in  
   };

   gpio_set_level(SPI_E_CSN_IO, 0);
   spi_device_transmit(__spi_e, &t);
   gpio_set_level(SPI_E_CSN_IO, 1);

}

uint8_t Bme280Read(uint8_t reg)
{
uint8_t out[2] = { (reg&0x7f)|0x80, 0xff };
   uint8_t in[2];

   spi_transaction_t t = {
      .flags = 0,
      .length = 8 * sizeof(out),
      .tx_buffer = out,
      .rx_buffer = in
   };

   gpio_set_level(SPI_E_CSN_IO, 0);
   spi_device_transmit(__spi_e, &t);
   gpio_set_level(SPI_E_CSN_IO, 1);
   return in[1];
}

int8_t Bme280WriteBuf(int8_t d,uint8_t reg,uint8_t *buf,uint16_t len)
{
uint16_t i;
 uint8_t out[2] = { 0, 0 };
   uint8_t in[2];

   spi_transaction_t t = {
      .flags = 0,
      .length = 8 * sizeof(out),
      .tx_buffer = out,
      .rx_buffer = in  
   };
   //printf("w_spi r=%x v=%x l=%x\r\n",reg,buf[0],len);
   gpio_set_level(SPI_E_CSN_IO, 0);
for(i=0;i<len;i++)   
{
  out[0]=(reg+i)&0x7f;
  out[1]=buf[i];   
  //printf("out=%x %x\r\n",out[0],out[1]);
  spi_device_transmit(__spi_e, &t);
}
   gpio_set_level(SPI_E_CSN_IO, 1);

return 0;
}


int8_t Bme280ReadBuf(int8_t d,uint8_t reg,uint8_t *buf,uint16_t len)
{
 uint16_t i;
 //uint8_t out[2] = { 0, 0 };
 // uint8_t in[2];
 spi_transaction_t t;
 memset(&t,0,sizeof(t));
 memset(buf_spi_tx,0,SPI_BUFFER);
 memset(buf_spi_rx,0,SPI_BUFFER);
 t.tx_buffer=buf_spi_tx;
 t.rx_buffer=buf_spi_rx;
 t.length=(len+1)*8;
 buf_spi_tx[0]=(reg&0x7f)+0x80;  

 gpio_set_level(SPI_E_CSN_IO, 0);
 spi_device_transmit(__spi_e, &t);
 /*   for(i=0;i<len;i++)
    {
    out[0]=((reg+i)&0x7F)+0x80;
    spi_device_transmit(__spi_e, &t);
    buf[i]=in[1];
    }*/
   gpio_set_level(SPI_E_CSN_IO, 1);
for(i=0;i<len;i++)
	{
	buf[i]=buf_spi_rx[1+i];
	}
return 0;
}
