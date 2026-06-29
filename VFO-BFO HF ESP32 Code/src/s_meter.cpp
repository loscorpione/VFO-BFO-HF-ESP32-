#include "s_meter.h"
#include "config.h"
#include "display.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// ============================================
// VARIABILI GLOBALI
// ============================================

int sMeterValue = 0;
int sMeterPeak = 0;
int previousSValue = -1;
unsigned long lastPeakUpdate = 0;

// Filtro a media mobile
const int SMOOTHING_WINDOW = 5;
int rawValues[SMOOTHING_WINDOW];
int valueIndex = 0;
int valueTotal = 0;

// ============================================
// INIZIALIZZAZIONE
// ============================================

void setupSMeter() {
    // Inizializza il filtro a media mobile
    for (int i = 0; i < SMOOTHING_WINDOW; i++) {
        rawValues[i] = 0;
    }
    
    // Disegna l'S-meter completo
    tft.fillRect(S_METER_X, S_METER_Y - 15, S_METER_WIDTH, S_METER_HEIGHT + 35, BACKGROUND_COLOR);
    
    // Disegna tutti i segmenti spenti
    for (int i = 0; i < S_METER_SEGMENTS; i++) {
        drawSMeterSegment(i, false);
    }
    
    // Disegna le etichette
    tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    tft.setTextSize(1);
    tft.drawString("S-METER", S_METER_X, S_METER_Y - 13);
}

// ============================================
// DISEGNO SEGMENTI
// ============================================

void drawSMeterSegment(int segment, bool state) {
    if (segment < 0 || segment >= S_METER_SEGMENTS) return;
    
    int segmentX = S_METER_X + (segment * S_METER_SEGMENT_WIDTH);
    
    if (segmentX < S_METER_X) return;
    if (segmentX + S_METER_SEGMENT_WIDTH > S_METER_X + S_METER_WIDTH) return;
    
    // Colore in base al segmento
    uint16_t segmentColor;
    if (segment < 16) {
        segmentColor = S_METER_LOW_COLOR;   // S1 a S9+30: Verde
    } else {
        segmentColor = S_METER_HIGH_COLOR;  // S9+40 a +60: Rosso
    }
    
    if (state) {
        tft.fillRect(segmentX, S_METER_Y, S_METER_SEGMENT_WIDTH - 1, S_METER_HEIGHT, segmentColor);
    } else {
        tft.fillRect(segmentX, S_METER_Y, S_METER_SEGMENT_WIDTH - 1, S_METER_HEIGHT, S_METER_BG_COLOR);
    }
}

// ============================================
// AGGIORNAMENTO S-METER
// ============================================

void updateSMeter() {
    int rawValue = analogRead(S_METER_PIN);
    
    // Filtro a media mobile
    valueTotal = valueTotal - rawValues[valueIndex];
    rawValues[valueIndex] = rawValue;
    valueTotal = valueTotal + rawValues[valueIndex];
    valueIndex = (valueIndex + 1) % SMOOTHING_WINDOW;
    
    int averageValue = valueTotal / SMOOTHING_WINDOW;
    
    // Converti in 25 segmenti (range 0-2000 regolabile per sensibilità)
    int newValue = map(constrain(averageValue, 0, 2000), 0, 2000, 0, S_METER_SEGMENTS);
    
    // Aggiorna segmenti se cambiato
    if (newValue != sMeterValue) {
        sMeterValue = newValue;
        
        if (sMeterValue != previousSValue) {
            // Spegni segmenti in eccesso
            if (sMeterValue < previousSValue) {
                for (int i = previousSValue; i > sMeterValue - 1; i--) {
                    drawSMeterSegment(i, false);
                }
            }
            // Accendi nuovi segmenti
            else if (sMeterValue > previousSValue) {
                for (int i = previousSValue; i < sMeterValue; i++) {
                    drawSMeterSegment(i, true);
                }
            }
            
            previousSValue = sMeterValue;
        }
    }
    
    // Gestione picco
    if (sMeterValue > sMeterPeak) {
        sMeterPeak = sMeterValue;
        lastPeakUpdate = millis();
        
        static int previousPeak = -1;
        
        // Cancella vecchio picco
        if (previousPeak > 0 && previousPeak <= S_METER_SEGMENTS) {
            int oldPeakX = S_METER_X + (previousPeak * S_METER_SEGMENT_WIDTH) - S_METER_SEGMENT_WIDTH;
            tft.fillRect(oldPeakX, S_METER_Y + S_METER_SEGMENT_WIDTH + 3, S_METER_SEGMENT_WIDTH - 1, 3, BACKGROUND_COLOR);
            tft.fillRect(oldPeakX, S_METER_Y - 3, S_METER_SEGMENT_WIDTH - 1, 3, BACKGROUND_COLOR);
        }
        
        // Disegna nuovo picco
        if (sMeterPeak > 0 && sMeterPeak <= S_METER_SEGMENTS) {
            int peakX = S_METER_X + (sMeterPeak * S_METER_SEGMENT_WIDTH) - S_METER_SEGMENT_WIDTH;
            tft.fillRect(peakX, S_METER_Y + S_METER_SEGMENT_WIDTH + 3, S_METER_SEGMENT_WIDTH - 1, 3, TFT_WHITE);
            tft.fillRect(peakX, S_METER_Y - 3, S_METER_SEGMENT_WIDTH - 1, 3, TFT_WHITE);
        }
        
        previousPeak = sMeterPeak;
    }
    
    // Reset picco dopo 150ms
    if (millis() - lastPeakUpdate > 150) {
        sMeterPeak = max(0, sMeterPeak - 1);
        lastPeakUpdate = millis();
    }
}