#include "modes.h"
#include "config.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

const char* modeNames[] = {"AM", "LSB", "USB", "CW"};
int currentMode = MODE_AM;

void changeMode() {
  currentMode = (currentMode + 1) % MODE_COUNT;
}

void updateModeOutputs() {
  digitalWrite(MODE_BIT0, currentMode & 0x01);
  digitalWrite(MODE_BIT1, (currentMode >> 1) & 0x01);
}

void updateModeInfo() {
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
  }