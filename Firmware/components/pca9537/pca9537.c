/* ========================================================================== */
/*                                                                            */
/*   pca9537.c                                                               */
/*   (c) 2018 didalab                                                        */
/*                                                                            */
/*   Description                                                              */
/*   library PCA9537 I2c I/O                                                  */
/* ========================================================================== */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include <string.h>
#include "eid430.h"


#define INIT_I2C
#include "eid430_i2c.h"


#define PCA_REG_DIR   0x03




void Write_Port(uint8_t p)
{
int ret;
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
i2c_master_write_byte(cmd, I2C_PCA_ADR << 1 | WRITE_BIT, ACK_CHECK_EN);
i2c_master_write_byte(cmd, 0x01, ACK_CHECK_EN);
i2c_master_write_byte(cmd, p, ACK_CHECK_EN);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret!=ESP_OK)
  printf("ret_pca_w=%x\r\n",ret);
}

uint8_t Read_Port()
{
int ret;
uint8_t p=0;
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
i2c_master_write_byte(cmd, I2C_PCA_ADR << 1 | WRITE_BIT, ACK_CHECK_EN);
i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret!=ESP_OK)
  {
  printf("ret_pca_w=%x\r\n",ret);
  return p;
  }
vTaskDelay(30 / portTICK_RATE_MS);
cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
i2c_master_write_byte(cmd, I2C_PCA_ADR << 1 | READ_BIT, ACK_CHECK_EN);
i2c_master_read_byte(cmd, &p, ACK_VAL);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret!=ESP_OK)
  printf("i2c_read pca ret=%x\r\n",ret);
return p;
}

void InitPCA(uint8_t p)
{
int ret;
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
#ifdef INIT_I2C
  InitI2c();
  vTaskDelay(30 / portTICK_RATE_MS);
#endif
printf("Init PCA9537...\r\n");
i2c_master_start(cmd);
i2c_master_write_byte(cmd, I2C_PCA_ADR << 1 | WRITE_BIT, ACK_CHECK_EN);
i2c_master_write_byte(cmd, PCA_REG_DIR, ACK_CHECK_EN);
i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret != ESP_OK)
  printf("pca init error ret_i2c=%x\r\n",ret);
  else
  {
  Write_Port(p);
  }
}

void SetIoPca(uint n,bool etat)
{
uint8_t p=1<<n;
uint8_t e_p;
e_p=Read_Port();
if (etat==true)
  {
  if ((e_p&p)==0)
    {
    e_p|=p;
    Write_Port(e_p);
    }
  }
  else
  {
  if ((e_p&p)!=0)  
    {
    e_p^=p;
    Write_Port(e_p);
    }
  }
}
