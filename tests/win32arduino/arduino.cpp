//
//  win32Arduino Library - v1.1 - 05/14/2016
//  Copyright (C) 2016 Antoine Beauchamp
//  The code & updates for the library can be found on http://end2endzone.com
//
// AUTHOR/LICENSE:
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 3.0 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License (LGPL-3.0) for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// DISCLAIMER:
//  This software is furnished "as is", without technical support, and with no 
//  warranty, express or implied, as to its usefulness for any purpose.
//
// PURPOSE:
//  The win32Arduino is a win32 library that implementation of most used arduino
//  functions which allows a library developer to unit test his code outside of
//  the arduino platform.
//
//  This library allows a windows user to easily test an arduino library using
//  your testing framework of choice. For instance, the unit tests of win32Arduino
//  library are executed using the Google Test framework.
//
// USAGE:
//  The following instructions show how to easily test an arduino library using
//  the Google Test framework. It assumes that you are already familiar with the
//  test API.
//
//  1. Create an executable project and configure the main() function to launch
//     Google Test's RUN_ALL_TESTS() macro.
//  2. Create a second static library project and add all the arduino files of
//     the desired library you need to test.
//  3. The library files are expected to include arduino.h. Modify the project's
//     Additionnal Include Directories to point to the win32Arduino library.
//
//  The project should compile properly without errors or unresolved extensions
//  allowing you to properly unit test each functions.
//
// HISTORY:
// 05/13/2016 v1.0 - Initial release.
// 05/14/2016 v1.1 - Implemented both simulated & realtime clock handling. The desired strategy is user selectable.
//

#include <stdarg.h>
#include <shlobj.h>
#undef max
#undef min

#include "arduino.h"

namespace arduino_stub
{
  //millis() support
  static clock_t app_clock_init()
  {
    return ::clock();
  }
  static clock_t gClockAppStartTime = arduino_stub::app_clock_init();
  
  static double diffclock(clock_t clockEnd,clock_t clockStart)
  {
    clock_t diffticks=clockEnd-clockStart;
    double diffms=(diffticks)/(CLOCKS_PER_SEC/1000.0);
    return diffms;
  }

  //pins data
  static uint8_t pinStates[256] = {0};

  //last command support
  static std::string gLogString;
  const char * getLastCommand()
  {
    return gLogString.c_str();
  }

  //function logging support
  std::string gLogFile = "arduino.log";
  std::string gPreviousLogFile;
  void setLogFile(const char * iFilePath)
  {
    gLogFile = iFilePath;
  }
  void log(const char * iValue)
  {
    if (gLogFile == "")
    {
      exit(1);
    }

    if (gPreviousLogFile == gLogFile)
    {
      //continue logging to the same file
      FILE * f = fopen(gLogFile.c_str(), "a");
      fputs(iValue, f);
      fclose(f);
    }
    else
    {
      //that is a new log file
      FILE * f = fopen(gLogFile.c_str(), "w");
      fputs(iValue, f);
      fclose(f);
    }

    //remember last output file
    gPreviousLogFile = gLogFile;

    //remember last command
    gLogString = iValue;
  }

  //clock hanlding
  static int gClockStrategy = CLOCK_SIMULATION;
  void setClockStrategy(int iClockStrategy)
  {
    gClockStrategy = iClockStrategy;
  }

  int getClockStrategy()
  {
    return gClockStrategy;
  }

}

void tone(byte iPin, uint16_t freq, uint32_t duration)
{
  char buffer[10240];
  sprintf(buffer, "tone(%d,%d,%d);\n", iPin, freq, duration);
  arduino_stub::log(buffer);
}

void noTone(byte iPin)
{
  char buffer[10240];
  sprintf(buffer, "noTone(%d);\n", iPin);
  arduino_stub::log(buffer);
}

void SerialPrinter::begin(uint16_t baudrate)
{
}

void SerialPrinter::printString(const char * iValue)
{
  printf("%s",  iValue);
  arduino_stub::log(iValue);
}

