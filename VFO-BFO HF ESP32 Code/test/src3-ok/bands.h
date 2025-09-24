#ifndef BANDS_H
#define BANDS_H

struct Band {
  const char* name;
  unsigned long startFreq;
  unsigned long endFreq;
};

extern Band bands[];
extern int currentBandIndex;
extern int totalBands;

int getBandIndex(unsigned long freq);
void changeBand();
void updateBandInfo();

#endif