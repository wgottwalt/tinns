#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpPing : public PUdpMsgAnalyser {
private:
    uint32_t mClientTime;

public:
    PUdpPing(PMsgDecodeData *nDecodeData);
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
