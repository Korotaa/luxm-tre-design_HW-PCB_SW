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

#include "lmic.h"
#include "eid430.h"

#include "Gestion_TOR.h"
#include "Gestion_Lum.h"

#include "lorawan_cnf.h"
#include "lorawan.h"

//Timing d'emmission (en ms / Typique 2000 / valeur min 500)
uint16_t Interval_Env=2000;

//Variables texte
static uint8_t mydata[] = "korota";

void SendDataLUM()
{

    printf("\r\n\r\n     ========= ENVOIE DES DATA SUR LORAWAN  =========     \r\n\r\n");
    gpio_set_level(LED0_IO,1);
    
    if (send_lum==true)
        {
        printf("Send Data LUM...\r\n");
        LMIC_setTxData2(LORAWAN_PORT_Lum,&lum_current,sizeof(lum_current),0);
        send_lum=false;
        }else
          {
          printf("Envoie texte...\r\n");
          LMIC_setTxData2(LORAWAN_PORT_ChaineCarac,mydata, sizeof(mydata)-1,0);
          }

    gpio_set_level(LED1_IO,0);
    printf("     ===============================================     \r\n\r\n");
    vTaskDelay(2000 / portTICK_RATE_MS);

}


void app_main(void)
{
    //Config led 
    printf("Init LED0...\r\n");  
    gpio_pad_select_gpio(LED0_IO);
    gpio_set_direction(LED0_IO,GPIO_MODE_DEF_OUTPUT);
    gpio_set_level(LED0_IO,0);
    
    // Fonction qui initialise la couche Lora et crée la tache principale de l'ordonanceur os_runloop
    InitLoRaWAN(SendDataLUM);
    //InitI2c();  // A ajouter ssi InitLoRaWAN(..) n'est pas actif
    vTaskDelay(500/ portTICK_PERIOD_MS); //attente
    
    // Fonction qui initialise crée la tache qui va récupérer la valeur du Lumetre
    InitGestionLum();
}
