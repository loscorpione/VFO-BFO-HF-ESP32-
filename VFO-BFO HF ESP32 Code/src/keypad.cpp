#include "keypad.h"
#include "config.h"
#include <Wire.h>
#include <I2CKeyPad.h>

// ============================================
// VARIABILI PRIVATE
// ============================================

I2CKeyPad keyPad(KEYPAD_I2C_ADDR);

static char lastKey = '\0';
static unsigned long lastKeyPressTime = 0;
static const char keys[] = "147*2580369#ABCDNF";
static bool keypadAvailable = false;
static uint8_t lastIndex = 255;      // Ultimo indice rilevato
static char lastReportedKey = '\0';  // Ultimo tasto segnalato

// ============================================
// INIZIALIZZAZIONE
// ============================================

void setupKeypad() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    
    if (keyPad.begin() == false) {
        Serial.println("Tastiera 4x4 NON rilevata (optional)");
        keypadAvailable = false;
        return;
    }
    
    keypadAvailable = true;
    Serial.println("Tastiera 4x4 rilevata e inizializzata");
}

// ============================================
// GESTIONE TASTIERA (rilevamento fronti)
// ============================================

void handleKeypad() {
    if (!keypadAvailable) return;
    
    uint8_t index = keyPad.getKey();
    static uint8_t lastIndex = 255;
    
    // Rileva solo il fronte di discesa (quando il tasto viene premuto)
    if (lastIndex >= 16 && index < 16) {
        char key = keys[index];
        if (millis() - lastKeyPressTime > KEYPAD_DEBOUNCE_MS) {
            lastKey = key;
            lastKeyPressTime = millis();
            // Debug
            // Serial.print("Tasto: ");
            // Serial.println(lastKey);
        }
    }
    
    // Aggiorna lo stato precedente
    lastIndex = index;
}

// ============================================
// FUNZIONI DI LETTURA
// ============================================

char getLastKey() {
    if (!keypadAvailable) return '\0';
    return lastKey;
}

void resetLastKey() {
    if (!keypadAvailable) return;
    lastKey = '\0';
}

bool isKeypadAvailable() {
    return keypadAvailable;
}