#include "modes.h"
#include "config.h"
#include "PLL.h"
#include "display.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

const char* modeNames[] = {"AM", "LSB", "USB", "CW"};
int currentMode = MODE_AM;

// Cambia alla modalità successiva
void changeMode() {
  currentMode = (currentMode + 1) % MODE_COUNT;
  updateBFOForMode();
}

// Aggiorna BFO in base alla modalità selezionata
void updateBFOForMode() {
  // Salva l'offset corrente per la modalità uscente
  if (bfoEnabled) {
    switch(currentMode) {
      case MODE_LSB:
        bfoPitchOffset[0] = currentBFOOffset;
        break;
      case MODE_USB:
        bfoPitchOffset[1] = currentBFOOffset;
        break;
      case MODE_CW:
        bfoPitchOffset[2] = currentBFOOffset;
        break;
    }
  }
  
  // Applica la nuova modalità
  switch(currentMode) {
    case MODE_AM:
      disableBFO();
      break;
    case MODE_LSB:
      currentBFOOffset = 0;  // ⬅️ RESET A ZERO
      bfoFrequency = BFO_LSB_BASE + currentBFOOffset;
      enableBFO();
      break;
    case MODE_USB:
      currentBFOOffset = 0;  // ⬅️ RESET A ZERO
      bfoFrequency = BFO_USB_BASE + currentBFOOffset;
      enableBFO();
      break;
    case MODE_CW:
      currentBFOOffset = 0;  // ⬅️ RESET A ZERO
      bfoFrequency = BFO_CW_BASE + currentBFOOffset;
      enableBFO();
      break;
  }
  updateBFO();
}

// Aggiorna visualizzazione della modalità
void updateModeInfo() {
  static int lastMode = -1;
  static bool lastBFOState = false;
  static int lastBFOOffset = 0;
  
  if (currentMode != lastMode || bfoEnabled != lastBFOState || currentBFOOffset != lastBFOOffset) {
    // Calcola la posizione del riquadro MODE (secondo riquadro)
    int boxX = POSITION_X + (BOX_WIDTH + BOX_SPACING);
    int boxWidth = BOX_WIDTH;
    
    // Pulisci l'area del testo
    tft.fillRect(boxX + 5, 218, boxWidth - 10, 15, BACKGROUND_COLOR);
    
    tft.setTextColor(MODE_COLOR, BACKGROUND_COLOR);
    tft.setTextSize(2);
    
    String modeText = modeNames[currentMode];
    
    // Calcola la posizione X centrata approssimativa
    int textWidth = modeText.length() * 12;
    int centeredX = boxX + (boxWidth - textWidth) / 2;
    
    // Disegna il testo centrato
    tft.drawString(modeText, centeredX, 218);
    
    lastMode = currentMode;
    lastBFOState = bfoEnabled;
    lastBFOOffset = currentBFOOffset;
  }
  
  drawBFODisplay();
}