#include "pcf8574.h"
#include "config.h"
#include <Arduino.h> 

// Dichiarazione delle variabili globali necessarie
extern unsigned long displayedFrequency;

// Variabile per memorizzare l'ultimo stato inviato
uint8_t lastOutputState = 0;

void setupPCF8574() {
  updatePCF8574Output();
}

void updatePCF8574Output() {
  uint8_t outputState = 0;
  
  // Controlla i range di frequenza e imposta i bit corrispondenti
  if (displayedFrequency >= 1600000 && displayedFrequency < 2500000) {
    outputState = 0b0001;  // 1.6-2.5 MHz
  }
  else if (displayedFrequency >= 2500000 && displayedFrequency < 4700000) {
    outputState = 0b0010;  // 2.5-4.7 MHz
  }
  else if (displayedFrequency >= 4700000 && displayedFrequency < 7500000) {
    outputState = 0b0011;  // 4.7-7.5 MHz
  }
  else if (displayedFrequency >= 7500000 && displayedFrequency < 14500000) {
    outputState = 0b0100;  // 7.5-14.5 MHz
  }
  else if (displayedFrequency >= 14500000 && displayedFrequency < 21500000) {
    outputState = 0b0101;  // 14.5-21.5 MHz
  }
  else if (displayedFrequency >= 21500000 && displayedFrequency <= 33000000) {
    outputState = 0b0110;  // 21.5-33 MHz
  }
  else {
    outputState = 0b0000;  // Fuori range
  }
  
  // Invia solo se lo stato è cambiato
  if (outputState != lastOutputState) {
    Wire.beginTransmission(PCF8574A_ADDRESS);
    Wire.write(outputState);
    Wire.endTransmission();
    
    lastOutputState = outputState;
    
    Serial.print("PCF8574: Frequenza ");
    Serial.print(displayedFrequency);
    Serial.print(" Hz -> Output: ");
    Serial.println(outputState, BIN);
  }
}