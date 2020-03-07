#pragma once

#include <cstdint>

class POutpost {
public:
    POutpost();
    ~POutpost();

    bool IsZoneOPArea(uint32_t nZoneID);     // checks if given zoneID is either outpost zone or underground of OP
    void SendOPAreaData(PClient* nClient); // Sends OP data to client (Owning clan/faction)
    uint32_t GetCurrentClan(uint32_t nOutpostID); // Get current ClanID of given Outpost
};
