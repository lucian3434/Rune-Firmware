#include "api.h"

// api version defined in api.h

Rune::SerialAPI::SerialAPI(Rune::Blaster* blaster) {
    obj = blaster;
    rxbuf = "";
    resetState();
}

void Rune::SerialAPI::toState(rxState_t newState) {
    rxState = newState;
    rxbuf = "";
}

void Rune::SerialAPI::resetState() {
    rxState = WAITING_FOR_START;
    rxMethod = METHOD_UNKNOWN;
    rxSection = "";
    rxField = "";
    rxRawDataLength = "";
    rxDataLength = 0;
    rxData = {};
    rxbuf = "";
}

void Rune::SerialAPI::reportError(apiError_t err) {
    uprintf("[ERR] %s '%s'\r\n", apiErrorStrings[err].data(), rxbuf.c_str());
}

void Rune::SerialAPI::reportError(apiError_t err, std::string highlight) {
    uprintf("[ERR] %s '%s'\r\n", apiErrorStrings[err].data(), highlight.c_str());
}

void Rune::SerialAPI::sendResponse(std::string data) {
    uprintf("[RES]%s:%s:%s\r\n", rxSection.c_str(), rxField.c_str(), data.c_str());
}

// gets called in main loop to parse messages as they are received
void Rune::SerialAPI::processRX() {
    while (stdio_usb_connected()) { // ignore this code if connected to usb
        int c = 0;
        c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) break; // quit if the serial interface doesn't have anything for us
        rxbuf.push_back((char)c);
        
        // try to parse whatever is in the buffer
        switch (rxState) {
            case WAITING_FOR_START:
                {
                    if (rxbuf.back() == ':') { // api message received
                        if (rxbuf == "GET:") {
                            rxMethod = METHOD_GET;
                            toState(READING_SECTION);
                        }
                        else if (rxbuf == "SET:") {
                            rxMethod = METHOD_SET;
                            toState(READING_SECTION);
                        }
                        else {
                            reportError(API_ERROR_UNKNOWN_METHOD);
                            resetState();
                        }
                    }
                    else if (rxbuf.back() == '\n') { // control word received
                        if (rxbuf == "APIVER\n") {
                            uprintf("APIVER%u\r\n", API_VERSION);
                        }
                        resetState();
                    }
                }
                break;
            
            case READING_SECTION:
                {
                    // wait for segment terminator
                    if (rxbuf.back() == ':') {
                        rxSection = rxbuf.substr(0, rxbuf.length() - 1);
                        toState(READING_FIELD);
                    }
                }
                break;

            case READING_FIELD:
                {
                    // wait for segment terminator
                    if (rxbuf.back() == ':') {
                        rxField = rxbuf.substr(0, rxbuf.length() - 1);
                        if (rxMethod == METHOD_GET) {
                            processInboundMessage();
                            resetState();
                        }
                        else if (rxMethod == METHOD_SET) {
                            toState(READING_DATA_LENGTH);
                        }
                    }
                }
                break;
                
            default:
                reportError(API_ERROR_INVALID_STATE);
                resetState(); // something unexpected happened so reset and try again
                break;
        }
    }
    if (!stdio_usb_connected() && rxState != WAITING_FOR_START) {
        // reset whenever disconnected in case something weird snuck in
        resetState();
    }
}

void Rune::SerialAPI::processInboundMessage() {
    // one of my least favorite parts of C++ is that the switch statement
    // is from C, so you can't do a switch on std::string values
    if (rxSection == "board") {
        if (rxField == "friendlyName") {
            sendResponse(HW::boards[obj->cfg->board_name]->friendlyName);
        }
        else if (rxField == "ver") {
            sendResponse(obj->fwVersionString);
        }
        else {
            reportError(API_ERROR_INVALID_FIELD);
        }
    }
    else {
        reportError(API_ERROR_UNKNOWN_SECTION, rxSection);
    }
}