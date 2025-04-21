#ifndef MATRIZ_H
#define MATRIZ_H

#include <stdint.h>

// Cores (assuma que estão definidas em outro lugar ou defina aqui)
static const uint32_t VERDE   = 0xAA000000;
static const uint32_t VERMELHO =  0x00AA0000;
static const uint32_t AMARELO  = 0xAAAA0000;
static const uint32_t ROXO  = 0x00333300;;

static const uint32_t icones[4][25] = {
    // Ícone 0: O verde
    {
         0,     VERDE,  VERDE,    VERDE,     0,
        VERDE, 0,     0, 0,     VERDE,
      VERDE,     0,     0,     0, VERDE,
         VERDE,  0,     0,  0,     VERDE,
         0,     VERDE,  VERDE,     VERDE,     0
    },
    // Ícone 1: ✕ amarelo
    {
   AMARELO,      0,      0,      0,  AMARELO,
      0,  AMARELO,      0,  AMARELO,      0,
      0,      0,  AMARELO,      0,      0,
      0,  AMARELO,      0,  AMARELO,      0,
   AMARELO,      0,      0,      0,  AMARELO
    },
        // Ícone 2: ✕ laraja
    {
   ROXO,      0,      0,      0,  ROXO,
      0,  ROXO,      0,  ROXO,      0,
      0,      0,  ROXO,      0,      0,
      0,  ROXO,      0,  ROXO,      0,
   ROXO,      0,      0,      0,  ROXO
    },
    // Ícone 3: ✕ vermelho
    {
  VERMELHO,      0,      0,      0, VERMELHO,
      0, VERMELHO,      0, VERMELHO,      0,
      0,      0, VERMELHO,      0,      0,
      0, VERMELHO,      0, VERMELHO,      0,
  VERMELHO,      0,      0,      0, VERMELHO
    }
};

#endif 