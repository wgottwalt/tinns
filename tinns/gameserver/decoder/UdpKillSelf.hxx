#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpKillSelf : public PUdpMsgAnalyser {
public:
    PUdpKillSelf(PMsgDecodeData *nDecodeData);
    //~PUdpKillSelf();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
