#include "EEPROM_manager.h"
#include "config.h"
#include "modes.h"
#include "functions.h"
#include "bands.h"

// ============================================
// VARIABILI ESTERNE
// ============================================

extern unsigned long displayedFrequency;
extern int currentMode;
extern unsigned long step;
extern bool agcFastMode;
extern bool attenuatorEnabled;

// ============================================
// ISTANZA GLOBALE
// ============================================

EEPROMManager eepromManager;

// ============================================
// FUNZIONI DI BASE
// ============================================

void EEPROMManager::begin() {
    Wire.setClock(100000);
    Wire.setTimeout(1000);
    delay(100);
}

bool EEPROMManager::write(uint16_t address, const uint8_t* data, uint16_t len) {
    if (address + len > EEPROM_SIZE) {
        Serial.println("❌ write: indirizzo fuori range");
        return false;
    }

    /* DEBUG
    Serial.print("✍️ write: addr=");
    Serial.print(address);
    Serial.print(" len=");
    Serial.println(len); */
    
    uint16_t bytesWritten = 0;
    
    while (bytesWritten < len) {
        uint16_t pageBoundary = (address + bytesWritten + 32) & 0xFFE0;
        uint16_t bytesInThisPage = min((uint16_t)(pageBoundary - (address + bytesWritten)), 
                                        (uint16_t)(len - bytesWritten));
        bytesInThisPage = min(bytesInThisPage, (uint16_t)32);
        
        Wire.beginTransmission(EXTERNAL_EEPROM_ADDRESS);
        Wire.write((uint8_t)((address + bytesWritten) >> 8));
        Wire.write((uint8_t)((address + bytesWritten) & 0xFF));
        
        for (uint16_t i = 0; i < bytesInThisPage; i++) {
            if (Wire.write(data[bytesWritten + i]) != 1) {
                Wire.endTransmission();
                return false;
            }
        }
        
        byte error = Wire.endTransmission();
        if (error != 0) {
            return false;
        }
        
        bytesWritten += bytesInThisPage;
        delay(10);
    }
    
    return true;
}

bool EEPROMManager::read(uint16_t address, uint8_t* data, uint16_t len) {
    if (address + len > EEPROM_SIZE) {
        return false;
    }
    
    uint16_t bytesRead = 0;
    
    while (bytesRead < len) {
        uint16_t bytesToRead = min((uint16_t)32, (uint16_t)(len - bytesRead));
        uint16_t currentAddress = address + bytesRead;
        
        Wire.beginTransmission(EXTERNAL_EEPROM_ADDRESS);
        Wire.write((uint8_t)(currentAddress >> 8));
        Wire.write((uint8_t)(currentAddress & 0xFF));
        
        if (Wire.endTransmission(false) != 0) {
            return false;
        }
        
        uint8_t received = Wire.requestFrom(EXTERNAL_EEPROM_ADDRESS, bytesToRead);
        if (received != bytesToRead) {
            return false;
        }
        
        for (uint16_t i = 0; i < bytesToRead; i++) {
            data[bytesRead + i] = Wire.read();
        }
        
        bytesRead += bytesToRead;
        delay(5);
    }
    
    return true;
}

// ============================================
// CHECKSUM
// ============================================

uint8_t EEPROMManager::calculateChecksum(const uint8_t* data, size_t len) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

bool EEPROMManager::verifyChecksum(const uint8_t* data, size_t len, uint8_t checksum) {
    return calculateChecksum(data, len) == checksum;
}

// ============================================
// SALVATAGGIO / CARICAMENTO CONFIG
// ============================================

bool EEPROMManager::loadConfig(RXConfig& config) {
    const int BLOCK_SIZE = 64;
    
    uint8_t* configBytes = (uint8_t*)&config;
    uint16_t totalSize = sizeof(RXConfig);
    uint16_t bytesRead = 0;
    
    while (bytesRead < totalSize) {
        uint16_t bytesToRead = min((uint16_t)BLOCK_SIZE, (uint16_t)(totalSize - bytesRead));
        uint16_t currentAddress = EEPROM_CONFIG_START + bytesRead;
        
        if (!read(currentAddress, configBytes + bytesRead, bytesToRead)) {
            return false;
        }
        
        bytesRead += bytesToRead;
    }
    
    uint8_t stored_checksum = config.checksum;
    config.checksum = 0;
    uint8_t calculated_checksum = calculateChecksum((uint8_t*)&config, sizeof(RXConfig) - 1);
    
    if (stored_checksum != calculated_checksum) {
        return false;
    }
    
    return true;
}

bool EEPROMManager::saveConfig(const RXConfig& config) {
    RXConfig configToSave = config;
    
    configToSave.checksum = 0;
    configToSave.checksum = calculateChecksum((uint8_t*)&configToSave, sizeof(RXConfig) - 1);
    
    return write(EEPROM_CONFIG_START, (uint8_t*)&configToSave, sizeof(RXConfig));
}

// ============================================
// MEMORIZZAZIONI
// ============================================

bool EEPROMManager::saveMemory(uint8_t slot, const MemoryChannel& memory) {
    if (slot >= 10) {
        return false;
    }
    
    uint16_t address = EEPROM_MEMORIES_START + (slot * sizeof(MemoryChannel));
    return write(address, (uint8_t*)&memory, sizeof(MemoryChannel));
}

bool EEPROMManager::loadMemory(uint8_t slot, MemoryChannel& memory) {
    if (slot >= 10) {
        return false;
    }
    
    uint16_t address = EEPROM_MEMORIES_START + (slot * sizeof(MemoryChannel));
    return read(address, (uint8_t*)&memory, sizeof(MemoryChannel));
}

