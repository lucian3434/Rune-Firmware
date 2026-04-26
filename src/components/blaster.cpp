#include "blaster.h"
#include "./../util.h"

Rune::Blaster::Blaster(Rune::Config* config) {
  logicLines = Rune::States();
  cfg = config;
  switches = {};
  pusher = nullptr;
}

bool Rune::Blaster::init(HW::Board* board) {
  // initialize io
  for (uint8_t i = 0; i < cfg->io_switches.size(); i++) {
    uprintf("IO %u -> ", cfg->io_switches[i].IO);
    uint8_t pin = board->io[cfg->io_switches[i].IO];
    bool pullup = cfg->io_switches[i].pullup;
    bool invert = cfg->io_switches[i].invert;
    switches.push_back(Debounce::Button(pin, invert, pullup));
    switches[i].init();

    // attach to virtual logic line if applicable
    switch (cfg->io_switches[i].function) {
      case Rune::Config::TRIG:
        logicLines.virtTrig.attach(&switches[i]);
        uprintf("TRIG");
        break;
      case Rune::Config::REV:
        logicLines.virtRev.attach(&switches[i]);
        uprintf("REV");
        break;
      default:
        break; // other functions can be implemented as needed
    }

    uprintf("\r\n");
  }

  // initialize pusher
  switch (cfg->pusher_type) {
    case Rune::Config::PUSHER_SCOTCH_YOKE: {
      // get the cycle switch
      Debounce::Button* cycle = nullptr;
      for (uint8_t i = 0; i < cfg->io_switches.size(); i++) {
        if (cfg->io_switches[i].function == Rune::Config::CYCLE) {
          cycle = &switches[i];
          break;
        }
      }

      // check to make sure that we actually found one
      if (cycle == nullptr) {
        uprintf("ERR: No cycle switch configured for scotch yoke pusher\r\n");
        return false;
      }

      // create pusher object
      Rune::PusherScotchYoke p = Rune::PusherScotchYoke(cycle);
      pusher = &p;
      break;
    }
    /*
    case Rune::Config::PUSHER_BASIC_SOLENOID:
      Rune::SolenoidPusher p = Rune::SolenoidPusher();
      pusher = &p;
      break; //*/
    default:
      break;
  }
  if (pusher != nullptr) {
    pusher->init();
  }

  return true; // successful initialization
}