# Binary RTTTL format definition #

The following document defines the field order and size (in bits) required for encoding / decoding of each melody as binary RTTTL.

This is actually a custom implementation of the RTTTL format. Using this format, one can achieve storing an highly compressed RTTTL melody which saves memory.

Note that all fields definition are defined in LSB to MSB order.

## Header ##

The first 16 bits stores the RTTTL default section (a.k.a header) which is defined as the following:

| Field name              | Size (bits) | Range    | Description                                                  |
|-------------------------|:-----------:|----------|--------------------------------------------------------------|
| Default duration index  |      3      | [0, 7]   | Matches the index used for `getDurationValueFromIndex()` API |
| Default octave index    |      2      | [0, 3]   | Matches the index used for `getOctaveValueFromIndex()` API.  |
| Beats per minutes (BPM) |      10     | [1, 900] |                                                              |
| Padding                 |      1      |          |                                                              |

## Notes ##

Next is each note's of the melody. Each note is encoded as 10 bits (or 16 bits) per note. Notes are defined as the following:

| Field name         | Size (bits) | Range   | Description                                                   |
|--------------------|:-----------:|---------|---------------------------------------------------------------|
| Duration index     |      3      | [0, 7]  | Matches the index used for `getDurationValueFromIndex()` API. |
| Note letter index  |      3      | [0, 7]  | Matches the index used for `getNoteValueFromIndex()` API.     |
| Pound              |      1      | boolean | Defines if the note is pounded or not.                        |
| Dotted             |      1      | boolean | Defines if the note is dotted or not.                         |
| Octave index       |      2      | [0, 3]  | Matches the index used for `getOctaveValueFromIndex()` API.   |
| Padding (optional) |      6      |         | See description below.                                        |

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

The [Play16bits example](examples\Play16Bits\Play16Bits.ino) shows how to use the library with 16-bits per note binary RTTTL:

