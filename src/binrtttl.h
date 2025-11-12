// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#ifndef BINRTTTL_H
#define BINRTTTL_H

#include "Arduino.h"

#define RTTTL_SONG_NAME_SIZE 11
#define RTTTL_NOTE_SIZE_BITS 10

namespace anyrtttl
{

#pragma pack(push, 1) // exact fit - no padding
union RTTTL_NOTE
{
  unsigned short raw;
  struct
  {
    DURATION_INDEX durationIdx    : 3; //ranges from 0 to 7. Matches index of getNoteDurationFromIndex()
    NOTE_LETTER_INDEX noteIdx     : 3; //ranges from 0 to 7. Matches index of getNoteLetterFromIndex()
    bool pound                    : 1; //ranges from 0 to 1. True if the note is pound
    bool dotted                   : 1; //ranges from 0 to 1. True if the duration is dotted
    OCTAVE_INDEX octaveIdx        : 2; //ranges from 0 to 3. Matches index of getNoteOctaveFromIndex()
    unsigned char padding         : 6;
  };
};

union RTTTL_CONTROL_SECTION
{
  unsigned short raw;
  //struct 
  //{
  //  DURATION_INDEX durationIdx :  3; //ranges from 0 to 7.  Matches index of getNoteDurationFromIndex()
  //  OCTAVE_INDEX octaveIdx     :  2; //ranges from 0 to 3.  Matches index of getNoteOctaveFromIndex()
  //  RTTTL_BPM bpm              : 10; //ranges from 0 to 900.
  //  bool               padding :  1;
  //};
  struct //aligned on 8 bits types
  {
    DURATION_INDEX durationIdx :  3; //ranges from 0 to 7.  Matches index of getNoteDurationFromIndex()
    OCTAVE_INDEX     octaveIdx :  2; //ranges from 0 to 3.  Matches index of getNoteOctaveFromIndex()
    unsigned char              :  3; //padding for bpm
    unsigned char              :  7; //padding for bpm
    unsigned char              :  1; //padding
  };
  struct //aligned on 16 bits types
  {
    unsigned short        :  5; //padding for durationIdx and octaveIdx
    RTTTL_BPM         bpm : 10; //ranges from 1 to 900.
    unsigned short        :  1; //padding
  };
};
#pragma pack(pop) //back to whatever the previous packing mode was

}; //anyrtttl namespace

#endif //BINRTTTL_H
