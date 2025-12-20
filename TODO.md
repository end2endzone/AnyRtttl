* Unit tests:
  * Melody with spaces between notes and in middle of notes.
  * Pause/silent notes (multiple)
  * Support uppercase notes ?
* README
  * Check documentation about MODE in "feature" section.
  * Add a section in README to talk about STRICT vs RELAXED parsing mode/rules.
* Update TestFramework to use LOG() statements instead of directly printing to the Serial Port. This will make the TestingFramework.hpp file compatible with x86-64/aarch64/PC compilation.
* Make code * Resilient RTTTL parsing: spaces or unknown characters in notes do not affect or break playback.
