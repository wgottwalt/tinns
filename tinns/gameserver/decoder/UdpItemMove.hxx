#pragma once

#include <cstdint>
#include "gameserver/decoder/UdpAnalyser.hxx"

class PContainer;

class PUdpItemMove : public PUdpMsgAnalyser {
private:
    uint8_t mSrcLoc;
    uint8_t mSrcX;
    uint8_t mSrcY;
    uint8_t mDstLoc;
    uint8_t mDstX;
    uint8_t mDstY;
    uint8_t mItemCnt;

public:
      PUdpItemMove(PMsgDecodeData *nDecodeData);
      //~PUdpItemMove();
      PUdpMsgAnalyser *Analyse();
      bool DoAction();

      static PContainer *GetContainerByLoc(PChar *nChar, uint8_t nLoc);
};

class PUdpItemMoveBP : public PUdpMsgAnalyser {
private:
    uint8_t mSrcSlotId;
    uint8_t mDumb;
    uint8_t mDstX;
    uint8_t mDstY;

public:
      PUdpItemMoveBP(PMsgDecodeData *nDecodeData);
      //~PUdpItemMove();
      PUdpMsgAnalyser *Analyse();
      bool DoAction();
};

class PUdpItemDropOnItem : public PUdpMsgAnalyser {
private:
    uint8_t mSrcLoc;
    uint8_t mSrcX;
    uint8_t mSrcY;
    uint8_t mDstLoc;
    uint8_t mDstX;
    uint8_t mDstY;

public:
    PUdpItemDropOnItem(PMsgDecodeData *nDecodeData);
    //~PUdpItemDropOnItem();
    PUdpMsgAnalyser *Analyse();
    bool DoAction();
};
