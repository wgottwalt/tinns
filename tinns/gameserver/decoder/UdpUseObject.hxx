#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpUseObject : public PUdpMsgAnalyser {
private:
    uint32_t mRawItemID;

    void OldHandler(); // Temp during migration only

public:
    PUdpUseObject(PMsgDecodeData *nDecodeData);
    //~PUdpUseObject();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpCloseItemContainer : public PUdpMsgAnalyser {
public:
    PUdpCloseItemContainer(PMsgDecodeData *nDecodeData);
    //~PUdpCloseItemContainer();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
