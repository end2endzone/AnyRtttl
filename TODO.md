* README
  * Check documentation about MODE in "feature" section.
  * Add a section in README to talk about STRICT vs RELAXED parsing mode/rules.
    * Show the following in this section:
    ```
    The specification has some notable differences from RTTTL strings found in the wild. In particular:

    * The name sometimes exceeds the 10 character limit.
    * RTTTL strings usually place the special duration '.' after the note instead of after the scale (a.k.a. octave).
    * The octave may sometimes be outside the 4-7 range.
    * RTTTL strings often contain spaces.
    * RTTTL strings are often lowercase.
    ```
* Update TestFramework to use LOG() statements instead of directly printing to the Serial Port. This will make the TestingFramework.hpp file compatible with x86-64/aarch64/PC compilation.
* Make code * Resilient RTTTL parsing: spaces or unknown characters in notes do not affect or break playback.
