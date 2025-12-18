#include <anyrtttl.h>
#include <pitches.h>
#include "TestingFramework.hpp"
#include "Logging.hpp"

// Define the BUZZER_PIN for current board
#define BUZZER_PIN 0 // Using a fake pin number

//project's constants & variables
const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
unsigned long gFakeMillisTimer = 0; // a fake milliseconds timer. Requied to speed up notes
//const size_t TEMP_BUFFER_SIZE = 256;
//char gTempBuffer[TEMP_BUFFER_SIZE];
bool gInsertTimestampsInLogs = true;
bool gOptimizeFameMillisTimerInToneCalls = true;
unsigned long gFakeMillisTimerJumpSize = 0;

void resetFakeTimer() {
  gFakeMillisTimer = 0;
}

void resetMelodyBuffer() {
  gLogBuffer.clear();
}

const char * getMillisTimestamp() {
  static char gTimestampBuffer[16];
  sprintf(gTimestampBuffer, "%06d: ", gFakeMillisTimer);
  return gTimestampBuffer;
}

//*******************************************************************************************************************
//  The following replacement functions prints the function call & parameters to a custom buffer string.
//*******************************************************************************************************************
void logTone(uint8_t pin, unsigned int frequency, unsigned long duration) {
  if (gInsertTimestampsInLogs)
    gLogBuffer += getMillisTimestamp();
  
  log("tone(pin,%d,%d);\n", frequency, duration);

  // optimize fake timer
  static const unsigned long JUMP_MIN_SIZE = 10;
  if (gOptimizeFameMillisTimerInToneCalls && duration > JUMP_MIN_SIZE) {
    gFakeMillisTimerJumpSize = (duration - JUMP_MIN_SIZE);
  }
}

void logNoTone(uint8_t pin) {
  if (gInsertTimestampsInLogs)
    log("%s", getMillisTimestamp());
    
  log("noTone(pin);\n");
}

void logDelay(unsigned long duration) {
  if (gInsertTimestampsInLogs)
    log("%s", getMillisTimestamp());
  
  log("delay(%d);\n", duration);
}

unsigned long fakeMillis(void) {
  unsigned long output = gFakeMillisTimer;

  // does fame timer must jump ?
  if (gFakeMillisTimerJumpSize) {
    unsigned long before = gFakeMillisTimer;
    gFakeMillisTimer += gFakeMillisTimerJumpSize;
    unsigned long after = gFakeMillisTimer;

    gFakeMillisTimerJumpSize = 0;

    log("gFakeMillisTimer jump from %d to %d\n", before, after);
  }
  else {
    // increase fake timer for the next call
    gFakeMillisTimer += 1;
  }

  log("gFakeMillisTimer is now %d\n", gFakeMillisTimer);

  return output;
}

TestResult testTetrisRamBlocking() {
  resetFakeTimer();
  resetMelodyBuffer();

  anyrtttl::blocking::play(BUZZER_PIN, tetris);

  // get the melody calls with timestamps
  std::string actual = gLogBuffer;
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

  ASSERT_STRING_EQ(expected.c_str(), actual.c_str());

  return TestResult::Pass;
}

TestResult testProgramMemoryBlocking() {
  resetFakeTimer();
  resetMelodyBuffer();

  const char melody[] PROGMEM = "Simpsons:d=4,o=5,b=160:32p,c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g";
  anyrtttl::blocking::playProgMem(BUZZER_PIN, melody);

  // get the melody calls with timestamps
  std::string actual = gLogBuffer;
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
  std::string actual = gLogBuffer;
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
    resetLog();

    // build the melody
    char note_character = 'a' + i;
    sprintf(melody, ":d=4,o=6,b=63:%c", note_character);

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gLogBuffer;

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
      resetLog();

      // build the melody
      int duration_value = myPowerFunction(2, i);
      sprintf(melody, ":d=%d,o=6,b=63:a", duration_value);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gLogBuffer;

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
      resetLog();

      // build the melody
      int duration_value = myPowerFunction(2, i);
      sprintf(melody, ":d=4,o=6,b=63:%da", duration_value);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gLogBuffer;

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
      resetLog();

      // build the melody
      char octave_character = '4' + i;
      sprintf(melody, ":d=4,o=%c,b=63:a", octave_character);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gLogBuffer;

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
      resetLog();

      // build the melody
      char octave_character = '4' + i;
      sprintf(melody, ":d=4,o=6,b=63:a%c", octave_character);

      // play
      anyrtttl::blocking::play(BUZZER_PIN, melody);

      // get the melody calls with timestamps
      std::string actual = gLogBuffer;

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
    resetLog();

    // build the melody
    uint16_t official_bpm = official_bpms[i];
    sprintf(melody, ":d=4,o=6,b=%d:a", official_bpm);

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gLogBuffer;

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
    resetLog();

    // build the melody
    uint16_t unofficial_bpm = unofficial_bpms[i];
    sprintf(melody, ":d=4,o=6,b=%d:a", unofficial_bpm);

    // play
    anyrtttl::blocking::play(BUZZER_PIN, melody);

    // get the melody calls with timestamps
    std::string actual = gLogBuffer;

    // build expected string
    uint16_t expected_duration = expected_durations[i];
    sprintf(expected_string, "tone(pin,1760,%d);", expected_duration);

    // assert
    ASSERT_STRING_CONTAINS(expected_string, actual.c_str());
  }

  return TestResult::Pass;
}

void setup() {
  // Do not initialize the BUZZER_PIN pin.
  // because BUZZER_PIN is a fake pin number
  // pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();

  //Use custom functions
  anyrtttl::setToneFunction(&logTone);
  anyrtttl::setNoToneFunction(&logNoTone);
  anyrtttl::setDelayFunction(&logDelay);
  anyrtttl::setMillisFunction(&fakeMillis);

  TEST(testSingleNotes);
  TEST(testOctaves);
  TEST(testDurations);
  TEST(testControlSectionBPM);
  TEST(testControlSectionBPMUnofficial);

  //TEST(testTetrisRamBlocking);
  //TEST(testProgramMemoryBlocking);
}

void loop() {
}
