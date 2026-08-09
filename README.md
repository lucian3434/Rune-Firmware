# Rune Firmware
Reference firmware for the Rune brushless blaster control board. Currently under development, this repository will likely change a good bit in the near future

## TODO
- [x] Function parity with pre-rewrite code
- [ ] Better noise filtering for PID control
- [ ] \(BIDSHOT code) Support for extended telemetry packets
- [ ] Make a better attempt to get everything set up before saying boot was successful (and add a safety flag so that most code doesn't run until after boot is complete)
- [ ] Flash read/write for configuration
- [ ] Getting started guide
- [ ] Current sensing closed loop solenoid code (possibly implemented)
- [ ] Voltage sensing closed loop solenoid code (lower priority)
- [ ] Motor PID autotune
- [ ] Screen support
- [ ] And then menu support
- [ ] Web UI (This will be a lot...)
- [ ] Update RPM logging code to current standard
- [ ] Support for pushbutton fire mode selector
- [ ] ESC passthrough mode for programming/updating
- [ ] More fire modes


## Features
- Bidirectional DSHOT by default
- Independent main logic and motor control loops
- Onboard LED indicates whether boot was successful or not
- Safety timeouts to prevent undesirable behaviour
- Target FPS set by selector position at boot
- ...many more to be added

## Usage (TODO)
> [!NOTE]
> Uses Pi Pico SDK in VSCode for development/building.
> To flash a board, hold the boot select button (momentary button next to the USB port) while plugging in your USB cable. When the board is properly connected and in bootloader mode, it should show up as a flash drive named RPI-RP2 on your computer. Drag the .uf2 file from the build folder onto the drive and it will disconnect, restart, and start executing your code.