```cpp
#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

// Define the BUZZER_PIN for current board
#if defined(ESP32)
#define BUZZER_PIN 25 // Using GPIO25 (pin labeled D25)
#elif defined(ESP8266)
#define BUZZER_PIN  2 // Using GPIO2  (pin labeled D4)
#else // base arduino models
#define BUZZER_PIN 9
#endif

// #define DEBUG_FUNCTION_READFROMDECODINGBUFFER 1
// #define SKETCH_NON_BLOCKING_MODE 1

#define SIZE_IN_BYTES(array_var) (sizeof(array_var)/sizeof(array_var[0]))
#define SIZE_IN_NOTES16(array_var) ( (SIZE_IN_BYTES(array_var) - sizeof(anyrtttl::RTTTL_CONTROL_SECTION)) / sizeof(anyrtttl::RTTTL_NOTE))

//project's constants
//RTTTL 16 bits binary format for the following: tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a
const unsigned char tetris16[] = {0x0A, 0x14, 0x12, 0x02, 0x33, 0x01, 0x03, 0x02, 0x0B, 0x02, 0x14, 0x02, 0x0C, 0x02, 0x03, 0x02, 0x33, 0x01, 0x2A, 0x01, 0x2B, 0x01, 0x03, 0x02, 0x12, 0x02, 0x0B, 0x02, 0x03, 0x02, 0x32, 0x01, 0x33, 0x01, 0x03, 0x02, 0x0A, 0x02, 0x12, 0x02, 0x02, 0x02, 0x2A, 0x01, 0x29, 0x01, 0x3B, 0x01, 0x0A, 0x02, 0x1B, 0x02, 0x2A, 0x02, 0x23, 0x02, 0x1B, 0x02, 0x12, 0x02, 0x13, 0x02, 0x03, 0x02, 0x12, 0x02, 0x0B, 0x02, 0x03, 0x02, 0x32, 0x01, 0x33, 0x01, 0x03, 0x02, 0x0A, 0x02, 0x12, 0x02, 0x02, 0x02, 0x2A, 0x01, 0x2A, 0x01};
const int notes_count = SIZE_IN_NOTES16(tetris16);
const anyrtttl::RTTTL_CONTROL_SECTION * tetris16_ctrl_section = (anyrtttl::RTTTL_CONTROL_SECTION*)tetris16;                // pointer to the control section bits within tetris16 buffer.
const anyrtttl::RTTTL_NOTE * tetris16_notes = (anyrtttl::RTTTL_NOTE*)(tetris16 + sizeof(anyrtttl::RTTTL_CONTROL_SECTION)); // pointer to the first note within tetris16 buffer.

// Defines an enum to know which block of data is parsed from the binary melody data
enum DecoderStateEnum {
  DECODER_STATE_NAME = 0,
  DECODER_STATE_CTRL_SECTION,
  DECODER_STATE_NOTES,
  DECODER_STATE_END,
};

//project's variables
char decoding_buffer[20] = {0};       // A temporary buffer in which we decode as text the RTTTL's binary Control Section or binary notes.
int decoding_buffer_index = 0;        // The index in decoding_buffer of the next character to read from decoding_buffer.
int tetris16_notes_index = 0;         // The index in tetris16_notes of the next note to play.
const char * fake_melody_address;     // The address of a fictionnal buffer containing the melody.
const char * previous_read_address;   // The address of the previous character read from melody.
char previous_read_character;         // The previous character read from the serial port.
DecoderStateEnum decoding_state = DECODER_STATE_NAME;

inline void decodeControlSection() {
  anyrtttl::toString(*tetris16_ctrl_section, decoding_buffer);
  decoding_buffer_index = 0;
}

inline void decodeNewNote() {
  anyrtttl::toString(*tetris16_ctrl_section, tetris16_notes[tetris16_notes_index], decoding_buffer);
  decoding_buffer_index = 0;

  // Move to next note for next decode
  tetris16_notes_index++;
}

inline void zeroiseDecodingBuffer() {
  decoding_buffer[0] = '\0';
  decoding_buffer_index = 0;
}

// Function readCharFromDecodingBuffer() returns all characters from decoding_buffer, one by one.
// The nth call this function returns the character at index n of decoding_buffer.
// When the end of the buffer is reached (character '\0'), it increases the decoding state and decodes the next block in decoding_buffer.
// The character NULL is returned when all states are decoded.
char readCharFromDecodingBuffer() {
  // read 1 character from decoding_buffer
  char c = decoding_buffer[decoding_buffer_index];

  // Check NULL character
  if (c == '\0') {
    // We reached the end of decoding_buffer.

    // Define how we should proceed. Move to the next decoding state or play next note ?
    bool jump_next_decoding_state = false;  // true if we should move to the next decoding step
    bool is_all_notes_played = (tetris16_notes_index >= notes_count);
    switch(decoding_state)
    {
      case DECODER_STATE_NAME:
        jump_next_decoding_state = true;
        decodeControlSection(); // Decode next block into decoding_buffer
        break;
      case DECODER_STATE_CTRL_SECTION:
        jump_next_decoding_state = true;
        decodeNewNote(); // Decode next block into decoding_buffer
        break;
      case DECODER_STATE_NOTES:
        if (is_all_notes_played) {
          jump_next_decoding_state = true;
          zeroiseDecodingBuffer();
        } else {
          decodeNewNote(); // Decode next block into decoding_buffer
        }
        break;
      case DECODER_STATE_END:
      default:
        zeroiseDecodingBuffer();
        break;
    };

    if (jump_next_decoding_state)
      decoding_state = (DecoderStateEnum)((int)decoding_state + 1);

    // Refresh character since we might have updated decoding_buffer or decoding_buffer_index
    c = decoding_buffer[decoding_buffer_index];
  }

  // move to next byte in decoding_buffer
  decoding_buffer_index++;

  return c;
}

// Function anyrtttlGetCharAdaptorFunc() read the first character from a fictionnal "melody buffer".
// For this sketch, iBuffer is an invalid buffer address.
// The function checks iBuffer and compares it with the previous address
// to know how many characters to actually read from function readCharFromDecodingBuffer().
// The function is blocking until the required number of characters is read.
// The last character read is returned by the function.
char anyrtttlGetCharAdaptorFunc(const char * iBuffer) {
  // Define how man characters to read
  size_t count = (size_t)(iBuffer - previous_read_address);

  // Read that many character
  for(size_t i=0; i < count; i++) {
    previous_read_character = readCharFromDecodingBuffer();
  }

  // Update the previous address read
  previous_read_address += count;

  // Convert newline characters as a NULL character to simulate the end of the RTTTL melody.
  // This triggers the melody playback code to stop.
  if (previous_read_character == '\r' || previous_read_character == '\n')
    previous_read_character = '\0';

  // Return the last character read
  return previous_read_character;
}

#ifdef ESP32
// Function esp32GetChannelForPin() maps a channel for a given pin.
// Returns a value between 0 and n where n is the maximum of channel for your board.
// Returns ESP32_INVALID_CHANNEL if there is no assigned channel for the given pin number.
// See your board documentation for details.
// See https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html#led-control-ledc
uint8_t getChannelForPin(uint8_t pin) {
  if (pin == BUZZER_PIN) return 0; // using channel 0 for pin BUZZER_PIN
  return 0xFF; // invalid
}
#endif // ESP32

void resetDecoding() {
  // init fake melody buffers
  previous_read_address = (const char*)0x0000;
  fake_melody_address = (const char*)0x0001; // Initialized to 1 charaacter after `previous_read_address` to force reading 1 character on the first call to anyrtttlGetCharAdaptorFunc().
  previous_read_character = '\0';

  // init decoding buffer with a new empty melody name with it's separator
  decoding_buffer[0] = ':';
  decoding_buffer[1] = '\0';
  decoding_buffer_index = 0;
  decoding_state = DECODER_STATE_NAME;

  tetris16_notes_index = 0;
}

#ifdef DEBUG_FUNCTION_READFROMDECODINGBUFFER
inline void debugReadFromDecodingBuffer() {
  char c = readCharFromDecodingBuffer();
  while(c!='\0') {
    Serial.print(c);
    c = readCharFromDecodingBuffer();
  }

  while(true) {
    delay(1000);
  }
}
#endif // DEBUG_FUNCTION_READFROMDECODINGBUFFER

void setup() {
  // silence BUZZER_PIN asap
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  resetDecoding();

  Serial.begin(115200);
  Serial.println("ready");

#ifdef ESP32
  // setup AnyRtttl for ESP32
  esp32::setChannelMapFunction(&getChannelForPin);  // Required for functions using esp32 core version 2.x.
  anyrtttl::setToneFunction(&esp32::tone);          // tell AnyRtttl to use AnyRtttl's specialized esp32 tone function.
  anyrtttl::setNoToneFunction(&esp32::noTone);      // tell AnyRtttl to use AnyRtttl's specialized esp32 noTone() function.

  // setup the pin for PWM tones.
  esp32::toneSetup(BUZZER_PIN);
#endif // ESP32

#ifdef DEBUG_FUNCTION_READFROMDECODINGBUFFER
  // debugging
  debugReadFromDecodingBuffer();
#endif // DEBUG_FUNCTION_READFROMDECODINGBUFFER
}

void loop() {
  #ifdef SKETCH_NON_BLOCKING_MODE
    // Non-blocking example

    if ( anyrtttl::nonblocking::done() ) 
    {
      // We are done playing the previous melody or
      // it is the first time we enter the loop() function.
      
      // Prepare for playing the next melody
      resetDecoding();
      
      // Start playing a new one
      anyrtttl::nonblocking::begin(BUZZER_PIN, fake_melody_address, &anyrtttlGetCharAdaptorFunc);
    }
    else {
      // continue playing
      anyrtttl::nonblocking::play();
    }
  #else
    // Blocking example

    // Prepare for playing the next melody
    resetDecoding();

    // Start playing
    anyrtttl::blocking::play(BUZZER_PIN, fake_melody_address, &anyrtttlGetCharAdaptorFunc);

    delay(1000);
  #endif // SKETCH_NON_BLOCKING_MODE
}
```



