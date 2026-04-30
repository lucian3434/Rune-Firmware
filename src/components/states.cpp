#include "states.h"

Rune::States::States() {
    wheelState = WHEEL_STOPPED;
    wheelsAtSpeed = false;
    virtRev = Rune::VirtualLogicLine();
    virtTrig = Rune::VirtualLogicLine();
}

Rune::VirtualLogicLine::VirtualLogicLine() {
}

void Rune::VirtualLogicLine::attach(Debounce::Button* ioswitch) {
    switches.push_back(ioswitch);
}

void Rune::VirtualLogicLine::update() {
    state <<= 1;
    for (uint8_t i = 0; i < switches.size(); i++) {
        if (switches[i]->isPressed()) state |= 0x01;
    }
}

bool Rune::VirtualLogicLine::getState() {
    return (bool)(state & 0x01);
}

bool Rune::VirtualLogicLine::isRisingEdge() {
    return (state & 0x03) == 0x01;
}

bool Rune::VirtualLogicLine::isFallingEdge() {
    return (state & 0x03) == 0x02;
}