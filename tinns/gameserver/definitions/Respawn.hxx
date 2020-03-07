#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefRespawn : public PDef {
private:
    //int32_t mIndex;
    int32_t mWorldID;
    int32_t mEntityID; // Station ID
    int32_t mHazardLevel;
    std::string mName; // Description
    std::string mFlag; // ???

public:
    PDefRespawn();
    //~PDefRespawn();

    bool LoadFromDef(PTokenList *Tokens);

    inline int32_t GetWorldID() const { return mWorldID; }
    inline int32_t GetEntityID() const { return mEntityID; }
    inline int32_t GetHazardLevel() const { return mHazardLevel; }
    inline const std::string &GetName() const { return mName; }
    inline const std::string &GetFlag() const { return mFlag; }
};

class PDefRespawnsMap : public PDefMap<PDefRespawn> {
public:
    int32_t GetRespawnEntity( uint32_t nWorldID, uint16_t nGROrder ) const;
};
