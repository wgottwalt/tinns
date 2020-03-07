#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdp0x22 : public PUdpMsgAnalyser {
public:
    PUdp0x22(PMsgDecodeData *nDecodeData);
    //~PUdp0x22();
    PUdpMsgAnalyser *Analyse();
    //bool DoAction();
};
