#pragma once

#include <stdint.h>
#include "./../states.h"

namespace Rune {
    // forward declaration
    class PusherGeneric;

    class FireModeGeneric {
        public:
            typedef enum {
                IDLE,
                FIRING
            } fireModeState_t;

            fireModeState_t state;
            volatile uint8_t shotsFired;
            FireModeGeneric() = default;
            virtual uint32_t getRPMCurve(uint32_t targetRPM); // defaults to step function
            virtual void tick(States* states, PusherGeneric* pusher) = 0; // must be implemented by child class
    };
}