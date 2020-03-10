#pragma once

#include <sstream>
#include <string>
#include <tuple>

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
        const std::string trim(const std::string &data, const char delim,
                               const bool at_start = true, const bool at_end = true);
        const std::string trim(const std::string &data, const bool at_start = true,
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

    namespace Math
    {
        template <typename T>
        T distanceApprox(const T x1, const T y1, const T z1, const T x2, const T y2, const T z2)
        {
            const T dx = (x1 >= x2) ? (x1 - x2) : (x2 - x1);
            const T dy = (y1 >= y2) ? (y1 - y2) : (y2 - y1);
            const T dz = (z1 >= z2) ? (z1 - z2) : (z2 - z1);
            T dmax = dx;
            uint32_t dminsum = dy;
            uint32_t dapprox = 0;

            if (dx < dy)
            {
                dmax = dy;
                dminsum = dx;
            }

            if (dmax < dz)
            {
                dminsum += dmax;
                dmax = dz;
            }
            else
                dminsum += dz;

            dapprox = static_cast<uint32_t>(dmax) + (dminsum / 3);
            if (dapprox > 65535)
                dapprox = 65535;

            return static_cast<T>(dapprox);
        }
    }

    void InitRandom(uint32_t nInitialisationValue);
    // u16 value between MinVal and MaxVal (inclusive) with max range 32768
    uint16_t GetRandom(uint16_t MaxVal, uint16_t MinVal = 0);
    float GetRandomFloat(); // f32 value between 0 and 1
}
