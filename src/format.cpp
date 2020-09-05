#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 

    int hours = seconds/3600;
    int minutes = (seconds - (hours * 3600))/60;
    int trailing_seconds = seconds - ((hours*3600) + (minutes*60));

    string hh = std::to_string(hours);
    string mm = std::to_string(minutes);
    string ss = std::to_string(trailing_seconds);

    // Pad with 0s if necessary.
    if (hours < 10) hh = "0" + hh;
    if (minutes < 10) mm = "0" + mm;
    if (trailing_seconds < 10) ss = "0" + ss;
    
    return hh+":"+mm+":"+ss;
}