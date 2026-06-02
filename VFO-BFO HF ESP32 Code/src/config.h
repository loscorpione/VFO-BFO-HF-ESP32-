#ifndef CONFIG_H
#define CONFIG_H

    // ============================================
    // CONFIGURAZIONE ENCODER VFO
    // ============================================

        // Tipo encoder: OPTICAL o MECHANICAL
        #define ENCODER_TYPE_OPTICAL 1
        #define ENCODER_TYPE_MECHANICAL 0

        // === CAMBIA QUI IL TIPO DEL TUO ENCODER ===
        #define ENCODER_TYPE ENCODER_TYPE_OPTICAL   // Per ottico 
        //#define ENCODER_TYPE ENCODER_TYPE_MECHANICAL   // Per meccanico

        // Pin encoder VFO
        #define VFO_ENC_CLK 35
        #define VFO_ENC_DT  34

        // Parametri specifici per tipo
        #if ENCODER_TYPE == ENCODER_TYPE_OPTICAL
            // Encoder Ottico 400PPR
            #define ENCODER_PPR 400
            #define ENCODER_ATTACH_MODE attachSingleEdge   // 400 conteggi/giro × 10Hz = 4000 Hz/giro
            //#define ENCODER_ATTACH_MODE attachHalfQuad   // 800 conteggi/giro x 10Hz = 8000 Hz/giro (più veloce)
            //#define ENCODER_ATTACH_MODE attachFullQuad   // 1600 conteggi/giro x 10Hz = 16000 Hz/giro (molto veloce, sconsigliato per ottico)

        #else
            // Encoder Meccanico 30PPR
            #define ENCODER_PPR 30
            #define ENCODER_ATTACH_MODE attachFullQuad     // 120 conteggi/giro × 10Hz = 1200 Hz/giro
            
        #endif

        // Sensibilità encoder VFO
        // Riduce la sensibilità: 1 = originale, 2 = metà, 4 = un quarto
        #define ENCODER_SENSITIVITY_DIVIDER 2

        
        // CONFIGURAZIONE STEP VFO
        #define STEP_10HZ 10
        #define STEP_100HZ 100
        #define STEP_1KHZ 1000
        #define STEP_10KHZ 10000
        
    // ============================================
    // CONFIGURAZIONE ENCODER BFO
    // ============================================

        #define BFO_ENC_CLK 12                  // Pin CLK dell'encoder BFO
        #define BFO_ENC_DT  13                  // Pin DT dell'encoder BFO

    // ============================================
    // CONFIGURAZIONE PULSANTI
    // ============================================

        #define SW_STEP 32                  // Pulsante cambio Step VFO
        #define SW_BAND 33                  // Pulsante cambio banda
        #define SW_MODE 25                  // Pulsante cambio modalità
        #define SW_AGC  26                  // Pulsante AGC Fast/Slow
        #define SW_ATT  27                  // Pulsante Attenuatore -20dB
        #define SW_SCAN 14                  // Pulsante Scan (futuro)
        #define SW_RTTY_CW  36              // Pulsante RTTY_CW (futuro)

        // Debounce per tutti i pulsanti
        const int buttonDebounce = 200;

    // ============================================
    // CONFIGURAZIONE INPUT/OUTPUT VARI
    // ============================================

        // Configurazione GPIO Ingresso S-Meter 
        #define S_METER_PIN 15              // Pin analogico per il S-meter
        
        // Configurazione GPIO Ingresso RTTY_CW 
        #define RTTY_CW_PIN 4               // Pin digitale per RTTY_CW (futuro)

        // Configurazione GPIO bus I2C 
        #define I2C_SCL 21                  // Pin SCL I2C
        #define I2C_SDA 22                  // Pin SDA I2C

    // ============================================
    // CONFIGURAZIONE PCF8574A
    // ============================================

        // Indirizzo I2C del PCF8574A
        #define PCF8574A_ADDRESS 0x20       // indirizzo PCF8574 (A0,A1,A2 collegati a GND)

        // Pin PCF8574A per codifica binaria
        #define PCF_MODE_BIT0  P0           // Pin P0 del PCF8574
        #define PCF_MODE_BIT1  P1           // Pin P1 del PCF8574
        #define PCF_MODE_BIT2  P2           // Pin P2 del PCF8574
        #define PCF_MODE_BIT3  P3           // Pin P3 del PCF8574
        #define PCF_MODE_BIT4  P4           // Pin P4 del PCF8574
        #define PCF_MODE_BIT5  P5           // Pin P5 del PCF8574
        #define PCF_MODE_BIT6  P6           // Pin P6 del PCF8574
        #define PCF_MODE_BIT7  P7           // Pin P7 del PCF8574 

    // ============================================
    // CONFIGURAZIONE EEPROM ESTERNA 24LC32
    // ============================================

        // Indirizzo I2C della 24LC32
        #define EXTERNAL_EEPROM_ADDRESS 0x50// Indirizzo EEPROM esterna (A0,A1,A2 collegati a GND)

        // Dimensione EEPROM in byte
        //#define EEPROM_SIZE 4096          // 24LC32 = 2Kbit = 4KB
        //#define EEPROM_SIZE 16384         // 24LC128 = 128Kbit = 16KB
        #define EEPROM_SIZE 32768           // 24LC256 = 256Kbit = 32KB

        // Timing salvataggio
        #define EEPROM_SAVE_DELAY 3000      // Salva dopo 3 secondi di inattività
        #define EEPROM_QUICK_SAVE_DELAY 500 // Salvataggio rapido per cambi importanti

    // ============================================
    // CONFIGURAZIONE FREQUENZE
    // ============================================

        // Frequenza IF del ricevitore
        #define IF_FREQUENCY 455000         // IF=455kHz

        // Frequenze BFO base (con offset di 1.5kHz)
        #define BFO_USB_BASE 453500         // BFO per USB (IF 455kHz - offset 1.5kHz)
        #define BFO_LSB_BASE 456500         // BFO per LSB (IF 455kHz + offset 1.5kHz)
        #define BFO_CW_BASE 454300          // BFO per CW (IF 455kHz - offset 700Hz) 

        // Limiti Pitch BFO         
        #define BFO_PITCH_MIN -300          // -300Hz
        #define BFO_PITCH_MAX 300           // +300Hz  
        #define BFO_PITCH_STEP 5            // 5Hz per click

    // ============================================
    // CONFIGURAZIONE DISPLAY
    // ============================================
    
        // Display VFO
        #define VFO_DISPLAY_X 15            // Posizione X del display VFO
        #define VFO_DISPLAY_Y 30            // Posizione Y del display VFO
        #define VFO_LABEL_SIZE 2
        #define VFO_LABEL_COLOR TFT_SKYBLUE

        // Display BFO
        #define BFO_DISPLAY_X 80            // Posizione X del display BFO
        #define BFO_DISPLAY_Y 98            // Posizione Y del display BFO
        #define BFO_DISPLAY_WIDTH 120       // Larghezza display BFO
        #define BFO_DISPLAY_HEIGHT 40       // Altezza display BFO
        #define BFO_GRAPH_WIDTH 100         // Larghezza grafico
        #define BFO_GRAPH_HEIGHT 12         // Altezza grafico
        #define BFO_GRAPH_X 73              // Posizione X grafico
        #define BFO_GRAPH_Y 115             // Posizione Y grafico
        #define BFO_MARKER_HEIGHT 10        // Altezza linea verticale
        #define BFO_CENTER_MARKER_HEIGHT 12 // Altezza marcatore centrale
        #define BFO_LABEL_COLOR TFT_SKYBLUE // Colore etichetta BFO

        // Display STEP
        #define STEP_BOX_X 250
        #define STEP_BOX_Y 102
        #define STEP_BOX_WIDTH 70
        #define STEP_BOX_HEIGHT 40
        #define STEP_BOX_TEXT_COLOR TFT_RED
        #define STEP_BOX_TEXT_SIZE 1

        // S-Meter - Pin e configurazione 
        #define S_METER_X 15                // Posizione X
        #define S_METER_Y 160               // Posizione Y
        #define S_METER_WIDTH 300           // Larghezza totale
        #define S_METER_HEIGHT 15           // Altezza
        #define S_METER_SEGMENTS 25         // Numero di segmenti
        #define S_METER_SEGMENT_WIDTH 12    // Larghezza di ogni segmento

        // Colori S-meter
        #define S_METER_LOW_COLOR TFT_GREEN
        #define S_METER_HIGH_COLOR TFT_RED
        #define S_METER_BG_COLOR TFT_DARKGREY

        // Posizione riquadri (banda, modalità, AGC, ATT) 
        #define POSITION_X 10              // Posizione X
        #define POSITION_Y 200            // Posizione Y
        #define BOX_NUM 4                 // Numero di riquadri
        #define BOX_WIDTH 70              // Larghezza di ogni riquadro
        #define BOX_HEIGHT 40             // Altezza di ogni riquadro
        #define BOX_SPACING 10            // Spaziatura tra i riquadri
        #define BOX_RADIUS 5              // Raggio angoli arrotondati
        #define TEXT_SIZE 2               // Dimensione testo riquadri
        #define TEXT_SIZE_TITLE 1         // Dimensione testo etichette

        // Colori personalizzati
        #define FREQUENCY_COLOR TFT_GREEN // Colore frequenza
        #define STEP_COLOR TFT_RED        // Colore step
        #define BAND_COLOR TFT_SKYBLUE    // Colore banda
        #define MODE_COLOR TFT_YELLOW     // Colore modalità
        #define BACKGROUND_COLOR TFT_BLACK// Colore sfondo
        #define BORDER_COLOR TFT_WHITE    // Colore bordo

        // Variabili globali 
        extern unsigned long vfoFrequency;          // Frequenza VFO + IF
        extern unsigned long displayedFrequency;    // Frequenza visualizzata (VFO + IF) 
        extern int currentMode;                     // Modalità corrente
        extern unsigned long step;                  // Passo VFO
        extern unsigned long minFreq;               // Frequenza minima di sintonia
        extern unsigned long maxFreq;               // Frequenza massima di sintonia
        extern bool bfoEnabled;                     // Stato BFO: true = attivo, false = disattivo
        extern unsigned long bfoFrequency;          // Frequenza BFO
        extern int bfoPitchOffset[];                // Offset BFO iniziale per ogni modalità
        extern int currentBFOOffset;                // Offset BFO corrente
        extern bool agcFastMode;                    // Stati AGC: true = Fast, false = Slow
        extern bool attenuatorEnabled;              // Stati ATT: true = -20dB attivo false = disattivato
        extern long si5351Calibration;              // Fattore di calibrazione SI5351

    

  #endif