#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

//  TODO: - put chunking code from PUdpSync2::DoAction() in a SendChunkedMessqage(PClient* nClient, PMessage* nMessage,
//                                                                                uint8_t nChunkSize = 220)

/**** PUdpSync0 ****/

PUdpSync0::PUdpSync0( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x01";
}

PUdpMsgAnalyser* PUdpSync0::Analyse()
{
  if ( mDecodeData->mMessage->GetSize() == 10 )
  {
    mDecodeData->mName << "=Sync0";
    if ( mDecodeData->mClientState->UDP.mState != PGameState::UDP::GUS_SYNC1 )
    {
      mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    }
    else
    {
      mDecodeData->mState = DECODE_FINISHED;
    }
    return this;
  }
  else
  {
    return new PUdpMsgUnknown( mDecodeData );
  }
}

bool PUdpSync0::DoAction()
{
  if (( mDecodeData->mState & DECODE_ACTION_READY ) && ( mDecodeData->mClientState->UDP.mState != PGameState::UDP::GUS_SYNC1 ) )
  {
    GetToSync1( mDecodeData );
    return true;
  }
  else
  {
    mDecodeData->mErrorDetail = "No action ready or client already in Sync1";
    return false;
  }
}

void PUdpSync0::GetToSync1( PMsgDecodeData* nDecodeData )
{
  PClient* nClient = nDecodeData->mClient;
  nClient->SetZoning();

  uint32_t loc = nClient->GetChar()->GetLocation();
//Console->Print("inside HandleGame : Location: %d", loc);
  SendZone( nClient, loc );

  // "aliverep" ?
  PMessage* tmpMsg = MsgBuilder->BuildAliveRepMsg( nClient );
  nClient->SendUDPMessage( tmpMsg );

  nDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  //State->UDP.mSynced=true;
  nDecodeData->mClientState->UDP.mState = PGameState::UDP::GUS_SYNC1;// To change later
//Console->Print("Initialize: UDP_ID");
  nClient->SetUDP_ID( 0 );
}


/**** PUdpSync1 ****/

PUdpSync1::PUdpSync1( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x03";
}

