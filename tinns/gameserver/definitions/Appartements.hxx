#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"
#include "gameserver/definitions/Map.hxx"

class PDefAppartement : public PDef {
private:
    //int32_t mIndex;
    std::string mName;
    std::string mWorldName;
    int32_t mValue;
    int32_t mPlaceCount;
    int32_t mPlace[8];
    int32_t mFaction;

public:
    PDefAppartement();
    //~PDefAppartement();

    bool LoadFromDef(PTokenList *Tokens);

    inline int32_t GetID() const { return mIndex; }
    inline const std::string &GetName() const { return mName; }
    inline const std::string &GetWorldName() const { return mWorldName; }
    inline int32_t GetValue() const { return mValue; }
    inline int32_t GetPlaceCount() const { return mPlaceCount; }
    inline int32_t GetPlace(int32_t nIdx) const { return ( (nIdx < mPlaceCount) ? mPlace[nIdx] : 0 ); }
    inline int32_t GetFaction() const { return mFaction; }
};

class PDefAppartementsMap : public PDefMap<PDefAppartement> {
public:
    inline std::map<int32_t, PDefAppartement*>::const_iterator ConstIteratorBegin() const { return mDefs.begin(); }
    inline std::map<int32_t, PDefAppartement*>::const_iterator ConstIteratorEnd() const { return mDefs.end(); }
};
