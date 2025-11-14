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
#include "rtttl_utils.h"
#include "esp32_tone.h"
#include "pitches.h"

//#define ANY_RTTTL_DEBUG
//#define ANY_RTTTL_INFO

namespace anyrtttl
{
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
  bpm_value_t bpm;            // melody beats per minutes. BPM usually expresses the number of quarter notes per minute.
  duration_value_t wholenote; // time for whole note in milliseconds.
  octave_value_t scale;       // current note scale.
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

}; //anyrtttl namespace

#endif //ANY_RTTTL_H
