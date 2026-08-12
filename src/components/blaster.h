#pragma once

#include "states.h"
#include "board_def.h"
#include "./../config.h"

#include "fire control/firemode.h"
#include "fire control/semi.h"
#include "fire control/burst.h"
#include "fire control/full.h"

#include "mech/pusher.h"
#include "mech/solenoid_pusher.h"
#include "mech/scotch_yoke_pusher.h"
#include "mech/current_sense_solenoid_pusher.h"

#include "motor/motor.h"
#include "motor/bidshot_motor.h"

#include "adc/adc.h"

#include "../pid.h"

namespace Rune {
    // this class manages the hardware and state of the blaster
    class Blaster {
        private:
        std::vector<Debounce::Button> switches;
        public:
        std::string fwVersionString = "v0.0.0";

        ADC::ADC adc;
        Rune::States logicLines;
        Rune::Config* cfg;
        PusherGeneric* pusher;
        FireModeGeneric** currFireMode;
        std::vector<Debounce::Button*> selectors;
        std::vector<Rune::FireModeGeneric*> fireModes;
        std::vector<Motor::Motor*> motors;
        std::vector<PID*> motorPIDs;
        uint8_t fpsCap;

        Blaster(Rune::Config* config);
        bool init(HW::Board* board);
        void updateIO();
    };
}