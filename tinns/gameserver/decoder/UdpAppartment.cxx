#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpAptLocInfo ****/

PUdpAptLocInfo::PUdpAptLocInfo(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x0a";
}

PUdpMsgAnalyser* PUdpAptLocInfo::Analyse()
{
  mDecodeData->mName << "=Requesting appartment location for NavRay";

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpAptLocInfo::DoAction()
{
Console->Print("Got request for app data");
  PMessage* tmpMsg = MsgBuilder->BuildCharAptLocInfoMsg (mDecodeData->mClient);

  if (tmpMsg)
    mDecodeData->mClient->SendUDPMessage(tmpMsg);

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
