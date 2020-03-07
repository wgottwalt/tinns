#include "gameserver/decoder/Includes.hxx"
#include "common/Includes.hxx"

void PUdpMsgDecoder::Init(PMessage *nMessage, PClient *nClient)
{
    mDecodeData.mMessage = nMessage;
    mDecodeData.mClient = nClient;
    mDecodeData.mState = (nMessage && nClient) ? DECODE_MORE : DECODE_UNDEF;
    mDecodeData.mUnknownType = 0;
    mDecodeData.mHandling0x13Sub = false;
    mDecodeData.Sub0x13Start = mDecodeData.Sub0x13StartNext = 0;
    mPacketName.erase();
    mDecodeData.mName.str(mPacketName);
    mDecodeData.mErrorDetail.erase();
    mDecodeData.mTraceKnownMsg = false;
    mDecodeData.mTraceUnknownMsg = false;
    mDecodeData.mTraceDump = false;
    if (mCurrentAnalyser)
    {
        delete mCurrentAnalyser;
        mCurrentAnalyser = nullptr;
    }
}

PUdpMsgDecoder::PUdpMsgDecoder()
{
  mCurrentAnalyser = nullptr;
  Reset();
}

PUdpMsgDecoder::PUdpMsgDecoder(PMessage *nMessage, PClient *nClient)
{
  mCurrentAnalyser = nullptr;
  Init(nMessage, nClient);
}

PUdpMsgDecoder::~PUdpMsgDecoder()
{
  if (mCurrentAnalyser)
  {
    delete mCurrentAnalyser;
  }
}

bool PUdpMsgDecoder::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser;

  if ((mDecodeData.mState & DECODE_MORE) || mDecodeData.mHandling0x13Sub)
  {
    if (mCurrentAnalyser)
    {
      delete mCurrentAnalyser;
      mCurrentAnalyser = nullptr;
    }
    if (mDecodeData.mHandling0x13Sub)
    {
      mPacketName.erase();
      mDecodeData.mName.str(mPacketName);
      mCurrentAnalyser = new PUdp0x13(&mDecodeData);
      mDecodeData.mState = DECODE_MORE;
    }
    else
    {
      mCurrentAnalyser = new PUdpMsgAnalyser(&mDecodeData);
    }

    while (mDecodeData.mState & DECODE_MORE)
    {
      nextAnalyser = mCurrentAnalyser->Analyse();
      if (mCurrentAnalyser != nextAnalyser)
      {
        delete mCurrentAnalyser;
        mCurrentAnalyser = nextAnalyser;
      }
    }
  }
  return (!(mDecodeData.mState & (DECODE_MORE | DECODE_ERROR | DECODE_UNKNOWN)));
}

bool PUdpMsgDecoder::Analyse(PMessage *nMessage, PClient *nClient)
{
  Init(nMessage, nClient);
  return Analyse();
}

void PUdpMsgDecoder::Init(PMessage *nMessage, PClient *nClient, PGameState *nClientState)
{
    mDecodeData.mClientState = nClientState;
    Init(nMessage, nClient);
}

uint8_t PUdpMsgDecoder::GetState()
{
    return mDecodeData.mState;
}

bool PUdpMsgDecoder::IsError()
{
    return (mDecodeData.mState & DECODE_ERROR);
}

bool PUdpMsgDecoder::IsKnown()
{
    return (!(mDecodeData.mState & DECODE_UNKNOWN));
}

bool PUdpMsgDecoder::MoreSubMsg()
{
    return mDecodeData.mHandling0x13Sub;
}

bool PUdpMsgDecoder::IsActionReady()
{
    return (mDecodeData.mState & DECODE_ACTION_READY);
}

bool PUdpMsgDecoder::IsActionDone()
{
    return (mDecodeData.mState & DECODE_ACTION_DONE);
}

bool PUdpMsgDecoder::IsTraceKnownMsg()
{
    return mDecodeData.mTraceKnownMsg;
}

bool PUdpMsgDecoder::IsTraceUnknownMsg()
{
    return mDecodeData.mTraceUnknownMsg;
}

bool PUdpMsgDecoder::IsTraceDump()
{
    return mDecodeData.mTraceDump;
}

void PUdpMsgDecoder::DumpMsg()
{
    if (mDecodeData.mMessage)
        mDecodeData.mMessage->Dump();
}

std::string const &PUdpMsgDecoder::GetName()
{
    return (mTmpName = mDecodeData.mName.str());
}

std::string const &PUdpMsgDecoder::GetError()
{
    return mDecodeData.mErrorDetail;
}

bool PUdpMsgDecoder::DoAction()
{
  if (mDecodeData.mState & DECODE_ACTION_READY)
  {
    return mCurrentAnalyser->DoAction();
  }
  else
  {
    return false;
  }
}

void PUdpMsgDecoder::Reset()
{
    Init(nullptr, nullptr);
}
