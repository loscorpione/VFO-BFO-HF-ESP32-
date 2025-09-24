#include "BFO.h"
#include "config.h"
#include "modes.h"

extern Si5351 si5351;
extern int currentMode;

// Variabili BFO
static int bfoOffset = 0;
static int lastBFOValue = 0;
static unsigned long lastBFOUpdate = 0;
static unsigned long bfoFrequency = 0;

void setupBFO() {
  pinMode(BFO_PIN, INPUT);
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA);
  si5351.output_enable(SI5351_CLK1, 1);
  updateBFO();
}

void updateBFO() {
  // Disabilita sempre il BFO inizialmente, poi riabilitalo solo se necessario
  si5351.output_enable(SI5351_CLK1, 0);
  
  // Se siamo in modalità AM, disabilita completamente il BFO e esci
  if (currentMode == MODE_AM) {
    return;
  }
  
  long baseFrequency = IF_FREQUENCY;
  long shiftRange = BFO_SHIFT_RANGE;
  
  switch(currentMode) {
    case MODE_LSB:
      baseFrequency += BFO_LSB_OFFSET;
      break;
    case MODE_USB:
      baseFrequency += BFO_USB_OFFSET;
      break;
    case MODE_CW:
      baseFrequency += BFO_CW_OFFSET;
      shiftRange = BFO_CW_SHIFT_RANGE;
      break;
    default:
      break;
  }
  
  // Abilita il BFO solo per le modalità che lo richiedono
  si5351.output_enable(SI5351_CLK1, 1);
  
  // Leggi il valore analogico e mappalo allo shift
  int analogValue = analogRead(BFO_PIN);
  bfoOffset = map(analogValue, 0, 4095, -shiftRange, shiftRange);
  
  // Calcola la frequenza finale del BFO
  bfoFrequency = baseFrequency + bfoOffset;
  
  // Imposta la frequenza del BFO su CLK1
  si5351.set_freq(bfoFrequency * 100ULL, SI5351_CLK1);
}

int getBFOOffset() {
  return bfoOffset;
}

unsigned long getBOFFrequency() {
  return bfoFrequency;
}