#include "pusher.h"
#include "./../fire control/firemode.h"
#include "./../../util.h"

Rune::PusherGeneric::PusherGeneric() {
  firemode = nullptr;
  pusherState = STOPPED;
}

Rune::PusherGeneric::PusherGeneric(FireModeGeneric*** firemode_curr) {
  firemode = firemode_curr;
  pusherState = STOPPED;
}

// returns true if the pusher module was successfully initialized
bool Rune::PusherGeneric::init() {
  return false;
}

Rune::PusherGeneric::pusherState_t Rune::PusherGeneric::getPusherState() {
  return pusherState;
}