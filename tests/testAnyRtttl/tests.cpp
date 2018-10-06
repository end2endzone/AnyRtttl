// testAnyRtttl.cpp : Defines the entry point for the console application.
//

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <string>
#include "arduino.h"
#include "anyrtttl.h"
#include "bitreader.h"
#include "tests.h"
#include "gTestHelper.h"

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
  
  void filterToneFunctions(gTestHelper::StringVector & calls)
  {
    gTestHelper::StringVector copy = calls;
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



  //--------------------------------------------------------------------------------------------------
  void Tests::SetUp()
  {
    //force realtime strategy of win32Arduino library
    arduino_stub::setClockStrategy(arduino_stub::CLOCK_REALTIME);
  }
  //--------------------------------------------------------------------------------------------------
  void Tests::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(Tests, testNonBlockingPlay)
  {
    //setup log file
    std::string logFile = std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()) + ".log";
    arduino_stub::setLogFile(logFile.c_str());

    //play the actual content
    anyrtttl::nonblocking::begin(PIEZO_PIN, tetris);
    while( !anyrtttl::nonblocking::done() )
    {
      anyrtttl::nonblocking::play();
    }

    //assert

    //load both files into memory
    gTestHelper & helper = gTestHelper::getInstance();
    gTestHelper::StringVector expectedCalls;
    ASSERT_TRUE (helper.getTextFileContent("expected_call_stack.log", expectedCalls));
    gTestHelper::StringVector actualCalls;
    ASSERT_TRUE (helper.getTextFileContent(logFile.c_str(), actualCalls));

    //assert that file ends with a noTone();
    std::string lastCall = actualCalls[actualCalls.size()-1];
    ASSERT_NE(lastCall.find("noTone("), std::string::npos);

    //compare tone() function calls with the "blocking" template
    //file since its the only common thing betwwen blocking and
    //non-blocking api.

    //filter out anything that is not tone(...)
    filterToneFunctions(expectedCalls);
    filterToneFunctions(actualCalls);

    ASSERT_EQ( expectedCalls.size(), actualCalls.size() );
    for(size_t i=0; i<expectedCalls.size(); i++)
    {
      std::string expected = expectedCalls[i];
      std::string actual = actualCalls[i];
      ASSERT_EQ(expected, actual);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(Tests, testInterruptedPlay)
  {
    //setup log file
    std::string logFile = std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()) + ".log";
    arduino_stub::setLogFile(logFile.c_str());

    anyrtttl::nonblocking::begin(PIEZO_PIN, tetris);

    //play for 5 sec then stop.
    //note: this is a blocking code section
    //use to demonstrate the use of stop()
    unsigned long start = millis();
    while( millis() - start < 5000 ) 
    {
      anyrtttl::nonblocking::play();
    }
    anyrtttl::nonblocking::stop();

    //assert

    //load output file into memory
    gTestHelper & helper = gTestHelper::getInstance();
    gTestHelper::StringVector actualCalls;
    ASSERT_TRUE (helper.getTextFileContent(logFile.c_str(), actualCalls));

    //assert that file ends with a noTone();
    std::string lastCall = actualCalls[actualCalls.size()-1];
    ASSERT_NE(lastCall.find("noTone("), std::string::npos);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(Tests, testBlockingPlay)
  {
    //setup log file
    std::string logFile = std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()) + ".log";
    arduino_stub::setLogFile(logFile.c_str());

    //play the actual content
    anyrtttl::blocking::play(PIEZO_PIN, tetris);

    //assert
    gTestHelper & helper = gTestHelper::getInstance();
    std::string diffReason;
    bool fileAreIdentical = helper.isFileEquals("expected_call_stack.log", logFile.c_str(), diffReason, 1);
    ASSERT_TRUE( fileAreIdentical ) << diffReason.c_str();
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(Tests, testBlockingPlay10)
  {
    //setup log file
    std::string logFile = std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()) + ".log";
    arduino_stub::setLogFile(logFile.c_str());

    //play the actual content
    bitreader.setBuffer(tetris10);
    anyrtttl::blocking::play10Bits(PIEZO_PIN, tetris10_length, &readNextBits);

    //assert
    gTestHelper & helper = gTestHelper::getInstance();
    std::string diffReason;
    bool fileAreIdentical = helper.isFileEquals("expected_call_stack.log", logFile.c_str(), diffReason, 1);
    ASSERT_TRUE( fileAreIdentical ) << diffReason.c_str();
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(Tests, testBlockingPlay16)
  {
    //setup log file
    std::string logFile = std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()) + ".log";
    arduino_stub::setLogFile(logFile.c_str());

    //play the actual content
    anyrtttl::blocking::play16Bits(PIEZO_PIN, tetris16, tetris16_length);

    //assert
    gTestHelper & helper = gTestHelper::getInstance();
    std::string diffReason;
    bool fileAreIdentical = helper.isFileEquals("expected_call_stack.log", logFile.c_str(), diffReason, 1);
    ASSERT_TRUE( fileAreIdentical ) << diffReason.c_str();
  }
  //--------------------------------------------------------------------------------------------------
  static int toneCounts = 0;
  static int noToneCounts = 0;
  static int delayCounts = 0;
  void myToneFunc(byte pin, uint16_t frequency, uint32_t duration)
  {
    toneCounts++;
  }
  void myNoToneFunc(byte pin)
  {
    noToneCounts++;
  }
  void myDelayFunc(uint32_t duration)
  {
    delayCounts++;
  }

  TEST_F(Tests, testCustomFunctionsPlay)
  {
    //setup log file
    std::string logFile = std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()) + ".log";
    arduino_stub::setLogFile(logFile.c_str());

    toneCounts = 0;
    noToneCounts = 0;
    delayCounts = 0;

    //use test's custom functions
    anyrtttl::setToneFunction(&myToneFunc);
    anyrtttl::setNoToneFunction(&myNoToneFunc);
    anyrtttl::setDelayFunction(&myDelayFunc);

    //play the actual content
    anyrtttl::blocking::play(PIEZO_PIN, tetris);

    //back to native arduino functions
    anyrtttl::setToneFunction(&tone);
    anyrtttl::setNoToneFunction(&noTone);
    anyrtttl::setDelayFunction(&delay);

    //assert
    ASSERT_GT( toneCounts  , 0 );
    ASSERT_GT( noToneCounts, 0 );
    ASSERT_GT( delayCounts , 0 );
  }
  //--------------------------------------------------------------------------------------------------
} // End namespace test
} // End namespace arduino
