// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#ifndef ENTITYUNIFICATION_SRC_TIMING_HELPER_H_
#define ENTITYUNIFICATION_SRC_TIMING_HELPER_H_

#include <chrono>
#include <string>


// Static Class to support developer with usefull keyfeatures.
class TimingHelper {
 public:
  TimingHelper();
  static std::string GetCurrentTime();
  void Start();
  void Stop();
  std::string GetMs() const;
  std::string GetNs() const;
  std::string Get() const;
 private:
  std::chrono::high_resolution_clock::time_point start_;
  std::chrono::high_resolution_clock::time_point end_;
};

#endif  // ENTITYUNIFICATION_SRC_TIMING_HELPER_H_
