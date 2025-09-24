#include "modes.h"
#include "config.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

const char* modeNames[] = {"LSB", "USB", "AM", "CW"};
int currentMode = MODE_LSB;

void changeMode() {
  currentMode = (currentMode + 1) % MODE_COUNT;
}

void updateModeOutputs() {
  digitalWrite(MODE_BIT0, currentMode & 0x01);
  digitalWrite(MODE_BIT1, (currentMode >> 1) & 0x01);
}

void updateModeInfo() {
  tft.fillRect(82, 118, 65, 15, BACKGROUND_COLOR);
  tft.setTextColor(MODE_COLOR, BACKGROUND_COLOR);
  tft.setTextSize(2);
  tft.drawString(modeNames[currentMode], 90, 118);
}