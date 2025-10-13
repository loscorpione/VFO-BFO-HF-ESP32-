#include "DigiOUT.h"
#include "config.h"
#include <Arduino.h> 

// Dichiarazione delle variabili globali necessarie
extern unsigned long displayedFrequency;

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
  updateDigiOUTOutput();
}

void updateDigiOUTOutput() {
  uint8_t outputState = 0;
  
  Serial.print("DigiOUT: Aggiornamento per frequenza ");
  Serial.println(displayedFrequency);
  
  // Controlla i range di frequenza e imposta i bit corrispondenti
  if (displayedFrequency >= 1600000 && displayedFrequency < 2500000) {
    outputState = 0b0001;  // 1.6-2.5 MHz
    Serial.println("Banda: 1.6-2.5 MHz");
  }
  else if (displayedFrequency >= 2500000 && displayedFrequency < 4700000) {
    outputState = 0b0010;  // 2.5-4.7 MHz
    Serial.println("Banda: 2.5-4.7 MHz");
  }
  else if (displayedFrequency >= 4700000 && displayedFrequency < 7500000) {
    outputState = 0b0011;  // 4.7-7.5 MHz
    Serial.println("Banda: 4.7-7.5 MHz");
  }
  else if (displayedFrequency >= 7500000 && displayedFrequency < 14500000) {
    outputState = 0b0100;  // 7.5-14.5 MHz
    Serial.println("Banda: 7.5-14.5 MHz");
  }
  else if (displayedFrequency >= 14500000 && displayedFrequency < 21500000) {
    outputState = 0b0101;  // 14.5-21.5 MHz
    Serial.println("Banda: 14.5-21.5 MHz");
  }
  else if (displayedFrequency >= 21500000 && displayedFrequency <= 33000000) {
    outputState = 0b0110;  // 21.5-33 MHz
    Serial.println("Banda: 21.5-33 MHz");
  }
  else {
    outputState = 0b0000;  // Fuori range
    Serial.println("Banda: Fuori range");
  }
  
  // Invia SEMPRE all'avvio (lastOutputState = 0xFF) o se lo stato è cambiato
  if (outputState != lastOutputState || lastOutputState == 0xFF) {
    Wire.beginTransmission(PCF8574A_ADDRESS);
    Wire.write(outputState);
    byte error = Wire.endTransmission();
    
    lastOutputState = outputState;

    /* // Debug output
    Serial.print("DigiOUT: Output inviato: ");
    Serial.print(outputState, BIN);
    Serial.print(" (");
    Serial.print(outputState, DEC);
    Serial.print(")");
    if (error == 0) {
      Serial.println(" [OK]");
    } else {
      Serial.print(" [ERROR: ");
      Serial.print(error);
      Serial.println("]");
    }
  } else {
    Serial.println("DigiOUT: Nessun cambiamento, output non inviato");
  } */
}