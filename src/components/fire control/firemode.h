#pragma once

#include <stdint.h>
#include "./../states.h"
#include "./../mech/pusher.h"

namespace Rune {
    class FireModeGeneric {
        public:
            uint8_t shotsFired;
            virtual uint32_t getRPMCurve(uint8_t motor, uint32_t targetRPM); // defaults to step function
            virtual void tick(States* states, PusherGeneric* pusher) = 0; // must be implemented by child class
    };
}