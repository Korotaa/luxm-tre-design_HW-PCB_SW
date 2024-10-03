#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"


#include "eid430.h"

#include "tls25711.h"

static const char* TAG_LUM ="LUM";

bool send_lum;
float lum_current;


// Définition de la tâche qui sera lancée dans le programme principal
static void taskLum(void *p)
{
int i;
uint8_t r_lux;
uint8_t enable;
uint8_t status=0;
uint8_t data[4];
int c1,c0;
float lum=0,lum1=0,lum2=0;
float cpl=2.72;
cpl/=53;




//user_delay_ms(2000);
r_lux=TlsReadReg(0x39,TLS_ID);
//TlsCommandRead(0x39,TLS_ID,&r_lux,1);
ESP_LOGI(TAG_LUM,"id_lux=%x\r\n",r_lux);
if (r_lux==ID_TSL25711||r_lux==ID_TSL25721)
    {
    if (r_lux==ID_TSL25721)
    {
        ESP_LOGI(TAG_LUM,"TLS25721 detect...\r\n");
        cpl=2.73;
        cpl/=60;
    }
    else
    {
    ESP_LOGI(TAG_LUM,"TLS25711 detect...\r\n");
    }
    printf("TaskLumiere cpl=%f\r\n",cpl);
    TlsWriteReg(0x39,TLS_ENABLE,0x01);
    vTaskDelay(100 / portTICK_RATE_MS);        
    TlsWriteReg(0x39,TLS_ENABLE,0x0B);  
    vTaskDelay(100 / portTICK_RATE_MS);
    TlsWriteReg(0x39,TLS_ATIME,0xff);  //2.72 ms
    vTaskDelay(100 / portTICK_RATE_MS);
    TlsWriteReg(0x39,TLS_WTIME,0xff);  //2.72 ms
    vTaskDelay(100 / portTICK_RATE_MS);
    TlsWriteReg(0x39,TLS_CONTROL,0);   // Gain=1
    vTaskDelay(100 / portTICK_RATE_MS);
    
    /*conf=0x0B;  // WEN AEN PON
    TlsCommandWrite(0x39,TLS_ENABLE,&conf,1);
    user_delay_ms(10);
    conf=0xFF;  // 2.72 ms
    TlsCommandWrite(0x39,TLS_ATIME,&conf,1);
    TlsCommandWrite(0x39,TLS_WTIME,&conf,1);    
    conf=0;    // 1x gain
    TlsCommandWrite(0x39,TLS_CONTROL,&conf,1);*/
    }
    else
    {
        ESP_LOGE(TAG_LUM,"le module ne repond pas!");
    }
    
while(1)
    {
    if (r_lux==ID_TSL25711||r_lux==ID_TSL25721)
        {
        vTaskDelay(2000 / portTICK_RATE_MS);
        status=TlsReadReg(0x39,TLS_STATUS);
        vTaskDelay(100 / portTICK_RATE_MS);
        enable=TlsReadReg(0x39,TLS_ENABLE);
        printf("TLS_STATUS=%x enable=%x\r\n",status,enable&0x1B);
        for(i=0;i<4;i++)
            {
            data[i]=TlsReadReg(0x39,TLS_C0DATA+i);
            vTaskDelay(10 / portTICK_RATE_MS);
            }
        //TlsCommandRead(0x39,TLS_C0DATA,data,4);
        //vTaskDelay(100 / portTICK_RATE_MS);
        printf("data =%x %x %x %x\r\n",data[0],data[1],data[2],data[3]);
        c0=(data[1]<<8)+data[0];
        c1=(data[3]<<8)+data[2];
        if (r_lux==ID_TSL25711)
        {
        lum1=(c0-2.0*c1)/cpl;
        lum2=(0.6*c0-c1)/cpl;   
        }
        else
        if (r_lux==ID_TSL25721)
        {
            lum1=(c0-1.87*c1)/cpl;
            lum2=(0.63*c0-c1)/cpl;
        }
        lum=0.0;
        if (lum1>0&&lum1>lum2)
            lum=lum1;
            else
            if (lum2>0&&lum2>lum1)
                lum=lum2;
        if (send_lum==false)
            {
            lum_current=lum;
            send_lum=true;
            }
        printf("lum=%f  lum1=%f lum2=%f c0=%x c1=%x\r\n",lum,lum1,lum2,c0,c1);                
        }
        else
        {
        ESP_LOGE(TAG_LUM,"Error pas de TSL id=%x\r\n",r_lux);
        vTaskDelay(2000 / portTICK_RATE_MS);
        }
    r_lux=TlsReadReg(0x39,TLS_ID);
    }
    
}

void InitGestionLum()
{
    printf("Init Gestion Luminosité...\r\n");
    send_lum=false;
    lum_current=0;
    printf("Init LED1...\r\n");
    gpio_pad_select_gpio(LED1_IO);
    gpio_set_direction(LED1_IO,GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(LED1_IO,0);
    esp_log_level_set(TAG_LUM,ESP_LOG_INFO);

    //Démarrage de la tache
    xTaskCreate(taskLum,"task_Lum",2048,NULL,5,NULL);
}
