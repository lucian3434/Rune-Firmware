#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>

#include "components/blaster.h"
#include "components/board_def.h"

#include "led/ws2812.h"


#define DEVMODE
#ifdef DEVMODE
// requires a character to be sent over serial before starting
#warning "Warning: Dev Mode enabled"
#endif

volatile static char kelly[] = "Adrian Kelly was a visionary"; 

void init();
bool systemControlLoop(repeating_timer_t *rt);
bool motorControlLoop(repeating_timer_t *rt);

// rpm logging
//#define USE_RPM_LOGGING
#ifdef USE_RPM_LOGGING
#define NUM_MOTORS 2
const uint32_t rpmLogLength = 4000;
volatile uint32_t rpmCache[rpmLogLength][NUM_MOTORS] = {0};
volatile uint16_t throttleCache[rpmLogLength][NUM_MOTORS] = {0}; // float gets converted to an integer [0, 1999]
volatile uint16_t cacheIndex = rpmLogLength + 1;
#endif


// loop variables for main logic loop
int32_t mainLoopFrequency = 1000; // main logic loop update frequency in hz
int32_t mainLoopTimeus = 1e6 / mainLoopFrequency; // main logic loop time in us

// loop variables for motor control loop
int32_t pidFrequency = 4000; // update frequency in hz
int32_t motorLoopTimeus = 1e6 / pidFrequency; // motor control loop time in us
uint32_t motorLoopLastRPM[8] = {0}; // rpm to ramp down from
absolute_time_t motorStateLastUpdate;
bool loggedSpinup = false;

Rune::Config config = Rune::Config();
Rune::Blaster blaster = Rune::Blaster(&config);
HW::Board* board;

LED::WS2812 led = LED::WS2812(0, pio1);

void init() {
  // initialize generic io and usb
  stdio_init_all();
  #ifdef DEVMODE
  getchar(); // wait for user to open a terminal
  #endif

  config.load();

  board = HW::boards[config.board_name];

  if (board->ws2812_data != HW::NO_ASSIGNMENT) {
    // initialize status LED
    led = LED::WS2812(board->ws2812_data, pio1);
    led.init();
    led.setColor(0x0000FF); // blue to signal that we're booting
    led.update();
  }

  uprintf("Initializing Board...");
  // ensure hardware is correctly initialized
  HW::initBoard(board);
  uprintf("COMPLETE\r\n");

  uprintf("Initializing Blaster...\r\n");
  blaster.init(board);
  uprintf("Initializing Blaster COMPLETE\r\n");
}

int main() {
  // call init function
  init();
  uint8_t bootStatus = 0;
  
  // wake pusher driver if necessary
  if (board->pusher_driver == HW::DRV824XS) {
    uprintf("Waking DRV...");
    while (!board->pusher_module->wake()) {
      uprintf("\r\nFailed to wake DRV. Retrying...");
      sleep_ms(100);
    }
    uprintf("COMPLETE\r\n");
  }
  bootStatus |= 0x1; // pusher driver ready

  // register main logic loop
  repeating_timer_t mainLogicLoopTimer;
  bool logicLoopAdded = add_repeating_timer_us(-mainLoopTimeus, systemControlLoop, NULL, &mainLogicLoopTimer);
  if (logicLoopAdded) {
    uprintf("Main logic loop registered!\r\n");
    bootStatus |= 0x2; // main logic loop ready
  }
  else {
    uprintf("Failed to register main logic loop\r\n");
  }

  // register motor control loop function to run at the specified pid frequency
  motorStateLastUpdate = get_absolute_time();
  repeating_timer_t motorControlLoopTimer;
  bool timerAdded = add_repeating_timer_us(-motorLoopTimeus, motorControlLoop, NULL, &motorControlLoopTimer);
  if (timerAdded) {
    uprintf("PID loop registered!\r\n");
    bootStatus |= 0x4; // motor control loop ready
  }
  else {
    uprintf("Failed to register PID loop\r\n");
  }


  if (board->ws2812_data != HW::NO_ASSIGNMENT) {
    uprintf("Final boot status code: 0x%X ", bootStatus);
    if (bootStatus == 0x7) {
      led.setColor(0x3d2700); // yellowish, not too bright
      uprintf("GOOD\r\n");
    }
    else {
      led.setColor(0xFF0000); // red so we know something is fucked up
      uprintf("BAD, see discord for help\r\n");
    }
    led.update();
  }

  // keep execution going
  uint32_t knownDrops = 0;
  while (true) {
    
    printLogBuffer();
    uint32_t newDrops = getDrops();
    if (knownDrops < newDrops) {
      uprintf("WARNING: Log buffer overflow, %u messages dropped\r\n", newDrops - knownDrops);
      knownDrops = newDrops;
    }
    /*
    sleep_ms(100);
    uprintf("Heartbeat\r\n");
    //*/

    #ifdef USE_RPM_LOGGING
    // dump cache once full
    if (cacheIndex == rpmLogLength) {
      uprintf("Motor 1 RPM, Motor 2 RPM, Motor 1 Throttle, Motor 2 Throttle\r\n");
      for (uint16_t i = 0; i < rpmLogLength; i++) {
        uprintf("%u, %u, %u, %u\r\n", rpmCache[i][0], rpmCache[i][1], throttleCache[i][0], throttleCache[i][1]);
      }
      cacheIndex++; // increment index again so we don't print this more than once
    }
    sleep_ms(10);
    #endif
  }
}

