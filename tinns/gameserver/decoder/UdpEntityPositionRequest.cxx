#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

PUdpEntityPosRequest::PUdpEntityPosRequest(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x0b";
}

PUdpMsgAnalyser* PUdpEntityPosRequest::Analyse()
{
  mDecodeData->mName << "=Entity position request";

//mDecodeData->mTraceUnknownMsg = true; // temp stop being bugged with unknown move msg
//mDecodeData->mTraceKnownMsg = true;

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 7);

  *nMsg >> mEntityID; //(uint16_t ... or uint32_t ???)

  if(mEntityID >= WORLDDATATEMPLATE_MAXPOSITEMS)
  {
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    mDecodeData->mErrorDetail = Misc::Ssprintf("Invalid position entity ID (%d)", mEntityID);
    Console->Print("%s Client[%d] sent invalid position entity Id[%d]", Console->ColorText(YELLOW, BLACK, "[Notice]"), mDecodeData->mClient->GetID(), mEntityID);
    mDecodeData->mTraceDump = true;
  }
  else
  {
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  }

  return this;
}

bool PUdpEntityPosRequest::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();
  PWorld* currentWorld = Worlds->GetWorld(nChar->GetLocation());
  float fpX, fpY, fpZ;
  uint16_t pX, pY, pZ;

  if(currentWorld)
  {
    if((mEntityID < WORLDDATATEMPLATE_MAXPOSITEMS) && currentWorld->getPositionItemPosition(mEntityID, &fpX, &fpY, &fpZ))
    {
      pX = (uint16_t) (fpX + 32000);
      pY = (uint16_t) (fpY + 32000);
      pZ = (uint16_t) (fpZ + 32000);

if(gDevDebug) Console->Print(GREEN, BLACK, "Client %d - Sending pos for entity %d : X=%d Y=%d Z=%d", mDecodeData->mClient->GetID(), mEntityID, pX, pY, pZ);
    }
    else if(!nClient->GetCharAwaitingWarpto(&pX, &pY, &pZ))
    {
      pX = pY = pZ = 0;
      Console->Print("%s Client[%d] requested invalid position entity %d. Position reset.", Console->ColorText(YELLOW, BLACK, "[Notice]"), mDecodeData->mClient->GetID(), mEntityID);
    }

    PMessage* tmpMsg;
    tmpMsg = MsgBuilder->BuildEntityPositionMsg(nClient, pX, pY, pZ);
    nClient->SendUDPMessage(tmpMsg);
    (nChar->Coords).mY=pY;
    (nChar->Coords).mZ=pZ;
    (nChar->Coords).mX=pX;
  }
  else
  {
    Console->Print(RED, BLACK, "[Warning] PUdpEntityPosRequest - Invalid world (%d)", nChar->GetLocation());
  }
//    if(IsRealMove && nClient->GetDebugMode(DBG_LOCATION))

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
