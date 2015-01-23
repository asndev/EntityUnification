// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#ifndef ENTITYUNIFICATION_SRC_FILE_WRITER_H_
#define ENTITYUNIFICATION_SRC_FILE_WRITER_H_

#include <fstream> // NOLINT
#include <string>

using std::string;

class FileWriter {
 public:
  FileWriter() {}
  ~FileWriter() { if (stream_.is_open()) stream_.close(); }
  explicit FileWriter(const std::string& filename) { stream_.open(filename); }
  void set_stream(const std::string& filename) { stream_.open(filename); }
  void write(const std::string& text) {
    if (stream_.is_open()) stream_ << text;
  }
 private:
  std::ofstream stream_;
};

#endif  // ENTITYUNIFICATION_SRC_FILE_WRITER_H_
