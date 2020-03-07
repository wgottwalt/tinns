#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpZoning1 : public PUdpMsgAnalyser {
private:
    uint32_t mNewLocation;
    uint16_t mNewEntity;
    uint8_t mUnknown;

public:
    PUdpZoning1(PMsgDecodeData *nDecodeData);
    //~PUdpZoning1();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpZoning2 : public PUdpMsgAnalyser {
public:
    PUdpZoning2(PMsgDecodeData *nDecodeData);
    //~PUdpZoning2();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpGenrepZoning : public PUdpMsgAnalyser {
private:
    uint32_t mNewLocation;
    uint16_t mNewEntity;

public:
    PUdpGenrepZoning(PMsgDecodeData *nDecodeData);
    //~PUdpGenrepZoning();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
    static bool DoEffectiveZoning(PClient *nClient, uint32_t nNewLocation, uint16_t nNewEntity);
};

class PUdpAptGRZoning : public PUdpMsgAnalyser {
public:
    PUdpAptGRZoning(PMsgDecodeData *nDecodeData);
    //~PUdpAptGRZoning();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
    static bool DoEffectiveZoning(PClient *nClient);
};

class PUdpAddGenrepToList : public PUdpMsgAnalyser {
private:
    uint32_t mLocation;
    uint16_t mEntity;

public:
    PUdpAddGenrepToList(PMsgDecodeData *nDecodeData);
    //~PUdpAddGenrepToList();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpAppartmentAccess : public PUdpMsgAnalyser {
private:
    uint16_t mAppartmentPlace;
    char *mPassword;

public:
    PUdpAppartmentAccess(PMsgDecodeData *nDecodeData);
    //~PUdpAppartmentAccess();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};


class PUdpEndOfZoning : public PUdpMsgAnalyser {
public:
    PUdpEndOfZoning(PMsgDecodeData *nDecodeData);
    //~PUdpEndOfZoning();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
