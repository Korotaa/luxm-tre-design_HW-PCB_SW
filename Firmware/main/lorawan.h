#ifndef _LORAWAN_H
#define _LORAWAN_H


typedef void (*send_data)();

void InitLoRaWAN(send_data);

#endif