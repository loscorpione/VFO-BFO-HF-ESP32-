#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern TFT_eSprite freqSprite;  // Aggiungi Sprite per la frequenza

void drawDisplayLayout();
void updateFrequencyDisplay();
void updateStepDisplay();
void drawBFODisplay();
String formatFrequency(unsigned long freq);
void setupFrequencySprite();    // Nuova funzione per inizializzare Sprite

#endif