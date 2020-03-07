#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefWorldModel : public PDef {
private:
    //int32_t mIndex;
    std::string mName;
    int32_t mUseFlags;
    int32_t mFunctionType;
    int32_t mFunctionValue;
    int32_t mHackDifficulty;
    int32_t mHackPenalty;

public:
    PDefWorldModel();
    //~PDefWorldModel();

    bool LoadFromDef(PTokenList *Tokens);

    inline int32_t GetID() const { return mIndex; }
    inline const std::string &GetName() const { return mName; }
    inline int32_t GetUseFlags() const { return mUseFlags; }
    inline int32_t GetFunctionType() const { return mFunctionType; }
    inline int32_t GetFunctionValue() const { return mFunctionValue; }
    inline int32_t GetHackDifficulty() const { return mHackDifficulty; }
    inline int32_t GetHackPenalty() const { return mHackPenalty; }
};
