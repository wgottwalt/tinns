#pragma once

#include <sstream>
#include <string>

namespace Misc
{
    namespace Net
    {
        struct IPv4 {
            union {
                uint32_t val;
                uint8_t raw[sizeof (uint32_t)];
                struct {
                    uint8_t a, b, c, d;
                };
            };
        };

        uint32_t ip4StringToUint32(const std::string &ip);
        const std::string uint32ToIp4String(const uint32_t ip);
    }

    namespace String
    {
        const std::string accessLevelToString(const int32_t level);
        void cleanUpString(std::string &str);
        std::string trim(const std::string &data, const char delim, const bool at_start = true,
                         const bool at_end = true);
        std::string trim(const std::string &data, const bool at_start = true,
                         const bool at_end = true);

        template <typename H, typename... T>
        constexpr std::string create(const H &head, const T &...tail)
        {
            std::stringstream strm;

            if constexpr (sizeof... (T))
                strm << head << create(tail...);
            else
                strm << head;

            return strm.str();
        }
    }

    uint16_t DistanceApprox(const uint16_t x1, const uint16_t y1, const uint16_t z1,
                            const uint16_t x2, const uint16_t y2, const uint16_t z2);
    float Distance(const float x1, const float y1, const float z1, const float x2, const float y2,
                   const float z2);
    float Distance(const float x1, const float y1, const float x2, const float y2);

    void InitRandom(uint32_t nInitialisationValue);
    // u16 value between MinVal and MaxVal (inclusive) with max range 32768
    uint16_t GetRandom(uint16_t MaxVal, uint16_t MinVal = 0);
    float GetRandomFloat(); // f32 value between 0 and 1
}
