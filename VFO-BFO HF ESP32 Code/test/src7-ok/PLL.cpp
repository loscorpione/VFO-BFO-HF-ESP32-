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
  si5351.output_enable(SI5351_CLK1, 0); // BFO inizialmente disabilitato
  
  // Configura il pin del potenziometro pitch BFO
  pinMode(BFO_PITCH_PIN, INPUT);
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

int readBFOPitch() {
  // Leggi il potenziometro e converti in offset ±2000Hz
  int rawValue = analogRead(BFO_PITCH_PIN);
  int offset = map(rawValue, 0, 4095, -BFO_PITCH_RANGE/2, BFO_PITCH_RANGE/2);
  
  // Applica deadzone per evitare drift
  if (abs(offset) < BFO_PITCH_DEADZONE) {
    offset = 0;
  }
  
  return offset;
}

void updateBFOFromPitch() {
  if (!bfoEnabled) return;
  
  int newOffset = readBFOPitch();
  
  // Aggiorna solo se l'offset è cambiato significativamente
  if (abs(newOffset - currentBFOOffset) > 5) {
    currentBFOOffset = newOffset;
    
    // Calcola la frequenza BFO in base alla modalità corrente
    switch(currentMode) {
      case MODE_LSB:
        bfoFrequency = BFO_LSB_BASE + currentBFOOffset;
        break;
      case MODE_USB:
        bfoFrequency = BFO_USB_BASE + currentBFOOffset;
        break;
      case MODE_CW:
        bfoFrequency = BFO_CW_BASE + currentBFOOffset;
        break;
    }
    
    updateBFO();
    
    // Debug
    Serial.print("BFO Pitch: ");
    Serial.print(currentBFOOffset);
    Serial.print(" Hz | Freq: ");
    Serial.println(bfoFrequency);
  }
}