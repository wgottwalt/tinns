#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdp0x13 : public PUdpMsgAnalyser {
public:
    PUdp0x13(PMsgDecodeData *nDecodeData);
    //~PUdp0x13();
    PUdpMsgAnalyser* Analyse();
    //bool DoAction();
};
