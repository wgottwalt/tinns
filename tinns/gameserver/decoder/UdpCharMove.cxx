#include <string>
#include "gameserver/decoder/Includes.hxx"
#include "gameserver/Includes.hxx"
#include "common/Includes.hxx"

#define JUMPHEIGHT 160

/**** PUdpCharPosUpdate ****/

PUdpCharPosUpdate::PUdpCharPosUpdate( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x20";
}

PUdpMsgAnalyser* PUdpCharPosUpdate::Analyse()
{
  mDecodeData->mName << "=Char position update";

//mDecodeData->mTraceUnknownMsg = true; // temp stop being bugged with unknown move msg
//mDecodeData->mTraceKnownMsg = true;

  PMessage* nMsg = mDecodeData->mMessage;
  nMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 4 );

  *nMsg >> mInfoBitfield;
//bitfield [C?AL UXZY] packet read from LSB
//          8.21 8421
//Console->Print(YELLOW, BLACK, "[DEBUG] PUdpCharPosUpdate: Bitfield value %02x", mInfoBitfield);
  if ( mInfoBitfield & 0x80 ) // Sitting on chair
  {
    if ( mInfoBitfield & 0x7f )
    {
      mDecodeData->mName << " + mixed bitfield (" << mInfoBitfield << ")";
      mDecodeData->mState = DECODE_UNKNOWN | DECODE_FINISHED;
      Console->Print( YELLOW, BLACK, "[DEBUG] PUdpCharPosUpdate: Client %d sent Mixed field value %x", mDecodeData->mClient->GetID(), mInfoBitfield );
    }
    else
    {
      mDecodeData->mName << " (Char sitting)";

      *nMsg >> mChairItemID;
      *nMsg >> mChairItemSeat;
      mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
//mDecodeData->mTraceKnownMsg = true;
//mDecodeData->mTraceDump = true;
      if ( gDevDebug ) Console->Print( "%s Localid %d sitting on object %d (0x%08x) seat %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mDecodeData->mClient->GetLocalID(), mChairItemID, mChairItemID, mChairItemSeat );
    }
  }
  else
  {
    if ( mInfoBitfield & 0x01 )
    {
      *nMsg >> mNewY;
    }
    if ( mInfoBitfield & 0x02 )
    {
      *nMsg >> mNewZ;
    }
    if ( mInfoBitfield & 0x04 )
    {
      *nMsg >> mNewX;
    }
    if ( mInfoBitfield & 0x08 )
    {
      *nMsg >> mNewUD;
    }
    if ( mInfoBitfield & 0x10 )
    {
      *nMsg >> mNewLR;
    }
    if ( mInfoBitfield & 0x20 )
    {
      *nMsg >> mNewAct;
    }
    if ( mInfoBitfield & 0x40 ) // purpose unknown
    {
      *nMsg >> mNewUnknown;
      if ( gDevDebug && mNewUnknown )
      {
        Console->Print( YELLOW, BLACK, "[DEBUG] PUdpCharPosUpdate: Client %d sent field 0x40 (in %x) value %x", mDecodeData->mClient->GetID(), mInfoBitfield, mNewUnknown );
      }
    }

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  }

  return this;
}

