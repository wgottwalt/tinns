#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefItemContainer : public PDef {
private:
    //int32_t mIndex;
    int32_t mNumItemsAtOnce;
    int32_t mRespawnTime;
    int32_t mNumItems;
    int32_t mItemId[6]; // <0: Group, >0: Item
    float mQuality[6]; // usually <1, <=> % ?
    int32_t mChance[6]; // a weight, not a %

    int32_t mCumulatedChance[6]; // Computed data in order to select random item entry

    void BuildCumulatedChance();

public:
    PDefItemContainer();
    //~PDefItemContainer();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetNumItemsAtOnce() const { return mNumItemsAtOnce; }
    inline int32_t GetRespawnTime() const { return mRespawnTime; }
    inline int32_t GetNumItems() const { return mNumItems; }
    inline int32_t GetItemId(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumItems)) ? mItemId[nIdx] : 0) ; }
    inline float GetQuality(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumItems)) ? mQuality[nIdx] : 0) ; }
    inline int32_t GetChance(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumItems)) ? mChance[nIdx] : 0) ; }
    int32_t GetRandomItemIdx() const; // Return Idx based on Entry chance
};
