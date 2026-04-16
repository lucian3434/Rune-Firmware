#pragma once

#include <vector>


namespace HW {
    enum adcFunction_t {
        NC,
        IO1,
        DRV_IPROPI,
        BATT_SENSE,
        ESC_CURR_SENSE
    };

    class Board {
        public:
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

        uint8_t getADCInput(adcFunction_t func);
        Board();
    };
}

extern HW::Board rune_v0_2;
extern HW::Board rune_v0_3;