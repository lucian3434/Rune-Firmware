#pragma once

#include "firemode.h"

namespace Rune {
    class FireModeBurst : public FireModeGeneric {
        private:
            uint8_t queued;
            bool cutoff;
            uint8_t size;
        public:
            FireModeBurst() = default;
            FireModeBurst(uint8_t burstSize, bool allowPartial);
            void tick(States* states, PusherGeneric* pusher) override;
    };
}