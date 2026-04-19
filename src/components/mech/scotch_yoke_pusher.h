#pragma once

#include "pusher.h"
#include "drv/drv824xs.h"
#include "pico/stdlib.h"
#include "./../../util.h"

namespace Rune {
    class PusherScotchYoke : public PusherGeneric {
        protected:
            // state of the scotch yoke pusher safety timeout
            enum pusherSafetyTimeout_t {
                WAITING,
                NONE
            };
            DRV::DRV824xS* driver;
            Debounce::Button* cycle;
            Rune::PusherGeneric::pusherState_t pusherState;
            Rune::PusherScotchYoke::pusherSafetyTimeout_t psTimeout;
            repeating_timer_t pusherSafetyCallbackTimer;

        public:
            PusherScotchYoke(Debounce::Button* cycleSwitch);
            bool init();
            void pusherTick();
    };
}