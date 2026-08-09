#include "blaster.h"
#include "./../util.h"

Rune::Blaster::Blaster(Rune::Config* config) {
  logicLines = Rune::States();
  cfg = config;
  switches = {};
  selectors = {};
  fireModes = {};
  motors = {};
  motorPIDs = {};
  currFireMode = nullptr;
  pusher = nullptr;
  fpsCap = 0;
  adc = ADC::ADC();
}

bool Rune::Blaster::init(HW::Board* board) {
  // initialize hardware then build software layers on top

  // initialize io
  
  // adc
  adc.init();
  // initialize a few channels by default
  uint8_t adcSlice = getADCSlice(board, HW::BATT_SENSE);
  if (adcSlice != HW::NO_ASSIGNMENT) {
    adc.addPin(adcSlice, HW::BATT_SENSE);
    uprintf("ADC channel %u -> BATT_SENSE\r\n", adcSlice);
  }
  adcSlice = getADCSlice(board, HW::DRV_IPROPI);
  if (adcSlice != HW::NO_ASSIGNMENT) {
    adc.addPin(adcSlice, HW::DRV_IPROPI);
    uprintf("ADC channel %u -> DRV_IPROPI\r\n", adcSlice);
  }
  adcSlice = getADCSlice(board, HW::ESC_CURR_SENSE);
  if (adcSlice != HW::NO_ASSIGNMENT) {
    adc.addPin(adcSlice, HW::ESC_CURR_SENSE);
    uprintf("ADC channel %u -> ESC_CURR_SENSE\r\n", adcSlice);
  }
  // anything on IO can be added later if needed


  // whatever switches are configured
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
      selectors.push_back(&switches[i]);
      uprintf("SELECT ");
    }

    uprintf("\r\n");
  }

  // initialize software layer
  
  // initialize fire modes
  fireModes.reserve(cfg->fire_modes.size());
  uprintf("Fire Modes:\r\n");
  for (uint8_t i = 0; i < cfg->fire_modes.size(); i++) {
    switch (cfg->fire_modes[i]) {
      case Rune::Config::FM_SEMI:
        fireModes.push_back(new Rune::FireModeSemi());
        uprintf("Semi ");
        break;
      case Rune::Config::FM_BURST:
        fireModes.push_back(new Rune::FireModeBurst(3, true));
        uprintf("Burst ");
        break;
      case Rune::Config::FM_FULL:
        fireModes.push_back(new Rune::FireModeFull());
        uprintf("FullAuto ");
        break;
      default:
        break;
    }
  }
  uprintf("\r\n");

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
        uprintf("ERROR: No cycle switch configured for scotch yoke pusher\r\n");
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
    
    case Rune::Config::PUSHER_BASIC_SOLENOID: {
      if (board->pusher_driver == HW::DRV824XS) {
        pusher = new Rune::SolenoidPusher(&currFireMode, cfg, board->pusher_module);
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

    case Rune::Config::PUSHER_CURRENT_SENSE_SOLENOID: {
      if (board->pusher_driver == HW::DRV824XS) {
        pusher = new Rune::PusherSolenoidCurrSensing(&currFireMode, cfg, &adc, board->pusher_module);
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
    
    default:
      break;
  }
  if (pusher != nullptr) {
    pusher->init();
  }

  // initialize motors
  motors.reserve(cfg->motors.size());
  motorPIDs.reserve(cfg->motors.size());
  for (uint8_t i = 0; i < cfg->motors.size(); i++) {
    motorPIDs.push_back(new PID);
    switch (cfg->motors[i].type) {
      case Rune::Config::MOTOR_BIDSHOT:
        motors.push_back(new Motor::BIDSHOTMotor(board->escs[cfg->motors[i].channel], pio0, Motor::BDSBitrate::DSHOT600, cfg->motors[i].poles));
        initPID(motorPIDs[i], cfg->motors[i].pidConfig.p, cfg->motors[i].pidConfig.i, cfg->motors[i].pidConfig.d);
        uprintf("Motor %u -> BIDSHOT on channel %u\r\n", i + 1, cfg->motors[i].channel);
        break;
      default:
        uprintf("ERR: Unsupported motor type %u on channel %u\r\n", cfg->motors[i].type, cfg->motors[i].channel);
        return false;
    }
    motors[i]->init();
  }

  // get current fps cap from selector position if using a slide selector
  fpsCap = 0;
  updateIO();
  if (cfg->selector_type == Rune::Config::SELECTOR_SLIDE) {
    for (uint8_t i = 0; i < selectors.size(); i++) {
        fpsCap |= selectors[i]->isPressed() << i;
      }
  }
  uprintf("Using FPS cap %u\r\n", fpsCap);

  uprintf("Current battery voltage: %.2fV\r\n", adc.readValue(HW::BATT_SENSE) * board->batteryVoltageMultiplier);

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