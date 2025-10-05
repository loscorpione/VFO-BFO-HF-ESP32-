#include "si5351.h"
#include "config.h"
#include <Wire.h>

Si5351 si5351;

// Variabile per stato BFO
bool bfoEnabled = false;
unsigned long bfoFrequency = 0;

void setupSI5351() {
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(100);

  if (si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0) == false) {
    while(1) {
      delay(1000);
    }
  }

  si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
  si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);
  si5351.output_enable(SI5351_CLK0, 1);
  si5351.output_enable(SI5351_CLK1, 0); // BFO inizialmente disabilitato
}

void updateFrequency() {
  si5351.set_freq(vfoFrequency * 100ULL, SI5351_CLK0);
}

void updateBFO() {
  if (bfoEnabled) {
    si5351.set_freq(bfoFrequency * 100ULL, SI5351_CLK1);
  }
}

void enableBFO() {
  bfoEnabled = true;
  si5351.output_enable(SI5351_CLK1, 1);
  updateBFO();
}

void disableBFO() {
  bfoEnabled = false;
  si5351.output_enable(SI5351_CLK1, 0);
}