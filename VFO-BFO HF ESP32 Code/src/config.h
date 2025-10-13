#ifndef CONFIG_H
#define CONFIG_H

// Configurazione GPIO
#define VFO_ENC_CLK 34              // Pin CLK dell'encoder VFO
#define VFO_ENC_DT  35              // Pin DT dell'encoder VFO
#define SW_STEP 32                  // Pulsante cambio Step VFO
#define SW_BAND 33                  // Pulsante cambio banda
#define SW_MODE 25                  // Pulsante cambio modalità
#define SW_AGC  26                  // Pulsante AGC Fast/Slow
#define SW_ATT  27                  // Pulsante Attenuatore -20dB
#define BFO_ENC_CLK 14              // Pin CLK dell'encoder BFO
#define BFO_ENC_DT  12              // Pin DT dell'encoder BFO

// Configurazione I2C GPIO
#define I2C_SDA 21                  // Pin SDA I2C
#define I2C_SCL 22                  // Pin SCL I2C

// Pin per codifica binaria modalità
#define PCF_MODE_BIT0  P4          // Pin P4 del PCF8574
#define PCF_MODE_BIT1  P5          // Pin P5 del PCF8574
//#define PCF_MODE_BIT2  P6          // Pin P6 del PCF8574 (futuro)

// Indirizzo I2C del PCF8574A
#define PCF8574A_ADDRESS 0x20

// Frequenza IF del ricevitore
#define IF_FREQUENCY 455000        // IF=455kHz

// Frequenze BFO base (con offset di 1.5kHz)
#define BFO_USB_BASE 453500       // BFO per USB (IF 455kHz - offset 1.5kHz)
#define BFO_LSB_BASE 456500       // BFO per LSB (IF 455kHz + offset 1.5kHz)
#define BFO_CW_BASE 454300        // BFO per CW (IF 455kHz - offset 700Hz) 

// Limiti Pitch BFO         
#define BFO_PITCH_MIN -200        // -200Hz
#define BFO_PITCH_MAX 200         // +200Hz  
#define BFO_PITCH_STEP 5          // 5Hz per click

// Display BFO
#define BFO_DISPLAY_X 70          // Posizione X del display BFO
#define BFO_DISPLAY_Y 102         // Posizione Y del display BFO
#define BFO_DISPLAY_WIDTH 120     // Larghezza display BFO
#define BFO_DISPLAY_HEIGHT 40     // Altezza display BFO
#define BFO_GRAPH_WIDTH 100       // Larghezza grafico
#define BFO_GRAPH_HEIGHT 12       // Altezza grafico
#define BFO_GRAPH_X 58            // Posizione X grafico
#define BFO_GRAPH_Y 120           // Posizione Y grafico
#define BFO_MARKER_HEIGHT 10      // Altezza linea verticale
#define BFO_CENTER_MARKER_HEIGHT 12 // Altezza marcatore centrale

// S-Meter - Pin e configurazione
#define S_METER_PIN 15            // Pin analogico per il S-meter
#define S_METER_X 10              // Posizione X
#define S_METER_Y 160             // Posizione Y
#define S_METER_WIDTH 300         // Larghezza totale
#define S_METER_HEIGHT 15         // Altezza
#define S_METER_SEGMENTS 25       // Numero di segmenti
#define S_METER_SEGMENT_WIDTH 12  // Larghezza di ogni segmento

// Colori S-meter
#define S_METER_LOW_COLOR TFT_GREEN
#define S_METER_HIGH_COLOR TFT_RED
#define S_METER_BG_COLOR TFT_DARKGREY

// Posizione riquadri (banda, modalità, AGC, ATT) 
#define POSITION_X 8              // Posizione X
#define POSITION_Y 200            // Posizione Y
#define BOX_NUM 4                 // Numero di riquadri
#define BOX_WIDTH 70              // Larghezza di ogni riquadro
#define BOX_HEIGHT 40             // Altezza di ogni riquadro
#define BOX_SPACING 8             // Spaziatura tra i riquadri
#define BOX_RADIUS 5              // Raggio angoli arrotondati
#define TEXT_SIZE 2               // Dimensione testo riquadri
#define TEXT_SIZE_TITLE 1         // Dimensione testo etichette

// Colori personalizzati
#define FREQUENCY_COLOR TFT_GREEN // Colore frequenza
#define STEP_COLOR TFT_RED        // Colore step
#define BAND_COLOR TFT_CYAN       // Colore banda
#define MODE_COLOR TFT_YELLOW     // Colore modalità
#define BACKGROUND_COLOR TFT_BLACK// Colore sfondo
#define BORDER_COLOR TFT_WHITE    // Colore bordo

// Debounce per tutti i pulsanti
const int buttonDebounce = 200;

// Variabili globali 
extern unsigned long vfoFrequency;          // Frequenza VFO + IF
extern unsigned long displayedFrequency;    // Frequenza visualizzata (VFO + IF)
extern unsigned long step;                  // Passo VFO
extern unsigned long minFreq;               // Frequenza minima di sintonia
extern unsigned long maxFreq;               // Frequenza massima di sintonia
extern bool bfoEnabled;                     // Stato BFO: true = attivo, false = disattivo
extern unsigned long bfoFrequency;          // Frequenza BFO
extern int bfoPitchOffset[];                // Offset BFO iniziale per ogni modalità
extern int currentBFOOffset;                // Offset BFO corrente
extern bool agcFastMode;                    // Stati AGC: true = Fast, false = Slow
extern bool attenuatorEnabled;              // Stati ATT: true = -20dB attivo false = disattivato

#endif