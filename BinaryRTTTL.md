# Binary RTTTL format definition #

The following document defines the field order and size (in bits) required for encoding / decoding of each melody as binary RTTTL.

This is actually a custom implementation of the RTTTL format. Using this format, one can achieve storing an highly compressed RTTTL melody which saves memory.

Note that all fields definition are defined in LSB to MSB order.

## Header ##

The first 16 bits stores the RTTTL default section (a.k.a header) which is defined as the following:

| Field name              | Size (bits) | Range    | Description                                                 |
|-------------------------|:-----------:|----------|-------------------------------------------------------------|
| Default duration index  |      3      | [0, 7]   | Matches the index used for `getNoteDurationFromIndex()` API |
| Default octave index    |      2      | [0, 3]   | Matches the index used for `getNoteOctaveFromIndex()` API.  |
| Beats per minutes (BPM) |      10     | [1, 900] |                                                             |
| Padding                 |      1      |          |                                                             |

## Notes ##

Next is each note's of the melody. Each note is encoded as 10 bits (or 16 bits) per note. Notes are defined as the following:

| Field name         | Size (bits) | Range   | Description                                                  |
|--------------------|:-----------:|---------|--------------------------------------------------------------|
| Duration index     |      3      | [0, 7]  | Matches the index used for `getNoteDurationFromIndex()` API. |
| Note letter index  |      3      | [0, 7]  | Matches the index used for `getNoteLetterFromIndex()` API.   |
| Pound              |      1      | boolean | Defines if the note is pounded or not.                       |
| Dotted             |      1      | boolean | Defines if the note is dotted or not.                        |
| Octave index       |      2      | [0, 3]  | Matches the index used for `getNoteOctaveFromIndex()` API.   |
| Padding (optional) |      6      |         | See description below.                                       |

The last field of a note (defined as `Padding`) is an optional 6 bits field. The AnyRtttl library supports both 10 bits per note and 16 bits per note definitions. Use the appropriate API for playing both format.



## 10 bits per note (no padding) ##

Each RTTTL note is encoded into 10 bits which is the minimum size of a note. This storage method is the best compression method for storing RTTTL melodies and reduces the usage of the dynamic memory to the minimum.

However, since all notes are not aligned on multiple of 8 bits, addressing each note by an offset is impossible which makes the playback harder. Each notes must be deserialized one after the other from a buffer using blocks of 10 bits which increases the program storage space footprint.

An external arduino library (or custom code) is required to allow AnyRtttl library to consume bits as needed. The arduino [BitReader](https://github.com/end2endzone/BitReader) library may be used for handling bit deserialization but any library that can extract a given number of bits from a buffer would work.



## 16 bits per note (with padding) ##

Each RTTTL note is encoded into 16 bits which is much better than the average 3.28 bytes per note text format. This storage method is optimum for storing RTTTL melodies and reduces the usage of the dynamic memory without increasing to much program storage space.

All notes are aligned on 16 bits. Addressing each note by an offset allows an easy playback. Only the first 10 bits of each 16 bits block is used. The value of the padding field is undefined.

# Playback #

The [Play10Bits](examples/Play10Bits/Play10Bits.ino) and [Play16Bits](examples/Play10Bits/Play10Bits.ino) are examples for showing AnyRtttl's capability to adapt to custom formats:

## Play 16 bits per note RTTTL ##

Note that this mode requires that an RTTTL melody be already converted to 16-bits per note binary format.

This feature uses a special function called `anyrtttlGetCharAdaptorFunc()` to convert RTTTL data encoded with 16 bits per note into standard RTTTL format. You can see how it works in the example below.

The following code shows how to use the library with 16-bits per note binary RTTTL:

```cpp
#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

//project's constants
#define BUZZER_PIN 8

//RTTTL 16 bits binary format for the following: tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a
const unsigned char tetris16[] = {0x0A, 0x14, 0x12, 0x02, 0x33, 0x01, 0x03, 0x02, 0x0B, 0x02, 0x14, 0x02, 0x0C, 0x02, 0x03, 0x02, 0x33, 0x01, 0x2A, 0x01, 0x2B, 0x01, 0x03, 0x02, 0x12, 0x02, 0x0B, 0x02, 0x03, 0x02, 0x32, 0x01, 0x33, 0x01, 0x03, 0x02, 0x0A, 0x02, 0x12, 0x02, 0x02, 0x02, 0x2A, 0x01, 0x29, 0x01, 0x3B, 0x01, 0x0A, 0x02, 0x1B, 0x02, 0x2A, 0x02, 0x23, 0x02, 0x1B, 0x02, 0x12, 0x02, 0x13, 0x02, 0x03, 0x02, 0x12, 0x02, 0x0B, 0x02, 0x03, 0x02, 0x32, 0x01, 0x33, 0x01, 0x03, 0x02, 0x0A, 0x02, 0x12, 0x02, 0x02, 0x02, 0x2A, 0x01, 0x2A, 0x01};
const int tetris16_length = 42;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
}

void loop() {
  anyrtttl::blocking::play16Bits(BUZZER_PIN, tetris16, tetris16_length);

  while(true)
  {
  }
}
```



## Play 10 bits per note RTTTL ##

Note that this mode requires that an RTTTL melody be already converted to 10-bits per note binary format.

This feature uses a special function called `anyrtttlGetCharAdaptorFunc()` to convert RTTTL data encoded with 10 bits per note into standard RTTTL format. You can see how it works in the example below.

Note that this code requires the [BitReader](https://github.com/end2endzone/BitReader) library to extract bits from the RTTTL binary buffer. The implementation of `readNextBits()` function delegates the job to the BitReader's `read()` method.

Create a function that will be used by AnyRtttl library to read bits as required. The signature of the library must look like this:
`char (*GetCharFuncPtr)(const char * iBuffer);`.

The following code shows how to use the library with 10-bits per note binary RTTTL:

```cpp
#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

//The BitReader library is required for extracting 10 bit blocks from the RTTTL buffer.
//It can be installed from Arduino Library Manager or from https://github.com/end2endzone/BitReader/releases
#include <bitreader.h>

//project's constants
#define BUZZER_PIN 8

//RTTTL 10 bits binary format for the following: tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a
const unsigned char tetris10[] = {0x0A, 0x14, 0x12, 0xCE, 0x34, 0xE0, 0x82, 0x14, 0x32, 0x38, 0xE0, 0x4C, 0x2A, 0xAD, 0x34, 0xA0, 0x84, 0x0B, 0x0E, 0x28, 0xD3, 0x4C, 0x03, 0x2A, 0x28, 0xA1, 0x80, 0x2A, 0xA5, 0xB4, 0x93, 0x82, 0x1B, 0xAA, 0x38, 0xE2, 0x86, 0x12, 0x4E, 0x38, 0xA0, 0x84, 0x0B, 0x0E, 0x28, 0xD3, 0x4C, 0x03, 0x2A, 0x28, 0xA1, 0x80, 0x2A, 0xA9, 0x04};
const int tetris10_length = 42;

//bit reader support
#ifndef USE_BITADDRESS_READ_WRITE
BitReader bitreader;
#else
BitAddress bitreader;
#endif
uint16_t readNextBits(uint8_t numBits)
{
  uint16_t bits = 0;
  bitreader.read(numBits, &bits);
  return bits;
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);

  bitreader.setBuffer(tetris10);
  
  Serial.begin(115200);
  Serial.println();
}

void loop() {
  anyrtttl::blocking::play10Bits(BUZZER_PIN, tetris10_length, &readNextBits);

  while(true)
  {
  }
}
```
