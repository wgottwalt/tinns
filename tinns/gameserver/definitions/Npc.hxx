#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefNpc : public PDef {
private:
    //int32_t mIndex;
    int32_t mModel; // <0: from characters.def, >0: from models.ini, [players] section (?)
    //int32_t mSoundId; // always 0: not used
    int32_t mNGT; // ? 1 - 4
    int32_t mGender; // 0 - 1
    int32_t mFaction;
    int32_t mHealth;
    int32_t mArmorId; // -1 or ref to NpcArmor
    int32_t mWeaponId;
    std::string mDialogScript;
    int32_t mCombat; // ? 20 - 800
    int32_t mLoot; // 0 or ref to ItemContained
    int32_t mMovementEnd; // 0 - 4
    int32_t mFunctionType; // 0 - 4
    float mModelScaling; // 0 - 0.9
    int32_t mMoneyLoose; // 0 - 190
    float mSkillScale; // 0 - 120
    std::string mStandardScript;
    std::string mStandardParameter;
    int32_t mMass; // 1 - 10000
    //int32_t mStartHour; // not used
    //int32_t mEndHour; // not used
    //std::string mTempScript; // not used
    int32_t mFlags; // values: 513, 259, 256, 128, 64, 48, 35, 34, 33, 32, 2, 1, 0

public:
    PDefNpc();
    //~PDefNpc();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetModel() const { return mModel; }
    inline int32_t GetNGT() const { return mNGT; }
    inline int32_t GetGender() const { return mGender; }
    inline int32_t GetFaction() const { return mFaction; }
    inline int32_t GetHealth() const { return mHealth; }
    inline int32_t GetArmorId() const { return mArmorId; }
    inline int32_t GetWeaponId() const { return mWeaponId; }
    inline const std::string &GetDialogScript() const { return mDialogScript; }
    inline int32_t GetCombat() const { return mCombat; }
    inline int32_t GetLoot() const { return mLoot; }
    inline int32_t GetMovementEnd() const { return mMovementEnd; }
    inline int32_t GetFunctionType() const { return mFunctionType; }
    inline float GetModelScaling() const { return mModelScaling; }
    inline int32_t GetMoneyLoose() const { return mMoneyLoose; }
    inline float GetSkillScale() const { return mSkillScale; }
    inline const std::string &GetStandardScript() const { return mStandardScript; }
    inline const std::string &GetStandardParameter() const { return mStandardParameter; }
    inline int32_t GetMass() const { return mMass; }
    inline int32_t GetFlags() const { return mFlags; }
};
