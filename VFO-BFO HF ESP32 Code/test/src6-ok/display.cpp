#include "display.h"
#include "config.h"
#include "bands.h"
#include "modes.h"
#include "s_meter.h"
#include "PLL.h"

TFT_eSPI tft;

void drawBFODisplay() {
  // Pulisci l'area del display BFO
  tft.fillRect(BFO_DISPLAY_X, BFO_DISPLAY_Y, BFO_DISPLAY_WIDTH, BFO_DISPLAY_HEIGHT, BACKGROUND_COLOR);
  
  // Disegna il grafico solo se BFO è attivo
  if (bfoEnabled) {
    // Disegna la linea di base del grafico
    tft.drawFastHLine(BFO_GRAPH_X, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT/2, BFO_GRAPH_WIDTH, TFT_WHITE);
    
    // Calcola la posizione della linea verticale in base alla frequenza BFO
    int markerPos = map(bfoFrequency, 452000, 458000, BFO_GRAPH_X, BFO_GRAPH_X + BFO_GRAPH_WIDTH);
    markerPos = constrain(markerPos, BFO_GRAPH_X, BFO_GRAPH_X + BFO_GRAPH_WIDTH);
    
    // Disegna la linea verticale
    tft.drawFastVLine(markerPos, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_GREEN);
    
    // Disegna il punto centrale
    tft.fillCircle(BFO_GRAPH_X + BFO_GRAPH_WIDTH/2, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT/2, 2, TFT_YELLOW);
    
    // Mostra la frequenza BFO
    tft.setTextColor(TFT_CYAN, BACKGROUND_COLOR);
    tft.setTextSize(1);
    tft.setCursor(BFO_DISPLAY_X, BFO_DISPLAY_Y);
    tft.print("BFO: ");
    tft.print(bfoFrequency / 1000.0, 2);
    tft.print(" kHz");
    
    // Etichette sul grafico
    tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    tft.setTextSize(1);
    tft.setCursor(BFO_GRAPH_X - 15, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 5);
    tft.print("452");
    tft.setCursor(BFO_GRAPH_X + BFO_GRAPH_WIDTH/2 - 5, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 5);
    tft.print("455");
    tft.setCursor(BFO_GRAPH_X + BFO_GRAPH_WIDTH - 15, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 5);
    tft.print("458");
  }
}

void drawDisplayLayout() {
  // Riquadro principale frequenza (in alto) - Arrotondato
   tft.fillRect(5, 30, 245, 55, BACKGROUND_COLOR);

  // Riquadri con etichetta  (banda, modalità, AGC, ATT)
  const char *LABELS[] = {"BAND", "MODE", "AGC", "ATT"};
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextFont(1);
  tft.setTextSize(TEXT_SIZE_TITLE);

  for (int i = 0; i < BOX_NUM; i++) {
    int x = POSITION_X + i * (BOX_WIDTH + BOX_SPACING);
    int y = POSITION_Y;
    tft.fillRoundRect(x, y, BOX_WIDTH, BOX_HEIGHT, BOX_RADIUS, BACKGROUND_COLOR);
    tft.drawRoundRect(x, y, BOX_WIDTH, BOX_HEIGHT, BOX_RADIUS, BORDER_COLOR);
    tft.drawRoundRect(x+1, y+1, BOX_WIDTH-2, BOX_HEIGHT-2, BOX_RADIUS, BORDER_COLOR);

    // Centra il testo
    String TEXT = LABELS[i];
    int textWidth = TEXT.length() * 5;
    int textX = x + (BOX_WIDTH - textWidth) / 2;
    int textY = y + 5; // Posiziona il testo vicino alla parte superiore del riquadro
    tft.drawString(TEXT, textX, textY);
  }

  // Misura Font etichetta   
  tft.setTextSize(2);

  // Etichetta Unita di misura Frequenza
  tft.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
  tft.drawString("Mhz", 280, 62);
  
  // Etichetta Step
  tft.setTextColor(TFT_RED, BACKGROUND_COLOR);
  tft.drawString("STEP:", 10, 100);
  
  // Inizializza S-meter
  setupSMeter();
  
  // Aggiungi le etichette S-unit (senza valore numerico)
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(1);
  
  for (int i = 0; i < S_METER_SEGMENTS; i++) {
    int segmentX = S_METER_X + (i * S_METER_SEGMENT_WIDTH);
    
    if (i == 0) tft.drawString("S", segmentX, S_METER_Y + S_METER_HEIGHT + 7);
    else if (i == 3) tft.drawString("1", segmentX, S_METER_Y + S_METER_HEIGHT + 7);
    else if (i == 6) tft.drawString("3", segmentX, S_METER_Y + S_METER_HEIGHT + 7);
    else if (i == 9) tft.drawString("5", segmentX, S_METER_Y + S_METER_HEIGHT + 7);
    else if (i == 12) tft.drawString("7", segmentX, S_METER_Y + S_METER_HEIGHT + 7);
    else if (i == 15) tft.drawString("9", segmentX, S_METER_Y + S_METER_HEIGHT + 7);
    else if (i == 18) {
      tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
      tft.drawString("+20", segmentX - 2, S_METER_Y + S_METER_HEIGHT + 7);
    } else if (i == 21) {
      tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
      tft.drawString("+40", segmentX - 2, S_METER_Y + S_METER_HEIGHT + 7);
    } else if (i == 24) {
      tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
      tft.drawString("+60", segmentX - 2, S_METER_Y + S_METER_HEIGHT + 7);
    }
  }
  
  // Ripristina il colore del testo
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);

  // Disegna il display BFO (inizialmente vuoto se BFO non attivo)
  drawBFODisplay();

}

void updateFrequencyDisplay() {
  String freqStr = formatFrequency(displayedFrequency); 
  tft.fillRect(10, 30, 265, 55, BACKGROUND_COLOR);
  tft.setTextColor(FREQUENCY_COLOR, BACKGROUND_COLOR);
  tft.setTextFont(7);
  tft.setTextSize(0);

  int xPos;
  int textWidth = freqStr.length() * 24;
  if (textWidth == 216) xPos = 25;      // se il testo è lungo 9 caratteri stampa da x=25
  else if (textWidth == 192) xPos = 57; // se il testo è lungo 8 caratteri stampa da x=57
  tft.drawString(freqStr, xPos, 30);
  
  tft.setTextFont(1);
  tft.setTextSize(2);
}

void updateStepDisplay() {
  tft.fillRect(70, 100, 85, 25, BACKGROUND_COLOR);
  tft.setTextColor(STEP_COLOR, BACKGROUND_COLOR);
  tft.setTextSize(2);
  
  String stepStr;
  if (step == 10) stepStr = "10Hz";
  else if (step == 100) stepStr = "100Hz";
  else if (step == 1000) stepStr = "1kHz";
  else if (step == 10000) stepStr = "10kHz";
  
  int textWidth = stepStr.length() * 12;
  int xPos = 70; 
  tft.drawString(stepStr, xPos, 100);
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