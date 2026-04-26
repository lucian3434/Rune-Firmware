#include "pusher.h"
#include "./../../util.h"

Rune::PusherGeneric::PusherGeneric() {
  return;
}

// returns true if the pusher module was successfully initialized
bool Rune::PusherGeneric::init() {
  return false;
}

// called every tick of the system control loop
void Rune::PusherGeneric::pusherTick() {
}