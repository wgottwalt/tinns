#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpOutfitter : public PUdpMsgAnalyser {
private:
    uint32_t mSkin;
    uint8_t mHead;
    uint8_t mTorso;
    uint8_t mLegs;
    uint8_t mHeadColor;
    uint8_t mTorsoColor;
    uint8_t mLegsColor;
    uint8_t mHeadDarkness;
    uint8_t mTorsoDarkness;
    uint8_t mLegsDarkness;

public:
    PUdpOutfitter(PMsgDecodeData *nDecodeData);
    //~PUdpOutfitter();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
