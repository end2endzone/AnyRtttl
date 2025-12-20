#define RTTTL_PARSER_RELAXED

#include <anyrtttl.h>
#include <pitches.h>
#include <stdint.h>
#include <sstream>
#include "TestingFramework.hpp"
#include "Logging.hpp"

// Define the BUZZER_PIN for current board
#define BUZZER_PIN 0 // Using a fake pin number

//project's constants & variables
const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
unsigned long gFakeMillisTimer = 0; // a fake milliseconds timer. Requied to speed up notes
bool gInsertTimestampsInLogs = true;
bool gOptimizeFameMillisTimerInToneCalls = true;
unsigned long gFakeMillisTimerJumpSize = 0;
std::string gMelodyOutput; // a global buffer to hold the rtttl commands output when calling functions such as tone(), noTone(), etc.
static const unsigned long INVALID_TIMER_TIMESTAMP = (unsigned long)-1;

void resetFakeTimer() {
  gFakeMillisTimer = 0;
}

void resetMelodyBuffer() {
  gMelodyOutput.clear();
}

const char * getMillisTimestamp() {
  static char gTimestampBuffer[16];
  sprintf(gTimestampBuffer, "%06d: ", gFakeMillisTimer);
  return gTimestampBuffer;
}

// Count how many times the given token appears in the string str.
size_t countTokens(const char * token, const char * str) {
  if (!str || !token || *token == '\0' || *str == '\0') {
    return 0;  // invalid input or empty token/str
  }

  size_t count = 0;
  const char *pos = str;

  pos = strstr(pos, token);
  while (pos != NULL) {
    count++;
    pos += strlen(token);  // move past the position of the token

    // search again
    pos = strstr(pos, token);
  }

  return count;
}

unsigned long getToneTimestamp(const char * token, const std::string & str) {
    std::istringstream iss(str);
    std::string line;

    // Read str line by line
    while (std::getline(iss, line)) {
        // Check if the line contains the token
        if (line.find(token) != std::string::npos) {
            // Extract the timestamp (first 6 characters before ':')
            std::string timestampStr = line.substr(0, 6);

            // Convert to integer
            unsigned long timestamp = (unsigned long)(std::stoi(timestampStr));
            return timestamp;
        }
    }

    // Token is not found
    return INVALID_TIMER_TIMESTAMP;
}

//*******************************************************************************************************************
//  The following replacement functions prints the function call & parameters to a custom buffer string.
//*******************************************************************************************************************
void logTone(uint8_t pin, unsigned int frequency, unsigned long duration) {
  if (gInsertTimestampsInLogs)
    gMelodyOutput += getMillisTimestamp();
  
  log(gMelodyOutput, "tone(pin,%d,%d);\n", frequency, duration);

  // optimize fake timer
  static const unsigned long JUMP_MIN_SIZE = 10;
  if (gOptimizeFameMillisTimerInToneCalls && duration > JUMP_MIN_SIZE) {
    gFakeMillisTimerJumpSize = (duration - JUMP_MIN_SIZE);
  }
}

void logNoTone(uint8_t pin) {
  if (gInsertTimestampsInLogs)
    log(gMelodyOutput, "%s", getMillisTimestamp());
    
  log(gMelodyOutput, "noTone(pin);\n");
}

void logDelay(unsigned long duration) {
  if (gInsertTimestampsInLogs)
    log(gMelodyOutput, "%s", getMillisTimestamp());
  
  log(gMelodyOutput, "delay(%d);\n", duration);
}

unsigned long fakeMillis(void) {
  unsigned long output = gFakeMillisTimer;

  // does fame timer must jump ?
  if (gFakeMillisTimerJumpSize) {
    unsigned long before = gFakeMillisTimer;
    gFakeMillisTimer += gFakeMillisTimerJumpSize;
    unsigned long after = gFakeMillisTimer;

    gFakeMillisTimerJumpSize = 0;
  }
  else {
    // increase fake timer for the next call
    gFakeMillisTimer += 1;
  }

  return output;
}

//*******************************************************************************************************************
//  Unit test functions
//*******************************************************************************************************************

TestResult testThisTestAlwaysPass() {
  return TestResult::Pass;
}

