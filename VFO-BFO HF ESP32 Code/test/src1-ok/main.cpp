#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "encoder.h"
#include "PLL.h"
#include "bands.h"
#include "modes.h"

// Variabili globali
unsigned long vfoFrequency = 7000000 - IF_FREQUENCY;
unsigned long displayedFrequency = 7000000;
unsigned long step = 1000;
unsigned long minFreq = 1000000;
unsigned long maxFreq = 30000000;

// Variabili per debounce
unsigned long lastEncoderRead = 0;
const int encoderReadInterval = 10;
unsigned long lastButtonPress = 0;
unsigned long lastBandButtonPress = 0;
unsigned long lastModeButtonPress = 0;
const int buttonDebounce = 200;
bool buttonPressed = false;
bool bandButtonPressed = false;
bool modeButtonPressed = false;

// Variabili encoder
int lastEncoded = 0;
int encoderCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inizializzazione display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BACKGROUND_COLOR);
  
  drawDisplayLayout();
  updateFrequencyDisplay();
  updateStepDisplay();
  updateBandInfo();
  updateModeInfo();

  // Configurazione encoder e pulsanti
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(SW_BAND, INPUT_PULLUP);
  pinMode(SW_MODE, INPUT_PULLUP);

  // Configurazione pin output modalità
  pinMode(MODE_BIT0, OUTPUT);
  pinMode(MODE_BIT1, OUTPUT);
  updateModeOutputs();
  
  // Lettura stato iniziale encoder
  lastEncoded = (digitalRead(ENC_DT) << 1) | digitalRead(ENC_CLK);

  // Inizializzazione SI5351
  setupSI5351();
  updateFrequency();

}

void loop() {
  readEncoder();

  // Cambio step con pulsante encoder
  if (digitalRead(ENC_SW) == LOW && !buttonPressed) {
    if (millis() - lastButtonPress > buttonDebounce) {
      buttonPressed = true;
      changeStep();
      lastButtonPress = millis();
      updateStepDisplay();
      delay(300);
    }
  }
  
  if (digitalRead(ENC_SW) == HIGH && buttonPressed) {
    buttonPressed = false;
  }

  // Cambio banda con pulsante banda
  if (digitalRead(SW_BAND) == LOW && !bandButtonPressed) {
    if (millis() - lastBandButtonPress > buttonDebounce) {
      bandButtonPressed = true;
      changeBand();
      lastBandButtonPress = millis();
      updateFrequency();
      updateFrequencyDisplay();
      updateBandInfo();
      delay(300);
    }
  }
  
  if (digitalRead(SW_BAND) == HIGH && bandButtonPressed) {
    bandButtonPressed = false;
  }

  // Cambio modalità con pulsante modalità
  if (digitalRead(SW_MODE) == LOW && !modeButtonPressed) {
    if (millis() - lastModeButtonPress > buttonDebounce) {
      modeButtonPressed = true;
      changeMode();
      lastModeButtonPress = millis();
      updateModeOutputs();
      updateModeInfo();
      delay(300);
    }
  }
  
  if (digitalRead(SW_MODE) == HIGH && modeButtonPressed) {
    modeButtonPressed = false;
  }
}