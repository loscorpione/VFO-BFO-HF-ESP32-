#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>

extern TFT_eSPI tft;

void drawDisplayLayout();
void updateFrequencyDisplay();
void updateStepDisplay();
void updateBandInfo();
void updateModeInfo();
void updateBFODisplay();
void drawBFOArea();
String formatFrequency(unsigned long freq);

#endif