# Soft Touch final project report

## Application description
Soft Touch is a tech demo for a USB-MIDI control surface that currently provides a single endless rotary encoder and some switches. 

The encoder may be assigned to a series of 'mappings'; when enabled, each of these mappings allow encoder turns or presses to generate USB-MIDI Control Change messages. Soft Touch allows the user to choose between different encoder-to-CC-message mappings. 

Together with the ubiquitous 'MIDI Learn' capability of most software musical instruments / effects / applications, the present incarnation Soft Touch enables a user to change several parameters on a software instrument (although, for now, only one parameter a time).

This is all, ultimately, in the service of my long-term goal of being able to make music with as little time as possible spent holding a mouse or looking at a software GUI and feeling like I'm still at work.

### Things I descoped from my original (week 6) intentions

Making good decisions about what not to do is really painful. Even without the fact that we were renovating our new home (with everything still in boxes from the move-in in July...) during the course, I had to be really severe with my ambitions to get a minimal viable project.

I wanted (still do) to work with the [multitouch Bela Trill][4], but when I began to dig deeper and realised I'd need to port their driver from a dependency on the Arduino `Wire` library, my head really started to hurt. I had some good encoders in the parts bin, so I hurriedly substituted one of those. 

I also ran out of time to put a colour I2C LCD display on the unit, so over to the onboard segment LCD.

On the upside, by cutting back and using the simplest things available, I think it probably cleared clutter to see and think about the design more perceptively - even if I was still crunching like mad to hit the extended deadline (thank you again!).

## Hardware description
The hardware for the Soft Touch project consists of a [Freescale/NXP KL46Z Mbed-enabled development board][1] (not actually EOL, as claimed in the demo video...), [a Bourns PCR11 rotary encoder][2], and a handful of passives to implement the manufacturer's recommended filtering circuit from the PCR11 datasheet. 

### Satisfying the peripheral use requirement:

1.  Both buttons on the KL46Z, as well as the push switch on the PCR11, are registered as available buttons in the firmware's UI manager (see: `ui/` and `drivers/button.h`). Please note that the assignment of these and other hardware UI elements is done thru the `UiStateInterface` and its derived classes. 

