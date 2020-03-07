#pragma once

#include <cstdint>

struct PMsgDecodeData;

class PUdpMsgAnalyser {
    friend class PUdpMsgDecoder;

protected:
    PMsgDecodeData* mDecodeData;

public:
    PUdpMsgAnalyser(PMsgDecodeData *nDecodeData);
    virtual ~PUdpMsgAnalyser();

    virtual PUdpMsgAnalyser *Analyse();
    virtual bool DoAction();

    static int32_t smAnaCount; // temp for check

};

class PUdpMsgUnknown : public PUdpMsgAnalyser {
public:
    PUdpMsgUnknown(PMsgDecodeData *nDecodeData);
    //~PUdpMsgUnknown();

    //bool DoAction();
};

class PUdpMsgIgnore : public PUdpMsgAnalyser {
public:
    PUdpMsgIgnore(PMsgDecodeData *nDecodeData);
    //~PUdpMsgUnknown();

    //bool DoAction();
};
