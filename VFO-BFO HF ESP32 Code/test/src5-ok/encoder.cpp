#include "encoder.h"
#include "config.h"
#include "bands.h"
#include "modes.h"
#include "display.h"
#include "PLL.h"
#include "DigiOUT.h"
#include <Arduino.h>

extern unsigned long lastEncoderRead;
extern int lastEncoded;
extern int encoderCount;
extern bool buttonPressed;
extern unsigned long lastButtonPress;

void readEncoder() {
  int MSB = digitalRead(ENC_CLK);
  int LSB = digitalRead(ENC_DT);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderCount++;
    if (encoderCount >= 2) {
      displayedFrequency += step;
      if (displayedFrequency > maxFreq) displayedFrequency = maxFreq;
      vfoFrequency = displayedFrequency + IF_FREQUENCY;
      updateFrequency();
      updateFrequencyDisplay();
      updateBandInfo();
      updateDigiOUTOutput();
      encoderCount = 0;
    }
  }
  else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderCount++;
    if (encoderCount >= 2) {
      displayedFrequency -= step;
      if (displayedFrequency < minFreq) displayedFrequency = minFreq;
      vfoFrequency = displayedFrequency + IF_FREQUENCY;
      updateFrequency();
      updateFrequencyDisplay();
      updateBandInfo();
      updateDigiOUTOutput(); 
      encoderCount = 0;
    }
  }

  lastEncoded = encoded;
  delayMicroseconds(100);
}

void changeStep() {
  switch(step) {
    case 10: step = 100; break;
    case 100: step = 1000; break;
    case 1000: step = 10000; break;
    case 10000: step = 10; break;
    default: step = 1000;
  }
}