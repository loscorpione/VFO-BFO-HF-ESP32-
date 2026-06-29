#ifndef PLL_H
#define PLL_H

#include <si5351.h>

// ============================================
// VARIABILI ESTERNE
// ============================================

extern Si5351 si5351;

// ============================================
// FUNZIONI
// ============================================

void setupSI5351();
void updateFrequency();
void updateBFO();
void enableBFO();
void disableBFO();
void calibrateSI5351(long calibration_factor);

#endif