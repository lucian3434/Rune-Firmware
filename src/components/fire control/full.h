#pragma once

#include "firemode.h"

namespace Rune {
    class FireModeFull : public FireModeGeneric {
        private:
            bool queued;
        public:
            FireModeFull();
            void tick(States* states, PusherGeneric* pusher) override;
    };
}