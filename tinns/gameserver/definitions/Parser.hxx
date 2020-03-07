#pragma once

#include <list>
#include <string>

typedef std::list<std::string> PTokenList;
typedef std::list<PTokenList *> PDefTokenList;

class PDefParser {
private:
    PDefTokenList mTokens;

public:
    PDefParser();
    ~PDefParser();
    bool Parse(const char *File);
    inline const PDefTokenList &GetTokens() const { return mTokens; }
};
