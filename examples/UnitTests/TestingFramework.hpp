#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdio>
#endif // ARDUINO

#include <string>
#include <stdarg.h>

#include "LoggingFramework.hpp"

std::string gTestTraces; // a global buffer to hold assertion outputs or traces while executing a test

// testPrint is a platform-aware print() implementation that
// automatically print the given c-string to the appropriate output.
void testPrint(const char* value) {
#ifdef ARDUINO
  Serial.print(value);
#else
  std::printf("%s", value);
#endif
}

// Typedef for a function pointer that injects a c-string
typedef void (*TestPrintFuncDelegate)(const char*);

// Global function to print a c-string to an output.
TestPrintFuncDelegate gTestPrintFuncPtr = &testPrint;

// testPrintv() print the given arguments using the print function pointer delegate gTestPrintFuncPtr().
int testPrintv(const char* format, ...) {
  std::string tempBuffer;

  // Print arguments to tempBuffer
  va_list args;
  va_start(args, format);
  int len = vlog(tempBuffer, format, args);
  va_end(args);

  // Delegate to print function pointer  gTestPrintFuncPtr().
  gTestPrintFuncPtr(tempBuffer.c_str());

  return len;
}

enum class TestResult {
  Unknown = 0,
  Pass,
  Fail,
  Skip,
  Timeout,
  Error,
};

typedef TestResult (*TestFunc)();

inline const char* toString(TestResult r) {
  switch (r) {
    case TestResult::Pass:    return "PASS";
    case TestResult::Fail:    return "FAIL";
    case TestResult::Skip:    return "SKIP";
    case TestResult::Timeout: return "TIMEOUT";
    case TestResult::Error:   return "ERROR";
    case TestResult::Unknown:
    default:                  return "UNKNOWN";
  }
}

inline const char* toUtf8Symbol(TestResult r) {
  switch (r) {
    case TestResult::Pass:    return "✅";
    case TestResult::Fail:    return "❌";
    case TestResult::Skip:    return "⏩";
    case TestResult::Timeout: return "⏱";
    case TestResult::Error:   return "⚠️";
    case TestResult::Unknown:
    default:                  return "❓";
  }
}

int testTracesAppend(const char *format, ...) {
  va_list args;
  va_start(args, format);
  int len = vlog(gTestTraces, format, args);
  va_end(args);

  return len;
}

void runTest(const char* testName, TestFunc func)
{
  // Reset test trace logs
  gTestTraces.clear();

  testPrintv("Running test: %s() --> ", testName);

  TestResult result = TestResult::Unknown;
  if (strncmp(testName, "DISABLED_", 9) == 0) {
    // Skip disabled tests.
    result = TestResult::Skip;
  }
  else {
    // Run test function and get result.
    result = func();
  }

  testPrintv("%s %s\n", toString(result), toUtf8Symbol(result));

  // Print test log traces if test is not PASS 
  if (result != TestResult::Pass) {
    if (!gTestTraces.empty())
      testPrintv("%s\n", gTestTraces.c_str());

    // And print again the name of the test
    testPrintv("%s  %s\n", toUtf8Symbol(result), toString(result));

    // Space failing tests by 1 line
    gTestPrintFuncPtr("\n");
  }
}

#define TEST(func) runTest(#func, func)

#define ASSERT_EQ(expected, actual) \
  do { \
    if ((expected) != (actual)) { \
      testTracesAppend("ASSERT_EQ failed: expected=%d actual=%d (file %s, line %d)", \
                        (int)(expected), (int)(actual), __FILE__, __LINE__); \
      return TestResult::Fail; \
    }\
  } while(0)

#define ASSERT_NE(expected, actual) \
  do { \
    if ((expected) == (actual)) { \
      testTracesAppend("ASSERT_NE failed: expected==actual==%d (file %s, line %d)", \
                        (int)(val1), __FILE__, __LINE__); \
      return TestResult::Fail; \
    }\
  } while (0)

#define ASSERT_NEAR(expected, actual, epsilon) \
  do { \
    if (std::abs((expected) - (actual)) > (epsilon)) { \
      testTracesAppend("ASSERT_NEAR failed: expected=%d actual=%d epsilon=%d (file %s, line %d)", \
                        (int)(expected), (int)(actual), (int)(epsilon), __FILE__, __LINE__); \
      return TestResult::Fail; \
    }\
  } while (0)

#define ASSERT_STRING_EQ(expected, actual) \
do { \
    if (strcmp((expected), (actual)) != 0) { \
        testTracesAppend("ASSERT_STREQ failed: expected=`%s` actual=`%s` (file %s, line %d)", \
                          (expected), (actual), __FILE__, __LINE__); \
      return TestResult::Fail; \
    }\
} while(0)

#define ASSERT_STRING_NE(expected, actual) \
do { \
    if (strcmp((expected), (actual)) == 0) { \
        testTracesAppend("ASSERT_STRING_NE failed: expected==actual==`%s` (file %s, line %d)", \
                          (actual), __FILE__, __LINE__); \
      return TestResult::Fail; \
    }\
} while(0)

#define ASSERT_STRING_CONTAINS(substring, actual) \
  do { \
    if (strstr((actual), (substring)) == 0) { \
      testTracesAppend("ASSERT_STRING_CONTAINS failed: `%s` not found in `%s` (file %s, line %d)", \
                        (substring), (actual), __FILE__, __LINE__); \
      return TestResult::Fail; \
    }\
  } while (0)

#define ASSERT_TRUE(cond) \
  do { \
    if (!(cond)) { \
      testTracesAppend("ASSERT_TRUE failed: condition was false (file %s, line %d)", __FILE__, __LINE__); \
      return TestResult::Fail; \
    }\
  } while (0)

#define ASSERT_FALSE(cond) \
  do { \
    if ((cond)) { \
      testTracesAppend("ASSERT_FALSE failed: condition was true (file %s, line %d)", __FILE__, __LINE__); \
      return TestResult::Fail; \
    }\
  } while (0)

#define ASSERT_FLOAT_EQ(expected, actual, epsilon) \
  do { \
    if (fabs((expected) - (actual)) > (epsilon)) { \
      testTracesAppend("ASSERT_FLOAT_EQ failed: expected=%f actual=%f epsilon=%f (file %s, line %d)", \
                        (expected), (actual), (epsilon), __FILE__, __LINE__); \
      return TestResult::Fail; \
    }\
  } while (0)
