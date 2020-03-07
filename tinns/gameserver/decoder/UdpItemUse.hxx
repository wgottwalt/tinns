#pragma once

#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpItemUse : public PUdpMsgAnalyser {
/*  private:
    u16 mWeaponId;
    u32 mTargetRawItemID;
    u8 mUnknown2;
    u8 mTargetedHeight;
    u8 mScore; // ??? looks quite random...
*/
public:
    PUdpItemUse(PMsgDecodeData *nDecodeData);
    //~PUdpItemUse();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
