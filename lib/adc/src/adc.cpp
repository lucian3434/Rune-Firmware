#include "adc/adc.h"
#include "pico/stdlib.h"

ADC::ADC::ADC() {
}

void ADC::ADC::init() {
    adc_init();
    adc_set_clkdiv(0.f);
}

void ADC::ADC::addPin(uint8_t channel, uint8_t channelName) {
    if (channel == 4) {
        adc_set_temp_sensor_enabled(true);
    } else {
        adc_gpio_init(channel + 26);
    }
    channels[channel] = channelName;
}

void ADC::ADC::removePin(uint8_t channelName) {
    for (uint8_t i = 0; i < 5; i++) {
        if (channels[i] == channelName) {
            channels[i] = 0;
            if (i == 4) {
                adc_set_temp_sensor_enabled(false);
            } else {
                gpio_deinit(i + 26);
            }
            break;
        }
    }
}

uint16_t ADC::ADC::readPin(uint8_t channelName) {
    for (uint8_t i = 0; i < 5; i++) {
        if (channels[i] == channelName) {
            adc_select_input(i);
            return adc_read();
        }
    }
    return 0; // channel not found
}