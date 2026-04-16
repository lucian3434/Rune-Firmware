#pragma once

// which board to use
#define RUNE_V0_3


// ########### Board definitions below ########### //


// pin definitions for use with a pico
#ifdef PICO_BOARD

// fill in your pin assignments here

#endif

// pin definitions for Rune V0.1
#ifdef RUNE_V0_1

#define ESC_M1 0
#define ESC_M2 1
#define ESC_M3 2
#define ESC_M4 3

#define ESC_TELEM 4

#define I2C_SCL 5
#define I2C_SDA 6

#define IO2 7
#define IO5 8
#define IO6 9
#define IO1 10
#define IO3 11
#define IO4 12

// a few unused pins here

#define DRV_NSLEEP 17
#define DRV_PH 18
#define DRV_EN 19
#define DRV_NFAULT 20
#define DRV_DRVOFF 21

// and a few more unused pins

#define ESC_ENABLE 25

// ADC pins
#define ADC_GPIO_OFFSET 26
#define ADC_BATT 26 // Battery voltage, ADC0
#define ADC_CURR_SENSE 27 // ESC current, ADC1
#define ADC_DRV_IPROPI 28 // DRV full bridge current, ADC2

#endif

#ifdef RUNE_V0_2

#define ESC_M1 0
#define ESC_M2 1
#define ESC_M3 2
#define ESC_M4 3

#define ESC_TELEM 4

#define I2C_SCL 5
#define I2C_SDA 6

#define IO2 7
#define IO5 8
#define IO6 9
#define IO1 10
#define IO3 11
#define IO4 12

// a few unused pins here

#define DRV_NSLEEP 16
#define DRV_EN 17
#define DRV_PH 18
#define DRV_MOSI 19
#define DRV_MISO 20
#define DRV_NSCS 21
#define DRV_SCLK 22

// one unused here

#define LED_DATA 24

#define ESC_ENABLE 25

// ADC pins
#define ADC_GPIO_OFFSET 26
#define ADC_BATT 26 // Battery voltage, ADC0
#define ADC_CURR_SENSE 27 // ESC current, ADC1
#define ADC_DRV_IPROPI 28 // DRV full bridge current, ADC2

#endif

#ifdef RUNE_V0_3

#define ESC_M1 0
#define ESC_M2 1
#define ESC_M3 2
#define ESC_M4 3

#define ESC_TELEM 4

// one unused here

#define I2C_SCL 6
#define I2C_SDA 7

#define IO2 8
#define IO5 9
#define IO6 10
#define IO3 11
#define IO4 12
#define IO7 13
#define IO8 14

// one unused here

#define DRV_NSLEEP 16
#define DRV_EN 17
#define DRV_PH 18
#define DRV_MOSI 19
#define DRV_MISO 20
#define DRV_NSCS 21
#define DRV_SCLK 22

// one unused here

#define LED_DATA 24

#define ESC_ENABLE 25

// ADC pins
#define ADC_GPIO_OFFSET 26
#define IO1 26 // IO pin connected to ADC, ADC0
#define ADC_DRV_IPROPI 27 // DRV full bridge current, ADC1
#define ADC_BATT 28 // Battery voltage, ADC2
#define ADC_CURR_SENSE 29 // ESC current, ADC3

#endif