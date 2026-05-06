#include "full.h"
#include "./../mech/pusher.h"

Rune::FireModeFull::FireModeFull() {
    queued = false;
}

void Rune::FireModeFull::tick(States* states, PusherGeneric* pusher) {
    if (states->virtTrig.isRisingEdge() && states->virtRev.getState()) {
        queued = true;
        shotsFired = 0;
    }
    else if ((shotsFired > 0) && !states->virtTrig.getState()) {
        queued = false; // stop firing if they let go of the trigger and we have already fired at least once
    }

    if (queued) {
        if (states->wheelsAtSpeed && (pusher->getPusherState() == pusher->STOPPED)) {
            pusher->startPusher(true);
        }
        states->virtRev.forceHigh();
    }
    else {
        pusher->stopPusher();
    }
}