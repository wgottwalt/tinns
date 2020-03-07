#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpMultiPart : public PUdpMsgAnalyser {
private:
    uint16_t mChunkNr;
    uint16_t mChunkTotal;
    uint8_t mSequence;
    PMessage *mChunk;

public:
    PUdpMultiPart(PMsgDecodeData *nDecodeData);
    //~PUdpPing();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
