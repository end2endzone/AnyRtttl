#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif // ARDUINO

#include <string>
#include <stdarg.h>
#include <cstdarg>
#include <cstdio>

std::string gLogBuffer; // a buffer for holding log calls

void resetLog() {
    gLogBuffer.clear();
}

// log() will print the given arguments to the given std::string.
int vlog(std::string & buffer, const char* format, va_list args) {
    // Copy args to measure length of output formatted string
    va_list copy;
    va_copy(copy, args);
    int len = vsnprintf(nullptr, 0, format, copy);
    va_end(copy);

    // Print formatted string into output buffer
    std::string tempBuffer(len, '\0');
    vsnprintf(tempBuffer.data(), tempBuffer.size() + 1, format, args);

    // Append go given buffer
    buffer += tempBuffer;

    return len;
}

// log() will print the given arguments to the given std::string.
int log(std::string & buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int len = vlog(buffer, format, args);
    va_end(args);

    return len;
}

// log() will print the given arguments to the global gLogBuffer string.
int log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int len = vlog(gLogBuffer, format, args);
    va_end(args);

    return len;
}
