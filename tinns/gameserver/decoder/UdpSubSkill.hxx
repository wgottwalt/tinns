#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpSubskillInc : public PUdpMsgAnalyser {
private:
    uint16_t SubskillID;

public:
    PUdpSubskillInc(PMsgDecodeData *nDecodeData);
    //~PUdpSubskillInc();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
