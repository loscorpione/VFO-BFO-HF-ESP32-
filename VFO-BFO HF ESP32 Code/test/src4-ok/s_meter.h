#ifndef S_METER_H
#define S_METER_H

#include "config.h"

extern int sMeterValue;
extern int sMeterPeak;
extern int previousSValue;  // Aggiungi questa variabile

void setupSMeter();
void updateSMeter();
void drawSMeter();
void drawSMeterSegment(int segment, bool state);  // Nuova funzione

#endif