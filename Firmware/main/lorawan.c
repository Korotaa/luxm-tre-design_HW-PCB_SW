#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include "soc/gpio_struct.h"
#include "driver/gpio.h"

#include "lmic.h"
#include "lorawan_cnf.h"
#include "ttn.h"
#include "lorawan.h"
#include "eid430.h"


send_data fct_send;

void RevBytes(u1_t* b, size_t c)
{
  u1_t i;
  for (i = 0; i < c / 2; i++)
  {
    unsigned char t = b[i];
    b[i] = b[c - 1 - i];
    b[c - 1 - i] = t;
  }
  if (c & 1)
    b[c / 2] = b[c / 2];
}

void os_getArtEui (u1_t* buf) 
{ 

memcpy(buf, APPEUI, 8);
RevBytes(buf, 8);
}

void os_getDevEui (u1_t* buf) 
{ 
memcpy(buf, DEVEUI, 8);
RevBytes(buf, 8);
}

void os_getDevKey (u1_t* buf) { }

void onEvent (ev_t ev) {
    printf("ONEVENT\r\n");
    printf("%d", os_getTime());
    printf(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            printf("EV_SCAN_TIMEOUT");
            break;
        case EV_BEACON_FOUND:
            printf("EV_BEACON_FOUND");
            break;
        case EV_BEACON_MISSED:
            printf("EV_BEACON_MISSED");
            break;
        case EV_BEACON_TRACKED:
            printf("EV_BEACON_TRACKED");
            break;
        case EV_JOINING:
            printf("EV_JOINING");
            break;
        case EV_JOINED:
            printf("EV_JOINED");
            break;
        case EV_RFU1:
            printf("EV_RFU1");
            break;
        case EV_JOIN_FAILED:
            printf("EV_JOIN_FAILED");
            break;
        case EV_REJOIN_FAILED:
            printf("EV_REJOIN_FAILED");
            break;
        case EV_TXCOMPLETE:
            printf("EV_TXCOMPLETE (includes waiting for RX windows)");
            if (LMIC.txrxFlags & TXRX_ACK)
              printf("Received ack");
            if (LMIC.dataLen) {
              printf("Received %d bytes of payload",LMIC.dataLen);
              //printf(LMIC.dataLen);
              //printf(" bytes of payload");
            }

            if (LMIC.opmode & OP_TXRXPEND) {
                printf("OP_TXRXPEND, not sending");
            } else { 
                // Prepare upstream data transmission at the next possible time.
                gpio_set_level(LED0_IO,1);
                if (fct_send!=NULL)
                    fct_send();
                //SendData();

                printf("Packet queued");
                gpio_set_level(LED0_IO,0);
            }
			printf("\r\n");
            //os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            printf("EV_LOST_TSYNC");
            break;
        case EV_RESET:
            printf("EV_RESET\r\n");
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            printf("EV_RXCOMPLETE\r\n");
            break;
        case EV_LINK_DEAD:
            printf("EV_LINK_DEAD\r\n");
            break;
        case EV_LINK_ALIVE:
            printf("EV_LINK_ALIVE\r\n");
            break;
        case EV_TXSTART:
            printf("EV_TXSTART\r\n");
            break;
         default:
            printf("Unknown event: %d\r\n", ev);
            break;
    }
}

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}
// cette tache gère l'appel en boucle infinie à os_run() qui est l'ordonanceur multitache
void os_runloop(void) {

  if (LMIC.opmode & OP_TXRXPEND) {
      printf("OP_TXRXPEND, not sending");
  } else {
      if (fct_send!=NULL)
        fct_send();
      // Prepare upstream data transmission at the next possible time.
     // LMIC_setTxData2(9, mydata, sizeof(mydata)-1, 0);
     // printf("Packet queued");
  }


  while(1) {
    os_run();
	//vTaskDelay(pdMS_TO_TICKS(2000));
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}


void InitLoRaWAN(send_data f_send)
{
    printf("Init LoraWAN...\r\n");
    fct_send=f_send;
    printf("Fct send=%x\r\n",(int)fct_send);
    
    
    os_init();
    printf("LMIC RESET\r\n");
    LMIC_reset();

    uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);

  LMIC_setupChannel(0, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(1, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
  LMIC_setupChannel(2, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(3, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(4, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(5, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band                                              
  LMIC_setupChannel(6, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
  LMIC_setupChannel(7, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK, DR_FSK), BAND_MILLI); 

  LMIC_setLinkCheckMode(0);  // choisir le canal (0-7)
  LMIC.dn2Dr = DR_SF9;
  LMIC_setDrTxpow(DR_SF7,14); //14);

   for(int i = 1; i <= 8; i++) LMIC_disableChannel(i);

  xTaskCreate(os_runloop, "os_runloop", 1024 * 2, (void* )0, 10, NULL);
}