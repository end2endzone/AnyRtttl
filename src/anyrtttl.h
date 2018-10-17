// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#ifndef ANY_RTTTL_H
#define ANY_RTTTL_H

#define ANY_RTTTL_VERSION 2.2

#include "Arduino.h"
#include "pitches.h"

//#define ANY_RTTTL_DEBUG
//#define ANY_RTTTL_INFO

namespace anyrtttl
{

/****************************************************************************
 * Custom functions
 ****************************************************************************/


/****************************************************************************
 * Description:
 *   Defines a function pointer to a tone() function
 ****************************************************************************/
typedef void (*ToneFuncPtr)(byte, uint16_t, uint32_t);

/****************************************************************************
 * Description:
 *   Defines a function pointer to a noTone() function
 ****************************************************************************/
typedef void (*NoToneFuncPtr)(byte);

/****************************************************************************
 * Description:
 *   Defines a function pointer to a delay() function
 ****************************************************************************/
typedef void (*DelayFuncPtr)(uint32_t);

/****************************************************************************
 * Description:
 *   Defines a function pointer to a millis() function
 ****************************************************************************/
typedef uint32_t (*MillisFuncPtr)(void);

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
 * Blocking API
 ****************************************************************************/
namespace blocking
{

/****************************************************************************
 * Description:
 *   Plays a native RTTTL melody.
 * Parameters:
 *   iPin:    The pin which is connected to the piezo buffer.
 *   iBuffer: The string buffer of the RTTTL melody.
 ****************************************************************************/
void play(byte iPin, const char * iBuffer);

/****************************************************************************
 * Description:
 *   Plays a native RTTTL melody which is stored in Program Memory (PROGMEM).
 * Parameters:
 *   iPin:    The pin which is connected to the piezo buffer.
 *   iBuffer: The string buffer of the RTTTL melody.
 ****************************************************************************/
void playProgMem(byte iPin, const char * iBuffer);

/****************************************************************************
 * Description:
 *   Plays a RTTTL melody which is encoded as 16 bits per notes.
 * Parameters:
 *   iPin:      The pin which is connected to the piezo buffer.
 *   iBuffer:   The binary buffer of the RTTTL melody. See remarks for details.
 *   iNumNotes: The number of notes within the given melody buffer.
 * Remarks:
 *   The first 16 bits of the buffer are reserved for the default section.
 *   See the definition of RTTTL_DEFAULT_VALUE_SECTION union for details.
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
 *   See the definition of RTTTL_DEFAULT_VALUE_SECTION union for details.
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
 *   iPin:    The pin which is connected to the piezo buffer.
 *   iBuffer: The string buffer of the RTTTL song.
 ****************************************************************************/
void begin(byte iPin, const char * iBuffer);

/****************************************************************************
 * Description:
 *   Automatically plays a new note when required.
 *   This function must constantly be called within the loop() function.
 *   Warning: inserting too long delays within the loop function may
 *   disrupt the NON-BLOCKING RTTTL library from playing properly.
 ****************************************************************************/
void play();

/****************************************************************************
 * Description:
 *   Stops playing the current song.
 ****************************************************************************/
void stop();

/****************************************************************************
 * Description:
 *   Return true when the library is playing the given RTTTL melody.
 ****************************************************************************/
bool isPlaying();

/****************************************************************************
 * Description:
 *   Return true when the library is done playing the given RTTTL song.
 ****************************************************************************/
bool done();

}; //nonblocking namespace

}; //anyrtttl namespace

#endif //ANY_RTTTL_H
