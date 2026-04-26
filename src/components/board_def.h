#pragma once

#include <vector>
#include <stdint.h>

namespace HW {
    const uint8_t NO_ASSIGNMENT = 0xff;

    enum adcFunction_t {
        NC,
        IO1,
        DRV_IPROPI,
        BATT_SENSE,
        ESC_CURR_SENSE
    };

    enum boardVersion_t {
        RUNE_V0_2 = 0,
        RUNE_V0_3 = 1
    };

    struct Board {
        std::vector<uint8_t> escs;
        uint8_t esc_enable;
        uint8_t esc_telem;

        uint8_t I2C_scl;
        uint8_t I2C_sda;

        std::vector<uint8_t> io;

        // this will need work to support other boards
        uint8_t DRV_nsleep;
        uint8_t DRV_en;
        uint8_t DRV_ph;
        uint8_t DRV_mosi;
        uint8_t DRV_miso;
        uint8_t DRV_cs;
        uint8_t DRV_sclk;

        uint8_t ws2812_data;
        
        adcFunction_t adc[4];
    };

    extern Board rune_v0_2;
    extern Board rune_v0_3;

    extern Board* boards[2];

    uint8_t getADCSlice(HW::Board* board, adcFunction_t func);
}
