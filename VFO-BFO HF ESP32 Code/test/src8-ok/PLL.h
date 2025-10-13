#ifndef PLL_H
#define PLL_H

#include <si5351.h>

extern Si5351 si5351;

void setupSI5351();
void updateFrequency();
void updateBFO();
void enableBFO();
void disableBFO();
void updateBFOFromPitch();       // Nuova funzione
void updateBFOFromEncoder();     // BFO da encoder
int readBFOEncoder();            // Lettura encoder

#endif