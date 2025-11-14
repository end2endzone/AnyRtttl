// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#ifndef ESP32_TONE_H
#define ESP32_TONE_H

#ifdef ESP32
namespace esp32
{

#ifndef ESP_ARDUINO_VERSION
  // ESP_ARDUINO_VERSION is undefined.
  #error This version of ESP32 arduino core does not define ESP_ARDUINO_VERSION.
#endif

/****************************************************************************
 * Description:
 *   Defines a function that allow one to map a channel number to a given pin.
 *   This definition is required if you are using ESP32 core version 2.x only.
 *   ESP32 core version 3.x manages channel automatically. 
 ****************************************************************************/
typedef uint8_t (*ChannelMapFuncPtr)(uint8_t);

/****************************************************************************
 * Description:
 *   Set a custom function to resolve a channel number for a given pin number.
 *   Required by esp32 tone functions when using ESP32 core version 2.x.
 * Parameters:
 *   iFunc: Pointer to a ChannelMapFuncPtr() type of function.
 ****************************************************************************/
void setChannelMapFunction(ChannelMapFuncPtr iFunc);

/****************************************************************************
 * Description:
 *   A function that return channel number 0 for any given pin.
 *   Compatible with setChannelMapFunction() function.
 ****************************************************************************/
uint8_t getChannelMapZero(uint8_t pin);

/****************************************************************************
 * Description:
 *   Function noTone() stop the PWM signal for the given pin.
 *   It does not detach the pin from it's assigned channel.
 *   You can have sequential calls to tone() and noTone().
 *   without having to call toneSetup() between calls.
 ****************************************************************************/
void noTone(uint8_t pin);

/****************************************************************************
 * Description:
 *   Function tone() set a pin to output a PWM signal that matches the given frequency.
 *   The duration argument is ignored. The function signature 
 *   matches arduino's tone() function for compatibility reasons.
 *   Arduino tone() function:
 *     https://docs.arduino.cc/language-reference/en/functions/advanced-io/tone/
 *   ESP32 ledcWriteTone() function:
 *     https://github.com/espressif/arduino-esp32/blob/2.0.17/cores/esp32/esp32-hal-ledc.c#L118
 ****************************************************************************/
void tone(uint8_t pin, unsigned int frq, unsigned long duration);

/****************************************************************************
 * Description:
 *   Function toneSetup() setup the given pin to output a PWM signal
 *   for generating tones with a piezo buzzer.
 ****************************************************************************/
void toneSetup(uint8_t pin);

}; //esp32 namespace

#endif // ESP32

#endif //ANY_RTTTL_H
