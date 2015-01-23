// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#include <string>
#include "./unification.h"
#include "./utility-tools.h"
#include "./timing-helper.h"

int main(int argc, char** argv) {
  using std::string;
  Unification uf;
  bool cmdArguments = uf.ParseCommandlineArguments(argc, argv);
  if (!cmdArguments) {
    uf.PrintUsage();
    return 0;
  }
  TimingHelper tf;
  std::cout << "Processing files..." << std::flush;
  tf.Start();
  uf.ProcessFiles();
  tf.Stop();
  std::cout << "done in " << tf.Get() << "." << std::endl;
  std::cout << "Unifying entities:" << std::endl;
  for (size_t i = 0; i < uf.iterations(); ++i) {
    std::cout << "[" << (i+1) << "]" << std::flush;
    tf.Start();
    uf.UnifyEntities();
    tf.Stop();
    std::cout << "\n...done in " << tf.Get() << "." << std::endl;
  }
  std::cout << "Generating output..." << std::flush;
  tf.Start();
  uf.GenerateOutput();
  tf.Stop();
  std::cout << "done in " << tf.Get() << "." << std::endl;
  uf.PrintStatistics();
}
