#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

/*
This example shows how AnyRtttl can be adapted to play an RTTTL melody that is received from the serial bus.
To use this example, connect to the board's serial port at 115200 baud and send the melody.
A newline character at the end is mandatory. A newline character triggers the end of the melody
and will stop the library from playing.

Copy one of the following in the clipboard and paste it in the serial monitor:

tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a
Arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6
mario:d=4,o=5,b=140:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6
*/

#define SKETCH_NON_BLOCKING_MODE

// Define the BUZZER_PIN for current board
#if defined(ESP32)
#define BUZZER_PIN 25 // Using GPIO25 (pin labeled D25)
#elif defined(ESP8266)
#define BUZZER_PIN  2 // Using GPIO2  (pin labeled D2)
#else // base arduino models
#define BUZZER_PIN 9
#endif

// project's constants
const char * fake_melody_address;     // The address of a fictionnal buffer containing the melody.
const char * previous_read_address;   // The address of the previous character read from melody.
char previous_read_character;         // The previous character read from the serial port.

// Function readSerialChar() read 1 character for the serial port.
// The function is blocking until a character is read from the serial port.
// The readed character is returned by the function.
char readSerialChar() {
  // Wait until a character is available
  while (Serial.available() == 0) {
    delay(1); // prevent watchdog from reseting some boards.
  }

  // Read and return the character
  char c = Serial.read();
  return c;
}

// Function serial2AnyRtttlGetCharAdaptorFunc() read the first character from a fictionnal "serial buffer".
// For this sketch, iBuffer is an invalid buffer address.
// The function checks iBuffer and compares it with the previous address
// to know how many characters to actually read from the serial port.
// The function is blocking until the required number of characters is read from the serial port.
// The last character read is returned by the function.
char serial2AnyRtttlGetCharAdaptorFunc(const char * iBuffer) {
  // Define how man characters to read
  size_t count = (size_t)(iBuffer - previous_read_address);

  // Read that many character
  for(size_t i=0; i < count; i++) {
    previous_read_character = readSerialChar();
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

void setup() {
  // silence BUZZER_PIN asap
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

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

}

void preparePlayingNextMelody() {
  // Sketch variables initialisation for next melody
  previous_read_address = (const char*)0x0000;
  fake_melody_address = (const char*)0x0001; // Initialized to 1 charaacter after `previous_read_address` to force reading 1 character on the first call to serial2AnyRtttlGetCharAdaptorFunc().
  previous_read_character = '\0';
}

void loop() {
  #ifdef SKETCH_NON_BLOCKING_MODE
    // Non-blocking example

    if ( anyrtttl::nonblocking::done() ) 
    {
      // We are done playing the previous melody or
      // it is the first time we enter the loop() function.
      
      // Prepare for playing the next melody
      preparePlayingNextMelody();
      
      // Start playing a new one
      anyrtttl::nonblocking::begin(BUZZER_PIN, fake_melody_address, &serial2AnyRtttlGetCharAdaptorFunc);
    }
    else {
      // continue playing
      anyrtttl::nonblocking::play();
    }
  #else
    // Blocking example

    // Prepare for playing the next melody
    preparePlayingNextMelody();

    // Start playing
    anyrtttl::blocking::play(BUZZER_PIN, fake_melody_address, &serial2AnyRtttlGetCharAdaptorFunc);

    delay(1000);
  #endif // SKETCH_NON_BLOCKING_MODE
}
