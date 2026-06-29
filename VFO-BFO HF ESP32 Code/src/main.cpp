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
#include "keypad.h"

void handleSerialCommands();
void calibrateSI5351(long calibration_factor);

// ============================================
// Variabile per calibrazione
// ============================================

int32_t currentCalibration = 0;

// ============================================
// Variabili globali
// ============================================

unsigned long vfoFrequency = 7000000 + IF_FREQUENCY;
unsigned long displayedFrequency = 7000000;
unsigned long step = 1000;
unsigned long minFreq = 1000000;
unsigned long maxFreq = 30000000;

// ============================================
// VARIABILI PER INSERIMENTO FREQUENZA
// ============================================

bool inputMode = false;
String inputDisplay = "";   // Per la visualizzazione (con punti)
String inputValue = "";     // Per il calcolo (senza punti)
unsigned long lastInputTime = 0;
const unsigned long INPUT_TIMEOUT = 5000;

// ============================================
// FUNZIONE CALIBRAZIONE SI5351 DA SERIALE
// ============================================

void handleSerialCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        // ============================================
        // COMANDI CALIBRAZIONE
        // ============================================
        
        if (command == "CAL_MAN?") {
            Serial.println("+------------------------------------------------------+");
            Serial.println("|           CALIBRAZIONE MANUALE - ISTRUZIONI          |");
            Serial.println("+------------------------------------------------------+");
            Serial.println("📌 SCOPO:");
            Serial.println("   Impostare manualmente il fattore di correzione del SI5351.\n");
            Serial.println("📋 PROCEDURA:");
            Serial.println("   1. RESETTA la calibrazione corrente: CAL_RESET");
            Serial.println("   2. Sintonizza il VFO su una frequenza precisa (es. 28.000.000 Hz)");
            Serial.println("   3. Misura la frequenza effettiva con un frequenzimetro");
            Serial.println("   4. Calcola il fattore: (Errore_Hz / Frequenza_target_Hz) × 1.000.000.000");
            Serial.println("   5. Invia il comando: CAL_MAN <valore>\n");
            Serial.println("📝 ESEMPIO:");
            Serial.println("   Target: 28.455.000 Hz (VFO + IF)");
            Serial.println("   Misurato: 28.454.710 Hz");
            Serial.println("   Errore: -290 Hz");
            Serial.println("   Fattore: (-290 × 1.000.000.000) / 28.455.000 = -10.193");
            Serial.println("   Comando: CAL_MAN -10193\n");
            Serial.println("⚠️  NOTA: Il segno del fattore può essere invertito sul tuo SI5351.");
            Serial.println("   Verifica con CAL_READ dopo l'impostazione.\n");
        }
        else if (command == "CAL_FREQ?") {
            Serial.println("+------------------------------------------------------+");
            Serial.println("|         CALIBRAZIONE AUTOMATICA - ISTRUZIONI         |");
            Serial.println("+------------------------------------------------------+");
            Serial.println("📌 SCOPO:");
            Serial.println("   Calibrare automaticamente il SI5351 misurando la frequenza.\n");
            Serial.println("📋 PROCEDURA:");
            Serial.println("   1. RESETTA la calibrazione corrente: CAL_RESET");
            Serial.println("   2. Sintonizza il VFO su una frequenza precisa (es. 28.000.000 Hz)");
            Serial.println("   3. Misura la frequenza effettiva con un frequenzimetro");
            Serial.println("   4. Invia: CAL_FREQ <frequenza_misurata>\n");
            Serial.println("📝 ESEMPIO:");
            Serial.println("   Frequenza target (VFO): 28.000.000 Hz");
            Serial.println("   Frequenza misurata: 28.454.710 Hz (VFO + IF)");
            Serial.println("   Comando: CAL_FREQ 28454710\n");
            Serial.println("✅ Il sistema calcolerà e applicherà il fattore di correzione.");
            Serial.println("   Verifica con CAL_READ dopo l'impostazione.\n");
        }
        else if (command.startsWith("CAL_MAN ")) {
            String valueStr = command.substring(8);
            valueStr.replace("<", "");
            valueStr.replace(">", "");
            long calValue = atol(valueStr.c_str());
            
            calibrateSI5351(calValue);
            eepromManager.saveCalibration(calValue);
            currentCalibration = calValue;
            
            Serial.print("✅ Calibrazione manuale applicata: ");
            Serial.println(calValue);
        }
        else if (command.startsWith("CAL_FREQ ")) {
            String freqStr = command.substring(9);
            freqStr.replace("<", "");
            freqStr.replace(">", "");
            long measuredFreq = atol(freqStr.c_str());
            
            long actualFreq = (long)displayedFrequency + (long)IF_FREQUENCY;
            long error = measuredFreq - actualFreq;
            long calValue = (long)(((long long)error * 1000000000LL) / actualFreq);
            
            Serial.println("\n=== CALIBRAZIONE AUTOMATICA ===");
            Serial.print("Target: "); Serial.print(actualFreq); Serial.println(" Hz");
            Serial.print("Misurato: "); Serial.print(measuredFreq); Serial.println(" Hz");
            Serial.print("Errore: "); Serial.print(error); Serial.println(" Hz");
            Serial.print("Fattore calcolato: "); Serial.println(calValue);
            
            calibrateSI5351(calValue);
            eepromManager.saveCalibration(calValue);
            currentCalibration = calValue;
            
            Serial.println("✅ Calibrazione automatica salvata.");
        }
        else if (command == "CAL_READ") {
            long readValue = 0;
            if (eepromManager.loadCalibration(readValue)) {
                currentCalibration = readValue;
                Serial.print("📊 Calibrazione corrente: ");
                Serial.println(currentCalibration);
            } else {
                Serial.println("❌ Nessuna calibrazione salvata");
            }
        }
        else if (command == "CAL_RESET") {
            calibrateSI5351(0);
            eepromManager.saveCalibration(0);
            currentCalibration = 0;
            Serial.println("🔄 Calibrazione resettata a 0");
        }
        else if (command == "HELP") {
            Serial.println("+------------------------------------------------------+");
            Serial.println("|              VFO-BFO RECEIVER - COMANDI               |");
            Serial.println("+------------------------------------------------------+");
            
            Serial.println("📌 COMANDI CALIBRAZIONE:\n");
            Serial.println("   CAL_MAN <valore>   - Imposta fattore manualmente");
            Serial.println("   CAL_MAN?           - Istruzioni calibrazione manuale");
            Serial.println("   CAL_FREQ <freq>    - Calibrazione automatica");
            Serial.println("   CAL_FREQ?          - Istruzioni calibrazione automatica");
            Serial.println("   CAL_READ           - Legge fattore corrente");
            Serial.println("   CAL_RESET          - Resetta calibrazione a 0\n");
            
            Serial.println("📌 ESEMPI:\n");
            Serial.println("   CAL_MAN -10193     - Imposta fattore -10193");
            Serial.println("   CAL_FREQ 28454710  - Calibra su 28.454.710 Hz");
            Serial.println("   CAL_READ           - Mostra fattore corrente\n");
            
            Serial.println("📌 ALTRI COMANDI:\n");
            Serial.println("   INFO               - Info sistema");
            Serial.println("   HELP               - Mostra questo aiuto\n");
            
            Serial.println("⚠️  IMPORTANTE:");
            Serial.println("   -La calibrazione è più precisa se effettuata su frequenze alte ");
            Serial.println("   (es. 28 MHz) e con un frequenzimetro preciso.");
            Serial.println("   -Prima di ogni calibrazione, resettare il fattore con CAL_RESET");
            Serial.println("   e verificare con CAL_READ prima di procedere.\n");
        }
        else if (command == "INFO") {
            Serial.println("\n=== VFO-BFO Receiver ===");
            Serial.print("📻 Frequenza: "); Serial.print(displayedFrequency); Serial.println(" Hz");
            Serial.print("📡 Modalità: "); Serial.println(modeNames[currentMode]);
            Serial.print("⚙️ Step: "); Serial.println(step);
            Serial.print("🔧 Calibrazione: "); Serial.println(currentCalibration);
            Serial.print("📶 BFO: "); Serial.println(bfoEnabled ? "ON" : "OFF");
            if (bfoEnabled) {
                Serial.print("   Frequenza BFO: "); Serial.println(bfoFrequency);
            }
            Serial.println();
        }
    }
}

