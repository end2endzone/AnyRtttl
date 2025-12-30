#pragma once

#include "StringFormatter.hpp"

std::string gLogBuffer; // a buffer for holding log calls

void resetLog() {
    gLogBuffer.clear();
}

static inline const char * logBaseName(const char * file_path) {
    if (!file_path) return "";

    // Extract file name
    // Find last separator
    const char* file_name = strrchr(file_path, '/'); // unix
    if (!file_name) {
        file_name = strrchr(file_path, '\\'); // windows
    }

    // If last separator is found, return "last separator" + 1.
    // Otherwise, return original file_path
    file_name = file_name ? file_name + 1 : file_path;

    return file_name;
}

int log(std::string & buffer, const char * file_path, int line, const char* format, ...) {
    const char *file_name = logBaseName(file_path);

    // Compute required length for new format string
    size_t needed = snprintf(NULL, 0, "%s, %d: %s", file_name, line, format) + 1; // +1 for the NULL terminated string.

    // Allocate a new format on heap
    char* new_format = (char*)malloc(needed);
    if (!new_format) {
        return -1; // allocation failed
    }

    // Build the format string
    snprintf(new_format, needed, "%s, %d: %s", file_name, line, format);

    // Delegate to vlog (since we have va_list)
    va_list args;
    va_start(args, format);
    int len = stringPrintf(buffer, new_format, args);
    va_end(args);

    // Free heap memory
    free(new_format);

    return len;
}

int log(const char * file_path, int line, const char* format, ...) {
    const char *file_name = logBaseName(file_path);

    // Compute required length for new format string
    size_t needed = snprintf(NULL, 0, "%s, %d: %s", file_name, line, format) + 1; // +1 for the NULL terminated string.

    // Allocate a new format on heap
    char* new_format = (char*)malloc(needed);
    if (!new_format) {
        return -1; // allocation failed
    }

    // Build the format string
    snprintf(new_format, needed, "%s, %d: %s", file_name, line, format);

    // Delegate to vlog (since we have va_list)
    va_list args;
    va_start(args, format);
    int len = stringPrintf(gLogBuffer, new_format, args);
    va_end(args);

    // Free heap memory
    free(new_format);

    return len;
}
