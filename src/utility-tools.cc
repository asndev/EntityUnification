// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#include "./utility-tools.h"
#include <fstream> // NOLINT
#include <string>
#include <sstream>

using std::string;

// _____________________________________________________________________________
void Utilities::Info(const std::string& s) {
  std::cout << GetInfo(s, "");
}

// _____________________________________________________________________________
void Utilities::Error(const std::string& s) {
  std::cerr << GetError(s, "");
}

// _____________________________________________________________________________
std::string Utilities::GetError(const string& s, const string& title) {
  string t = (title == "") ? "" : " (" + title + ") ";
  std::ostringstream oss;
  oss << kBoldMode << kRedMode << "Error" << t << ": " << kResetMode
      << s << '\n';
  return oss.str();
}

// _____________________________________________________________________________
std::string Utilities::GetInfo(const string& s, const string& title) {
  string t = (title == "") ? "" : " (" + title + ")";
  std::ostringstream oss;
  oss << kBoldMode << kRedMode << "Info" << t << ": " << kResetMode
      << s << '\n';
  return oss.str();
}

// _____________________________________________________________________________
int Utilities::GetFileSize(const std::string& filename) {
  std::ifstream file(filename);
  file.seekg(0, std::ios_base::end);
  int size = file.tellg();
  file.close();
  return size;
}

// _____________________________________________________________________________
void Utilities::GenerateFile(const string& filename,
                             const std::ostringstream& text) {
  if (filename != "") {
    std::ofstream ostream;
    ostream.open(filename);
    ostream << text.str();
    ostream.close();
  }
}

// _____________________________________________________________________________
void Utilities::GenerateFileIfNotExists(const std::string& filename,
                                        const std::ostringstream& text) {
  std::ifstream file(filename);
  if (!file.good()) GenerateFile(filename, text);
}

// _____________________________________________________________________________
void Utilities::AppendToFile(const string& filename,
                             const std::ostringstream& text) {
  FILE* write;
  write = fopen(filename.c_str(), "a+");
  fprintf(write, "%s \n", text.str().c_str());
  fclose(write);
}

// _____________________________________________________________________________
void Utilities::GenerateFile(const string& filename,
                             const std::string& text) {
  if (filename != "") {
    std::ofstream ostream;
    ostream.open(filename);
    ostream << text;
    ostream.close();
  }
}
