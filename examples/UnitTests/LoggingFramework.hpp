#pragma once

#include "StringFormatter.hpp"

// Type for the platform-specific function that output log messages.
typedef void (*log_output_fn_t)(const char* message);

// Global function pointer used to forward message to the platform-specific output function.
static log_output_fn_t gLogOutputFunc = NULL;

static inline const char* logBaseName(const char* file_path) {
  if (!file_path) return "";

  // Extract file name
  // Find last separator
  const char* file_name = strrchr(file_path, '/');  // unix
  if (!file_name) {
    file_name = strrchr(file_path, '\\');  // windows
  }

  // If last separator is found, return "last separator" + 1.
  // Otherwise, return original file_path
  file_name = file_name ? file_name + 1 : file_path;

  return file_name;
}

void logMessageInternal(const char* message) {
#if defined(ARDUINO) || defined(ESP32)
  Serial.print(message);
#else
  std::printf(message);
#endif
}

void logTimestamp(char* buffer, size_t buffer_size) {
#if defined(ARDUINO) || defined(ESP32)
  unsigned long now = millis();
  snprintf(buffer, buffer_size, "%06u", now);
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  unsigned long now = (unsigned long)(tv.tv_sec * 1000UL + tv.tv_usec / 1000UL);
  snprintf(buffer, buffer_size, "%10u", now);
#endif
}

static inline void logInternal(const char* file, int line, const char* format, ...) {
  if (gLogOutputFunc == NULL) {
    gLogOutputFunc = &logMessageInternal;
  }

  // Format the message body
  std::string message;
  va_list args;
  va_start(args, format);
  // Determine required size (C99: vsnprintf with NULL and 0 returns required length)
  int len = stringPrintf(message, format, args);
  va_end(args);

  // Build timestamp string
  // Choose a reasonable size for timestamp string
  const size_t TIMESTAMP_BUFFER_SIZE = 64;
  char timestamp[TIMESTAMP_BUFFER_SIZE];
  logTimestamp(timestamp, sizeof(timestamp));

  // Get base filename
  const char* filename = logBaseName(file);

  // Format a final message in format "<timestamp> (<filename>, line <linenumber>): "
  std::string final_message;
  stringPrintf(final_message, "%s (%s:%d): %s", timestamp, filename, line, message.c_str());

  // Output the final message
  gLogOutputFunc(final_message.c_str());
}

// Public macro: printf-style, supports zero or more args
// Use the GNU/MSVC extension '##__VA_ARGS__' to allow zero args cleanly.
#if defined(_MSC_VER)
  // MSVC supports __VA_ARGS__ but not the '##' trick; use a wrapper
  #define LOG(fmt, ...) logInternal(__FILE__, __LINE__, fmt, __VA_ARGS__)
#else
  #define LOG(fmt, ...) logInternal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#endif
