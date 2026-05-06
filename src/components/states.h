#pragma once

#include "debounce/button.h"
#include <vector>

namespace Rune {

    class VirtualLogicLine {
        private:
        std::vector<Debounce::Button*> switches;
        uint8_t state;

        public:
        VirtualLogicLine();
        void attach(Debounce::Button* ioswitch);
        bool getState();
        bool isRisingEdge();
        bool isFallingEdge();
        void update();
        void forceHigh();
    };
    
    class States {
        public:
        typedef enum {
            WHEEL_STOPPED,
            WHEEL_ACCELERATING,
            WHEEL_SLOWING,
            WHEEL_IDLE
        } wheelState_t;
        
        wheelState_t wheelState;
        bool wheelsAtSpeed;
        VirtualLogicLine virtRev;
        VirtualLogicLine virtTrig;
        // more can be added as necessary
        States();

        
    };
}