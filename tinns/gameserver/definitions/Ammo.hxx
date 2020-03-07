#pragma once

#include <cstdint>
#include "gameserver/definitions/Definition.hxx"

class PDefAmmo : public PDef {
private:
    //int32_t mIndex;
	int32_t mDamageId; // related to damage.def
	int32_t mWeaponShotId; // related to shots.def ?
	int32_t mMagSize;
    int32_t mShotId; // related to shots.def ??? strange: hardly ever set in ammo.def

public:
    PDefAmmo();
    //~PDefAmmo();

    bool LoadFromDef( PTokenList *Tokens );

    inline int32_t GetDamageId() const { return mDamageId; }
	inline int32_t GetWeaponShotId() const { return mWeaponShotId; }
    inline int32_t GetMagSize() const { return mMagSize; }
	inline int32_t GetShotId() const { return mShotId; }
};
