#pragma once

#include "util.h"
#include "pico/stdlib.h"
#include <string>
#include <string_view>
#include <vector>

#include "components/blaster.h"
#include "components/board_def.h"

#define API_VERSION 0

// inbound packet formats:
// GET:section:field:
// SET:section:field:datalength:data

namespace Rune {
    class SerialAPI {
        private:
            std::string rxbuf;
            Rune::Blaster* obj;

            enum apiError_t {
                API_ERROR_UNKNOWN_METHOD = 0,
                API_ERROR_UNKNOWN_SECTION = 1,
                API_ERROR_INVALID_FIELD = 2,
                API_ERROR_ZERO_LENGTH = 3,
                API_ERROR_INVALID_STATE = 4
            };
            static constexpr std::string_view apiErrorStrings[] = {
                "Unknown Method",
                "Unknown Section",
                "Invalid Field",
                "Zero Data Length",
                "Serial API entered invalid state"
            };

            enum rxState_t {
                WAITING_FOR_START,
                READING_METHOD, // unnecessary?
                READING_SECTION,
                READING_FIELD,
                READING_DATA_LENGTH,
                READING_DATA
            } rxState;

            enum rxMethod_t {
                METHOD_UNKNOWN,
                METHOD_GET,
                METHOD_SET
            } rxMethod;

            std::string rxSection;
            std::string rxField;
            std::string rxRawDataLength;
            uint16_t rxDataLength;
            std::vector<uint8_t> rxData;

            void toState(rxState_t newState);
            void resetState();
            void processInboundMessage();
            void reportError(apiError_t err);
            void reportError(apiError_t err, std::string highlight);
            void sendResponse(std::string data);


        public:
            SerialAPI(Rune::Blaster* blaster);
            void processRX();
    };
}