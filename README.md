# JD's Soft Touch 

## Pitch

Soft Touch is a tech demo for a USB MIDI control surface providing a single endless rotary encoder. The encoder can be assigned to send USB-MIDI Control Change messages.

Soft Touch allows the user to choose between different encoder-to-CC-message mappings, meaning that the encoder can change several parameters on a software instrument (although only one at a time, for now).

## Tech overview 

I developed and built this app using Mbed Studio on an Apple (Silicon) Mac mini. 

NOTE: I did take a pretty concerted stab at getting Platform IO or the Mbed CLI tools to do it for me instead, but had total blocker problems getting built binaries onto my dev board: something to do with the  `pyocd` being called by those tools. I installed Mbed Studio and suddenly programming my board became possible. To figure about later. 

Mbed Studio seems nice. It uses CMake to organise the project, which I have met before and can just about deal with. The project CMakeLists.txt doesn't follow best practices - I just jammed the source files into that big list - I'll get to it.

This project was built against Mbed OS `6.17.0` in the `bare-metal` profile. The dev board is the NRND/EOLed NXP/Freescale KL46Z, which I received about five years ago as part of a 1-week training course.

In principle, you can build this project for any Mbed-supporting dev board with the `USBDEVICE` capability. I haven't tested this yet and I haven't prepared a schematic for the off-board electronics yet either. 

## Building the project

(Note: Untested approximation of what you'd need to do to set up to build the project from scratch.)

1. Install Mbed Studio 
2. Set up a local checkout of the Mbed OS files. There are lots of these and I wanted to keep them outside my project. To do this: 
3. Create a 'New Program' (select 'Mbed OS 6' and 'Empty Mbed Program') in a directory. The tools will download Mbed OS 6.
4. Clone this project's repo into a nearby directory. 
5. Open the project directory 
6. (Fudge ahead:) Link the project to the directory where the Mbed OS 6 download is.
7. Hit build and send to the board. 

### Build / test advisories  

* Mbed `6.17.0`'s target definition for the Freescale KL46Z doesn't identify the board with `USBDEVICE` functionality, but the board can do it (the populated USB mini-B is a dead giveaway). I've overridden this in the `mbed_app.json` project file. 
*However*, the `mbed-os/targets/TARGET_Freescale/USBPhy_Kinetis.cpp` file will not compile until you lean in and add missing `#include <cstddef>` and `#include <cstring>` to satisfy definitions for `NULL` and `memset`.

* You will need to set the Mbed Studio's `Serial Monitor` baud rate to `115200` or you'll only see some of the monitor messages.

* You will need to connect a USB host to the second USB port on the KL46Z (i.e., the one that isn't the programmer port), because the `USBMIDI` driver blocks until it sees a connection.

* On the KL46Z you can interact with the dev console from Mbed Studio's `Serial Monitor` pane (if you set the baud rate, it's 115k2). 

I was determined to make myself do a bunch of this work in CPP, which I'm rusty with, and to try to come to terms with approaches and patterns for concurrent-ish embedded outside the paradigm I'm reasonably familiar with (e.g., message-passing on the XMOS microcontrollers). Please be critical concerning my OOP!

## Calculations

### Encoder data stream 
Assembled against a Bourns PEC11R rotational encoder https://uk.farnell.com/bourns/pec11r-4225f-s0024/encoder-incremental-2ch-60rpm/dp/2381866) with the following specs:

* max. operating RPM: 60
* pulses per revolution: 24
* theoretical max output from single encoder: 24 pulses per second 
* so min. sampling rate: 96 samples per second

The `QEI` library (see below) took care of driving this part. 

## Attribution and licenses

### Mbed OS

```
[Mbed OS] is provided under the Apache-2.0 license. Contributions to this project are accepted under the same license. 

This project [i.e., Mbed OS] contains code from other projects. The original license text is included in those source files. They must comply with our license guide.
```

This platform provided the USB MIDI driver stack.

### CLI implementation

The CLI implementation in this project is a fairly crude CPP port of the code Elecia presented in the Wokwi RPi simulation, ~extended with some new cnmmands~ (not really, I ran out of time.. would have helped, though!).

### Quadrature Encoder Interface (QEI)

(C) 2010 Arm, MIT license, original author: Aaron Berk. https://os.mbed.com/users/aberk/code/QEI/

As hosted at that URL, this is an Mbed 2 driver - I put some effort into making it build against Mbed 6.

### Freescale segment LCD driver 

I originally obtained the Freescale dev board on a training course around five years ago. The project files 

```
FRDM-s401.h 
LCDConfig.h 
s401.c 
LCD.h 
LCD.cpp
```

were sourced from the software pack provided with the dev board (I made some modifications to the `LCD` class, but the greater part of it is not original code). I think these are in large part files from Freescale (although the board support package that survives on the NXP site is in some weird self-extracting format that made this unreasonably hard to confirm). I reckon the LCD class was implemented by staff at the training provider, and I couldn't find a license file in the package. Not ideal, but I can re-implement it if need be. 

The `STLCD` derived class and a couple of the LCD class methods are my own work.

