#ifndef CONFIG_H
#define CONFIG_H

// Configurazione encoder
#define ENC_CLK 34
#define ENC_DT  35
#define ENC_SW  32
#define SW_BAND 33
#define SW_MODE 25

// Pin per codifica binaria modalità
#define MODE_BIT0 12
#define MODE_BIT1 14

// Configurazione I2C
#define I2C_SDA 21
#define I2C_SCL 22

// Frequenza IF del ricevitore
#define IF_FREQUENCY 455000

// BFO Configuration
#define BFO_PIN 15          // GPIO15 per controllo shift BFO
#define BFO_LSB_OFFSET -1500 // -1.5 kHz per LSB
#define BFO_USB_OFFSET 1500  // +1.5 kHz per USB
#define BFO_CW_OFFSET 700    // +700 Hz per CW
#define BFO_SHIFT_RANGE 1500 // ±1.5 kHz range
#define BFO_CW_SHIFT_RANGE 500 // ±500 Hz range per CW
#define BFO_UPDATE_INTERVAL 100 // Intervallo aggiornamento BFO in ms
#define BFO_UPDATE_THRESHOLD 5  // Soglia di cambiamento per aggiornamento BFO

// Variabili globali
extern unsigned long vfoFrequency;
extern unsigned long displayedFrequency;
extern unsigned long step;
extern unsigned long minFreq;
extern unsigned long maxFreq;

// Colori personalizzati
#define FREQUENCY_COLOR TFT_GREEN
#define STEP_COLOR TFT_RED
#define BAND_COLOR TFT_CYAN
#define MODE_COLOR TFT_YELLOW
#define BACKGROUND_COLOR TFT_BLACK
#define BORDER_COLOR TFT_WHITE

#endif