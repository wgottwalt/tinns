#pragma once

#include <cstdint>
#include <map>
#include <string>
#include "gameserver/definitions/Definition.hxx"

class PDefWorldFile : public PDef {
private:
    //int32_t mIndex;
    std::string mName; // dat filename with ending extension and starting ./ or ./worlds/ REMOVED
    bool mFileInWorldsDir; // TRUE if worlds/ must be appendend before mName to get real file name (dat file at least)

public:
    PDefWorldFile();
    //~PDefWorldFile();

    bool LoadFromDef(PTokenList *Tokens);

    inline const std::string &GetName() const { return mName; }
    inline const std::string GetBasicFileName() const { return (mFileInWorldsDir ? (std::string("worlds/") + mName) : mName); };
};


class PDefWorldFilesMap : public PDefMap<PDefWorldFile> {
public:
    bool Load(const char* nName, const char* nFilename);
    inline std::map<int32_t, PDefWorldFile*>::const_iterator ConstIteratorBegin() const { return mDefs.begin(); }
    inline std::map<int32_t, PDefWorldFile*>::const_iterator ConstIteratorEnd() const { return mDefs.end(); }
};
