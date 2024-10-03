#ifndef __GESTION_TOR_H
#define __GESTION_TOR_H

typedef struct 
{
    uint32_t cpt;
    int etat;
} Data_TOR;

extern Data_TOR d_tor;
extern bool Send_TOR;

void InitGestionTOR();

#endif