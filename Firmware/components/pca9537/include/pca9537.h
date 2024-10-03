#ifndef __PCA9537_H__
#define __PCA9537_H__

void Write_Port(uint8_t p);
uint8_t Read_Port();
void InitPCA(uint8_t p);
void SetIoPca(uint n,bool etat);

#endif