// ============================================
// FUNZIONI INSERIMENTO FREQUENZA
// ============================================

void startFrequencyInput(char firstDigit) {
    inputMode = true;
    inputDisplay = "";
    inputValue = "";
    inputDisplay += firstDigit;
    inputValue += firstDigit;
    lastInputTime = millis();
    
    showFrequencyInputMode(true);
    updateFrequencyInputDisplay(inputDisplay);
    
    // ---------------------------------------------
    // DEBUG Modalità inserimento attivata e primo digit
    // ---------------------------------------------
    // Serial.print("Input mode: ");
    // Serial.println(inputValue);

}

void updateFrequencyInput(char digit) {
    if (!inputMode) return;
    
    lastInputTime = millis();
    
    if (digit >= '0' && digit <= '9') {
        // Aggiorna il valore reale (senza punti)
        if (inputValue.length() < 7) {
            inputValue += digit;
        } else {
            return;
        }
        
        // Costruisci la stringa per il display con punti automatici
        inputDisplay = "";
        for (int i = 0; i < inputValue.length(); i++) {
            inputDisplay += inputValue[i];
            if (i == 1 && inputValue.length() > 2) {
                inputDisplay += ".";   // Primo punto dopo 2 cifre
            }
            if (i == 4 && inputValue.length() > 5) {
                inputDisplay += ".";   // Secondo punto dopo 5 cifre
            }
        }
    }
    
    updateFrequencyInputDisplay(inputDisplay);

    // ---------------------------------------------
    // DEBUG frequenza inserita e visualizzata
    // ---------------------------------------------
    // Serial.print("Display: ");
    // Serial.println(inputDisplay);
    // Serial.print("Value: ");
    // Serial.println(inputValue);

}

