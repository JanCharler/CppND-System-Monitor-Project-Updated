#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream fs(kProcDirectory + kMeminfoFilename);
  float total = -1;
  float free = -1;
  float used = -1;

  // Read from /proc/meminfo.
  while (fs.is_open()) {
    string line;
    while (std::getline(fs, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      string token;
      float val;
      std::istringstream ss(line);
      ss >> token >> val;
      if (token == "MemTotal") total = val;
      if (token == "MemFree") free = val;
      if (total != -1 && free != -1) break;  //
    }
    fs.close();
  }
  // Calculate and return used percentage
  used = total - free;
  return used / total;
}

// Done: Read and return the system uptime
long LinuxParser::UpTime() {
  double upTime = -1;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  filestream >> upTime;
  return upTime;
}

// Done: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Done: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::fstream filestream(kProcDirectory + '/' + std::to_string(pid) +
                          kStatFilename);
  vector<string> cpu_data;
  string token;
  vector<string> tokens{"-1"};

  // Gather the first 22 tokens, start from index 1 to make accessing easier
  for (int i = 1; i <= 22; i++) {
    token = "";
    filestream >> token;
    tokens.push_back(token);
  }

  // Tokens 14, 15, 16, 17 are utime, stime, cutime and cstime respectively.
  // Token 22 is starttime
  float utime = 0;
  float stime = 0;
  float cutime = 0;
  float cstime = 0;

  if (tokens[14] != "") utime = stof(tokens[14]);
  if (tokens[15] != "") stime = stof(tokens[15]);
  if (tokens[16] != "") cutime = stof(tokens[16]);
  if (tokens[17] != "") cstime = stof(tokens[17]);

  // Total jiffy calculation:
  float total_time = utime + stime;
  total_time = total_time + cutime +
               cstime;  // use if wanting to include children processes too
  return total_time;
}

// Done: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> data = LinuxParser::CpuUtilization();
  if (data.size() < 8) return 0;

  int user = 0;
  int nice = 0;
  int system = 0;
  int irq = 0;
  int softirq = 0;
  int steal = 0;

  if (data[0] != "") user = std::stoi(data[0]);
  if (data[1] != "") nice = std::stoi(data[1]);
  if (data[2] != "") system = std::stoi(data[2]);
  if (data[5] != "") irq = std::stoi(data[5]);
  if (data[6] != "") softirq = std::stoi(data[6]);
  if (data[7] != "") steal = std::stoi(data[7]);
  // int guest = std::stoi(data[8]);
  // int guest_nice = std::stoi(data[9]);

  int active_jiffies = user + nice + system + irq + softirq + steal;

  return active_jiffies;
}

// Done: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> data = LinuxParser::CpuUtilization();
  if (data.size() < 5) return 0;

  int idle = 0;
  int iowait = 0;

  if (data[3] != "") idle = std::stoi(data[3]);
  if (data[4] != "") iowait = std::stoi(data[4]);
  int idle_jiffies = idle + iowait;

  return idle_jiffies;
}

// Done: Read and return CPU utilization data
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line;
  std::vector<string> cpu_aggr_data;
  while (std::getline(filestream, line)) {
    std::istringstream iss(line);
    string token;
    string data;
    iss >> token;
    if (token == "cpu") {
      while (iss) {
        iss >> data;
        cpu_aggr_data.push_back(data);
      }
      filestream.close();
      break;
    }
  }
  return cpu_aggr_data;
}

// Done: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  int processes;
  std::istringstream iss;
  string line;
  while (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      iss.str(line);
      string token;
      iss >> token;
      if (token == "processes") {
        iss >> processes;
        filestream.close();
      }
      continue;
    }
  }
  return processes;
}

// Done: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  int running_processes = -1;
  std::istringstream iss;
  string line;
  while (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      iss.str(line);
      string token;
      iss >> token;
      if (token == "procs_running") {
        iss >> running_processes;
        filestream.close();
      }
      continue;
    }
    filestream.close();
  }
  return running_processes;
}

// Done: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + '/' + std::to_string(pid) +
                           kCmdlineFilename);
  string line;
  filestream >> line;
  return line;
}

// Done: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::ifstream filestream(kProcDirectory + '/' + std::to_string(pid) +
                           kStatusFilename);
  std::istringstream iss;
  string line, token;
  double ram;
  while (std::getline(filestream, line)) {
    iss.str(line);
    iss >> token;
    ;
    if (token == "VmSize:") {
      iss >> ram;
      ram /= 1000;
      break;
    }
    continue;
  }
  return std::to_string((int)ram);
}

// Done: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatusFilename);
  std::istringstream iss;
  string line;
  string token, uid;
  while (std::getline(filestream, line)) {
    iss.str(line);
    iss >> token;
    if (token == "Uid:") {
      iss >> uid;
      break;
    }
  }
  return uid;
}

// Done: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);  // Get user id

  std::ifstream filestream(kPasswordPath);
  std::istringstream iss;
  string line, username, password, this_uid;
  while (std::getline(filestream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    iss.str(line);
    iss >> username >> password >> this_uid;
    if (this_uid == uid) break;
    username = "";
    continue;
  }
  return username;
}

// Done: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + '/' + std::to_string(pid) +
                           kStatFilename);
  string token;
  for (int i = 0; i < 22; i++) {
    filestream >> token;
  }
  long clock_ticks = 0;
  if (token != "") clock_ticks = std::stol(token);

  long up_time = clock_ticks / sysconf(_SC_CLK_TCK);
  return up_time;
}
