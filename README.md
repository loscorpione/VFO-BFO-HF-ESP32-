VFO Basato su Si5351 e gestito da un ESP32 Devkit V1 attualmente il progetto è in costruzione.

Pensato principalmente per un ricevitore ma non è detto che in futuro venga implementata la funzione di trasmissione.

il progetto definitivo dovrà prevedere:
  -VFO con IF 455Khz 
  -BFO per la demodulazione di LSB, USB, CW con regolazione del "pitch" per un migliore adattamento
  -Uscita binaria a 2 bit indicante la modalità LSB, USB, CW, AM
  -Uscita binaria a 4 bit indicante la banda utilizzata per la selezione dell'opportuno BPF.
  -Eventuale decodifica per RTTY, CW on board
  -Memorizzazione su eeprom esterna o Fram 
  -Web server per la gestione remota tramite wi-fi ed eventuale proiezione in rete

I comandi sono:
  -ENCODER: come selettore di frequenza.
  -STEP: per cambio frequenza (10Hz, 100Hz, 1KHz, 10KHZ) con indicazione su display.
  -BAND: per la selezione della banda con indicazione su display delle bande radioamatoriali e non.
  -MODE: seleziona il tipo di demodulazione, con indicazione su pin binaria.
  -ATT: selettore attenuatore
