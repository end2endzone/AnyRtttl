#pragma once

#include <Arduino.h>
#include <string>
#include <stdarg.h>

enum class TestResult {
  Pass = 0,
  Fail,
  Skip,
  Timeout,
  Error
};

typedef TestResult (*TestFunc)();

inline const char* ToString(TestResult r) {
  switch (r) {
    case TestResult::Pass:    return "PASS";
    case TestResult::Fail:    return "FAIL";
    case TestResult::Skip:    return "SKIP";
    case TestResult::Timeout: return "TIMEOUT";
    case TestResult::Error:   return "ERROR";
    default:                  return "UNKNOWN";
  }
}

void RunTest(const char* testName, TestFunc func)
{
  Serial.print("Running test: ");
  Serial.println(testName);

  TestResult result = func();

  Serial.print("Result: ");
  Serial.println(ToString(result));
}

#define TEST(func) RunTest(#func, func)

#define ASSERT_EQ(expected, actual) \
  do { \
    if (!((expected) == (actual))) { \
      Serial.print("ASSERT_EQ failed at line "); \
      Serial.print(__LINE__); \
      Serial.print(": expected "); Serial.print(#expected); \
      Serial.print(" == actual "); Serial.print(#actual); \
      Serial.print("\nwhere expected is:\n"); Serial.print(expected); \
      Serial.print("\nwhere   actual is:\n"); Serial.print(actual); \
      Serial.println(""); \
      return TestResult::Fail; \
    } \
  } while (0)

#define ASSERT_NE(expected, actual) \
  do { \
    if (!((expected) != (actual))) { \
      Serial.print("ASSERT_NE failed at line "); \
      Serial.print(__LINE__); \
      Serial.print(": expected "); Serial.print(#expected); \
      Serial.print(" != actual "); Serial.print(#actual); \
      Serial.print("\nwhere expected is:\n"); Serial.print(expected); \
      Serial.print("\nwhere   actual is:\n"); Serial.print(actual); \
      Serial.println(""); \
      return TestResult::Fail; \
    } \
  } while (0)

#define ASSERT_STRING_EQ(expected, actual) \
  do { \
    if (strcmp((expected), (actual)) != 0) { \
      Serial.print("ASSERT_STRING_EQ failed at line "); \
      Serial.print(__LINE__); \
      Serial.print(": expected "); Serial.print(#expected); \
      Serial.print(" == actual "); Serial.print(#actual); \
      Serial.print("  (\""); Serial.print(expected); Serial.print("\" != \""); Serial.print(actual); Serial.println("\")"); \
      return TestResult::Fail; \
    } \
  } while (0)

#define ASSERT_STRING_NE(expected, actual) \
  do { \
    if (strcmp((expected), (actual)) == 0) { \
      Serial.print("ASSERT_STRING_NE failed at line "); \
      Serial.print(__LINE__); \
      Serial.print(": expected "); Serial.print(#expected); \
      Serial.print(" != actual "); Serial.print(#actual); \
      Serial.print("  (\""); Serial.print(expected); Serial.print("\" == \""); Serial.print(actual); Serial.println("\")"); \
      return TestResult::Fail; \
    } \
  } while (0)

#define ASSERT_TRUE(actual) \
  do { \
    if (!(actual)) { \
      Serial.print("ASSERT_TRUE failed at line "); \
      Serial.print(__LINE__); \
      Serial.print(": "); Serial.print(#actual); Serial.println(" is false"); \
      return TestResult::Fail; \
    } \
  } while (0)

#define ASSERT_FALSE(actual) \
  do { \
    if (actual) { \
      Serial.print("ASSERT_FALSE failed at line "); \
      Serial.print(__LINE__); \
      Serial.print(": "); Serial.print(#actual); Serial.println(" is true"); \
      return TestResult::Fail; \
    } \
  } while (0)

#define ASSERT_FLOAT_EQ(expected, actual, tol) \
  do { \
    float _diff = fabs((expected) - (actual)); \
    if (_diff > (tol)) { \
      Serial.print("ASSERT_FLOAT_EQ failed at line "); \
      Serial.print(__LINE__); \
      Serial.print(": expected "); Serial.print(#expected); \
      Serial.print(" ~= actual "); Serial.print(#actual); \
      Serial.print("  (diff="); Serial.print(_diff); \
      Serial.print(", tol="); Serial.print(tol); Serial.println(")"); \
      return TestResult::Fail; \
    } \
  } while (0)
