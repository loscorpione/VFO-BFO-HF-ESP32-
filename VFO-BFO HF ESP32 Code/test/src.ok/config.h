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

// S-meter configuration
#define SMETER_PIN 0
#define SMETER_SAMPLES 10  // Numero di campioni per la media
#define S9_THRESHOLD 3095   // Soglia per S9 (da calibrare)
//
#define SMETER_WIDTH 150
#define SMETER_HEIGHT 15
#define SMETER_X 165
#define SMETER_Y 118
#define SMETER_UPDATE_THRESHOLD 5  // Soglia di cambiamento per aggiornamento

#endif