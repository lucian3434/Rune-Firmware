#include "config.h"

Rune::Config::Config() {
  io_switches = {};
}

bool Rune::Config::load() {
  // check flash for a valid config
  if (false) {
    // implement loading config from flash here
    return true;
  }
  else {
    // load defaults
    board_name = HW::RUNE_V0_2;

    // load default switch configuration
    io_switches.push_back({.function = TRIG, .IO = 2, .pullup = true, .invert = true});
    io_switches.push_back({.function = REV, .IO = 3, .pullup = true, .invert = true});
    io_switches.push_back({.function = SELECT, .IO = 6, .pullup = true, .invert = true});
    io_switches.push_back({.function = SELECT, .IO = 5, .pullup = true, .invert = true});

    pusher_type = PUSHER_SCOTCH_YOKE;
  }

  return false;
}