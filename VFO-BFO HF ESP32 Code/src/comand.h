#ifndef COMAND_H
#define COMAND_H

// Stati AGC e ATT
extern bool agcFastMode;
extern bool attenuatorEnabled;

// Variabili debounce
extern bool AGCButtonPressed;
extern bool ATTButtonPressed;
extern unsigned long lastAGCButtonPress;
extern unsigned long lastATTButtonPress;

// Funzioni AGC
void changeAGC();
void updateAGC();
void updateAGCDisplay();

// Funzioni ATT
void changeATT();
void updateATT();
void updateATTDisplay();

// Funzioni di gestione (per il main loop)
void checkAGCButton();
void checkATTButton();

#endif