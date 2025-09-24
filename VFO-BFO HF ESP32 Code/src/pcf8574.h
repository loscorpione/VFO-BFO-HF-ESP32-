#ifndef PCF8574_H
#define PCF8574_H

#include <Wire.h>

// Dichiarazione delle variabili globali necessarie
extern unsigned long displayedFrequency;

void setupPCF8574();
void updatePCF8574Output();

#endif