#include <anyrtttl.h>
#include <binrtttl.h>
#include <foortttl.h>
#include <binrtttl2.h>
#include <pitches.h>

#include "esp32-hal.h"
#include "esp32-hal-ledc.h"
#include "driver/ledc.h"
#include "esp32-hal-periman.h"
#include "soc/gpio_sig_map.h"
#include "esp_rom_gpio.h"
#include "hal/ledc_ll.h"


bool action1_has_run = false;
bool action2_has_run = false;
unsigned long millis_start = 0;

// project's constants
#define BUZZER_PIN 5 // Using GPIO5 (pin labeled D5)
//const char siren[] PROGMEM = "";

const char two_short[] PROGMEM = "two_short:d=4,o=5,b=100:16e6,16e6";
const char beep_long[] PROGMEM = "long:d=1,o=5,b=100:e6";
const char siren[] PROGMEM = "siren:d=8,o=5,b=100:d,e,d,e,d,e,d,e";
const char scale_up[] PROGMEM = "scale_up:d=32,o=5,b=100:c,c#,d#,e,f#,g#,a#,b";

const char tetris[] PROGMEM = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char arkanoid[] PROGMEM = "Arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6";
const char mario[] PROGMEM = "mario:d=4,o=5,b=140:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
const char bond[] PROGMEM = "Bond:d=4,o=5,b=80:32p,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d#6,16d#6,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d6,16c#6,16c#7,c.7,16g#6,16f#6,g#.6";
// Note: James Bond theme is defined as inline code in loop() function (also stored in flash memory) 

#if defined(ESP8266)
  #error This sketch is not compatible with ESP8266.
#endif

#define BUZZER_1_PIN  5 // Using GPIO5  (pin labeled  D5)
#define BUZZER_2_PIN 32 // Using GPIO32 (pin labeled D32)

// Function getChannelForPin() maps a channel for a given pin.
// Returns a value between 0 and n where n is the maximum of channel for your board.
// Returns ESP32_INVALID_CHANNEL if there is no assigned channel for the given pin number.
// See your board documentation for details.
// See https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html#led-control-ledc
uint8_t getChannelForPin(uint8_t pin) {
  if (pin == BUZZER_1_PIN) return 0; // using channel 0 for pin BUZZER_1_PIN
  if (pin == BUZZER_2_PIN) return 2; // using channel 2 for pin BUZZER_2_PIN
  return 0xFF; // invalid
}

void printBus(const char * name, ledc_channel_handle_t *bus) {
  Serial.println("=================");
  Serial.println(name);
  Serial.println((unsigned long)bus, HEX);
  Serial.println(bus->pin, HEX);
  Serial.println(bus->channel, HEX);
  Serial.println(bus->channel_resolution, HEX);
  Serial.println(bus->timer_num, HEX);
  Serial.println("=================");
}

