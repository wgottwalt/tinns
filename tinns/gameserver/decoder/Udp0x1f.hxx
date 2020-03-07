#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdp0x1f : public PUdpMsgAnalyser {
public:
    PUdp0x1f(PMsgDecodeData *nDecodeData);
    //~PUdp0x1f();
    PUdpMsgAnalyser *Analyse();
    //bool DoAction();
};
