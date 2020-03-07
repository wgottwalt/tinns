#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpNPCDialogClose : public PUdpMsgAnalyser {
private:
    uint16_t mPlayerID;
    uint32_t mNPCID;

public:
    PUdpNPCDialogClose(PMsgDecodeData *nDecodeData);
    //~PUdpNPCDialogClose();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpNPCDialogAction : public PUdpMsgAnalyser {
private:
    uint16_t mPlayerID;
    uint8_t mAnswerNr;

public:
    PUdpNPCDialogAction(PMsgDecodeData *nDecodeData);
    //~PUdpNPCDialogAction();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
