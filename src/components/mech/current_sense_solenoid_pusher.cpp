#include "current_sense_solenoid_pusher.h"
#include "./../fire control/firemode.h"

Rune::PusherSolenoidCurrSensing::PusherSolenoidCurrSensing(FireModeGeneric*** firemode_curr, Rune::Config* config, ADC::ADC *adc, DRV::DRV824xS* drv) {
  firemode = firemode_curr;
  cfg = config;
  adcm = adc;
  driver = drv;
  pusherState = STOPPED;
  solenoidState = RETRACTED;
  resetSenseVariables();
}

// returns true if the pusher module was successfully initialized
bool Rune::PusherSolenoidCurrSensing::init() {
  driver->init();
  return true;
}

void Rune::PusherSolenoidCurrSensing::pusherTick() {

  // state logic
  if (time_reached(solenoidNextStateChange)) {
    solenoidLastStateChange = get_absolute_time();
    switch (solenoidState) {
      case EXTENDING:
        (**firemode)->shotsFired++;
        solenoidState = RETRACTING;
        solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange, cfg->solOffTimems);
        ulogf("WARN: Solenoid did not extend within 200ms safety timeout");
        break;
      case RETRACTING:
        if (stopOnCycle) {
          solenoidState = RETRACTED;
          pusherState = STOPPED;
        }
        else { // continue firing
          solenoidState = EXTENDING;
          solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange, 200);
          resetSenseVariables();
        }
      default:
        solenoidState = RETRACTING; // shouldn't end up here? but just in case
        pusherState = STOPPED;
        break;
    }
  }
  
  if (solenoidState == EXTENDING) {
    driver->drive();
    uint32_t sample = adcm->readValue(HW::DRV_IPROPI);
    //ulogf("%u\r\n", sample);
    if (sample > max) {
      max = sample; // get peak current
      min = (max * 19) / 20; // prevent false detection by forcing a minimum below 95% of max current
    }
    else if (sample < min) { // only update min after the first peak
      min = sample;
      minReached = true; // set flag that the dip has been reached
    }
    if (minReached && map(sample, min, max, 0, 100) > 10) { // if current has dipped and then recovered 10% of dip
      driver->coast(); // stop driving the solenoid
      ulogf("[INFO] Solenoid extension time: %ums\r\n", to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(solenoidLastStateChange));
      solenoidState = RETRACTING; // regular retraction cycle
      solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange, cfg->solOffTimems);
      solenoidLastStateChange = get_absolute_time();
      (**firemode)->shotsFired++; // increment shots fired
    }
  }
  else {
    driver->coast();
  }
}

void Rune::PusherSolenoidCurrSensing::startPusher(bool single) {
  if (pusherState == STOPPED) { // dont reset pusher state if we try to fire again while the pusher is running
    pusherState = RUNNING;
    solenoidState = EXTENDING;
    solenoidLastStateChange = get_absolute_time();
    solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange, 200);
    resetSenseVariables();
  }
  stopOnCycle = single;
}

void Rune::PusherSolenoidCurrSensing::stopPusher() {
  stopOnCycle = true;
}

int32_t Rune::PusherSolenoidCurrSensing::map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Rune::PusherSolenoidCurrSensing::resetSenseVariables() {
  max = 0;
  min = 0xffffffff;
  minReached = false;
}