#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpItemSlotUse : public PUdpMsgAnalyser {
private:
    uint8_t mTargetSlot;

public:
    PUdpItemSlotUse(PMsgDecodeData *nDecodeData);
    //~PUdpItemSlotUse();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
