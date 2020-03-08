#include "gameserver/decoder/Includes.hxx"
#include "gameserver/definitions/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

/**** PUdp0x1f ****/

PUdpPopupResponse::PUdpPopupResponse( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x09";
}

PUdpMsgAnalyser* PUdpPopupResponse::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser = nullptr;
  mDecodeData->mState = DECODE_MORE;
  uint8_t MsgType = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 12 ); //uint32_t but only uint8_t used

  switch ( MsgType )
  {
    case 0x06:
    {
      nextAnalyser = new PUdpVhcAccessResponse( mDecodeData );
      break;
    }
    case 0x07:
    {
      nextAnalyser = new PUdpVentureWarpConfirm( mDecodeData );
      break;
    }
    default:
    {
      mDecodeData->mUnknownType = MsgType;
      mDecodeData->mTraceUnknownMsg = true;
      break;
    }
  }

  if ( ! nextAnalyser )
  {
    nextAnalyser = new PUdpMsgUnknown( mDecodeData );
  }

  return nextAnalyser;
}

/**** PUdpVentureWarpConfirm ****/

PUdpVentureWarpConfirm::PUdpVentureWarpConfirm( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x05";
}

PUdpMsgAnalyser* PUdpVentureWarpConfirm::Analyse()
{
  mDecodeData->mName << "=Venture Warp confirmation";

  PMessage* cMsg = mDecodeData->mMessage;
  cMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 16 );
  ( *cMsg ) >> mUnknown1;
  ( *cMsg ) >> mUnknown2;
  ( *cMsg ) >> mRawItemId;
  ( *cMsg ) >> mStatus;
  //cMsg->Dump();
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpVentureWarpConfirm::DoAction()
{
  PClient* nClient = mDecodeData->mClient;

  if ( mStatus == 1 )
  {
    uint32_t newLocation;
    do
    {
      newLocation = Worlds->GetWorldIdFromWorldmap( Misc::GetRandom( PWorlds::mOutdoorWorldmapHSize, 0 ), Misc::GetRandom( PWorlds::mOutdoorWorldmapVSize, 0 ) );
    }
    while ( ! newLocation );

    uint16_t nEntity = 10;
    uint16_t nEntityType = 1;

    PMessage* tmpMsg = MsgBuilder->BuildAptLiftUseMsg( nClient, newLocation, nEntity, nEntityType );
    nClient->SendUDPMessage( tmpMsg );

    if ( gDevDebug )
      Console->Print( "Client[%d]: Venture Warping to zone %d (entity %d/%d)", nClient->GetID(), newLocation, nEntity, nEntityType );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}


/**** PUdpVhcAccessResponse ****/

PUdpVhcAccessResponse::PUdpVhcAccessResponse( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x06";
}

PUdpMsgAnalyser* PUdpVhcAccessResponse::Analyse()
{
  mDecodeData->mName << "=Response to vhc access request";

  PMessage* cMsg = mDecodeData->mMessage;
  cMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 16 );
  ( *cMsg ) >> mVhcAccessRequestId;
  ( *cMsg ) >> mUnknown;
  ( *cMsg ) >> mStatus;

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpVhcAccessResponse::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();
  PVhcAccessRequestList* nAccessRequests = nChar->GetVhcAccessRequestList();

  if ( gDevDebug )
    Console->Print( "Access response for Req n°%d : %d (unknown %d)", mVhcAccessRequestId, mStatus, mUnknown );

  if ( nAccessRequests->RegisterResponse( mVhcAccessRequestId, ( mStatus == 1 ) ) )
  {
    uint32_t requesterCharId = 0;
    uint32_t vehicleId = 0;
    nAccessRequests->GetInfo( mVhcAccessRequestId, &requesterCharId, &vehicleId );
    PClient* requesterClient = ClientManager->getClientByChar( requesterCharId );
    PChar* requesterChar = (requesterClient ? requesterClient->GetChar() : nullptr);

    if ( requesterChar && nAccessRequests->Check( mVhcAccessRequestId ) )
    {
      PWorld* CurrentWorld = Worlds->GetWorld( requesterChar->GetLocation() );
      if ( CurrentWorld )
      {
        PSpawnedVehicle* tVhc = CurrentWorld->GetSpawnedVehicles()->GetVehicleByGlobalId( vehicleId );

        if ( tVhc )
        {
          PUdpVhcUse::DoFreeSitting( requesterClient,  tVhc, tVhc->GetLocalId() );
        }
      }
    }
    else
    {
      PMessage* tmpMsg = MsgBuilder->BuildText100Msg( requesterClient, 20, 0 ); // "req refused" msg
      requesterClient->SendUDPMessage( tmpMsg );
    }
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
