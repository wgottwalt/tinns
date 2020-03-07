#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PChar;
class PClient;
class PSpawnedVehicle;

class PUdpCharPosUpdate : public PUdpMsgAnalyser {
private:
    uint8_t mInfoBitfield;
    uint16_t mNewY;
    uint16_t mNewZ;
    uint16_t mNewX;
    uint8_t mNewUD;
    uint8_t mNewLR;
    uint8_t mNewAct;
    uint8_t mNewUnknown;
    uint32_t mChairItemID; // uint16_t or uint32_t ???
    uint8_t mChairItemSeat;

public:
    PUdpCharPosUpdate(PMsgDecodeData *nDecodeData);
    //~PUdpCharMoves();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpCharExitChair : public PUdpMsgAnalyser {
public:
    PUdpCharExitChair(PMsgDecodeData *nDecodeData);
    //~PUdpCharExitChair();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();

    //nClient is optionnal, nVhc is to limit to that vhc, nForce to force exit event when cab door closed
    static bool DoLeaveChair(PChar *nChar, PClient *nClient = nullptr, PSpawnedVehicle *nVhc = nullptr,
                             bool nForce = false);
};

class PUdpCharJump : public PUdpMsgAnalyser {
public:
    PUdpCharJump(PMsgDecodeData *nDecodeData);
    //~PUdpCharJump();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpCharTargeting : public PUdpMsgAnalyser {
public:
    PUdpCharTargeting(PMsgDecodeData *nDecodeData);
    //~PUdpCharTargeting();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
