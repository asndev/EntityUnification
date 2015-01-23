// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <string>
#include "./utility-tools.h"

using std::string;

// _____________________________________________________________________________
TEST(UtilitiesTest, getError) {
  {
    string testString = "This is an error Message";
    testString = Utilities::kBoldMode + string("")
                 + Utilities::kRedMode + string("Error: ")
                 + Utilities::kResetMode + testString +
                 string("\n");
    string s = "This is an error Message";
    ASSERT_EQ(testString, Utilities::GetError(s, ""));
  }
  {
    string testString = "";
    testString = Utilities::kBoldMode + string("")
                 + Utilities::kRedMode + string("Error: ")
                 + Utilities::kResetMode + testString +
                 string("\n");
    string s = "";
    ASSERT_EQ(testString, Utilities::GetError(s, ""));
  }
}
