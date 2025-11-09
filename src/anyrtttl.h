// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#ifndef ANY_RTTTL_H
#define ANY_RTTTL_H

#define ANY_RTTTL_VERSION 2.4

#include "Arduino.h"
#include "binrtttl.h"
#include "pitches.h"

//#define ANY_RTTTL_DEBUG
//#define ANY_RTTTL_INFO

namespace anyrtttl
{
/****************************************************************************
 * Constants
 ****************************************************************************/
#define RTTTL_DEFAULT_DURATION  4 // default duration for melodies that do not specify a melody duration in the control section.
#define RTTTL_DEFAULT_OCTAVE    6 // default   octave for melodies that do not specify a melody   octave in the control section.
#define RTTTL_DEFAULT_BPM      63 // default      bpm for melodies that do not specify a melody      bpm in the control section.



/****************************************************************************
 * Custom typedefs
 ****************************************************************************/

/****************************************************************************
 * Description:
 *   Defines a function pointer to get the first character from a buffer.
 * Parameters:
 *   iBuffer:   Address of a buffer to read.
 * Returns:
 *   Returns a single character of an RTTTL token.
 ****************************************************************************/
typedef char (*GetCharFuncPtr)(const char * iBuffer);

/****************************************************************************
 * Description:
 *   Defines a custom type for storing a note duration.
 ****************************************************************************/
typedef uint16_t TONE_DURATION;

/****************************************************************************
 * Structure definitions
 ****************************************************************************/
typedef struct rtttl_context_t {
  byte pin;                   // the pin assigned to this context.
  const char * buffer;        // address of the melody. Can be from RAM or PROGMEM address space.
  const char * next;          // address of the next byte to process within buffer.
  GetCharFuncPtr getCharPtr;  // a custom function to get the first byte from `next` buffer.
  byte default_dur;           // default duration of notes in the melody. Use this value for notes that do not specify a duration.
  byte default_oct;           // default  octave  of notes in the melody. Use this value for notes that do not specify an octave.
  RTTTL_BPM bpm;              // melody beats per minutes. BPM usually expresses the number of quarter notes per minute.
  RTTTL_DURATION wholenote;   // time for whole note in milliseconds.
  RTTTL_OCTAVE_VALUE scale;   // current note scale.
  TONE_DURATION duration;     // current note duration.
  unsigned long nextNoteMs;   // timestamp in milliseconds of end of note (start of next).
  bool playing;
  byte noteOffset;
} rtttl_context_t;

/****************************************************************************
 * Description:
 *   Define a global context that is used to support legacy function apis.
 ****************************************************************************/
extern rtttl_context_t gGlobalContext;

/****************************************************************************
 * Custom functions
 ****************************************************************************/

/****************************************************************************
 * Description:
 *   Initialize an RtttlContext with default values.
 * Parameters:
 *   c:      The context to initialize
 ****************************************************************************/
void initContext(rtttl_context_t & c);

/****************************************************************************
 * Description:
 *   Defines a function pointer to a tone() function
 ****************************************************************************/
typedef void (*ToneFuncPtr)(uint8_t pin, unsigned int frequency, unsigned long duration);

/****************************************************************************
 * Description:
 *   Defines a function pointer to a noTone() function
 ****************************************************************************/
typedef void (*NoToneFuncPtr)(uint8_t pin);

/****************************************************************************
 * Description:
 *   Defines a function pointer to a delay() function
 ****************************************************************************/
#if defined(ESP32)
typedef void (*DelayFuncPtr)(uint32_t);
#else
typedef void (*DelayFuncPtr)(unsigned long);
#endif

/****************************************************************************
 * Description:
 *   Defines a function pointer to a millis() function
 ****************************************************************************/
typedef unsigned long (*MillisFuncPtr)(void);

/****************************************************************************
 * Description:
 *   Defines the tone() function used by AnyRtttl.
 * Parameters:
 *   iFunc: Pointer to a tone() replacement function.
 ****************************************************************************/
void setToneFunction(ToneFuncPtr iFunc);

/****************************************************************************
 * Description:
 *   Defines the noTone() function used by AnyRtttl.
 * Parameters:
 *   iFunc: Pointer to a noTone() replacement function.
 ****************************************************************************/
void setNoToneFunction(NoToneFuncPtr iFunc);

/****************************************************************************
 * Description:
 *   Defines the delay() function used by AnyRtttl.
 * Parameters:
 *   iFunc: Pointer to a delay() replacement function.
 ****************************************************************************/
void setDelayFunction(DelayFuncPtr iFunc);

/****************************************************************************
 * Description:
 *   Defines the millis() function used by AnyRtttl.
 * Parameters:
 *   iFunc: Pointer to a millis() replacement function.
 ****************************************************************************/
void setMillisFunction(MillisFuncPtr iFunc);

/****************************************************************************
 * Description:
 *   Read the first byte of a buffer stored in RAM.
 * Parameters:
 *   iBuffer: A string buffer stored in RAM.
 ****************************************************************************/
char readCharMem(const char * iBuffer);

/****************************************************************************
 * Description:
 *   Read the first byte of a buffer stored in PROGMEM.
 * Parameters:
 *   iBuffer: A string buffer stored in PROGMEM.
 ****************************************************************************/
char readCharPgm(const char * iBuffer);


/****************************************************************************
 * Blocking API
 ****************************************************************************/
namespace blocking
{

/****************************************************************************
 * Description:
 *   Plays a native RTTTL melody.
 * Parameters:
 *   c:               An RTTTL context to keep track of the melody's state.
 *   iPin:            The pin which is connected to the piezo buffer.
 *   iBuffer:         The string buffer of the RTTTL melody.
 *   iGetCharFuncPtr: A function pointer to read 1 byte (char) from the given buffer.
 ****************************************************************************/
void play(rtttl_context_t & c, byte iPin, const char * iBuffer, GetCharFuncPtr iGetCharFuncPtr);

/****************************************************************************
 * Legacy API functions
 ****************************************************************************/
inline void play(byte iPin, const char * iBuffer, GetCharFuncPtr iGetCharFuncPtr) { play(anyrtttl::gGlobalContext, iPin, iBuffer, iGetCharFuncPtr); }
inline void play(byte iPin, const char * iBuffer)                                 { play(anyrtttl::gGlobalContext, iPin, iBuffer, &anyrtttl::readCharMem); }
inline void play(byte iPin, const __FlashStringHelper* str)                       { play(anyrtttl::gGlobalContext, iPin, (const char *)str, &anyrtttl::readCharPgm); }
inline void playProgMem(byte iPin, const char * iBuffer)                          { play(anyrtttl::gGlobalContext, iPin, iBuffer, &anyrtttl::readCharPgm); }
inline void play_P(byte iPin, const char * iBuffer)                               { play(anyrtttl::gGlobalContext, iPin, iBuffer, &anyrtttl::readCharPgm); }
inline void play_P(byte iPin, const __FlashStringHelper* str)                     { play(anyrtttl::gGlobalContext, iPin, (const char *)str, &anyrtttl::readCharPgm); }


/****************************************************************************v
 * Description:
 *   Plays a RTTTL melody which is encoded as 16 bits per notes.
 * Parameters:
 *   iPin:      The pin which is connected to the piezo buffer.
 *   iBuffer:   The binary buffer of the RTTTL melody. See remarks for details.
 *   iNumNotes: The number of notes within the given melody buffer.
 * Remarks:
 *   The first 16 bits of the buffer are reserved for the default section.
 *   See the definition of RTTTL_CONTROL_SECTION union for details.
 *   Each successive notes are encoded as 16 bits per note as defined by
 *   RTTTL_NOTE union.
 ****************************************************************************/
void play16Bits(int iPin, const unsigned char * iBuffer, int iNumNotes);

/****************************************************************************
 * Description:
 *   Defines a function pointer which is used by the play10Bits() function as 
 *   a bit provider function. The signature of a compatible function must be
 *   the following: uint16_t foo(uint8_t iNumBits);
 ****************************************************************************/
typedef uint16_t (*BitReadFuncPtr)(uint8_t);

/****************************************************************************
 * Description:
 *   Plays a RTTTL melody which is encoded as 10 bits per notes.
 * Parameters:
 *   iPin:      The pin which is connected to the piezo buffer.
 *   iNumNotes: The number of notes within the given melody.
 *   iFuncPtr:  Pointer to a function which is used by play10Bits() as a bit  The binary buffer of the RTTTL melody. See remarks for details.
 * Remarks:
 *   The first 16 bits of the buffer are reserved for the default section.
 *   See the definition of RTTTL_CONTROL_SECTION union for details.
 *   Each successive notes are encoded as 10 bits per note as defined by
 *   RTTTL_NOTE union.
 ****************************************************************************/
void play10Bits(int iPin, int iNumNotes, BitReadFuncPtr iFuncPtr);

}; //blocking namespace



/****************************************************************************
 * Non-blocking API
 ****************************************************************************/
namespace nonblocking
{

/****************************************************************************
 * Description:
 *   Setups the AnyRtttl library for non-blocking mode and ready to
 *   decode a new RTTTL song.
 * Parameters:
 *   c:               An RTTTL context to keep track of the melody's state.
 *   iPin:            The pin which is connected to the piezo buffer.
 *   iBuffer:         The string buffer of the RTTTL song.
 *   iGetCharFuncPtr: A function pointer to read 1 byte (char) from the given buffer.
 ****************************************************************************/
void begin(rtttl_context_t & c, byte iPin, const char * iBuffer, GetCharFuncPtr iGetCharFuncPtr);

/****************************************************************************
 * Description:
 *   Automatically plays a new note when required.
 *   This function must constantly be called within the loop() function.
 *   Warning: inserting too long delays within the loop function may
 *   disrupt the NON-BLOCKING RTTTL library from playing properly.
 * Parameters:
 *   c:       An RTTTL context to keep track of the melody's state.
 ****************************************************************************/
void play(rtttl_context_t & c);

/****************************************************************************
 * Description:
 *   Stops playing the current song.
 * Parameters:
 *   c:       An RTTTL context to keep track of the melody's state.
 ****************************************************************************/
void stop(rtttl_context_t & c);

/****************************************************************************
 * Description:
 *   Return true when the library is playing the given RTTTL melody.
 * Parameters:
 *   c:       An RTTTL context to keep track of the melody's state.
 ****************************************************************************/
bool isPlaying(rtttl_context_t & c);

/****************************************************************************
 * Description:
 *   Return true when the library is done playing the given RTTTL song.
 * Parameters:
 *   c:       An RTTTL context to keep track of the melody's state.
 ****************************************************************************/
bool done(rtttl_context_t & c);

// helper functions
inline void begin(rtttl_context_t & c, byte iPin, const char * iBuffer)             { begin(c, iPin, iBuffer, &anyrtttl::readCharMem); }
inline void begin(rtttl_context_t & c, byte iPin, const __FlashStringHelper* str)   { begin(c, iPin, (const char *)str, &anyrtttl::readCharPgm); }
inline void beginProgMem(rtttl_context_t & c, byte iPin, const char * iBuffer)      { begin(c, iPin, iBuffer, &anyrtttl::readCharPgm); }
inline void begin_P(rtttl_context_t & c, byte iPin, const char * iBuffer)           { begin(c, iPin, iBuffer, &anyrtttl::readCharPgm); }
inline void begin_P(rtttl_context_t & c, byte iPin, const __FlashStringHelper* str) { begin(c, iPin, (const char *)str, &anyrtttl::readCharPgm); }

/****************************************************************************
 * Legacy API functions
 ****************************************************************************/
inline void begin(byte iPin, const char * iBuffer, GetCharFuncPtr iGetCharFuncPtr)  { begin(anyrtttl::gGlobalContext, iPin, iBuffer, iGetCharFuncPtr); }
inline void begin(byte iPin, const char * iBuffer)                                  { begin(anyrtttl::gGlobalContext, iPin, iBuffer, &anyrtttl::readCharMem); }
inline void begin(byte iPin, const __FlashStringHelper* str)                        { begin(anyrtttl::gGlobalContext, iPin, (const char *)str, &anyrtttl::readCharPgm); }
inline void beginProgMem(byte iPin, const char * iBuffer)                           { begin(anyrtttl::gGlobalContext, iPin, iBuffer, &anyrtttl::readCharPgm); }
inline void begin_P(byte iPin, const char * iBuffer)                                { begin(anyrtttl::gGlobalContext, iPin, iBuffer, &anyrtttl::readCharPgm); }
inline void begin_P(byte iPin, const __FlashStringHelper* str)                      { begin(anyrtttl::gGlobalContext, iPin, (const char *)str, &anyrtttl::readCharPgm); }
inline void play()                                                                  { play(anyrtttl::gGlobalContext); }
inline void stop()                                                                  { stop(anyrtttl::gGlobalContext); }
inline bool done()                                                                  { return done(anyrtttl::gGlobalContext); }
inline bool isPlaying()                                                             { return isPlaying(anyrtttl::gGlobalContext); }

}; //nonblocking namespace



/****************************************************************************
 * ESP32 support functions
 ****************************************************************************/
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
 *   Defines the getChannelForPin() function used by AnyRtttl.
 * Parameters:
 *   iFunc: Pointer to a tone() replacement function.
 ****************************************************************************/
void setChannelMapFunction(ChannelMapFuncPtr iFunc);

/****************************************************************************
 * Description:
 *   Return the channel number registered for a given pin.
 ****************************************************************************/
uint8_t getChannelMapZero(uint8_t pin);

/****************************************************************************
 * Description:
 *   Function esp32NoTone() stop the PWM signal for the given pin.
 *   It does not detach the pin from it's assigned channel.
 *   You can have sequential calls to esp32Tone() and esp32NoTone().
 *   without having to call esp32ToneSetup() between calls.
 ****************************************************************************/
void noTone(uint8_t pin);

/****************************************************************************
 * Description:
 *   Function esp32Tone() set a pin to output a PWM signal that matches the given frequency.
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
 *   Function esp32ToneSetup() setup the given pin to output a PWM signal
 *   for generating tones with a piezo buzzer.
 ****************************************************************************/
void toneSetup(uint8_t pin);

}; //esp32 namespace

#endif // ESP32

}; //anyrtttl namespace

#endif //ANY_RTTTL_H
