// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <string>
#include "./file-writer.h"

using std::string;

// _____________________________________________________________________________
TEST(FileWriterTest, write) {
  FileWriter fw("./TestData/file-writer-test.txt");
  fw.write("1234");
  fw.write("5678");
}
