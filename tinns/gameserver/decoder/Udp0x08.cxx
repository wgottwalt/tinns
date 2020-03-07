#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"

PUdp0x08::PUdp0x08(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x08";
}

PUdpMsgAnalyser* PUdp0x08::Analyse()
{
    mDecodeData->mName << "=Client crash";
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

    return this;
}

bool PUdp0x08::DoAction()
{
    // Client crashed, close connection from our side
    GameServer->ClientDisconnected(mDecodeData->mClient);
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;

    return true;
}
