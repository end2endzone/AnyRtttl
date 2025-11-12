// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#ifndef RTTTL_UTILS_H
#define RTTTL_UTILS_H

#include "Arduino.h"
#include "pitches.h"

namespace anyrtttl
{

/****************************************************************************
 * Custom typedefs
 ****************************************************************************/
typedef unsigned char DURATION_INDEX;
typedef unsigned char NOTE_LETTER_INDEX;
typedef unsigned char OCTAVE_INDEX;
typedef unsigned char BPM_INDEX;

typedef unsigned short RTTTL_DURATION;
typedef          char  RTTTL_NOTE_LETTER;
typedef unsigned char  RTTTL_OCTAVE_VALUE;
typedef unsigned short RTTTL_BPM;

/****************************************************************************
 * Constants
 ****************************************************************************/
static constexpr RTTTL_DURATION RTTTL_DEFAULT_DURATION = 4;   // default duration for melodies that do not specify a melody duration in the control section.
static constexpr RTTTL_OCTAVE_VALUE RTTTL_DEFAULT_OCTAVE = 6; // default   octave for melodies that do not specify a melody   octave in the control section.
static constexpr RTTTL_BPM RTTTL_DEFAULT_BPM = 63;            // default      bpm for melodies that do not specify a melody      bpm in the control section.

static constexpr DURATION_INDEX       INVALID_DURATION_INDEX        = (DURATION_INDEX)-1;
static constexpr NOTE_LETTER_INDEX    INVALID_NOTE_LETTER_INDEX     = (NOTE_LETTER_INDEX)-1;
static constexpr OCTAVE_INDEX         INVALID_OCTAVE_INDEX          = (OCTAVE_INDEX)-1;
static constexpr BPM_INDEX            INVALID_BPM_INDEX             = (BPM_INDEX)-1;

static constexpr RTTTL_NOTE_LETTER gNoteLetters[] = {'c','d','e','f','g','a','b','p'};
static constexpr uint16_t gNoteLettersCount = sizeof(gNoteLetters)/sizeof(gNoteLetters[0]);

static constexpr int gNoteOffsets[] = { 1, 3, 5, 6, 8, 10, 12, 0};
static constexpr uint16_t gNoteOffsetsCount = sizeof(gNoteOffsets)/sizeof(gNoteOffsets[0]);

static constexpr RTTTL_DURATION gNoteDurations[] = {1, 2, 4, 8, 16, 32};
static constexpr uint16_t gNoteDurationsCount = sizeof(gNoteDurations)/sizeof(gNoteDurations[0]);

static constexpr RTTTL_OCTAVE_VALUE gNoteOctaves[] = {4, 5, 6, 7};
static constexpr uint16_t gNoteOctavesCount = sizeof(gNoteOctaves)/sizeof(gNoteOctaves[0]);

static constexpr RTTTL_BPM gNoteBpms[] = {25, 28, 31, 35, 40, 45, 50, 56, 63, 70, 80, 90, 100, 112, 125, 140, 160, 180, 200, 225, 250, 285, 320, 355, 400, 450, 500, 565, 635, 715, 800, 900};
static constexpr uint16_t gNoteBpmsCount = sizeof(gNoteBpms)/sizeof(gNoteBpms[0]);

inline RTTTL_NOTE_LETTER getNoteLetterFromIndex(NOTE_LETTER_INDEX iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteLettersCount)
    return gNoteLetters[iIndex];
  return -1;
}

inline NOTE_LETTER_INDEX findNoteLetterIndex(RTTTL_NOTE_LETTER n)
{
  for(NOTE_LETTER_INDEX i=0; i<gNoteLettersCount; i++)
  {
    if (getNoteLetterFromIndex(i) == n)
    {
      return i;
    }
  }
  return -1;
}

inline int getNoteOffsetFromLetterIndex(NOTE_LETTER_INDEX iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteLettersCount)
    return gNoteOffsets[iIndex];
  return 0;
}

inline int getNoteOffsetFromLetter(RTTTL_NOTE_LETTER n)
{
  NOTE_LETTER_INDEX index = findNoteLetterIndex(n);
  return getNoteOffsetFromLetterIndex(index);
}

inline RTTTL_DURATION getNoteDurationFromIndex(DURATION_INDEX iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteDurationsCount)
    return gNoteDurations[iIndex];
  return -1;
}

inline DURATION_INDEX findNoteDurationIndex(RTTTL_DURATION n)
{
  for(DURATION_INDEX i=0; i<gNoteDurationsCount; i++)
  {
    if (getNoteDurationFromIndex(i) == n)
    {
      return i;
    }
  }
  return -1;
}

inline RTTTL_OCTAVE_VALUE getNoteOctaveFromIndex(OCTAVE_INDEX iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteOctavesCount)
    return gNoteOctaves[iIndex];
  return -1;
}

inline OCTAVE_INDEX findNoteOctaveIndex(RTTTL_OCTAVE_VALUE n)
{
  for(OCTAVE_INDEX i=0; i<gNoteOctavesCount; i++)
  {
    if (getNoteOctaveFromIndex(i) == n)
    {
      return i;
    }
  }
  return -1;
}

inline RTTTL_BPM getBpmFromIndex(BPM_INDEX iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteBpmsCount)
    return gNoteBpms[iIndex];
  return -1;
}

inline BPM_INDEX findBpmIndex(RTTTL_BPM n)
{
  for(BPM_INDEX i=0; i<gNoteBpmsCount; i++)
  {
    if (getBpmFromIndex(i) == n)
    {
      return i;
    }
  }
  return INVALID_BPM_INDEX;
}

}; //anyrtttl namespace

#endif //RTTTL_UTILS_H
