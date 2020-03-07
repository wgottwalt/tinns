#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpEntityPosRequest : public PUdpMsgAnalyser {
private:
    uint16_t mEntityID; // u16 or u32 ???

public:
    PUdpEntityPosRequest(PMsgDecodeData *nDecodeData);
    //~PUdpEntityPosRequest();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
