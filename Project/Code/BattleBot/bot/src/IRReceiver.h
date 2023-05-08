#ifndef IRReceiver_h
#define IRReceiver_h

#include <stdint.h>

void IrReceiverSetup();
int IrReceiverLoop(uint32_t* ID);

#endif