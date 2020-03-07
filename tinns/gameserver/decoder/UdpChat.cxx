#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpChatLocal ****/

PUdpChatLocal::PUdpChatLocal(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x1b";
}

PUdpMsgAnalyser* PUdpChatLocal::Analyse()
{
  mDecodeData->mName << "=Local chat";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpChatLocal::DoAction()
{
  // temp
  Chat->HandleGameChat(mDecodeData->mClient, mDecodeData->mMessage->GetMessageData() + mDecodeData->Sub0x13Start);
    /*PMessage* cMsg = mDecodeData->mMessage;
    uint32_t ClientTime = cMsg->U32Data(mDecodeData->Sub0x13Start+2);

    PMessage* tmpMsg = MsgBuilder->BuildPingMsg(mDecodeData->mClient, ClientTime);
    mDecodeData->mClient->SendUDPMessage(tmpMsg);*/

    //cMsg->SetNextByteOffset(mDecodeData->Sub0x13StartNext);
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

/**** PUdpChatGlobal ****/

PUdpChatGlobal::PUdpChatGlobal(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x3b";
}

PUdpMsgAnalyser* PUdpChatGlobal::Analyse()
{
  //uint16_t dumb;
  mDecodeData->mName << "=Global chat";

/*  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 12);
  *nMsg >> mVehicleID; // ? not uint32_t ???
  *nMsg >> dumb;
  *nMsg >> mVehicleSeat;*/

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpChatGlobal::DoAction()
{
  // Temp
  Chat->HandleGameChat(mDecodeData->mClient, mDecodeData->mMessage->GetMessageData() + mDecodeData->Sub0x13Start);
/*  PMessage* tmpMsg = MsgBuilder->BuildCharEnteringVhcMsg (mDecodeData->mClient, mVehicleID, mVehicleSeat);
  ClientManager->UDPBroadcast(tmpMsg, mDecodeData->mClient);
*/
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpChatListAdd ****/

PUdpChatListAdd::PUdpChatListAdd(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x33";
}

PUdpMsgAnalyser* PUdpChatListAdd::Analyse()
{
  mDecodeData->mName << "=Add char to chat list";

  PMessage* nMsg = mDecodeData->mMessage;
  uint8_t PSize = nMsg->U8Data(mDecodeData->Sub0x13Start);
  mChatList = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 8);

  if ((mChatList == 1) || (mChatList == 2))
  {
    if ((PSize > 8) && (nMsg->U8Data(mDecodeData->Sub0x13StartNext -1) == 0))
    {
      mAddedCharname = (char*)mDecodeData->mMessage->GetMessageData() + mDecodeData->Sub0x13Start + 9;
      mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    }
    else
    {
      mDecodeData->mState = DECODE_ERROR;
      mDecodeData->mErrorDetail = "Invalid charname position";
    }

    return this;
  }
  else
  {
    mDecodeData->mUnknownType = mChatList;
    return new PUdpMsgUnknown(mDecodeData);
  }
}

bool PUdpChatListAdd::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  std::string AddedChar = mAddedCharname;
  PChar* tChar = Chars->GetChar(AddedChar);
  uint32_t AddedCharID = (tChar ? tChar->GetID() : 0);
  bool AddResult = false;

  if (AddedCharID)
  {
    switch(mChatList)
    {
      case 1:
      {
        AddResult = nClient->GetChar()->SetDirectChat(AddedCharID);
        break;
      }
      case 2:
      {
        AddResult = nClient->GetChar()->AddBuddy(AddedCharID);
        break;
      }
    }
  }

  if (AddResult)
  {
    PMessage* tmpMsg = MsgBuilder->BuildChatAddMsg (nClient, AddedCharID, mChatList);
    nClient->SendUDPMessage(tmpMsg);
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpChatListRemove ****/

PUdpChatListRemove::PUdpChatListRemove(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x39";
}

PUdpMsgAnalyser* PUdpChatListRemove::Analyse()
{
  mDecodeData->mName << "=remove char from chat list";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 8);
  (*nMsg) >> mChatList;

  if ((mChatList == 1) || (mChatList == 2))
  {
    (*nMsg) >> mRemovedCharID;
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
  }
  else
  {
    mDecodeData->mUnknownType = mChatList;
    return new PUdpMsgUnknown(mDecodeData);
  }
}

bool PUdpChatListRemove::DoAction()
{
  PClient* nClient = mDecodeData->mClient;

  bool RemoveResult = false;

  if (mRemovedCharID)
  {
    switch(mChatList)
    {
      case 1:
      {
        RemoveResult = nClient->GetChar()->SetDirectChat(0);
        break;
      }
      case 2:
      {
        RemoveResult = nClient->GetChar()->RemoveBuddy(mRemovedCharID);
        break;
      }
    }
  }

  // No known response yet ...
  /*if (AddResult)
  {
    PMessage* tmpMsg = MsgBuilder->BuildChatAddMsg (nClient, mRemovedCharID, mChatList);
    nClient->SendUDPMessage(tmpMsg);
  }*/

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

/**** PUdpChatChannels ****/

PUdpChatChannels::PUdpChatChannels(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x4c";
}

PUdpMsgAnalyser* PUdpChatChannels::Analyse()
{
  mDecodeData->mName << "=update listening custom chat channels selection";

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 8);
  (*nMsg) >> mChannelFlags;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpChatChannels::DoAction()
{
  PChar* nChar = mDecodeData->mClient->GetChar();
  nChar->SetActiveChannels(mChannelFlags);
//Console->Print("Channel flag: %08x", mChannelFlags);

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
