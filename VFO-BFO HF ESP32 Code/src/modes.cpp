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
  updateBFOForMode();  // Aggiorna BFO quando cambia modalità
}

void updateModeOutputs() {
  digitalWrite(MODE_BIT0, currentMode & 0x01);
  digitalWrite(MODE_BIT1, (currentMode >> 1) & 0x01);
}

void updateBFOForMode() {
  switch(currentMode) {
    case MODE_AM:
      disableBFO();
      break;
    case MODE_LSB:
      bfoFrequency = BFO_LSB_FREQ;
      enableBFO();
      break;
    case MODE_USB:
      bfoFrequency = BFO_USB_FREQ;
      enableBFO();
      break;
    case MODE_CW:
      bfoFrequency = BFO_CW_FREQ;
      enableBFO();
      break;
  }
  updateBFO(); // Aggiorna frequenza uscita BFO
  drawBFODisplay();  // Aggiorna il display BFO
}

void updateModeInfo() {
  static int lastMode = -1; // Memorizza l'ultima modalità visualizzata
  static bool lastBFOState = false; // Memorizza l'ultimo stato BFO
  
  // Aggiorna solo se la modalità è cambiata o lo stato BFO è cambiato
  if (currentMode != lastMode || bfoEnabled != lastBFOState) {
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
  }
  
  // Aggiorna sempre il display BFO (ha già la sua logica di ottimizzazione)
  drawBFODisplay();
}