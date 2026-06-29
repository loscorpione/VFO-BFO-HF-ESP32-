#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>

// ============================================
// VARIABILI ESTERNE
// ============================================

extern TFT_eSPI tft;
extern TFT_eSprite freqSprite;

// ============================================
// FUNZIONI
// ============================================

void drawDisplayLayout();
void updateFrequencyDisplay();
void updateStepDisplay();
void drawBFODisplay();
String formatFrequency(unsigned long freq);
void setupFrequencySprite();
void drawSplashScreen();

// ============================================
// FUNZIONI PER INSERIMENTO FREQUENZA DA TASTIERA
// ============================================

void showFrequencyInputMode(bool active);
void updateFrequencyInputDisplay(String input);
void showInputConfirm();
void showInputError();
void hideInputMode();

#endif