#include "config.h"

Rune::Config::Config() {
  io_switches = {};
  motors = {};
  selector_type = NO_SELECTOR;
  pusher_type = NO_PUSHER;
}

bool Rune::Config::load() {
  // check flash for a valid config
  // ... in the future, when that is implemented. for now just load defaults
  bool successfullyLoaded = false;
  if (successfullyLoaded) { 
    // implement loading config from flash here
    
  }
  else {
    // load defaults
    board_name = HW::RUNE_V0_2;

    // load default switch configuration
    io_switches.push_back({.function = (Rune::Config::switchFunction_t)(TRIG | REV), .IO = 2, .pullup = true, .invert = true});
    //io_switches.push_back({.function = REV, .IO = 1, .pullup = true, .invert = false});
    io_switches.push_back({.function = CYCLE, .IO = 1, .pullup = true, .invert = true});
    io_switches.push_back({.function = SELECT, .IO = 6, .pullup = true, .invert = true});
    io_switches.push_back({.function = SELECT, .IO = 5, .pullup = true, .invert = true});

    // default selector type
    selector_type = SELECTOR_SLIDE;

    // load default pusher configuration
    pusher_type = PUSHER_SCOTCH_YOKE;

    // load default motor configuration
    MotorPIDConfig pidset = {.p = 0.00025, .i = 0.0000001, .d = -0.001};
    motors.push_back({.channel = 1, .type = MOTOR_BIDSHOT, .poles = 14, .pidConfig = pidset});
    motors.push_back({.channel = 2, .type = MOTOR_BIDSHOT, .poles = 14, .pidConfig = pidset});

    // load motor rpm targets
    // this is probably overly simplistic
    uint32_t rpmTargets[] = {19500, 24000, 37000};
    for (uint8_t cap = 0; cap < 3; cap++) {
      motor_rpm_targets.push_back({});
      for (uint8_t motor = 0; motor < motors.size(); motor++) {
        motor_rpm_targets[cap].push_back(rpmTargets[cap]);
      }
    }

    // load fire modes
    fire_modes.push_back(FM_BURST);
    fire_modes.push_back(FM_SEMI);
    fire_modes.push_back(FM_FULL);
  }

  return successfullyLoaded;
}