#include "display.h"
#include "config.h"
#include "bands.h"
#include "modes.h"
#include "s_meter.h"
#include "PLL.h"
#include "logo80x80.h"

// ============================================
// VARIABILI GLOBALI
// ============================================

TFT_eSPI tft;
TFT_eSprite freqSprite(&tft);

// ============================================
// SPLASH SCREEN
// ============================================

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
    tft.drawString("VFO/BFO with ESP32 + SI5351" + String(SW_VERSION), 10, lineY + 10);


    
    // DEBUG I2C
    int debugY = lineY + 25;
    tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    tft.setTextSize(1);
    tft.drawString("I2C Devices:", 10, debugY);
    
    debugY += 12;
    byte error, address;
    int deviceCount = 0;
    
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        
        if (error == 0) {
            deviceCount++;
            String deviceInfo = "  0x" + String(address, HEX);
            

            if (address == 0x20) deviceInfo += " PCF8574A - Digital Output Expander";
            else if (address == 0x22) deviceInfo += " PCF8574T - Keypad Matrix (4x4) Expander";
            else if (address == 0x50 & EEPROM_SIZE == 4096) deviceInfo += " EEPROM   - 24LC32 (4Kbit)";
            else if (address == 0x50 & EEPROM_SIZE == 8192) deviceInfo += " EEPROM   - 24LC64 (8Kbit)";
            else if (address == 0x50 & EEPROM_SIZE == 16384) deviceInfo += " EEPROM   - 24LC128 (16Kbit)";
            else if (address == 0x50 & EEPROM_SIZE == 32768) deviceInfo += " EEPROM   - 24LC256 (32Kbit)";
            else if (address == 0x60) deviceInfo += " SI5351A  - programmable signal generator";
            else deviceInfo += " Unknown Device";
            
            tft.drawString(deviceInfo, 10, debugY);
            debugY += 12;
            
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
    tft.drawString("For calibration: type HELP on the serial monitor.", 10, debugY + 35);

    delay(Time_SPLASH_SCREEN);
    tft.fillScreen(BACKGROUND_COLOR);
}

// ============================================
// LAYOUT INIZIALE
// ============================================

