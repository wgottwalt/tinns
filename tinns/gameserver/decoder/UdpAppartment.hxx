#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpAptLocInfo : public PUdpMsgAnalyser {
public:
    PUdpAptLocInfo(PMsgDecodeData *nDecodeData);
    //~PUdpAptLocInfo();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
