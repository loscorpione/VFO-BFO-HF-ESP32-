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

void handleSerialCommands();
void calibrateSI5351(long calibration_factor); // Dichiarazione

// Variabile per calibrazione - AGGIUNGI
int32_t currentCalibration = 0;

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

// Variabile per calibrazione
//int32_t currentCalibration = 0;

// Funzione per gestire comandi seriali
void handleSerialCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command.startsWith("CAL ")) {
            // Comando: CAL <valore>
            String valueStr = command.substring(4);
            long calValue = valueStr.toInt();
            
            calibrateSI5351(calValue);
            eepromManager.saveCalibration(calValue);
            currentCalibration = calValue;
            
            Serial.print("Calibrazione applicata e salvata: ");
            Serial.println(calValue);
            
        } else if (command == "CAL_READ") {
            // Legge calibrazione corrente
            Serial.print("Calibrazione corrente: ");
            Serial.println(currentCalibration);
            
        } else if (command == "CAL_RESET") {
            // Resetta calibrazione
            calibrateSI5351(0);
            eepromManager.saveCalibration(0);
            currentCalibration = 0;
            Serial.println("Calibrazione resettata a 0");
            
        } else if (command == "HELP") {
            // Mostra aiuto
            Serial.println("Comandi calibrazione SI5351:");
            Serial.println("CAL <valore>  - Imposta calibrazione (es: CAL 1250)");
            Serial.println("CAL_READ      - Legge calibrazione corrente");
            Serial.println("CAL_RESET     - Resetta calibrazione a 0");
            Serial.println("HELP          - Mostra questo aiuto");
            Serial.println("INFO          - Informazioni sistema");
            
        } else if (command == "INFO") {
            // Informazioni sistema
            Serial.println("=== VFO-BFO Receiver ===");
            Serial.print("Frequenza: ");
            Serial.print(displayedFrequency);
            Serial.println(" Hz");
            Serial.print("Modalità: ");
            Serial.println(modeNames[currentMode]);
            Serial.print("Step: ");
            Serial.println(step);
            Serial.print("Calibrazione SI5351: ");
            Serial.println(currentCalibration);
            Serial.print("BFO: ");
            Serial.println(bfoEnabled ? "ON" : "OFF");
            if (bfoEnabled) {
                Serial.print("Frequenza BFO: ");
                Serial.println(bfoFrequency);
            }
        }
    }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

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
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BACKGROUND_COLOR);

  // Inizializza EEPROM e carica configurazione
  eepromManager.begin();
  eepromManager.loadRXState();

  long savedCalibration = 0;
  if (eepromManager.loadCalibration(savedCalibration)) {
      calibrateSI5351(savedCalibration);
  }
  // Calcola vfoFrequency
  vfoFrequency = displayedFrequency + IF_FREQUENCY;

  // Inizializza encoder
  setupEncoders();
  
  // Inizializza DigiOUT
  setupDigiOUT();

  // Inizializza SI5351
  setupSI5351();

  // Disegna layout del display
  drawDisplayLayout(); 

  // Aggiorna tutti i display
  updateFrequencyDisplay();
  updateFrequency();
  updateStepDisplay();
  updateModeInfo();
  updateModeOutputs();
  updateBandInfo();
  updateAGCDisplay();
  updateATTDisplay();

  // Informazioni per calibrazione via seriale
  Serial.println("VFO-BFO Ready - Invio 'HELP' per comandi calibrazione");
}

void loop() {
  readVFOEncoder();

  // Gestione Pitch BFO
  static int lastBFOOffset = 0;
  updateBFOFromEncoder();
  
  if (currentBFOOffset != lastBFOOffset) {
    drawBFODisplay();
    lastBFOOffset = currentBFOOffset;
  }

  // Gestione comandi seriali
  handleSerialCommands();

  // Gestione pulsante step
  if (digitalRead(SW_STEP) == LOW && !buttonPressed) {
    if (millis() - lastButtonPress > buttonDebounce) {
      buttonPressed = true;
      changeStep();
      lastButtonPress = millis();
      updateStepDisplay();
      eepromManager.requestQuickSave();
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
      eepromManager.requestQuickSave();
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
      eepromManager.requestQuickSave();
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

  // Aggiorna S-meter ogni 50ms
  static unsigned long lastSMeterUpdate = 0;
  if (millis() - lastSMeterUpdate > 50) {
    updateSMeter();
    lastSMeterUpdate = millis();
  }

  // Gestione salvataggio EEPROM
  eepromManager.update();
}