TestResult testThisTestAlwaysFailsStringContains() {
  ASSERT_STRING_CONTAINS("foo", "this is a complete haystack string");
  return TestResult::Pass;
}

TestResult testThisTestAlwaysFailsIntegerEquals() {
  ASSERT_EQ(-1, 42);
  return TestResult::Pass;
}

void SerialPrintMelodyOutput(int line) {
  Serial.print("Line ");
  Serial.print(line);
  Serial.print(": ");
  Serial.println(gMelodyOutput.c_str());
}

TestResult testTetrisRamBlocking() {
  resetFakeTimer();
  resetMelodyBuffer();

  anyrtttl::blocking::play(BUZZER_PIN, tetris);

  // get the melody calls with timestamps
  std::string expected = ""
    "noTone(0);\n"
    "tone(0,1319,375);\n"
    "noTone(0);\n"
    "tone(0,988,187);\n"
    "noTone(0);\n"
    "tone(0,1047,187);\n"
    "noTone(0);\n"
    "tone(0,1175,187);\n"
    "noTone(0);\n"
    "tone(0,1319,93);\n"
    "noTone(0);\n"
    "tone(0,1175,93);\n"
    "noTone(0);\n"
    "tone(0,1047,187);\n"
    "noTone(0);\n"
    "tone(0,988,187);\n"
    "noTone(0);\n"
    "tone(0,880,375);\n"
    "noTone(0);\n"
    "tone(0,880,187);\n"
    "noTone(0);\n"
    "tone(0,1047,187);\n"
    "noTone(0);\n"
    "tone(0,1319,375);\n"
    "noTone(0);\n"
    "tone(0,1175,187);\n"
    "noTone(0);\n"
    "tone(0,1047,187);\n"
    "noTone(0);\n"
    "tone(0,988,375);\n"
    "noTone(0);\n"
    "tone(0,988,187);\n"
    "noTone(0);\n"
    "tone(0,1047,187);\n"
    "noTone(0);\n"
    "tone(0,1175,375);\n"
    "noTone(0);\n"
    "tone(0,1319,375);\n"
    "noTone(0);\n"
    "tone(0,1047,375);\n"
    "noTone(0);\n"
    "tone(0,880,375);\n"
    "noTone(0);\n"
    "tone(0,880,750);\n"
    "noTone(0);\n"
    "noTone(0);\n"
    "tone(0,1175,375);\n"
    "noTone(0);\n"
    "tone(0,1397,187);\n"
    "noTone(0);\n"
    "tone(0,1760,375);\n"
    "noTone(0);\n"
    "tone(0,1568,187);\n"
    "noTone(0);\n"
    "tone(0,1397,187);\n"
    "noTone(0);\n"
    "tone(0,1319,375);\n"
    "noTone(0);\n"
    "tone(0,1319,187);\n"
    "noTone(0);\n"
    "tone(0,1047,187);\n"
    "noTone(0);\n"
    "tone(0,1319,375);\n"
    "noTone(0);\n"
    "tone(0,1175,187);\n"
    "noTone(0);\n"
    "tone(0,1047,187);\n"
    "noTone(0);\n"
    "tone(0,988,375);\n"
    "noTone(0);\n"
    "tone(0,988,187);\n"
    "noTone(0);\n"
    "tone(0,1047,187);\n"
    "noTone(0);\n"
    "tone(0,1175,375);\n"
    "noTone(0);\n"
    "tone(0,1319,375);\n"
    "noTone(0);\n"
    "tone(0,1047,375);\n"
    "noTone(0);\n"
    "tone(0,880,375);\n"
    "noTone(0);\n"
    "tone(0,880,375);\n"
    "noTone(0);\n";
  std::string actual = gMelodyOutput;

  ASSERT_STRING_EQ(expected.c_str(), actual.c_str());

  return TestResult::Pass;
}

