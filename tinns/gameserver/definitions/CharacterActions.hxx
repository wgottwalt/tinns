#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefCharAction : public PDef {
private:
    //int32_t mIndex;
    int32_t mNumOfSsq;
    int32_t mSsqId[8];
    float mModFactor[8];

public:
    PDefCharAction();
    //~PDefCharAction();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetNumOfSsq() const { return mNumOfSsq; }
    inline int32_t GetSsqId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumOfSsq)) ? mSsqId[nIdx] : 0) ; }
    inline int32_t GetModFactor(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumOfSsq)) ? mModFactor[nIdx] : 0) ; }
};
