#ifndef MODES_H
#define MODES_H

// ============================================
// ENUM MODALITA'
// ============================================

enum Mode {
    MODE_AM   = 0,   // 00 in binario
    MODE_LSB  = 1,   // 01 in binario
    MODE_USB  = 2,   // 10 in binario
    MODE_CW   = 3,   // 11 in binario
    MODE_COUNT
};

// ============================================
// VARIABILI ESTERNE
// ============================================

extern const char* modeNames[];
extern int currentMode;

// ============================================
// FUNZIONI
// ============================================

void changeMode();
void updateModeInfo();
void updateBFOForMode();

#endif