#pragma once

#include <cstdint>
#include <string>

class PChar;

class PAppartements {
public:
    PAppartements();
    //~PAppartements();

    uint32_t CreateBaseAppartement(uint32_t nCharID, std::string nPassword, uint8_t nFactionID);
    void DeleteCharAppartements(uint32_t nCharID);
    bool CanFreelyEnter(PChar* nChar, uint32_t nLocation);
    int GetAptID(unsigned int AptLoc, const uint8_t *pass);
    int GetAptType(int AptID);
    int GetAptOwner(int loc);
    int GetAptLocation(int loc);
};
