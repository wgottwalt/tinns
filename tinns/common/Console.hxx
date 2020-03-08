#pragma once

#include <chrono>
#include <fstream>
#include <string>

enum COLORS : uint8_t {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};

class PConsole {
public:
    //--- public constructors ---
	PConsole(const std::string &logfile);
    PConsole(const PConsole &rhs) = delete;
    PConsole(PConsole &&rhs) = delete;
	~PConsole();

    //--- public operators ---
    PConsole &operator=(const PConsole &rhs) = delete;
    PConsole &operator=(PConsole &&rhs) = delete;

    //--- public methods ---
	void Print(const char *Fmt_, ...);
	void Print(COLORS foreground, COLORS background, const char *Fmt_, ...);
	char *ColorText(COLORS foreground, COLORS background, const char *Fmt, ...);

	void LPrint(const char *Fmt_, ...);
	void LPrint(COLORS foreground, COLORS background, const char *Fmt_, ...);
	void LClose();
	void Update();

protected:
    //--- protected methods ---
    const std::string color(const COLORS fg, const COLORS bg) const;
    const std::string reset() const;

private:
    //--- private properties ---
    std::ofstream _logfile;
    time_t _lastlogtime;
};

extern class PConsole *Console;
