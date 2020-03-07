#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PSpawnedVehicle;
class PVhcCoordinates;

class PUdpVhcMove : public PUdpMsgAnalyser {
private:
    uint16_t mVhcLocalId;
    uint16_t mNewY;
    uint16_t mNewZ;
    uint16_t mNewX;
    uint16_t mNewLR;
    uint16_t mNewRoll;
    uint16_t mUnk1; // always 0x0001 ?
    uint8_t mMoveType;
    uint8_t mNewUD;
    uint8_t mFF; // always 0xff ?
    uint8_t mAction; // &1 = Left, &2 = Right, &4 = Forward, &8 = Backward

public:
    PUdpVhcMove(PMsgDecodeData *nDecodeData);
    //~PUdpVhcMove();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
    bool DoVhcZoning(PSpawnedVehicle *currVhc, uint32_t currWorldId, uint32_t destWorldId, PVhcCoordinates *destPos);
};

class PUdpVhcUse : public PUdpMsgAnalyser {
private:
    uint32_t mVehicleID;
    uint8_t mVehicleSeat;

public:
    PUdpVhcUse(PMsgDecodeData *nDecodeData);
    //~PUdpVhcUse();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
    static void DoFreeSitting(PClient *nClient, PSpawnedVehicle *nVhc, uint32_t nRawVhcLocalId, uint8_t nSeatId = 254);
};

class PUdpSubwayUpdate : public PUdpMsgAnalyser {
private:
    uint32_t mVehicleID;
    uint16_t mPosition;
    uint8_t mDoorOpened;

public:
    PUdpSubwayUpdate(PMsgDecodeData *nDecodeData);
    //~PUdpSubwayUpdate();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpRequestVhcInfo : public PUdpMsgAnalyser {
private:
    uint32_t mVehicleID;

public:
    PUdpRequestVhcInfo(PMsgDecodeData *nDecodeData);
    //~PUdpRequestVhcInfo();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
