#include "solenoid_pusher.h"
#include "./../fire control/firemode.h"

Rune::SolenoidPusher::SolenoidPusher(FireModeGeneric*** firemode_curr, Rune::Config* config, DRV::DRV824xS* drv) {
  firemode = firemode_curr;
  cfg = config;
  driver = drv;
  pusherState = STOPPED;
  solenoidState = RETRACTED;
}

// returns true if the pusher module was successfully initialized
bool Rune::SolenoidPusher::init() {
  driver->init();
  return true;
}

void Rune::SolenoidPusher::pusherTick() {

  // state logic
  if (time_reached(solenoidNextStateChange)) {
    solenoidLastStateChange = get_absolute_time();
    switch (solenoidState) {
      case EXTENDING:
        (**firemode)->shotsFired++;
        solenoidState = RETRACTING;
        solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange, cfg->solOffTimems);
        break;
      case RETRACTING:
        if (stopOnCycle) {
          solenoidState = RETRACTED;
          pusherState = STOPPED;
        }
        else {
          solenoidState = EXTENDING;
          solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange, cfg->solOnTimems);
        }
      default:
        solenoidState = RETRACTING; // shouldn't end up here? but just in case
        pusherState = STOPPED;
        break;
    }
  }
  
  if (solenoidState == EXTENDING) {
    driver->drive();
  }
  else {
    driver->coast();
  }
}

void Rune::SolenoidPusher::startPusher(bool single) {
  pusherState = RUNNING;
  solenoidState = EXTENDING;
  solenoidLastStateChange = get_absolute_time();
  solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange, cfg->solOnTimems);
  stopOnCycle = single;
}

void Rune::SolenoidPusher::stopPusher() {
  stopOnCycle = true;
}