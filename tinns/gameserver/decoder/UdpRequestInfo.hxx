#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpReqInfo : public PUdpMsgAnalyser {
private:
    uint16_t mRequestType;
    uint32_t mInfoId;

public:
    PUdpReqInfo(PMsgDecodeData *nDecodeData);
    //~PUdpCharInfo();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