void SerialPrinter::printlnString(const char * iValue)
{
  printf("%s\n",iValue);
  std::string tmp = iValue;
  tmp.append("\n");
  arduino_stub::log(tmp.c_str());
}

void SerialPrinter::println()
{
  printf("\n");
}

SerialPrinter Serial;

//----------------------------------------------------------------------

//https://www.arduino.cc/en/Reference/HomePage

//digital read/write
//#define HIGH 255
//#define LOW 0
const char * toDigitalPinString(uint8_t value)
{
  if (value == HIGH)
    return "HIGH";
  else if (value == LOW)
    return "LOW";
  return "EXPECTING_HIGH_OR_LOW";
}

//pin modes
//#define OUTPUT 0
//#define INPUT 1
//#define INPUT_PULLUP 2
const char * toPinModeString(uint8_t value)
{
  if (value == OUTPUT)
    return "OUTPUT";
  else if (value == INPUT)
    return "INPUT";
  else if (value == INPUT_PULLUP)
    return "INPUT_PULLUP";
  return "EXPECTING_OUTPUT_INPUT_OR_INPUT_PULLUP";
}

//shiftOut definition
//#define MSBFIRST 0
//#define LSBFIRST 1
const char * toBitOrderString(uint8_t value)
{
  if (value == MSBFIRST)
    return "MSBFIRST";
  else if (value == LSBFIRST)
    return "LSBFIRST";
  return "EXPECTING_MSBFIRST_OR_LSBFIRST";
}

void pinMode(uint8_t pin, uint8_t mode)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  const char * modeString = toPinModeString(mode);
  sprintf(buffer, "%s(%d, %s);\n", __FUNCTION__, pin, modeString);
  arduino_stub::log(buffer);
}

void digitalWrite(uint8_t pin, uint8_t value)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  const char * digitalString = toDigitalPinString(value);
  sprintf(buffer, "%s(%d, %s);\n", __FUNCTION__, pin, digitalString);
  arduino_stub::log(buffer);

  //update pin state
  arduino_stub::pinStates[pin] = value;
}

uint8_t digitalRead(uint8_t pin)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d);\n", __FUNCTION__, pin);
  arduino_stub::log(buffer);

  //update pin state
  return arduino_stub::pinStates[pin];
}

void analogWrite(uint8_t pin, uint8_t value)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d, %d);\n", __FUNCTION__, pin, (int)value);
  arduino_stub::log(buffer);

  //update pin state
  arduino_stub::pinStates[pin] = value;
}

uint16_t analogRead(uint8_t pin)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d);\n", __FUNCTION__, pin);
  arduino_stub::log(buffer);

  //update pin state
  return arduino_stub::pinStates[pin];
}

void analogReadResolution(uint8_t bits)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d);\n", __FUNCTION__, bits);
  arduino_stub::log(buffer);
}

void analogWriteResolution(uint8_t bits)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d);\n", __FUNCTION__, bits);
  arduino_stub::log(buffer);
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t data)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d, %d, %s, %d);\n", __FUNCTION__, dataPin, clockPin, toBitOrderString(bitOrder), data);
  arduino_stub::log(buffer);
}

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d, %d, %s);\n", __FUNCTION__, dataPin, clockPin, toBitOrderString(bitOrder));
  arduino_stub::log(buffer);

  return 0;
}

uint32_t pulseIn(uint8_t pin, uint8_t digitalState, uint32_t timeout)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d, %s, %d);\n", __FUNCTION__, pin, toDigitalPinString(digitalState), timeout);
  arduino_stub::log(buffer);

  return 200; //200 usec
}

uint32_t pulseIn(uint8_t pin, uint8_t digitalState)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d, %s);\n", __FUNCTION__, pin, toDigitalPinString(digitalState));
  arduino_stub::log(buffer);

  return 200; //200 usec
}

