#include "DigiOUT.h"
#include "config.h"
#include "modes.h" 
#include "comand.h"
#include <Arduino.h> 

// Dichiarazione delle variabili globali necessarie
extern unsigned long displayedFrequency;
extern int currentMode;

// Variabile per memorizzare l'ultimo stato inviato
uint8_t lastOutputState = 0xFF; // Inizializza con un valore impossibile

void setupDigiOUT() {
  // Inizializza la comunicazione I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(100);
  
  // Reset del PCF8574 - imposta tutte le uscite a 0
  Wire.beginTransmission(PCF8574A_ADDRESS);
  Wire.write(0b00000000);
  Wire.endTransmission();
  delay(50);
  
  // Forza l'aggiornamento al primo avvio
  lastOutputState = 0xFF;
  
  // Aggiorna con lo stato corretto
  updateModeOutputs();
}

void updateModeOutputs() {
  uint8_t outputState = 0;
  
   // 1. Calcola codice banda (bit 0-3)
  if (displayedFrequency >= 1600000 && displayedFrequency < 2500000) {
    outputState = 0b0001;
  }
  else if (displayedFrequency >= 2500000 && displayedFrequency < 4700000) {
    outputState = 0b0010;
  }
  else if (displayedFrequency >= 4700000 && displayedFrequency < 7500000) {
    outputState = 0b0011;
  }
  else if (displayedFrequency >= 7500000 && displayedFrequency < 14500000) {
    outputState = 0b0100;
  }
  else if (displayedFrequency >= 14500000 && displayedFrequency < 21500000) {
    outputState = 0b0101;
  }
  else if (displayedFrequency >= 21500000 && displayedFrequency <= 33000000) {
    outputState = 0b0110;
  }
  else {
    outputState = 0b0000;
  }
  
  // 2. Aggiungi codice modalità (bit 4-5)
  outputState |= (currentMode << 4);
  
  // 3. Aggiungi AGC (bit 6)
  if (agcFastMode) {
    outputState |= (1 << 6);  // AGC Fast
  }
  
  // 4. Aggiungi ATT (bit 7)
  if (attenuatorEnabled) {
    outputState |= (1 << 7);  // ATT -20dB abilitato
  }
  
  if (outputState != lastOutputState) {
    Wire.beginTransmission(PCF8574A_ADDRESS);
    Wire.write(outputState);
    byte error = Wire.endTransmission();
    
    lastOutputState = outputState;
    
    Serial.print("DigiOUT: Banda:");
    Serial.print(outputState & 0x0F, BIN);
    Serial.print(" Mode:");
    Serial.print((outputState >> 4) & 0x03, BIN);
    Serial.print(" AGC:");
    Serial.print((outputState >> 6) & 0x01 ? "Fast" : "Slow");
    Serial.print(" ATT:");
    Serial.print((outputState >> 7) & 0x01 ? "ON" : "OFF");
    Serial.print(" Output:");
    Serial.print(outputState, BIN);
    Serial.println(error == 0 ? " [OK]" : " [ERROR]");
  }
}

