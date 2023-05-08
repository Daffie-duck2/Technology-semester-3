#define DECODE_HASH
#include "IRReceiver.h"
#include "PinDefinitionsAndMore.h"
#include <stdint.h>
#include <IRremote.hpp>

void IrReceiverSetup() {
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

// Return values:
// Returns -2: No message received
// Returns -1: Wrong or unknown protocol sent
// Returns  0: Invalis parameters
// Returns  1: Succesfull

int IrReceiverLoop(uint32_t* ID) {
    if (ID == NULL) {
        return 0;
    }
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.decodedRawData != 0)
        {
            *ID = IrReceiver.decodedIRData.decodedRawData;
            IrReceiver.resume();
            return 1;
        }

        IrReceiver.resume();
        return -1;
    }
    return -2;
}