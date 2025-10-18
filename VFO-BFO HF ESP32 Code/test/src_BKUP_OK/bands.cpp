#include "bands.h"
#include "config.h"
#include "DigiOUT.h" 
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// Definizione delle bande
Band bands[] = {
  {"160m", 1830000, 1850000},
  {"80m", 3500000, 3800000},
  {"60m", 5351000, 5366000},
  {"40m", 7000000, 7200000},
  {"30m", 10100000, 10150000},
  {"20m", 14000000, 14350000},
  {"17m", 18068000, 18168000},
  {"15m", 21000000, 21450000},
  {"12m", 24890000, 24990000},
  {"10m", 28000000, 29700000}
};

int currentBandIndex = 3;
int totalBands = sizeof(bands) / sizeof(bands[0]);

int getBandIndex(unsigned long freq) {
  for (int i = 0; i < totalBands; i++) {
    if (freq >= bands[i].startFreq && freq <= bands[i].endFreq) {
      return i;
    }
  }
  return -1;
}

// Cambia alla banda successiva
void changeBand() {
  currentBandIndex = (currentBandIndex + 1) % totalBands;
  displayedFrequency = bands[currentBandIndex].startFreq;
  vfoFrequency = displayedFrequency + IF_FREQUENCY;
  updateModeOutputs();// Aggiorna le uscite digitali
}

// Aggiorna la visualizzazione della banda
void updateBandInfo() {
  static int lastBandIndex = -1;
  
  int bandIndex = getBandIndex(displayedFrequency);
  
  if (bandIndex != lastBandIndex) {
    int boxX = POSITION_X;
    int boxWidth = BOX_WIDTH;
    
    // Pulisci l'area del testo
    tft.fillRect(boxX + 5, 218, boxWidth - 10, 15, BACKGROUND_COLOR);
    
    tft.setTextColor(BAND_COLOR, BACKGROUND_COLOR);
    tft.setTextSize(2);
    
    if (bandIndex >= 0) {
      String bandName = bands[bandIndex].name;
      
      // Calcola la posizione X centrata approssimativa
      int textWidth = bandName.length() * 12;
      int centeredX = boxX + (boxWidth - textWidth) / 2;
      
      // Disegna il testo centrato
      tft.drawString(bandName, centeredX, 218);
      currentBandIndex = bandIndex;
    } else {
      tft.drawString("    ", boxX + 5, 218);
    }
    
    lastBandIndex = bandIndex;
  }
  
  updateModeOutputs();
}