void confirmFrequencyInput() {
    if (!inputMode) return;
    
    int len = inputValue.length();
    unsigned long newFreq = 0;
    
    if (len == 2) {
        // "14" → 14.000.000 Hz
        newFreq = inputValue.toInt() * 1000000UL;
    }
    else if (len >= 4 && len <= 7) {
        // Prime 2 cifre = MHz
        String mhzStr = inputValue.substring(0, 2);
        unsigned long mhz = mhzStr.toInt();
        
        // Cifre successive
        String restStr = inputValue.substring(2);
        
        // kHz (prime 3 cifre del resto)
        unsigned long khz = 0;
        if (restStr.length() >= 1) {
            String khzStr = restStr.substring(0, min(3, (int)restStr.length()));
            khz = khzStr.toInt();
            if (khzStr.length() == 1) khz *= 100;
            else if (khzStr.length() == 2) khz *= 10;
        }
        
        // Decine di Hz (cifre rimanenti dopo i kHz)
        unsigned long tensHz = 0;
        if (restStr.length() > 3) {
            String tensStr = restStr.substring(3);
            tensHz = tensStr.toInt();
            if (tensStr.length() == 1) tensHz *= 10;
        }
        
        newFreq = mhz * 1000000UL + khz * 1000UL + tensHz * 10UL;
        
        // ---------------------------------------------
        // DEBUG parsing frequenza da input
        // ---------------------------------------------
        // Serial.print("restStr: ");
        // Serial.println(restStr);
        // Serial.print("kHz: ");
        // Serial.println(khz);
        // Serial.print("tensHz: ");
        // Serial.println(tensHz);
    }
    
        // ---------------------------------------------
        // DEBUG input frequenza
        // ---------------------------------------------
        // Serial.print("Input value: ");
        // Serial.println(inputValue);
        // Serial.print("Frequenza (Hz): ");
        // Serial.println(newFreq);

    if (newFreq >= 1000000 && newFreq <= 30000000) {
        displayedFrequency = newFreq;
        vfoFrequency = displayedFrequency + IF_FREQUENCY;
        updateFrequency();
        updateBandInfo();
         eepromManager.requestQuickSave();
        showInputConfirm();
    } else {
        
        // ---------------------------------------------
        // DEBUG frequenza non valida
        // ---------------------------------------------
        // Serial.println("Frequenza non valida!");

        showInputError();  
    }
    
    inputMode = false;
    inputDisplay = "";
    inputValue = "";
}

