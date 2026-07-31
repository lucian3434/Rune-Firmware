#pragma once

#include <stdint.h>
#include "hardware/adc.h"

namespace ADC {
    class ADC {
        protected:
            uint8_t channels[5] = {0};
        public:
            ADC();
            void init(); 
            void addPin(uint8_t channel, uint8_t channelName);
            void removePin(uint8_t channelName);
            uint16_t readPin(uint8_t channelName);
    };
}