namespace realtime
{

uint32_t micros()
{
  //based on millis() implementation.

  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s();\n", "micros");
  arduino_stub::log(buffer);

  //copy millis() implementation
  //realtime
  clock_t now = ::clock();
  double diffMs = arduino_stub::diffclock(now, arduino_stub::gClockAppStartTime);
  double diffMicros = diffMs * 1000;

  static const uint32_t MAX_MICROS = (uint32_t)0xFFFFFFFF;
  while(diffMicros > (double)MAX_MICROS)
  {
    diffMicros -= (double)(MAX_MICROS);
  }
  uint32_t finalMicros = (uint32_t)diffMicros;
  return finalMicros;
}

uint32_t millis()
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s();\n", "millis");
  arduino_stub::log(buffer);

  //realtime
  clock_t now = ::clock();
  double diffMs = arduino_stub::diffclock(now, arduino_stub::gClockAppStartTime);
  uint32_t diffFinal = (uint32_t)diffMs;
  return diffFinal;
}

void delay(uint32_t value)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d);\n", "delay", value);
  arduino_stub::log(buffer);

  //realtime
  Sleep(value);
}

void delayMicroseconds(uint16_t value)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s();\n", "delayMicroseconds");
  arduino_stub::log(buffer);

  //realtime
  uint32_t microSeconds = value;
  uint32_t milliSeconds = microSeconds/1000;
  if (milliSeconds == 0)
    milliSeconds = 1;
  Sleep(milliSeconds);
}

}; //namespace realtime

namespace simulation
{

static uint32_t gMicroResolution = 8; //8 usec resolution (increment for each calls)
static uint32_t gMicroCounter = 0;
uint32_t micros()
{
  //counter increments
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s();\n", "micros");
  arduino_stub::log(buffer);

  gMicroCounter += gMicroResolution;
  return gMicroCounter;
}

uint32_t millis()
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s();\n", "millis");
  arduino_stub::log(buffer);

  //based on micro
  gMicroCounter += gMicroResolution;
  
  uint32_t microSeconds = gMicroCounter;
  uint32_t milliSeconds = microSeconds/1000;
  return milliSeconds;
}

void delay(uint32_t value)
{
  //based on millis() timing
  uint32_t startTime = millis();
  uint32_t endTime = startTime + value;
  while( millis() < endTime )
  {
  }
}

void delayMicroseconds(uint16_t value)
{
  //based on micros() timing
  uint32_t startTime = micros();
  uint32_t endTime = startTime + value;
  while( micros() < endTime )
  {
  }
}

}; //namespace simulation

uint32_t micros()
{
  if (arduino_stub::gClockStrategy == arduino_stub::CLOCK_SIMULATION)
    return simulation::micros();
  else
    return realtime::micros();
}

uint32_t millis()
{
  if (arduino_stub::gClockStrategy == arduino_stub::CLOCK_SIMULATION)
    return simulation::millis();
  else
    return realtime::millis();
}

void delay(uint32_t value)
{
  if (arduino_stub::gClockStrategy == arduino_stub::CLOCK_SIMULATION)
    return simulation::delay(value);
  else
    return realtime::delay(value);
}

void delayMicroseconds(uint16_t value)
{
  if (arduino_stub::gClockStrategy == arduino_stub::CLOCK_SIMULATION)
    return simulation::delayMicroseconds(value);
  else
    return realtime::delayMicroseconds(value);
}

//pow(base, exponent)
//sqrt(x)

//#define CHANGE 2
//#define RISING 3
//#define FALLING 4
//typedef void (*ISR)();
const char * toInterruptModeString(uint8_t value)
{
  if (value == HIGH)
    return "HIGH";
  else if (value == LOW)
    return "LOW";
  else if (value == CHANGE)
    return "CHANGE";
  else if (value == RISING)
    return "RISING";
  else if (value == FALLING)
    return "FALLING";
  return "EXPECTING_HIGH_LOW_CHANGE_RISING_OR_FALLING";
}