void forceExitInputMode() {
    if (!inputMode) return;

    // Salva lo stato corrente del TFT
    uint8_t savedFont = tft.textfont;
    uint8_t savedSize = tft.textsize;
    
    inputMode = false;
    inputDisplay = "";
    inputValue = "";
    
    // Rigenera l'intera area frequenza
    tft.fillRect(VFO_DISPLAY_X, VFO_DISPLAY_Y, 250, 60, BACKGROUND_COLOR);
    
    // Ridisegna la frequenza direttamente senza sprite
    tft.setTextColor(FREQUENCY_COLOR, BACKGROUND_COLOR);
    tft.setTextFont(7);
    tft.setTextSize(0);
    tft.setCursor(VFO_DISPLAY_X , VFO_DISPLAY_Y +5 );
    tft.print(formatFrequency(displayedFrequency));

    // Ripristina lo stato salvato
    tft.setTextFont(savedFont);
    tft.setTextSize(savedSize);
    
    // ---------------------------------------------
    // DEBUG uscita forzata da modalità inserimento
    // ---------------------------------------------
    // Serial.println("Uscita forzata - Display ripristinato");
}

void cancelFrequencyInput() {
    if (!inputMode) return;
    
    inputMode = false;
    inputDisplay = "";
    inputValue = "";
    updateFrequencyDisplay();
    
    // ---------------------------------------------
    // DEBUG input annullato
    // ---------------------------------------------    
    // Serial.println("Input annullato");
}

void checkInputTimeout() {
    if (inputMode && (millis() - lastInputTime > INPUT_TIMEOUT)) {

    forceExitInputMode();

    // ---------------------------------------------
    // DEBUG timeout inserimento frequenza
    // ---------------------------------------------                
    // Serial.println("Input timeout");

    }
}



// ============================================
// SETUP
// ============================================
void setup() {

  // Inizializzazione seriale
  Serial.begin(115200);
  delay(1000);

  // Inizializzazione I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);
  delay(100);

  // === DIAGNOSTICA I2C ===
  Serial.println("\n=== DIAGNOSTICA I2C ===");
  
  byte error, address;
  int nDevices = 0;
  
  Serial.println("Scansione bus I2C...");
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Dispositivo trovato all'indirizzo 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      if (address == 0x60) Serial.println(" -> SI5351");
      else if (address == 0x20) Serial.println(" -> PCF8574A (Expander I/O)");
      else if (address == 0x50) Serial.println(" -> EEPROM");
      else if (address == 0x22) Serial.println(" -> PCF8574T (Tastiera matrix)");
      else Serial.println(" -> Sconosciuto");
      
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("Nessun dispositivo I2C trovato!");
  } else {
    Serial.print("Totale dispositivi trovati: ");
    Serial.println(nDevices);
  }
  Serial.println("=== FINE DIAGNOSTICA ===\n");

  // Configurazione pin encoder VFO (pull-up esterni)
  pinMode(VFO_ENC_CLK, INPUT);
  pinMode(VFO_ENC_DT, INPUT);
  pinMode(SW_STEP, INPUT_PULLUP);

  // Configura encoder pitch BFO
  pinMode(BFO_ENC_CLK, INPUT_PULLUP);
  pinMode(BFO_ENC_DT, INPUT_PULLUP);

  // Configura pulsanti
  pinMode(SW_AGC, INPUT_PULLUP);
  pinMode(SW_ATT, INPUT_PULLUP);
  pinMode(SW_BAND, INPUT_PULLUP);
  pinMode(SW_MODE, INPUT_PULLUP);
  pinMode(SW_SCAN, INPUT_PULLUP);


  // Inizializza display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BACKGROUND_COLOR);

  // Mostra la schermata iniziale
  drawSplashScreen(); 

    // Inizializza EEPROM
    eepromManager.begin();
    
    // Carica configurazione RX 
    eepromManager.loadRXState();

  long savedCalibration = 0;
  if (eepromManager.loadCalibration(savedCalibration)) {
      calibrateSI5351(savedCalibration);
      currentCalibration = savedCalibration;
      Serial.print("Calibrazione caricata da EEPROM: ");  
      Serial.println(savedCalibration);
  } else {
      Serial.println("Nessuna calibrazione trovata in EEPROM, usando default (0)");
  }
  
  vfoFrequency = displayedFrequency + IF_FREQUENCY;

  // inizializza tastiera a matrice 4x4 via PCF8574T
  setupKeypad();  

  // Inizializza encoder
  setupEncoders();
  
  // Inizializza DigiOUT
  setupDigiOUT();

  // Inizializza SI5351
  setupSI5351();

  // Forza l'attivazione del BFO
  updateBFOForMode();  
  
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

  Serial.println("VFO-BFO Ready - Invio 'HELP' per comandi calibrazione");
}