bool systemControlLoop(repeating_timer_t *rt) {
  blaster.updateIO();

  // log state updates
  if (blaster.logicLines.virtTrig.isRisingEdge()) {
    #ifdef USE_RPM_LOGGING
    //ulogf("Cache index: %u\r\n", cacheIndex);
    cacheIndex = 0;
    #endif
    blaster.logicLines.virtRev.forceHigh(); // force rev high if we are firing
    ulogf("Trig\r\n");
  }
  if (blaster.logicLines.virtRev.isRisingEdge()) {
    ulogf("Rev\r\n");
  }

  // set new fire mode if necessary
  uint8_t selectorPos;
  switch (config.selector_type) {
    case Rune::Config::SELECTOR_SLIDE:
      selectorPos = 0;
      for (uint8_t i = 0; i < blaster.selectors.size(); i++) {
        selectorPos |= blaster.selectors[i]->isPressed() << i;
      }
      break;
    default:
      selectorPos = 0;
      break;
  }
  
  // check that a valid selector position was produced
  if (selectorPos < blaster.fireModes.size()) {
    blaster.currFireMode = &(blaster.fireModes[selectorPos]);
  }

  // update fire mode status
  (*blaster.currFireMode)->tick(&blaster.logicLines, blaster.pusher);

  // update pusher status
  blaster.pusher->pusherTick();

  // update wheel state
  if (blaster.logicLines.virtRev.getState()) {
    // regardless of anything else, if the rev line is high, we are accelerating
    blaster.logicLines.wheelState = Rune::States::WHEEL_ACCELERATING;
    
    if (blaster.logicLines.virtRev.isRisingEdge()) {
      motorStateLastUpdate = get_absolute_time();
      ulogf("MotorState -> ACCELERATING\r\n");
    }
  }
  else {
    if (blaster.logicLines.wheelState == Rune::States::WHEEL_ACCELERATING && blaster.pusher->getPusherState() == Rune::PusherGeneric::STOPPED && (*blaster.currFireMode)->state == Rune::FireModeGeneric::IDLE) {
      // if we have the wheels revving but the firemode and pusher are both done, start slowing down
      blaster.logicLines.wheelState = Rune::States::WHEEL_SLOWING;
      motorStateLastUpdate = get_absolute_time();
      ulogf("MotorState ACCELERATING -> SLOWING\r\n");
    }
    
    // motor control loop switches from WHEEL_SLOWING to stopped or idle automatically

    else if (blaster.logicLines.wheelState == Rune::States::WHEEL_IDLE) {
      //if (absolute_time_diff_us(motorStateLastUpdate, get_absolute_time()) / 1000ll > (int64_t)config.idletimems) {
      if (to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(motorStateLastUpdate) > config.idletimems) {
        // stop the wheels if we have been idling for long enough
        blaster.logicLines.wheelState = Rune::States::WHEEL_STOPPED;
        motorStateLastUpdate = get_absolute_time();
        ulogf("MotorState IDLE -> STOPPED\r\n");
      }
    }
  }

  return true;
}

