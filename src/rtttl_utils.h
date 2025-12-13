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
typedef unsigned char duration_index_t;
typedef unsigned char note_index_t;
typedef unsigned char octave_index_t;
typedef unsigned char bpm_index_t;

typedef unsigned short duration_value_t;  // a note duration value variable. See 'gNoteDurations' array. 
typedef          char  note_value_t;      // a note letter value variable. See 'gNoteValues' array. 
typedef unsigned char  octave_value_t;    // a note octave value variable. See 'gNoteDurations' array. 
typedef unsigned short bpm_value_t;       // a melody bmp value. See 'gNoteBpms' array.

/****************************************************************************
 * Constants
 ****************************************************************************/
static constexpr duration_value_t   RTTTL_DEFAULT_DURATION_VALUE = 4;   // default duration for melodies that do not specify a melody duration in the control section.
static constexpr octave_value_t     RTTTL_DEFAULT_OCTAVE_VALUE = 6;     // default   octave for melodies that do not specify a melody   octave in the control section.
static constexpr bpm_value_t        RTTTL_DEFAULT_BPM_VALUE = 63;       // default      bpm for melodies that do not specify a melody      bpm in the control section.

static constexpr duration_index_t   INVALID_DURATION_INDEX        = (duration_index_t)-1;
static constexpr note_index_t       INVALID_NOTE_LETTER_INDEX     = (note_index_t)-1;
static constexpr octave_index_t     INVALID_OCTAVE_INDEX          = (octave_index_t)-1;
static constexpr bpm_index_t        INVALID_BPM_INDEX             = (bpm_index_t)-1;

static constexpr note_value_t gNoteValues[] = {'c','d','e','f','g','a','b','p'};
static constexpr uint16_t gNoteValuesCount = sizeof(gNoteValues)/sizeof(gNoteValues[0]);

static constexpr int gNoteOffsets[] = { 1, 3, 5, 6, 8, 10, 12, 0};
static constexpr uint16_t gNoteOffsetsCount = sizeof(gNoteOffsets)/sizeof(gNoteOffsets[0]);

static constexpr duration_value_t gNoteDurations[] = {1, 2, 4, 8, 16, 32};
static constexpr uint16_t gNoteDurationsCount = sizeof(gNoteDurations)/sizeof(gNoteDurations[0]);

static constexpr octave_value_t gNoteOctaves[] = {4, 5, 6, 7};
static constexpr uint16_t gNoteOctavesCount = sizeof(gNoteOctaves)/sizeof(gNoteOctaves[0]);

static constexpr bpm_value_t gNoteBpms[] = {25, 28, 31, 35, 40, 45, 50, 56, 63, 70, 80, 90, 100, 112, 125, 140, 160, 180, 200, 225, 250, 285, 320, 355, 400, 450, 500, 565, 635, 715, 800, 900};
static constexpr uint16_t gNoteBpmsCount = sizeof(gNoteBpms)/sizeof(gNoteBpms[0]);

inline note_value_t getNoteValueFromIndex(note_index_t iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteValuesCount)
    return gNoteValues[iIndex];
  return -1;
}

inline note_index_t findNoteIndexFromNoteValue(note_value_t n)
{
  for(note_index_t i=0; i<gNoteValuesCount; i++)
  {
    if (getNoteValueFromIndex(i) == n)
    {
      return i;
    }
  }
  return -1;
}

inline int getNoteOffsetFromNoteIndex(note_index_t iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteValuesCount)
    return gNoteOffsets[iIndex];
  return 0;
}

inline int findNoteOffsetFromNoteValue(note_value_t n)
{
  note_index_t index = findNoteIndexFromNoteValue(n);
  return getNoteOffsetFromNoteIndex(index);
}

inline duration_value_t getDurationValueFromIndex(duration_index_t iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteDurationsCount)
    return gNoteDurations[iIndex];
  return -1;
}

inline duration_index_t findDurationIndexFromValue(duration_value_t n)
{
  for(duration_index_t i=0; i<gNoteDurationsCount; i++)
  {
    if (getDurationValueFromIndex(i) == n)
    {
      return i;
    }
  }
  return -1;
}

inline octave_value_t getOctaveValueFromIndex(octave_index_t iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteOctavesCount)
    return gNoteOctaves[iIndex];
  return -1;
}

inline octave_index_t findOctaveIndexFromValue(octave_value_t n)
{
  for(octave_index_t i=0; i<gNoteOctavesCount; i++)
  {
    if (getOctaveValueFromIndex(i) == n)
    {
      return i;
    }
  }
  return -1;
}

inline bpm_value_t getBpmValueFromIndex(bpm_index_t iIndex)
{
  if (iIndex >= 0 && iIndex < gNoteBpmsCount)
    return gNoteBpms[iIndex];
  return -1;
}

inline bpm_index_t findBpmIndexFromValue(bpm_value_t n)
{
  for(bpm_index_t i=0; i<gNoteBpmsCount; i++)
  {
    if (getBpmValueFromIndex(i) == n)
    {
      return i;
    }
  }
  return INVALID_BPM_INDEX;
}

}; //anyrtttl namespace

#endif //RTTTL_UTILS_H
