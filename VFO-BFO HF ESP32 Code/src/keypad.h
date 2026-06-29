#ifndef KEYPAD_H
#define KEYPAD_H

#include <Arduino.h>

// ============================================
// FUNZIONI TASTIERA
// ============================================

//Inizializza la tastiera a matrice 4x4 via PCF8574T
//Da chiamare una volta nel setup() dopo Wire.begin()
void setupKeypad();

// Gestisce la lettura dei tasti (da chiamare nel loop) 
void handleKeypad();

// Restituisce l'ultimo tasto premuto
// @return carattere del tasto, o '\0' se nessun tasto
char getLastKey();

// Resetta il buffer dell'ultimo tasto letto
void resetLastKey();

// Funzione per verificare se la tastiera è disponibile
bool isKeypadAvailable();

#endif