void drawDisplayLayout() {
    setupFrequencySprite();

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

    tft.fillRoundRect(STEP_BOX_X, STEP_BOX_Y, STEP_BOX_WIDTH, STEP_BOX_HEIGHT, BOX_RADIUS, BACKGROUND_COLOR);
    tft.drawRoundRect(STEP_BOX_X, STEP_BOX_Y, STEP_BOX_WIDTH, STEP_BOX_HEIGHT, BOX_RADIUS, BORDER_COLOR);
    tft.drawRoundRect(STEP_BOX_X+1, STEP_BOX_Y+1, STEP_BOX_WIDTH-2, STEP_BOX_HEIGHT-2, BOX_RADIUS, BORDER_COLOR);

    tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    tft.setTextSize(STEP_BOX_TEXT_SIZE);
    tft.drawString("STEP", STEP_BOX_X + 25, STEP_BOX_Y + 5);

    tft.setTextSize(VFO_LABEL_SIZE);
    tft.setTextColor(VFO_LABEL_COLOR, BACKGROUND_COLOR);
    tft.drawString("MHz", VFO_DISPLAY_X + 260, VFO_DISPLAY_Y + 40);
    
    setupSMeter();
    
    tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    tft.setTextSize(1);
    
    for (int i = 0; i < S_METER_SEGMENTS; i++) {
        int segmentX = S_METER_X + (i * S_METER_SEGMENT_WIDTH);
        
        if (i == 0) {
            tft.drawString("S", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
        } else if (i == 3) {
            tft.drawString("1", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
        } else if (i == 6) {
            tft.drawString("3", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
        } else if (i == 9) {
            tft.drawString("5", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
        } else if (i == 12) {
            tft.drawString("7", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
        } else if (i == 15) {
            tft.drawString("9", segmentX, S_METER_Y + S_METER_HEIGHT + 5);
        } else if (i == 18) {
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
}

// ============================================
// GRAFICA FREQUENZA
// ============================================

static String lastFreqStr = "";
static int lastSpriteWidth = 0;
static int lastSpriteX = 0;

void updateFrequencyDisplay() {
    String freqStr = formatFrequency(displayedFrequency);
    
    // Calcola posizione
    int xPos;
    int textWidth = freqStr.length() * 24;
    if (textWidth == 216) xPos = 0;
    else if (textWidth == 192) xPos = 32;
    else xPos = 32;
    
    // Aggiorna solo se cambiato (opzionale, riduce flickering)
    static String lastFreqStr = "";
    if (freqStr != lastFreqStr) {
        freqSprite.fillSprite(BACKGROUND_COLOR);
        freqSprite.setTextColor(FREQUENCY_COLOR, BACKGROUND_COLOR);
        freqSprite.setTextFont(7);
        freqSprite.setTextSize(0);
        freqSprite.drawString(freqStr, xPos, 5);
        freqSprite.pushSprite(VFO_DISPLAY_X, VFO_DISPLAY_Y);
        lastFreqStr = freqStr;
    }
}

void setupFrequencySprite() {
    freqSprite.setColorDepth(8);
    freqSprite.createSprite(250, 60);
    freqSprite.fillSprite(BACKGROUND_COLOR);
    freqSprite.setTextColor(FREQUENCY_COLOR, BACKGROUND_COLOR);
    freqSprite.setTextFont(7);
    freqSprite.setTextSize(0);
}

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

// ============================================
// GRAFICA STEP
// ============================================

void updateStepDisplay() {
    static String lastStepStr = "";
    
    String stepStr;
    if (step == 10) stepStr = "10Hz";
    else if (step == 100) stepStr = "100Hz";
    else if (step == 1000) stepStr = "1kHz";
    else if (step == 10000) stepStr = "10kHz";
    else stepStr = "ERR";
    
    if (stepStr != lastStepStr) {
        tft.fillRect(STEP_BOX_X + 2, STEP_BOX_Y + 15, STEP_BOX_WIDTH - 4, STEP_BOX_HEIGHT - 20, BACKGROUND_COLOR);
        tft.setTextColor(STEP_COLOR, BACKGROUND_COLOR);
        //tft.setTextFont(1);
        tft.setTextSize(2);
        
        int textWidth = stepStr.length() * 12;
        int centeredX = STEP_BOX_X + (STEP_BOX_WIDTH - textWidth) / 2;
        
        tft.drawString(stepStr, centeredX, STEP_BOX_Y + 15);
        lastStepStr = stepStr;
    }
}

// ============================================
// GRAFICA BFO
// ============================================

static bool bfoDisplayInitialized = false;
static bool lastBFOEnabled = false;
static unsigned long lastBFOFreq = 0;

void drawBFOStaticElements() {
    tft.fillRect(BFO_DISPLAY_X - 60, BFO_DISPLAY_Y, BFO_DISPLAY_WIDTH + 75, BFO_DISPLAY_HEIGHT, BACKGROUND_COLOR);
    
    tft.setTextColor(BFO_LABEL_COLOR, BACKGROUND_COLOR);
    tft.setTextFont(1);
    tft.setTextSize(2);
    tft.drawString("BFO:", BFO_DISPLAY_X - 60, BFO_DISPLAY_Y + 15);
    tft.drawString("kHz", BFO_DISPLAY_X + BFO_GRAPH_WIDTH + 5, BFO_DISPLAY_Y + 15);
    
    tft.drawFastHLine(BFO_GRAPH_X, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT / 2, BFO_GRAPH_WIDTH, TFT_WHITE);
    
    int centerX = BFO_GRAPH_X + BFO_GRAPH_WIDTH / 2;
    tft.drawFastVLine(centerX - 1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);
    tft.drawFastVLine(centerX, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);
    tft.drawFastVLine(centerX + 1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);

    tft.setTextColor(TFT_WHITE, BACKGROUND_COLOR);
    tft.setTextSize(1);
    tft.setCursor(BFO_GRAPH_X, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 2);
    tft.print("453");
    tft.setCursor(BFO_GRAPH_X + BFO_GRAPH_WIDTH / 2 - 8, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 2);
    tft.print("455");
    tft.setCursor(BFO_GRAPH_X + BFO_GRAPH_WIDTH - 18, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT + 2);
    tft.print("457");
    
    bfoDisplayInitialized = true;
}

void updateBFODynamicElements() {
    if (!bfoEnabled) return;
    
    tft.fillRect(BFO_DISPLAY_X, BFO_DISPLAY_Y, BFO_DISPLAY_WIDTH - 100, 20, BACKGROUND_COLOR);
    tft.fillRect(BFO_GRAPH_X, BFO_GRAPH_Y, BFO_GRAPH_WIDTH, BFO_GRAPH_HEIGHT, BACKGROUND_COLOR);
    
    tft.drawFastHLine(BFO_GRAPH_X, BFO_GRAPH_Y + BFO_GRAPH_HEIGHT / 2, BFO_GRAPH_WIDTH, TFT_WHITE);
    
    int centerX = BFO_GRAPH_X + BFO_GRAPH_WIDTH / 2;
    tft.drawFastVLine(centerX - 1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);
    tft.drawFastVLine(centerX, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);
    tft.drawFastVLine(centerX + 1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_RED);

    int markerPos = map(bfoFrequency, 453000, 457000, BFO_GRAPH_X, BFO_GRAPH_X + BFO_GRAPH_WIDTH);
    markerPos = constrain(markerPos, BFO_GRAPH_X, BFO_GRAPH_X + BFO_GRAPH_WIDTH);
    
    tft.drawFastVLine(markerPos - 1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_GREEN);
    tft.drawFastVLine(markerPos, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_GREEN);
    tft.drawFastVLine(markerPos + 1, BFO_GRAPH_Y, BFO_GRAPH_HEIGHT, TFT_GREEN);
    
    tft.setTextColor(BFO_LABEL_COLOR, BACKGROUND_COLOR);
    tft.setTextFont(1);      
    tft.setTextSize(1);
    tft.setCursor(BFO_GRAPH_X + BFO_GRAPH_WIDTH / 2 - 21, BFO_DISPLAY_Y + 5);

    unsigned long khz = bfoFrequency / 1000;
    unsigned long hz = bfoFrequency % 1000;
    tft.print(khz);
    tft.print(".");
    if (hz < 100) tft.print("0");
    if (hz < 10) tft.print("0");
    tft.print(hz);
}

void drawBFODisplay() {
    if (!bfoDisplayInitialized || bfoEnabled != lastBFOEnabled) {
        if (bfoEnabled) {
            drawBFOStaticElements();
            updateBFODynamicElements();
        } else {
            tft.fillRect(BFO_DISPLAY_X - 60, BFO_DISPLAY_Y, BFO_DISPLAY_WIDTH + 80, BFO_DISPLAY_HEIGHT, BACKGROUND_COLOR);
            bfoDisplayInitialized = false;
        }
        lastBFOEnabled = bfoEnabled;
    }
    
    if (bfoEnabled && abs((long)(bfoFrequency - lastBFOFreq)) > 0) {
        updateBFODynamicElements();
        lastBFOFreq = bfoFrequency;
    }   
}

// ============================================
// INSERIMENTO FREQUENZA DA TASTIERA
// ============================================

void showFrequencyInputMode(bool active) {
    if (active) {
        tft.fillRect(VFO_DISPLAY_X, VFO_DISPLAY_Y, 250, 60, BACKGROUND_COLOR);
    } else {
        updateFrequencyDisplay();
    }
}

void updateFrequencyInputDisplay(String input) {
    tft.fillRect(VFO_DISPLAY_X, VFO_DISPLAY_Y, 250, 60, BACKGROUND_COLOR);
    
    freqSprite.fillSprite(BACKGROUND_COLOR);
    freqSprite.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
    freqSprite.setTextFont(7);
    freqSprite.setTextSize(0);
    
    freqSprite.drawString(input, 0, 5);
    freqSprite.pushSprite(VFO_DISPLAY_X, VFO_DISPLAY_Y);
}

void showInputConfirm() {
    freqSprite.fillSprite(BACKGROUND_COLOR);
    freqSprite.setTextColor(TFT_GREEN, BACKGROUND_COLOR);
    freqSprite.setTextFont(2);
    freqSprite.setTextSize(3);
    freqSprite.drawString("OK setting", 24, 5);
    freqSprite.pushSprite(VFO_DISPLAY_X, VFO_DISPLAY_Y);
    
    freqSprite.setTextColor(TFT_YELLOW, BACKGROUND_COLOR);
    freqSprite.setTextFont(7);
    freqSprite.setTextSize(0);

    delay(500);
    updateFrequencyDisplay();
}

void showInputError() {
        // Salva il font corrente
    uint8_t savedFont = tft.textfont;
    uint8_t savedSize = tft.textsize;

    // Mostra errore in rosso con font grande
    freqSprite.fillSprite(BACKGROUND_COLOR);
    freqSprite.setTextColor(TFT_RED, BACKGROUND_COLOR);
    freqSprite.setTextFont(2);
    freqSprite.setTextSize(3);
    freqSprite.drawString("Out of range", 5, 5);
    freqSprite.pushSprite(VFO_DISPLAY_X, VFO_DISPLAY_Y);
    
    delay(1000);
    
     // RIPRISTINO FORZATO
    tft.fillRect(VFO_DISPLAY_X, VFO_DISPLAY_Y, 250, 60, BACKGROUND_COLOR);
    tft.setTextColor(FREQUENCY_COLOR, BACKGROUND_COLOR);
    tft.setTextFont(7);
    tft.setTextSize(0);
    tft.setCursor(VFO_DISPLAY_X, VFO_DISPLAY_Y + 5);
    tft.print(formatFrequency(displayedFrequency));

        // RIPRISTINA IL FONT DEL TFT
    tft.setTextFont(savedFont);
    tft.setTextSize(savedSize);
  
}

void hideInputMode() {
    updateFrequencyDisplay();
}