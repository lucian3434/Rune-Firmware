#include "util.h"

ringbufslot_t logbuf[LOG_CACHE_SIZE];
std::atomic<uint32_t> logbuf_drops = 0;
std::atomic<uint32_t> logbuf_head = 0;
uint32_t logbuf_tail = 0;

// because printf() over usb blocks if a serial connection isnt active apparently
void uprintf(const char* format, ...) {
    if (!stdio_usb_connected()) {
        return;  // don't do anything if usb isn't connected
    }
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void ulogf(const char* format, ...) {
    // get an index
    uint32_t index = logbuf_head.fetch_add(1, std::memory_order_relaxed);

    // drop the message if the buffer is full; this should
    // be rare enough that it doesn't cause issues
    if ((index - logbuf_tail) >= LOG_CACHE_SIZE) {
        logbuf_drops.fetch_add(1, std::memory_order_relaxed);
        return;
    }

    // if we haven't dropped the message, grab a pointer to our slot
    ringbufslot_t* slot = &logbuf[index & (LOG_CACHE_SIZE - 1)];

    // format the message
    va_list args;
    va_start(args, format);
    vsnprintf(slot->msg, LOG_MSG_LENGTH, format, args);
    va_end(args);

    // mark the message as ready
    slot->ready.store(true, std::memory_order_release);
}

// dump cache to console; should be called regularly in main loop
void printLogBuffer() {
    uint32_t lim = LOG_CACHE_SIZE;
    while (lim--) {
        ringbufslot_t* slot = &logbuf[logbuf_tail & (LOG_CACHE_SIZE - 1)];

        if (!slot->ready.load(std::memory_order_acquire)) {
            break; // no more messages to print
        }

        uprintf("%s", slot->msg); // print the message

        // mark the slot as empty
        slot->ready.store(false, std::memory_order_release);
        logbuf_tail++;
    }
}

uint32_t getDrops() {
    return logbuf_drops.load(std::memory_order_relaxed);
}