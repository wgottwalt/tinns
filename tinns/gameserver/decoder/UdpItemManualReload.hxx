#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpItemManualReload : public PUdpMsgAnalyser {
private:
    uint8_t mUnknown;
    uint8_t GetMaxLoadableAmmos(uint8_t nBeltSlotId);

public:
    PUdpItemManualReload(PMsgDecodeData *nDecodeData);
    //~PUdpItemManualReload();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpReloadAnimStart : public PUdpMsgAnalyser {
public:
    PUdpReloadAnimStart(PMsgDecodeData *nDecodeData);
    //~PUdpReloadAnimStart();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
