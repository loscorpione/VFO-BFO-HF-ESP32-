#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Wire.h>
#include <Arduino.h>
#include "config.h"

// ============================================
// STRUTTURE DATI
// ============================================

// Canale di memoria
struct MemoryChannel {
    uint32_t frequency;
    uint8_t mode;
    char label[16];
    bool valid;
};

// Configurazione principale RX
struct RXConfig {
    // Impostazioni correnti
    uint32_t current_frequency;
    uint8_t current_mode;
    uint32_t current_step;
    bool agc_fast;
    bool attenuator;
    
    // Calibrazione SI5351
    // int32_t calibration_factor;
    // int32_t freq_offset;
    // uint32_t last_calibration;
    
    // Memorizzazioni
    MemoryChannel memories[10];
    
    // Verifica integrità
    uint8_t checksum;
};

// ============================================
// CLASSE EEPROM_MANAGER
// ============================================

class EEPROMManager {
public:
    // Funzioni di base
    void begin();
    bool saveConfig(const RXConfig& config);
    bool loadConfig(RXConfig& config);
    bool saveCalibration(long calibration_factor);
    bool loadCalibration(long& calibration_factor);
    bool saveMemory(uint8_t slot, const MemoryChannel& memory);
    bool loadMemory(uint8_t slot, MemoryChannel& memory);
    bool formatEEPROM();
    
    // Funzioni per salvataggio ritardato
    void requestSave();
    void requestQuickSave();
    void update();
    bool isSavePending();
    
    // Funzioni per gestione stato RX
    bool loadRXState();
    void saveRXState();
    void setDefaultRXConfig();
    RXConfig& getCurrentRXConfig();

private:
    bool write(uint16_t address, const uint8_t* data, uint16_t len);
    bool read(uint16_t address, uint8_t* data, uint16_t len);
    uint8_t calculateChecksum(const uint8_t* data, size_t len);
    bool verifyChecksum(const uint8_t* data, size_t len, uint8_t checksum);
    
    // Variabili per salvataggio ritardato
    unsigned long lastSaveRequest = 0;
    unsigned long saveDelay = EEPROM_SAVE_DELAY;
    bool savePending = false;
    RXConfig pendingConfig;
    RXConfig currentConfig;
};

extern EEPROMManager eepromManager;

#endif