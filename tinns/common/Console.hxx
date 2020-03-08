#pragma once

#include <chrono>
#include <fstream>

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
private:
	std::ofstream mLogFile;
	time_t mLastLogTime;

public:
	PConsole(const char *nLogFile);
	~PConsole();
	void Print(const char *Fmt_, ...);
	void Print(COLORS foreground, COLORS background, const char *Fmt_, ...);
	char *ColorText(COLORS foreground, COLORS background, const char *Fmt, ...);

	void LPrint(const char *Fmt_, ...);
	void LPrint(COLORS foreground, COLORS background, const char *Fmt_, ...);
	void LClose();
	void Update();
};

extern class PConsole *Console;
