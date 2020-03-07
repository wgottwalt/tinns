#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefSubSkill : public PDef {
private:
    //int32_t mIndex;
    std::string mName;
    std::string mShortName;
    float mStrengthenFactor;
    int32_t mNumActionModifiers;
    int32_t *mActionModifiers;

public:
    PDefSubSkill();
    ~PDefSubSkill();

    bool LoadFromDef(PTokenList *Tokens);

    inline const std::string &GetName() const { return mName; }
    inline const std::string &GetShortName() const { return mShortName; }
    inline float GetStrengthenFactor() const { return mStrengthenFactor; }
    inline int32_t GetNumActionModifiers() const { return mNumActionModifiers; }
    inline int32_t GetActionModifier(int32_t Index) const { return mActionModifiers[Index]; }
};
