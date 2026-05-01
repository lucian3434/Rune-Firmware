#pragma once

#include "./../../config.h"
#include "./../../util.h"

namespace Rune {
    // forward declaration
    class FireModeGeneric;

    // Prototype class for a pusher module. Not for direct use.
    class PusherGeneric {
        public:
            enum pusherState_t {
                RUNNING,
                STOPPED
            };

        protected:
            FireModeGeneric** firemode;
            pusherState_t pusherState;

        public:
            PusherGeneric(); // not for actual use, just to satisfy the compiler
            PusherGeneric(FireModeGeneric** firemode_curr);
            virtual ~PusherGeneric() = default;
            virtual bool init();
            virtual void pusherTick() = 0;
            virtual pusherState_t getPusherState();
            virtual void startPusher(bool single = false) = 0;
            virtual void stopPusher() = 0;
    };
}