#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefRecycle : public PDef {
private:
    //int32_t mIndex;
    int32_t mResultItemId; // We use resultitem as mIndex
    int32_t mBuildTime;
    int32_t mNumParts;
    int32_t mPartId[8];

public:
    PDefRecycle();
    //~PDefRecycle();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetResultItemId() const { return mResultItemId; }
    inline int32_t GetBuildTime() const { return mBuildTime; }
    inline int32_t GetNumParts() const { return mNumParts; }
    inline int32_t GetPartId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumParts)) ? mPartId[nIdx] : 0 ) ; }
};
