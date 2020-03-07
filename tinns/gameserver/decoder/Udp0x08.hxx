#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdp0x08 : public PUdpMsgAnalyser {
public:
    PUdp0x08(PMsgDecodeData *nDecodeData);
    //~PUdp0x08();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};
