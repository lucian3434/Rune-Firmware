#include "burst.h"
#include "./../mech/pusher.h"

Rune::FireModeBurst::FireModeBurst(uint8_t burstSize, bool allowPartial) {
    state = IDLE;
    queued = 0;
    shotsFired = 0;
    cutoff = allowPartial;
    size = burstSize;
}

void Rune::FireModeBurst::tick(States* states, PusherGeneric* pusher) {
    if (states->virtTrig.isRisingEdge() && states->virtRev.getState()) {
        queued = size;
        shotsFired = 0;
        state = FIRING;
    }

    if (cutoff && states->virtTrig.isFallingEdge()) {
        queued = 1; // ensure at least one shot is fired
    }

    if (queued > shotsFired) {
        //uprintf("shots fired: %u\r\n", shotsFired);
        if (states->wheelsAtSpeed && (pusher->getPusherState() == pusher->STOPPED)) {
            pusher->startPusher(true);
        }
        states->virtRev.forceHigh();
    }
    else {
        state = IDLE;
        pusher->stopPusher();
    }
}