bool EEPROMManager::formatEEPROM() {
    uint8_t blank[32];
    memset(blank, 0xFF, 32);
    
    for (uint16_t addr = 0; addr < EEPROM_SIZE; addr += 32) {
        if (!write(addr, blank, min(32, EEPROM_SIZE - addr))) {
            return false;
        }
        delay(50);
    }
    
    return true;
}

// ============================================
// SALVATAGGIO RITARDATO
// ============================================

void EEPROMManager::requestSave() {
    lastSaveRequest = millis();
    saveDelay = EEPROM_SAVE_DELAY;
    savePending = true;
    
    pendingConfig.current_frequency = displayedFrequency;
    pendingConfig.current_mode = currentMode;
    pendingConfig.current_step = step;
    pendingConfig.agc_fast = agcFastMode;
    pendingConfig.attenuator = attenuatorEnabled;
}

void EEPROMManager::requestQuickSave() {
    lastSaveRequest = millis();
    saveDelay = EEPROM_QUICK_SAVE_DELAY;
    savePending = true;
    
    pendingConfig.current_frequency = displayedFrequency;
    pendingConfig.current_mode = currentMode;
    pendingConfig.current_step = step;
    pendingConfig.agc_fast = agcFastMode;
    pendingConfig.attenuator = attenuatorEnabled;
}

void EEPROMManager::update() {
    if (savePending && (millis() - lastSaveRequest > saveDelay)) {
        saveConfig(pendingConfig);
        savePending = false;
    }
}

bool EEPROMManager::isSavePending() {
    return savePending;
}

// ============================================
// GESTIONE STATO RX
// ============================================

bool EEPROMManager::loadRXState() {
    if (loadConfig(currentConfig)) {
        displayedFrequency = currentConfig.current_frequency;
        currentMode = currentConfig.current_mode;
        step = currentConfig.current_step;
        agcFastMode = currentConfig.agc_fast;
        attenuatorEnabled = currentConfig.attenuator;
        
        updateBFOForMode();
        return true;
    } else {
        setDefaultRXConfig();
        saveConfig(currentConfig);
        
        displayedFrequency = currentConfig.current_frequency;
        currentMode = currentConfig.current_mode;
        step = currentConfig.current_step;
        agcFastMode = currentConfig.agc_fast;
        attenuatorEnabled = currentConfig.attenuator;
        
        updateBFOForMode();
        return false;
    }
}

void EEPROMManager::saveRXState() {
    currentConfig.current_frequency = displayedFrequency;
    currentConfig.current_mode = currentMode;
    currentConfig.current_step = step;
    currentConfig.agc_fast = agcFastMode;
    currentConfig.attenuator = attenuatorEnabled;
    
    saveConfig(currentConfig);
}

void EEPROMManager::setDefaultRXConfig() {
    memset(&currentConfig, 0, sizeof(RXConfig));
    
    currentConfig.current_frequency = 7000000;
    currentConfig.current_mode = MODE_LSB;
    currentConfig.current_step = 1000;
    currentConfig.agc_fast = true;
    currentConfig.attenuator = false;
    
    for (int i = 0; i < 10; i++) {
        currentConfig.memories[i].valid = false;
    }
}

RXConfig& EEPROMManager::getCurrentRXConfig() {
    return currentConfig;
}

// ============================================
// CALIBRAZIONE SI5351
// ============================================

bool EEPROMManager::saveCalibration(long calibration_factor) {
    uint32_t timestamp = millis();
    uint8_t data[12];
    
    memcpy(data, &calibration_factor, 4);
    memset(data + 4, 0, 4);
    memcpy(data + 8, &timestamp, 4);
    
    // // DEBUG: stampa i dati prima di scrivere
    // Serial.print("Salvataggio calibrazione: ");
    // Serial.print(calibration_factor);
    // Serial.print(" su indirizzo ");
    // Serial.println(EEPROM_CALIBRATION);
    
    // for (int i = 0; i < 12; i++) {
    //     Serial.print("0x");
    //     if (data[i] < 16) Serial.print("0");
    //     Serial.print(data[i], HEX);
    //     Serial.print(" ");
    // }
    // Serial.println();
    
    bool success = write(EEPROM_CALIBRATION, data, 12);

        // ATTENDI CHE LA SCRITTURA SIA COMPLETATA
    delay(50);
    
    // VERIFICA LEGGENDO CIÒ CHE È STATO SCRITTO
    uint8_t verify[12];
    if (read(EEPROM_CALIBRATION, verify, 12)) {
        long writtenValue;
        memcpy(&writtenValue, verify, 4);
        if (writtenValue == calibration_factor) {
            Serial.print("✅ Verifica OK: ");
            Serial.println(writtenValue);
        } else {
            Serial.print("⚠️ Verifica fallita: scritto ");
            Serial.print(calibration_factor);
            Serial.print(", letto ");
            Serial.println(writtenValue);
        }
    }
    
    return success;
}

bool EEPROMManager::loadCalibration(long& calibration_factor) {
    uint8_t data[12];
    
    Serial.print("Caricamento calibrazione da indirizzo ");
    Serial.println(EEPROM_CALIBRATION);
    
    if (!read(EEPROM_CALIBRATION, data, 12)) {
        Serial.println("❌ Lettura EEPROM fallita");
        return false;
    }
    
    // DEBUG: stampa i dati letti
    Serial.print("Dati letti: ");
    for (int i = 0; i < 12; i++) {
        Serial.print("0x");
        if (data[i] < 16) Serial.print("0");
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    memcpy(&calibration_factor, data, 4);
    uint32_t timestamp;
    memcpy(&timestamp, data + 8, 4);
    
    Serial.print("Calibrazione caricata: ");
    Serial.print(calibration_factor);
    Serial.print(" (salvata: ");
    Serial.print(timestamp);
    Serial.println(" ms)");
    
    return true;
}