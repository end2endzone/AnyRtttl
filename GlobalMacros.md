# Introduction

In header‑only Arduino libraries, macros defined at the beginning of a sketch are automatically visible to all compiled code. This works because the library consists entirely of header files, which are included directly into the sketch during compilation.

However, many Arduino libraries are distributed with their own separate C++ source files (\*.cpp). In these cases, a macro that is only defined at the start of your sketch does not propagate into the library's source files. As a result, those files remain unaffected by the sketch‑level macro definition.

To influence the behavior of such libraries, you need to define a **global** C/C++ macro within the build process. A global macro sets compile‑time constants that apply across the entire project, ensuring both your sketch and the library's source files are compiled with the same configuration.

> **Important:**  
A global macro applies to **every Arduino library** compiled **across all your sketches**. This means any conditional compilation directives (such as `#ifdef` or `#if defined(...)`) within those libraries will recognize and use the macro. Consequently, it can influence features such as logging behavior, timing, memory usage, or even board compatibility.



# Purpose

The following document provides step‑by‑step instructions for creating a global macro across the most commonly used development environments, including the Arduino IDE, VS Code with the PlatformIO extension, and the Arduino CLI.

These guidelines will help ensure that your macro definitions are applied properly throughout your entire project files and its libraries.



# IDE specific instructions

This section provides instructions tailored to the following coding environments.



## Arduino IDE



### What is `platform.local.txt` ?
`platform.local.txt` is an optional configuration file used by the Arduino build system to override or extend the default build flags defined in `platform.txt` for a specific board platform. It is useful for adding custom compiler flags, enabling debugging, or optimizing builds.

> **Note**  
> `platform.local.txt` is honored by the Arduino platform (cores) build system. Other ecosystems (e.g., PlatformIO) provide their *own* mechanisms for adding global macros/flags.

Where to place `platform.local.txt` depends on your operating system and the specific *core* you are targeting (e.g., AVR for Arduino Uno, `esp8266`, or `esp32`).

Place `platform.local.txt` in the **same directory** as that core's `platform.txt`.



### Official documentation
* **Arduino IDE**: <https://www.arduino.cc/en/software>  
* **Arduino Platform Specification**: https://docs.arduino.cc/arduino-cli/platform-specification
* **Arduino IDE platform.local.txt**: https://docs.arduino.cc/arduino-cli/platform-specification/#platformlocaltxt



### Create `platform.local.txt`

Follow these instructions to create a `platform.local.txt` file for your board:
1. Open a text editor (e.g., Notepad++, VS Code).
2. Add your custom build flags. Example:
   ```
   compiler.c.extra_flags=-DENABLE_GLOBAL_DEBUG -DMY_OTHER_MACRO=1 -DFOOBAR_LIBRARY_DEBUG
   compiler.cpp.extra_flags=-DENABLE_GLOBAL_DEBUG -DMY_OTHER_MACRO=1 -DFOOBAR_LIBRARY_DEBUG
   ```
3. Save the file as `platform.local.txt`. Ensure the extension is `.txt` and the file's EOL character uses unix LF, even on Windows.
4. Place it in the correct folder for the board platform (see sections below).
5. Restart Arduino IDE after adding the file.

> **Note**  
> The Arduino IDE may need to be restarted after adding the file for the change to take effect.

The example above defines `ENABLE_GLOBAL_DEBUG`, `MY_OTHER_MACRO` and `FOOBAR_LIBRARY_DEBUG` global macros that will affect your sketch **and** all libraries built with the selected core.

To confirm you are in the right folder:

1. In Arduino IDE, Go to **File → Preferences → Settings** and check: "_Show verbose output during **compilation**_".
2. Compile a sketch. The console log will show the exact path to the `platform.txt` being used. Look for a path location in format `packages/<platform>/hardware/<platform>/<version>/`.

For example:
```
Using board 'esp32doit-devkit-v1' from platform in folder: C:\Users\JohnDoe\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.3.2
Using core 'esp32' from platform in folder: C:\Users\JohnDoe\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.3.2
```
3. The folders above should contain the `platform.txt` file.
4. Place your `platform.local.txt` beside it.



