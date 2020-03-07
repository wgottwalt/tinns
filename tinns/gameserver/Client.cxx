#include "gameserver/Includes.hxx"
#include "gameserver/decoder/Includes.hxx"
#include "common/Includes.hxx"

PClient::PClient( int Index )
{
  mAccountID = 0;
  mAccountLevel = 0;
  mIndex = Index;
  mCharID = 0;
  mConnection = PCC_NONE;
  mRemotePort = 0;
  m_TCPConnection = nullptr;
  m_UDPConnection = nullptr;

  for ( int i = 0; i < DEBUG_MODES ; i++ )
    mDebugMode[i] = false;

  //********
  mAwaitingWarpto = false;
  mTargetX = 0;
  mTargetY = 0;
  mTargetZ = 0;
  //********
  mActorRemoveMode = false;
  mAcceptNPCUpdates = false;

  testval8 = 0;
}

PClient::~PClient()
{
  if ( m_TCPConnection )
  {
    delete m_TCPConnection;
    m_TCPConnection = nullptr;
  }
  if ( m_UDPConnection )
  {
    delete m_UDPConnection;
    m_UDPConnection = nullptr;
  }
}
/// ******************************************************

bool PClient::GetDebugMode(PDebugMode nDebugID)
{
    return mDebugMode[nDebugID];
}

bool PClient::IsAcceptingNPCUpdates()
{
    return mAcceptNPCUpdates;
}

void PClient::SetAcceptNPCUpdates(bool nVal)
{
    mAcceptNPCUpdates = nVal;
}

bool PClient::IsZoning()
{
    return mZoning;
}

void PClient::SetZoning(bool nVal)
{
    mZoning = nVal;
    if (!nVal)
        mVhcZoning = false;
}

bool PClient::IsVhcZoning()
{
    return mVhcZoning;
}

void PClient::SetVhcZoning(bool nVal)
{
    mVhcZoning = nVal;
}

uint32_t PClient::GetIndex() const
{
    return mIndex;
}

uint32_t PClient::GetID() const
{
    return mIndex;
}

uint32_t PClient::GetLocalID() const
{
    return mIndex + 1;
}

uint32_t PClient::GetCharID() const
{
    return mCharID;
}

int PClient::GetRemoteUDPPort() const
{
    return mRemotePort;
}

bool PClient::IsInRemoveActorMode()
{
    return mActorRemoveMode;
}

void PClient::SetRemoveActorMode(bool nNewValue)
{
    mActorRemoveMode = nNewValue;
}

void PClient::SetRemoteUDPPort(int port)
{
    mRemotePort = port;
}

void PClient::SetCharID(int id)
{
    mCharID = id;
}

