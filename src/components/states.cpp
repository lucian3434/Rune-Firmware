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

// this is probably pretty sketchy honestly
// this is included so that a fire mode can keep the rev line high if it is released while firing
// however a more elegant solution would probably just be to poll the fire mode along with the switches
// though im hesitant to say that is a truly elegant way of doing things itself
void Rune::VirtualLogicLine::forceHigh() {
    state |= 0x1;
}