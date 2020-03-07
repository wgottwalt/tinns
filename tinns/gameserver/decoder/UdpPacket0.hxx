#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpPacket0 : public PUdpMsgAnalyser {
public:
    PUdpPacket0(PMsgDecodeData *nDecodeData);
    //~PUdpPacket0();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
