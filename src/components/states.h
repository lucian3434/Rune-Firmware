#pragma once

#include "debounce/button.h"
#include <vector>

namespace Rune {
    class States {
        public:
        VirtualLogicLine virtRev;
        VirtualLogicLine virtTrig;
        // more can be added as necessary
        States();
    };

    class VirtualLogicLine {
        private:
        std::vector<Debounce::Button*> switches;

        public:
        VirtualLogicLine();
        void attach(Debounce::Button* ioswitch);
        bool getState();
    };
}