bool PUdpCharPosUpdate::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();
  PMessage* tmpMsg;

  if ( ! (mInfoBitfield & 0x80) )
  {
    uint32_t nSeatableObjectId;
    uint8_t nSeatId;
    if( nChar->GetSeatInUse(&nSeatableObjectId, &nSeatId) == seat_vhc )
    {
      tmpMsg = MsgBuilder->BuildCharUseSeatMsg( nClient, nSeatableObjectId, nSeatId );
      nClient->FillInUDP_ID(tmpMsg);
      nClient->SendUDPMessage( tmpMsg );
      mChairItemID = nSeatableObjectId;
      mChairItemSeat = nSeatId;
      mInfoBitfield = 0x80;
    }
  }

  if ( mInfoBitfield & 0x80 ) // Sitting on chair
  {
    //tmpMsg = MsgBuilder->BuildCharSittingMsg(nClient, mChairItemID);
    //tmpMsg = MsgBuilder->BuildCharSittingMsg(nClient);
    //ClientManager->UDPBroadcast(tmpMsg, nClient, 5000); // TODO: Get the range from config
    tmpMsg = MsgBuilder->BuildCharPosUpdateMsg( nClient );
    ClientManager->UDPBroadcast( tmpMsg, nClient, 5000 ); // TODO: Get the range from config
  }
  else
  {
    bool IsRealMove = false;

    if (( mInfoBitfield & 0x01 ) && ( nChar->Coords.mY != mNewY ) )
    {
      nChar->Coords.mY = mNewY;
      IsRealMove = true;
    }
    if (( mInfoBitfield & 0x02 ) && ( nChar->Coords.mZ != mNewZ ) )
    {
      nChar->Coords.mZ = mNewZ;
      IsRealMove = true;
    }
    if (( mInfoBitfield & 0x04 ) && ( nChar->Coords.mX != mNewX ) )
    {
      nChar->Coords.mX = mNewX;
      IsRealMove = true;
    }
    if ( mInfoBitfield & 0x08 )
    {
      nChar->Coords.mUD = mNewUD;
      //IsRealMove = true;
    }
    if (( mInfoBitfield & 0x10 ) && ( nChar->Coords.mLR != mNewLR ) )
    {
      nChar->Coords.mLR = mNewLR;
      IsRealMove = true;
    }
    if ( mInfoBitfield & 0x20 )
    {
      nChar->Coords.mAct = mNewAct;
    }
    if ( mInfoBitfield & 0x20 )
    {
      nChar->Coords.mUnknown = mNewUnknown;
    }
    // movement action byte mask:
    // 0x00 NC has no focus (player alt+tab'ed out)
    // 0x02 kneeing
    // 0x20 Walk mode (= not run mode)
    // 0x08 left step
    // 0x10 right step
    // 0x40 forward
    // 0x80 backward
    // bits:  00000000
    //        BFWRL.K.

    if ( IsRealMove )
    {
      nChar->SetDirtyFlag();
    }
    /*if(IsRealMove)
    if(mInfoBitfield == 0x7f)
    {
      tmpMsg = MsgBuilder->BuildCharPosUpdateMsg(nClient);
      ClientManager->UDPBroadcast(tmpMsg, nClient, 5000); // TODO: Get the range from config
    }*/

    uint16_t nSaveZ = nChar->Coords.mZ;
    if(nChar->Coords.mJumpingState)
    {
      mInfoBitfield |= 0x02; // Update Z and Act only
      if(nChar->Coords.mJumpingState == 1)
      {
        if ( gDevDebug ) Console->Print( "%s Send moving jump", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
        (nChar->Coords.mZ) += JUMPHEIGHT;
        nChar->Coords.mJumpingState = 2; // Jump done
      }
      else
        nChar->Coords.mJumpingState = 0; // No pending jump
    }

    tmpMsg = MsgBuilder->BuildCharPosUpdate2Msg( nClient, mInfoBitfield );

    if(nChar->Coords.mJumpingState)
      nChar->Coords.mZ = nSaveZ;

    ClientManager->UDPBroadcast( tmpMsg, nClient, 5000, true );

    if ( IsRealMove && nClient->GetDebugMode( DBG_LOCATION ) )
    {
      char DbgMessage[128];
      float f[3];
//      uint32_t h[3];
      f[0] = nChar->Coords.mY - 32000;
      f[1] = nChar->Coords.mZ - 32000;
      f[2] = nChar->Coords.mX - 32000;
      snprintf( DbgMessage, 128, "position y:%0.1f z:%0.1f x:%0.1f lr:%d (Act=%x BF=%x)", f[0], f[1], f[2], nChar->Coords.mLR, nChar->Coords.mAct , mInfoBitfield );
      Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
    }
    /*{
      uint16_t p[3];
      p[0] = nChar->Coords.mY;
      p[1] = nChar->Coords.mZ;
      p[2] = nChar->Coords.mX;
      for(int i = 0; i<3; i++)
      {
        if(p[i]< nChar->Coords.minPos[i])
          nChar->Coords.minPos[i] = p[i];
        if(p[i]> nChar->Coords.maxPos[i])
          nChar->Coords.maxPos[i] = p[i];
      }
    }*/

  if(gDevDebug && IsRealMove)
    Console->Print("%s Char %d position : X(%d) Y(%d) Z(%d) U/D(%d) L/R(%d) Action(%02x)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mDecodeData->mClient->GetID(), nChar->Coords.mX, nChar->Coords.mY, nChar->Coords.mZ, nChar->Coords.mUD, nChar->Coords.mLR, nChar->Coords.mAct);
  }
  if ( mInfoBitfield >= 0x7f )
  {
    tmpMsg = MsgBuilder->BuildCharHealthUpdateMsg( nClient );
    ClientManager->UDPBroadcast( tmpMsg, nClient );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}


/**** PUdpCharExitChair ****/

PUdpCharExitChair::PUdpCharExitChair( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x22";
}

PUdpMsgAnalyser* PUdpCharExitChair::Analyse()
{
  mDecodeData->mName << "=Char exiting chair";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharExitChair::DoAction()
{
  DoLeaveChair( mDecodeData->mClient->GetChar(), mDecodeData->mClient );

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

bool PUdpCharExitChair::DoLeaveChair( PChar* nChar, PClient* nClient, PSpawnedVehicle* nVhc, bool nForce )
{
  if ( ! nClient )
    nClient = ClientManager->getClientByChar( nChar->GetID() );
  if ( ! nClient )
  {
    Console->Print("%s PUdpCharExitChair::DoLeaveChair called without PClient* and client not available when searching by char.", Console->ColorText( RED, BLACK, "[ERROR]" ) );
    return false;
  }

  PMessage* tmpMsg;
  uint32_t cSeatObjectId;
  uint8_t cSeatId;
  PSeatType cSeatType = nChar->GetSeatInUse( &cSeatObjectId, &cSeatId );
  uint32_t tNowTime = GameServer->GetGameTime();
  bool ReadyToExit = false;

  // This is only for investigation on subway timing
  if ( nClient->GetDebugMode( DBG_SUBWAY ) && ( cSeatType == seat_subway ) )
  {
    char DbgMessage[80];
    uint8_t tCabId;
    Subway->GetInfoIndex( cSeatObjectId, &tCabId );
    uint8_t tStationId = Subway->GetStation( cSeatObjectId, tNowTime );
    std::string* StationName = Subway->GetStationName( tStationId );
    std::string OpenState = ( Subway->IsDoorOpen( cSeatObjectId, tNowTime ) ? "open" : "closed" );

    snprintf( DbgMessage, 80, "Cab %d (%d) is in station %s (%d). Door is %s (time: %d)", tCabId, cSeatObjectId, StationName->c_str(), tStationId, OpenState.c_str(), tNowTime );
    DbgMessage[79] = 0;
    Chat->send( nClient, CHAT_DIRECT, "System", DbgMessage );
    delete StationName;
  }
  // end investigation

  if ( cSeatType )
  {
    PWorld* tWorld = Worlds->GetWorld( nChar->GetLocation() );

    if ( cSeatType == seat_chair )
    {
      tWorld->CharLeaveChair( nClient->GetLocalID(), cSeatObjectId );
      nChar->SetSeatInUse( seat_none );
      ReadyToExit = true;
    }
    else if ( cSeatType == seat_subway )
    {
      if ( Subway->IsDoorOpen( cSeatObjectId, tNowTime ) || nForce )
      {
        Subway->GetStationExitPosition( &( nChar->Coords ), Subway->GetStation( cSeatObjectId, tNowTime ), Misc::GetRandomFloat() );
        Subway->UnsetSeatUser( cSeatObjectId, cSeatId, nChar->GetID() );
        nChar->SetSeatInUse( seat_none );
        ReadyToExit = true;
      }
      else
      {
        tmpMsg = MsgBuilder->BuildText100Msg( nClient, 6, cSeatObjectId ); // "Damn, locked"
        nClient->SendUDPMessage( tmpMsg );
      }
    }
    else if ( cSeatType == seat_vhc )
    {
      PSpawnedVehicle* tVhc = tWorld->GetSpawnedVehicles()->GetVehicle( cSeatObjectId );
      if ( tVhc && (!nVhc || (tVhc == nVhc) || nForce) )
      {
        PVhcCoordinates tCoords = tVhc->GetPosition();
        tVhc->UnsetSeatUser( cSeatId, nChar->GetID() );
        nChar->SetSeatInUse( seat_none );
        nChar->Coords.SetPosition( tCoords.GetY(), tCoords.GetZ(), tCoords.GetX() ); // to complete with LR
        ReadyToExit = true;
      }
    }
    else
    {
      Console->Print( "%s PUdpCharExitChair::DoLeaveChair : Invalid seat type %d", Console->ColorText( RED, BLACK, "[ERROR]" ), cSeatType );
    }

    if ( ReadyToExit )
    {
      nChar->SetSeatInUse( seat_none );

      tmpMsg = MsgBuilder->BuildCharExitSeatMsg( nClient );
      ClientManager->UDPBroadcast( tmpMsg, nClient );

      if ( gDevDebug ) Console->Print( "%s Localchar %d get up from chair %d.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nClient->GetLocalID(), cSeatObjectId );
      return true;
    }
  }

  return false;
}

/**** PUdpCharJump ****/

PUdpCharJump::PUdpCharJump( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x02";
}

PUdpMsgAnalyser* PUdpCharJump::Analyse()
{
  mDecodeData->mName << "=Char jumping";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharJump::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();

  if( nChar->Coords.mAct & 0xd8 ) // if moving, don't jump now and wait next update
  {
    nChar->Coords.mJumpingState = 1; // Jump pending. Do at next pos update
    if ( gDevDebug ) Console->Print( "%s Prepare moving jump", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
  }
  else // If not moving, jump now
  {
    if ( gDevDebug ) Console->Print( "%s Send static jump", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    uint8_t mInfoBitfield = 0x22; // Update Z and Act only
    uint16_t nSaveZ = nChar->Coords.mZ;
    (nChar->Coords.mZ) += JUMPHEIGHT;
    PMessage* tmpMsg = MsgBuilder->BuildCharPosUpdate2Msg( nClient, mInfoBitfield );
    nChar->Coords.mZ = nSaveZ;
    nChar->Coords.mJumpingState = 2; // Jump done
    ClientManager->UDPBroadcast( tmpMsg, nClient, 5000, true );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;

  return true;
}


/**** PUdpCharTargeting ****/

PUdpCharTargeting::PUdpCharTargeting( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x2d";
}

PUdpMsgAnalyser* PUdpCharTargeting::Analyse()
{
  mDecodeData->mName << "=Targeting char";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpCharTargeting::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  uint16_t CharID = mDecodeData->mMessage->U16Data( mDecodeData->Sub0x13Start + 2 );
  uint8_t strangeval1 = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 4 );
  uint8_t strangeval2 = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 5 );
  uint8_t strangeval3 = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 6 );

  tChar->SetLookingAt( CharID );

  if ( gDevDebug )
    Console->Print( "%s Char %d targeting char %d. // %d %d %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mDecodeData->mClient->GetID(), CharID, strangeval1, strangeval2, strangeval3 );
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