uint16_t PClient::GetUDP_ID()
{
  if ( m_UDPConnection )
    return m_UDPConnection->GetUDP_ID();
  else
    Console->Print( "%s Unable to get UDP_ID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
  return 0;
}

void PClient::SetUDP_ID( int id )
{
  if ( m_UDPConnection )
    m_UDPConnection->SetUDP_ID( id );
  else
    Console->Print( "%s Unable to set UDP_ID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
}

void PClient::IncreaseUDP_ID()
{
  if ( m_UDPConnection )
    m_UDPConnection->IncreaseUDP_ID();
  else
    Console->Print( "%s Unable to increase UDP_ID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
}

uint16_t PClient::GetSessionID()
{
  if ( m_UDPConnection )
    return m_UDPConnection->GetSessionID();
  else
    Console->Print( "%s Unable to get UDP SessionID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
  return 0;
}

uint16_t PClient::GetTransactionID()
{
  if ( m_UDPConnection )
    return m_UDPConnection->GetTransactionID();
  else
    Console->Print( "%s Unable to get UDP TransactionID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
  return 0;
}

void PClient::ResetTransactionID()
{
  if ( m_UDPConnection )
    m_UDPConnection->ResetTransactionID();
  else
    Console->Print( "%s Unable to reset UDP TransactionID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
}

void PClient::IncreaseTransactionID( uint8_t nInc )
{
  if ( m_UDPConnection )
    m_UDPConnection->IncreaseTransactionID( nInc );
  else
    Console->Print( "%s Unable to increase UDP TransactionID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
}

void PClient::FillInUDP_ID( PMessage* nMessage )
{
  uint16_t CurrPos;

  if (( nMessage->GetSize() > 9 ) && ( nMessage->U8Data( 0x00 ) == 0x13 ) )
  {
    CurrPos = 5;
    while ( CurrPos < nMessage->GetSize() ) // do UDP_ID mgt for each 0x03 subtype subpacket
    {
      if ( nMessage->U8Data( CurrPos + 1 ) == 0x03 )
      {
        IncreaseUDP_ID();
        nMessage->U16Data( CurrPos + 2 ) = GetUDP_ID();
      }
      CurrPos = CurrPos + nMessage->U8Data( CurrPos ) + 1;
    }
  }
  nMessage->U16Data( 0x01 ) = GetUDP_ID();
  nMessage->U16Data( 0x03 ) = GetSessionID();
}

void PClient::setTCPConnection(ConnectionTCP *conn)
{
    m_TCPConnection = conn;
    m_UDPConnection = nullptr;
    mConnection = PCC_GAME;
}

void PClient::setUDPConnection(ConnectionUDP *conn)
{
    m_UDPConnection = conn;
}

ConnectionTCP *PClient::getTCPConn()
{
    return m_TCPConnection;
}

ConnectionUDP *PClient::getUDPConn()
{
    return m_UDPConnection;
}

void PClient::SendTCPMessage(PMessage *nMessage)
{
    if (m_TCPConnection)
        m_TCPConnection->SendMessage(nMessage);
    else
        delete nMessage;
}

void PClient::FragmentAndSendUDPMessage( PMessage* nMessage, uint8_t nType )
{
  PMessage* ChunkBuffer;
  PMessage* ChunkMsg;
  const uint16_t ChunkSize = 220;
  uint16_t StartIncUDPIDOnChunk = 0;
  uint16_t IncludedHeaderSize = 0;
  bool ReplaceFirstByte = false;
  uint8_t ReplaceFirstByteValue = 0;
  uint16_t MultiTriggeringSize = 0;

  switch ( nType )
  {
    case 0x68: // Terminal ReceiveDB
    {
        ReplaceFirstByte = true;
        ReplaceFirstByteValue = 0x21;
        MultiTriggeringSize = 220;
        IncludedHeaderSize = 9;
        StartIncUDPIDOnChunk = 1;
        break;
    }
    case 0x04:
    {
      Console->Print( RED, BLACK, "[Error] PClient::FragmentAndSendUDPMessage: Message type 0x%02x not managed yet", nType );
      break;
    }
    case 0x05: //CharOpenContainerMsg with header & UDP_ID incremented
    {
      StartIncUDPIDOnChunk = 1;
      IncludedHeaderSize = 9;
      ReplaceFirstByte = true;
      ReplaceFirstByteValue = 0x15;
      MultiTriggeringSize = 230;
      break;
    }
    case 0x06: // For sending Custom LUA Scripts to client
    {
        break;
    }
    case 0x19: //BaselineMsg (with no header)
    {
      break;
    }
    case 0xac: //BuildTraderItemListMsg with header & UDP_ID incremented
    {
      StartIncUDPIDOnChunk = 1;
      IncludedHeaderSize = 9;
      ReplaceFirstByte = true;
      ReplaceFirstByteValue = 0x15;
      MultiTriggeringSize = 230;
      break;
    }
    default:
    {
      Console->Print( RED, BLACK, "[Error] PClient::FragmentAndSendUDPMessage: Message type 0x%02x not managed", nType );
      break;
    }
  }

  if ( nMessage->GetSize() >= MultiTriggeringSize )
  {
    if ( gDevDebug )
      Console->Print( YELLOW, BLACK, "[Debug] Fragmenting message type 0x%02x", nType );
    if ( ReplaceFirstByte )
    {
      nMessage->U8Data( IncludedHeaderSize ) = ReplaceFirstByteValue;
    }

    uint16_t ChunksNum = ( nMessage->GetSize() - IncludedHeaderSize + ChunkSize - 1 ) / ChunkSize;

    for ( uint16_t ChunkID = 0; ChunkID < ChunksNum; ChunkID++ )
    {
      ChunkBuffer = nMessage->GetChunk( IncludedHeaderSize, ChunkSize, ChunkID );
      if (ChunkBuffer == nullptr)
      {
        Console->Print( RED, BLACK, "[Error] PClient::FragmentAndSendUDPMessage: Bad chunk number: %d for size %d", ChunksNum, nMessage->GetSize() );
        break;
      }

      ChunkMsg = new PMessage( ChunkSize + 15 );
      if ( ChunkID >= StartIncUDPIDOnChunk )
      {
        IncreaseUDP_ID();
      }

      *ChunkMsg << ( uint8_t )0x13;
      *ChunkMsg << ( uint16_t )GetUDP_ID();
      *ChunkMsg << ( uint16_t )GetSessionID();
      *ChunkMsg << ( uint8_t )( 9 + ChunkBuffer->GetSize() );
      *ChunkMsg << ( uint8_t )0x03;
      *ChunkMsg << ( uint16_t )GetUDP_ID();
      *ChunkMsg << ( uint8_t )0x07; // Fragmented
      *ChunkMsg << ( uint16_t )ChunkID;
      *ChunkMsg << ( uint16_t )ChunksNum;
      *ChunkMsg << ( uint8_t )nType;
      *ChunkMsg << *ChunkBuffer;

      delete ChunkBuffer;
      //Console->Print(YELLOW, BLACK, "[Debug] Sending Fragment %d/%d", ChunkID+1, ChunksNum);
      //ChunkMsg->Dump();
      SendUDPMessage( ChunkMsg );
    }
    delete nMessage;
  }
  else
  {
    //Console->Print(YELLOW, BLACK, "[Debug] Sending message WITHOUT Fragmenting");
    SendUDPMessage( nMessage );
  }
}

void PClient::SendUDPMessage(PMessage *nMessage, bool nVIP)
{
    if (m_UDPConnection)
        m_UDPConnection->SendMessage(nMessage, nVIP);
    else
        delete nMessage;
}

void PClient::SetDebugMode( PDebugMode nDebugID, bool nVal )
{
  if ( nDebugID == DBG_ALL )
  {
    for ( int i = 0; i < DEBUG_MODES ; i++ )
      mDebugMode[i] = nVal;
  }
  else
    mDebugMode[nDebugID] = nVal;
}

bool PClient::ChangeCharLocation( uint32_t nLocation, bool DoForce )
{
  if ( gDevDebug )
    Console->Print( "%s PClient::ChangeCharLocation", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
  if ( Worlds->IsValidWorld( nLocation ) )
  {
    mAcceptNPCUpdates = false; // Zone changed, reject NPC updates till initial NPC spawn
    PChar* tChar = GetChar();
    uint32_t CurrentLocation = tChar->GetLocation();
    if (( CurrentLocation == nLocation ) && !DoForce )
      return true;

    // DoForce is used in GM teleport and jail/unjail command to free player from jail
    if (( tChar->IsJailed() == true ) && !DoForce )
    {
      if ( CurrentLocation != 550 && CurrentLocation != 551 ) // If player managed to "get out of jail" somehow, replace him there
      {
        // Do nothing here. This doesnt work as long as we are able to warp the player around just by
        // sending some packets out. Enable/modify this if fixed
        //tChar->SetLocation(550);
      }
      // Player still in 550 or 551 and trying to warp out (by death for example) just
      // return true and let him respawn at 550 entrypoint 0
      //return true;
    }

    PWorld* nWorld;
    if (( nWorld = Worlds->LeaseWorld( nLocation ) ) )
    {
      if ( tChar->GetLocationLeased() )
      {
        // TAke care of sitting chars
        uint32_t ChairObjectId;
        uint8_t tSeatId;
        PSeatType tSeatType;
        if (( tSeatType = tChar->GetSeatInUse( &ChairObjectId, &tSeatId ) ) )
        {
          bool vhcZoning = false;
          PSpawnedVehicle* tVhc = 0;
          if ( (tSeatType == seat_vhc) && IsVhcZoning() ) // If seat is vhc,
          { // Do  additionnal check
            if (( tVhc = nWorld->GetSpawnedVehicles()->GetVehicle( ChairObjectId ) ) )
            {
              if ( tVhc->GetSeatUser( tSeatId ) == tChar->GetID() )
              {
                vhcZoning = true;
              }
            }
          }

          if ( ! vhcZoning )
          {
            PUdpCharExitChair::DoLeaveChair( tChar, this, tVhc, true );
          }
          /*{
            Worlds->GetWorld( CurrentLocation )->CharLeaveChair( GetLocalID(), ChairObjectId );
            tChar->SetSeatInUse( seat_none );
          }*/
        }
        this->InitWarpCircle();
        this->InitCharVanish();
        Worlds->ReleaseWorld( CurrentLocation );
      }
      tChar->SetLocation( nLocation );
      tChar->SetLocationLeased();

      return true;
    }
  }
  else if ( nLocation != 1 ) // try to fall back if bad location
  {
    Console->Print( YELLOW, BLACK, "[Warning] Client %d want to zone to invalid world %d. Falling back to world 1", mIndex, nLocation );
    return ChangeCharLocation( 1, DoForce );
  }

  return false;
}

int PClient::GetConnection() const
{
    return mConnection;
}

const char *PClient::GetAddress() const
{
    return m_TCPConnection->getRemoteAddress();
}

uint32_t PClient::GetAccountID() const
{
    return mAccountID;
}

int PClient::GetAccountLevel() const
{
    return mAccountLevel;
}

void PClient::GameDisconnect()
{
  mAccountID = 0;

  if ( m_TCPConnection )
  {
    delete m_TCPConnection;
    m_TCPConnection = nullptr;
  }

  if ( m_UDPConnection )
  {
    delete m_UDPConnection;
    m_UDPConnection = nullptr;
  }

  /**** Will be better to put that char-saving-at-disconnect in Char destructor, when only used Chars will be loaded ****/
  PChar *tChar = GetChar();
  if ( tChar )
  {
    SetZoning();
    if ( tChar->GetLocationLeased() )
    {
      if ( tChar->GetSeatInUse() )
      {
        //if(gDevDebug)
          Console->Print( "%s Trying to get leaving char out of her seat", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
        PUdpCharExitChair::DoLeaveChair(tChar, this, nullptr, true);
      }
      /* // replaced by the lines above
      PSeatType cSeatType;
      uint32_t cSeatObjectId;
      uint8_t cSeatId;
      PWorld* cWorld;

      cWorld = Worlds->GetWorld(tChar->GetLocation());
      cSeatType = tChar->GetSeatInUse(&cSeatObjectId, &cSeatId);
      if(cSeatType)
      {
        if(cSeatType == seat_chair)
        {
          cWorld->CharLeaveChair(GetLocalID(), cSeatObjectId);
          tChar->SetSeatInUse(seat_none);
        }
        else if(cSeatType == seat_subway)
        {
          Subway->UnsetSeatUser(cSeatObjectId, cSeatId, GetLocalID());
          tChar->SetSeatInUse(seat_none);
        }
        else if(cSeatType == seat_vhc)
        {
          cWorld->GetSpawnedVehicles()->GetVehicle(cSeatObjectId)->UnsetSeatUser(cSeatId, GetLocalID());
          tChar->SetSeatInUse(seat_none);
        }
        else
        {
          Console->Print(RED, BLACK, "[Notice] PClient::GameDisconnect : Leaving seat of unkown type %d", cSeatType);
        }
      }
      */
    }

    if ( tChar->IsDirty() )
    {
      bool res = tChar->SQLSave();
      if ( res )
        Console->Print( "%s GameDisconnect: Char %i (Client %i) saved before disconnect.", Console->ColorText( GREEN, BLACK, "[DEBUG]" ),  tChar->GetID(), mIndex );
      else
        Console->Print( RED, BLACK, "[DEBUG] GameDisconnect: Char %i (Client %i) saving before disconnect and FAILED.", tChar->GetID(), mIndex );
    }
    else
    {
      Console->Print( "%s GameDisconnect: Char %i (Client %i) no save needed.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), tChar->GetID(), mIndex );
      if ( !tChar->IsOnline() )
        Console->Print( "%s GameDisconnect: Char %i (Client %i) wasn't marked as ingame anyway...", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), tChar->GetID(), mIndex );
    }

    if ( tChar->GetLocationLeased() )
    {
      if ( gDevDebug )
        Console->Print( "%s Sending char leaving effect", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      InitWarpCircle();
      InitCharVanish();

      Worlds->ReleaseWorld( tChar->GetLocation() );
      tChar->SetLocationLeased( false );
    }

    Chars->RemoveChar( mCharID );
    delete tChar;
    mCharID = 0;
  }
  else
  {
    //Console->Print(YELLOW, BLACK, "GameDisconnect: Client %i had no char online.", mIndex);
  }

  mConnection = PCC_NONE;
}

void PClient::RefreshAccountInfo( PAccount *Account )
{
  mAccountID = Account->GetID();
  mAccountLevel = Account->GetLevel();
}

void PClient::LoggedIn(PAccount *Account)
{
    RefreshAccountInfo(Account);
}

void PClient::Update()
{
  if ( m_TCPConnection )
  {
    if ( m_TCPConnection->timeOut() )
    {
      Console->Print( "%s GameSocket: Client %i: timeout", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), mIndex );
      GameServer->ClientDisconnected( this );
    }
    else
    {
      if ( !m_TCPConnection->update() )
      {
        GameServer->ClientDisconnected( this );
      }
    }
  }

  if ( m_UDPConnection )
  {
    if ( m_UDPConnection->timeOut() )
    {
      Console->Print( "%s Game UDP: Client %i: timeout", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), mIndex );
      GameServer->UDPStreamClosed( this );
    }
    else
    {
      if ( !m_UDPConnection->update() )
      {
        GameServer->UDPStreamClosed( this );
      }
    }
  }
}

PChar* PClient::GetChar() const
{
  if ( mCharID )
  {
    return Chars->GetChar( mCharID );
  }
  else
  {
    return nullptr;
  }
}

int PClient::getZoneID() const
{
    return m_ZoneID;
}

void PClient::SetAwaitingWarpto(bool yesno, uint16_t NewX, uint16_t NewY, uint16_t NewZ)
{
    mAwaitingWarpto = yesno;
    mTargetX = NewX;
    mTargetY = NewY;
    mTargetZ = NewZ;
}

bool PClient::GetCharAwaitingWarpto( uint16_t* PosX, uint16_t* PosY, uint16_t* PosZ )
{
  if ( PosX )
    *PosX = mTargetX;
  if ( PosY )
    *PosY = mTargetY;
  if ( PosZ )
    *PosZ = mTargetZ;

  return mAwaitingWarpto;
}

void PClient::InitWarpCircle()
{
  PMessage* tmpMsg_circle = MsgBuilder->BuildCharShowGlowCircleMsg( this );
  ClientManager->UDPBroadcast( tmpMsg_circle, this, 1000, true ); // send only in a 1000 radius
}

void PClient::InitCharVanish()
{
  PMessage* tmpMsg_vanish = MsgBuilder->BuildRemoveWorldObjectMsg( GetLocalID() );
  ClientManager->UDPBroadcast( tmpMsg_vanish, this, 0, true );
}
