#include "VFO_BFO.h"
#include "config.h"
#include "bands.h"
#include "modes.h"
#include "display.h"
#include "PLL.h"
#include "DigiOUT.h"
#include "EEPROM_manager.h"
#include <ESP32Encoder.h>
#include <Arduino.h>

// ============================================
// VARIABILI GLOBALI ESTERNE
// ============================================
extern unsigned long lastEncoderRead;
extern int lastEncoded;
extern int encoderCount;
extern bool buttonPressed;
extern unsigned long lastButtonPress;

// ============================================
// ENCODER VFO (PCNT hardware)
// ============================================
ESP32Encoder vfoEncoder;

// Variabili per il BFO (meccanico)
static int lastPitchEncoded = 0;
static int pitchEncoderCount = 0;

// ============================================
// INIZIALIZZAZIONE ENCODER VFO (adattiva)
// ============================================
void setupEncoders() {
    // Configura i pin come input (pull-up già presenti esternamente)
    pinMode(VFO_ENC_CLK, INPUT);
    pinMode(VFO_ENC_DT, INPUT);
    
    // Inizializza l'encoder usando la macro definita in config.h
    vfoEncoder.ENCODER_ATTACH_MODE(VFO_ENC_CLK, VFO_ENC_DT);
    
    // Azzera il contatore
    vfoEncoder.clearCount();
    
}

// ============================================
// LETTURA ENCODER VFO
// ============================================
void readVFOEncoder() {
    static long lastCount = 0;
    static long remainder = 0;   // Accumula i residui della divisione
    
    long currentCount = vfoEncoder.getCount();
    long delta = currentCount - lastCount;
    
    if (delta != 0) {
        #ifdef ENCODER_SENSITIVITY_DIVIDER
            // Aggiungi il residuo precedente e calcola nuovo delta
            delta = delta + remainder;
            remainder = delta % ENCODER_SENSITIVITY_DIVIDER;
            delta = delta / ENCODER_SENSITIVITY_DIVIDER;
            
            if (delta == 0) {
                // Nessun impulso completo, ma salva la posizione
                lastCount = currentCount;
                return;
            }
        #endif

        long freqChange = delta * step;
        displayedFrequency += freqChange;
        
        // Controllo limiti
        if (displayedFrequency > maxFreq) displayedFrequency = maxFreq;
        if (displayedFrequency < minFreq) displayedFrequency = minFreq;
        
        vfoFrequency = displayedFrequency + IF_FREQUENCY;
        updateFrequency();
        updateFrequencyDisplay();
        updateBandInfo();
        updateModeOutputs();
        
        lastCount = currentCount;
        eepromManager.requestSave();
    }
}

// ============================================
// CAMBIO STEP
// ============================================
void changeStep() {
    switch(step) {
        case 10: step = 100; break;
        case 100: step = 1000; break;
        case 1000: step = 10000; break;
        case 10000: step = 10; break;
        default: step = 10;
    }
    
    Serial.print("Step: ");
    if (step == 10) Serial.println("10Hz");
    else if (step == 100) Serial.println("100Hz");
    else if (step == 1000) Serial.println("1kHz");
    else if (step == 10000) Serial.println("10kHz");
}

// ============================================
// ENCODER BFO (MECCANICO) - invariato
// ============================================
int readBFOEncoder() {
    static unsigned long lastDebounceTime = 0;
    static int lastStableState = 0;
    static int lastEncodedLocal = 0;
    
    // Debounce per encoder meccanico (5ms)
    if (millis() - lastDebounceTime < 5) {
        return 0;
    }
    
    int MSB = digitalRead(BFO_ENC_CLK);
    int LSB = digitalRead(BFO_ENC_DT);
    int encoded = (MSB << 1) | LSB;
    
    if (encoded != lastStableState) {
        lastStableState = encoded;
        lastDebounceTime = millis();
        return 0;
    }
    
    int sum = (lastPitchEncoded << 2) | encoded;
    int direction = 0;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        pitchEncoderCount++;
        if (pitchEncoderCount >= 2) {
            direction = 1;
            pitchEncoderCount = 0;
        }
    }
    else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        pitchEncoderCount++;
        if (pitchEncoderCount >= 2) {
            direction = -1;
            pitchEncoderCount = 0;
        }
    }

    lastPitchEncoded = encoded;
    return direction;
}

void updateBFOFromEncoder() {
    if (!bfoEnabled) return;
    
    int direction = readBFOEncoder();
    
    if (direction != 0) {
        int newOffset = currentBFOOffset + (direction * BFO_PITCH_STEP);
        
        if (newOffset < BFO_PITCH_MIN) newOffset = BFO_PITCH_MIN;
        if (newOffset > BFO_PITCH_MAX) newOffset = BFO_PITCH_MAX;
        
        if (newOffset != currentBFOOffset) {
            currentBFOOffset = newOffset;
            
            switch(currentMode) {
                case MODE_LSB: bfoFrequency = BFO_LSB_BASE + currentBFOOffset; break;
                case MODE_USB: bfoFrequency = BFO_USB_BASE + currentBFOOffset; break;
                case MODE_CW: bfoFrequency = BFO_CW_BASE + currentBFOOffset; break;
            }
            
            updateBFO();
        }
    }
}