#include "display.h"
#include "config.h"
#include "bands.h"
#include "modes.h"
#include "s_meter.h"
#include "PLL.h"

TFT_eSPI tft;
TFT_eSprite freqSprite = TFT_eSprite(&tft);  // Definisci lo Sprite

// Variabili per l'aggiornamento della frequenza
static String lastFreqStr = "";
static int lastSpriteWidth = 0;
static int lastSpriteX = 0;

// Disegna lo sprite della frequenza
void setupFrequencySprite() {
  // Crea uno sprite di 250x60 pixel (abbastanza grande per la frequenza)
  freqSprite.setColorDepth(8);
  freqSprite.createSprite(250, 60);
  freqSprite.fillSprite(BACKGROUND_COLOR);
  freqSprite.setTextColor(FREQUENCY_COLOR, BACKGROUND_COLOR);
  freqSprite.setTextFont(7);
  freqSprite.setTextSize(0);
}

// Disegna il display BFO
void drawBFODisplay() {
  static unsigned long lastBFOFreq = 0;
  static bool lastBFOEnabled = false;
  
  if (abs((long)(bfoFrequency - lastBFOFreq)) > 100 || bfoEnabled != lastBFOEnabled) {
    tft.fillRect(BFO_DISPLAY_X-60, BFO_DISPLAY_Y, BFO_DISPLAY_WIDTH+60, BFO_DISPLAY_HEIGHT, BACKGROUND_COLOR);
    
    if (bfoEnabled) {
      // Disegna il grafico della frequenza BFO
      tft.drawFastHLine(BFO_GRAPH_X, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT/2, BFO_GRAPH_WIDTH, TFT_WHITE);

      // Calcola la posizione del marcatore
      int markerPos = map(bfoFrequency, 453000, 457000, BFO_GRAPH_X, BFO_GRAPH_X + BFO_GRAPH_WIDTH);
      markerPos = constrain(markerPos, BFO_GRAPH_X, BFO_GRAPH_X + BFO_GRAPH_WIDTH);
      
      // Disegna il marcatore della frequenza BFO
      tft.drawFastVLine(markerPos, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_GREEN);
      tft.drawFastVLine(markerPos+1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_GREEN);
      
      // Disegna il marcatore centrale
      tft.drawFastVLine(BFO_GRAPH_X + BFO_GRAPH_WIDTH/2-1, BFO_GRAPH_Y-(BFO_CENTER_MARKER_HEIGHT-BFO_GRAPH_HEIGHT)/2, BFO_CENTER_MARKER_HEIGHT, TFT_RED);
      //tft.drawFastVLine(BFO_GRAPH_X + BFO_GRAPH_WIDTH/2+1, BFO_GRAPH_Y-(BFO_CENTER_MARKER_HEIGHT-BFO_GRAPH_HEIGHT)/2, BFO_CENTER_MARKER_HEIGHT, TFT_RED);
      tft.drawFastVLine(BFO_GRAPH_X + BFO_GRAPH_WIDTH/2, BFO_GRAPH_Y-(BFO_CENTER_MARKER_HEIGHT-BFO_GRAPH_HEIGHT)/2, BFO_CENTER_MARKER_HEIGHT, TFT_RED);

      tft.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
      tft.setTextSize(1);
      tft.setCursor(BFO_DISPLAY_X+BFO_DISPLAY_WIDTH/2-30, BFO_DISPLAY_Y+5);
      tft.print(bfoFrequency / 1000.0, 2);

      tft.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
      tft.setTextSize(2);
      tft.drawString("BFO:", BFO_DISPLAY_X-60, BFO_DISPLAY_Y+15);
      
      // Disegna le etichette del grafico
      tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
      tft.setTextSize(1);
      tft.setCursor(BFO_GRAPH_X, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 2);
      tft.print("453");
      tft.setCursor(BFO_GRAPH_X + BFO_GRAPH_WIDTH/2 - 8, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 2);
      tft.print("455");
      tft.setCursor(BFO_GRAPH_X + BFO_GRAPH_WIDTH - 18, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 2);
      tft.print("457");
    }
    
    lastBFOFreq = bfoFrequency;
    lastBFOEnabled = bfoEnabled;
  }
}
// Aggiorna la visualizzazione della frequenza
void updateFrequencyDisplay() {
  String freqStr = formatFrequency(displayedFrequency); 
  
  // Aggiorna solo se la stringa è cambiata
  if (freqStr != lastFreqStr) {

    // Calcola la posizione in base alla lunghezza
    int xPos;
    int textWidth = freqStr.length() * 24;
    
    if (textWidth == 216) xPos = 0;       // 9 caratteri
    else if (textWidth == 192) xPos = 32; // 8 caratteri
    else xPos = 32; // Default
    
    // Pulisci lo sprite
    freqSprite.fillSprite(BACKGROUND_COLOR);
    
    // Disegna la frequenza sullo sprite
    freqSprite.drawString(freqStr, xPos, 5);
    
    // Calcola la posizione di destinazione sul display
    int destX = 25; // Posizione fissa sul display
    int destY = 30;
    
    // Push dello sprite sul display (senza transparenza, tutto opaco)
    freqSprite.pushSprite(destX, destY);
    
    lastFreqStr = freqStr;
    lastSpriteWidth = textWidth;
    lastSpriteX = xPos;
  }
}

