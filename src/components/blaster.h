#pragma once

#include "states.h"
#include "board_def.h"
#include "./../config.h"
#include "mech/pusher.h"
#include "mech/solenoid_pusher.h"
#include "mech/scotch_yoke_pusher.h"

namespace Rune {
    // this class manages the hardware and state of the blaster
    class Blaster {
        public:
        Rune::States logicLines;
        Rune::Config* cfg;
        std::vector<Debounce::Button> switches;
        PusherGeneric* pusher;

        Blaster(Rune::Config* config);
        bool init(HW::Board* board);
        void updateIO();
    };
}