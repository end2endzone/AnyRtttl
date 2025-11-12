#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

// Define the BUZZER_PIN for current board
#if defined(ESP32)
#define BUZZER_PIN 25 // Using GPIO25 (pin labeled D25)
#elif defined(ESP8266)
#define BUZZER_PIN  2 // Using GPIO2  (pin labeled D2)
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

static inline char digitToChar(int d) {
    return '0' + d;
}

char * mini_itoa(int n, char *buf) {
    int div = 10000;
    while (div > 1 && n / div == 0)
        div /= 10;

    do {
        *buf = digitToChar(n / div);
        buf++;
        n %= div;
        div /= 10;
    } while (div);

    return buf;
}

void toString(const anyrtttl::RTTTL_CONTROL_SECTION & tetris16_ctrl_section, const anyrtttl::RTTTL_NOTE & note, char * buffer) {
  if (note.durationIdx != tetris16_ctrl_section.durationIdx)
    buffer = mini_itoa(anyrtttl::gNoteDurations[note.durationIdx], buffer);

  buffer[0] = anyrtttl::gNoteLetters[note.noteIdx];
  buffer++;

  if (note.pound) {
    buffer[0] = '#';
    buffer++;
  }

  if (note.dotted) {
    buffer[0] = '.';
    buffer++;
  }

  if (note.octaveIdx != tetris16_ctrl_section.octaveIdx)
    buffer = mini_itoa(anyrtttl::gNoteOctaves[note.octaveIdx], buffer);

  buffer[0] = ',';  // separator
  buffer++;

  buffer[0] = '\0';
}

void toString(const anyrtttl::RTTTL_CONTROL_SECTION & tetris16_ctrl_section, char * buffer) {
  // duration
  buffer[0] = 'd';
  buffer[1] = '=';
  buffer += 2;

  buffer = mini_itoa(anyrtttl::gNoteDurations[tetris16_ctrl_section.durationIdx], buffer);

  buffer[0] = ',';
  buffer++;

  // octave
  buffer[0] = 'o';
  buffer[1] = '=';
  buffer += 2;

  buffer = mini_itoa(anyrtttl::gNoteOctaves[tetris16_ctrl_section.octaveIdx], buffer);

  buffer[0] = ',';
  buffer++;

  // bmp
  buffer[0] = 'b';
  buffer[1] = '=';
  buffer += 2;

  buffer = mini_itoa(tetris16_ctrl_section.bpm, buffer);

  buffer[0] = ':';  // separator
  buffer++;

  buffer[0] = '\0';
}

inline void decodeControlSection() {
  toString(*tetris16_ctrl_section, decoding_buffer);
  decoding_buffer_index = 0;
}

inline void decodeNewNote() {
  toString(*tetris16_ctrl_section, tetris16_notes[tetris16_notes_index], decoding_buffer);
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
  anyrtttl::esp32::setChannelMapFunction(&getChannelForPin);  // Required for functions using esp32 core version 2.x.
  anyrtttl::setToneFunction(&anyrtttl::esp32::tone);          // tell AnyRtttl to use AnyRtttl's specialized esp32 tone function.
  anyrtttl::setNoToneFunction(&anyrtttl::esp32::noTone);      // tell AnyRtttl to use AnyRtttl's specialized esp32 noTone() function.

  // setup the pin for PWM tones.
  anyrtttl::esp32::toneSetup(BUZZER_PIN);
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
