#pragma once

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <type_traits>
#include "Misc.hxx"
#include "Time.hxx"

namespace Logging
{
    //--- basic types ---
    enum class Color : uint8_t {
        Black = 0,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White
    };

    class ToCout {
    protected:
        //--- protected methods ---
        std::ostream &out()
        {
            return std::cout;
        }
    };

    class ToCerr {
    protected:
        //--- protected methods ---
        std::ostream &err()
        {
            return std::cerr;
        }
    };

    template <typename OutputPolicy = ToCout, typename ErrorPolicy = ToCerr>
    class Log : protected OutputPolicy, protected ErrorPolicy {
    public:
        //--- public constructors ---
        Log(const std::string &filename = "")
        : OutputPolicy(), ErrorPolicy(), _mutex(), _cv(), _marks_thread(&Log::periodicMark, this),
          _logfile(), _mark_duration(std::chrono::minutes(15)), _use_logfile(true),
          _use_console(true), _use_timestamps(true), _use_marks(true)
        {
            if (_use_logfile)
            {
                if (!filename.empty())
                {
                    if (_logfile.open(filename); !_logfile.is_open() || !_logfile.good())
                    {
                        _logfile.close();
                        _use_logfile = false;
                        ErrorPolicy::err() << "error: unable to open logfile '" << filename << "'"
                                           << std::endl;
                    }
                }
                else
                    _use_logfile = false;
            }

            print("Log() :: marks duration = ", Time::sec(_mark_duration), " second(s)");
        }

        Log(const Log &rhs) = delete;
        Log(Log &&rhs) = delete;

        ~Log()
        {
            print("Log() :: shutdown");

            if (_use_marks)
            {
                _use_marks = false;
                _cv.notify_one();
                _marks_thread.join();
            }

            if (_use_logfile)
            {
                if (_logfile.is_open() && _logfile.good())
                {
                    _logfile.flush();
                    _logfile.close();
                }
            }
        }

        //--- public operators ---
        Log &operator=(const Log &rhs) = delete;
        Log &operator=(Log &&rhs) = delete;

        //--- public methods ---
        void setPeriodicMarksDuration(const Time::Duration &duration)
        {
            print("Log() :: changed marks duration");
            _mark_duration = duration;
        }

        Time::Duration periodicMarksDuration() const
        {
            return _mark_duration;
        }

        void setUseLogToFile(const bool use = true)
        {
            _use_logfile = use;
            if (use)
                print("Log() :: switched logging to file on");
            else
                print("Log() :: switched logging to file off");
        }

        bool useLogToFile() const
        {
            return _use_logfile;
        }

        void setUseLogToConsole(const bool use = true)
        {
            _use_console = true;
            if (use)
                print("Log() :: switched logging to console on");
            else
                print("Log() :: switched logging to console off");
        }

        bool useLogToConsole() const
        {
            return _use_console;
        }

        void setUsePeriodicMarks(const bool use = true)
        {
            _use_marks = use;
            if (!use)
            {
                _cv.notify_one();
                _marks_thread.join();
                print("Log() :: stopped periodic marks");
            }
            else
            {
                _marks_thread = std::thread(&Log::periodicMark, this);
                print("Log() :: restarted periodic marks");
            }
        }

        bool usePeriodicMarks() const
        {
            return _use_marks;
        }

        void setUseTimestamps(const bool use = true)
        {
            _use_timestamps = use;
            if (use)
                print("Log() :: switched use of timestamps on");
            else
                print("Log() :: switched use of timestamps off");
        }

        bool useTimestamps() const
        {
            return _use_timestamps;
        }

        template <typename... T>
        void print(const T &...tail)
        {
            if (_use_logfile || _use_console)
            {
                const std::string text(Misc::String::create(tail...));
                const std::string date(Time::toDateString(Time::now()));

                if (_use_console)
                {
                    if (_use_timestamps)
                        OutputPolicy::out() << '[' << date << "] " << text << std::endl;
                    else
                        OutputPolicy::out() << text << std::endl;
                }
                if (_use_logfile)
                {
                    if (_use_timestamps)
                        _logfile << '[' << date << "] " << text << '\n';
                    else
                        _logfile << text << '\n';
                }
            }
        }

    protected:
        //--- protected methods ---
        const std::string color(const Color fg, const Color bg) const
        {
            return "\x1B[0;" +
                   std::to_string(static_cast<std::underlying_type<Color>::type>(fg) + 30) + ";" +
                   std::to_string(static_cast<std::underlying_type<Color>::type>(bg) + 40) + "m";
        }

        const std::string resetColor() const
        {
            return "\x1B[0;37;40m";
        }

        void periodicMark()
        {
            std::unique_lock lock(_mutex);

            _cv.wait_for(lock, _mark_duration);
            while (_use_marks)
            {
                print("--- MARK ---");
                _cv.wait_for(lock, _mark_duration);
            }
        }

    private:
        //--- private properties ---
        std::mutex _mutex;
        std::condition_variable _cv;
        std::thread _marks_thread;
        std::ofstream _logfile;
        Time::Duration _mark_duration;
        bool _use_logfile;
        bool _use_console;
        bool _use_timestamps;
        bool _use_marks;
    };
}
