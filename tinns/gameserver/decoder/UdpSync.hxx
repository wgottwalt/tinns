#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpSync0 : public PUdpMsgAnalyser {
public:
    PUdpSync0(PMsgDecodeData *nDecodeData);
    //~PUdpSync0();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();

    static void GetToSync1(PMsgDecodeData *nDecodeData);
};

class PUdpSync1 : public PUdpMsgAnalyser {
public:
    PUdpSync1(PMsgDecodeData *nDecodeData);
    //~PUdpSync1();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpSync2 : public PUdpMsgAnalyser {
private:
    uint32_t mClientTime;

public:
    PUdpSync2(PMsgDecodeData *nDecodeData);
    //~PUdpSync2();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
