#pragma once

#include <cstdint>
#include <map>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefScripts : public PDef {
private:
    //int32_t mIndex;
    std::string mIdentifier;
    std::string mLuaFile;
    std::string mScriptHeader;

public:
    PDefScripts();
    //~PDefWeapon();

    bool LoadFromDef( PTokenList *Tokens );

    inline const std::string &GetIdentifier() const { return mIdentifier; }
    inline const std::string &GetLuaFile() const { return mLuaFile; }
    inline const std::string &GetScriptHeader() const { return mScriptHeader; }
};

class PDefScriptsMap : public PDefMap<PDefScripts> {
public:
    //bool Load(const char* nName, const char* nFilename);
    inline std::map<int32_t, PDefScripts*>::const_iterator ConstIteratorBegin() const { return mDefs.begin(); }
    inline std::map<int32_t, PDefScripts*>::const_iterator ConstIteratorEnd() const { return mDefs.end(); }
};
