// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#include "./timing-helper.h"

#include <chrono>
#include <string>
#include <sstream>
#include <ctime>

using std::string;

// _____________________________________________________________________________
TimingHelper::TimingHelper() {}

// _____________________________________________________________________________
void TimingHelper::Start() {
  start_ = std::chrono::high_resolution_clock::now();
}

// _____________________________________________________________________________
void TimingHelper::Stop() {
  end_ = std::chrono::high_resolution_clock::now();
}

// _____________________________________________________________________________
string TimingHelper::GetCurrentTime() {
  auto now = std::chrono::system_clock::to_time_t(
          std::chrono::system_clock::now());
  std::string s = std::ctime(&now); // NOLINT
  return s;
}

// _____________________________________________________________________________
string TimingHelper::Get() const {
  string time_unit = "ms";
  auto diff =
    (std::chrono::duration_cast<std::chrono::nanoseconds>(end_ - start_).count()
     * 0.000001);
  std::ostringstream oss;
  if (diff > 1000) {
    diff = diff / 1000;
    time_unit = "s";
    if (diff > 60) {
      diff = diff / 60;
      time_unit = "min";
    }
  }
  oss << diff << time_unit;
  return oss.str();
}

// _____________________________________________________________________________
string TimingHelper::GetMs() const {
  string time_unit = "ms";
  auto diff =
    (std::chrono::duration_cast<std::chrono::nanoseconds>(end_ - start_).count()
     * 0.000001);
  std::ostringstream oss;
  oss << diff << time_unit;
  return oss.str();
}

// _____________________________________________________________________________
string TimingHelper::GetNs() const {
  std::ostringstream oss;
  oss <<
    std::chrono::duration_cast<std::chrono::nanoseconds>(end_ - start_).count();
  oss << "ns";
  return oss.str();
}
