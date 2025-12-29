![AnyRtttl logo](https://github.com/end2endzone/AnyRtttl/raw/master/docs/AnyRtttl-splashscreen.png)


[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Github Releases](https://img.shields.io/github/release/end2endzone/AnyRtttl.svg)](https://github.com/end2endzone/AnyRtttl/releases)
[![Visitors](https://visitor-badge.laobi.icu/badge?page_id=end2endzone.AnyRtttl)](https://github.com/end2endzone/AnyRtttl)

[![Button Install](https://img.shields.io/badge/Install-brightgreen?logoColor=white&logo=GitBook)](https://www.ardu-badge.com/AnyRtttl)
[![Button Changelog](https://img.shields.io/badge/Changelog-blue?logoColor=white&logo=googleforms)](CHANGES)



# AnyRtttl #

AnyRtttl is a feature rich arduino library for playing [RTTTL](http://www.end2endzone.com/anyrtttl-a-feature-rich-arduino-library-for-playing-rtttl-melodies/#Quick_recall_of_the_RTTTL_format) melodies. The library offers much more interesting features than relying on the widely available `void play_rtttl(char *p)` function. The library supports all best RTTTL features.

## Features ##

* Really small increase in memory & code footprint compared to the usual blocking algorithm.
* Blocking & Non-Blocking modes available.
* Support custom `tone()`, `noTone()` and `millis()` functions.
* Compatible with external Tone libraries.
* Supports RTTTL melodies stored in RAM or Program Memory (`PROGMEM`).
* Compatible with any custom or arbitrary RTTTL format that can be decoded as legacy RTTTL.
* Support a STRICT or RELAXED parsing mode. See [Strict parsing mode](#strict-parsing-mode) and [Relaxed parsing mode](#relaxed-parsing-mode).
* Support for playing 2 melodies simultaneously (using 2 speakers on two different pins). See [ESP32DualPlayRtttl](examples/ESP32DualPlayRtttl/ESP32DualPlayRtttl.ino) example.
* Supports highly compressed RTTTL binary format. See [Play16Bits](examples\Play16Bits\Play16Bits.ino) or [Play10Bits](examples\Play10Bits\Play10Bits.ino) examples.



## Status ##

Build:

| Service | Build | Tests |
|----|-------|-------|
| Windows Server 2019 | [![Build on Windows](https://github.com/end2endzone/AnyRtttl/actions/workflows/build_windows.yml/badge.svg)](https://github.com/end2endzone/AnyRtttl/actions/workflows/build_windows.yml) | [![Tests on Windows](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/end2endzone/58cf6c72c08e706335337d5ef9ca48e8/raw/AnyRtttl.master.Windows.json)](https://github.com/end2endzone/AnyRtttl/actions/workflows/build_windows.yml) |
| Ubuntu 22.04        | [![Build on Linux](https://github.com/end2endzone/AnyRtttl/actions/workflows/build_linux.yml/badge.svg)](https://github.com/end2endzone/AnyRtttl/actions/workflows/build_linux.yml)       | [![Tests on Linux](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/end2endzone/58cf6c72c08e706335337d5ef9ca48e8/raw/AnyRtttl.master.Linux.json)](https://github.com/end2endzone/AnyRtttl/actions/workflows/build_linux.yml)       |




# Purpose #

After publishing [NonBlockingRtttl](https://github.com/end2endzone/NonBlockingRTTTL) arduino library, I started using the library in more complex projects which was requiring other libraries. I quickly ran into the hell of library dependencies and library conflicts. I realized that I needed more features that could help me prototype faster.

Other libraries available which allows you to "play" a melody in [RTTTL](http://www.end2endzone.com/anyrtttl-a-feature-rich-arduino-library-for-playing-rtttl-melodies/#Quick_recall_of_the_RTTTL_format) format suffer the same issue: they are based on blocking APIs or the RTTTL data is not optimized for space.

AnyRtttl is different since it packs multiple RTTTL related features in a single library. It supports [blocking](https://en.wikipedia.org/wiki/Blocking_(computing)) & [non-blocking](http://en.wikipedia.org/wiki/Non-blocking_algorithm) API which makes it suitable to be used by more advanced algorithm. For instance, when using the non-blocking API, the melody can be stopped when a button is pressed. The library is also compatible with external Tone libraries and it supports highly compressed RTTTL binary formats.

**Existing Non-Blocking code**

Most of the code that can play a melody on internet are build the same way: sequential calls to `tone()` and `delay()` functions using hardcoded values. This type of implementation might be good for robots but not for realtime application or projects that needs to monitor pins while the song is playing.

With AnyRtttl non-blocking mode, your program can read/write IOs pins while playing and react on changes. Implementing a "stop" or "next song" push button is easy!



# Usage #

The following instructions show how to use the library.



## Blocking mode ##

To play a RTTTL melody using the blocking mode API, you call `anyrtttl::blocking::play(BUZZER_PIN, melody);` with your buzzer pin and the RTTTL melody string. This function will play the entire melody sequentially, pausing execution until the melody finishes.

The blocking api will:
* Play the RTTTL string note by note.
* Halt execution until the melody finishes, returning the control of the execution to the your sketch.
* Uses Arduino's `tone()` and `millis()` internally to implement delay between notes.

The blocking api can be used in your sketch `setup()` or `loop()` sections. 

This is ideal for simple sketches where you do not need multitasking. For example:
* Startup sounds
* Simple demos
* One-shot audio feedback



## Non-blocking mode ##

The non-blocking API will let the melody play in the background while your `loop()` keeps running. It will not freeze your sketch allowing you sketch to process other tasks simultaneously. 

With the non-blocking api, you need to
* Call `begin()` to initialize the library with a new melody.
* Call `play()` in the `loop()` at a high and consistent rate to allow playback to advance notes at their scheduled times.
* Make sure all other tasks in `loop()` execute quickly so they do not delay playback timing. Any long-running operations in `loop()` will introduce timing jitter and can delay note transitions.

The non-blocking api can not be used in your sketch `setup()`. It must be used in the `loop()` section.

This is ideal for sketches that are multitasks. For example, sketches that:
* Read sensors continuously
* Handle buttons and debouncing
* Update displays
* Maintain communication (Serial, I2C, SPI)
* Run state machines

In summary:

Call `anyrtttl::nonblocking::begin()` to setup AnyRtttl library in non-blocking mode.

Then call `anyrtttl::nonblocking::play()` to update the library's state and play notes as required.

Use `anyrtttl::done()` or `anyrtttl::nonblocking::isPlaying()` to know if the library is done playing the given song.

Anytime, one can call `anyrtttl::nonblocking::stop()` to stop playing the current song.

The following code shows how to use the library in non-blocking mode:

```cpp
#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

//project's constants
#define BUZZER_PIN 8
const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char * arkanoid = "Arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6";
const char * mario = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
byte songIndex = 0; //which song to play when the previous one finishes

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
}

void loop() {
  // If we are not playing something 
  if ( !anyrtttl::nonblocking::isPlaying() )
  {
    // Play a song based on songIndex.
    if (songIndex == 0)
      anyrtttl::nonblocking::begin(BUZZER_PIN, tetris);
    else if (songIndex == 1)
      anyrtttl::nonblocking::begin(BUZZER_PIN, arkanoid);
    else if (songIndex == 2)
      anyrtttl::nonblocking::begin(BUZZER_PIN, mario);

    //Set songIndex ready for next song
    songIndex++;
  }
  else
  {
    anyrtttl::nonblocking::play();
  }
}
```


## Macros ##

Use `ANY_RTTTL_VERSION` to get the current version of the library.

Define `ANY_RTTTL_INFO` to enable library state debugging via the serial port.
Define `ANY_RTTTL_DEBUG` to enable more detailed, advanced debugging of the library. See [GlobalMacros.md](GlobalMacros.md) which provides instructions for creating global macros.

Define the global macro `ANY_RTTTL_DONT_USE_TONE_LIB` to disable linking with Arduino's built‑in `tone()` and `noTone()` functions. When defined, AnyRtttl will not use these functions and your sketch will not link or depend on the tone library.

Define the global macro `ANY_RTTTL_NO_DEFAULT_FUNCTIONS` to disable all default function assignments. In this mode, AnyRtttl will not provide default implementations for its internal function pointers.

If you use either `ANY_RTTTL_DONT_USE_TONE_LIB` or `ANY_RTTTL_NO_DEFAULT_FUNCTIONS`, you must manually configure AnyRtttl at runtime by calling `anyrtttl::setToneFunction()`, `anyrtttl::setNoToneFunction()` or `anyrtttl::setMillisFunction()` before attempting to play a melody.

> **Note:**  
AnyRtttl is distributed with its own separate C++ source files (\*.cpp). A macro that is only defined at the start of your sketch does not propagate into the library's source files. As a result, AnyRtttl mostly remain unaffected by the sketch‑level macro definition.
&nbsp;  
&nbsp;  
See [GlobalMacros.md](GlobalMacros.md) which provides instructions for creating global macros.

### Parsing modes ###

This library supports two parsing modes: _Strict_ and _Relaxed_. It provides different levels of input quality and validation requirements. The selected mode determines how rigorously the parser validates input and how it reacts to malformed or ambiguous data.



#### Strict parsing mode ####

Strict mode do not validates the RTTTL melody content. It expect the melody to match the [original Nokia's specification](#format-specification).

Use macro `RTTTL_PARSER_STRICT` to configure the library in strict parsing mode.

* Strict has lowest memory and program footprint.
* May fail with invalid syntax or unsupported constructs.
* Has no error handling.
* Parsing errors may result in potential crashes.
* Supports names longer than the 10 character limit.



#### Relaxed parsing mode ####

Relaxed mode prioritizes robustness and usability. The parser attempts to interpret and recover from minor issues instead of failing.

Use macro `RTTTL_PARSER_RELAXED` to configure the library in relaxed parsing mode. This mode is also the default mode when `RTTTL_PARSER_STRICT` and `RTTTL_PARSER_RELAXED` are not specified.

* Relaxed is more resilient to invalid characters or parsing errors.
* Supports names longer than the 10 character limit.
* Supports RTTTL note duration of 64 or 128.
* Supports any BPM values between 10 and 2000.
* Supports RTTTL melodies with uppercase characters.
* Supports RTTTL melodies with spaces.
* Supports each control in the control section (d, o and b) to be specified in any order.
* Supports dotted notes in format `<duration><note><octave><.>` (Nokia's specification) or the alternate format `<duration><note><.><octave>` (Nokia's Simpsons example).



## Playing RTTTL data stored in flash (program) memory ##

AnyRtttl also supports RTTTL melodies stored in flash or Program Memory (PROGMEM).

The `anyrtttl::nonblocking::begin()` function supports _Program Memory_ macros such as `FPSTR()` or `F()`.

The following code shows how to use the library with RTTTL data stored in flash (program) memory instead of SRAM:

```cpp
#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

//project's constants
#define BUZZER_PIN 8
const char tetris[] PROGMEM = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char arkanoid[] PROGMEM = "Arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6";
const char mario[] PROGMEM = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
// James Bond theme defined in inline code below (also stored in flash memory) 
byte songIndex = 0; //which song to play when the previous one finishes

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
}

void loop() {
  // If we are not playing something 
  if ( !anyrtttl::nonblocking::isPlaying() )
  {
    // Play a song based on songIndex.
    if (songIndex == 0)
      anyrtttl::nonblocking::beginProgMem(BUZZER_PIN, tetris);
    else if (songIndex == 1)
      anyrtttl::nonblocking::begin_P(BUZZER_PIN, arkanoid);
    else if (songIndex == 2)
      anyrtttl::nonblocking::begin(BUZZER_PIN, FPSTR(mario));
    else if (songIndex == 3)
      anyrtttl::nonblocking::begin(BUZZER_PIN, F("Bond:d=4,o=5,b=80:32p,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d#6,16d#6,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d6,16c#6,16c#7,c.7,16g#6,16f#6,g#.6"));

    //Set songIndex ready for next song
    songIndex++;
  }
  else
  {
    anyrtttl::nonblocking::play();
  }
}
```



# Advanced Usage #



## External Tone or Timer #0 libraries ##


### Custom millis() function (timer0) ###

The library also supports custom `millis()` function. If a project requires modification to the microcontroller's build-in Timer #0, the `millis()` function may be impacted and behave incorrectly. To maximize compatibility, one can supply a custom function which behaves like the original to prevent altering playback.


### Custom tone() and noTone() functions (timer2) ###

The AnyRtttl library is also flexible by allowing you to use the build-in arduino `tone()` and `noTone()` functions or an implementation from any external library which makes it compatible with any Tone library in the market.

> **Note:**  
When using your own functions for implementing `tone()` and `noTone()`, it is recommended to also define macro `ANY_RTTTL_DONT_USE_TONE_LIB`.
&nbsp;  
&nbsp;  
By default, AnyRtttl uses Arduino's built‑in `tone()` and `noTone()` functions. This automatically link your sketch with the tone library, which may lead to compilation or linking errors in situations where Timer2 is already used. To avoid this, you can define the global macro `ANY_RTTTL_DONT_USE_TONE_LIB`.
&nbsp;  
&nbsp;  
See [configuration](#Configuration) section for more details.

### Example on Arduino Nano ##

On the Arduino Nano, the [tone() function relies on Timer2](https://forum.arduino.cc/t/timers-used-by-nano/1103697/5). If Timer2 is already in use for another task, the built-in `tone()` and `noTone()` functions will conflict with it. In that case, you will need to create your own custom versions and configure AnyRtttl to use them.

For example :
```cpp
#include <avr/interrupt.h>
// ...
ISR(TIMER2_COMPA_vect) { }

// Define tone() and noTone() versions that relies on Timer1.
void timer1_tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
  // ...
}
void timer1_no_tone(uint8_t pin) {
  // ...
}

void setup()
{
	anyrtttl::setToneFunction(&timer1_tone);
	anyrtttl::setNoToneFunction(&timer1_no_tone);
}
```



## Binary RTTTL / Compatibility with custom RTTTL formats ##

AnyRtttl can be configured for playing your custom format. AnyRtttl can use a custom function for decoding such a custom format. This allows the library to be compatible with any custom RTTTL formats that can be decoded as legacy RTTTL.

For example, AnyRtttl library can be adapted to play RTTTL data which is stored as binary data instead of text. This is actually a custom implementation of the RTTTL format. Using this format, one can achieve storing an highly compressed RTTTL melody which saves memory.

The [Play10Bits](examples/Play10Bits/Play10Bits.ino) and [Play16Bits](examples/Play10Bits/Play10Bits.ino) are examples for showing AnyRtttl's capability to adapt to custom formats.

See [BinaryRTTTL.md](BinaryRTTTL.md) for a definition of this custom RTTTL format.



## Custom Tone function (a.k.a. RTTTL 2 code) ##

This example shows how custom functions can be used by the AnyRtttl library to convert an RTTTL melody to arduino code.

First define replacement functions like the following:

```cpp
void serialTone(byte pin, uint16_t frequency, uint32_t duration) {
  Serial.print("tone(");
  Serial.print(pin);
  Serial.print(",");
  Serial.print(frequency);
  Serial.print(",");
  Serial.print(duration);
  Serial.println(");");
}

void serialNoTone(byte pin) {
  Serial.print("noTone(");
  Serial.print(pin);
  Serial.println(");");
}

```

Each new functions prints the function call & arguments to the serial port.

In the `setup()` function, setup the AnyRtttl library to use the new functions:

```cpp
//Use custom functions
anyrtttl::setToneFunction(&serialTone);
anyrtttl::setNoToneFunction(&serialNoTone);
```

Use the `anyrtttl::blocking::play()` API for "playing" an RTTTL melody and monitor the output of the serial port to see the actual arduino code generated by the library.

The following code shows how to use the library with custom functions:

```cpp
#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

//project's constants
#define BUZZER_PIN 8
const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";

//*******************************************************************************************************************
//  The following replacement functions prints the function call & parameters to the serial port.
//*******************************************************************************************************************
void serialTone(byte pin, uint16_t frequency, uint32_t duration) {
  Serial.print("tone(");
  Serial.print(pin);
  Serial.print(",");
  Serial.print(frequency);
  Serial.print(",");
  Serial.print(duration);
  Serial.println(");");
}

void serialNoTone(byte pin) {
  Serial.print("noTone(");
  Serial.print(pin);
  Serial.println(");");
}


void setup() {
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();

  //Use custom functions
  anyrtttl::setToneFunction(&serialTone);
  anyrtttl::setNoToneFunction(&serialNoTone);
}

void loop() {
  anyrtttl::blocking::play(BUZZER_PIN, tetris);

  while(true)
  {
  }
}
```




# Examples #

More AnyRtttl examples are also available:

* [Basic](examples/Basic/Basic.ino)
* [BlockingProgramMemoryRtttl](examples/BlockingProgramMemoryRtttl/BlockingProgramMemoryRtttl.ino)
* [BlockingRtttl](examples/BlockingRtttl/BlockingRtttl.ino)
* [BlockingWithNonBlocking](examples/BlockingWithNonBlocking/BlockingWithNonBlocking.ino)
* [ESP32DualPlayRtttl](examples/ESP32DualPlayRtttl/ESP32DualPlayRtttl.ino)
* [ESP32Rtttl](examples/ESP32Rtttl/ESP32Rtttl.ino)
* [ESP8266-NodeMCU](examples/ESP8266-NodeMCU/ESP8266-NodeMCU.ino)
* [IoT-beeps](examples/IoT-beeps/IoT-beeps.ino)
* [NonBlockingProgramMemoryRtttl](examples/NonBlockingProgramMemoryRtttl/NonBlockingProgramMemoryRtttl.ino)
* [NonBlockingRtttl](examples/NonBlockingRtttl/NonBlockingRtttl.ino)
* [NonBlockingStopBeforeEnd](examples/NonBlockingStopBeforeEnd/NonBlockingStopBeforeEnd.ino)
* [Play10Bits](examples/Play10Bits/Play10Bits.ino)
* [Play16Bits](examples/Play16Bits/Play16Bits.ino)
* [PlaySerialRtttl](examples/PlaySerialRtttl/PlaySerialRtttl.ino)
* [Rtttl2Code](examples/Rtttl2Code/Rtttl2Code.ino)




# RTTTL Format #

## Nokia's original specification ##

This library implements the [original Nokia Phone specification](http://merwin.bespin.org/t4a/specs/nokia_rtttl.txt) ([backup copy here](docs\nokia_rtttl.txt)).

This format is specified as the following:  
`<name>:<control-section>:<tone-commands>,<tone-commands>...`


### Control Section: ###

The control section is optional.
It defines the following parameters for the melody:

* `d=<value>` : Default duration of a note if unspecified.
* `o=<value>` : Default octave of a note if unspecified.
* `b=<value>` : Beats per minutes of a quarter note


### Tone commands: ###

Tones can be represented in the following format:

`[<duration>] <note> [<scale>] [.] <delimiter>` where :

* `duration` is the duration divider of full note duration, eg. 4 represents a quarter note.
* `note` is the note name (one of `p`,`c`,`c#`,`d`,`d#`,`e`,`f`,`f#`,`g`,`g#`,`a`,`a#`,`b`). The note `p` is a special note that represents a pause, a silent note in the melody.
* `scale` is the scale of the melody: from 4 to 7.
* `.` is a dotted note which increases the duration.

The duration, octave and dot are optional.


## Nokia's ambiguity with dotted notes (Simpsons example) ##

The original Nokia RTTTL specification defines the note format as `[<duration>] <note> [<scale>] [<special-duration>] <delimiter>`, which implies that the dot should appear after the optional `<scale>` character.

However, the official example of the _Simpsons_ ringtone included in the specification, place the dot before the scale character (e.g., `g.6` instead of `g6.`). This inconsistency suggests that both placements were accepted by early Nokia implementations, and parsers should be tolerant of either ordering when interpreting RTTTL strings.

Example: `Simpsons:d=4,o=5,b=160:32p,c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g`.


## Other specifications: ##

* An old but accurate specification:  
  https://www.mobilefish.com/tutorials/rtttl/rtttl_quickguide_specification.html
* The original Nokia Phones specification:  
  http://merwin.bespin.org/t4a/specs/nokia_rtttl.txt
* The specification on Wikipedia:  
  https://en.wikipedia.org/wiki/Ring_Tone_Text_Transfer_Language#Technical_specification
* A more modern RTTTL specification document:  
  https://rtttl.skully.tech/rtttl_specification


## RTTTL ressources ##

Online RTTTL players:

* A web based RTTTL melody composer:  
  https://rtttl.skully.tech/
* Online RTTTL player:  
  https://adamonsoon.github.io/rtttl-play/
* Nokia Composer:  
  https://eddmann.com/nokia-composer-web/

Other:

* RTTTL documentation on ESPHome:  
  https://esphome.io/components/rtttl/
* Converting Arduino melodies:  
  https://end2endzone.com/how-to-convert-arduino-code-to-actual-rtttl-melodies-using-librtttl-and-anyrtttl/#game-of-throne

Melody databases:

* https://picaxe.com/RTTTL-Ringtones-for-Tune-Command/
* https://www.voip-info.org/rtttl-melodies/
* Multiple melodies for IoT projects: [example IoT-beeps](examples\IoT-beeps\IoT-beeps.ino).
* https://github.com/end2endzone/smart-doorbell-homeassistant/blob/main/src/doorbell/rtttl_melodies.txt
* https://github.com/end2endzone/smart-doorbell-homeassistant/blob/main/src/doorbell/rtttl_ringtones.txt




# Building #

Please refer to file [INSTALL.md](INSTALL.md) for details on how installing/building the application.




# Platforms #

AnyRtttl has been tested with the following platform:

  * Linux x86/x64
  * Windows x86/x64




# Versioning #

We use [Semantic Versioning 2.0.0](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/end2endzone/AnyRtttl/tags).




# Authors #

* **Antoine Beauchamp** - *Initial work* - [end2endzone](https://github.com/end2endzone)

See also the list of [contributors](https://github.com/end2endzone/AnyRtttl/blob/master/AUTHORS) who participated in this project.




# License #

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
