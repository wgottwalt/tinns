#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefItemRestriction : public PDef {
private:
    //int32_t mIndex;
    int32_t mNumRestrictions;
    int32_t mSsqId[6]; // Skill/Subs
    int32_t mMinValue[6];

public:
    PDefItemRestriction();
    //~PDefItemRestriction();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetNumRestrictions() const { return mNumRestrictions; }
    inline int32_t GetSsqId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumRestrictions)) ? mSsqId[nIdx] : 0) ; }
    inline float GetMinValue(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumRestrictions)) ? mMinValue[nIdx] : 0) ; }
};
