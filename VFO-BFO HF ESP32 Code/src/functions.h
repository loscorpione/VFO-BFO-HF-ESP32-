#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// ============================================
// VARIABILI ESTERNE
// ============================================

// Stati AGC e ATT
extern bool agcFastMode;
extern bool attenuatorEnabled;

// Variabili debounce
extern bool AGCButtonPressed;
extern bool ATTButtonPressed;
extern unsigned long lastAGCButtonPress;
extern unsigned long lastATTButtonPress;

// ============================================
// FUNZIONI AGC
// ============================================

void changeAGC();
void updateAGC();
void updateAGCDisplay();

// ============================================
// FUNZIONI ATT
// ============================================

void changeATT();
void updateATT();
void updateATTDisplay();

// ============================================
// FUNZIONI DI GESTIONE (MAIN LOOP)
// ============================================

void checkAGCButton();
void checkATTButton();

#endif