bool motorControlLoop(repeating_timer_t *rt) {
  bool atTarget;
  if (blaster.logicLines.wheelState == Rune::States::WHEEL_ACCELERATING) {
    atTarget = true; // assume we are at target until proven otherwise
  }
  else {
    atTarget = false; // assume we are not at target
  }
  uint32_t msSinceLastUpdate = to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(motorStateLastUpdate);

  for (uint8_t i = 0; i < blaster.motors.size(); i++) {
    float throttlePoint = 0.0;
    if (config.motors[i].type == Rune::Config::MOTOR_BIDSHOT) {

      // get rpm from motor
      uint32_t rpm = ((Motor::BIDSHOTMotor*)(blaster.motors[i]))->readTelemetry();
      if (((rpm & 0xff000000) == 0xff000000) && (blaster.logicLines.wheelState != Rune::States::WHEEL_IDLE)) {
        //ulogf("M%u: Error 0x%x\r\n", i + 1, rpm);
        rpm = *(blaster.motorPIDs[i]->lastRPM); // for now, feed in old data
      }
      *(blaster.motorPIDs[i]->lastRPM) = rpm;

      
      // motor state logic
      if (blaster.logicLines.wheelState == Rune::States::WHEEL_SLOWING) {
        loggedSpinup = false;

        // calculate new throttle
        int64_t timeUntilEndOfRamp = (int64_t)config.rampDownTimems - (int64_t)msSinceLastUpdate;
        if (timeUntilEndOfRamp < 0) timeUntilEndOfRamp = 0;
        uint32_t targetRPM = motorLoopLastRPM[i];
        if (config.idletimems > 0) {
          targetRPM -= config.idle_rpm_targets[i];
        } // slightly cursed way of doing this
        targetRPM = (uint32_t)(((uint64_t)targetRPM * (uint64_t)timeUntilEndOfRamp) / (uint64_t)config.rampDownTimems); 
        if (config.idletimems > 0) {
          targetRPM += config.idle_rpm_targets[i];
        }

        throttlePoint = updatePID(blaster.motorPIDs[i], targetRPM, rpm);
      }
      else if (blaster.logicLines.wheelState == Rune::States::WHEEL_ACCELERATING) {
        uint32_t targetRPM = (*blaster.currFireMode)->getRPMCurve(config.motor_rpm_targets[blaster.fpsCap][i]);
        throttlePoint = updatePID(blaster.motorPIDs[i], targetRPM, rpm);
        
        if (rpm < (targetRPM - 500)) {
          atTarget = false;
        } // reset flag if any motor is below target rpm

        motorLoopLastRPM[i] = targetRPM; // store last target rpm for ramp down
      }
      else if (blaster.logicLines.wheelState == Rune::States::WHEEL_STOPPED) {
        // same as accelerating, just dont need to update atTarget
        //throttlePoint = updatePID(blaster.motorPIDs[i], 0, rpm);
        throttlePoint = 0.0;
        zeroPID(blaster.motorPIDs[i]);
      }
      else { // wheelState == IDLE 
        throttlePoint = updatePID(blaster.motorPIDs[i], config.idle_rpm_targets[i], rpm);
      }

      #ifdef USE_RPM_LOGGING
      // rpm logging
      if (cacheIndex < rpmLogLength) {
        rpmCache[cacheIndex][i] = rpm;
        throttleCache[cacheIndex][i] = (uint16_t)(throttlePoint * 1999);
      }
      #endif
    }

    blaster.motors[i]->setThrottle(throttlePoint);
  }

  // check whether we are done slowing down the motors
  if ((blaster.logicLines.wheelState == Rune::States::WHEEL_SLOWING) && (msSinceLastUpdate > config.rampDownTimems)) {
    if (config.idletimems > 0) {
      blaster.logicLines.wheelState = Rune::States::WHEEL_IDLE;
      ulogf("MotorState SLOWING -> IDLE\r\n");
    }
    else {
      blaster.logicLines.wheelState = Rune::States::WHEEL_STOPPED;
      ulogf("MotorState SLOWING -> STOPPED\r\n");
      ulogf("");
    }
    motorStateLastUpdate = get_absolute_time();
  }

  #ifdef USE_RPM_LOGGING
  if (cacheIndex < rpmLogLength) {
    cacheIndex++;
  }
  #endif

  blaster.logicLines.wheelsAtSpeed = atTarget;
  if (!loggedSpinup && blaster.logicLines.wheelsAtSpeed) {
    ulogf("Spinup Time: %ums\r\n", to_ms_since_boot(get_absolute_time()) - to_ms_since_boot(motorStateLastUpdate));
    loggedSpinup = true;
  }
  return true;
}