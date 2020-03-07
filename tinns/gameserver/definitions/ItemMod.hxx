#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefItemMod : public PDef {
private:
    //int32_t mIndex;
    int32_t mType; // if mType > 0, it refers to a weapon type from weapons.def +1
    //int32_t mDuration; // "implant base build time" = ???? always 0 anyway...
    int32_t mChangeNum;
    int32_t mChangeTarget[4]; // qualifier id   0 - 7   item qualifier id   4=range
    float mChangeValue[4];
    float mChangeScale[4];
    std::string mName;
    //int32_t mIconId;

public:
    PDefItemMod();
    //~PDefItemMod();

    bool LoadFromDef( PTokenList *Tokens );

    inline const std::string &GetName() const { return mName; }
    inline int32_t GetType() const { return mType; }
    //inline int32_t GetDuration() const { return mDuration; }
    inline int32_t GetChangeNum() const { return mChangeNum; }
    inline int32_t GetChangeTarget(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mChangeNum)) ? mChangeTarget[nIdx] : 0) ; }
    inline float GetChangeValue(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mChangeNum)) ? mChangeValue[nIdx] : 0) ; }
    inline float GetChangeScale(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mChangeNum)) ? mChangeScale[nIdx] : 0) ; }
};
