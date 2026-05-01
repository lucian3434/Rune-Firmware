#pragma once

#include "pusher.h"
#include "drv/drv824xs.h"
#include "pico/stdlib.h"
#include "./../../util.h"
#include "debounce/button.h"

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
            alarm_id_t pusherSafetyAlarmId;
            bool stopOnCycle;

            void startSafetyTimer();

        public:
            PusherScotchYoke(FireModeGeneric** firemode_curr, Debounce::Button* cycleSwitch, DRV::DRV824xS* drv);
            bool init() override;
            void pusherTick() override;
            void startPusher(bool single = false) override;
            void stopPusher() override;

            static int64_t pusherSafetyCallback(alarm_id_t id, __unused void* userData);
    };
}