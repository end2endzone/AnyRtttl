// ---------------------------------------------------------------------------
// AUTHOR/LICENSE:
//  The following code was written by Antoine Beauchamp. For other authors, see AUTHORS file.
//  The code & updates for the library can be found at https://github.com/end2endzone/AnyRtttl
//  MIT License: http://www.opensource.org/licenses/mit-license.php
// ---------------------------------------------------------------------------

#include <stdio.h>
#include <string>

#include "TestAnyRtttl.h"
#include "Arduino.h"
#include "anyrtttl.h"
#include "bitreader.h"

#include "rapidassist/strings.h"
#include "rapidassist/testing.h"
#include "rapidassist/filesystem.h"

#include "IncrementalClockStrategy.h"

using namespace testarduino;

//rtttl native format
const char * tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";

//RTTTL 10 bits binary format for the following: tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a
//Compatible with AnyRtttl library v2.0
//The code & updates for the AnyRtttl library can be found on http://end2endzone.com
const unsigned char tetris10[] = {0x0A, 0x14, 0x12, 0xCE, 0x34, 0xE0, 0x82, 0x14, 0x32, 0x38, 0xE0, 0x4C, 0x2A, 0xAD, 0x34, 0xA0, 0x84, 0x0B, 0x0E, 0x28, 0xD3, 0x4C, 0x03, 0x2A, 0x28, 0xA1, 0x80, 0x2A, 0xA5, 0xB4, 0x93, 0x82, 0x1B, 0xAA, 0x38, 0xE2, 0x86, 0x12, 0x4E, 0x38, 0xA0, 0x84, 0x0B, 0x0E, 0x28, 0xD3, 0x4C, 0x03, 0x2A, 0x28, 0xA1, 0x80, 0x2A, 0xA9, 0x04};
const int tetris10_length = 42;

//RTTTL 16 bits binary format for the following: tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a
//Compatible with AnyRtttl library v2.0
//The code & updates for the AnyRtttl library can be found on http://end2endzone.com
const unsigned char tetris16[] = {0x0A, 0x14, 0x12, 0x02, 0x33, 0x01, 0x03, 0x02, 0x0B, 0x02, 0x14, 0x02, 0x0C, 0x02, 0x03, 0x02, 0x33, 0x01, 0x2A, 0x01, 0x2B, 0x01, 0x03, 0x02, 0x12, 0x02, 0x0B, 0x02, 0x03, 0x02, 0x32, 0x01, 0x33, 0x01, 0x03, 0x02, 0x0A, 0x02, 0x12, 0x02, 0x02, 0x02, 0x2A, 0x01, 0x29, 0x01, 0x3B, 0x01, 0x0A, 0x02, 0x1B, 0x02, 0x2A, 0x02, 0x23, 0x02, 0x1B, 0x02, 0x12, 0x02, 0x13, 0x02, 0x03, 0x02, 0x12, 0x02, 0x0B, 0x02, 0x03, 0x02, 0x32, 0x01, 0x33, 0x01, 0x03, 0x02, 0x0A, 0x02, 0x12, 0x02, 0x02, 0x02, 0x2A, 0x01, 0x2A, 0x01};
const int tetris16_length = 42;


#define PIEZO_PIN 99

namespace arduino { namespace test
{

  //bit reader support
  #ifndef USE_BITADDRESS_READ_WRITE
  BitReader bitreader;
  #else
  BitAddress bitreader;
  #endif
  uint16_t readNextBits(uint8_t numBits)
  {
    uint16_t bits = 0;
    bitreader.read(numBits, &bits);
    return bits;
  }
  
  void filterToneFunctions(ra::strings::StringVector & calls)
  {
    ra::strings::StringVector copy = calls;
    calls.clear();

    for(size_t i=0; i<copy.size(); i++)
    {
      std::string & currentCall = copy[i];
      if (currentCall.find("tone(") != std::string::npos)
      {
        //that a tone(...) function call. Keep it
        calls.push_back(currentCall);
      }
    }
  }

  bool readFile(const char * iPath, std::string & content)
  {
    //static const std::string EMPTY;
    content = "";

    //allocate a buffer which can hold the content of the file
    uint32_t file_size = ra::filesystem::GetFileSize(iPath);
    //uint32_t buffer_size = file_size + 1; //+1 for the ending \0 character

    FILE * f = fopen(iPath, "rb");
    if (!f)
      return false;

    char * buffer = new char[file_size]; 
    if (!buffer)
    {
      fclose(f);
      return false;
    }
    memset(buffer, 0, file_size);

    //read the content
    size_t read_size = fread(buffer, 1, file_size, f);
    if (read_size != file_size)
    {
      delete[] buffer;
      fclose(f);
      return false;
    }

    fclose(f);

    //copy the content of the buffer to the output string
    content.assign(buffer, file_size);

    bool success = (content.size() == file_size);
    return success;
  }

  bool writeFile(const char * iPath, const std::string & content)
  {
    FILE * f = fopen(iPath, "wb");
    if (!f)
      return false;

    size_t write_size = fwrite(content.c_str(), 1, content.size(), f);
    if (write_size != content.size())
    {
      fclose(f);
      return false;
    }

    fclose(f);
    return true;
  }

  bool fileContentReplace(const std::string & iPath, const std::string & iOldValue, const std::string & iNewValue)
  {
    std::string content;
    if (!readFile(iPath.c_str(), content))
      return false;

    int num_finding = ra::strings::Replace(content, iOldValue, iNewValue);

    //does the file was modified?
    if (num_finding)
    {
      //yes, write modifications to the file
      if (!writeFile(iPath.c_str(), content))
        return false;
    }

    return true;
  }

  //--------------------------------------------------------------------------------------------------
  void TestAnyRtttl::SetUp()
  {
    IncrementalClockStrategy & gClock = IncrementalClockStrategy::getInstance();
    gClock.setMicrosecondsResolution(3000); //3ms increments
    
    //force realtime strategy of win32Arduino library
    testarduino::setClockStrategy(&gClock);
  }
  //--------------------------------------------------------------------------------------------------
  void TestAnyRtttl::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
} // End namespace test
} // End namespace arduino
