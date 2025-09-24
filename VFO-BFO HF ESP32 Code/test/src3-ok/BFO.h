#ifndef BFO_H
#define BFO_H

#include <Arduino.h>
#include <si5351.h>

// Dichiarazioni funzioni BFO
void setupBFO();
void updateBFO();
int getBFOOffset();
unsigned long getBOFFrequency();

#endif