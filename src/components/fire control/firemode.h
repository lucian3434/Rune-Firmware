#pragma once

#include <stdint.h>
#include "./../states.h"

namespace Rune {
    // forward declaration
    class PusherGeneric;

    class FireModeGeneric {
        public:
            volatile uint8_t shotsFired;
            FireModeGeneric() = default;
            virtual uint32_t getRPMCurve(uint8_t motor, uint32_t targetRPM); // defaults to step function
            virtual void tick(States* states, PusherGeneric* pusher) = 0; // must be implemented by child class
    };
}