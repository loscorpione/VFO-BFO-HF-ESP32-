#ifndef VFO_BFO_H
#define VFO_BFO_H

// Funzioni encoder VFO
void setupEncoders();
void readVFOEncoder();
void changeStep();

// Funzioni encoder BFO
int readBFOEncoder();
void updateBFOFromEncoder();

#endif