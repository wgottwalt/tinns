#include "gameserver/decoder/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpMsgAnalyser ****/

int PUdpMsgAnalyser::smAnaCount = 0; // temp for check

PUdpMsgAnalyser::PUdpMsgAnalyser(PMsgDecodeData* nDecodeData)
{
  mDecodeData = nDecodeData;
  if (++smAnaCount > 2) // temp for check
    Console->Print(RED, BLACK, "Analyser instances count: %d", smAnaCount);  // temp for check
}

PUdpMsgAnalyser::~PUdpMsgAnalyser()
{
  --smAnaCount;
}

PUdpMsgAnalyser* PUdpMsgAnalyser::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser;
  uint8_t MsgType;

  mDecodeData->mState = DECODE_MORE;
//mDecodeData->mTraceKnownMsg = true; // Don't want to trace all known messages
mDecodeData->mTraceUnknownMsg = true; // Want to show all unknown messages

  *(mDecodeData->mMessage) >> MsgType;
  switch(MsgType)
  {
    case 0x01:
    {
      nextAnalyser = new PUdpSync0(mDecodeData);
      break;
    }
    case 0x03:
    {
      nextAnalyser = new PUdpSync1(mDecodeData);
      break;
    }
    case 0x08:
    {
        nextAnalyser = new PUdp0x08(mDecodeData);
        break;
    }
    case 0x13:
    {
      nextAnalyser = new PUdp0x13(mDecodeData);
      break;
    }
    default:
    {
      mDecodeData->mUnknownType = MsgType;
      nextAnalyser = new PUdpMsgUnknown(mDecodeData);
      break;
    }
  }
 return nextAnalyser;
}

bool PUdpMsgAnalyser::DoAction()
{
  return false; // no action at this level
}


/**** Unkown UDP message ****/

PUdpMsgUnknown::PUdpMsgUnknown(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  char hexstr[16];

  nDecodeData->mState = DECODE_UNKNOWN;
  snprintf(hexstr, 16, "/0x%02x", mDecodeData->mUnknownType);
  mDecodeData->mName << hexstr;
  nDecodeData->mName << "=Unknown";
}

/*PUdpMsgUnknown::~PUdpMsgUnknown()
{
}*/

/*bool PUdpMsgUnknown::DoAction()
{
  return false;
}*/

/**** Ignore UDP message ****/

PUdpMsgIgnore::PUdpMsgIgnore(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  char hexstr[16];

  nDecodeData->mState = DECODE_FINISHED;
  snprintf(hexstr, 16, "/0x%02x", mDecodeData->mUnknownType);
  mDecodeData->mName << hexstr;
  nDecodeData->mName << "=Ignore";
}

/*PUdpMsgUnknown::~PUdpMsgUnknown()
{
}*/

/*bool PUdpMsgUnknown::DoAction()
{
  return false;
}*/
