#pragma once

#include "pusher.h"
#include "drv/drv824xs.h"
#include "pico/stdlib.h"
#include "./../../config.h"
/*
namespace Rune {
    class SolenoidPusher : public PusherGeneric {
        public:
            SolenoidPusher(wheelUpdateCallback_t callback, firemode_t **firemode_curr, DRV::DRV824xS *drv);
            bool init();
            void triggerRisingEdge();
            void triggerFallingEdge();
            void pusherTick();
            void updatePusherState(Rune::PusherGeneric::pusherState_t newState);

            // used for runtime configuration of the extend time
            void setExtendTime(uint16_t timeMs) {
                extendTimeMs = timeMs;
            }

            // used for runtime configuration of the retract time
            void setRetractTime(uint16_t timeMs) {
                retractTimeMs = timeMs;
            }


        protected:
            DRV::DRV824xS *driver;
            Rune::PusherGeneric::pusherState_t pusherState;
            Rune::PusherGeneric::pusherState_t pusherLastState;
            repeating_timer_t pusherStateTimer;

        private: 
            enum solenoidState_t {
                EXTENDING,
                RETRACTING,
                RETRACTED
            }; 
            
            absolute_time_t solenoidLastStateChange;
            absolute_time_t solenoidNextStateChange;
            uint16_t extendTimeMs = EXTEND_TIME_MS;
            uint16_t retractTimeMs = RETRACT_TIME_MS;
            solenoidState_t solenoidState =  RETRACTED;

    };
}
    */