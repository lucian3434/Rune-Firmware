#pragma once

#include "pico/stdlib.h"
#include <stdio.h>
#include <stdarg.h>
#include <atomic>

#define LOG_MSG_LENGTH 64
#define LOG_CACHE_SIZE 128 // only supports powers of 2

typedef struct {
    std::atomic<bool> ready;
    char msg[LOG_MSG_LENGTH]; 
} ringbufslot_t;

extern ringbufslot_t logbuf[LOG_CACHE_SIZE];
extern std::atomic<uint32_t> logbuf_drops;
extern std::atomic<uint32_t> logbuf_head;
extern uint32_t logbuf_tail;

// prints directly to console (DO NOT USE IN TIMERS/ISRS)
void uprintf(const char* format, ...);

// prints to a cache (much faster)
void ulogf(const char* format, ...);

void printLogBuffer();

uint32_t getDrops();