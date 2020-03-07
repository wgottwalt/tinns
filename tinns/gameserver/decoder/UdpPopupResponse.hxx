#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpPopupResponse : public PUdpMsgAnalyser {
public:
    PUdpPopupResponse(PMsgDecodeData *nDecodeData);
    //~PUdpPopupResponse();
    PUdpMsgAnalyser *Analyse();
    //bool DoAction();
};

class PUdpVentureWarpConfirm : public PUdpMsgAnalyser {
private:
    uint32_t mRawItemId;
    uint16_t mStatus;
    uint32_t mUnknown1;
    uint16_t mUnknown2;

public:
    PUdpVentureWarpConfirm(PMsgDecodeData *nDecodeData);
    //~PUdpVentureWarpConfirm();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpVhcAccessResponse : public PUdpMsgAnalyser {
private:
    uint32_t mVhcAccessRequestId;
    uint16_t mStatus;
    uint16_t mUnknown;

public:
    PUdpVhcAccessResponse(PMsgDecodeData *nDecodeData);
    //~PUdpVhcAccessResponse();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