TestResult testProgramMemoryBlocking() {
  resetFakeTimer();
  resetMelodyBuffer();

  const char melody[] PROGMEM = "Simpsons:d=4,o=5,b=160:32p,c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g";
  anyrtttl::blocking::playProgMem(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gMelodyOutput;
  std::string expected = ""
    "noTone(0);\n"
    "tone(0,1319,375);\n"
    "noTone(0);\n";

  ASSERT_STRING_EQ(expected.c_str(), actual.c_str());

  return TestResult::Pass;
}

TestResult TestSmalestRtttlPgmBlocking() {
  resetFakeTimer();
  resetMelodyBuffer();

  const char melody[] PROGMEM = ":d=4,o=5,b=100:c";
  anyrtttl::blocking::playProgMem(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gMelodyOutput;
  std::string expected = ""
    "noTone(0);\n"
    "tone(0,1319,375);\n"
    "noTone(0);\n";

  ASSERT_STRING_EQ(expected.c_str(), actual.c_str());

  return TestResult::Pass;
}

TestResult testSingleNotes() {
  static const uint16_t expected_frequencies[] = {
    1760, // a
    1976, // b
    1047, // c
    1175, // d
    1319, // e
    1397, // f
    1568, // g
  };
  static const int expected_frequencies_count = sizeof(expected_frequencies)/sizeof(expected_frequencies[0]);

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};
  for(int i = 0; i < expected_frequencies_count; i++) {
    resetFakeTimer();
    resetMelodyBuffer();

    // build the melody
    char note_character = 'a' + i;
    sprintf(melody, ":d=4,o=6,b=63:%c", note_character);

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gMelodyOutput;

    // build expected string
    uint16_t expected_frequency = expected_frequencies[i];
    sprintf(expected_string, "tone(pin,%d,952);", expected_frequency);

    // assert
    ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  }

  return TestResult::Pass;
}

int myPowerFunction(int base, int exponent) {
  int result = 1;
  for (int i = 0; i < exponent; i++) {
    result *= base;
  }
  return result;
}

TestResult testDurations() {
  static const uint16_t expected_durations[] = {
    3808, //  1a  2^0
    1904, //  2a  2^1
     952, //  4a  2^2
     476, //  8a  2^3
     238, // 16a  2^4
     119, // 32a  2^5
  };
  static const int expected_durations_count = sizeof(expected_durations)/sizeof(expected_durations[0]);

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};
  for(int i = 0; i < expected_durations_count; i++) {
    
    // ------------------------
    // Test in Control Section
    // ------------------------
    {
      resetFakeTimer();
      resetMelodyBuffer();

      // build the melody
      int duration_value = myPowerFunction(2, i);
      sprintf(melody, ":d=%d,o=6,b=63:a", duration_value);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gMelodyOutput;

      // build expected string
      uint16_t expected_duration = expected_durations[i];
      sprintf(expected_string, "tone(pin,1760,%d);", expected_duration);

      // assert
      ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
    }

    // ------------------------
    // Test in individual note
    // ------------------------
    {
      resetFakeTimer();
      resetMelodyBuffer();

      // build the melody
      int duration_value = myPowerFunction(2, i);
      sprintf(melody, ":d=4,o=6,b=63:%da", duration_value);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gMelodyOutput;

      // build expected string
      uint16_t expected_duration = expected_durations[i];
      sprintf(expected_string, "tone(pin,1760,%d);", expected_duration);

      // assert
      ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
    }
  }

  return TestResult::Pass;
}

TestResult testDurationsInvalid() {
  
  #if defined(RTTTL_PARSER_STRICT)
  // Not supported in STRICT parsing mode.
  return TestResult::Skip;
  #endif // RTTTL_PARSER_STRICT

  // Assert the parsing will not fail with invalid durations.
  // The parsing algorithm should ignore these out of scope durations values
  // and keep the default duration 4, always resulting in 952 ms note.

  static const uint16_t invalid_durations[] = {0, 64};
  static const int invalid_durations_count = sizeof(invalid_durations)/sizeof(invalid_durations[0]);

  static const uint16_t expected_durations[] = {
    952, // 0a
    952, // 64a
  };
  static const int expected_durations_count = sizeof(expected_durations)/sizeof(expected_durations[0]);

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};
  for(int i = 0; i < invalid_durations_count; i++) {
    
    // ------------------------
    // Test in Control Section
    // ------------------------
    {
      resetFakeTimer();
      resetMelodyBuffer();

      // build the melody
      int duration_value = invalid_durations[i];
      sprintf(melody, ":d=%d,o=6,b=63:a", duration_value);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gMelodyOutput;

      // build expected string
      uint16_t expected_duration = expected_durations[i];
      sprintf(expected_string, "tone(pin,1760,%d);", expected_duration);

      // assert
      ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
    }

    // ------------------------
    // Test in individual note
    // ------------------------
    {
      resetFakeTimer();
      resetMelodyBuffer();

      // build the melody
      int duration_value = invalid_durations[i];
      sprintf(melody, ":d=4,o=6,b=63:%da", duration_value);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gMelodyOutput;

      // build expected string
      uint16_t expected_duration = expected_durations[i];
      sprintf(expected_string, "tone(pin,1760,%d);", expected_duration);

      // assert
      ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
    }
  }

  return TestResult::Pass;
}

