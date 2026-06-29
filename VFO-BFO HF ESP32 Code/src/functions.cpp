#include "functions.h"
#include "config.h"
#include "DigiOUT.h"
#include "display.h"
#include "EEPROM_manager.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================
// VARIABILI GLOBALI
// ============================================

// Variabili AGC
bool agcFastMode = true;
bool AGCButtonPressed = false;
unsigned long lastAGCButtonPress = 0;

// Variabili ATT
bool attenuatorEnabled = false;
bool ATTButtonPressed = false;
unsigned long lastATTButtonPress = 0;

// ============================================
// FUNZIONI AGC
// ============================================

void changeAGC() {
    agcFastMode = !agcFastMode;
}

void updateAGC() {
    updateModeOutputs();
}

void updateAGCDisplay() {
    int boxX = POSITION_X + 2 * (BOX_WIDTH + BOX_SPACING);
    int boxWidth = BOX_WIDTH;
    
    tft.fillRect(boxX + 5, POSITION_Y + 18, boxWidth - 10, 15, BACKGROUND_COLOR);
    
    tft.setTextColor(agcFastMode ? TFT_GREEN : TFT_YELLOW, BACKGROUND_COLOR);
    tft.setTextSize(2);
    
    String agcText = agcFastMode ? "FAST" : "SLOW";
    
    int textWidth = agcText.length() * 12;
    int centeredX = boxX + (boxWidth - textWidth) / 2;
    
    tft.drawString(agcText, centeredX, POSITION_Y + 18);
}

void checkAGCButton() {
    static bool lastAGCState = HIGH;
    
    bool currentState = digitalRead(SW_AGC);
    
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
    
    if (currentState == HIGH && AGCButtonPressed) {
        AGCButtonPressed = false;
    }
}

// ============================================
// FUNZIONI ATT
// ============================================

void changeATT() {
    attenuatorEnabled = !attenuatorEnabled;
}

void updateATT() {
    updateModeOutputs();
}

void updateATTDisplay() {
    int boxX = POSITION_X + 3 * (BOX_WIDTH + BOX_SPACING);
    int boxWidth = BOX_WIDTH;
    
    tft.fillRect(boxX + 5, POSITION_Y + 18, boxWidth - 10, 15, BACKGROUND_COLOR);
    
    tft.setTextColor(attenuatorEnabled ? TFT_RED : TFT_WHITE, BACKGROUND_COLOR);
    tft.setTextSize(2);
    
    String attText = attenuatorEnabled ? "-20dB" : "0dB";
    
    int textWidth = attText.length() * 12;
    int centeredX = boxX + (boxWidth - textWidth) / 2;
    
    tft.drawString(attText, centeredX, POSITION_Y + 18);
}

void checkATTButton() {
    static bool lastATTState = HIGH;
    
    bool currentState = digitalRead(SW_ATT);
    
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
    
    if (currentState == HIGH && ATTButtonPressed) {
        ATTButtonPressed = false;
    }
}