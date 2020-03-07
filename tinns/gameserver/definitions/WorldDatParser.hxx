#pragma once

#include <cstdint>
#include <string>

class PFile;
class PWorldDataTemplate;

class PWorldDatParser {
private:
    PFile* f;
    std::string mNCDataPath;

    PWorldDataTemplate* mWorld;
    bool mDiscardPassiveObjects;

    bool ProcessSec2ElemType3(uint32_t nSize);
    bool ProcessSec2ElemType5(uint32_t nSize);
    bool ProcessSec2NPCEntry(uint32_t nSize);

public:
    PWorldDatParser();
    ~PWorldDatParser();

    int32_t LoadDatFile(const std::string& nFilename, PWorldDataTemplate* nWorld, const bool nDiscardPassiveObjects = true, const bool nTestAccesOnly = false);
};
