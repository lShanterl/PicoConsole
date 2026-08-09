# PicoGameConsole (Shamino Console)
 
A handheld game console built from scratch, powered by a **Raspberry Pi Pico** and a **Waveshare ST7735 1.8" LCD**, with a custom PCB and its own lightweight C/C++ game framework.
 
<img src="/pcb/picogameconsole.png" alt="PicoGameConsole PCB" width="800"/>

## Table of Contents
- [Overview](#overview)
- [Software Design](#software-design)
- [Hardware Design](#hardware-design)
- [Bill of Materials](#bill-of-materials)
- [Building the Firmware](#building-the-firmware)
- [Prerequisites](#prerequisites)
- [Build Commands (MinGW)](#build-commands-mingw)
- [Flashing the Pico](#flashing-the-pico)
- [Adding Your Own Game](#adding-your-own-game)
## Overview
 
PicoGameConsole (also called the **Shamino Console**) is a self-contained handheld gaming device. The project covers both the electronics - a custom PCB around the Pico - and the firmware that drives the display, reads input, and runs games.
 
## Software Design
 
The console framework aims to be **elegant**, **readable**, and as **performant** as possible. It's written mostly in embedded-style C, with a bit of C++ used only for convenience features. The functional, modular structure makes it straightforward to port to other platforms.
 
Adding a new game is as simple as declaring a `GameInfo` entry in `games_list.hpp` and implementing its logic in a corresponding `.cpp` file - see the [Adding Your Own Game](#adding-your-own-game) section and the example games already included in the repository.
 
## Hardware Design
 
The latest revision of the Shamino Console features a custom PCB, which noticeably improves build quality, reliability, and overall feel compared to the original breadboard prototype. A 3D-printed case is planned for a future release.
  
## Bill of Materials
 
| Component | Notes |
|---|---|
| [Raspberry Pi Pico (W)](https://www.raspberrypi.com/products/raspberry-pi-pico/) | Main microcontroller |
| [Waveshare ST7735 1.8" LCD](https://www.waveshare.com/wiki/1.8inch_LCD_Module) | Display |
| [Generic thumb joystick](https://botland.store/joystick/1942-thumb-joystick-5904422356231.html) | Analog input |
| 2x [12x12mm tact switches](https://botland.store/tact-switch/3663-tact-switch-12x12mm-with-cap-red-mushroom-5pcs-5904422307486.html) | Buttons |
| [Female headers for Raspberry Pi Pico](https://botland.store/raspberry-pi-pico-accessories/18863-set-of-female-connectors-for-raspberry-pi-pico-5904422328528.html) | Socket for the Pico |
| [5V 12mm passive buzzer](https://botland.store/buzzers-sound-generators/18290-buzzer-without-generator-5v-1mm-tht-5904422366025.html) | Sound |
| ~330Ω resistor | - |
| ~470Ω resistor | - |
| ~104 ceramic resistor | - |
| [1x8 goldpin connector](https://botland.store/connectors-goldpin/20031-straight-goldpin-1x40-connector-with-254mm-pitch-black-10pcs-justpi-5904422329198.html) | - |
| 2x 5mm M3 screws *(optional)* | Mounts the LCD |
| 4x 10mm M3 screws | Mounts the PCB to the 3D-printed case |
| [2x AA battery holder with power switch](https://botland.store/battery-holders/738-battery-holder-for-2-aa-r6-batteries-with-cover-and-switch-5904422329617.html) | 3x AA also should work fine |
 
## Building the Firmware
 
### Prerequisites
 
- [Pico SDK](https://github.com/raspberrypi/pico-sdk) - set the `PICO_SDK_PATH` environment variable
- [ARM GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) - set the `PICO_TOOLCHAIN_PATH` environment variable
- [CMake](https://cmake.org/) - add to `PATH`
- [MinGW](https://winlibs.com/) or another C++ compiler - add to `PATH`
  
### Build Commands (MinGW)
 
```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
make        # or: mingw32-make
```
 
If the build succeeds, a `GameOnConsole.uf2` file will be produced.
 
### Flashing the Pico
 
1. Hold the **BOOTSEL** button while plugging the Pico into USB (it will mount as a mass-storage drive).
2. Copy `GameOnConsole.uf2` onto the Pico drive.
3. On Windows, you can instead run the included `run.bat` script, which copies the built `.uf2` for you - you'll likely need to edit it to point to your Pico's drive letter.

## Adding Your Own Game
 
1. Declare a new `GameInfo` entry inside `games_list.hpp`.
2. Implement the game's logic in its own `.cpp` file under `games/`.
3. Rebuild the firmware (see [Building the Firmware](#building-the-firmware)) and flash it to your console.
Check the existing example games in `games/` for a template to follow.
