#ifndef MODES_H
#define MODES_H

enum Mode {
  MODE_AM,
  MODE_LSB,
  MODE_USB,
  MODE_CW,
  MODE_COUNT
};

extern const char* modeNames[];
extern int currentMode;

void changeMode();
void updateModeOutputs();
void updateModeInfo();
void updateBFOForMode();  // Nuova funzione

#endif