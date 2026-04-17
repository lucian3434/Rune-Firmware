#include "blaster.h"

Rune::Blaster::Blaster(Rune::Config* config) {
  logicLines = Rune::States();
  cfg = config;
  switches = {};
}

void Rune::Blaster::init(HW::Board* board) {
  //cfg->load();

  for (uint8_t i = 0; i < cfg->io_switches.size(); i++) {
    switches.push_back(Debounce::Button(board->io[cfg->io_switches[i].IO], cfg->io_switches[i].pullup, cfg->io_switches[i].invert));
    switches[i].init();

    switch (cfg->io_switches[i].function) {
      case Rune::Config::TRIG:
        logicLines.virtTrig.attach(&switches[i]);
        break;
      case Rune::Config::REV:
        logicLines.virtRev.attach(&switches[i]);
        break;
      default:
        break; // other functions can be implemented as needed
    }

    // initialize DRV for use with a given pusher
    switch (cfg->pusher_type)
    {
    case Rune::Config::PUSHER_SCOTCH_YOKE:
    case Rune::Config::PUSHER_BASIC_SOLENOID:
      
      break;
    
    default:
      break;
    }
  }
}