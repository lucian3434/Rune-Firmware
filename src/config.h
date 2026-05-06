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
        TRIG = 1,
        REV = 2,
        CYCLE = 4,
        SELECT = 8,
        IDLE = 16
      };

      enum pusherType_t {
        NO_PUSHER = 0,
        PUSHER_SCOTCH_YOKE = 1,
        PUSHER_BASIC_SOLENOID = 2
      };

      enum selectorType_t {
        NO_SELECTOR = 0,
        SELECTOR_SLIDE = 1,
        SELECTOR_PUSH = 2, // not supported
      };

      struct IOSwitchConfig {
        switchFunction_t function;
        uint8_t IO;
        bool pullup;
        bool invert;
      };

      enum motorType_t {
        MOTOR_DSHOT = 0, // not supported
        MOTOR_BIDSHOT = 1
      };

      struct MotorPIDConfig {
        float p;
        float i;
        float d;
      };

      struct MotorConfig {
        uint8_t channel;
        motorType_t type;
        uint8_t poles;
        MotorPIDConfig pidConfig;
      };

      enum fireMode_t {
        FM_SAFETY = 0,
        FM_SEMI = 1,
        FM_BURST = 2,
        FM_FULL = 3
      };

      std::vector<IOSwitchConfig> io_switches;
      selectorType_t selector_type;
      std::vector<MotorConfig> motors;
      HW::boardVersion_t board_name;
      pusherType_t pusher_type;
      std::vector<std::vector<uint32_t>> motor_rpm_targets; // [cap][motor]
      std::vector<fireMode_t> fire_modes;

      bool load();
      Config();
  };
}