#pragma once

#if defined(ARDUINO) || defined(ESP32)
  // Arduino/ESP32 environment
  #include <Arduino.h>
#else
  // PC environment
#endif

#include <string>
#include <stdarg.h>
#include <cstdarg>
#include <cstdio>

// stringPrintf() will print the given variable argument list to the given output std::string.
int stringPrintf(std::string & buffer, const char* format, va_list args) {
  // Copy args to measure length of output formatted string
  va_list copy;
  va_copy(copy, args);
  int len = vsnprintf(nullptr, 0, format, copy);
  va_end(copy);

  // Print formatted string into temporary buffer
  std::string tempBuffer(len, '\0');
  vsnprintf(tempBuffer.data(), tempBuffer.size() + 1, format, args);

  // Move tempBuffer by appending it to the given buffer
  buffer += std::move(tempBuffer);  // tempBuffer is now empty

  return len;
}

// stringPrintf() will print the given arguments to the given output std::string.
int stringPrintf(std::string & buffer, const char* format, ...) {
  va_list args;
  va_start(args, format);
  int len = stringPrintf(buffer, format, args);
  va_end(args);

  return len;
}
