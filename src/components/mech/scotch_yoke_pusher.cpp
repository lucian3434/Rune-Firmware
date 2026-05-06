#include "scotch_yoke_pusher.h"
#include "./../fire control/firemode.h"

Rune::PusherScotchYoke::PusherScotchYoke(FireModeGeneric** firemode_curr, Debounce::Button* cycleSwitch, DRV::DRV824xS* drv) {
  firemode = firemode_curr;
  cycle = cycleSwitch;
  driver = drv;
  psTimeout = NONE;
  pusherState = STOPPED;
  stopOnCycle = true;
}

bool Rune::PusherScotchYoke::init() {
  driver->init();
  cycle->init();
  return true;
}

void Rune::PusherScotchYoke::pusherTick() {
  if (pusherState == RUNNING) {
    if (cycle->isRisingEdge()) {
      // increment shots fired
      (*firemode)->shotsFired++;

      if (stopOnCycle) {
        pusherState = STOPPED;
        driver->brake();
      }

      ulogf("INFO: Cycle switch pressed\r\n");
    }
    else {
      driver->drive();
    }
  }   
}

void Rune::PusherScotchYoke::startPusher(bool single) {
  pusherState = RUNNING;
  stopOnCycle = single;

  // make sure safety timeout won't stop us early
  if (psTimeout == WAITING) {
    cancel_alarm(pusherSafetyAlarmId);
    psTimeout = NONE;
  }

  // then set the safety again if we're in single shot mode since stopPusher() might not be called
  if (single) {
    startSafetyTimer();
  }
}

int64_t Rune::PusherScotchYoke::pusherSafetyCallback(alarm_id_t id, void* userData) {
  Rune::PusherScotchYoke* self = static_cast<Rune::PusherScotchYoke*>(userData);
  if (self->pusherState == RUNNING) {
    self->pusherState = STOPPED;
    self->driver->brake();
    ulogf("WARNING: Pusher safety timeout triggered. Check your pusher and cycle switch.\r\n");
  }
  return 0;
}

void Rune::PusherScotchYoke::stopPusher() {
  stopOnCycle = true;
  startSafetyTimer();
}

void Rune::PusherScotchYoke::startSafetyTimer() {
  // cancel existing alarm if it's already running
  if (psTimeout != NONE) {
    cancel_alarm(pusherSafetyAlarmId);
    psTimeout = NONE;
  }

  // schedule safety timeout
  pusherSafetyAlarmId = add_alarm_in_ms(200, pusherSafetyCallback, this, false);
  if (pusherSafetyAlarmId >= 0) {
    psTimeout = WAITING;
  }
  else {
    ulogf("ERROR: Failed to start pusher safety timer\r\n");
  }
}