void attachInterrupt(uint8_t pin, ISR func, uint8_t mode)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d, ISR=0x%x, %s);\n", __FUNCTION__, pin, func, toInterruptModeString(mode));
  arduino_stub::log(buffer);
}

void detachInterrupt(uint8_t pin)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s(%d);\n", __FUNCTION__, pin);
  arduino_stub::log(buffer);
}

void noInterrupts()
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s();\n", __FUNCTION__);
  arduino_stub::log(buffer);
}

void interrupts()
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s();\n", __FUNCTION__);
  arduino_stub::log(buffer);
}

/*
Characters 
-isAlphaNumeric() 
-isAlpha() 
-isAscii() 
-isWhitespace() 
-isControl() 
-isDigit() 
-isGraph() 
-isLowerCase() 
-isPrintable() 
-isPunct() 
-isSpace() 
-isUpperCase() 
-isHexadecimalDigit() 
*/
bool isAlpha(int8_t value);
bool isDigit(int8_t value);
bool isAlphaNumeric(int8_t value)
{
  return isAlpha(value) || isDigit(value);
}

bool isAlpha(int8_t value)
{
  bool alpha =  value >= 'A' && value <= 'Z' ||
                value >= 'a' && value <= 'z';
  return alpha;
}

bool isAscii(int8_t value)
{
  return true;  
}

bool isWhitespace(int8_t value)
{
  bool white =  value == ' '  ||
                value == '\t' ||
                value == '\a' ||
                value == '\n';
  return white;  
}

bool isControl(int8_t value)
{
  //http://ascii.cl/control-characters.htm
  return value >=0 && value <= 31;
}

bool isDigit(int8_t value)
{
  bool digit = value >= '0' && value <= '9';
  return digit;
}

bool isGraph(int8_t value)
{
  return false;
}

bool isLowerCase(int8_t value)
{
  return value >= 'a' && value <= 'z';
}

bool isUpperCase(int8_t value);
bool isPunct(int8_t value);
bool isPrintable(int8_t value)
{
  return  isAlphaNumeric(value) ||
          isWhitespace(value) ||
          isAscii(value) ||
          isLowerCase(value) ||
          isUpperCase(value) ||
          isPunct(value);
}

bool isPunct(int8_t value)
{
/*
Symbol ASCII 
! 33 
" 34 
# 35 
$ 36 
% 37 
& 38 
' 39 
( 40 
) 41 
* 42 
+ 43 
, 44 
- 45 
. 46 
/ 47 
: 58 
; 59 
< 60 
= 61 
> 62 
? 63 
 
Symbol ASCII
@ 64
[ 91
\ 92
] 93
^ 94
_ 95
` 96
{ 123
¦ 124
} 125
~ 126
0 48
1 49
2 50
3 51
4 52
5 53
6 54
7 55
8 56
9 57
*/   
  return  (value >=  33 && value <=  47) ||
          (value >=  58 && value <=  64) ||
          (value >=  91 && value <=  96) ||
          (value >= 123 && value <= 126);
}

bool isSpace(int8_t value)
{
  return value == ' ';
}

bool isUpperCase(int8_t value)
{
  return value >= 'A' && value <= 'Z';
}

bool isHexadecimalDigit(int8_t value)
{
  return  isDigit(value) ||
          (value >= 'a' && value <= 'f') ||
          (value >= 'A' && value <= 'F') ;
}

/*
Random Numbers
-randomSeed() 
-random() 
*/
void randomSeed(int16_t value)
{
  ::srand(value);
}
void randomSeed(int32_t value)
{
  ::srand(value);
}

int32_t random(int32_t min, int32_t max)
{
  int systemMax = RAND_MAX;
  int value = rand(); //between 0 (inclusive) and RAND_MAX (exclusive).
  int32_t width = max-min;
  value = value%width; //from 0 (inclusive) to width (exclusive)
  value += min;
  return value;
}
int32_t random(int32_t max)
{
  return random(0, max);
}
