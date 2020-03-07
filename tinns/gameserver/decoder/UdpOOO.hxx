#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpOOO : public PUdpMsgAnalyser {
public:
    PUdpOOO(PMsgDecodeData *nDecodeData);
    //~PUdpPing();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