TestResult testOctaves() {
  static const uint16_t expected_frequencies[] = {
     440, // a4
     880, // a5
    1760, // a6
    3520, // a7
  };
  static const int expected_frequencies_count = sizeof(expected_frequencies)/sizeof(expected_frequencies[0]);

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};
  for(int i = 0; i < expected_frequencies_count; i++) {
    
    // ------------------------
    // Test in Control Section
    // ------------------------
    {
      resetFakeTimer();
      resetMelodyBuffer();

      // build the melody
      char octave_character = '4' + i;
      sprintf(melody, ":d=4,o=%c,b=63:a", octave_character);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gMelodyOutput;

      // build expected string
      uint16_t expected_frequency = expected_frequencies[i];
      sprintf(expected_string, "tone(pin,%d,952);", expected_frequency);

      // assert
      ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
    }

    // ------------------------
    // Test in individual note
    // ------------------------
    {
      resetFakeTimer();
      resetMelodyBuffer();

      // build the melody
      char octave_character = '4' + i;
      sprintf(melody, ":d=4,o=6,b=63:a%c", octave_character);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gMelodyOutput;

      // build expected string
      uint16_t expected_frequency = expected_frequencies[i];
      sprintf(expected_string, "tone(pin,%d,952);", expected_frequency);

      // assert
      ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
    }
  }

  return TestResult::Pass;
}

TestResult testOctavesInvalid() {
  
  #if defined(RTTTL_PARSER_STRICT)
  // Not supported in STRICT parsing mode.
  return TestResult::Skip;
  #endif // RTTTL_PARSER_STRICT

  // Assert the parsing will not fail with invalid octaves.
  // The parsing algorithm should ignore these out of scope octave values
  // and keep the default octave 6, always resulting in 1760 Hz note frequency.

  static const uint16_t invalid_octaves[] = {1, 2, 3, 8, 9};
  static const int invalid_octaves_count = sizeof(invalid_octaves)/sizeof(invalid_octaves[0]);

  static const uint16_t expected_frequencies[] = {
    1760, // a1
    1760, // a2
    1760, // a3
    1760, // a8
    1760, // a9
  };
  static const int expected_frequencies_count = sizeof(expected_frequencies)/sizeof(expected_frequencies[0]);

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};
  for(int i = 0; i < invalid_octaves_count; i++) {
    
    // ------------------------
    // Test in Control Section
    // ------------------------
    {
      resetFakeTimer();
      resetMelodyBuffer();

      // build the melody
      int octave_value = invalid_octaves[i];
      sprintf(melody, ":d=4,o=%d,b=63:a", octave_value);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gMelodyOutput;

      // build expected string
      uint16_t expected_frequency = expected_frequencies[i];
      sprintf(expected_string, "tone(pin,%d,952);", expected_frequency);

      // assert
      ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
    }

    // ------------------------
    // Test in individual note
    // ------------------------
    {
      resetFakeTimer();
      resetMelodyBuffer();

      // build the melody
      int octave_value = invalid_octaves[i];
      sprintf(melody, ":d=4,o=6,b=63:a%d", octave_value);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gMelodyOutput;

      // build expected string
      uint16_t expected_frequency = expected_frequencies[i];
      sprintf(expected_string, "tone(pin,%d,952);", expected_frequency);

      // assert
      ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
    }
  }

  return TestResult::Pass;
}

