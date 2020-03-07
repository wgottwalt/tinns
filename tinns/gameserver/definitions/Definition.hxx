#pragma once

#include <cstdint>
#include <list>
#include <string>

typedef std::list<std::string> PTokenList;

/* mother class for PDef* classes */
/* provides default members required for def file loading */
class PDef {
protected:
    int32_t mIndex;

public:
    PDef();
    virtual ~PDef();

    bool LoadFromDef(PTokenList *Tokens);

    int32_t GetIndex() const;
    const std::string &GetName() const;
};
