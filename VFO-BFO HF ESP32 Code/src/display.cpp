#include "display.h"
#include "config.h"
#include "bands.h"
#include "modes.h"
#include "s_meter.h"
#include "PLL.h"
#include "logo80x80.h"   // <-- Includi il logo

TFT_eSPI tft;
TFT_eSprite freqSprite(&tft);

//################################ Disegna SplashScreen #####################################
void drawSplashScreen() {
  tft.fillScreen(BACKGROUND_COLOR);
  
  const int logoX = 10;
  const int logoY = 10;
  
  tft.setSwapBytes(true);
  tft.pushImage(logoX, logoY, 80, 80, (uint16_t*)Logo_80x80);
  
  tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
  tft.setTextSize(2);
  tft.setTextFont(1);
  String text = "HF RECEIVER V1.1";
  int textX = logoX + 80 + 15;
  int textY = logoY + (80 / 2) - 8;
  tft.drawString(text, textX, textY);
  
  tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
  tft.setTextSize(1);
  tft.setTextFont(1);
  String subText = "By IV3LDJ";
  int textWidth = text.length() * 12;
  int subTextWidth = subText.length() * 6;
  int subTextX = textX + (textWidth - subTextWidth);
  int subTextY = textY + 24;

  tft.drawString(subText, subTextX, subTextY);
  
  int lineY = logoY + 80 + 15;
  tft.drawLine(10, lineY, tft.width() - 10, lineY, TFT_WHITE);
  
  tft.setTextColor(TFT_GREEN, BACKGROUND_COLOR);
  tft.setTextSize(1);
  tft.drawString("VFO/BFO with ESP32 + SI5351 V1.1", 10, lineY + 10);

    // === DEBUG I2C ===
  int debugY = lineY + 25;
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(1);
  tft.drawString("I2C Devices:", 10, debugY);
  
  // Scansiona i dispositivi I2C
  debugY += 12;
  byte error, address;
  int deviceCount = 0;
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      deviceCount++;
      String deviceInfo = "  0x" + String(address, HEX);
      
      // Aggiungi descrizione per indirizzi noti
      if (address == 0x20) deviceInfo += " PCF8574A";
      else if (address == 0x22) deviceInfo += " PCF8574T";
      else if (address == 0x50) deviceInfo += " EEPROM";
      else if (address == 0x60) deviceInfo += " SI5351";
      
      tft.drawString(deviceInfo, 10, debugY);
      debugY += 12;
      
      // Limita a 6 righe per non uscire dallo schermo
      if (deviceCount >= 6) break;
    }
  }
  
  tft.setTextColor(TFT_CYAN, BACKGROUND_COLOR);
  tft.drawString("Total: " + String(deviceCount) + " devices", 10, debugY);
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
  tft.drawString("VFO-BFO Ready", 10, debugY + 15);

  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, BACKGROUND_COLOR);
  tft.drawString("For calibration: type HELP on the serial monitor.", 10, debugY + 35 );

  delay(8000);
  tft.fillScreen(BACKGROUND_COLOR);
}


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

  // Disegna riquadro step
  tft.fillRoundRect(STEP_BOX_X, STEP_BOX_Y, STEP_BOX_WIDTH, STEP_BOX_HEIGHT, BOX_RADIUS, BACKGROUND_COLOR);
  tft.drawRoundRect(STEP_BOX_X, STEP_BOX_Y, STEP_BOX_WIDTH, STEP_BOX_HEIGHT, BOX_RADIUS, BORDER_COLOR);
  tft.drawRoundRect(STEP_BOX_X+1, STEP_BOX_Y+1, STEP_BOX_WIDTH-2, STEP_BOX_HEIGHT-2, BOX_RADIUS, BORDER_COLOR);

  // Scrivi l'unità di Step
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(STEP_BOX_TEXT_SIZE);
  tft.drawString("STEP", STEP_BOX_X + 25, STEP_BOX_Y + 5);

  // Scrivi l'unità di Frequenza
  tft.setTextSize(VFO_LABEL_SIZE);
  tft.setTextColor(VFO_LABEL_COLOR, BACKGROUND_COLOR);
  tft.drawString("MHz", VFO_DISPLAY_X+260, VFO_DISPLAY_Y+40);
  
  // Inizializza l'S-meter
  setupSMeter();
  
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  tft.setTextSize(1);
  
  for (int i = 0; i < S_METER_SEGMENTS; i++) {
    int segmentX = S_METER_X + (i * S_METER_SEGMENT_WIDTH);
    
    if (i == 0) tft.drawString("S", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
    else if (i == 3) tft.drawString("1", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
    else if (i == 6) tft.drawString("3", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
    else if (i == 9) tft.drawString("5", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
    else if (i == 12) tft.drawString("7", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
    else if (i == 15) tft.drawString("9", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
    else if (i == 18) {
      tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
      tft.drawString("+20", segmentX - 2, S_METER_Y + S_METER_HEIGHT + 5);
    } else if (i == 21) {
      tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
      tft.drawString("+40", segmentX - 2, S_METER_Y + S_METER_HEIGHT + 5);
    } else if (i == 24) {
      tft.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
      tft.drawString("+60", segmentX - 2, S_METER_Y + S_METER_HEIGHT + 5);
    }
  }
  
  tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
  
/*   // Inizializza la visualizzazione della frequenza
  lastFreqStr = ""; // Forza l'aggiornamento iniziale
  updateFrequencyDisplay(); */
}

//################################ Grafica Frequenza #####################################

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
    else if (textWidth == 192) xPos = 32; // 8 caratteri (spostato a sinistra)
    else xPos = 32; // Default (spostato a sinistra)
    
    // Pulisci lo sprite
    freqSprite.fillSprite(BACKGROUND_COLOR);
    
    // Disegna la frequenza sullo sprite
    freqSprite.drawString(freqStr, xPos, 5);
    
    // Calcola la posizione di destinazione sul display 
    int destX = VFO_DISPLAY_X; 
    int destY = VFO_DISPLAY_Y;
    
    // Push dello sprite sul display
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
  else stepStr = "ERR";
  
  if (stepStr != lastStepStr) {
    tft.fillRect(STEP_BOX_X+2, STEP_BOX_Y+15, STEP_BOX_WIDTH-4, STEP_BOX_HEIGHT-20, BACKGROUND_COLOR);
    tft.setTextColor(STEP_COLOR, BACKGROUND_COLOR);
    tft.setTextSize(2);
    
    int textWidth = stepStr.length() * 12;
    int centeredX = STEP_BOX_X + (STEP_BOX_WIDTH - textWidth) / 2;
    
    tft.drawString(stepStr, centeredX, STEP_BOX_Y+15);
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
  tft.setTextColor(BFO_LABEL_COLOR , BACKGROUND_COLOR);
  tft.setTextSize(2);
  tft.drawString("BFO:", BFO_DISPLAY_X-60, BFO_DISPLAY_Y+15);
  tft.drawString("kHz", BFO_DISPLAY_X+BFO_GRAPH_WIDTH+5, BFO_DISPLAY_Y+15);
  
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
  tft.setTextColor(BFO_LABEL_COLOR, BACKGROUND_COLOR);
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
      tft.fillRect(BFO_DISPLAY_X-60, BFO_DISPLAY_Y, BFO_DISPLAY_WIDTH+80, BFO_DISPLAY_HEIGHT, BACKGROUND_COLOR);
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




