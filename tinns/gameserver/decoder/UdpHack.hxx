#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpHackFail : public PUdpMsgAnalyser {
public:
    PUdpHackFail(PMsgDecodeData *nDecodeData);
    //~PUdpHackFail();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpHackSuccess : public PUdpMsgAnalyser {
public:
    PUdpHackSuccess(PMsgDecodeData *nDecodeData);
    //~PUdpHackSuccess();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
