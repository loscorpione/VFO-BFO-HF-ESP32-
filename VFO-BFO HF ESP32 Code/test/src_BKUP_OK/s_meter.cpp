#include "s_meter.h"
#include "config.h"
#include "display.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

int sMeterValue = 0;
int sMeterPeak = 0;
int previousSValue = -1;
unsigned long lastPeakUpdate = 0;

// Filtro a media mobile per stabilizzare il segnale
const int SMOOTHING_WINDOW = 5;
int rawValues[SMOOTHING_WINDOW];
int valueIndex = 0;
int valueTotal = 0;

void setupSMeter() {
  
  // Inizializza il filtro a media mobile
  for (int i = 0; i < SMOOTHING_WINDOW; i++) {
    rawValues[i] = 0;
  }
  
  // Disegna l'S-meter completo una volta all'inizio
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

void drawSMeterSegment(int segment, bool state) {
  if (segment < 0 || segment >= S_METER_SEGMENTS) return;
  
  int segmentX = S_METER_X + (segment * S_METER_SEGMENT_WIDTH);
  
  // Assicurati che il segmento non vada fuori dall'area
  if (segmentX < S_METER_X) return;
  if (segmentX + S_METER_SEGMENT_WIDTH > S_METER_X + S_METER_WIDTH) return;
  // Determina il colore in base al segmento
  uint16_t segmentColor;
  if (segment < 16) {
    segmentColor = S_METER_LOW_COLOR;      // S1 a S9+30: Verde
  } else {
    segmentColor = S_METER_HIGH_COLOR;     // S9+40 a +60: Rosso
  }
  
  // Disegna il segmento
  if (state) {
    tft.fillRect(segmentX, S_METER_Y, S_METER_SEGMENT_WIDTH - 1, S_METER_HEIGHT, segmentColor);
  } else {
    tft.fillRect(segmentX, S_METER_Y, S_METER_SEGMENT_WIDTH - 1, S_METER_HEIGHT, S_METER_BG_COLOR);
  }
}

void updateSMeter() {
  // Leggi il valore analogico
  int rawValue = analogRead(S_METER_PIN);
  
  // Applica filtro a media mobile
  valueTotal = valueTotal - rawValues[valueIndex];
  rawValues[valueIndex] = rawValue;
  valueTotal = valueTotal + rawValues[valueIndex];
  valueIndex = (valueIndex + 1) % SMOOTHING_WINDOW;
  
  int averageValue = valueTotal / SMOOTHING_WINDOW;
  
  // Converti in valore per 25 segmenti con alta risoluzione
  int newValue = map(constrain(averageValue, 0, 3000), 0, 3000, 0, S_METER_SEGMENTS);
  
  // Aggiorna solo se c'è un cambiamento reale
  if (newValue != sMeterValue) {
    sMeterValue = newValue;
    
    // Aggiorna solo i segmenti che sono cambiati
    if (sMeterValue != previousSValue) {
      // Spegni i segmenti in eccesso
      if (sMeterValue < previousSValue) {
        for (int i = previousSValue; i > sMeterValue-1; i--) {
          drawSMeterSegment(i, false);
        }
      } 
      // Accendi i nuovi segmenti
      else if (sMeterValue > previousSValue) {
        for (int i = previousSValue; i < sMeterValue; i++) {
          drawSMeterSegment(i, true);
        }
      }
      
      previousSValue = sMeterValue;
    }
  }
  
  // Aggiorna il picco
  if (sMeterValue > sMeterPeak) {
    sMeterPeak = sMeterValue;
    lastPeakUpdate = millis();
    
  // Aggiorna immediatamente l'indicatore di picco
    static int previousPeak = -1;
    if (previousPeak > 0 && previousPeak <= S_METER_SEGMENTS) {
      int oldPeakX = S_METER_X + (previousPeak * S_METER_SEGMENT_WIDTH) - (S_METER_SEGMENT_WIDTH );
      tft.fillRect(oldPeakX, S_METER_Y + S_METER_SEGMENT_WIDTH+3, S_METER_SEGMENT_WIDTH-1, 3, BACKGROUND_COLOR); // Cancella il vecchio picco
      tft.fillRect(oldPeakX, S_METER_Y -3, S_METER_SEGMENT_WIDTH-1, 3, BACKGROUND_COLOR); // Cancella il vecchio picco
    }
    
    if (sMeterPeak > 0 && sMeterPeak <= S_METER_SEGMENTS) {
      int peakX = S_METER_X + (sMeterPeak * S_METER_SEGMENT_WIDTH) - (S_METER_SEGMENT_WIDTH );
      tft.fillRect(peakX, S_METER_Y + S_METER_SEGMENT_WIDTH+3, S_METER_SEGMENT_WIDTH-1, 3, TFT_WHITE); // Disegna il nuovo picco
      tft.fillRect(peakX, S_METER_Y -3, S_METER_SEGMENT_WIDTH-1, 3, TFT_WHITE); // Disegna il nuovo picco
    }
    
    previousPeak = sMeterPeak;
  }

  // Reset del picco dopo 150 millisecondi
  if (millis() - lastPeakUpdate > 150) {
    sMeterPeak = max(0, sMeterPeak - 1);
    lastPeakUpdate = millis();
  }
  
 
}