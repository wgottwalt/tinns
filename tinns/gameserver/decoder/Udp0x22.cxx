#include "gameserver/decoder/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdp0x22 ****/

PUdp0x22::PUdp0x22(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x22";
}

PUdpMsgAnalyser* PUdp0x22::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser = nullptr;
  mDecodeData->mState = DECODE_MORE;
  uint8_t MsgType = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 5);
  switch(MsgType) // MsgType is probably uint16_t rather than uint8_t
  {
    case 0x03: // Zoning phase 2
    {
      nextAnalyser = new PUdpZoning2(mDecodeData);
      break;
    }
    case 0x06: // Char/Clan/Rank/Map Info request
    {
      nextAnalyser = new PUdpReqInfo(mDecodeData);
      break;
    }
    case 0x0b: // Entity position request
    {
      nextAnalyser = new PUdpEntityPosRequest(mDecodeData);
      break;
    }
    case 0x0d: // Zoning phase 1
    {
      nextAnalyser = new PUdpZoning1(mDecodeData);
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