PUdpMsgAnalyser* PUdpSync1::Analyse()
{
  mDecodeData->mName << "=Sync1";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpSync1::DoAction()
{
  if (( mDecodeData->mState & DECODE_ACTION_READY ) && ( mDecodeData->mClientState->UDP.mState == PGameState::UDP::GUS_SYNC1 ) )
  {

    /*PMessage* tmpMsg = new PMessage(15);
    PClient* nClient = mDecodeData->mClient;

    nClient->IncreaseUDP_ID();

    *tmpMsg << (uint8_t)0x13;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint16_t)nClient->GetSessionID();
    *tmpMsg << (uint8_t)0x0a; // Message length place;
    *tmpMsg << (uint8_t)0x03;
    *tmpMsg << (uint16_t)nClient->GetUDP_ID();
    *tmpMsg << (uint8_t)0x23;
    *tmpMsg << (uint8_t)0x0f;
    *tmpMsg << (uint8_t)0x00; // ??
    *tmpMsg << (uint8_t)0x03; // ??
    *tmpMsg << (uint8_t)0x00; // ??
    *tmpMsg << (uint8_t)0x01; // ??
    *tmpMsg << (uint8_t)0x00; // ??

    (*tmpMsg)[5] = (uint8_t)(tmpMsg->GetSize() - 6);
    nClient->SendUDPMessage(tmpMsg);*/

    mDecodeData->mClientState->UDP.mState = PGameState::UDP::GUS_SYNC2;
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
  }
  else
    return false;
}

/**** PUdpSync2 ****/

PUdpSync2::PUdpSync2( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x0c";
}

PUdpMsgAnalyser* PUdpSync2::Analyse()
{
  mDecodeData->mName << "=Sync2";

  mClientTime = mDecodeData->mMessage->U32Data( mDecodeData->Sub0x13Start + 2 );

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpSync2::DoAction()
{
  if (( mDecodeData->mState & DECODE_ACTION_READY ) && ( mDecodeData->mClientState->UDP.mState == PGameState::UDP::GUS_SYNC2 ) )
  {
    PClient* nClient = mDecodeData->mClient;

    // Baseline message chunking & sending
    PMessage* BaselineMsg = MsgBuilder->BuildBaselineMsg( nClient );
    nClient->FragmentAndSendUDPMessage( BaselineMsg, 0x19 );

    // Sending "CharInfo3/Zoning2Msg" message
    // Removed because same as Zoning2Msg
    PMessage* Zoning2Msg = MsgBuilder->BuildZoning2Msg( nClient, mClientTime );
    nClient->SendUDPMessage( Zoning2Msg );

    mDecodeData->mClientState->UDP.mState = PGameState::UDP::GUS_SYNC3;
    nClient->SetZoning( false );

    // If char is sitting (vhz zoning), send it now to client
    uint32_t nSeatableObjectId;
    uint8_t nSeatId;
    if( nClient->GetChar()->GetSeatInUse(&nSeatableObjectId, &nSeatId) )
    {
      if( gDevDebug )
        Console->Print( YELLOW, BLACK, "[DEBUG] PUdpSync2::DoAction : Char %d sitting on vhc id %d, seat %d", nClient->GetLocalID(), nSeatableObjectId, nSeatId  );
      PMessage* SittingMsg = MsgBuilder->BuildCharUseSeatMsg( nClient, nSeatableObjectId, nSeatId );
      nClient->FillInUDP_ID(SittingMsg);
      nClient->SendUDPMessage( SittingMsg );
    }

    //Temp: send Subway to client if in subway
    if ( nClient->GetChar()->GetLocation() == PWorlds::mNcSubwayWorldId )
    {
      PMessage* SubwayMsg = MsgBuilder->BuildSubwaySpawnMsg( nClient, false );
      nClient->SendUDPMessage( SubwayMsg );
      SubwayMsg = MsgBuilder->BuildSubwaySpawnMsg( nClient, true );
      nClient->SendUDPMessage( SubwayMsg );
    }

    // Send spawned vehicles
    PWorld* CurrentWorld = Worlds->GetWorld( nClient->GetChar()->GetLocation() );
    if ( CurrentWorld )
    {
      PSpawnedVhcList* VhcList = CurrentWorld->GetSpawnedVehicles()->GetSpawnedVehicles();
      PSpawnedVehicle* VhcEntry;
      PMessage* VhcMsg;

      while ( ! VhcList->empty() )
      {
        VhcEntry = VhcList->front();
        VhcList->pop();
        if( gDevDebug )
          Console->Print( YELLOW, BLACK, "[DEBUG] PUdpSync2::DoAction : Send info for vhc id %d", VhcEntry->GetLocalId() );
        VhcMsg = MsgBuilder->BuildVhcPosUpdateMsg( VhcEntry );
        nClient->FillInUDP_ID( VhcMsg );
        nClient->SendUDPMessage( VhcMsg );
        VhcMsg = MsgBuilder->BuildVhcInfoMsg( nClient, VhcEntry );
        nClient->SendUDPMessage( VhcMsg );
      }
    }

    // Dispatching info to & from other chars in zone
    int nbSent;
    nbSent = ClientManager->SendUDPZoneWelcomeToClient( nClient );
//Console->Print(GREEN, BLACK, " %d Welcome message were sent to client %d", nbSent, Client->GetIndex());

    PMessage* HelloMsg = MsgBuilder->BuildCharHelloMsg( nClient );
    nbSent = ClientManager->UDPBroadcast( HelloMsg, nClient );
//Console->Print(GREEN, BLACK, "Client %d: Hello message sent to %d chars", Client->GetIndex(), nbSent);

    // Send worldactors to client
    WorldActors->InitWorld( nClient );

    // Send NPC information to client
    NPCManager->InitPlayer( nClient );

    //Console->Print("OP Data inc");
    Outposts->SendOPAreaData( nClient );

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
  }
  else
    return false;
}
