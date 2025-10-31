// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------
#include "Arduino.h"
#include "anyrtttl.h"

/*********************************************************
 * RTTTL Library data
 *********************************************************/

namespace anyrtttl
{

const uint16_t notes[] = { NOTE_SILENT,
NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};

#define isdigit(n) (n >= '0' && n <= '9')
static const byte NOTES_PER_OCTAVE = 12;

// Define a global context for supporting legacy api functions.
// Legacy api functions did not required an rtttl_context_t as first parameter to play a melody.
// All legacy functions uses this default context as the first parameter for newer apis.
rtttl_context_t gGlobalContext = {0};

const char * readNumber(const char * iBuffer, int & oValue, ReadCharFuncPtr iReadCharFunc)
{
  oValue = 0;
  while(isdigit(iReadCharFunc(iBuffer)))
  {
    oValue = (oValue * 10) + (iReadCharFunc(iBuffer) - '0');
    iBuffer++;
  }
  return iBuffer;
}

void serialPrint(const char * iBuffer, ReadCharFuncPtr iReadCharFunc)
{
  char c = iReadCharFunc(iBuffer);
  while(c) {
    Serial.print(c);
    iBuffer++;
    c = iReadCharFunc(iBuffer);
  }
}

/****************************************************************************
 * Custom functions
 ****************************************************************************/

ToneFuncPtr _tone = &tone;
NoToneFuncPtr _noTone = &noTone;
DelayFuncPtr _delay = &delay;
MillisFuncPtr _millis = &millis;

void setToneFunction(ToneFuncPtr iFunc) {
  _tone = iFunc;
}

void setNoToneFunction(NoToneFuncPtr iFunc) {
  _noTone = iFunc;
}

void setDelayFunction(DelayFuncPtr iFunc) {
  _delay = iFunc;
}

void setMillisFunction(MillisFuncPtr iFunc) {
  _millis = iFunc;
}

char readChar(const char * iBuffer) {
  return *iBuffer;
}

char readChar_P(const char * iBuffer) {
  return pgm_read_byte_near(iBuffer);
}



/****************************************************************************
 * Blocking API
 ****************************************************************************/
namespace blocking
{

void play(rtttl_context_t & c, byte iPin, const char * iBuffer, ReadCharFuncPtr iReadCharFunc) {
  // Absolutely no error checking in here

  // init context
  initContext(c);
  
  c.pin = iPin;
  c.default_dur = 4;
  c.default_oct = 6;
  c.bpm = 63;
  c.buffer = iBuffer;
  c.readCharFunc = iReadCharFunc;
  
  #ifdef ANY_RTTTL_DEBUG
  Serial.print("playing: ");
  serialPrint(c.buffer, c.readCharFunc);
  Serial.println();
  #endif

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  while(c.readCharFunc(c.buffer) != ':') c.buffer++; // ignore name
  c.buffer++;                        // skip ':'

  // get default duration
  if(c.readCharFunc(c.buffer) == 'd')
  {
    c.buffer++; c.buffer++;           // skip "d="
    c.buffer = readNumber(c.buffer, c.tmpNumber, c.readCharFunc);
    if(c.tmpNumber > 0)
      c.default_dur = c.tmpNumber;
    c.buffer++;                      // skip comma
  }

  #ifdef ANY_RTTTL_INFO
  Serial.print("ddur: "); Serial.println(c.default_dur, 10);
  #endif

  // get default octave
  if(c.readCharFunc(c.buffer) == 'o')
  {
    c.buffer++; c.buffer++;           // skip "o="
    c.buffer = readNumber(c.buffer, c.tmpNumber, c.readCharFunc);
    if(c.tmpNumber >= 3 && c.tmpNumber <= 7)
      c.default_oct = c.tmpNumber;
    c.buffer++;                      // skip comma
  }

  #ifdef ANY_RTTTL_INFO
  Serial.print("doct: "); Serial.println(c.default_oct, 10);
  #endif

  // get BPM
  if(c.readCharFunc(c.buffer) == 'b')
  {
    c.buffer++; c.buffer++;         // skip "b="
    c.buffer = readNumber(c.buffer, c.tmpNumber, c.readCharFunc);
    c.bpm = c.tmpNumber;
    c.buffer++;                    // skip colon
  }

  #ifdef ANY_RTTTL_INFO
  Serial.print("bpm: "); Serial.println(c.bpm, 10);
  #endif

  // BPM usually expresses the number of quarter notes per minute
  c.wholenote = (60 * 1000L / c.bpm) * 4;  // this is the time for whole noteOffset (in milliseconds)

  #ifdef ANY_RTTTL_INFO
  Serial.print("wn: "); Serial.println(c.wholenote, 10);
  #endif

  // now begin note loop
  while(c.readCharFunc(c.buffer))
  {
    // first, get note duration, if available
    c.buffer = readNumber(c.buffer, c.tmpNumber, c.readCharFunc);
    
    if(c.tmpNumber)
      c.duration = c.wholenote / c.tmpNumber;
    else
      c.duration = c.wholenote / c.default_dur;  // we will need to check if we are a dotted noteOffset after

    // now get the note
    c.noteOffset = getNoteOffsetFromLetter(c.readCharFunc(c.buffer));
    c.buffer++;

    // now, get optional '#' sharp
    if(c.readCharFunc(c.buffer) == '#')
    {
      c.noteOffset++;
      c.buffer++;
    }

    // now, get optional '.' dotted note
    if(c.readCharFunc(c.buffer) == '.')
    {
      c.duration += c.duration/2;
      c.buffer++;
    }
  
    // now, get scale
    if(isdigit(c.readCharFunc(c.buffer)))
    {
      c.scale = c.readCharFunc(c.buffer) - '0';
      c.buffer++;
    }
    else
    {
      c.scale = c.default_oct;
    }

    if(c.readCharFunc(c.buffer) == ',')
      c.buffer++;       // skip comma for next note (or we may be at the end)

    // now play the note
    if(c.noteOffset)
    {
      uint16_t frequency = notes[(c.scale - 4) * NOTES_PER_OCTAVE + c.noteOffset];

      #ifdef ANY_RTTTL_INFO
      Serial.print("Playing: ");
      Serial.print(c.scale, 10); Serial.print(' ');
      Serial.print(c.noteOffset, 10); Serial.print(" (");
      Serial.print(frequency, 10);
      Serial.print(") ");
      Serial.println(c.duration, 10);
      #endif

      _tone(c.pin, frequency, c.duration);
      _delay(c.duration+1);
      _noTone(c.pin);
    }
    else
    {
      #ifdef ANY_RTTTL_INFO
      Serial.print("Pausing: ");
      Serial.println(duration, 10);
      #endif
      _delay(c.duration);
    }
  }
}

// Helper legacy api functions
void play(byte iPin, const char * iBuffer)              { play(gGlobalContext, iPin, iBuffer, &readChar); }
void play(byte iPin, const __FlashStringHelper* str)    { play(gGlobalContext, iPin, (const char *)str, &readChar_P); }
void playProgMem(byte iPin, const char * iBuffer)       { play(gGlobalContext, iPin, iBuffer, &readChar_P); }
void play_P(byte iPin, const char * iBuffer)            { play(gGlobalContext, iPin, iBuffer, &readChar_P); }
void play_P(byte iPin, const __FlashStringHelper* str)  { play(gGlobalContext, iPin, (const char *)str, &readChar_P); }

void play16Bits(int iPin, const unsigned char * iBuffer, int iNumNotes) {
  // Absolutely no error checking in here

  // Use global context for playing
  rtttl_context_t & c = gGlobalContext;
  initContext(c);

  c.pin = iPin;
  c.buffer = (const char*)iBuffer;

  RTTTL_DEFAULT_VALUE_SECTION * defaultSection = (RTTTL_DEFAULT_VALUE_SECTION *)iBuffer;
  RTTTL_NOTE * notesBuffer = (RTTTL_NOTE *)iBuffer;

  c.bpm = defaultSection->bpm;

  #ifdef ANY_RTTTL_DEBUG
  Serial.print("numNotes=");
  Serial.println(iNumNotes);
  // format: d=N,o=N,b=NNN:
  Serial.print("d=");
  Serial.print(getNoteDurationFromIndex(defaultSection->durationIdx));
  Serial.print(",o=");
  Serial.print(getNoteOctaveFromIndex(defaultSection->octaveIdx));
  Serial.print(",b=");
  Serial.println(c.bpm);
  #endif
  
  // BPM usually expresses the number of quarter notes per minute
  c.wholenote = (60 * 1000L / c.bpm) * 4;  // this is the time for whole noteOffset (in milliseconds)

  // now begin note loop
  for(int i=0; i<iNumNotes; i++) {
    const RTTTL_NOTE & n = notesBuffer[i+1]; //offset by 16 bits for RTTTL_DEFAULT_VALUE_SECTION

    // first, get note duration, if available
    c.duration = c.wholenote / getNoteDurationFromIndex(n.durationIdx);

    // now get the note
    //c.noteOffset = noteOffsets[n.noteIdx];
    c.noteOffset = getNoteOffsetFromLetterIndex(n.noteIdx);

    // now, get optional '#' sharp
    if(n.pound)
    {
      c.noteOffset++;
    }

    // now, get optional '.' dotted note
    if(n.dotted)
    {
      c.duration += c.duration/2;
    }

    // now, get scale
    c.scale = getNoteOctaveFromIndex(n.octaveIdx);

    if(c.noteOffset)
    {
      #ifdef ANY_RTTTL_DEBUG
      Serial.print(getNoteDurationFromIndex(n.durationIdx));
      static const char noteCharacterValues[] =   {'c','d','e','f','g','a','b','p'};
      Serial.print(noteCharacterValues[n.noteIdx]);
      Serial.print( (n.pound ? "#" : "") );
      Serial.print( (n.dotted ? "." : "") );
      Serial.println(getNoteOctaveFromIndex(n.octaveIdx));
      #endif
      
      uint16_t frequency = notes[(c.scale - 4) * NOTES_PER_OCTAVE + c.noteOffset];

      _tone(c.pin, frequency, c.duration);
      _delay(c.duration+1);
      _noTone(c.pin);
    }
    else
    {
      #ifdef ANY_RTTTL_DEBUG
      Serial.print(getNoteDurationFromIndex(n.durationIdx));
      static const char noteCharacterValues[] =   {'c','d','e','f','g','a','b','p'};
      Serial.print(noteCharacterValues[n.noteIdx]);
      Serial.print( (n.pound ? "#" : "") );
      Serial.print( (n.dotted ? "." : "") );
      Serial.println();
      #endif

      _delay(c.duration);
    }
  }
}

void play10Bits(int iPin, int iNumNotes, BitReadFuncPtr iFuncPtr) {
  // Absolutely no error checking in here

  // Use global context for playing
  rtttl_context_t & c = gGlobalContext;
  initContext(c);

  c.pin = iPin;

  //read default section
  RTTTL_DEFAULT_VALUE_SECTION defaultSection;
  defaultSection.raw = iFuncPtr(16);

  c.bpm = defaultSection.bpm;

  #ifdef ANY_RTTTL_DEBUG
  Serial.print("numNotes=");
  Serial.println(iNumNotes);
  // format: d=N,o=N,b=NNN:
  Serial.print("d=");
  Serial.print(getNoteDurationFromIndex(defaultSection.durationIdx));
  Serial.print(",o=");
  Serial.print(getNoteOctaveFromIndex(defaultSection.octaveIdx));
  Serial.print(",b=");
  Serial.println(c.bpm);
  #endif
  
  // BPM usually expresses the number of quarter notes per minute
  c.wholenote = (60 * 1000L / c.bpm) * 4;  // this is the time for whole noteOffset (in milliseconds)

  // now begin note loop
  for(int i=0; i<iNumNotes; i++) {
    RTTTL_NOTE n;
    n.raw = iFuncPtr(10);

    // first, get note duration, if available
    c.duration = c.wholenote / getNoteDurationFromIndex(n.durationIdx);

    // now get the note
    c.noteOffset = getNoteOffsetFromLetterIndex(n.noteIdx);

    // now, get optional '#' sharp
    if(n.pound)
    {
      c.noteOffset++;
    }

    // now, get optional '.' dotted note
    if(n.dotted)
    {
      c.duration += c.duration/2;
    }

    // now, get scale
    c.scale = getNoteOctaveFromIndex(n.octaveIdx);

    if(c.noteOffset)
    {
      #ifdef ANY_RTTTL_DEBUG
      Serial.print(getNoteDurationFromIndex(n.durationIdx));
      static const char noteCharacterValues[] =   {'c','d','e','f','g','a','b','p'};
      Serial.print(noteCharacterValues[n.noteIdx]);
      Serial.print( (n.pound ? "#" : "") );
      Serial.print( (n.dotted ? "." : "") );
      Serial.println(getNoteOctaveFromIndex(n.octaveIdx));
      #endif
      
      uint16_t frequency = notes[(c.scale - 4) * 12 + c.noteOffset];
      _tone(c.pin, frequency, c.duration);
      _delay(c.duration+1);
      _noTone(iPin);
    }
    else
    {
      #ifdef ANY_RTTTL_DEBUG
      Serial.print(getNoteDurationFromIndex(n.durationIdx));
      static const char noteCharacterValues[] =   {'c','d','e','f','g','a','b','p'};
      Serial.print(noteCharacterValues[n.noteIdx]);
      Serial.print( (n.pound ? "#" : "") );
      Serial.print( (n.dotted ? "." : "") );
      Serial.println();
      #endif

      _delay(c.duration);
    }
  }
}



}; //blocking namespace


/****************************************************************************
 * Non-blocking API
 ****************************************************************************/
namespace nonblocking
{


//pre-declaration
void nextnote();
void nextnote(rtttl_context_t & c);

void begin(rtttl_context_t & c, byte iPin, const char * iBuffer, ReadCharFuncPtr iReadCharFunc)
{
  // init context
  initContext(c);

  //init values
  c.pin = iPin;
  c.buffer = iBuffer;
  c.bufferIndex = 0;
  c.default_dur = 4;
  c.default_oct = 6;
  c.bpm=63;
  c.playing = true;
  c.delayToNextNote = 0;
  c.readCharFunc = iReadCharFunc;
  
  #ifdef ANY_RTTTL_DEBUG
  Serial.print("playing: ");
  serialPrint(c.buffer, c.readCharFunc);
  Serial.println();
  #endif

  //stop current note
  noTone(c.pin);

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  //read buffer until first note
  while(c.readCharFunc(c.buffer) != ':') c.buffer++;     // ignore name
  c.buffer++;                           // skip ':'

  // get default duration
  if(c.readCharFunc(c.buffer) == 'd')
  {
    c.buffer++; c.buffer++;               // skip "d="
    c.buffer = readNumber(c.buffer, c.tmpNumber, c.readCharFunc);
    if(c.tmpNumber > 0)
      c.default_dur = c.tmpNumber;
    c.buffer++;                         // skip comma
  }

  #ifdef ANY_RTTTL_INFO
  Serial.print("ddur: "); Serial.println(c.default_dur, 10);
  #endif
  
  // get default octave
  if(c.readCharFunc(c.buffer) == 'o')
  {
    c.buffer++; c.buffer++;               // skip "o="
    c.buffer = readNumber(c.buffer, c.tmpNumber, c.readCharFunc);
    if(c.tmpNumber >= 3 && c.tmpNumber <= 7)
      c.default_oct = c.tmpNumber;
    c.buffer++;                         // skip comma
  }

  #ifdef ANY_RTTTL_INFO
  Serial.print("doct: "); Serial.println(c.default_oct, 10);
  #endif
  
  // get BPM
  if(c.readCharFunc(c.buffer) == 'b')
  {
    c.buffer++; c.buffer++;              // skip "b="
    c.buffer = readNumber(c.buffer, c.tmpNumber, c.readCharFunc);
    c.bpm = c.tmpNumber;
    c.buffer++;                   // skip colon
  }

  #ifdef ANY_RTTTL_INFO
  Serial.print("bpm: "); Serial.println(c.bpm, 10);
  #endif

  // BPM usually expresses the number of quarter notes per minute
  c.wholenote = (60 * 1000L / c.bpm) * 4;  // this is the time for whole noteOffset (in milliseconds)

  #ifdef ANY_RTTTL_INFO
  Serial.print("wn: "); Serial.println(c.wholenote, 10);
  #endif
}

// helper functions
void begin(rtttl_context_t & c, byte iPin, const char * iBuffer)             { begin(c, iPin, iBuffer, &readChar); }
void begin(rtttl_context_t & c, byte iPin, const __FlashStringHelper* str)   { begin(c, iPin, (const char *)str, &readChar_P); }
void beginProgMem(rtttl_context_t & c, byte iPin, const char * iBuffer)      { begin(c, iPin, iBuffer, &readChar_P); }
void begin_P(rtttl_context_t & c, byte iPin, const char * iBuffer)           { begin(c, iPin, iBuffer, &readChar_P); }
void begin_P(rtttl_context_t & c, byte iPin, const __FlashStringHelper* str) { begin(c, iPin, (const char *)str, &readChar_P); }

void nextnote(rtttl_context_t & c)
{
  //stop current note
  _noTone(c.pin);

  // first, get note duration, if available
  c.buffer = readNumber(c.buffer, c.tmpNumber, c.readCharFunc);
  
  if(c.tmpNumber)
    c.duration = c.wholenote / c.tmpNumber;
  else
    c.duration = c.wholenote / c.default_dur;  // we will need to check if we are a dotted noteOffset after

  // now get the note
  c.noteOffset = getNoteOffsetFromLetter(c.readCharFunc(c.buffer));
  c.buffer++;

  // now, get optional '#' sharp
  if(c.readCharFunc(c.buffer) == '#')
  {
    c.noteOffset++;
    c.buffer++;
  }

  // now, get optional '.' dotted note
  if(c.readCharFunc(c.buffer) == '.')
  {
    c.duration += c.duration/2;
    c.buffer++;
  }

  // now, get scale
  if(isdigit(c.readCharFunc(c.buffer)))
  {
    c.scale = c.readCharFunc(c.buffer) - '0';
    c.buffer++;
  }
  else
  {
    c.scale = c.default_oct;
  }

  if(c.readCharFunc(c.buffer) == ',')
    c.buffer++;       // skip comma for next note (or we may be at the end)

  // now play the note
  if(c.noteOffset)
  {
    #ifdef ANY_RTTTL_INFO
    Serial.print("Playing: ");
    Serial.print(c.scale, 10); Serial.print(' ');
    Serial.print(c.noteOffset, 10); Serial.print(" (");
    Serial.print(notes[(c.scale - 4) * NOTES_PER_OCTAVE + c.noteOffset], 10);
    Serial.print(") ");
    Serial.println(c.duration, 10);
    #endif
    
    uint16_t frequency = notes[(c.scale - 4) * NOTES_PER_OCTAVE + c.noteOffset];
    _tone(c.pin, frequency, c.duration);
    
    c.delayToNextNote = _millis() + (c.duration+1);
  }
  else
  {
    #ifdef ANY_RTTTL_INFO
    Serial.print("Pausing: ");
    Serial.println(duration, 10);
    #endif
    
    c.delayToNextNote = _millis() + (c.duration);
  }
}

void play(rtttl_context_t & c)
{
  //if done playing the song, return
  if (!c.playing)
  {
    #ifdef ANY_RTTTL_DEBUG
    Serial.println("done playing...");
    #endif
    
    return;
  }
  
  //are we still playing a note ?
  unsigned long m = _millis();
  if (m < c.delayToNextNote)
  {
    #ifdef ANY_RTTTL_DEBUG
    Serial.println("still playing a note...");
    #endif
    
    //wait until the note is completed
    return;
  }

  //ready to play the next note
  if (c.readCharFunc(c.buffer) == '\0')
  {
    //no more notes. Reached the end of the last note

    #ifdef ANY_RTTTL_DEBUG
    Serial.println("end of note...");
    #endif
    
    c.playing = false;

    //stop current note (if any)
    _noTone(c.pin);

    return; //end of the song
  }
  else
  {
    //more notes to play...

    #ifdef ANY_RTTTL_DEBUG
    Serial.println("next note...");
    #endif
    
    nextnote(c);
  }
}

void stop(rtttl_context_t & c)
{
  if (c.playing)
  {
    //increase song buffer until the end
    while (c.readCharFunc(c.buffer) != '\0')
    {
      c.buffer++;
    }
  }

  c.playing = false;

  //stop current note (if any)
  _noTone(c.pin);
}

bool done(rtttl_context_t & c)
{
  return !c.playing;
}

bool isPlaying(rtttl_context_t & c)
{
  return c.playing;
}

/****************************************************************************
 * Legacy API functions
 ****************************************************************************/
void begin(byte iPin, const char * iBuffer, ReadCharFuncPtr iReadCharFunc)  { begin(gGlobalContext, iPin, iBuffer, &readChar); }
void begin(byte iPin, const char * iBuffer)                                 { begin(gGlobalContext, iPin, iBuffer, &readChar); }
void begin(byte iPin, const __FlashStringHelper* str)                       { begin(gGlobalContext, iPin, (const char *)str, &readChar_P); }
void beginProgMem(byte iPin, const char * iBuffer)                          { begin(gGlobalContext, iPin, iBuffer, &readChar_P); }
void begin_P(byte iPin, const char * iBuffer)                               { begin(gGlobalContext, iPin, iBuffer, &readChar_P); }
void begin_P(byte iPin, const __FlashStringHelper* str)                     { begin(gGlobalContext, iPin, (const char *)str, &readChar_P); }

void nextnote()
{
  nextnote(gGlobalContext);
}

void play()
{
  play(gGlobalContext);
}

void stop()
{
  stop(gGlobalContext);
}

bool done()
{
  return done(gGlobalContext);
}

bool isPlaying()
{
  return isPlaying(gGlobalContext);
}

}; //nonblocking namespace


/****************************************************************************
 * ESP32 support functions
 ****************************************************************************/
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

void initContext(rtttl_context_t & c) {
  c.buffer = NULL;
  c.readCharFunc = &readChar;
  c.bufferIndex = -32760;
  c.default_dur = 4;
  c.default_oct = 5;
  c.bpm = 63;
  c.wholenote = 0;
  c.pin = -1;
  c.delayToNextNote = 0; //milliseconds before playing the next note
  c.playing = false;
  c.duration = 0;
  c.noteOffset = 0;
  c.scale = 0;
  c.tmpNumber = 0;
}

}; //anyrtttl namespace
