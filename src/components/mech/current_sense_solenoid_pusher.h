#pragma once

#include "pusher.h"
#include "drv/drv824xs.h"
#include "pico/stdlib.h"
#include "adc/adc.h"
#include "./../../util.h"

namespace Rune {
    class PusherSolenoidCurrSensing : public PusherGeneric {
        protected:
            DRV::DRV824xS *driver;
            bool stopOnCycle;
            Rune::Config* cfg;
            ADC::ADC *adcm;

        private: 
            enum solenoidState_t {
                EXTENDING,
                RETRACTING,
                RETRACTED
            }; 
            
            absolute_time_t solenoidLastStateChange;
            absolute_time_t solenoidNextStateChange;
            solenoidState_t solenoidState;

            uint32_t max;
            uint32_t min;
            bool minReached;

            int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
            void resetSenseVariables();

        public:
            PusherSolenoidCurrSensing(FireModeGeneric*** firemode_curr, Rune::Config* config, ADC::ADC *adc, DRV::DRV824xS* drv);
            bool init() override;
            void pusherTick() override;
            void startPusher(bool single = false) override;
            void stopPusher() override;
    };
}