#pragma once

#include <cstdint>
#include <map>
#include <string>
#include "gameserver/definitions/Definition.hxx"

struct PSkillInfo {
    int32_t mStart;
    int32_t mMax;
    int32_t mGrow;

    inline PSkillInfo()
    {
        mStart = mMax = mGrow = 0;
    }
};

struct PSkillPtsInfo {
    int32_t mSkill;
    int32_t mPoints;

    inline PSkillPtsInfo()
    {
        mSkill = mPoints = 0;
    }
};

struct PSubSkillPtsInfo {
    int32_t mSubSkill;
    int32_t mPoints;

    inline PSubSkillPtsInfo()
    {
        mSubSkill = mPoints = 0;
    }
};

struct PStartLevelInfo {
    int32_t mSubSkill;
    int32_t mLevel;

    inline PStartLevelInfo()
    {
        mSubSkill = mLevel = 0;
    }

};

class PDefCharKind : public PDef {
private:
    typedef std::map<int32_t, PSkillPtsInfo*> PSkillPtsMap;
    typedef std::map<int32_t, PSubSkillPtsInfo*> PSubSkillPtsMap;
    typedef std::map<int32_t, PStartLevelInfo*> PStartLevelMap;

    //int32_t mIndex;
    std::string mName;
    int32_t mType;

    // TODO: shouldnt this be a map?
    PSkillInfo *mSkillInfo;

    PSkillPtsMap mSkillPts;
    PSubSkillPtsMap mSubSkillPts;
    PStartLevelMap mStartLevels;
    int32_t mMoney;
    uint32_t mInventory[8];

public:
    PDefCharKind();
    ~PDefCharKind();

    bool LoadFromDef(PTokenList *Tokens);

    inline const std::string &GetName() const { return mName; }
    inline int32_t GetType() const { return mType; }
    inline const PSkillInfo &GetSkillInfo(int32_t Skill) const { return mSkillInfo[Skill-1]; }

    inline int32_t GetStartMoney() const { return mMoney; }
    inline uint32_t GetStartInventory(uint8_t Index) const { return ((Index < 7) ?  mInventory[Index] : 0); }
    // TODO: mission get() functions
};
