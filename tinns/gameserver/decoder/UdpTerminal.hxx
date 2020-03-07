#pragma once

#include <cstdint>
#include <string>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpReceiveDB : public PUdpMsgAnalyser {
private:
    static const uint8_t mMaxOptions = 9; // Largest: forum\forenlist.tsc(21, 114,...)
    uint16_t mTerminalSessionId;
    std::string mCommandName;
    std::string mOptions[mMaxOptions];
    uint8_t mOptionsCount;

    uint16_t mUnknown1;
    uint8_t mUnknown2;
    uint16_t mDBId;

    bool ActionVehicleListing();
    bool ActionVehicleControl();

public:
    PUdpReceiveDB(PMsgDecodeData *nDecodeData);
    //~PUdpReceiveDB();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpUpdateDB : public PUdpMsgAnalyser {
private:
    static const uint8_t mMaxOptions = 7; // Largest: forum\bbcnewthread.tsc(66):
    uint16_t mTerminalSessionId;
    std::string mCommandName;
    std::string mOptions[mMaxOptions];
    uint8_t mOptionsCount;

    uint16_t mUnknown1;
    uint8_t mUnknown2;
    uint16_t mDBId;

public:
    PUdpUpdateDB(PMsgDecodeData *nDecodeData);
    //~PUdpUpdateDB();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpTryAccessDB : public PUdpMsgAnalyser {
private:
    static const uint8_t mMaxOptions = 9; // Largest: stockx\depot.tsc(227):
    uint16_t mTerminalSessionId;
    std::string mCommandName;
    std::string mOptions[mMaxOptions];
    uint8_t mOptionsCount;

    uint16_t mUnknown1;
    uint8_t mUnknown2;
    uint16_t mDBId;

public:
    PUdpTryAccessDB(PMsgDecodeData *nDecodeData);
    //~PUdpTryAccessDB();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpQueryDB : public PUdpMsgAnalyser { // Also called "ServerMessage" in .tsc scripts!
private:
    static const uint8_t mMaxOptions = 5; // Largest: politics\transcomment.tsc(36):
    uint16_t mTerminalSessionId;
    uint16_t mDBId;
    std::string mDBCommandName;
    std::string mCommandName;
    std::string mOptions[mMaxOptions];
    uint8_t mOptionsCount;

    bool ActionSpawnVehicle();
    bool ActionRepairVehicle();
    bool ActionDismissVehicle();

public:
    PUdpQueryDB(PMsgDecodeData *nDecodeData);
    //~PUdpQueryDB();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpTeminal0x1f : public PUdpMsgAnalyser {
private:
    uint16_t mTerminalSessionId;

public:
    PUdpTeminal0x1f(PMsgDecodeData *nDecodeData);
    //~PUdpTeminal0x1f();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
