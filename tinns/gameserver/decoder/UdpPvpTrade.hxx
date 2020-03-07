#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpPvPTrade : public PUdpMsgAnalyser {
public:
    PUdpPvPTrade(PMsgDecodeData *nDecodeData);
    //~PUdpPvPTrade();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
