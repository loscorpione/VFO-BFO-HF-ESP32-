#ifndef CONFIG_H
#define CONFIG_H

// Configurazione encoder
#define ENC_CLK 34
#define ENC_DT  35
#define ENC_SW  32
#define SW_BAND 33
#define SW_MODE 25

// Pin per codifica binaria modalità
#define PCF_MODE_BIT0  P4    // Pin P4 del PCF8574
#define PCF_MODE_BIT1  P5    // Pin P5 del PCF8574  
#define PCF_MODE_BIT2  P6    // Pin P6 del PCF8574 (futuro)

// Configurazione I2C
#define I2C_SDA 21
#define I2C_SCL 22

// Indirizzo I2C del PCF8574A
#define PCF8574A_ADDRESS 0x20

// Encoder Pitch BFO 
#define BFO_PITCH_CLK 14        
#define BFO_PITCH_DT  12        
#define BFO_PITCH_MIN -200      // -200Hz
#define BFO_PITCH_MAX 200       // +200Hz  
#define BFO_PITCH_STEP 5        // 5Hz per click

// Frequenze BFO base (con offset di 1.5kHz)
#define BFO_USB_BASE 453500       // BFO per USB (IF 455kHz - offset 1.5kHz)
#define BFO_LSB_BASE 456500       // BFO per LSB (IF 455kHz + offset 1.5kHz)
#define BFO_CW_BASE 454300        // BFO per CW (IF 455kHz - offset 700Hz) - mantenuto per CW

// Display BFO
#define BFO_DISPLAY_X 70      // Posizione X del display BFO
#define BFO_DISPLAY_Y 102      // Posizione Y del display BFO
#define BFO_DISPLAY_WIDTH 120  // Larghezza display BFO
#define BFO_DISPLAY_HEIGHT 40  // Altezza display BFO
#define BFO_GRAPH_WIDTH 100    // Larghezza grafico
#define BFO_GRAPH_HEIGHT 12     // Altezza grafico
#define BFO_GRAPH_X 58       // Posizione X grafico
#define BFO_GRAPH_Y 120        // Posizione Y grafico
#define BFO_MARKER_HEIGHT 10   // Altezza linea verticale
#define BFO_CENTER_MARKER_HEIGHT 12  // Altezza marcatore centrale

// Frequenza IF del ricevitore
#define IF_FREQUENCY 455000

// S-Meter - Pin e configurazione
#define S_METER_PIN 15          // Pin analogico per il S-meter
#define S_METER_X 10             // Posizione X
#define S_METER_Y 160           // Posizione Y 
#define S_METER_WIDTH 300       // Larghezza totale
#define S_METER_HEIGHT 15       // Altezza
#define S_METER_SEGMENTS 25     // Numero di segmenti
#define S_METER_SEGMENT_WIDTH 12 // Larghezza di ogni segmento

// Colori S-meter
#define S_METER_LOW_COLOR TFT_GREEN
#define S_METER_HIGH_COLOR TFT_RED
#define S_METER_BG_COLOR TFT_DARKGREY
#define S_METER_BORDER_COLOR TFT_WHITE

// Posizione riquadri (banda, modalità, AGC, ATT) 
#define POSITION_X 8
#define POSITION_Y 200
#define BOX_NUM 4
#define BOX_WIDTH 70
#define BOX_HEIGHT 40
#define BOX_SPACING 8
#define BOX_RADIUS 5
#define TEXT_SIZE 2
#define TEXT_SIZE_TITLE 1

// Controllo AGC e ATT
#define AGC_BUTTON_PIN 26      // Pulsante AGC Fast/Slow
#define ATT_BUTTON_PIN 27      // Pulsante Attenuatore -20dB

// Debounce per tutti i pulsanti
const int buttonDebounce = 200;

// Stati AGC e ATT
extern bool agcFastMode;       // true = Fast, false = Slow
extern bool attenuatorEnabled; // true = -20dB attivo

// Variabili globali
extern unsigned long vfoFrequency;
extern unsigned long displayedFrequency;
extern unsigned long step;
extern unsigned long minFreq;
extern unsigned long maxFreq;
extern bool bfoEnabled;
extern unsigned long bfoFrequency;
extern int bfoPitchOffset[];
extern int currentBFOOffset;
extern bool agcFastMode;       
extern bool attenuatorEnabled; 

// Colori personalizzati
#define FREQUENCY_COLOR TFT_GREEN
#define STEP_COLOR TFT_RED
#define BAND_COLOR TFT_CYAN
#define MODE_COLOR TFT_YELLOW
#define BACKGROUND_COLOR TFT_BLACK
#define BORDER_COLOR TFT_WHITE

#endif