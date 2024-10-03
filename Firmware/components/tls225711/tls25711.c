

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include <string.h>
#include "eid430.h"

#include "eid430_i2c.h"



void TlsCommandWrite(uint8_t a_tls,uint8_t code_command,uint8_t *buffer,uint8_t l_buf)
{
int ret;
printf("c=%x v=%x\n",code_command,buffer[0]);
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
i2c_master_write_byte(cmd, (a_tls << 1) | WRITE_BIT, ACK_CHECK_EN);
i2c_master_write_byte(cmd, (0x80|code_command), ACK_CHECK_EN);
i2c_master_write(cmd,buffer,l_buf,ACK_CHECK_EN);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret!=ESP_OK)
  printf("ret_tls_w=%x\n",ret);
}

void TlsWriteReg(uint8_t a_tls,uint8_t r_adr,uint8_t reg)
{
int ret;
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
i2c_master_write_byte(cmd, (a_tls << 1) | WRITE_BIT, ACK_CHECK_EN);
i2c_master_write_byte(cmd, (0x80|r_adr), ACK_CHECK_EN);
i2c_master_write_byte(cmd,reg,ACK_CHECK_EN);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret!=ESP_OK)
    {
    int tmp;
    i2c_get_timeout(I2C_MASTER_NUM,&tmp);
  printf("ret_tls_wr=%x t_out=%d\n",ret,tmp);
  }
}

uint8_t TlsReadReg(uint8_t a_tls,uint8_t r_adr)
{
uint8_t r=0;
int ret;
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
i2c_master_write_byte(cmd, (a_tls << 1) | WRITE_BIT, ACK_CHECK_EN);
i2c_master_write_byte(cmd, (0x80|r_adr), ACK_CHECK_EN);
i2c_master_start(cmd);
i2c_master_write_byte(cmd, (a_tls << 1) | READ_BIT, ACK_CHECK_EN);
i2c_master_read_byte(cmd, &r, NACK_VAL);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret!=ESP_OK)
    {
    int tmp;
    i2c_get_timeout(I2C_MASTER_NUM,&tmp);
  printf("i2c_read_reg tls ret=%x t_out=%d\n",ret,tmp);
  }

return r;
}

void TlsRead(uint8_t a_tls,uint8_t *buffer,uint8_t l_buf)
{
int ret;
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
i2c_master_write_byte(cmd, (a_tls << 1) | READ_BIT, ACK_CHECK_EN);
i2c_master_read(cmd,buffer,l_buf,NACK_VAL);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret!=ESP_OK)
  printf("i2c_read tls ret=%x\n",ret);
}

void TlsCommandRead(uint8_t atls,uint8_t code,uint8_t *buffer,uint8_t l_buf)
{
int ret;
//uint8_t i;
//printf("cmd=%x l_r=%x\n",code,l_buf);
uint8_t *p_buf=buffer;
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
i2c_master_write_byte(cmd, (atls << 1) | WRITE_BIT, ACK_CHECK_EN);
i2c_master_write_byte(cmd,(0xA0| code), ACK_CHECK_EN);
i2c_master_start(cmd);
i2c_master_write_byte(cmd, (atls << 1) | READ_BIT, ACK_CHECK_EN);
if (l_buf>1)
  {
  i2c_master_read(cmd,buffer,l_buf-1,ACK_VAL);
  p_buf+=(l_buf-1);
  i2c_master_read_byte(cmd,p_buf,NACK_VAL);    
  }
  else
  {
    i2c_master_read_byte(cmd,p_buf,NACK_VAL);
  }
i2c_master_read(cmd,buffer,l_buf,NACK_VAL);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret!=ESP_OK)
    {
    int tmp;
    i2c_get_timeout(I2C_MASTER_NUM,&tmp);
    printf("i2c_write_command=%x t_out=%d\n",ret,tmp);
    return ;
    }
//printf("cmd=%x l_r=%x\n",code,l_buf);
/*vTaskDelay(30 / portTICK_RATE_MS);
cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
i2c_master_write_byte(cmd, (atls << 1) | READ_BIT, ACK_CHECK_EN);
i2c_master_read(cmd,buffer,l_buf,ACK_VAL);
i2c_master_stop(cmd);
ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
i2c_cmd_link_delete(cmd);
if (ret!=ESP_OK)
    {
    printf("i2c_commandread tls ret=%x\n",ret);
    }*/
}
