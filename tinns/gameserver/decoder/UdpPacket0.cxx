#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"

/**** PUdpPacket0 ****/

PUdpPacket0::PUdpPacket0(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x2a";
//nDecodeData->mTraceDump=true;
//mDecodeData->mTraceKnownMsg = true;
}

PUdpMsgAnalyser* PUdpPacket0::Analyse()
{
  mDecodeData->mName << "=Packet0";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpPacket0::DoAction()
{
  if (mDecodeData->mState & DECODE_ACTION_READY)
  {
    PMessage* tmpMsg = MsgBuilder->BuildPacket0Msg(mDecodeData->mClient);
    mDecodeData->mClient->SendUDPMessage(tmpMsg);

    //mDecodeData->mMessage->SetNextByteOffset(mDecodeData->Sub0x13StartNext);
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
  }
  else
    return false;
}
