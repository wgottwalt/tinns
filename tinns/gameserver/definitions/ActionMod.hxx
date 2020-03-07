#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefActionMod : public PDef {
private:
    //int mIndex;
    float mStartValue;
    int32_t mNumOfSsq;
    int32_t mSsqId[8];
    float mModFactor[8];

public:
    PDefActionMod();
    //~PDefActionMod();

    bool LoadFromDef(PTokenList *Tokens);

    inline float GetStartValue() const { return mStartValue; }
    inline int32_t GetNumOfSsq() const { return mNumOfSsq; }
    inline int32_t GetSsqId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumOfSsq)) ? mSsqId[nIdx] : 0) ; }
	inline float GetModFactor(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumOfSsq)) ? mModFactor[nIdx] : 0) ; }
};
