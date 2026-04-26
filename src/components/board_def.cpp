#include "board_def.h"

// consider renaming
uint8_t HW::getADCSlice(HW::Board* board, adcFunction_t func) {
    for (uint8_t i = 0; i < 4; i++) {
        if (board->adc[i] == func) return i;
    }
    return 0xff; // not found
}


HW::Board HW::rune_v0_2 = {
    .escs = {0, 1, 2, 3},
    .esc_enable = 25,
    .esc_telem = 4,
    .I2C_scl = 5,
    .I2C_sda = 6,
    .io = {HW::NO_ASSIGNMENT, 10, 7, 11, 12, 8, 9},
    .DRV_nsleep = 16,
    .DRV_en = 17,
    .DRV_ph = 18,
    .DRV_mosi = 19,
    .DRV_miso = 20,
    .DRV_cs = 21,
    .DRV_sclk = 22,
    .ws2812_data = 24,
    .adc = {HW::BATT_SENSE, HW::ESC_CURR_SENSE, HW::DRV_IPROPI, HW::NC}
};

HW::Board HW::rune_v0_3 = {
    .escs = {0, 1, 2, 3},
    .esc_enable = 25,
    .esc_telem = 4,
    .I2C_scl = 6,
    .I2C_sda = 7,
    .io = {HW::NO_ASSIGNMENT, 26, 8, 11, 12, 9, 10, 13, 14},
    .DRV_nsleep = 16,
    .DRV_en = 17,
    .DRV_ph = 18,
    .DRV_mosi = 19,
    .DRV_miso = 20,
    .DRV_cs = 21,
    .DRV_sclk = 22,
    .ws2812_data = 24,
    .adc = {HW::IO1, HW::DRV_IPROPI, HW::BATT_SENSE, HW::ESC_CURR_SENSE}
};

HW::Board* HW::boards[] = {
    &HW::rune_v0_2,
    &HW::rune_v0_3
};