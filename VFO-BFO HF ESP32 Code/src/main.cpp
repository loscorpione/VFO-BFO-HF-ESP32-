#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "encoder.h"
#include "PLL.h"
#include "bands.h"
#include "modes.h"
#include "s_meter.h"
#include "DigiOUT.h" 
#include "comand.h"


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

  // Configura pulsanti con pull-up interni - AGGIUNGI QUESTE RIGHE
  pinMode(AGC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ATT_BUTTON_PIN, INPUT_PULLUP);
  
  // Test lettura iniziale
  Serial.print("AGC PIN iniziale: ");
  Serial.println(digitalRead(AGC_BUTTON_PIN) ? "HIGH" : "LOW");
  Serial.print("ATT PIN iniziale: ");
  Serial.println(digitalRead(ATT_BUTTON_PIN) ? "HIGH" : "LOW");

  // Inizializzazione display PRIMA di tutto
  Serial.println("Inizializzazione display...");
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BACKGROUND_COLOR);
  
  drawDisplayLayout(); // Questo inizializza anche lo sprite

  // DEBUG: Verifica frequenza iniziale
  Serial.print("Frequenza iniziale - displayedFrequency: ");
  Serial.println(displayedFrequency);
  Serial.print("Frequenza iniziale - vfoFrequency: ");
  Serial.println(vfoFrequency);

  // Inizializzazione DigiOUT - DEVE essere PRIMA di tutto
  Serial.println("Inizializzazione DigiOUT...");
  setupDigiOUT();

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

  // Configura encoder pitch BFO
  pinMode(BFO_PITCH_CLK, INPUT_PULLUP);
  pinMode(BFO_PITCH_DT, INPUT_PULLUP);

  // Inizializzazione uscite digitali
  updateModeOutputs();
  
  // Lettura stato iniziale encoder
  lastEncoded = (digitalRead(ENC_DT) << 1) | digitalRead(ENC_CLK);

  // Inizializzazione SI5351
  setupSI5351();
  updateFrequency();

  // Inizializzazione DigiOUT - DEVE essere PRIMA di updateBandInfo()
  updateModeOutputs();

  // Inizializzazione BFO per modalità corrente
  updateBFOForMode();

  // Aggiorna le informazioni di banda e modalità DOPO l'inizializzazione del DigiOUT
  updateBandInfo();
  updateModeInfo();

  // INIZIALIZZA display AGC/ATT 
  updateAGCDisplay();
  updateATTDisplay();
}

void loop() {
  readEncoder();

  static int lastBFOOffset = 0;
  updateBFOFromEncoder();
  
  // Aggiorna display se il pitch BFO è cambiato
  if (currentBFOOffset != lastBFOOffset) {
    drawBFODisplay();
    lastBFOOffset = currentBFOOffset;
  }

  // Gestione pulsanti AGC e ATT
  checkAGCButton();
  checkATTButton();
  
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

  // Aggiorna S-meter ogni 100ms
  static unsigned long lastSMeterUpdate = 0;
  if (millis() - lastSMeterUpdate > 100) {
    updateSMeter();
    lastSMeterUpdate = millis();
  }


}