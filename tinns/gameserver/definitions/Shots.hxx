#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefShot : public PDef {
private:
    //int32_t mIndex;
    int32_t mDamageId;
    int32_t mMass;
    int32_t mRadius;
    float mSpeed;
    //int32_t mHitSound;
    //int32_t mCustomType;
    //std::string mCustomFrameFx;
    //std::string mCustomHitFx;

public:
    PDefShot();
    //~PDefShot();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetDamageId() const { return mDamageId; }
    inline int32_t GetMass() const { return mMass; }
    inline int32_t GetRadius() const { return mRadius; }
    inline int32_t GetSpeed() const { return mSpeed; }
};
