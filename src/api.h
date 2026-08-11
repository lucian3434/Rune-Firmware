#pragma once

#include "util.h"
#include "pico/stdlib.h"

#define API_VERSION 0
#define API_BUFFER_SIZE 128

extern char apibuf[API_BUFFER_SIZE];
extern uint16_t apibuf_idx;

void processRX();