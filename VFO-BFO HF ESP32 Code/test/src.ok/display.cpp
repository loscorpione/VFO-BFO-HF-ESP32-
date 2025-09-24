#include "display.h"
#include "config.h"
#include "bands.h"
#include "modes.h"

TFT_eSPI tft;

void drawDisplayLayout() {
  // Riquadro principale frequenza (in alto) - Arrotondato
  tft.fillRoundRect(5, 5, 230, 90, 10, BACKGROUND_COLOR);
  tft.drawRoundRect(5, 5, 230, 90, 10, BORDER_COLOR);
  tft.drawRoundRect(6, 6, 228, 88, 10, BORDER_COLOR);
  
  // Riquadro step (in basso a destra) - Arrotondato
  tft.fillRoundRect(240, 55, 75, 35, 8, BACKGROUND_COLOR);
  tft.drawRoundRect(240, 55, 75, 35, 8, BORDER_COLOR);
  tft.drawRoundRect(241, 56, 73, 33, 8, BORDER_COLOR);
  
  // Riquadro banda (in basso a sinistra) - Arrotondato
  tft.fillRoundRect(5, 100, 65, 40, 5, BACKGROUND_COLOR);
  tft.drawRoundRect(5, 100, 65, 40, 5, BORDER_COLOR);
  
  // Riquadro modalità (in basso al centro) - Arrotondato
  tft.fillRoundRect(80, 100, 70, 40, 5, BACKGROUND_COLOR);
  tft.drawRoundRect(80, 100, 70, 40, 5, BORDER_COLOR);
  
  // Etichetta Frequency
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(1);
  tft.drawString("FREQUENCY", 10, 15);
  
  // Etichetta Step
  tft.drawString("STEP", 250, 40);
  
  // Etichetta Band
  tft.drawString("BAND", 10, 108);
  
  // Etichetta Mode
  tft.drawString("MODE", 85, 108);
  
  // Disegna il layout dell'S-meter
  drawSMeterLayout();
}

void updateFrequencyDisplay() {
  String freqStr = formatFrequency(displayedFrequency);
  
  tft.fillRect(10, 30, 220, 55, BACKGROUND_COLOR);
  tft.setTextColor(FREQUENCY_COLOR, BACKGROUND_COLOR);
  tft.setTextSize(4);
  
  int textWidth = freqStr.length() * 24;
  int xPos = 10 + (220 - textWidth) / 2;
  if (xPos < 10) xPos = 10;
  tft.drawString(freqStr, xPos, 35);
}

void updateStepDisplay() {
  tft.fillRoundRect(245, 60, 65, 25, 5, BACKGROUND_COLOR);
  tft.setTextColor(STEP_COLOR, BACKGROUND_COLOR);
  tft.setTextSize(2);
  
  String stepStr;
  if (step == 10) stepStr = "10Hz";
  else if (step == 100) stepStr = "100Hz";
  else if (step == 1000) stepStr = "1kHz";
  else if (step == 10000) stepStr = "10kHz";
  
  int textWidth = stepStr.length() * 12;
  int xPos = 245 + (65 - textWidth) / 2;
  tft.drawString(stepStr, xPos, 63);
}

String formatFrequency(unsigned long freq) {
  String result;
  
  if (freq >= 1000000) {
    unsigned long mhz = freq / 1000000;
    unsigned long hz = freq % 1000000;
    unsigned long khz_part = hz / 1000;
    unsigned long hz_part = hz % 1000;
    
    result = String(mhz) + ".";
    if (khz_part < 100) result += "0";
    if (khz_part < 10) result += "0";
    result += String(khz_part);
    result += ".";
    unsigned short last_two_digits = hz_part / 10;
    if (last_two_digits < 10) result += "0";
    result += String(last_two_digits);
  } else {
    unsigned long khz = freq / 1000;
    unsigned long hz = freq % 1000;
    result = String(khz) + ".";
    unsigned short decimal_part = hz / 10;
    if (decimal_part < 10) result += "0";
    result += String(decimal_part);
  }
  
  return result;
}

void drawSMeterLayout() {
  // Riquadro S-meter (in basso a destra)
  tft.fillRoundRect(160, 100, 155, 40, 5, BACKGROUND_COLOR);
  tft.drawRoundRect(160, 100, 155, 40, 5, BORDER_COLOR);
  
  // Etichetta S-meter
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(1);
  tft.drawString("S-METER", 165, 108);
}

void updateSMeter(int value) {
  static int lastMeterWidth = -1; // Memorizza l'ultima larghezza della barra
  
  // Mappa il valore analogico (0-4095) alla larghezza del display (0-150)
  int meterWidth = map(value, 0, 4095, 0, 150);
  
  // Limita la larghezza massima
  if (meterWidth > 150) meterWidth = 150;
  
  // Aggiorna solo se la larghezza è cambiata in modo significativo (per evitare flicker)
  if (abs(meterWidth - lastMeterWidth) < 2) {
    return; // Cambiamento troppo piccolo, non aggiornare
  }
  
  // Determina il colore in base alla soglia S9
  uint16_t meterColor = (value < S9_THRESHOLD) ? TFT_GREEN : TFT_RED;
  
  // Se la barra si sta restringendo, cancella la parte in eccesso
  if (meterWidth < lastMeterWidth) {
    tft.fillRect(165 + meterWidth, 118, lastMeterWidth - meterWidth, 15, BACKGROUND_COLOR);
  }
  // Se la barra si sta espandendo, disegna la nuova parte
  else if (meterWidth > lastMeterWidth) {
    tft.fillRect(165 + lastMeterWidth, 118, meterWidth - lastMeterWidth, 15, meterColor);
  }
  
  // Se il colore è cambiato, ridisegna l'intera barra (raro)
  static uint16_t lastMeterColor = TFT_GREEN;
  if (meterColor != lastMeterColor) {
    tft.fillRect(165, 118, meterWidth, 15, meterColor);
    lastMeterColor = meterColor;
  }
  
  lastMeterWidth = meterWidth;
  
  // Mostra il valore numerico (aggiorna solo se cambiato in modo significativo)
  static int lastDisplayValue = -1;
  if (abs(value - lastDisplayValue) > 10) { // Soglia per aggiornare il numero
    tft.fillRect(165, 135, 50, 15, BACKGROUND_COLOR);
    tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    tft.setTextSize(1);
    tft.drawString("S:" + String(value), 165, 135);
    lastDisplayValue = value;
  }
}