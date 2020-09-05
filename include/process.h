#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  // Accessors
  int Pid();                               // Done: See src/process.cpp
  std::string User();                      // Done: See src/process.cpp
  std::string Command();                   // Done: See src/process.cpp
  float CpuUtilization();                  // Done: See src/process.cpp
  std::string Ram();                       // Done: See src/process.cpp
  long int UpTime();                       // Done: See src/process.cpp
  bool operator<(Process const& a) const;  // Done: See src/process.cpp
  bool operator>(Process const& a) const;
  void PopulateData(int pid); // Searches file system and populates data based on pid given

 private:
  int pid_;
  std::string user_;
  std::string command_;
  float cpuUtilization_;
  std::string ram_;
  long int uptime_;
};

#endif