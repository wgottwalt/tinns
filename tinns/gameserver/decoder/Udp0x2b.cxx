#include "gameserver/decoder/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdp0x2b ****/

PUdp0x2b::PUdp0x2b(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x2b";
}

PUdpMsgAnalyser* PUdp0x2b::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser = nullptr;
  mDecodeData->mState = DECODE_MORE;
  uint8_t MsgType = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 5);
  switch(MsgType)
  {
    case 0x17: // ReceiveDB Terminal Command
    {
      nextAnalyser = new PUdpReceiveDB(mDecodeData);
      break;
    }
    case 0x18: // UpdateDB Terminal Command
    {
      nextAnalyser = new PUdpUpdateDB(mDecodeData);
      break;
    }
    case 0x19: // TryAccess Terminal Command
    {
      nextAnalyser = new PUdpTryAccessDB(mDecodeData);
      break;
    }
    case 0x1b: // DB Query and Command
    {
      nextAnalyser = new PUdpQueryDB(mDecodeData);
      break;
    }
    case 0x1f: // Citycom?
    {
      nextAnalyser = new PUdpTeminal0x1f(mDecodeData);
      break;
    }

    default:
    {
      mDecodeData->mUnknownType = MsgType;
      break;
    }
  }

  if (! nextAnalyser)
  {
    nextAnalyser = new PUdpMsgUnknown(mDecodeData);
  }

  return nextAnalyser;
}
