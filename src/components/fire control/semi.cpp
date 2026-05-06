#include "semi.h"
#include "./../mech/pusher.h"

Rune::FireModeSemi::FireModeSemi() {
    queued = false;
}

void Rune::FireModeSemi::tick(States* states, PusherGeneric* pusher) {
    // queue a shot if the trigger is pressed while we are revving
    if (states->virtTrig.isRisingEdge() && states->virtRev.getState()) {
        queued = true;
    }

    if (queued) {
        if (states->wheelsAtSpeed && (pusher->getPusherState() == pusher->STOPPED)) {
            queued = false;
            pusher->startPusher(true);
        }
        states->virtRev.forceHigh();
    }
}