TestResult testControlSectionBPM() {
  static const uint16_t official_bpms[] = {25, 28, 31, 35, 40, 45, 50, 56, 63, 70, 80, 90, 100, 112, 125, 140, 160, 180, 200, 225, 250, 285, 320, 355, 400, 450, 500, 565, 635, 715, 800, 900};
  static const int official_bpms_count = sizeof(official_bpms)/sizeof(official_bpms[0]);

  static const uint16_t expected_durations[] = {
    2400, //  25
    2142, //  28
    1935, //  31
    1714, //  35
    1500, //  40
    1333, //  45
    1200, //  50
    1071, //  56
     952, //  63
     857, //  70
     750, //  80
     666, //  90
     600, // 100
     535, // 112
     480, // 125
     428, // 140
     375, // 160
     333, // 180
     300, // 200
     266, // 225
     240, // 250
     210, // 285
     187, // 320
     169, // 355
     150, // 400
     133, // 450
     120, // 500
     106, // 565
      94, // 635
      83, // 715
      75, // 800
      66, // 900
  };
  static const int expected_durations_count = sizeof(expected_durations)/sizeof(expected_durations[0]);

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};
  for(int i = 0; i < official_bpms_count; i++) {
    resetFakeTimer();
    resetMelodyBuffer();

    // build the melody
    uint16_t official_bpm = official_bpms[i];
    sprintf(melody, ":d=4,o=6,b=%d:a", official_bpm);

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gMelodyOutput;

    // build expected string
    uint16_t expected_duration = expected_durations[i];
    sprintf(expected_string, "tone(pin,1760,%d);", expected_duration);

    // assert
    ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  }

  return TestResult::Pass;
}

TestResult testControlSectionBPMUnofficial() {
  static const uint16_t unofficial_bpms[] = {10, 60, 300, 700, 1000};
  static const int unofficial_bpms_count = sizeof(unofficial_bpms)/sizeof(unofficial_bpms[0]);

  static const uint16_t expected_durations[] = {
    6000, //   10
    1000, //   60
     200, //  300
      85, //  700
      60, // 1000
  };
  static const int expected_durations_count = sizeof(expected_durations)/sizeof(expected_durations[0]);

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};
  for(int i = 0; i < unofficial_bpms_count; i++) {
    resetFakeTimer();
    resetMelodyBuffer();

    // build the melody
    uint16_t unofficial_bpm = unofficial_bpms[i];
    sprintf(melody, ":d=4,o=6,b=%d:a", unofficial_bpm);

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gMelodyOutput;

    // build expected string
    uint16_t expected_duration = expected_durations[i];
    sprintf(expected_string, "tone(pin,1760,%d);", expected_duration);

    // assert
    ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  }

  return TestResult::Pass;
}

TestResult testShortestMelody() {

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};

  resetFakeTimer();
  resetMelodyBuffer();

  // build the melody
  sprintf(melody, "::a");

  // play
  anyrtttl::blocking::play(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gMelodyOutput;

  // build expected string
  sprintf(expected_string, "tone(pin,1760,952);");

  // assert
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());

  return TestResult::Pass;
}

TestResult testComplexNotes() {

  static const char * expected_notes[] = {
    "tone(pin,1865,3808);", // 1a#
    "tone(pin,3951,1904);", // 2b7
    "tone(pin,1109,476);",  // 8c#6
    "tone(pin,1245,357);",  // 16d#.
    "tone(pin,2960,357);",  // 16f#.7
  };
  static const int expected_notes_count = sizeof(expected_notes)/sizeof(expected_notes[0]);

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};

  resetFakeTimer();
  resetMelodyBuffer();

  // build the melody
  sprintf(melody, ":d=4,o=6,b=63:1a#,2b7,8c#6,16d#.,16f#.7");

  // play
  anyrtttl::blocking::play(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gMelodyOutput;

  // assert all notes are found
  for(int i = 0; i < expected_notes_count; i++) {
    // build expected string
    const char * expected_note = expected_notes[i];
    sprintf(expected_string, "tone(pin,1760,%d);", expected_note);

    // assert this note is found in the output
    ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  }

  // Assert only all tones are found
  size_t count = countTokens("tone(", actual.c_str());
  ASSERT_EQ((size_t)expected_notes_count, count);

  return TestResult::Pass;
}

