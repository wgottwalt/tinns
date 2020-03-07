#include <iomanip>
#include <sstream>
#include "common/Time.hxx"

namespace Time
{
    //--- functions ---

    Timepoint now()
    {
        return Clock::now();
    }

    time_t nowTimeT()
    {
        return toTimeT(now());
    }

    time_t toTimeT(const Timepoint &timepoint)
    {
        return Clock::to_time_t(timepoint);
    }

    Timepoint toTimepoint(const time_t timet)
    {
        return Clock::from_time_t(timet);
    }

    std::string toDateString(const time_t timet, const std::string &format)
    {
        const struct std::tm temp = *std::localtime(&timet);
        std::stringstream result;

        result << std::put_time(&temp, format.c_str());

        return result.str();
    }

    std::string toDateString(const Timepoint &timepoint, const std::string &format)
    {
        return toDateString(toTimeT(timepoint), format);
    }
}
