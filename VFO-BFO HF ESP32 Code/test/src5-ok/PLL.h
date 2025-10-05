#ifndef PLL_H
#define PLL_H

#include <si5351.h>

extern Si5351 si5351;

void setupSI5351();
void updateFrequency();

#endif