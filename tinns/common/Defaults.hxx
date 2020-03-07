#pragma once

#include <string>
#include "common/Time.hxx"

namespace DefMain
{
    static const std::string Name = "TinNS";
    static const std::string Version = "0.3 C++17 development";
    static const std::string Revision = Time::toDateString(Time::now()).c_str();
}
