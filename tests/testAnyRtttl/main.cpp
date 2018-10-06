// testAnyRtttl.cpp : Defines the entry point for the console application.
//

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
  ::testing::GTEST_FLAG(output) = "xml:testAnyRtttl.testResults.xml";
  ::testing::GTEST_FLAG(print_time) = true;

  ::testing::InitGoogleTest(&argc, argv);
  int wResult = RUN_ALL_TESTS(); //Find and run all tests

  return wResult; // returns 0 if all the tests are successful, or 1 otherwise
}
