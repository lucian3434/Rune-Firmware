#include "states.h"

Rune::States::States() {
    virtRev = Rune::VirtualLogicLine();
    virtTrig = Rune::VirtualLogicLine();
}

Rune::VirtualLogicLine::VirtualLogicLine() {
}

void Rune::VirtualLogicLine::attach(Debounce::Button* ioswitch) {
    switches.push_back(ioswitch);
}

bool Rune::VirtualLogicLine::getState() {
    bool state = false;
    for (uint8_t i = 0; i < switches.size(); i++) {
        state |= switches[i]->isPressed();
    }
    return state;
}