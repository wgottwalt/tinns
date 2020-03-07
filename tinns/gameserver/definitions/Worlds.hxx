#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefWorld : public PDef {
private:
    //int32_t mIndex;
    std::string mName;
    std::string mDatFile;
    int32_t mFlags;

public:
    PDefWorld();
    //~PDefWorld();

    bool LoadFromDef(PTokenList *Tokens);

    inline const std::string &GetName() const { return mName; }
    inline const std::string &GetDatFile() const { return mDatFile; }
    inline int32_t GetFlags() const { return mFlags; }
};
