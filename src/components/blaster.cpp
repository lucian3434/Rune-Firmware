#include "blaster.h"
#include "./../util.h"

Rune::Blaster::Blaster(Rune::Config* config) {
  logicLines = Rune::States();
  cfg = config;
  switches = {};
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

    uprintf("\r\n");
  }

  // initialize pusher driver
  /* TODO: add support for anything other than a DRV824x
  switch (board->pusher_driver)
  {
  case HW::DRV824XS: */
    DRV::DRV824xS drv = DRV::DRV824xS(board->DRV_ph, board->DRV_en, board->DRV_nsleep, board->DRV_mosi, board->DRV_miso, board->DRV_cs, board->DRV_sclk, spi0);
    /*
    break;
  
  default:
    break;
  }
  */

  // hardware initialization complete, move on to software layer
  // initialize fire modes
  Rune::FireModeGeneric* curr_mode;

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
      pusher = new Rune::PusherScotchYoke(&curr_mode, cycle, &drv);
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