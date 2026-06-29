#ifndef DIGIOUT_H
#define DIGIOUT_H

#include <Wire.h>
#include "functions.h"

// ============================================
// VARIABILI ESTERNE
// ============================================

extern unsigned long displayedFrequency;
extern int currentMode;
extern bool agcFastMode;
extern bool attenuatorEnabled;

// ============================================
// FUNZIONI
// ============================================

void setupDigiOUT();
void updateModeOutputs();
void updateAGCState(); 
void updateATTState();

#endif