// ============================================
// LOOP PRINCIPALE
// ============================================
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

  // ============================================
  // GESTIONE PULSANTI 
  // ============================================
  
  // Pulsante STEP
  {
    static bool stepButtonPressed = false;
    static unsigned long lastStepButtonPress = 0;
    
    if (digitalRead(SW_STEP) == LOW && !stepButtonPressed) {
      if (millis() - lastStepButtonPress > buttonDebounce) {
        stepButtonPressed = true;
        changeStep();
        lastStepButtonPress = millis();
        updateStepDisplay();
        eepromManager.requestQuickSave();
        delay(300);
      }
    }  
    if (digitalRead(SW_STEP) == HIGH && stepButtonPressed) {
      stepButtonPressed = false;
    }
  }

  // Pulsante BANDA
  {
    static bool bandButtonPressed = false;
    static unsigned long lastBandButtonPress = 0;
    
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
  }

  // Pulsante MODALITA'
  {
    static bool modeButtonPressed = false;
    static unsigned long lastModeButtonPress = 0;
    
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
  
  // Gestione tastiera a matrice 4x4 (solo se presente)
    if (isKeypadAvailable()) {
        handleKeypad();
        
        char key = getLastKey();
        if (key != '\0') {
            if (inputMode) {
                // Siamo in modalità inserimento frequenza
                if (key >= '0' && key <= '9') {
                    updateFrequencyInput(key);
                } else if (key == 'A' || key == 'a') {
                    confirmFrequencyInput();
                } else if (key == 'C' || key == 'c' || key == '#') {
                    cancelFrequencyInput();
                }
            } else {
                // Modalità normale
                switch (key) {
                    case '*':
                        // Step Down
                        if (step == 10) step = 10000;
                        else if (step == 100) step = 10;
                        else if (step == 1000) step = 100;
                        else if (step == 10000) step = 1000;
                        updateStepDisplay();
                        // Debug
                        // Serial.print("Step: ");
                        // Serial.println(step);
                        break;
                        
                    case '#':
                        // Step Up
                        if (step == 10) step = 100;
                        else if (step == 100) step = 1000;
                        else if (step == 1000) step = 10000;
                        else if (step == 10000) step = 10;
                        updateStepDisplay();
                        // Debug
                        // Serial.print("Step: ");
                        // Serial.println(step);
                        break;
                        
                    case 'A':
                    case 'a':
                        Serial.println("Tasto A: funzione futura");
                        break;
                        
                    case 'B':
                    case 'b':
                        Serial.println("Tasto B: funzione futura");
                        break;
                        
                    case 'C':
                    case 'c':
                        Serial.println("Tasto C: funzione futura");
                        break;
                        
                    case 'D':
                    case 'd':
                        Serial.println("Tasto D: funzione futura");
                        break;
                        
                    default:
                        if (key >= '0' && key <= '9') {
                            startFrequencyInput(key);
                        }
                        break;
                }
            }
            resetLastKey();
        }
    }
    
    // Controllo timeout modalità input
    checkInputTimeout();

}