#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

static const int32_t NUMFACTIONS = 20;  // for faction relations only

class PDefFaction : public PDef {
private :
    //int32_t mIndex;
    std::string mName;
    int32_t mStartValue;
    bool mAffected;
    int32_t mSL;
    int32_t mRelations[NUMFACTIONS];
public :
    PDefFaction();
    //~PDefFaction();

    bool LoadFromDef(PTokenList *Tokens);

    inline const std::string &GetName() const { return mName; }
    inline int32_t GetStartValue() const { return mStartValue; }
    inline bool GetAffected() const { return mAffected; }
    inline int32_t GetSL() const { return mSL; };
    int32_t GetRelation(int32_t Faction) const;
};
