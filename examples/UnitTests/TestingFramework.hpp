#pragma once

#include <Arduino.h>
#include <string>
#include <stdarg.h>

enum class TestResult {
  Unknown = 0,
  Pass,
  Fail,
  Skip,
  Timeout,
  Error,
};

typedef TestResult (*TestFunc)();

inline const char* ToString(TestResult r) {
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

inline const char* ToUtf8Symbol(TestResult r) {
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

void RunTest(const char* testName, TestFunc func)
{
  Serial.print("Running test: ");
  Serial.print(testName);
  Serial.print("() --> ");

  TestResult result = TestResult::Unknown;
  if (strncmp(testName, "DISABLED_", 9) == 0) {
    // Skip disabled tests.
    result = TestResult::Skip;
  }
  else {
    // Run test function and get result.
    result = func();
  }

  Serial.print(ToString(result));
  Serial.print(" ");
  Serial.println(ToUtf8Symbol(result));
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
      Serial.print("\nwhere len(expected) is: "); Serial.print(strlen(expected)); \
      Serial.print("\nwhere   len(actual) is: "); Serial.print(strlen(actual)); \
      Serial.print("\nwhere expected is:\n"); Serial.print(expected); \
      Serial.print("\nwhere   actual is:\n"); Serial.print(actual); \
      Serial.println(""); \
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
      Serial.print("\nwhere expected is:\n"); Serial.print(expected); \
      Serial.print("\nwhere   actual is:\n"); Serial.print(actual); \
      Serial.println(""); \
      return TestResult::Fail; \
    } \
  } while (0)

#define ASSERT_STRING_CONTAINS(substring, actual) \
  do { \
    if (strstr((actual), (substring)) == nullptr) { \
      Serial.print("ASSERT_STRING_CONTAINS failed at line "); \
      Serial.print(__LINE__); \
      Serial.print(": expected substring "); Serial.print(#substring); \
      Serial.print(" to be contained in string "); Serial.print(#actual); \
      Serial.print("\nwhere substring is:\n"); Serial.print(substring); \
      Serial.print("\nwhere actual is:\n"); Serial.print(actual); \
      Serial.println(""); \
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

#define ASSERT_FLOAT_EQ(expected, actual, epsilon) \
  do { \
    float _diff = fabs((expected) - (actual)); \
    if (_diff > (epsilon)) { \
      Serial.print("ASSERT_FLOAT_EQ failed at line "); \
      Serial.print(__LINE__); \
      Serial.print(": expected "); Serial.print(#expected); \
      Serial.print(" ~= actual "); Serial.print(#actual); \
      Serial.print("  (diff="); Serial.print(_diff); \
      Serial.print(", epsilon="); Serial.print(epsilon); Serial.println(")"); \
      return TestResult::Fail; \
    } \
  } while (0)
