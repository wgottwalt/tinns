#include "gameserver/decoder/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdpItemUse ****/

PUdpItemUse::PUdpItemUse( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x01";
}

PUdpMsgAnalyser* PUdpItemUse::Analyse()
{
  mDecodeData->mName << "=Inventory item use";
/*
  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 8 );

  ( *nMsg ) >> mWeaponId;
  ( *nMsg ) >> mTargetRawItemID; // !!! 0x000003fe when shooting with no target
  ( *nMsg ) >> mUnknown2; // aiming ??? 0 to 52 +?
  ( *nMsg ) >> mTargetedHeight; // range 0 (bottom) to 26 (?) (top)
  ( *nMsg ) >> mScore; // range 0x00 to 0xff  Score ???
*/

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemUse::DoAction()
{
  //PClient* nClient = mDecodeData->mClient;
  //PChar* tChar = nClient->GetChar();

  //PMessage* tmpMsg = MsgBuilder->BuildHeldItemUsedMsg( nClient->GetLocalID(), mWeaponId, mTargetRawItemID, mUnknown2, mTargetedHeight, 0 ); // 'score' is not broadcasted, but set to 0
  //ClientManager->UDPBroadcast( tmpMsg, nClient );

  //if ( gDevDebug )
  {
    Console->Print( "%s Using inventory item", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    mDecodeData->mMessage->Dump();
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