## Play 10 bits per note RTTTL ##

Note that this mode requires that an RTTTL melody be already converted to 10-bits per note binary format.

This feature uses a special function called `anyrtttlGetCharAdaptorFunc()` to convert RTTTL data encoded with 10 bits per note into standard RTTTL format. You can see how it works in the example below.

Note that this code requires the [BitReader](https://github.com/end2endzone/BitReader) library to extract bits from the RTTTL binary buffer. The implementation of `readNextBits()` function delegates the job to the BitReader's `read()` method.

Create a function that will be used by AnyRtttl library to read bits as required. The signature of the library must look like this:
`char (*GetCharFuncPtr)(const char * iBuffer);`.

The [Play10bits example](examples\Play10Bits\Play10Bits.ino) shows how to use the library with 10-bits per note binary RTTTL:

```cpp
#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

//The BitReader library is required for extracting 10 bit blocks from the RTTTL buffer.
//It can be installed from Arduino Library Manager or from https://github.com/end2endzone/BitReader/releases
#include <bitreader.h>

// Define the BUZZER_PIN for current board
#if defined(ESP32)
#define BUZZER_PIN 25 // Using GPIO25 (pin labeled D25)
#elif defined(ESP8266)
#define BUZZER_PIN  2 // Using GPIO2  (pin labeled D4)
#else // base arduino models
#define BUZZER_PIN 9
#endif

// #define DEBUG_FUNCTION_READFROMDECODINGBUFFER 1
// #define SKETCH_NON_BLOCKING_MODE 1

#define SIZE_IN_BYTES(array_var) (sizeof(array_var)/sizeof(array_var[0]))
#define SIZE_IN_NOTES10(array_var) ( (SIZE_IN_BYTES(array_var) - sizeof(anyrtttl::RTTTL_CONTROL_SECTION)) * 8 / 10);

//project's constants
//RTTTL 10 bits binary format for the following: tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a
const unsigned char tetris10[] = {0x0A, 0x14, 0x12, 0xCE, 0x34, 0xE0, 0x82, 0x14, 0x32, 0x38, 0xE0, 0x4C, 0x2A, 0xAD, 0x34, 0xA0, 0x84, 0x0B, 0x0E, 0x28, 0xD3, 0x4C, 0x03, 0x2A, 0x28, 0xA1, 0x80, 0x2A, 0xA5, 0xB4, 0x93, 0x82, 0x1B, 0xAA, 0x38, 0xE2, 0x86, 0x12, 0x4E, 0x38, 0xA0, 0x84, 0x0B, 0x0E, 0x28, 0xD3, 0x4C, 0x03, 0x2A, 0x28, 0xA1, 0x80, 0x2A, 0xA9, 0x04};
const int notes_count = SIZE_IN_NOTES10(tetris10);
const anyrtttl::RTTTL_CONTROL_SECTION * tetris10_ctrl_section = (anyrtttl::RTTTL_CONTROL_SECTION*)tetris10;                // pointer to the control section bits within tetris16 buffer.
const anyrtttl::RTTTL_NOTE * tetris10_note_bits = (anyrtttl::RTTTL_NOTE*)(tetris10 + sizeof(anyrtttl::RTTTL_CONTROL_SECTION)); // pointer to the first note within tetris16 buffer.

// Defines an enum to know which block of data is parsed from the binary melody data
enum DecoderStateEnum {
  DECODER_STATE_NAME = 0,
  DECODER_STATE_CTRL_SECTION,
  DECODER_STATE_NOTES,
  DECODER_STATE_END,
};

//project's variables
char decoding_buffer[20] = {0};       // A temporary buffer in which we decode as text the RTTTL's binary Control Section or binary notes.
int decoding_buffer_index = 0;        // The index in decoding_buffer of the next character to read from decoding_buffer.
int tetris16_notes_index = 0;         // The index in tetris16_notes of the next note to play.
const char * fake_melody_address;     // The address of a fictionnal buffer containing the melody.
const char * previous_read_address;   // The address of the previous character read from melody.
char previous_read_character;         // The previous character read from the serial port.
DecoderStateEnum decoding_state = DECODER_STATE_NAME;
#ifndef USE_BITADDRESS_READ_WRITE
BitReader bitreader;                  // Support for read 10 bit chunks. 
#else
BitAddress bitreader;                 // Support for read 10 bit chunks. 
#endif

inline void decodeControlSection() {
  anyrtttl::toString(*tetris10_ctrl_section, decoding_buffer);
  decoding_buffer_index = 0;
}

inline void decodeNewNote() {
  anyrtttl::RTTTL_NOTE note = {0};
  
  // read 10 bits from the note data bits
  bitreader.read(10, &note.raw);

  // convert note to string
  anyrtttl::toString(*tetris10_ctrl_section, note, decoding_buffer);
  decoding_buffer_index = 0;

  // Move to next note for next decode
  tetris16_notes_index++;
}

inline void zeroiseDecodingBuffer() {
  decoding_buffer[0] = '\0';
  decoding_buffer_index = 0;
}

// Function readCharFromDecodingBuffer() returns all characters from decoding_buffer, one by one.
// The nth call this function returns the character at index n of decoding_buffer.
// When the end of the buffer is reached (character '\0'), it increases the decoding state and decodes the next block in decoding_buffer.
// The character NULL is returned when all states are decoded.
char readCharFromDecodingBuffer() {
  // read 1 character from decoding_buffer
  char c = decoding_buffer[decoding_buffer_index];

  // Check NULL character
  if (c == '\0') {
    // We reached the end of decoding_buffer.

    // Define how we should proceed. Move to the next decoding state or play next note ?
    bool jump_next_decoding_state = false;  // true if we should move to the next decoding step
    bool is_all_notes_played = (tetris16_notes_index >= notes_count);
    switch(decoding_state)
    {
      case DECODER_STATE_NAME:
        jump_next_decoding_state = true;
        decodeControlSection(); // Decode next block into decoding_buffer
        break;
      case DECODER_STATE_CTRL_SECTION:
        jump_next_decoding_state = true;
        decodeNewNote(); // Decode next block into decoding_buffer
        break;
      case DECODER_STATE_NOTES:
        if (is_all_notes_played) {
          jump_next_decoding_state = true;
          zeroiseDecodingBuffer();
        } else {
          decodeNewNote(); // Decode next block into decoding_buffer
        }
        break;
      case DECODER_STATE_END:
      default:
        zeroiseDecodingBuffer();
        break;
    };

    if (jump_next_decoding_state)
      decoding_state = (DecoderStateEnum)((int)decoding_state + 1);

    // Refresh character since we might have updated decoding_buffer or decoding_buffer_index
    c = decoding_buffer[decoding_buffer_index];
  }

  // move to next byte in decoding_buffer
  decoding_buffer_index++;

  return c;
}

// Function anyrtttlGetCharAdaptorFunc() read the first character from a fictionnal "melody buffer".
// For this sketch, iBuffer is an invalid buffer address.
// The function checks iBuffer and compares it with the previous address
// to know how many characters to actually read from function readCharFromDecodingBuffer().
// The function is blocking until the required number of characters is read.
// The last character read is returned by the function.
char anyrtttlGetCharAdaptorFunc(const char * iBuffer) {
  // Define how man characters to read
  size_t count = (size_t)(iBuffer - previous_read_address);

  // Read that many character
  for(size_t i=0; i < count; i++) {
    previous_read_character = readCharFromDecodingBuffer();
  }

  // Update the previous address read
  previous_read_address += count;

  // Convert newline characters as a NULL character to simulate the end of the RTTTL melody.
  // This triggers the melody playback code to stop.
  if (previous_read_character == '\r' || previous_read_character == '\n')
    previous_read_character = '\0';

  // Return the last character read
  return previous_read_character;
}

#ifdef ESP32
// Function esp32GetChannelForPin() maps a channel for a given pin.
// Returns a value between 0 and n where n is the maximum of channel for your board.
// Returns ESP32_INVALID_CHANNEL if there is no assigned channel for the given pin number.
// See your board documentation for details.
// See https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html#led-control-ledc
uint8_t getChannelForPin(uint8_t pin) {
  if (pin == BUZZER_PIN) return 0; // using channel 0 for pin BUZZER_PIN
  return 0xFF; // invalid
}
#endif // ESP32

void resetDecoding() {
  // init fake melody buffers
  previous_read_address = (const char*)0x0000;
  fake_melody_address = (const char*)0x0001; // Initialized to 1 charaacter after `previous_read_address` to force reading 1 character on the first call to anyrtttlGetCharAdaptorFunc().
  previous_read_character = '\0';

  // init decoding buffer with a new empty melody name with it's separator
  decoding_buffer[0] = ':';
  decoding_buffer[1] = '\0';
  decoding_buffer_index = 0;
  decoding_state = DECODER_STATE_NAME;

  tetris16_notes_index = 0;

  bitreader.reset();
  bitreader.setBuffer(tetris10_note_bits);
}

#ifdef DEBUG_FUNCTION_READFROMDECODINGBUFFER
inline void debugReadFromDecodingBuffer() {
  char c = readCharFromDecodingBuffer();
  while(c!='\0') {
    Serial.print(c);
    c = readCharFromDecodingBuffer();
  }

  while(true) {
    delay(1000);
  }
}
#endif // DEBUG_FUNCTION_READFROMDECODINGBUFFER

void setup() {
  // silence BUZZER_PIN asap
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  resetDecoding();

  Serial.begin(115200);
  Serial.println("ready");

#ifdef ESP32
  // setup AnyRtttl for ESP32
  esp32::setChannelMapFunction(&getChannelForPin);  // Required for functions using esp32 core version 2.x.
  anyrtttl::setToneFunction(&esp32::tone);          // tell AnyRtttl to use AnyRtttl's specialized esp32 tone function.
  anyrtttl::setNoToneFunction(&esp32::noTone);      // tell AnyRtttl to use AnyRtttl's specialized esp32 noTone() function.

  // setup the pin for PWM tones.
  esp32::toneSetup(BUZZER_PIN);
#endif // ESP32

#ifdef DEBUG_FUNCTION_READFROMDECODINGBUFFER
  // debugging
  debugReadFromDecodingBuffer();
#endif // DEBUG_FUNCTION_READFROMDECODINGBUFFER
}

void loop() {
  #ifdef SKETCH_NON_BLOCKING_MODE
    // Non-blocking example

    if ( anyrtttl::nonblocking::done() ) 
    {
      // We are done playing the previous melody or
      // it is the first time we enter the loop() function.
      
      // Prepare for playing the next melody
      resetDecoding();
      
      // Start playing a new one
      anyrtttl::nonblocking::begin(BUZZER_PIN, fake_melody_address, &anyrtttlGetCharAdaptorFunc);
    }
    else {
      // continue playing
      anyrtttl::nonblocking::play();
    }
  #else
    // Blocking example

    // Prepare for playing the next melody
    resetDecoding();

    // Start playing
    anyrtttl::blocking::play(BUZZER_PIN, fake_melody_address, &anyrtttlGetCharAdaptorFunc);

    delay(1000);
  #endif // SKETCH_NON_BLOCKING_MODE
}
```
