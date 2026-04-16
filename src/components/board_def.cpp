#include "board_def.h"

HW::Board::Board() {}

uint8_t HW::Board::getADCInput(adcFunction_t func) {
    for (uint8_t i = 0; i < 4; i++) {
        if (adc[i] == func) return i;
    }
    return 0xff; // not found
}

HW::Board rune_v0_2 = HW::Board();
rune_v0_2.