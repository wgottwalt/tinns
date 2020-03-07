#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefSkill : public PDef {
private:
    //int32_t mIndex;
    std::string mName;
    std::string mShortName;
    int32_t mNumSubSkills;
    int32_t *mSubSkills;
public:
    PDefSkill();
    ~PDefSkill();

    bool LoadFromDef(PTokenList *Tokens);

    inline const std::string &GetName() const { return mName; }
    inline const std::string &GetShortName() const { return mShortName; }
    inline int32_t GetNumSubSkills() const { return mNumSubSkills; }
    inline int32_t GetSubSkill(int32_t Index) const { return mSubSkills[Index]; }
};
