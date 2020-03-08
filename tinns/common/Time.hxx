#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
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

    template <typename T = Clock>
    constexpr Timepoint now()
    {
        return T::now();
    }

    template <typename T = Clock>
    constexpr time_t toTimeT(const Timepoint &timepoint)
    {
        return T::to_time_t(timepoint);
    }

    template <typename T = Clock>
    constexpr Timepoint toTimepoint(const time_t timet)
    {
        return T::from_time_t(timet);
    }

    template <typename T = Clock>
    constexpr time_t nowTimeT()
    {
        return toTimeT<T>(now<T>());
    }

    template <typename T = Clock>
    constexpr std::string toDateString(const time_t timet,
                                       const std::string &format = DefCompactDateTime)
    {
        const struct std::tm temp = *std::localtime(&timet);
        std::stringstream result;

        result << std::put_time(&temp, format.c_str());

        return result.str();
    }

    template <typename T = Clock>
    constexpr std::string toDateString(const typename T::time_point &timepoint,
                                       const std::string &format = DefCompactDateTime)
    {
        return toDateString(toTimeT(timepoint), format);
    }
}
