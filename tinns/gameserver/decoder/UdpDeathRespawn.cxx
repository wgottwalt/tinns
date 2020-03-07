#include "gameserver/decoder/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpDeathRespawn ****/

PUdpDeathRespawn::PUdpDeathRespawn( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x2f";
}

PUdpMsgAnalyser* PUdpDeathRespawn::Analyse()
{
  PMessage* tmpMsg = mDecodeData->mMessage;

  mDecodeData->mName << "=Death respawn selection";

  tmpMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 8 );
  ( *tmpMsg ) >> mEntity;
  ( *tmpMsg ) >> mWorldId;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpDeathRespawn::DoAction()
{
  //PClient* nClient = mDecodeData->mClient;
  //PChar* nChar = nClient->GetChar();

  //PMessage* tmpMsg = MsgBuilder->BuildReqInfoAnswerMsg(mDecodeData->mClient, mRequestType, mInfoId, Answer, len);
  //mDecodeData->mClient->SendUDPMessage(tmpMsg);

  //if ( gDevDebug )
    Console->Print( "%s Request to respawn in zone %d, entity %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mWorldId, mEntity );

  if( mWorldId == 0xffffffff )
  {
    PUdpAptGRZoning::DoEffectiveZoning( mDecodeData->mClient );
  }
  else
  {
    PUdpGenrepZoning::DoEffectiveZoning( mDecodeData->mClient, mWorldId, mEntity );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return ( true );
}
