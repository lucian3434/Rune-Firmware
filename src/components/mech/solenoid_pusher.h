#pragma once

#include "pusher.h"
#include "drv/drv824xs.h"
#include "pico/stdlib.h"
#include "./../../util.h"

namespace Rune {
    class SolenoidPusher : public PusherGeneric {
        protected:
            DRV::DRV824xS *driver;
            bool stopOnCycle;
            Rune::Config* cfg;

        private: 
            enum solenoidState_t {
                EXTENDING,
                RETRACTING,
                RETRACTED
            }; 
            
            absolute_time_t solenoidLastStateChange;
            absolute_time_t solenoidNextStateChange;
            solenoidState_t solenoidState;

        public:
            SolenoidPusher(FireModeGeneric*** firemode_curr, Rune::Config* config, DRV::DRV824xS* drv);
            bool init() override;
            void pusherTick() override;
            void startPusher(bool single = false) override;
            void stopPusher() override;
    };
}