TestResult testControlSectionMissingControls() {

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};

  // :d=16,o=5,b=160:a

  // ------------------------
  // Test missing duration
  // ------------------------
  {
    resetFakeTimer();
    resetMelodyBuffer();

    // build the melody
    sprintf(melody, ":o=5,b=160:a");

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gMelodyOutput;

    // build expected string
    sprintf(expected_string, "tone(pin,880,375);");

    // assert
    ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  }

  // ------------------------
  // Test missing octave
  // ------------------------
  {
    resetFakeTimer();
    resetMelodyBuffer();

    // build the melody
    sprintf(melody, ":d=16,b=160:a");

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gMelodyOutput;

    // build expected string
    sprintf(expected_string, "tone(pin,1760,93);");

    // assert
    ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  }

  // ------------------------
  // Test missing octave
  // ------------------------
  {
    resetFakeTimer();
    resetMelodyBuffer();

    // build the melody
    sprintf(melody, ":d=16,o=5:a");

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gMelodyOutput;

    // build expected string
    sprintf(expected_string, "tone(pin,880,238);");

    // assert
    ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  }

  return TestResult::Pass;
}

TestResult testDottedNoteNokia() {

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};

  resetFakeTimer();
  resetMelodyBuffer();

  // build the melody
  sprintf(melody, ":d=4,o=6,b=63:32a.");

  // play
  anyrtttl::blocking::play(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gMelodyOutput;

  // assert note 32a. is found
  sprintf(expected_string, "tone(pin,1760,178);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());

  return TestResult::Pass;
}

TestResult testDottedNoteRelaxed() {
  
  // Testing an alternative format of dotted notes
  // Official Nokia's format is [duration][note][.][octave].
  // This alternative format is [duration][note][octave][.].

  #if defined(RTTTL_PARSER_STRICT)
  // Not supported in STRICT parsing mode.
  return TestResult::Skip;
  #endif // RTTTL_PARSER_STRICT

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};

  resetFakeTimer();
  resetMelodyBuffer();

  // build the melody
  sprintf(melody, ":d=4,o=6,b=63:32.a");

  // play
  anyrtttl::blocking::play(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gMelodyOutput;

  // assert note 32a. is found
  sprintf(expected_string, "tone(pin,1760,178);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  
  return TestResult::Pass;
}

TestResult testPauseNotes() {
  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};

  resetFakeTimer();
  resetMelodyBuffer();

  // build the melody
  sprintf(melody, ":d=8,o=6,b=45:,a.,32p,2b.");

  // play
  anyrtttl::blocking::play(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gMelodyOutput;
  testTracesAppend("melody=`%s`\n", actual.c_str());

  // assert note a. is found
  sprintf(expected_string, "tone(pin,1760,999);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  uint16_t note1Timestamp = getToneTimestamp(expected_string, actual.c_str());

  // assert note 2b. is found
  sprintf(expected_string, "tone(pin,1976,3999);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  uint16_t note2Timestamp = getToneTimestamp(expected_string, actual.c_str());

  // Assert only 2 notes
  size_t count = countTokens("tone(", actual.c_str());
  ASSERT_EQ(2, count);

  // Compute timestamp difference and add traces
  uint16_t actualDiff = (note2Timestamp - note1Timestamp);
  testTracesAppend("note1Timestamp=%d\n", note1Timestamp);
  testTracesAppend("note2Timestamp=%d\n", note2Timestamp);
  testTracesAppend("actualDiff=%d\n", actualDiff);

  // Assert notes are delayed by a a. duration + 32p duration
  static const uint16_t expected_diff = 1000 + 166;
  static const uint16_t epsilon = 25;
  ASSERT_NEAR(expected_diff, actualDiff, epsilon);

  return TestResult::Pass;
}

TestResult testInvalidNoteAreIgnored() {
  
  #if defined(RTTTL_PARSER_STRICT)
  // Not supported in STRICT parsing mode.
  return TestResult::Skip;
  #endif // RTTTL_PARSER_STRICT

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};

  resetFakeTimer();
  resetMelodyBuffer();

  // build the melody
  sprintf(melody, ":d=4,o=6,b=63:2a,z,16b");

  // play
  anyrtttl::blocking::play(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gMelodyOutput;

  // assert note 2a is found
  sprintf(expected_string, "tone(pin,1760,1904);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());

  // assert note 16b is found
  sprintf(expected_string, "tone(pin,1976,238);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());

  // Assert only 2 notes
  size_t count = countTokens("tone(", actual.c_str());
  ASSERT_EQ(2, count);

  return TestResult::Pass;
}

TestResult testMelodyWithSpaces() {
  
  #if defined(RTTTL_PARSER_STRICT)
  // Not supported in STRICT parsing mode.
  return TestResult::Skip;
  #endif // RTTTL_PARSER_STRICT

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};

  resetFakeTimer();
  resetMelodyBuffer();

  // build the melody
  sprintf(melody, ":d=4,o=6,b=63:2a , 4b,8 c, 16 d ");

  // play
  anyrtttl::blocking::play(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gMelodyOutput;

  // assert note 2a is found
  sprintf(expected_string, "tone(pin,1760,1904);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());

  // assert note 4b is found
  sprintf(expected_string, "tone(pin,1976,952);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());

  // assert note 8c is found
  sprintf(expected_string, "tone(pin,1047,952);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());

  // assert note 16d is found
  sprintf(expected_string, "tone(pin,1175,952);");
  ASSERT_STRING_CONTAINS(expected_string, actual.c_str());

  return TestResult::Pass;
}

TestResult testControlSectionAnyOrder() {

  #if defined(RTTTL_PARSER_STRICT)
  // Not supported in STRICT parsing mode.
  return TestResult::Skip;
  #endif // RTTTL_PARSER_STRICT

  typedef struct permutation_t {
    int a;
    int b;
    int c;
  } permutation_t;
  static const permutation_t permutations[] = {
    {0, 1, 2},
    {0, 2, 1},
    {1, 0, 2},
    {1, 2, 0},
    {2, 0, 1},
    {2, 1, 0},
  };
  static const char * sections[] {
    "d=32",
    "o=7",
    "b=100",
  };
  static const int permutations_count = sizeof(permutations)/sizeof(permutations[0]);

  static const size_t MELODY_BUFFER_SIZE = 256;
  char melody[MELODY_BUFFER_SIZE] = {0};
  char expected_string[MELODY_BUFFER_SIZE] = {0};
  for(int i = 0; i < permutations_count; i++) {
    resetFakeTimer();
    resetMelodyBuffer();

    // build the melody
    permutation_t permutation = permutations[i];
    sprintf(melody, ":%s,%s,%s:a", 
      sections[permutation.a],
      sections[permutation.b],
      sections[permutation.c]);

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gMelodyOutput;

    // build expected string
    sprintf(expected_string, "tone(pin,3520,75);");

    // assert
    ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  }

  return TestResult::Pass;
}

void setup() {
  // Do not initialize the BUZZER_PIN pin.
  // because BUZZER_PIN is a fake pin number.
  // pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();

  //Use custom functions
  anyrtttl::setToneFunction(&logTone);
  anyrtttl::setNoToneFunction(&logNoTone);
  anyrtttl::setDelayFunction(&logDelay);
  anyrtttl::setMillisFunction(&fakeMillis);

  //TEST(testThisTestAlwaysPass);
  //TEST(testThisTestAlwaysFailsStringContains);
  //TEST(testThisTestAlwaysFailsIntegerEquals);

  TEST(testSingleNotes);
  TEST(testOctaves);
  TEST(testOctavesInvalid);
  TEST(testDurations);
  TEST(testDurationsInvalid);
  TEST(testControlSectionBPM);
  TEST(testControlSectionBPMUnofficial);
  TEST(testControlSectionMissingControls);
  TEST(testDottedNoteNokia);
  TEST(testDottedNoteRelaxed);
  TEST(testPauseNotes);
  TEST(testShortestMelody);
  TEST(testComplexNotes);
  TEST(testInvalidNoteAreIgnored);
  TEST(testMelodyWithSpaces);
  TEST(testControlSectionAnyOrder);

  //TEST(testTetrisRamBlocking);
  //TEST(testProgramMemoryBlocking);
}

void loop() {
}
