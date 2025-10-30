#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

//#define LOCAL_DEBUGGING 1

#define BUZZER_1_PIN  5 // Using GPIO5  (pin labeled  D5)
#define BUZZER_2_PIN 32 // Using GPIO32 (pin labeled D32)

#ifdef LOCAL_DEBUGGING
#include "esp32-hal.h"
#include "esp32-hal-ledc.h"
#include "driver/ledc.h"
#include "esp32-hal-periman.h"
#include "soc/gpio_sig_map.h"
#include "esp_rom_gpio.h"
#include "hal/ledc_ll.h"
#endif // LOCAL_DEBUGGING

// project's constants
const char tetris[] PROGMEM = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char mario[] PROGMEM = "mario:d=4,o=5,b=140:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";

#if defined(ESP8266)
  #error This sketch is not compatible with ESP8266.
#endif

// project's variables
bool buzzer1_has_started = false; // flag to know if buzzer 1 has started playing 
bool buzzer2_has_started = false; // flag to know if buzzer 2 has started playing 
unsigned long buzzer1_start_time = 0; // time stamps at which we start playing buzzer 1
unsigned long buzzer2_start_time = 0; // time stamps at which we start playing buzzer 2
anyrtttl::rtttl_context_t buzzer1_context = {0};
anyrtttl::rtttl_context_t buzzer2_context = {0};

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

#ifdef LOCAL_DEBUGGING
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
#endif // LOCAL_DEBUGGING

void setup() {
  // silence buzzer pins asap
  pinMode(BUZZER_1_PIN, OUTPUT);
  digitalWrite(BUZZER_1_PIN, LOW);
  pinMode(BUZZER_2_PIN, OUTPUT);
  digitalWrite(BUZZER_2_PIN, LOW);

  Serial.begin(115200);
  Serial.println("ready");

  // setup AnyRtttl for ESP32
  anyrtttl::esp32::setChannelMapFunction(&getChannelForPin);  // Required for functions using esp32 core version 2.x.
  anyrtttl::setToneFunction(&anyrtttl::esp32::tone);          // tell AnyRtttl to use AnyRtttl's specialized esp32 tone function.
  anyrtttl::setNoToneFunction(&anyrtttl::esp32::noTone);      // tell AnyRtttl to use AnyRtttl's specialized esp32 noTone() function.

  // Define a unique channel for each buzzer pin.
  uint8_t channel_for_buzzer1 = getChannelForPin(BUZZER_1_PIN);
  uint8_t channel_for_buzzer2 = getChannelForPin(BUZZER_2_PIN);

  // Setup the pins for PWM tones.
  // To be able to play multiple rtttl melodies simultaneously,
  // we need to bind a unique timer for each buzzer.
  // In other words, we cannot use `anyrtttl::esp32::toneSetup(BUZZER_PIN)`.
  // To force a unique timer for each buzzer we register each pins to use a different frequency.
  ledcAttachChannel(BUZZER_1_PIN, 1000, 10, channel_for_buzzer1); // Attach the pin to the LEDC channel
  ledcAttachChannel(BUZZER_2_PIN, 2000, 10, channel_for_buzzer2); // Attach the pin to the LEDC channel
  
#ifdef LOCAL_DEBUGGING
  ledc_channel_handle_t *bus1 = (ledc_channel_handle_t *)perimanGetPinBus(BUZZER_1_PIN, ESP32_BUS_TYPE_LEDC);
  ledc_channel_handle_t *bus2 = (ledc_channel_handle_t *)perimanGetPinBus(BUZZER_2_PIN, ESP32_BUS_TYPE_LEDC);  
  printBus("bus1", bus1);
  printBus("bus2", bus2);
#endif // LOCAL_DEBUGGING

  delay(2000);

  // Compute time stamps at which each melody must start playing
  unsigned long now = millis();
  buzzer1_start_time = now + 2000;
  buzzer2_start_time = now + 5000;
}

void loop() {
  unsigned long now = millis();

  // Start playing on buzzer 1 ?
  if (!buzzer1_has_started & now >= buzzer1_start_time) {
    buzzer1_has_started = true;
    anyrtttl::nonblocking::beginProgMem(buzzer1_context, BUZZER_1_PIN, mario);
    anyrtttl::nonblocking::play(buzzer1_context);
  }

  // Start playing on buzzer 2 ?
  if (!buzzer2_has_started & now >= buzzer2_start_time) {
    buzzer2_has_started = true;
    anyrtttl::nonblocking::beginProgMem(buzzer2_context, BUZZER_2_PIN, tetris);
    anyrtttl::nonblocking::play(buzzer2_context);
  }

  // Continue playing each buzzer melodies ?
  if ( !anyrtttl::nonblocking::done(buzzer1_context) ) 
  {
    anyrtttl::nonblocking::play(buzzer1_context);
  }
  if ( !anyrtttl::nonblocking::done(buzzer2_context) ) 
  {
    anyrtttl::nonblocking::play(buzzer2_context);
  }

  // Check if both melodies have done playing
  if (buzzer1_has_started &&
      buzzer2_has_started &&
      anyrtttl::nonblocking::done(buzzer1_context) &&
      anyrtttl::nonblocking::done(buzzer2_context) ) {
    
    // stay silent
    while(true) {delay(1000);}
  }
}
