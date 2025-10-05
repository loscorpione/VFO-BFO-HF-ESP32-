#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>


extern TFT_eSPI tft;

void drawDisplayLayout();
void updateFrequencyDisplay();
void updateStepDisplay();
String formatFrequency(unsigned long freq);


#endif