#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefOutpost : public PDef {
private:
    //int32_t mIndex;
    std::string mName;
    int32_t mType;
    int32_t mStandardFaction;
    float mRevenue;
    float mConquestReward;
    int32_t mMaxSecurity;
    int32_t mInfluenceZone[8]; // ex: 12 for zone A12, 112 for B12

public:
    PDefOutpost();
    //~PDefOutpost();

    bool LoadFromDef( PTokenList *Tokens );

    inline const std::string &GetName() const { return mName; }
    inline int32_t GetType() const { return mType; }
    inline int32_t GetStandardFaction() const { return mStandardFaction; }
    inline float GetRevenue() const { return mRevenue; }
    inline float GetConquestReward() const { return mConquestReward; }
    inline int32_t GetMaxSecurity() const { return mMaxSecurity; }
    inline int32_t GetInfluenceZone(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < 8)) ? mInfluenceZone[nIdx] : 0) ; }
};
