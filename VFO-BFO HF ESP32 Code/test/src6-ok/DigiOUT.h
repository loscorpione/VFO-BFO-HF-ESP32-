#ifndef DIGIOUT_H
#define DIGIOUT_H

#include <Wire.h>

// Dichiarazione delle variabili globali necessarie
extern unsigned long displayedFrequency;

void setupDigiOUT();
void updateDigiOUTOutput();

#endif