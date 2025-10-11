#include <anyrtttl.h>
#include <binrtttl.h>
#include <pitches.h>

// project's constants
#define BUZZER_PIN 5 // Using GPIO5 (pin labeled D5)
const char tetris[] PROGMEM = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
const char arkanoid[] PROGMEM = "Arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6";
const char mario[] PROGMEM = "mario:d=4,o=5,b=140:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
// Note: James Bond theme is defined as inline code in loop() function (also stored in flash memory) 

#ifdef ESP_ARDUINO_VERSION
  #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    // Code specific to ESP32 core 3.x
    // Core 3.x manages channels automatically:
    // in all esp32 functions, input parameter 'channel' has been changed to 'pin'.
    // See official documentation for migrating from core 2.x to 3.x:
    // https://docs.espressif.com/projects/arduino-esp32/en/latest/migration_guides/2.x_to_3.0.html

    #define LEDC_RESOLUTION 10

    // Function esp32NoTone() stop the PWM signal for the given pin.
    // It does not detach the pin from it's assigned channel.
    // You can have sequential calls to esp32Tone() and esp32NoTone()
    // without having to call esp32ToneSetup() between calls.
    void esp32NoTone(uint8_t pin) {
      ledcWriteTone(pin, 0);
    }

    // Function esp32Tone() set a pin to output a PWM signal that matches the given frequency.
    // The duration argument is ignored. The function signature 
    // matches arduino's tone() function for compatibility reasons.
    // Arduino tone() function:
    //   https://docs.arduino.cc/language-reference/en/functions/advanced-io/tone/
    // ESP32 ledcWriteTone() function:
    //   https://github.com/espressif/arduino-esp32/blob/2.0.17/cores/esp32/esp32-hal-ledc.c#L118
    void esp32Tone(uint8_t pin, unsigned int frq, unsigned long duration) {
      // don't care about the given duration
      ledcWriteTone(pin, frq);
    }

    // Function esp32ToneSetup() setup the given pin to output a PWM signal for generating tones with a piezo buzzer.
    void esp32ToneSetup(uint8_t pin) {
      ledcAttach(pin, 1000, LEDC_RESOLUTION);
    }
    
  #elif ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
    // Code specific to ESP32 core 2.x

    #define ESP32_INVALID_CHANNEL 0xFF

    // Function esp32GetChannelForPin() maps a channel for a given pin.
    // Returns a value between 0 and n where n is the maximum of channel for your board.
    // Returns ESP32_INVALID_CHANNEL if there is no assigned channel for the given pin number.
    // See your board documentation for details.
    // See https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/ledc.html#led-control-ledc
    uint8_t esp32GetChannelForPin(uint8_t pin) {
      if (pin == BUZZER_PIN) return 0; // using channel 0 for pin BUZZER_PIN
      return ESP32_INVALID_CHANNEL;
    }

    // Function esp32NoTone() stop the PWM signal for the given pin.
    // It does not detach the pin from it's assigned channel.
    // You can have sequential calls to esp32Tone() and esp32NoTone()
    // without having to call esp32ToneSetup() between calls.
    //
    // Notes:
    // tone() and noTone() are not implemented for Arduino core for the ESP32
    // See https://github.com/espressif/arduino-esp32/issues/980
    // and https://github.com/espressif/arduino-esp32/issues/1720
    void esp32NoTone(uint8_t pin) {
      uint8_t channel = esp32GetChannelForPin(pin);
      ledcWriteTone(channel, 0); // Silence the buzzer without detaching
    }

    // Function esp32Tone() set a pin to output a PWM signal that matches the given frequency.
    // The duration argument is ignored. The function signature 
    // matches arduino's tone() function for compatibility reasons.
    // Arduino tone() function:
    //   https://docs.arduino.cc/language-reference/en/functions/advanced-io/tone/
    // ESP32 ledcWriteTone() function:
    //   https://github.com/espressif/arduino-esp32/blob/2.0.17/cores/esp32/esp32-hal-ledc.c#L118
    void esp32Tone(uint8_t pin, unsigned int frq, unsigned long duration) {
      // don't care about the given duration
      uint8_t channel = esp32GetChannelForPin(pin);
      ledcWriteTone(channel, frq);
    }

    // Function esp32ToneSetup() setup the given pin to output a PWM signal for generating tones with a piezo buzzer.
    void esp32ToneSetup(uint8_t pin) {
      uint8_t channel = esp32GetChannelForPin(pin);
      ledcAttachPin(BUZZER_PIN, channel); // Attach the pin to the LEDC channel
    }

  #else
    #error ESP32 arduino version unsupported
  #endif
#else
  // ESP_ARDUINO_VERSION is undefined.
  #error ESP32 arduino version unsupported.
#endif

void setup() {
  Serial.println(__LINE__);
  
  // silence BUZZER_PIN asap
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);
  Serial.println("ready");

  pinMode(BUZZER_PIN, OUTPUT);

#ifdef ESP32
  // setup AnyRtttl for ESP32
  esp32ToneSetup(BUZZER_PIN);
  anyrtttl::setToneFunction(&esp32Tone);
  anyrtttl::setNoToneFunction(&esp32NoTone);
#endif
}

void loop() {
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

  while(true)
  {
  }
}
