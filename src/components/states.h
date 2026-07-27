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
            WHEEL_STOPPED = 0,
            WHEEL_ACCELERATING = 1,
            WHEEL_SLOWING = 2,
            WHEEL_IDLE = 3
        } wheelState_t;
        
        wheelState_t wheelState;
        volatile bool wheelsAtSpeed;
        VirtualLogicLine virtRev;
        VirtualLogicLine virtTrig;
        // more can be added as necessary
        States();

        
    };
}