#ifndef DIGIOUT_H
#define DIGIOUT_H

#include <Wire.h>
#include "functions.h"

// Dichiarazione delle variabili globali necessarie
extern unsigned long displayedFrequency;
extern int currentMode;
extern bool agcFastMode;
extern bool attenuatorEnabled;

void setupDigiOUT();
void updateModeOutputs();
void updateAGCState(); 
void updateATTState();

#endif