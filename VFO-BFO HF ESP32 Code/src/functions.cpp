#include "functions.h"
#include "config.h"
#include "DigiOUT.h"
#include "display.h"
#include "EEPROM_manager.h"
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
  // Calcola la posizione del riquadro AGC (terzo riquadro)
  int boxX = POSITION_X + 2 * (BOX_WIDTH + BOX_SPACING);
  int boxWidth = BOX_WIDTH;
  
  // Pulisci l'area del testo
  tft.fillRect(boxX + 5, POSITION_Y + 18, boxWidth - 10, 15, BACKGROUND_COLOR);
  
  tft.setTextColor(agcFastMode ? TFT_GREEN : TFT_YELLOW, BACKGROUND_COLOR);
  tft.setTextSize(2);
  
  String agcText = agcFastMode ? "FAST" : "SLOW";
  
  // Calcola la posizione X centrata approssimativa
  int textWidth = agcText.length() * 12;
  int centeredX = boxX + (boxWidth - textWidth) / 2;
  
  // Disegna il testo centrato
  tft.drawString(agcText, centeredX, POSITION_Y + 18);
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
  // Calcola la posizione del riquadro ATT (quarto riquadro)
  int boxX = POSITION_X + 3 * (BOX_WIDTH + BOX_SPACING);
  int boxWidth = BOX_WIDTH;
  
  // Pulisci l'area del testo
  tft.fillRect(boxX + 5, POSITION_Y + 18, boxWidth - 10, 15, BACKGROUND_COLOR);
  
  tft.setTextColor(attenuatorEnabled ? TFT_RED : TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(2);
  
  String attText = attenuatorEnabled ? "-20dB" : "0dB";
  
  // Calcola la posizione X centrata approssimativa
  // Per testo size 2: circa 12 pixel per carattere
  int textWidth = attText.length() * 12;
  int centeredX = boxX + (boxWidth - textWidth) / 2;
  
  // Disegna il testo centrato
  tft.drawString(attText, centeredX, POSITION_Y + 18);
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