// Aggiorna la visualizzazione dello step
void updateStepDisplay() {
  static String lastStepStr = "";
  
  String stepStr;
  if (step == 10) stepStr = "10Hz";
  else if (step == 100) stepStr = "100Hz";
  else if (step == 1000) stepStr = "1kHz";
  else if (step == 10000) stepStr = "10kHz";
  
  if (stepStr != lastStepStr) {
    tft.fillRect(260, 100, 85, 25, BACKGROUND_COLOR);
    tft.setTextColor(STEP_COLOR, BACKGROUND_COLOR);
    tft.setTextSize(2);
    
    tft.drawString(stepStr, 255, 100);
    lastStepStr = stepStr;
  }
}

// Formatta la frequenza in una stringa leggibile
String formatFrequency(unsigned long freq) {
  static char buffer[12];
  
  if (freq >= 1000000) {
    unsigned long mhz = freq / 1000000;
    unsigned long hz = freq % 1000000;
    unsigned long khz_part = hz / 1000;
    unsigned long hz_part = hz % 1000;
    
    snprintf(buffer, sizeof(buffer), "%lu.%03lu.%02lu", mhz, khz_part, hz_part / 10);
  } else {
    unsigned long khz = freq / 1000;
    unsigned long hz = freq % 1000;
    snprintf(buffer, sizeof(buffer), "%lu.%02lu", khz, hz / 10);
  }
  
  return String(buffer);
}

// Disegna il layout iniziale del display
void drawDisplayLayout() {        
  // Prima inizializza lo sprite
  setupFrequencySprite();

  // Riquadri con etichetta (banda, modalità, AGC, ATT)
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

    String TEXT = LABELS[i];
    int textWidth = TEXT.length() * 5;
    int textX = x + (BOX_WIDTH - textWidth) / 2;
    int textY = y + 5;
    tft.drawString(TEXT, textX, textY);
  }

  tft.setTextSize(2);
  tft.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
  tft.drawString("MHz", 280, 70);
  
  tft.setTextColor(TFT_RED, BACKGROUND_COLOR);
  tft.drawString("STEP:", 195, 100);
  
  setupSMeter();
  
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
      tft.drawString("+20", segmentX - 2, S_METER_Y + S_METER_HEIGHT + 3);
    } else if (i == 21) {
      tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
      tft.drawString("+40", segmentX - 2, S_METER_Y + S_METER_HEIGHT + 3);
    } else if (i == 24) {
      tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
      tft.drawString("+60", segmentX - 2, S_METER_Y + S_METER_HEIGHT + 3);
    }
  }
  
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  drawBFODisplay();
  
  // Inizializza la visualizzazione della frequenza
  lastFreqStr = ""; // Forza l'aggiornamento iniziale
  updateFrequencyDisplay();
}