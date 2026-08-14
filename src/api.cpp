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

void Rune::SerialAPI::sendResponse(std::string section, std::string field, std::string data) {
    uprintf("[RES]%s:%s:%s\r\n", section.c_str(), field.c_str(), data.c_str());
}

bool Rune::SerialAPI::isSection(std::string sec) {
    return rxSection == sec || rxSection == "*";
}

bool Rune::SerialAPI::isField(std::string fie) {
    return rxField == fie || rxField == "*";
}

// because std::toString() uses vsnprintf, it limits float precision when very close to 0
// this is really just std::toString(float __val) modified to make that less of an issue
std::string Rune::SerialAPI::floatToExp(float val) {
    const int __n = 
      __gnu_cxx::__numeric_traits<float>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<std::string>(&std::vsnprintf, __n,
					   "%e", val);
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
    // if/if instead of if/else in case a * is sent for section or field
    if (isSection("board")) {
        if (isField("friendlyName")) {
            sendResponse("board", "friendlyName", HW::boards[obj->cfg->board_name]->friendlyName);
        }
        if (isField("name")) {
            sendResponse("board", "name", std::to_string(obj->cfg->board_name));
        }
        if (isField("ver")) {
            sendResponse("board", "ver", obj->fwVersionString);
        }
        if (isField("bat")) {
            sendResponse("board", "bat", std::to_string(obj->adc.readValue(HW::BATT_SENSE) * HW::boards[obj->cfg->board_name]->batteryVoltageMultiplier));
        }
    }
    if (isSection("motors")) {
        if (isField("num")) {
            sendResponse("motors", "num", std::to_string(obj->cfg->motors.size()));
        }
        if (isField("rampdownms")) {
            sendResponse("motors", "rampdownms", std::to_string(obj->cfg->rampDownTimems));
        }
        if (isField("idletimes")) {
            sendResponse("motors", "idletimes", std::to_string(obj->cfg->idletimems / 1000));
        }
        for (uint8_t i = 0; i < obj->cfg->motors.size(); i++) {
            std::string motor = "motor";
            motor.append(std::to_string(i));
            if (isField(motor)) {
                std::string msgobj = "";
                msgobj += std::to_string(obj->cfg->motors[i].channel) + ",";
                msgobj += std::to_string(obj->cfg->motors[i].type) + ",";
                msgobj += std::to_string(obj->cfg->motors[i].poles) + ",";
                msgobj += floatToExp(obj->cfg->motors[i].pidConfig.p) + ",";
                msgobj += floatToExp(obj->cfg->motors[i].pidConfig.i) + ",";
                msgobj += floatToExp(obj->cfg->motors[i].pidConfig.d);
                sendResponse("motors", motor, msgobj);
            }
        }
    }
    if (isSection("pusher")) {
        if (isField("type")) {
            sendResponse("pusher", "type", std::to_string(obj->cfg->pusher_type));
        }
        if (isField("offms")) {
            sendResponse("pusher", "offms", std::to_string(obj->cfg->solOffTimems));
        }
        if (isField("onms")) {
            sendResponse("pusher", "onms", std::to_string(obj->cfg->solOnTimems));
        }
    }
    if (isSection("io")) {
        if (isField("num")) {
            sendResponse("io", "num", std::to_string(obj->cfg->io_switches.size()));
        }
        if (isField("seltype")) {
            sendResponse("io", "seltype", std::to_string(obj->cfg->selector_type));
        }
        for (uint8_t i = 0; i < obj->cfg->io_switches.size(); i++) {
            std::string io = "io";
            io.append(std::to_string(i));
            if (isField(io)) {
                std::string msgobj = "";
                msgobj += std::to_string(obj->cfg->io_switches[i].function) + ",";
                msgobj += std::to_string(obj->cfg->io_switches[i].IO) + ",";
                msgobj += std::to_string(obj->cfg->io_switches[i].pullup) + ",";
                msgobj += std::to_string(obj->cfg->io_switches[i].invert);
                sendResponse("io", io, msgobj);
            }
        }
    }
    /*
    else {
        reportError(API_ERROR_UNKNOWN_SECTION, rxSection);
    }
    */
}