// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#ifndef ENTITYUNIFICATION_SRC_UTILITY_TOOLS_H_
#define ENTITYUNIFICATION_SRC_UTILITY_TOOLS_H_

#include <gtest/gtest.h>
#include <chrono>
#include <string>

// Static Class to support developer with usefull keyfeatures.
class Utilities {
 public:
  // special characters
  static constexpr char* kResetMode = const_cast<char*>("\033[0m");
  static constexpr char* kBoldMode = const_cast<char*>("\033[1m");
  static constexpr char* kRedMode = const_cast<char*>("\033[0;31m");
  static void Info(const std::string& s);
  static void Info(const std::string& s, const std::string& title);
  static void Error(const std::string& s);
  static void Error(const std::string& s, const std::string& title);
  static void GenerateFile(const std::string& filename,
                           const std::ostringstream& text);
  static void GenerateFile(const std::string& filename,
                           const std::string& text);
  static void GenerateFileIfNotExists(const std::string& filename,
                                      const std::ostringstream& text);
  static void AppendToFile(const std::string& filename,
                           const std::ostringstream& text);
  static int GetFileSize(const std::string& filename);
  static std::string GetError(const std::string& s, const std::string& title);
  static std::string GetInfo(const std::string& s, const std::string& title);
 private:
  Utilities() {}
};

#endif  // ENTITYUNIFICATION_SRC_UTILITY_TOOLS_H_
