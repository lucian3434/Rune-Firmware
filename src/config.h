#pragma once

#include "components/board_def.h"
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <stdbool.h>

namespace Rune {
  class Config {
    public:
      enum switchFunction_t {
        TRIG = 0,
        REV = 1,
        CYCLE = 2,
        SELECT = 3
      };

      enum pusherType_t {
        NO_PUSHER = 0,
        PUSHER_SCOTCH_YOKE = 1,
        PUSHER_BASIC_SOLENOID = 2
      };

      struct IOSwitch {
        switchFunction_t function;
        uint8_t IO;
        bool pullup;
        bool invert;
      };

      std::vector<IOSwitch> io_switches;
      HW::boardVersion_t board_name;
      pusherType_t pusher_type;

      bool load();
      Config();
  };
}