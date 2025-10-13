#include "functions.h"
#include "config.h"
#include "DigiOUT.h"
#include "display.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// Variabili AGC
bool agcFastMode = true;
bool AGCButtonPressed = false;
unsigned long lastAGCButtonPress = 0;

// Variabili ATT
bool attenuatorEnabled = false;
bool ATTButtonPressed = false;
unsigned long lastATTButtonPress = 0;


// Funzioni per gestione AGC
void changeAGC() {
  agcFastMode = !agcFastMode;
}

void updateAGC() {
  updateModeOutputs(); // Aggiorna PCF8574
}

void updateAGCDisplay() {
  // Aggiorna solo il display AGC
  tft.fillRect(POSITION_X + 2*(BOX_WIDTH + BOX_SPACING) + 5, POSITION_Y + 20, 
               BOX_WIDTH - 10, 15, BACKGROUND_COLOR);
  tft.setTextColor(agcFastMode ? TFT_GREEN : TFT_YELLOW, BACKGROUND_COLOR);
  tft.setTextSize(2);
  tft.setCursor(POSITION_X + 2*(BOX_WIDTH + BOX_SPACING) + 12, POSITION_Y + 18);
  tft.print(agcFastMode ? "FAST" : "SLOW");
}

void checkAGCButton() {
 static bool lastAGCState = HIGH; // Assume inizialmente non premuto
  
  bool currentState = digitalRead(SW_AGC);
  
  // Se passa da HIGH a LOW (fronti di discesa)
  if (currentState == LOW && lastAGCState == HIGH) {
    if (millis() - lastAGCButtonPress > buttonDebounce) {
      AGCButtonPressed = true;
      changeAGC();
      lastAGCButtonPress = millis();
      updateAGC();
      updateAGCDisplay();
    }
  }
  
  lastAGCState = currentState;
  
  // Reset dello stato pressed quando il pulsante viene rilasciato
  if (currentState == HIGH && AGCButtonPressed) {
    AGCButtonPressed = false;
  }
}


// Funzioni per gestione ATT
void changeATT() {
  attenuatorEnabled = !attenuatorEnabled;
}

void updateATT() {
  updateModeOutputs(); // Aggiorna PCF8574
}

void updateATTDisplay() {
  // Aggiorna solo il display ATT
  tft.fillRect(POSITION_X + 3*(BOX_WIDTH + BOX_SPACING) + 5, POSITION_Y + 18, 
               BOX_WIDTH - 10, 15, BACKGROUND_COLOR);
  tft.setTextColor(attenuatorEnabled ? TFT_RED : TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(2);
  tft.setCursor(POSITION_X + 3*(BOX_WIDTH + BOX_SPACING) + 8, POSITION_Y + 18);
  tft.print(attenuatorEnabled ? "-20dB" : "0dB");
}

void checkATTButton() {
  static bool lastATTState = HIGH; // Assume inizialmente non premuto
  
  bool currentState = digitalRead(SW_ATT);
  
  // Se passa da HIGH a LOW (fronti di discesa)
  if (currentState == LOW && lastATTState == HIGH) {
    if (millis() - lastATTButtonPress > buttonDebounce) {
      ATTButtonPressed = true;
      changeATT();
      lastATTButtonPress = millis();
      updateATT();
      updateATTDisplay();
    }
  }
  
  lastATTState = currentState;
  
  // Reset dello stato pressed quando il pulsante viene rilasciato
  if (currentState == HIGH && ATTButtonPressed) {
    ATTButtonPressed = false;
  }
}