#include "pusher.h"
#include "./../../util.h"

Rune::PusherGeneric::PusherGeneric(FireModeGeneric** firemode_curr) {
  firemode = firemode_curr;
}

// returns true if the pusher module was successfully initialized
bool Rune::PusherGeneric::init() {
  return false;
}

Rune::PusherGeneric::pusherState_t Rune::PusherGeneric::getPusherState() {
  return pusherState;
}