#### Windows
- **Arduino AVR Core**  
  Path: `%LOCALAPPDATA%\Arduino15\packages\arduino\hardware\avr\<version>\`  
  Example: `C:\Users\<YourUser>\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\`
- **ESP8266**  
  Path: `%LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\<version>\`  
  Example: `C:\Users\<YourUser>\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.1.2\`
- **ESP32**  
  Path: `%LOCALAPPDATA%\Arduino15\packages\esp32\hardware\esp32\<version>\`  
  Example: `C:\Users\<YourUser>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.3.2\`



#### macOS

Preferences and packages live under your user Library.
- **Arduino AVR Core**  
  Path: `~/Library/Arduino15/packages/arduino/hardware/avr/<version>/`
- **ESP8266**  
  Path: `~/Library/Arduino15/packages/esp8266/hardware/esp8266/<version>/`
- **ESP32**  
  Path: `~/Library/Arduino15/packages/esp32/hardware/esp32/<version>/`



#### Linux

Preferences and packages live under `~/.arduino15`.
- **Arduino AVR Core**  
  Path: `~/.arduino15/packages/arduino/hardware/avr/<version>/`
- **ESP8266**  
  Path: `~/.arduino15/packages/esp8266/hardware/esp8266/<version>/`
- **ESP32**  
  Path: `~/.arduino15/packages/esp32/hardware/esp32/<version>/`



### Visual Studio Code - with PlatformIO extension

Official documentation:  
- **PlatformIO Docs**: https://docs.platformio.org/en/latest/
- **PlatformIO with VS Code**: https://platformio.org/install/ide?install=vscode

To define a per-project macro in a PlatformIO project using VS Code, you need to use the build_flags option in the project's platformio.ini configuration file.

PlatformIO does not use `platform.local.txt`. Instead, define global macros and flags in your project's `platformio.ini`:

#### Create `platformio.ini`

1. Create or open the `platformio.ini` file:  
   This file is located in the root directory of your PlatformIO project.
2. Locate or create an environment section:  
   PlatformIO projects are organized into environments, denoted by `[env:name]`. You can apply the macro to a specific environment (e.g., `[env:esp32dev]`) or globally to all environments using `[env]`.
3. Add the build_flags option:  
   Within the chosen environment section, add a `build_flags` line.
4. Define your macro:  
   Use the `-D` flag followed immediately by your *macro name* and *optional value*.

#### Examples

* To define a macros for all environments:
```ini
[env]
; ... other options ...
build_flags = 
  -DENABLE_GLOBAL_DEBUG -DMY_OTHER_MACRO=1 -DFOOBAR_LIBRARY_DEBUG
```

In your source code, you can check for this macro using `#ifdef ENABLE_GLOBAL_DEBUG`.

* To define a macro with a numeric value (e.g., `BAUD` with value `115200`):
```ini
[env]
; ... other options ...
build_flags = 
  -DBAUD=115200
```

In your source code, you can use it directly, for example `Serial.begin(BAUD);`.

* To define a macro with a string value, you must wrap the value in escaped double quotes \" within single quotes to prevent shell evaluation.
```ini
[env]
; ... other options ...
build_flags = 
  '-DDEVICE_NAME="MyDevice"'
```

In your C/C++ code, this will be treated as the string `"MyDevice"`.

If you prefer to keep flags separated from `platformio.ini`, use an **extra script**:

```ini
; platformio.ini
extra_scripts = pre:extra_flags.py
```

```python
# extra_flags.py
Import("env")
# Append macros/flags globally (affects all sources and libraries in the env)
env.Append(
    CCFLAGS=["-DENABLE_GLOBAL_DEBUG"],
    CXXFLAGS=["-DENABLE_GLOBAL_DEBUG"],
)
```

> **Effect on libraries**  
In PlatformIO, macros supplied via `build_flags` or `env.Append` are passed to the compiler for **all** translation units, including libraries, mirroring the global behavior you get with Arduino's `platform.local.txt`.




### Arduino CLI

Official documentation:  
- **Arduino CLI**: https://arduino.github.io/arduino-cli/latest/

The Arduino CLI uses the same platform layout as the Arduino IDE, so `platform.local.txt` placement is identical. After creating `platform.local.txt`, you can build with CLI commands such as:

```bash
arduino-cli core install arduino:avr
arduino-cli compile -b arduino:avr:uno /path/to/your/sketch
```

To provide per-project macros *without* using `platform.local.txt`, you can also pass flags via `--build-property`:

```bash
arduino-cli compile -b esp32:esp32:esp32dev \
  --build-property compiler.c.extra_flags="-DENABLE_GLOBAL_DEBUG -DMY_OTHER_MACRO=1 -DFOOBAR_LIBRARY_DEBUG" \
  --build-property compiler.cpp.extra_flags="-DENABLE_GLOBAL_DEBUG -DMY_OTHER_MACRO=1 -DFOOBAR_LIBRARY_DEBUG" \
  /path/to/your/sketch
```

These flags are applied globally for that compile invocation and affect libraries as well.

