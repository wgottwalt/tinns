#pragma once

#include <cstdint>

#ifndef _PCRE_H
#include "pcre.h"
#endif

class RegEx {
private:
    pcre *re;
    pcre_extra *pe;
    int32_t substrcount;
    int32_t *ovector;
    const char *lastsubject;
    int32_t slen;
    const char **matchlist;

    inline void ClearMatchList(void)
    {
        if (matchlist)
        {
            pcre_free_substring_list(matchlist);
            matchlist = nullptr;
        }
    }

public:
    RegEx(const char *regex, int32_t options = 0);
    ~RegEx();
    inline int32_t SubStrings(void) const { return substrcount; }
    bool Search(const char *subject, int32_t len = -1, int32_t options = 0);
    bool SearchAgain(int32_t options = 0);
    const char *Match(int32_t i = 1);
};
