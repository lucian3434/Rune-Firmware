#include "full.h"
#include "./../mech/pusher.h"

Rune::FireModeFull::FireModeFull() {
    state = IDLE;
}

void Rune::FireModeFull::tick(States* states, PusherGeneric* pusher) {
    if (states->virtTrig.isRisingEdge() && states->virtRev.getState()) {
        state = FIRING;
        shotsFired = 0;
    }
    else if ((shotsFired > 0) && !states->virtTrig.getState()) {
        state = IDLE; // stop firing if they let go of the trigger and we have already fired at least once
    }

    if (state == FIRING) {
        if (states->wheelsAtSpeed && (pusher->getPusherState() == pusher->STOPPED)) {
            pusher->startPusher(true);
        }
        states->virtRev.forceHigh();
    }
    else {
        pusher->stopPusher();
    }
}