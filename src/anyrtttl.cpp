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

static const uint16_t gNotes[] = { NOTE_SILENT,
NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};

static const byte NOTES_PER_OCTAVE = 12;

// Define a global context for supporting legacy api functions.
// Legacy api functions did not required an rtttl_context_t as first parameter to play a melody.
// All legacy functions uses this default context as the first parameter for newer apis.
rtttl_context_t gGlobalContext = {0};

#define isDigitInlined(n) (n >= '0' && n <= '9')

inline char peekChar(rtttl_context_t & c)
{
  char character = c.getCharPtr(c.next);
  return character;
}

inline char readChar(rtttl_context_t & c)
{
  char character = c.getCharPtr(c.next);
  c.next++;
  return character;
}

int readInteger(rtttl_context_t & c)
{
  int value = 0;

  // read first character
  char character = peekChar(c); // peek only at the next character
  while(isDigitInlined(character))
  {
    character = readChar(c); // actually move the read offset
    value = (value * 10) + (character - '0');

    // read next character
    character = peekChar(c); // peek only at the next character
  }

  return value;
}

#ifdef ANY_RTTTL_DEBUG
void serialPrint(rtttl_context_t & c)
{
  // read first character
  char character = readChar(c);
  while(character) {
    Serial.print(character);

    // read next character
    character = readChar(c);
  }
}
#endif

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

char readCharMem(const char * iBuffer) {
  char output = *iBuffer;
  return output;
}

char readCharPgm(const char * iBuffer) {
  char output = pgm_read_byte_near(iBuffer);
  return output;
}



/****************************************************************************
 * Blocking API
 ****************************************************************************/
namespace blocking
{

void play(rtttl_context_t & c, byte iPin, const char* iBuffer, GetCharFuncPtr iGetCharFuncPtr) {
  // Implement blocking code using the non-blocking apis.

  // Init the context for playing this melody
  anyrtttl::nonblocking::begin(c, iPin, iBuffer, iGetCharFuncPtr);
  
  // Loop until the melody has played
  while( !anyrtttl::nonblocking::done(c) ) 
  {
    anyrtttl::nonblocking::play(c);
    delay(1); // prevent watchdog to reset the board.
  }
}



}; //blocking namespace


/****************************************************************************
 * Non-blocking API
 ****************************************************************************/
