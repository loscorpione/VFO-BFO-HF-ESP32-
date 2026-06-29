#ifndef BANDS_H
#define BANDS_H

// ============================================
// STRUTTURA BANDA
// ============================================

struct Band {
    const char* name;
    unsigned long startFreq;
    unsigned long endFreq;
};

// ============================================
// VARIABILI ESTERNE
// ============================================

extern Band bands[];
extern int currentBandIndex;
extern int totalBands;

// ============================================
// FUNZIONI
// ============================================

int getBandIndex(unsigned long freq);
void changeBand();
void updateBandInfo();

#endif