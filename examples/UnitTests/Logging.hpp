#pragma once

#include <Arduino.h>
#include <string>
#include <stdarg.h>

std::string gLogBuffer; // a buffer for holding log calls

void resetLog() {
    gLogBuffer.clear();
}

void log(const char *format, ...) {
    va_list args;

    // First pass: compute required buffer size
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

    // Store temporary buffer
    gLogBuffer += buffer;

    // Clean up
    free(buffer);
}
