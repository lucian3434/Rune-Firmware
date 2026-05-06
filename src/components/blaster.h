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

namespace Rune {
    // this class manages the hardware and state of the blaster
    class Blaster {
        private:
        std::vector<Debounce::Button> switches;
        public:
        Rune::States logicLines;
        Rune::Config* cfg;
        PusherGeneric* pusher;
        FireModeGeneric** currFireMode;
        std::vector<Debounce::Button*> selectors;
        std::vector<Rune::FireModeGeneric*> fireModes;

        Blaster(Rune::Config* config);
        bool init(HW::Board* board);
        void updateIO();
    };
}