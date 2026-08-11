#include "api.h"
#include "string.h"

char apibuf[API_BUFFER_SIZE] = {};
uint16_t apibuf_idx = 0;

void processRX() {
    while (stdio_usb_connected()) {
        int c = 0;
        c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) return;
        //uprintf("char: %hu\r\n", c);
        if (c == '\n') {
            apibuf[apibuf_idx] = 0x00;
            if (strcmp(apibuf, "APIVER") == 0) {
                ulogf("APIVER0\r\n");
            }
            else {
                // do something with whatever is in the buffer
                
            }
            // clear index for buffer reuse
            apibuf_idx = 0;
        }
        else {
            apibuf[apibuf_idx] = (char)c;
            apibuf_idx++;
            if (apibuf_idx >= API_BUFFER_SIZE) {
                ulogf("[ERR] API buffer overflow\r\n");
                apibuf_idx = 0;
            }
        }
    }
}