#include "config.h"

Rune::Config::Config() {
  io_switches = {};
  motors = {};
  selector_type = NO_SELECTOR;
  pusher_type = NO_PUSHER;

  solOnTimems = 25; // typical safe settings
  solOffTimems = 65;
  
  rampDownTimems = 1000;
  idletimems = 0;
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
    io_switches.push_back({.function = (Rune::Config::switchFunction_t)(TRIG), .IO = 2, .pullup = true, .invert = true});
    io_switches.push_back({.function = REV, .IO = 1, .pullup = true, .invert = false});
    //io_switches.push_back({.function = CYCLE, .IO = 1, .pullup = true, .invert = true});
    io_switches.push_back({.function = SELECT, .IO = 6, .pullup = true, .invert = true});
    io_switches.push_back({.function = SELECT, .IO = 5, .pullup = true, .invert = true});

    // default selector type
    selector_type = SELECTOR_SLIDE;

    // load default pusher configuration
    //pusher_type = PUSHER_SCOTCH_YOKE;
    pusher_type = PUSHER_BASIC_SOLENOID;
    solOnTimems = 25; // typical safe settings
    solOffTimems = 65;

    // load default motor configuration
    //MotorPIDConfig pidset = {.p = 0.00025, .i = 0.0000001, .d = -0.001};
    MotorPIDConfig pidset = {.p = 0.00025, .i = 0.0000001, .d = 0.0};
    motors.push_back({.channel = 1, .type = MOTOR_BIDSHOT, .poles = 14, .pidConfig = pidset});
    motors.push_back({.channel = 2, .type = MOTOR_BIDSHOT, .poles = 14, .pidConfig = pidset});

    // load motor rpm targets
    // this is probably overly simplistic
    uint32_t rpmTargets[] = {24000, 19500, 37000};
    //uint32_t rpmTargets[] = {10000, 10000, 10000};
    for (uint8_t cap = 0; cap < 3; cap++) {
      motor_rpm_targets.push_back({});
      for (uint8_t motor = 0; motor < motors.size(); motor++) {
        motor_rpm_targets[cap].push_back(rpmTargets[cap]);
      }
    }
    for (uint8_t motor = 0; motor < motors.size(); motor++) {
      idle_rpm_targets.push_back(2000);
    }

    // load fire modes
    fire_modes.push_back(FM_BURST);
    fire_modes.push_back(FM_SEMI);
    fire_modes.push_back(FM_FULL);

    // load default ramp down/idle times
    rampDownTimems = 500;
    idletimems = 10000;
  }

  return successfullyLoaded;
}