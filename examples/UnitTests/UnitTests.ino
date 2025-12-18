#include <anyrtttl.h>
#include <pitches.h>
#include "TestingFramework.hpp"

// Define the BUZZER_PIN for current board
#define BUZZER_PIN 0 // Using a fake pin number

//project's constants & variables
const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
unsigned long gFakeMillisTimer = 0; // a fake milliseconds timer. Requied to speed up notes
std::string gLogBuffer; // a buffer for holding the melody buffer
const size_t TEMP_BUFFER_SIZE = 256;
char gTempBuffer[TEMP_BUFFER_SIZE];
bool gInsertTimestampsInLogs = true;

void SerialFormattedPrint(const char *format, ...)
{
    va_list args;

    // Compute required buffer size
    va_start(args, format);
    int size = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (size <= 0) {
        return; // formatting error
    }

    // Allocate buffer for output
    char *buffer = (char *)malloc(size + 1); // +1 for null terminator
    if (!buffer) {
        return; // allocation failed
    }

    // Second pass: actually format into buffer
    va_start(args, format);
    vsnprintf(buffer, size + 1, format, args);
    va_end(args);

    // Print once
    Serial.print(buffer);

    // Clean up
    free(buffer);
}

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
  
  sprintf(gTempBuffer, "tone(%d,%d,%d);\n", pin, frequency, duration);
  gLogBuffer += gTempBuffer;

  // optimize fake timer
  if (duration > 10) {
    gFakeMillisTimer += (duration - 10);
  }  
}

void logNoTone(uint8_t pin) {
  if (gInsertTimestampsInLogs)
    gLogBuffer += getMillisTimestamp();
    
  sprintf(gTempBuffer, "noTone(%d);\n", pin);
  gLogBuffer += gTempBuffer;
}

void logDelay(unsigned long duration) {
  if (gInsertTimestampsInLogs)
    gLogBuffer += getMillisTimestamp();
  
  sprintf(gTempBuffer, "delay(%d);\n", duration);
  gLogBuffer += gTempBuffer;
}

unsigned long fakeMillis(void) {
  unsigned long output = gFakeMillisTimer;

  // increase fake timer for the next call
  gFakeMillisTimer += 1;

  return output;
}

TestResult TestTetrisRamBlocking() {
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

  TEST(TestTetrisRamBlocking);
}

void loop() {
}
