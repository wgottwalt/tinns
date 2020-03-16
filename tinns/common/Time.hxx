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

    template <typename T = Clock>
    constexpr size_t ns(const typename T::duration &duration)
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }

    template <typename T = Clock>
    constexpr size_t us(const typename T::duration &duration)
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }

    template <typename T = Clock>
    constexpr size_t ms(const typename T::duration &duration)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    template <typename T = Clock>
    constexpr size_t sec(const typename T::duration &duration)
    {
        return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    }

    template <typename T = Clock>
    constexpr size_t min(const typename T::duration &duration)
    {
        return std::chrono::duration_cast<std::chrono::minutes>(duration).count();
    }

    template <typename T = Clock>
    constexpr size_t hour(const typename T::duration &duration)
    {
        return std::chrono::duration_cast<std::chrono::hours>(duration).count();
    }
}
