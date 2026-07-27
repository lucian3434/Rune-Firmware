#include "semi.h"
#include "./../mech/pusher.h"

Rune::FireModeSemi::FireModeSemi() {
    state = IDLE;
}

void Rune::FireModeSemi::tick(States* states, PusherGeneric* pusher) {
    // queue a shot if the trigger is pressed while we are revving
    if (states->virtTrig.isRisingEdge() && states->virtRev.getState()) {
        state = FIRING;
    }

    if (state == FIRING) {
        if (states->wheelsAtSpeed && (pusher->getPusherState() == pusher->STOPPED)) {
            state = IDLE;
            pusher->startPusher(true);
        }
        states->virtRev.forceHigh();
    }
}