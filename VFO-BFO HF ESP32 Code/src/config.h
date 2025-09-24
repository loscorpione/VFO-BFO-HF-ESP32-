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

// Indirizzo I2C del PCF8574A
#define PCF8574A_ADDRESS 0x20

// Frequenza IF del ricevitore
#define IF_FREQUENCY 455000

// S-Meter - Pin e configurazione
#define S_METER_PIN 15          // Pin analogico per il S-meter
#define S_METER_X 10             // Posizione X
#define S_METER_Y 200           // Posizione Y (parte bassa libera del display)
#define S_METER_WIDTH 300       // Larghezza totale
#define S_METER_HEIGHT 15       // Altezza
#define S_METER_SEGMENTS 25     // Numero di segmenti
#define S_METER_SEGMENT_WIDTH 12 // Larghezza di ogni segmento

// Colori S-meter
#define S_METER_LOW_COLOR TFT_GREEN
#define S_METER_HIGH_COLOR TFT_RED
#define S_METER_BG_COLOR TFT_DARKGREY
#define S_METER_BORDER_COLOR TFT_WHITE

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