#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefWeapon : public PDef {
private:
    //int32_t mIndex;
    std::string mName;
    //int32_t mFpsmodel;
    //int32_t mAttachmodel;
    //int32_t mMunactor;
    //float mDroptime; // useful ?
    int32_t mItemIndex;
    //int32_t effectcolor[3]; // R,G,B
    //int32_t dynamiclight;
    //float lighttime;
    //int32_t soundindex;
    //float mShotTime; // useful ?
    int32_t mAmmoUse;
    int32_t mPSIuse;
    float mStaminaUse;
    //float upthrow;
    //int32_t weaponHold; // ???
    int32_t mWeaponType;
    int32_t mDiscardable;
    float mSkillFactor;
    //int32_t mint32_tgtRad;
    //int32_t maxtgtRad;
    int32_t mMaxRange;
    int32_t mAggressiveWeapon;
    float mDamageMultiplicator;
    int32_t mAmmoTypes[8];
    int32_t mAmmoStartFlags; // ???
    //int32_t customclasstype; // ?
    //int32_t unknown // ?
    int32_t mShotCnt;
    //float shotduration; // maybe useful later ?
    //std::string shotfx;
    //float attachposx;
    //float attachposy;
    //float attachposz;
    //float fpposx;
    //float fpposy;
    //float fpposz;
    int32_t mBaseWeaponId;
    //int32_t weaponcolor;
    //int32_t reloadsound;
    int32_t mItemModGroup;
    int32_t mItemModGroupFlags;
    int32_t mRareWeaponFx;

public:
    PDefWeapon();
    //~PDefWeapon();

    bool LoadFromDef( PTokenList *Tokens );

    inline const std::string &GetName() const { return mName; }
    inline int32_t GetItemID() const { return mItemIndex; }
    inline int32_t GetAmmoUse() const { return mAmmoUse; }
    inline int32_t GetPsiUse() const { return mPSIuse; }
    inline float GetStaminaUse() const { return mStaminaUse; }
    inline int32_t GetWeaponType() const { return mWeaponType; }
    inline bool IsDiscardable() const { return mDiscardable; }
    inline float GetSkillFactor() const { return mSkillFactor; }
    inline int32_t GetMaxRange() const { return mMaxRange; }
    inline bool IsAggressiveWeapon() const { return mAggressiveWeapon; }
    inline float GetDamageMultiplicator() const { return mDamageMultiplicator; }
    inline int32_t GetAmmoType(int32_t nIdx) const { return ( ((nIdx >= 0) && (nIdx < 8)) ? mAmmoTypes[nIdx] : 0) ; }
    inline int32_t GetAmmoStartFlags() const { return mAmmoStartFlags; }
    inline int32_t GetShotCnt() const { return mShotCnt; }
    inline int32_t GetBaseWeaponId() const { return mBaseWeaponId; }
    inline int32_t GetItemModGroup() const { return mItemModGroup; }
    inline int32_t GetItemModGroupFlags() const { return mItemModGroupFlags; }
    inline int32_t GetRareWeaponFx() const { return mRareWeaponFx; }
};
