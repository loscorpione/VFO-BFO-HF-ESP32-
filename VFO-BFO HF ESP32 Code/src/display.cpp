#include "display.h"
#include "config.h"
#include "bands.h"
#include "modes.h"
#include "s_meter.h"
#include "PLL.h"


TFT_eSPI tft; // Definisci l'oggetto TFT_eSPI

//################################ Layout Iniziale #####################################
// Disegna il layout iniziale del display
void drawDisplayLayout() {        
  // Prima inizializza lo sprite
  setupFrequencySprite();

  // Riquadri  (banda, modalità, AGC, ATT)
  const char *LABELS[] = {"BAND", "MODE", "AGC", "ATT"};
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextFont(1);
  tft.setTextSize(TEXT_SIZE_TITLE);

  // Disegna i riquadri con etichetta
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

  // Scrivi l'unità di Frequenza
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
  tft.drawString("MHz", 280, 70);
  
  // Scrivi l'unità di Step
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(1);
  tft.drawString("STEP", 265, 90);
  
  // Inizializza l'S-meter
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
  
/*   // Inizializza la visualizzazione della frequenza
  lastFreqStr = ""; // Forza l'aggiornamento iniziale
  updateFrequencyDisplay(); */
}

//################################ Grafica Frequenza #####################################
// Definisci lo Sprite della frequenza
TFT_eSprite freqSprite = TFT_eSprite(&tft);  

// Variabili per l'aggiornamento della frequenza
static String lastFreqStr = "";
static int lastSpriteWidth = 0;
static int lastSpriteX = 0;

// Aggiorna la visualizzazione della frequenza VFO
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

//############################# Grafica Step #####################################
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
    
    tft.drawString(stepStr, 255, 105);
    lastStepStr = stepStr;
  }
}

//################################ Grafica BFO #####################################
// Variabili per il BFO
static bool bfoDisplayInitialized = false;
static bool lastBFOEnabled = false;
static unsigned long lastBFOFreq = 0;

// Disegna gli elementi statici del BFO
void drawBFOStaticElements() {
  tft.fillRect(BFO_DISPLAY_X-60, BFO_DISPLAY_Y, BFO_DISPLAY_WIDTH+75, BFO_DISPLAY_HEIGHT, BACKGROUND_COLOR);
  
  // Disegna la scritta "BFO: e kHz"
  tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
  tft.setTextSize(2);
  tft.drawString("BFO:", BFO_DISPLAY_X-60, BFO_DISPLAY_Y+15);
  tft.drawString("kHz", BFO_DISPLAY_X+BFO_GRAPH_WIDTH-5, BFO_DISPLAY_Y+15);
  
  // Disegna il grafico della frequenza BFO
  tft.drawFastHLine(BFO_GRAPH_X, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT/2, BFO_GRAPH_WIDTH, TFT_WHITE);
  
  // Disegna il marcatore centrale (ROSSO) - CORREZIONE
  int centerX = BFO_GRAPH_X + BFO_GRAPH_WIDTH/2;
  tft.drawFastVLine(centerX-1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);
  tft.drawFastVLine(centerX, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);
  tft.drawFastVLine(centerX+1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);

  // Disegna le etichette del grafico
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(1);
  tft.setCursor(BFO_GRAPH_X, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 2);
  tft.print("453");
  tft.setCursor(BFO_GRAPH_X + BFO_GRAPH_WIDTH/2 - 8, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 2);
  tft.print("455");
  tft.setCursor(BFO_GRAPH_X + BFO_GRAPH_WIDTH - 18, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 2);
  tft.print("457");
  
  bfoDisplayInitialized = true;
}

// Aggiorna solo gli elementi dinamici del BFO
void updateBFODynamicElements() {
  if (!bfoEnabled) return;
  
  // Pulisci solo l'area della frequenza visualizzata
  tft.fillRect(BFO_DISPLAY_X, BFO_DISPLAY_Y, BFO_DISPLAY_WIDTH-100, 20, BACKGROUND_COLOR);
  
  // Pulisci l'area del grafico (solo la parte del marker verde)
  // MA mantieni la linea bianca e il marcatore centrale
  tft.fillRect(BFO_GRAPH_X, BFO_GRAPH_Y, BFO_GRAPH_WIDTH, BFO_GRAPH_HEIGHT, BACKGROUND_COLOR);
  
  // Ridisegna la linea orizzontale del grafico
  tft.drawFastHLine(BFO_GRAPH_X, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT/2, BFO_GRAPH_WIDTH, TFT_WHITE);
  
  // Ridisegna il marcatore centrale (ROSSO)
  int centerX = BFO_GRAPH_X + BFO_GRAPH_WIDTH/2;
  tft.drawFastVLine(centerX-1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);
  tft.drawFastVLine(centerX, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);
  tft.drawFastVLine(centerX+1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);

  // Calcola la posizione del marcatore VERDE
  int markerPos = map(bfoFrequency, 453000, 457000, BFO_GRAPH_X, BFO_GRAPH_X + BFO_GRAPH_WIDTH);
  markerPos = constrain(markerPos, BFO_GRAPH_X, BFO_GRAPH_X + BFO_GRAPH_WIDTH);
  
  // Disegna il marcatore della frequenza BFO (VERDE)
  tft.drawFastVLine(markerPos-1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_GREEN);
  tft.drawFastVLine(markerPos, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_GREEN);
  tft.drawFastVLine(markerPos+1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_GREEN);
  
  // Visualizza frequenza BFO con 3 cifre decimali - CORREZIONE
  tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
  tft.setTextSize(1);
  //tft.setCursor(BFO_DISPLAY_X+BFO_DISPLAY_WIDTH/2-30, BFO_DISPLAY_Y+5);
  tft.setCursor(BFO_GRAPH_X+BFO_GRAPH_WIDTH/2-21, BFO_DISPLAY_Y+5);

  // Visualizza frequenza BFO con tutte e 3 le cifre decimali
  unsigned long khz = bfoFrequency / 1000;
  unsigned long hz = bfoFrequency % 1000;
  tft.print(khz);
  tft.print(".");
  // Aggiungi zeri iniziali se necessario
  if (hz < 100) tft.print("0");
  if (hz < 10) tft.print("0");
  tft.print(hz);
}

// Disegna il display BFO
void drawBFODisplay() {
  // Se il BFO è stato disabilitato o è la prima volta, ridisegna tutto
  if (!bfoDisplayInitialized || bfoEnabled != lastBFOEnabled) {
    if (bfoEnabled) {
      drawBFOStaticElements();
      updateBFODynamicElements();
    } else {
      // Se il BFO è disabilitato, pulisci l'area
      tft.fillRect(BFO_DISPLAY_X-60, BFO_DISPLAY_Y, BFO_DISPLAY_WIDTH+75, BFO_DISPLAY_HEIGHT, BACKGROUND_COLOR);
      bfoDisplayInitialized = false;
    }
    lastBFOEnabled = bfoEnabled;
  }
  
  // Aggiorna gli elementi dinamici solo se il BFO è abilitato e la frequenza è cambiata
  if (bfoEnabled && abs((long)(bfoFrequency - lastBFOFreq)) > 0) { // Aggiorna per ogni variazione
    updateBFODynamicElements();
    lastBFOFreq = bfoFrequency;
  }
}




