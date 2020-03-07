#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PUdpHeldItemBasicAction : public PUdpMsgAnalyser {
public:
    PUdpHeldItemBasicAction(PMsgDecodeData *nDecodeData);
    //~PUdpHeldItemBasicAction();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpHeldItemAimedAction : public PUdpMsgAnalyser {
private:
    uint16_t mWeaponId;
    uint32_t mTargetRawItemID;
    uint8_t mAiming; // 0: minimal
    uint8_t mTargetedHeight; // 0: bottom to 26: top
    uint8_t mScore; // ??? looks quite random...

public:
    PUdpHeldItemAimedAction(PMsgDecodeData *nDecodeData);
    //~PUdpHeldItemAimedAction();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpHeldItemLaunchingAction : public PUdpMsgAnalyser {
private:
    uint16_t mWeaponId;
    uint16_t mSourceY;
    uint16_t mSourceZ;
    uint16_t mSourceX;
    uint8_t mSourceUD;
    uint8_t mSourceLR;
    uint16_t mUnknown1;
    uint32_t mUnknown2; // client timestamp ? => TODO: compare with data from ping request
    uint32_t mTargetRawItemID;

public:
    PUdpHeldItemLaunchingAction( PMsgDecodeData *nDecodeData );
    //~PUdpHeldItemLaunchingAction();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};

class PUdpItemAddonActivation : public PUdpMsgAnalyser {
private:
    uint8_t mAddonIdx;

public:
    PUdpItemAddonActivation(PMsgDecodeData *nDecodeData);
    //~PUdpItemAddonActivation();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
