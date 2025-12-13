// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#include "Arduino.h"
#include "esp32_tone.h"

#ifdef ESP32
namespace esp32
{
  // A function that maps a given pin to a channel.
  // Default to an implementation which always maps to channel 0.
  // See function setChannelMapFunction() to change the default mapping function.
  ChannelMapFuncPtr channelMapFunc = &getChannelMapZero;

  void setChannelMapFunction(ChannelMapFuncPtr iFunc)
  {
    channelMapFunc = iFunc;
  }

  uint8_t getChannelMapZero(uint8_t pin) {
    return 0;
  }

  #ifdef ESP_ARDUINO_VERSION
    #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
      // Code specific to ESP32 core 3.x
      // Core 3.x manages channels automatically:
      // in all esp32 functions, input parameter 'channel' has been changed to 'pin'.
      // See official documentation for migrating from core 2.x to 3.x:
      // https://docs.espressif.com/projects/arduino-esp32/en/latest/migration_guides/2.x_to_3.0.html

      #define LEDC_RESOLUTION 10

      void noTone(uint8_t pin) {
        ledcWriteTone(pin, 0);
      }

      void tone(uint8_t pin, unsigned int frq, unsigned long duration) {
        // don't care about the given duration
        ledcWriteTone(pin, frq);
      }

      void toneSetup(uint8_t pin) {
        ledcAttach(pin, 1000, LEDC_RESOLUTION);
      }
      
    #elif ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
      // Code specific to ESP32 core 2.x
      // Core 2.x uses channels instead of pins:
      // in all esp32 functions, input parameter is a 'channel' instead of a given pin.
      // AnyRtttl uses the function assigned to 'channelMapFunc' to get the channel number
      // matching the user's pin.
      // See official documentation for migrating from core 2.x to 3.x:
      // https://docs.espressif.com/projects/arduino-esp32/en/latest/migration_guides/2.x_to_3.0.html

      #define ESP32_INVALID_CHANNEL 0xFF

      void noTone(uint8_t pin) {
        uint8_t channel = channelMapFunc(pin);
        ledcWriteTone(channel, 0); // Silence the buzzer without detaching
      }

      void tone(uint8_t pin, unsigned int frq, unsigned long duration) {
        // don't care about the given duration
        uint8_t channel = channelMapFunc(pin);
        ledcWriteTone(channel, frq);
      }

      void toneSetup(uint8_t pin) {
        uint8_t channel = channelMapFunc(pin);
        ledcAttachPin(pin, channel); // Attach the pin to the LEDC channel
      }

    #else
      #error ESP32 arduino version unsupported
    #endif
  #else
    // ESP_ARDUINO_VERSION is undefined.
    #error ESP32 arduino version unsupported.
  #endif

}; //esp32 namespace
#endif // ESP32
