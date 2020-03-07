#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpDeathRespawn : public PUdpMsgAnalyser {
private:
    uint32_t mEntity;
    uint32_t mWorldId;

public:
    PUdpDeathRespawn(PMsgDecodeData *nDecodeData);
    //~PUdpDeathRespawn();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
