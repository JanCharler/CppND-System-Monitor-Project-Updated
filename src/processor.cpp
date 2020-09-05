#include <vector>
#include <string>

#include "processor.h"
#include "linux_parser.h"

using std::vector;
using std::string;

// Done: Return the aggregate CPU utilization
float Processor::Utilization() {  
    return static_cast<float>(LinuxParser::ActiveJiffies()) / LinuxParser::Jiffies();
}