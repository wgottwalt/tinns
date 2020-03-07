#pragma once

#include <chrono>
#include <string>

namespace Time
{
    //--- types and constants ---
    using Clock = std::chrono::system_clock;
    using Rep = Clock::rep;
    using Period = Clock::period;
    using Duration = Clock::duration;
    using Timepoint = Clock::time_point;

    // fromat: YYYYMMDD.hhmmss - 24h
    static const std::string DefCompactDateTime = "%Y%m%j.%H%M%S";
    static const std::string DefDateTime = "%Y/%m/%j %H:%M:%S";

    //--- functions ---
    Timepoint now();
    time_t nowTimeT();

    time_t toTimeT(const Timepoint &timepoint);
    Timepoint toTimepoint(const time_t timet);

    std::string toDateString(const time_t timet, const std::string &format = DefCompactDateTime);
    std::string toDateString(const Timepoint &timepoint, const std::string &format = DefCompactDateTime);
}
