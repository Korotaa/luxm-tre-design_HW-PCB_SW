#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <soc/gpio_struct.h>
#include <driver/gpio.h>

#include <eid430.h>
#include "Gestion_TOR.h"


#define GPIO_BP_PIN_SEL     (1ULL<<BP_IO)
#define  ESP_INTR_FLAG_DEFAUT   0

static xQueueHandle bp_evt_queue = NULL;

uint32_t cpt_bp;
int etat_bp;
bool Send_TOR;
Data_TOR d_tor;

static void IRAM_ATTR bp_isr_handler(void *arg)
{
    uint32_t bp_num=(uint32_t)arg;
    xQueueSendFromISR(bp_evt_queue,&bp_num,NULL);
}

static void taskBP(void *arg)
{
    uint32_t io_num;
    while(1)
        {
            if (xQueueReceive(bp_evt_queue,&io_num,portMAX_DELAY)) //sur changement d'etat
                { 
                    cpt_bp++;
                    etat_bp=gpio_get_level(io_num);
                    d_tor.etat=etat_bp;
                    d_tor.cpt=cpt_bp;
                    Send_TOR=true;
                    printf("GPIO[%d]=intr, val=%d cpt=%d\r\n",io_num,etat_bp,cpt_bp);
                }
             vTaskDelay(1000 / portTICK_RATE_MS);
        }
}

static void taskLed(void *p)
{
    printf("Gestion Led...\r\n");
    while(1)
    {
        printf("Led1 off\r\n");
        gpio_set_level(LED1_IO,1);
        vTaskDelay(1000 / portTICK_RATE_MS);
        printf("Led1 On\r\n");
        gpio_set_level(LED1_IO,0);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void InitGestionTOR()
{
    gpio_config_t io_conf;
    printf("Init Gestion TOR\r\n");
    printf("Init LED1...\r\n");
    gpio_pad_select_gpio(LED1_IO);
    gpio_set_direction(LED1_IO,GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(LED1_IO,0);

    io_conf.intr_type=GPIO_PIN_INTR_POSEDGE;
    io_conf.pin_bit_mask=GPIO_BP_PIN_SEL;
    io_conf.mode=GPIO_MODE_INPUT;
    io_conf.pull_up_en=0;
    gpio_config(&io_conf);
    //change gpio intrrupt type for one pin
    gpio_set_intr_type(BP_IO, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    bp_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(taskBP, "BP Task", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAUT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(BP_IO, bp_isr_handler, (void*) (BP_IO));
    
    //remove isr handler for gpio number.
    gpio_isr_handler_remove(BP_IO);
    //hook isr handler for specific gpio pin again
    gpio_isr_handler_add(BP_IO,bp_isr_handler, (void*) BP_IO);

    cpt_bp=0;
    etat_bp=0;
    Send_TOR=false;
    memset(&d_tor,0,sizeof(d_tor));

    xTaskCreate(taskLed,"task_led",2048,NULL,5,NULL);
}