void setup() {
  // silence BUZZER_PIN asap
  pinMode(BUZZER_1_PIN, OUTPUT);
  digitalWrite(BUZZER_1_PIN, LOW);
  pinMode(BUZZER_2_PIN, OUTPUT);
  digitalWrite(BUZZER_2_PIN, LOW);

  Serial.begin(115200);
  Serial.println("ready");

  // setup AnyRtttl 1 for ESP32
  anyrtttl::esp32::setChannelMapFunction(&getChannelForPin);  // Required for functions using esp32 core version 2.x.
  anyrtttl::setToneFunction(&anyrtttl::esp32::tone);          // tell AnyRtttl to use AnyRtttl's specialized esp32 tone function.
  anyrtttl::setNoToneFunction(&anyrtttl::esp32::noTone);      // tell AnyRtttl to use AnyRtttl's specialized esp32 noTone() function.

  // setup AnyRtttl 2 for ESP32
  foortttl::esp32::setChannelMapFunction(&getChannelForPin);  // Required for functions using esp32 core version 2.x.
  foortttl::setToneFunction(&foortttl::esp32::tone);          // tell AnyRtttl to use AnyRtttl's specialized esp32 tone function.
  foortttl::setNoToneFunction(&foortttl::esp32::noTone);      // tell AnyRtttl to use AnyRtttl's specialized esp32 noTone() function.

  // setup the pin for PWM tones.
  //foortttl::esp32::toneSetup(BUZZER_1_PIN);
  //foortttl::esp32::toneSetup(BUZZER_2_PIN);

  uint8_t channel_pin_1 = getChannelForPin(BUZZER_1_PIN);
  uint8_t channel_pin_2 = getChannelForPin(BUZZER_2_PIN);

  ledcAttachChannel(BUZZER_1_PIN, 1000, 10, channel_pin_1); // Attach the pin to the LEDC channel
  ledcAttachChannel(BUZZER_2_PIN, 2000, 10, channel_pin_2); // Attach the pin to the LEDC channel
  
  ledc_channel_handle_t *bus1 = (ledc_channel_handle_t *)perimanGetPinBus(BUZZER_1_PIN, ESP32_BUS_TYPE_LEDC);
  ledc_channel_handle_t *bus2 = (ledc_channel_handle_t *)perimanGetPinBus(BUZZER_2_PIN, ESP32_BUS_TYPE_LEDC);  

  printBus("bus1", bus1);
  printBus("bus2", bus2);

  /*
  Serial.println((unsigned long)bus1, HEX);
  Serial.println(bus1->pin, HEX);
  Serial.println(bus1->channel, HEX);
  Serial.println(bus1->channel_resolution, HEX);
  Serial.println(bus1->, HEX);
  Serial.println("=================");
  Serial.println("bus2");
  Serial.println((unsigned long)bus2, HEX);
  Serial.println(bus2->channel, HEX);
  Serial.println(bus2->timer_num, HEX);
  */

  //while(true){}

  delay(2000);
  millis_start = millis();
}

void stop_loop() {
  while(true)
  {
  }
}

void loop() {
  /*
  anyrtttl::blocking::playProgMem(BUZZER_PIN, tetris);
  delay(1000);

  anyrtttl::blocking::play_P(BUZZER_PIN, arkanoid);
  delay(1000);

#if defined(ESP8266) || defined(ESP32)
  anyrtttl::blocking::play(BUZZER_PIN, FPSTR(mario));
  delay(1000);
#endif

  anyrtttl::blocking::play(BUZZER_PIN, F("Bond:d=4,o=5,b=80:32p,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d#6,16d#6,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d6,16c#6,16c#7,c.7,16g#6,16f#6,g#.6"));
  delay(1000);
  */

  //anyrtttl::esp32::tone(BUZZER_2_PIN, NOTE_CS8, 1);
  //delay(1000);

  /*
  for(int i=0; i<3; i++) {
    anyrtttl::esp32::tone(BUZZER_1_PIN, 550, 1);
    delay(500);
    anyrtttl::esp32::noTone(BUZZER_1_PIN);
    delay(500);
  }
  */

  /*
  anyrtttl::nonblocking::beginProgMem(BUZZER_1_PIN, siren);
  anyrtttl::nonblocking::play();
  while( !anyrtttl::nonblocking::done() ) 
  {
    anyrtttl::nonblocking::play();
  }
  */


  unsigned long elapsed = millis() - millis_start;

  if (action1_has_run == false && elapsed > 2000) {
    // start action 1
    action1_has_run = true;
    anyrtttl::nonblocking::beginProgMem(BUZZER_1_PIN, mario);
    anyrtttl::nonblocking::play();
  }

  if (action2_has_run == false && elapsed > 4000) {
    // start action 1
    action2_has_run = true;
    foortttl::nonblocking::beginProgMem(BUZZER_2_PIN, tetris);
    foortttl::nonblocking::play();
  }

  // process any rtttl speakers
  if ( !anyrtttl::nonblocking::done() ) 
  {
    anyrtttl::nonblocking::play();
  }
  if ( !foortttl::nonblocking::done() ) 
  {
    foortttl::nonblocking::play();
  }

  // if both have done playing
  if (action1_has_run &&
      action2_has_run &&
      anyrtttl::nonblocking::done() &&
      foortttl::nonblocking::done() ) {
    stop_loop();
  }

  /*
  for(int i=0; i<3; i++) {
    anyrtttl::esp32::tone(BUZZER_2_PIN, NOTE_CS8, 1);
    delay(500);
    anyrtttl::esp32::noTone(BUZZER_2_PIN);
    delay(500);
  }
  */

  //delay(1000);
  //anyrtttl::esp32::noTone(BUZZER_2_PIN);

}
