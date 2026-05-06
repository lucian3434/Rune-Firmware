#pragma once

#include "firemode.h"

namespace Rune {
    class FireModeSemi : public FireModeGeneric {
        private:
            bool queued;
        public:
            FireModeSemi();
            void tick(States* states, PusherGeneric* pusher);
    };
}