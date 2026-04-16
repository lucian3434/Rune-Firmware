#include "solenoid_pusher.h"

Rune::SolenoidPusher::SolenoidPusher(wheelUpdateCallback_t callback, firemode_t **firemode_curr, DRV::DRV824xS *drv) {
  updateWheelState = callback;
  firemode = firemode_curr;
  driver = drv;
  pusherState = STOPPED;
}

// returns true if the pusher module was successfully initialized
bool Rune::SolenoidPusher::init() {
  driver->init();
  return true;
}

// handler code for when the trigger is pressed
void Rune::SolenoidPusher::triggerRisingEdge() {
}

void Rune::SolenoidPusher::triggerFallingEdge() {
}

void Rune::SolenoidPusher::pusherTick() {
  
  if (pusherState == RUNNING) {
    // if we are here, we are up to speed on fw and ready to fire
    if (pusherLastState == STOPPED && solenoidState == RETRACTED) {
      // clear last state
      updatePusherState(RUNNING);

      //start pusher and start timers
      driver->drive();
      solenoidState = EXTENDING; // set the state to extending
      solenoidLastStateChange = get_absolute_time(); // record the time we started extending
      solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange,extendTimeMs); // set the next state change time
    }

    if (time_reached(solenoidNextStateChange)) {
      // if we are here, we have reached the time to change the solenoid state
      if (solenoidState == EXTENDING) {
        // if we were extending, we now need to retract
        shotsFired++;
        driver->coast(); 
        solenoidState = RETRACTING; // set the state to retracting
        solenoidLastStateChange = get_absolute_time(); 
        solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange, retractTimeMs); 
      } else if (solenoidState == RETRACTING) {
       
        if ((shotsFired >= (*firemode)->numShots) || ((!trig.isPressed()) && ((*firemode)->burstMode == 0))){
          // stop the pusher if we've finished the burst or let go of the trigger
          updatePusherState(STOPPED);
          updateWheelState(SLOWING); // and tell the wheels to slow down too
        } else {
          // still more shots to fire
          driver->drive();
          solenoidState = EXTENDING; // set the state to extending
          solenoidLastStateChange = get_absolute_time(); 
          solenoidNextStateChange = delayed_by_ms(solenoidLastStateChange, retractTimeMs); 
        }
      }
    }
  } else {
    // make sure we have graceful shutdown
    if (time_reached(solenoidNextStateChange)) {
      if (solenoidState == RETRACTING) {
        solenoidState = RETRACTED;
      }
    }
  }
}

void Rune::SolenoidPusher::updatePusherState(Rune::PusherGeneric::pusherState_t newState) {
  pusherLastState = pusherState;
  pusherState = newState;
  if (newState == RUNNING) {
    shotsFired = 0;
  }
}

