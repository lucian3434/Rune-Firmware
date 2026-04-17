#pragma once

#include "./../../config.h"

namespace Rune {
    // Prototype class for a pusher module. Not for direct use.
    class PusherGeneric {
        public:
            enum pusherState_t {
                RUNNING,
                STOPPED
            };
            PusherGeneric();
            virtual bool init();
            virtual void pusherTick();
    };
}