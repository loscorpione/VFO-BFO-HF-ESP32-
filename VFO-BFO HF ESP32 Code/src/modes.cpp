#include "modes.h"
#include "config.h"
#include "PLL.h"
#include "display.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

const char* modeNames[] = {"AM", "LSB", "USB", "CW"};
int currentMode = MODE_AM;

void changeMode() {
  currentMode = (currentMode + 1) % MODE_COUNT;
  updateBFOForMode();
}


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

void updateModeInfo() {
  static int lastMode = -1;
  static bool lastBFOState = false;
  static int lastBFOOffset = 0;
  
  if (currentMode != lastMode || bfoEnabled != lastBFOState || currentBFOOffset != lastBFOOffset) {
    int Pos_X;
    if (currentMode == MODE_AM) Pos_X= 112;
    else if (currentMode == MODE_LSB) Pos_X= 106;
    else if (currentMode == MODE_USB) Pos_X= 106;
    else if (currentMode == MODE_CW) Pos_X= 112;
    
    tft.fillRect(100, 218, 47, 15, BACKGROUND_COLOR);
    tft.setTextFont(1);
    tft.setTextColor(MODE_COLOR, BACKGROUND_COLOR);
    tft.setTextSize(2);
    tft.drawString(modeNames[currentMode], Pos_X, 218);
    
    lastMode = currentMode;
    lastBFOState = bfoEnabled;
    lastBFOOffset = currentBFOOffset;
  }
  
  drawBFODisplay();
}