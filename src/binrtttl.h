// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#ifndef BINRTTTL_H
#define BINRTTTL_H

#include "Arduino.h"
#include "rtttl_utils.h"

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
    duration_index_t durationIdx  : 3; //ranges from 0 to 7. Matches index of getDurationValueFromIndex()
    note_index_t noteIdx          : 3; //ranges from 0 to 7. Matches index of getNoteValueFromIndex()
    bool pound                    : 1; //ranges from 0 to 1. True if the note is pound
    bool dotted                   : 1; //ranges from 0 to 1. True if the duration is dotted
    octave_index_t octaveIdx      : 2; //ranges from 0 to 3. Matches index of getOctaveValueFromIndex()
    unsigned char padding         : 6;
  };
};

union RTTTL_CONTROL_SECTION
{
  unsigned short raw;
  //struct 
  //{
  //  duration_index_t durationIdx :  3; //ranges from 0 to 7.  Matches index of getDurationValueFromIndex()
  //  octave_index_t octaveIdx     :  2; //ranges from 0 to 3.  Matches index of getOctaveValueFromIndex()
  //  bpm_value_t bpm              : 10; //ranges from 0 to 900.
  //  bool                 padding :  1;
  //};
  struct //aligned on 8 bits types
  {
    duration_index_t durationIdx :  3; //ranges from 0 to 7.  Matches index of getDurationValueFromIndex()
    octave_index_t     octaveIdx :  2; //ranges from 0 to 3.  Matches index of getOctaveValueFromIndex()
    unsigned char                :  3; //padding for bpm
    unsigned char                :  7; //padding for bpm
    unsigned char                :  1; //padding
  };
  struct //aligned on 16 bits types
  {
    unsigned short        :  5; //padding for durationIdx and octaveIdx
    bpm_value_t       bpm : 10; //ranges from 1 to 900.
    unsigned short        :  1; //padding
  };
};
#pragma pack(pop) //back to whatever the previous packing mode was


inline char digitToChar(int d) {
    return '0' + d;
}

char * itoa(int n, char *buf) {
    int div = 10000;
    while (div > 1 && n / div == 0)
        div /= 10;

    do {
        *buf = digitToChar(n / div);
        buf++;
        n %= div;
        div /= 10;
    } while (div);

    return buf;
}

void toString(const RTTTL_CONTROL_SECTION & ctrl_section, const RTTTL_NOTE & note, char * buffer) {
  if (note.durationIdx != ctrl_section.durationIdx)
    buffer = itoa(gNoteDurations[note.durationIdx], buffer);

  buffer[0] = gNoteValues[note.noteIdx];
  buffer++;

  if (note.pound) {
    buffer[0] = '#';
    buffer++;
  }

  if (note.dotted) {
    buffer[0] = '.';
    buffer++;
  }

  if (note.octaveIdx != ctrl_section.octaveIdx)
    buffer = itoa(gNoteOctaves[note.octaveIdx], buffer);

  buffer[0] = ',';  // separator
  buffer++;

  buffer[0] = '\0';
}

void toString(const RTTTL_CONTROL_SECTION & ctrl_section, char * buffer) {
  // duration
  buffer[0] = 'd';
  buffer[1] = '=';
  buffer += 2;

  buffer = itoa(gNoteDurations[ctrl_section.durationIdx], buffer);

  buffer[0] = ',';
  buffer++;

  // octave
  buffer[0] = 'o';
  buffer[1] = '=';
  buffer += 2;

  buffer = itoa(gNoteOctaves[ctrl_section.octaveIdx], buffer);

  buffer[0] = ',';
  buffer++;

  // bmp
  buffer[0] = 'b';
  buffer[1] = '=';
  buffer += 2;

  buffer = itoa(ctrl_section.bpm, buffer);

  buffer[0] = ':';  // separator
  buffer++;

  buffer[0] = '\0';
}

}; //anyrtttl namespace

#endif //BINRTTTL_H