2. The 2-bit quad encoder output of the PCR11. This is managed by [Aaron Berk's `QEI` library][3] for Mbed OS 2, which I updated to build against Mbed OS 6, and wrapped up in the `drivers/encoder.h` class.

3. Both LEDs onboard the KL46Z are set up as simple digital outputs (couldn't think of a suitable use for PWM application at the time, although I can now). One of these provides a heartbeat signal. See `drivers/led.h`

4. The serial UART provided by the KL46Z's onboard programmer implements debug logging and a serial console through the Mbed OS `BufferedSerial` driver, although the console itself is currently just a proof-of-concept port of Elecia's RPi example to a single (badly overgrown) CPP class, `console/Console.h` and I set aside ambitions to use it as a test harness for each component of the firmware as I burned through my extension.

5. The KL46Z's USB peripheral port is driven by the Mbed OS `USBMIDI` driver. Here I just read the API docs and wrote a very limited task, `drivers/midi/UsbTransceiver` to manage it.

6. The KL46Z's segment LCD display is driven by code that I'm fairly certain (I grabbed the files from a zip archive provided during an earlier training course, but I did some digital archaeology) originates from [the Freescale KL46Z board support package][1], with a wrapper class I implemented on top - see `drivers/slcd`.

## Software description
I was determined to: 

* get some prototyping experience with Mbed OS 
* learn how to set up an encoder properly!
* see if I could get a handle on writing an 'almost concurrent' bare-metal application on a Cortex-M, which I'd never really worked on before
* try and do as much as I could in CPP-with-classes, so that I could begin thinking in design patterns. The ambition here was that the concrete nature of the application would prevent a paralysing attack of Abstraction Vertigo, which often happens to me otherwise.

### Code in general

As such, the application is organised around four main tasks. Each of these shares a chunk of the superloop, while there's also an 8kHz timer-driven interrupt for polling - although at present this isn't used as much as is necessary, and the polling rate is a sort-of arbitrary choice - it's faster than necessary for a device with one control, but I'm anticipating adding many more (Charlieplexed) controls in future versions.  

Each of these tasks has a single work queue implemented using the Mbed `CircularBuffer`, which is interrupt-safe. There's a universal exchange format, `STEventMessage`, defined in `soft_touch.h`. Each task offers a public `Post` method that other tasks may call to feed it work, on the basis of "push the data into the other task's buffer, then yield ASAP". 

Each task then services its queue outside of ISR time to get things done; as part of getting something done, it may `Post` to other tasks. Calling `Post` may reveal that the other task's work queue is full - so far, the best response I can think of is to throw the request away, although there are bound to be exceptions to this.

At present, `UsbMidiTransceiver` ignores any/all traffic from the USB MIDI host: it can't do that forever, especially if I want to have future versions show more visual feedback (e.g., when the host changes target parameters _itself_, e.g. by mouseclick).

### Original components
I came up with the message-passing approach here by myself, although I have a nasty feeling I don't fully understand it - I'm a concurrency nerd, and I think there's an unidentified safety condition (that the current system unwittingly obeys) which has to be maintained to avoid the chance of deadlock. I think the current implementation is simple enough, but I expect to discover it as I add features.

The `UiStateInterface` and `SysCtrlMappingInterface` interfaces use the same basic approach to the same basic problem, e.g. "how to cleanly transition between similar temporary states?" 

This is the State design pattern, and I was delighted to happen upon it (in [Dive Into Design Patterns][5]) and recognise that it's what I needed. 

In the case of the `Ui`, each of the two available states provide an interpretive layer between:

* The logical `Ui` events described in the interface (`Turn`, `Press`, `Display`, etc...)
* The application commands they correspond to (e.g. `Turn` corresponds to `Load Mapping` in one state, `Update Target Value` in the other, while the `PressAndHold` and `ReleaseFromHold` events on `BoardSwLH` trigger the transition between the two `Ui` states)

It was *particularly* satisfying that this pattern allowed me to cleanly control the segment LCD during each of the `Ui` states:

* `UiCfgState`: the LCD readout shows the index and current target value of each mapping so that the user can scroll across the list of mappings. When the screen shows the mapping of interest, the user can release the `BoardSwLH` button, which selects the displayed mapping for control
* `UiCtrlState`: the LCD readout shows the index and current target value of the _currently controlled_ mapping. 

Elsewhere, I threw Singleton around a fair bit. 

Also in the `Ui` generally, the `Button` class features a simple state machine to turn switch state changes into logical `Press`, `PressAndHold`, `Release`, etc events. The 'just keep bitshifting' tactic to reading the input, and the enum classification of logical button events, if not the state machine itself, are both derived from [Emilié Gillet's open-source firmware for the Mutable Instruments Eurorack modules][6].

### Reused components

As mentioned above, these include: 

1. Elecia's `Consolinator` example from Week 4? 5? (This I adapted, probably quite crudely, into a CPP module.)
2. Mbed OS `USBMIDI` and `Buffered Serial` drivers and interrupt-safe `CircularBuffer` and `Ticker` implementations
3. Aaron Berk's `QEI` library for reading quad encoders,
4. Freescale's own `SLCD` driver for the KL46Z's segment LCD, 


## Architectural diagrams

### Hardware architecture

### Software architecture

## Build instructions

### How to build the firmware
Please see `./README.md` in this repository. 

Please note that as far as Mbed OS 6.19 is concerned, there are some oddball mods and overrides required to convince the tools that the KL46Z really really does have a USB transceiver (and the project won't build until you do). 

I am pretty confident I've documented all of these gotchas.

Please also note that I had absolutely no luck building and flashing this project with any tools other than Mbed Studio. I didn't try the online/cloud dev tools.

### Hardware
In principle, any Mbed-compatible dev board with its own USB transceiver (e.g., in addition to the programming interface) should be a capable target for this firmware. But a rudimentary BOM is:

1. [Freescale/NXP KL46Z][1]
2. [Bourns PCR11 encoder][2]
3. 5x 10K resistors (4 if you don't want the encoder push switch)
5. 2x 10nF caps

Please see the 'Suggested Filter Circuit' on p3 of the Bourns datasheet.

Please also be advised that the onboard programmer for the KL46Z [needs a firmware update][8]! 

### Software
I have been able to get this to build in [Mbed Studio][7]. I found this much easier to get configured than PlatformIO - specifically, the programming/debugging steps in PlatformIO seemed to only work when they felt like it.

### Debugging and testing

There is a 115k2 serial UART enabled for logging. There is also a console but, once I was happy with initial testing, it's not actually connected to the rest of the system - I ran out of time to plumb it into the SysCtrl/UI architecture.

I generally had a slightly weird time with setting breakpoints and things like that from the Mbed Studio IDE - the console debugger seemed to work better, but only just, and sometimes it would seem to fixate on breaking at points I hadn't set. I suspect [the `mbed-tools` CLI option][9] is most robust / most tested, but I couldn't get these to flash an image from my Mac - some sort of problem with PyOCD dependencies, I dunno.

### Power

This thing powers from USB2, as most commercial devices in its class tend to. I have distant ambitions scale this up to fairly large (e.g. 4-rack-unit) LED illuminated panels with around 80 controls to a panel; once I have charlieplexing for an encoder array sorted out, I'll need to start power profiling.   

## Future

### Necessary to be ready for production
I'd need USB mftr/device identification - don't know how that works - and industrial design, a bunch of stuff!

But broadly, to indicate awareness if not compliance, here are a bunch of issues with the code (as distinct from 'features to add'): 

* Fix the developer console and implement mocking/test functions for the queues! This will do as a substitute for the tricksy IDE debugger (I hope...)
* It's going to need a bootloader and better tolerance of waiting for the USB MIDI host to connect (sure, it's powered over USB so can't do much without the link, but as implemented right now the device will lock hard _until a application opens a connection through the host's USB interface._ Not acceptable.)
* I need to document the class relationships and state machines properly (and make myself learn 'enough' UML diagramming, in the process) ... and then submit it for code review to someone who groks CPP well...
* I don't think I need anything remotely as powerful as this MCU. There are Mbed OS features to help with runtime system profiling: I haven't found time to explore them since I got the basic application in working order. 
* I haven't made a serious study of the timing requirements involved in these interfaces: 
  * I suspect I'm polling the UI hardware an order of magnitude faster than I need to be. I'm still apprehensive about reliance on interrupts.
  * The segment LCD will probably accept a much slower clock, 
  * I haven't carefully considered the maximal inbound MIDI data rate (which is, in practice, probably limited by the underlying USB anyway)
  * There might easily be a more power-efficient way to run the poll and deferred-work tasks than the `Ticker` and `This_thread::sleep_for` tactics the firmware uses at present. I did look at the `LowPowerTicker` driver at one point, but I don't quite remember why I set it aside. To be revisited.
* At the moment, the whole thing runs from RAM. Flash should be segmented, at a minimum, to allow for 'failsafe' & 'upgrade' application segments, along with areas to load in user-defined application configuration. 
* It's a unidirectional MIDI device, which is very limiting. Despite the fact that MIDI is comparatively slow, there are established mechanisms for triggering visual feedback (particularly good for controlling software primarily concerned with patterns, like virtual drum machines) and also device firmware upgrade.

### Extensions

An approximately-ordered list of features I'd like to add: 

* Address the issues detailed above.
* Implement device firmware update.
* Implement visual feedback and other responses to host-to-Soft-Touch MIDI traffic.
* Integrate the Bela Trill parts over I2C, closer to the original design brief - I've bought them, after all ...
* Profile the system and transfer it to a more economical platform, and add inputs for an array of momentary footswitches.
* Implement support for the 14-bit control change messages supported by some (not all) MIDI systems.
* Design a PCB to support a larger, charlieplexed array of the same encoder component, along with an backlit LCD or LED screen.
* Replace the host transport (USB-MIDI) with Bluetooth. This means I have to come up with another (and relatively sustainable) means of powering the thing, but would give me a chance to implement OpenSoundControl instead of MIDI (i.e., 2008's idea of device control vs. 1978's idea - higher-resolution controls, timers, interesting stuff)

## Grading

Category | Score | Reasons
--- | --- | ---
Meets minimum goals | 2.5 | All project goals are met; reasonably smart state machines (see: button debouncing; Ui manager; SysCtrl mappings); additional actuators. Console is threadbare, though. 
Completeness of deliverables | 3 | Code is readable, report is fairly thorough, video demonstrates system working as intended  
Clear intentions and working code | 2.5 | System performs as intended, CPP code is fairly clear although almost certainly in need of review / style / linting, analysis and optimisation
Reusing code | 3 | Several reused components (Segment LCD, QEI, Mbed drivers) with license details & links provided. Applicable licenses do not prohibit commercial applications. 
Originality and scope of goals | 2 | Tackling Mbed is perhaps unusual (I observed the words of caution in one of the videos w/r/t the instability of their HAL, guessed that it described a specific situation in 2020? 2019?, and made a guarded guess that the opportunity to re-use their USB Midi stack would be worth the risk). MIDI controllers might not be all that daring, I suppose, but I'm interested in the topic and I managed to complete it. Making a decently usable device with a relatively minimal UI feels like a bit of an achievement, though.
Self-assessment | | 
Power analysis / DFU / System profiling | 1 | I've acknowledged the need for each and waved a finger in the air a bit, but I've not really put any focused effort into any of these yet. 
Version control | 3 | I've definitely been making commits. I think at one point I even jumped out on a branch.


[1]:https://www.nxp.com/design/design-center/development-boards/general-purpose-mcus/freedom-development-platform-for-kinetis-kl3x-and-kl4x-mcus:FRDM-KL46Z 
[2]:https://www.bourns.com/docs/product-datasheets/pec11r.pdf
[3]: https://os.mbed.com/users/aberk/code/QEI//file/5c2ad81551aa/QEI.h/
[4]:https://bela.io/products/trill/ 
[5]:https://sourcemaking.com/design_patterns
[6]:https://github.com/pichenettes/eurorack/blob/master/yarns/drivers/switches.h 
[7]:https://os.mbed.com/studio/
[8]:https://os.mbed.com/platforms/FRDM-KL46Z/
[9]:https://os.mbed.com/docs/mbed-os/v6.16/build-tools/mbed-cli-2.html 