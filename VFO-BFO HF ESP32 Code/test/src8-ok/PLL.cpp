#include "si5351.h"
#include "config.h"
#include "modes.h"
#include <Wire.h>

Si5351 si5351;


// Variabili per stato BFO
bool bfoEnabled = false;
unsigned long bfoFrequency = 0;

// Offset pitch per ogni modalità (LSB, USB, CW)
int bfoPitchOffset[3] = {0, 0, 0}; // Inizialmente zero
int currentBFOOffset = 0;

// Variabili encoder pitch BFO
static int lastPitchEncoded = 0;
static int pitchEncoderCount = 0;

void setupSI5351() {
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(100);

  if (si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0) == false) {
    while(1) {
      delay(1000);
    }
  }

  si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);
  si5351.output_enable(SI5351_CLK0, 1);
  si5351.output_enable(SI5351_CLK1, 0);
  
  /* // Configura encoder pitch BFO
  pinMode(BFO_PITCH_CLK, INPUT_PULLUP);
  pinMode(BFO_PITCH_DT, INPUT_PULLUP); */

  // Lettura stato iniziale encoder
  lastPitchEncoded = (digitalRead(BFO_PITCH_DT) << 1) | digitalRead(BFO_PITCH_CLK);
  
  Serial.print("🔧 Encoder inizializzato - Stato: ");
  Serial.println(lastPitchEncoded, BIN);
}

void updateFrequency() {
  si5351.set_freq(vfoFrequency * 100ULL, SI5351_CLK0);
}

void updateBFO() {
  if (bfoEnabled) {
    si5351.set_freq(bfoFrequency * 100ULL, SI5351_CLK1);
  }
}

void enableBFO() {
  bfoEnabled = true;
  si5351.output_enable(SI5351_CLK1, 1);
  updateBFO();// Aggiorna con l'offset corrente
}

void disableBFO() {
  bfoEnabled = false;
  si5351.output_enable(SI5351_CLK1, 0);
}


int readBFOEncoder() {
  static unsigned long lastStableTime = 0;
  const unsigned long STABLE_TIME = 3; // 3ms di stabilità
  
  int MSB = digitalRead(BFO_PITCH_CLK);
  int LSB = digitalRead(BFO_PITCH_DT);
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
      
      Serial.print("🎛️ BFO Pitch: ");
      Serial.print(currentBFOOffset);
      Serial.print("Hz | Freq: ");
      Serial.println(bfoFrequency);
      Serial.print("🎛️ BFO - Raw: ");
Serial.print(bfoFrequency);
Serial.print("Hz | Offset: ");
Serial.print(currentBFOOffset);
Serial.print("Hz | Display: ");
Serial.print(bfoFrequency / 1000);
Serial.print(".");
unsigned long hz = bfoFrequency % 1000;
if (hz < 100) Serial.print("0");
if (hz < 10) Serial.print("0");
Serial.print(hz);
Serial.println(" kHz");
    }
  }
}

void updateBFOFromPitch() {
  
}