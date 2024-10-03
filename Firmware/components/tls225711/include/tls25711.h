#ifndef __TLS25711_H
#define __TLS25711_H

#define TLS_ENABLE  0
#define TLS_ATIME   1
#define TLS_WTIME   3
#define TLS_AILTL   4
#define TLS_AILTH   5
#define TLS_AIHTL   6
#define TLS_AIHTH   7
#define TLS_PERS    0x0c
#define TLS_CONFIG  0x0d
#define TLS_CONTROL 0x0f
#define TLS_ID      0x12
#define TLS_STATUS  0x13
#define TLS_C0DATA  0x14
#define TLS_C0DATAH 0x15
#define TLS_C1DATA  0x16
#define TLS_C1DATAH 0x17

#define COMMAND_REPEAT  0
#define COMMAND_INC     0x20

#define ID_TSL25711     0x04
#define ID_TSL25721     0x34

void TlsCommandWrite(uint8_t a_tls,uint8_t code_command,uint8_t *buffer,uint8_t l_buf);
void TlsRead(uint8_t a_tls,uint8_t *buffer,uint8_t l_buf);
void TlsCommandRead(uint8_t atls,uint8_t code,uint8_t *buffer,uint8_t l_buf);


void TlsWriteReg(uint8_t a_tls,uint8_t r_adr,uint8_t reg);
uint8_t TlsReadReg(uint8_t a_tls,uint8_t r_adr);
  

#endif