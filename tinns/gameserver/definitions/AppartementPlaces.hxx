#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefAppPlace : public PDef {
private :
    //int32_t mIndex;
    std::string mName;
    int32_t mExitWorldID;
    int32_t mExitWorldEntity;
    int32_t mSewerLevel;

public :
    PDefAppPlace();
    //~PDefAppPlace();

    bool LoadFromDef(PTokenList *Tokens);

    inline const std::string &GetName() const { return mName; }
    inline int32_t GetExitWorldID() const { return mExitWorldID; }
    inline int32_t GetExitWorldEntity() const { return mExitWorldEntity; }
    inline int32_t GetSewerLevel() const { return mSewerLevel; }
};
