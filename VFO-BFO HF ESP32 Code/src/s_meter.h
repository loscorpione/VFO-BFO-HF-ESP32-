#ifndef S_METER_H
#define S_METER_H

#include "config.h"

// ============================================
// VARIABILI ESTERNE
// ============================================

extern int sMeterValue;
extern int sMeterPeak;
extern int previousSValue;

// ============================================
// FUNZIONI
// ============================================

void setupSMeter();
void updateSMeter();
void drawSMeter();
void drawSMeterSegment(int segment, bool state);

#endif