namespace nonblocking
{


//pre-declaration
void nextNote();
void nextNote(rtttl_context_t & c);

void begin(rtttl_context_t & c, byte iPin, const char * iBuffer, GetCharFuncPtr iGetCharFuncPtr)
{
  // init context
  initContext(c);

  //init values
  c.pin = iPin;
  c.melodyDefaultDur = RTTTL_DEFAULT_DURATION_VALUE;
  c.melodyDefaultOct = RTTTL_DEFAULT_OCTAVE_VALUE;
  c.bpm=RTTTL_DEFAULT_BPM_VALUE;
  c.buffer = iBuffer;
  c.next = iBuffer;
  c.getCharPtr = iGetCharFuncPtr;
  c.playing = true;

  int number = 0;
 
  #ifdef ANY_RTTTL_DEBUG
  Serial.print("playing: ");
  serialPrint(c);
  Serial.println();
  #endif

  //stop current note
  noTone(c.pin);

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  //read buffer until first note
  while(peekChar(c) != ':') c.next++; // ignore name
  c.next++;                           // skip ':'

  // get default duration
  if(peekChar(c) == 'd')
  {
    c.next += 2;                      // skip "d="
    number = readInteger(c);
    if(number > 0)
      c.melodyDefaultDur = number;
    c.next++;                         // skip comma
  }

  #ifdef ANY_RTTTL_INFO
  Serial.print("ddur: "); Serial.println(c.melodyDefaultDur, 10);
  #endif
  
  // get default octave
  if(peekChar(c) == 'o')
  {
    c.next += 2;                      // skip "o="
    number = readInteger(c);
    if(number >= 3 && number <= 7)
      c.melodyDefaultOct = number;
    c.next++;                         // skip comma
  }

  #ifdef ANY_RTTTL_INFO
  Serial.print("doct: "); Serial.println(c.melodyDefaultOct, 10);
  #endif
  
  // get BPM
  if(peekChar(c) == 'b')
  {
    c.next += 2;                      // skip "b="
    number = readInteger(c);
    c.bpm = number;
    c.next++;                         // skip colon
  }

  #ifdef ANY_RTTTL_INFO
  Serial.print("bpm: "); Serial.println(c.bpm, 10);
  #endif

  // BPM usually expresses the number of quarter notes per minute
  c.wholeNote = (60 * 1000L / c.bpm) * 4;  // this is the time for whole note (in milliseconds)

  #ifdef ANY_RTTTL_INFO
  Serial.print("wn: "); Serial.println(c.wholeNote, 10);
  #endif
}

void nextNote(rtttl_context_t & c)
{
  int number = 0;

  //stop current note
  _noTone(c.pin);

  // first, get note duration, if available
  number = readInteger(c);
  
  if(number > 0)
    c.duration = c.wholeNote / number;
  else
    c.duration = c.wholeNote / c.melodyDefaultDur;  // we will need to check if we are a dotted note after

  // now get the note
  c.noteOffset = findNoteOffsetFromNoteValue(peekChar(c));
  c.next++;                           // skip note letter

  // now, get optional '#' sharp
  if(peekChar(c) == '#')
  {
    c.noteOffset++;
    c.next++;                         // skip '#'
  }

  // now, get optional '.' dotted note
  if(peekChar(c) == '.')
  {
    c.duration += c.duration/2;
    c.next++;                         // skip '.'
  }

  // now, get scale
  if(isdigit(peekChar(c)))
  {
    c.scale = peekChar(c) - '0';
    c.next++;                         // skip scale
  }
  else
  {
    c.scale = c.melodyDefaultOct;
  }

  if(peekChar(c) == ',')
    c.next++;                         // skip comma for next note (or we may be at the end)

  // now play the note
  if(c.noteOffset)
  {
    #ifdef ANY_RTTTL_INFO
    Serial.print("Playing: ");
    Serial.print(c.scale, 10); Serial.print(' ');
    Serial.print(c.noteOffset, 10); Serial.print(" (");
    Serial.print(gNotes[(c.scale - 4) * NOTES_PER_OCTAVE + c.noteOffset], 10);
    Serial.print(") ");
    Serial.println(c.duration, 10);
    #endif
    
    uint16_t frequency = gNotes[(c.scale - 4) * NOTES_PER_OCTAVE + c.noteOffset];
    _tone(c.pin, frequency, c.duration);
    
    c.nextNoteMs = _millis() + (c.duration+1);
  }
  else
  {
    #ifdef ANY_RTTTL_INFO
    Serial.print("Pausing: ");
    Serial.println(duration, 10);
    #endif
    
    c.nextNoteMs = _millis() + (c.duration);
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
  if (m < c.nextNoteMs)
  {
    #ifdef ANY_RTTTL_DEBUG
    Serial.println("still playing a note...");
    #endif
    
    //wait until the note is completed
    return;
  }

  //ready to play the next note
  if (peekChar(c) == '\0')
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
    
    nextNote(c);
  }
}

void stop(rtttl_context_t & c)
{
  if (c.playing)
  {
    //increase song buffer until the end
    while (peekChar(c) != '\0')
    {
      c.next++;
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

}; //nonblocking namespace

void initContext(rtttl_context_t & c) {
  c.pin = -1;
  c.buffer = NULL;
  c.next = NULL;
  c.getCharPtr = &readCharMem;
  c.melodyDefaultDur = RTTTL_DEFAULT_DURATION_VALUE;
  c.melodyDefaultOct = RTTTL_DEFAULT_OCTAVE_VALUE;
  c.bpm = RTTTL_DEFAULT_BPM_VALUE;
  c.wholeNote = 0;
  c.scale = 0;
  c.duration = 0;
  c.nextNoteMs = 0;
  c.playing = false;
  c.noteOffset = 0;
}

}; //anyrtttl namespace
