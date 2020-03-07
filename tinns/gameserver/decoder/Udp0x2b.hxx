#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdp0x2b : public PUdpMsgAnalyser {
public:
    PUdp0x2b(PMsgDecodeData *nDecodeData);
    //~PUdp0x2b();
    PUdpMsgAnalyser *Analyse();
    //bool DoAction();
};
