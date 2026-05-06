#include "blaster.h"
#include "./../util.h"

Rune::Blaster::Blaster(Rune::Config* config) {
  logicLines = Rune::States();
  cfg = config;
  switches = {};
  fireModes = {};
  currFireMode = nullptr;
  pusher = nullptr;
}

bool Rune::Blaster::init(HW::Board* board) {
  // initialize hardware then build software layers on top
  // initialize io
  switches.reserve(cfg->io_switches.size());

  for (uint8_t i = 0; i < cfg->io_switches.size(); i++) {
    uprintf("IO %u -> ", cfg->io_switches[i].IO);
    uint8_t pin = board->io[cfg->io_switches[i].IO];
    bool pullup = cfg->io_switches[i].pullup;
    bool invert = cfg->io_switches[i].invert;
    switches.push_back(Debounce::Button(pin, invert, pullup));
    switches[i].init();

    // attach to virtual logic line if applicable
    if (cfg->io_switches[i].function & Rune::Config::TRIG) {
      logicLines.virtTrig.attach(&switches[i]);
      uprintf("TRIG ");
    }
    if (cfg->io_switches[i].function & Rune::Config::REV) {
      logicLines.virtRev.attach(&switches[i]);
      uprintf("REV ");
    }
    if (cfg->io_switches[i].function & Rune::Config::CYCLE) {
      // connect in pusher init
      uprintf("CYCLE ");
    }
    if (cfg->io_switches[i].function & Rune::Config::SELECT) {
      // add to set of selector switches
      uprintf("SELECT ");
    }

    uprintf("\r\n");
  }

  // initialize software layer
  
  // initialize fire modes
  fireModes.reserve(cfg->fire_modes.size());
  for (uint8_t i = 0; i < cfg->fire_modes.size(); i++) {
    switch (cfg->fire_modes[i]) {
      case Rune::Config::FM_SEMI:
        fireModes.push_back(new Rune::FireModeSemi());
        break;
      case Rune::Config::FM_BURST:
        fireModes.push_back(new Rune::FireModeBurst(3, true));
        break;
      case Rune::Config::FM_FULL:
        fireModes.push_back(new Rune::FireModeFull());
        break;
      default:
        break;
    }
  }
  //currFireMode = &fireModes[0];

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
      if (board->pusher_driver == HW::DRV824XS) {
        pusher = new Rune::PusherScotchYoke(&currFireMode, cycle, board->pusher_module);
      }
      else {
        uprintf("ERROR: Non-DRV824xS pusher not currently supported\r\n");
        uprintf(" - while attempting to initialize pusher object in blaster.cpp\r\n");
        while (true) {
          uprintf("ERR:!DRV\r\n");
          sleep_ms(100);
        }; // loop forever so they know something is Wrong
      }
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

void Rune::Blaster::updateIO() {
  // ensure that all io is updated a single time
  for (Debounce::Button& button : switches) {
    button.update();
  }

  logicLines.virtRev.update();
  logicLines.virtTrig.update();
}