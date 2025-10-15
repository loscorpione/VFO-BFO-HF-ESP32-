#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "VFO_BFO.h" 
#include "PLL.h"
#include "bands.h"
#include "modes.h"
#include "s_meter.h"
#include "DigiOUT.h" 
#include "functions.h"
#include "EEPROM_manager.h"


// Variabili globali
unsigned long vfoFrequency = 7000000 + IF_FREQUENCY;
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

bool buttonPressed = false;
bool bandButtonPressed = false;
bool modeButtonPressed = false;

// Variabili encoder
int lastEncoded = 0;
int encoderCount = 0;


void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Avvio VFO-BFO...");

  // Configurazione encoder VFO con pull-up interni 
  pinMode(VFO_ENC_CLK, INPUT_PULLUP);
  pinMode(VFO_ENC_DT, INPUT_PULLUP);
  pinMode(SW_STEP, INPUT_PULLUP);

  // Configura encoder pitch BFO con pull-up interni 
  pinMode(BFO_ENC_CLK, INPUT_PULLUP);
  pinMode(BFO_ENC_DT, INPUT_PULLUP);

  // Configura pulsanti con pull-up interni 
  pinMode(SW_AGC, INPUT_PULLUP);
  pinMode(SW_ATT, INPUT_PULLUP);
  pinMode(SW_BAND, INPUT_PULLUP);
  pinMode(SW_MODE, INPUT_PULLUP);

  // Inizializza display
  Serial.println("Inizializzazione display...");
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BACKGROUND_COLOR);

  // Inizializza encoder
  setupEncoders();
  
  // Inizializza DigiOUT
  setupDigiOUT();

  // Inizializza SI5351
  setupSI5351();

  // Inizializza EEPROM
  eepromManager.begin();
  eepromManager.loadRXState(); 
    // Calcola vfoFrequency in base alla configurazione caricata
  vfoFrequency = displayedFrequency + IF_FREQUENCY;

  // Disegna layout del display (Anche lo sprite)
  drawDisplayLayout(); 

  // Aggiorna display frequenza
  updateFrequencyDisplay();

  // Aggiorna frequenza VFO
  updateFrequency();

  // Aggiorna riquadro step
  updateStepDisplay();

  // Aggiorna riquadro modalità
  updateModeInfo();

  // Imposta uscite digitali
  updateModeOutputs();
  
  // Aggiorna riquadro banda
  updateBandInfo();

  // Aggiorna BFO per modalità corrente
  updateBFOForMode();

  // Aggiorna riquadro AGC
  updateAGCDisplay();

  // Aggiorna riquadro ATT
  updateATTDisplay();
  
}

void loop() {
  readVFOEncoder();

  // Gestione Pitch BFO
  static int lastBFOOffset = 0;
  updateBFOFromEncoder();
  
  if (currentBFOOffset != lastBFOOffset) {
    drawBFODisplay();
    lastBFOOffset = currentBFOOffset;
    eepromManager.requestSave(); // Salvataggio ritardato per BFO
  }

  
  // Gestione pulsante step (Encoder switch)
  if (digitalRead(SW_STEP) == LOW && !buttonPressed) {
    if (millis() - lastButtonPress > buttonDebounce) {
      buttonPressed = true;
      changeStep();
      lastButtonPress = millis();
      updateStepDisplay();
      eepromManager.requestQuickSave(); // Salvataggio rapido per step
      delay(300);
    }
  }  
  if (digitalRead(SW_STEP) == HIGH && buttonPressed) {
    buttonPressed = false;
  }

  // Gestione pulsante banda
  if (digitalRead(SW_BAND) == LOW && !bandButtonPressed) {
    if (millis() - lastBandButtonPress > buttonDebounce) {
      bandButtonPressed = true;
      changeBand();
      lastBandButtonPress = millis();
      updateFrequency();
      updateFrequencyDisplay();
      updateBandInfo();
      eepromManager.requestQuickSave(); // Salvataggio rapido per banda
      delay(300);
    }
  } 
  if (digitalRead(SW_BAND) == HIGH && bandButtonPressed) {
    bandButtonPressed = false;
  }

  // Gestione pulsante modalità
  if (digitalRead(SW_MODE) == LOW && !modeButtonPressed) {
    if (millis() - lastModeButtonPress > buttonDebounce) {
      modeButtonPressed = true;
      changeMode();
      lastModeButtonPress = millis();
      updateModeOutputs();
      updateModeInfo();
      eepromManager.requestQuickSave(); // Salvataggio rapido per modalità
      delay(300);
    }
  } 
  if (digitalRead(SW_MODE) == HIGH && modeButtonPressed) {
    modeButtonPressed = false;
  }

  // Gestione pulsante AGC
  checkAGCButton();

  // Gestione pulsante ATT
  checkATTButton();

  // Aggiorna S-meter ogni 100ms
  static unsigned long lastSMeterUpdate = 0;
  if (millis() - lastSMeterUpdate > 100) {
    updateSMeter();
    lastSMeterUpdate = millis();
  }

  eepromManager.update(); // Gestione salvataggio EEPROM

}