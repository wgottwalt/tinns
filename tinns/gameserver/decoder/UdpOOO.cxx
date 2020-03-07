#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpOOO ****/

PUdpOOO::PUdpOOO(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x01";
}

PUdpMsgAnalyser* PUdpOOO::Analyse()
{
  mDecodeData->mName << "=Out Of Order";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpOOO::DoAction()
{
    uint16_t MissingUDP_ID = mDecodeData->mMessage->U16Data(mDecodeData->Sub0x13Start+5);

    mDecodeData->mClient->getUDPConn()->ReSendUDPMessage(MissingUDP_ID);
    //Console->Print("%s Out of Order packet received ! (Client is missing UDPID %d) ***not managed yet***", Console->ColorText(YELLOW, BLACK, "[Notice]"), MissingUDP_ID);

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
