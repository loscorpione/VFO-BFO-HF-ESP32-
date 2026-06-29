#include "DigiOUT.h"
#include "config.h"
#include "modes.h" 
#include "functions.h"
#include <Arduino.h> 

// ============================================
// VARIABILI GLOBALI
// ============================================

extern unsigned long displayedFrequency;
extern int currentMode;

// Variabile per memorizzare l'ultimo stato inviato
uint8_t lastOutputState = 0xFF;  // Inizializza con un valore impossibile

// ============================================
// INIZIALIZZAZIONE PCF8574
// ============================================

void setupDigiOUT() {
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

// ============================================
// AGGIORNAMENTO USCITE
// ============================================

void updateModeOutputs() {
    uint8_t outputState = 0;
    
    // ============================================
    // BIT 0-2: Selezione filtri passa banda
    // ============================================
    if (displayedFrequency >= 1600000 && displayedFrequency < 2500000) {
        outputState = 0b001;  // 160m
    }
    else if (displayedFrequency >= 2500000 && displayedFrequency < 4700000) {
        outputState = 0b010;  // 80m
    }
    else if (displayedFrequency >= 4700000 && displayedFrequency < 7500000) {
        outputState = 0b011;  // 60m
    }
    else if (displayedFrequency >= 7500000 && displayedFrequency < 14500000) {
        outputState = 0b100;  // 40m/30m
    }
    else if (displayedFrequency >= 14500000 && displayedFrequency < 21500000) {
        outputState = 0b101;  // 20m/17m
    }
    else if (displayedFrequency >= 21500000 && displayedFrequency <= 33000000) {
        outputState = 0b110;  // 15m/12m/10m
    }
    else {
        outputState = 0b000;  // Fuori banda
    }

    // ============================================
    // BIT 3-4: Selezione modalità
    // Mappatura: AM=00, LSB=01, USB=10, CW=11
    // ============================================
    outputState |= (currentMode << 3);
    
    // ============================================
    // BIT 5: Selettore AGC (1 = Fast, 0 = Slow)
    // ============================================
    if (agcFastMode) {
        outputState |= (1 << 5);
    }

    // ============================================
    // BIT 6: Selettore ATT (1 = -20dB abilitato)
    // ============================================
    if (attenuatorEnabled) {
        outputState |= (1 << 6);
    }

    // ============================================
    // BIT 7: BFO Enable (attivo per LSB, USB, CW)
    // ============================================
    if (currentMode != MODE_AM && bfoEnabled) {
        outputState |= (1 << 7);
    }
    
    // ============================================
    // INVIO DATI (solo se cambiato)
    // ============================================
    if (outputState != lastOutputState) {
        Wire.beginTransmission(PCF8574A_ADDRESS);
        Wire.write(outputState);
        byte error = Wire.endTransmission();
        
        lastOutputState = outputState;
        
        // Debug output (commentato)
        // Serial.print("DigiOUT: Banda:");
        // Serial.print(outputState & 0x07, BIN);
        // Serial.print(" Mode:");
        // Serial.print((outputState >> 3) & 0x03, BIN);
        // Serial.print(" AGC:");
        // Serial.print((outputState >> 5) & 0x01 ? "Fast" : "Slow");
        // Serial.print(" ATT:");
        // Serial.print((outputState >> 6) & 0x01 ? "ON" : "OFF");
        // Serial.print(" BFO:");
        // Serial.print((outputState >> 7) & 0x01 ? "ON" : "OFF");
        // Serial.print(" Output:");
        // Serial.print(outputState, BIN);
        // Serial.println(error == 0 ? " [OK]" : " [ERROR]");
    }
}