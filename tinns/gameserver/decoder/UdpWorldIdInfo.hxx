#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PWorldIDInfoReq : public PUdpMsgAnalyser {
    uint32_t mInfoId;

public:
    PWorldIDInfoReq(PMsgDecodeData *nDecodeData);
    //~PWorldIDInfoReq();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
