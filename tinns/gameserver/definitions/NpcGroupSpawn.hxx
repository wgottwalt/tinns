#pragma once

#include <cstdint>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefNpcGroupSpawn : public PDef {
private:
    //int32_t mIndex;
    int32_t mIgnoreNearPC;
    int32_t mNumNpc; // Size of the list. But that should be the size of the group...
    int32_t mNpcType[8];
    std::string mScript[8];
    std::string mScriptParameter[8];
    int32_t mFunctionValue[8];
    int32_t mSpawnChance[8];

public:
    PDefNpcGroupSpawn();
    //~PDefNpcGroupSpawn();

    bool LoadFromDef( PTokenList *Tokens );

    int32_t GetIgnoreNearPC() const;
    int32_t GetNumNpc() const;
    int32_t GetNpcType(int32_t nIdx) const;
    const std::string& GetScript(int32_t nIdx) const;
    const std::string& GetScriptParameter(int32_t nIdx) const;
    int32_t GetFunctionValue(int32_t nIdx) const;
    int32_t GetSpawnChance(int32_t nIdx) const;
};
