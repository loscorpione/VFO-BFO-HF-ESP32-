
#include "VFO_BFO.h"
#include "config.h"
#include "bands.h"
#include "modes.h"
#include "display.h"
#include "PLL.h"
#include "DigiOUT.h"
#include "EEPROM_manager.h"
#include <Arduino.h>

// Variabili globali esterne
extern unsigned long lastEncoderRead;
extern int lastEncoded;
extern int encoderCount;
extern bool buttonPressed;
extern unsigned long lastButtonPress;

// Variabili encoder pitch BFO
static int lastPitchEncoded = 0;
static int pitchEncoderCount = 0;

// ==================== INIZIALIZZAZIONE ENCODER ====================

void setupEncoders() {
  // Inizializzazione encoder VFO
  lastEncoded = (digitalRead(VFO_ENC_DT) << 1) | digitalRead(VFO_ENC_CLK);
  
  // Inizializzazione encoder BFO
  lastPitchEncoded = (digitalRead(BFO_ENC_DT) << 1) | digitalRead(BFO_ENC_CLK);
}

// ==================== ENCODER VFO ====================

void readVFOEncoder() {
  static unsigned long lastUpdate = 0;
  const unsigned long UPDATE_INTERVAL = 50000; // 50ms tra gli aggiornamenti
  
  int MSB = digitalRead(VFO_ENC_CLK);
  int LSB = digitalRead(VFO_ENC_DT);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderCount++;
    if (encoderCount >= 2) {
      if (micros() - lastUpdate > UPDATE_INTERVAL) {
        displayedFrequency += step;
        if (displayedFrequency > maxFreq) displayedFrequency = maxFreq;
        vfoFrequency = displayedFrequency + IF_FREQUENCY;
        updateFrequency();
        updateFrequencyDisplay(); 
        updateBandInfo();
        updateModeOutputs(); 
        encoderCount = 0;
        lastUpdate = micros();

        eepromManager.requestSave(); // Salvataggio ritardato per VFO
      }
    }
  }
  else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderCount++;
    if (encoderCount >= 2) {
      if (micros() - lastUpdate > UPDATE_INTERVAL) {
        displayedFrequency -= step;
        if (displayedFrequency < minFreq) displayedFrequency = minFreq;
        vfoFrequency = displayedFrequency + IF_FREQUENCY;
        updateFrequency();
        updateFrequencyDisplay(); 
        updateBandInfo();
        updateModeOutputs();
        encoderCount = 0;
        lastUpdate = micros();

        eepromManager.requestSave(); // Salvataggio ritardato per VFO
      }
    }
  }

  lastEncoded = encoded;
  delayMicroseconds(100);
}

void changeStep() {
  switch(step) {
    case 10: step = 100; break;
    case 100: step = 1000; break;
    case 1000: step = 10000; break;
    case 10000: step = 10; break;
    default: step = 1000;
  }
}

// ==================== ENCODER BFO ====================


// Legge encoder pitch BFO e ritorna -1 (indietro), +1 (avanti) o 0 (nessun movimento)
int readBFOEncoder() {
  static unsigned long lastStableTime = 0;
  const unsigned long STABLE_TIME = 3; // 3ms di stabilità
  
  int MSB = digitalRead(BFO_ENC_CLK);
  int LSB = digitalRead(BFO_ENC_DT);
  int encoded = (MSB << 1) | LSB;

  // Aspetta che il segnale sia stabile per 3ms
  static int lastEncoded = 0;
  static unsigned long changeTime = 0;
  
  if (encoded != lastEncoded) {
    changeTime = millis();
    lastEncoded = encoded;
    return 0;
  }
  
  if (millis() - changeTime < STABLE_TIME) {
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

// Aggiorna BFO in base al movimento dell'encoder pitch
void updateBFOFromEncoder() {
  if (!bfoEnabled) return;
  
  int direction = readBFOEncoder();
  
  if (direction != 0) {
    int newOffset = currentBFOOffset + (direction * BFO_PITCH_STEP);
    
    // Applica limiti come nel VFO
    if (newOffset < BFO_PITCH_MIN) newOffset = BFO_PITCH_MIN;
    if (newOffset > BFO_PITCH_MAX) newOffset = BFO_PITCH_MAX;
    
    if (newOffset != currentBFOOffset) {
      currentBFOOffset = newOffset;
      
      // Calcola frequenza BFO con offset
      switch(currentMode) {
        case MODE_LSB: bfoFrequency = BFO_LSB_BASE + currentBFOOffset; break;
        case MODE_USB: bfoFrequency = BFO_USB_BASE + currentBFOOffset; break;
        case MODE_CW: bfoFrequency = BFO_CW_BASE + currentBFOOffset; break;
      }
      
      updateBFO();
    }
  }
}