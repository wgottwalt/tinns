#include <cstdarg>
#include "common/Includes.hxx"

namespace Misc
{
    namespace Net
    {
        uint32_t ip4StringToUint32(const std::string &ip)
        {
            if (!ip.empty())
            {
                std::string tmp = "";
                uint32_t result = 0;

                for (auto chr : ip)
                {
                    switch (chr)
                    {
                        case '.':
                            result <<= 8;
                            result |= std::stoul(tmp);
                            tmp.clear();
                            continue;
                        default:
                            tmp += chr;
                    }
                }
                result <<= 8;
                result |= std::stoul(tmp);

                return result;
            }

            return 0;
        }

        const std::string uint32ToIp4String(const uint32_t ip)
        {
            const IPv4 nip = {.val = ip};
            std::string result = "0.0.0.0";

            if (Endian::Order::Native == Endian::Order::Little)
                result = std::to_string(nip.d) + "." + std::to_string(nip.c) + "." +
                         std::to_string(nip.b) + "." + std::to_string(nip.a);
            else if (Endian::Order::Native == Endian::Order::Big)
                result = std::to_string(nip.a) + "." + std::to_string(nip.b) + "." +
                         std::to_string(nip.c) + "." + std::to_string(nip.d);

            return result;
        }
    }

    namespace String
    {
        const std::string accessLevelToString(const int32_t level)
        {
            switch (level)
            {
                case 0:
                    return "Unregistered";
                case 1:
                    return "Registered";
                case 30:
                    return "Volunteer";
                case 50:
                    return "Gamemaster";
                case 100:
                    return "Administrator";
                default:
                    return "Error";
            }
        }

        void cleanUpString(std::string &str)
        {
            if (str.length() > 3)
            {
                auto pos = str.find('\\');

                while (pos != std::string::npos)
                {
                    str.replace(pos, 1, " ");
                    pos = str.find('\\', pos + 1);
                }

                if (str == "   ")
                    str.clear();
                else
                    str = trim(str);
            }
            else
                str.clear();
        }

        const std::string trim(const std::string &data, const char delim, const bool at_start,
                               const bool at_end)
        {
            std::string tmp(data);

            if (at_start)
                while (!tmp.empty() && (*(tmp.begin())) == delim)
                    tmp.erase(tmp.begin(), tmp.begin() + 1);
            if (at_end)
                while (!tmp.empty() && (*(tmp.end() - 1)) == delim)
                    tmp.erase(tmp.end() - 1, tmp.end());

            return tmp;
        }

        const std::string trim(const std::string &data, const bool at_start, const bool at_end)
        {
            std::string tmp(data);

            if (at_start)
                while (!tmp.empty() && std::isspace(*(tmp.begin())))
                    tmp.erase(tmp.begin(), tmp.begin() + 1);
            if (at_end)
                while (!tmp.empty() && std::isspace(*(tmp.end() - 1)))
                    tmp.erase(tmp.end() - 1, tmp.end());

            return tmp;
        }
    }
}
